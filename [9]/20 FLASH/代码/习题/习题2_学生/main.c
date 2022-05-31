#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "humiture.h"

static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;
static EXTI_InitTypeDef 		EXTI_InitStructure;

static RTC_TimeTypeDef  		RTC_TimeStructure;
static RTC_InitTypeDef  		RTC_InitStructure;
static RTC_DateTypeDef 			RTC_DateStructure;

static char recv_buf[20];
static int recv_event=0;
static volatile uint32_t 		g_rtc_wakeup_event=0;
static volatile uint32_t 		secpos=0x8010000;
static volatile uint32_t 		secpos_t=0x8010000;
static int count=0;


#pragma import(__use_no_semihosting_swi)
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	return ch;
}
void _sys_exit(int return_code) {

}


void usart1_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口1工作
	USART_Cmd(USART1, ENABLE);
}

void rtc_init(void)
{

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		
	
	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);
	
	//使能LSE振荡时钟
	RCC_LSEConfig(RCC_LSE_ON);

	//等待外部晶振继续（官方代码要添加超时）
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//选择LSE作为RTC的外部振荡时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable the RTC Clock，使能RTC的硬件时钟 */
	RCC_RTCCLKCmd(ENABLE);	
	
	//等待所有寄存器就绪
	RTC_WaitForSynchro();
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)/(uwSynchPrediv + 1)=1Hz*/
	
	
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;		//异步分频
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;		//同步分频
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);
	
	/* Set the date: Wednesday April 22th 2020， */
	RTC_DateStructure.RTC_Year = 0x20;
	RTC_DateStructure.RTC_Month = RTC_Month_April;
	RTC_DateStructure.RTC_Date = 0x22;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Wednesday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 16h 12mn 00s PM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 0x16;
	RTC_TimeStructure.RTC_Minutes = 0x12;
	RTC_TimeStructure.RTC_Seconds = 0x00; 
	
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
	
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认是0
	RTC_SetWakeUpCounter(0);
	
	//清除RTC唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//使能唤醒功能
	RTC_WakeUpCmd(ENABLE);
	
	
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	

	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void rtc_alarm_init(void)
{	
	/* 允许RTC的A闹钟触发中断 */
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);
	
	/* 清空标志位 */
	RTC_ClearFlag(RTC_FLAG_ALRBF);

	/*使能外部中断控制线17的中断*/
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/*使能闹钟的中断 */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

int main(void)
{
	int i,j;
	int humi_flag = 0;
	uint8_t buf[12];
	RTC_TimeTypeDef Time;
	RTC_DateTypeDef Date;
	
	//串口1波特率:115200bps
	usart1_init(115200);	
	humi_init();
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1688)
	{
		//rtc的初始化
		rtc_init();
		
		//设置备份寄存器0
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1688);
	}
	else
	{
		/* Enable the PWR clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		
		
		/* Allow access to RTC */
		PWR_BackupAccessCmd(ENABLE);
		
		//使能LSE振荡时钟
		RCC_LSEConfig(RCC_LSE_ON);

		//等待外部晶振继续（官方代码要添加超时）
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		
		//选择LSE作为RTC的外部振荡时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		/* Enable the RTC Clock，使能RTC的硬件时钟 */
		RCC_RTCCLKCmd(ENABLE);	
		
		//等待所有寄存器就绪
		RTC_WaitForSynchro();	
		
		//清除RTC唤醒中断标志
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		//使能RTC唤醒中断
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		//使能唤醒功能
		RTC_WakeUpCmd(ENABLE);

		EXTI_ClearITPendingBit(EXTI_Line22);
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
	}

	//解除写访问
	FLASH_Unlock();	
	
	//将一切的标志归零
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	
	//擦除扇区4，擦除最小单元速度为4个字节
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3))
	{
		printf("FLASH_EraseSector sector 4 fail\r\n");
		
		while(1);
	
	}
	FLASH_Lock();
	while(1)
	{
		if(g_rtc_wakeup_event == 1)
		{
			//获取时间
			RTC_GetTime(RTC_Format_BCD,&Time);
			
			//获取日期
			RTC_GetDate(RTC_Format_BCD,&Date);
			
			
			if(humi_flag)
			{
				humi_start(buf);
				
				buf[0] = Date.RTC_Year;
				buf[1] = Date.RTC_Month;
				buf[2] = Date.RTC_Date;
				buf[3] = Date.RTC_WeekDay;
				buf[4] = Time.RTC_Hours;
				buf[5] = Time.RTC_Minutes;
				buf[6] = Time.RTC_Seconds;
				
				//解除写访问
				FLASH_Unlock();	
				if(count<100)
				{
					for(i=0; i<11; i++)
					{	
						//写入数据
						if(FLASH_COMPLETE!=FLASH_ProgramWord(secpos, buf[i]))
						{
							printf("FLASH_ProgramWord at addr %x fail\r\n",secpos);
							while(1);
						}

						secpos += 4;
					}
					count++;
				}
				else
				{
					printf("存储已满\r\n");
					humi_flag = 0;
				}
				
				//添加写保护
				FLASH_Lock();
			}
			
			g_rtc_wakeup_event=0;
		}
		if(recv_event == 1)
		{
			if(strstr(recv_buf,"DATE SET") != NULL)	
			{	
				const char s[2] = "-";				
				char *token;

				token = strtok(recv_buf,s);
				
				token = strtok(NULL,s);
				RTC_DateStructure.RTC_Year = atoi(token);
				token = strtok(NULL,s);
				RTC_DateStructure.RTC_Month = atoi(token);
				token = strtok(NULL,s);
				RTC_DateStructure.RTC_Date = atoi(token);
				token = strtok(NULL,s);
				RTC_DateStructure.RTC_WeekDay = atoi(token);
				
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);	//修改日期
			}
			else if(strstr(recv_buf,"TIME SET") != NULL)
			{
				const char s[2] = "-"; 
				char *token;

				token = strtok(recv_buf,s);
				
				token = strtok(NULL,s);
				RTC_TimeStructure.RTC_Hours   = atoi(token);
				token = strtok(NULL,s);
				RTC_TimeStructure.RTC_Minutes =  atoi(token);
				token = strtok(NULL,s);
				RTC_TimeStructure.RTC_Seconds = atoi(token);
	
				RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);	//修改时间
			}
			else if(strstr(recv_buf,"start") != NULL)
			{
				humi_flag = 1;
				printf("start ok\r\n");
			}
			else if(strstr(recv_buf,"stop") != NULL)
			{
				humi_flag = 0;
				printf("stop ok\r\n");
			}
			else if(strstr(recv_buf,"show") != NULL)
			{
				
				for(j=0 ;j<count; j++)
				{
					for(i=0; i<11; i++)
					{
						//读取扇区4的数据
						buf[i] = *(volatile uint32_t*)(secpos_t);
						
						secpos_t += 4;
					}
					
					printf("[%03d]20%02x/%02x/%02x Week %x %02x:%02x:%02x Humi=%d.%d Temp=%d.%d\r\n",
								j+1,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],
								buf[7],buf[8],buf[9],buf[10]);
				}
				
				secpos_t = 0x8010000;
			}
			else if(strstr(recv_buf,"clear") != NULL)
			{
				//解除写访问
				FLASH_Unlock();	
				
				//将一切的标志归零
				FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
				
				//擦除扇区4，擦除最小单元速度为4个字节
				if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3))
				{
					printf("FLASH_EraseSector sector 4 fail\r\n");
					
					while(1);
				
				}
				FLASH_Lock();
				
				secpos  = 0x8010000;
				count 	= 0;
				
				printf("clear ok\r\n");
			}
			
			recv_event=0;
			
			memset((void *)recv_buf,0,sizeof recv_buf);
		}
	}
}



void USART1_IRQHandler(void)
{
	static uint32_t i=0;	

	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{	
		if(USART_ReceiveData(USART1)=='#'||i>sizeof(recv_buf))
		{
			i = 0;
			recv_event = 1;	//接收完毕
		}
		else
		{
			recv_buf[i] = USART_ReceiveData(USART1);
			i++;
		}
		
		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}


void RTC_WKUP_IRQHandler(void)
{

	//检测标志位
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{		
		g_rtc_wakeup_event=1;
		
		//清空标志位
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}


}



