#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

static GPIO_InitTypeDef  	GPIO_InitStructure;
static USART_InitTypeDef 	USART_InitStructure;
static NVIC_InitTypeDef 	NVIC_InitStructure;		
static EXTI_InitTypeDef  	EXTI_InitStructure;
static RTC_TimeTypeDef  	RTC_TimeStructure;
static RTC_InitTypeDef  	RTC_InitStructure;
static RTC_DateTypeDef 		RTC_DateStructure;

static volatile uint32_t 	g_rtc_wakeup_event=0;
static volatile uint32_t 	g_rtc_alarm_event=0;

static volatile uint8_t  	g_usart1_recv_buf[128]={0};
static volatile uint32_t 	g_usart1_recv_cnt = 0;
static volatile uint32_t 	g_usart1_event=0;


//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  
	
	return ch;
}   

void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD =SystemCoreClock/8/1000000*nus; 	//时间加载	  		 
	SysTick->VAL  =0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; 		//使能滴答定时器开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));			//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; 		//关闭计数器
	SysTick->VAL =0X00;       						//清空计数器 
}

void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=SystemCoreClock/8/1000*nms;		//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;    	//能滴答定时器开始倒数 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));			//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;    	//关闭计数器
	SysTick->VAL =0X00;     		  				//清空计数器	  	    
} 

void LED_Init(void)
{    	 
  
	//使能GPIOE，GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);			

	//GPIOF9,F10初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//普通输出模式，
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);							//初始化GPIOF，把配置的数据写入寄存器						


	//GPIOE13,PE14初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;		//LED2和LED3对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;					//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);							//初始化GPIOE，把配置的数据写入寄存器

	GPIO_SetBits(GPIOF,GPIO_Pin_9  | GPIO_Pin_10);			    	//GPIOF9,PF10设置高，灯灭
	GPIO_SetBits(GPIOE,GPIO_Pin_13 | GPIO_Pin_14);		
}

void BEEP_Init(void)
{
	//GPIOF时钟
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE);			

	//GPIOF8初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 ;					//蜂鸣器对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//普通输出模式，
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);							//初始化GPIOF，把配置的数据写入寄存器						

	//蜂鸣器禁鸣
	PFout(8)=0;
}


void USART1_Init(uint32_t baud)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 							//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);							//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 						//GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 						//GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 						//GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;								//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											//初始化PA9，PA10

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baud;										//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART1, &USART_InitStructure); 										//初始化串口1
	
	USART_Cmd(USART1, ENABLE);  													//使能串口1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;								//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;							//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;								//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器
}

void rtc_alarm_init(void)
{
	/* Enable RTC Alarm A Interrupt，允许RTC的A闹钟触发中断 */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
	/* 清空标志位 */
	RTC_ClearFlag(RTC_FLAG_ALRAF);

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

void rtc_alarm_set(RTC_AlarmTypeDef RTC_AlarmStructure)
{
	/* 关闭闹钟，若不关闭，配置闹钟触发的中断有BUG，无论怎么配置，只要到00秒，则触发中断*/
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
	/* Configure the RTC Alarm A register，配置RTC的A闹钟 */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	/* Enable the alarm ，让RTC的闹钟A工作*/
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
	
}

void rtc_date_init(void)
{

	/* Enable the PWR clock，使能电源时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC，允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);
	
	//使能外部低速晶振，提高精度
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSI is ready，等待外部低速震荡电路稳定 */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)

	/* Select the RTC Clock Source，选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable the RTC Clock,使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);
	
	//等待RTC相关寄存器就绪
	RTC_WaitForSynchro();
	
	/*  Configure the RTC data register and RTC prescaler 
	
		RTC时钟频率 = 32768Hz / 128 / 256 = 1Hz
	*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数128
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;					//同步分频系数256
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);
  
	/* Set the date: 2017/10/12 星期四 */
	RTC_DateStructure.RTC_Year = 0x17;
	RTC_DateStructure.RTC_Month = RTC_Month_October;
	RTC_DateStructure.RTC_Date = 0x12;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Thursday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	 
	 /* Set the time to 11:11:11 AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x11;
	RTC_TimeStructure.RTC_Minutes = 0x11;
	RTC_TimeStructure.RTC_Seconds = 0x11; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

	/* Indicator for the RTC configuration,写备份寄存器 */
	//RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	
	//关闭WAKE UP
	RTC_WakeUpCmd(DISABLE);		

	//唤醒时钟选择RTC配置好的时钟
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

	//设置WAKE UP自动重装载寄存器，写入零值默认是0+1=1
	RTC_SetWakeUpCounter(0);								
	
	
	//注意：必须添加清空RTC唤醒中断标志位，否则RTC不会触发
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	/* Enable RTC Wakeup Interrupt ，使能RTC唤醒中断*/
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	 
	/* Enable the Wakeup ，开启RTCWakeup定时器*/
	RTC_WakeUpCmd(ENABLE);
	

	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	 
	/* Enable the RTC Wakup Interrupt，使能RTC唤醒中断 */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	/* Indicator for the RTC configuration，写备份寄存器，用于建立判断标志是否要重置RTC的日期与时间 */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x4567);	
}


int main(void)
{ 
	uint32_t i=0;
	
	uint32_t time_cnt=0;
	uint32_t time_sum=0;
	
	RTC_TimeTypeDef  	RTC_TimeStructure;
	RTC_DateTypeDef 	RTC_DateStructure;
	RTC_AlarmTypeDef 	RTC_AlarmStructure;	
	
	char *p;

	//使能GPIOG的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//LED初始化
	LED_Init();		
	
	//蜂鸣器初始化
	BEEP_Init();
		
	//设置中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//串口1,波特率115200bps,开启接收中断
	USART1_Init(115200);
	
	//rtc初始化
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x4567)
	{  
		rtc_date_init();
	}
	else
	{
		/* Enable the PWR clock ，使能电源时钟*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
		/* Allow access to RTC，允许访问RTC */
		PWR_BackupAccessCmd(ENABLE);
		
		/* Wait for RTC APB registers synchronisation，等待所有的RTC寄存器就绪 */
		RTC_WaitForSynchro();	
		

		//关闭唤醒功能
		RTC_WakeUpCmd(DISABLE);
		
		//为唤醒功能选择RTC配置好的时钟源
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		
		//设置唤醒计数值为自动重载，写入值默认是0，1->0
		RTC_SetWakeUpCounter(0);
		
		//清除RTC唤醒中断标志
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		//使能RTC唤醒中断
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		//使能唤醒功能
		RTC_WakeUpCmd(ENABLE);			


		/* 配置外部中断控制线22，实现RTC唤醒*/
		EXTI_ClearITPendingBit(EXTI_Line22);
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* 使能RTC唤醒中断 */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	//RTC闹钟初始化
	rtc_alarm_init();

	while(1)
	{
			
			//rtc唤醒事件
			if(g_rtc_wakeup_event)
			{
				//获取时间
				RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
				printf("%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);					
				
				
				//获取日期
				RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
				printf("20%02x/%02x/%02xWeek:%x\r\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay);
						
				g_rtc_wakeup_event=0;
			}
			
			//rtc闹钟事件
			if(g_rtc_alarm_event)
			{
				//模拟闹钟滴滴的声音,并闪烁LED灯
				if(time_sum<=6)
				{
					time_cnt++;
					delay_ms(1);					
					if(time_cnt==80)
					{
						PFout(8)=1;
						PFout(10)=0;
					}
					else if(time_cnt==160)
					{
						PFout(8)=0;
						PFout(10)=1;
					}
					else if(time_cnt==240)
					{
						PFout(8)=1;
						PFout(10)=0;
					}
					else if(time_cnt==320)
					{
						PFout(8)=0;
						PFout(10)=1;
					}				
					else if(time_cnt>=1000)
					{
						time_sum+=1;
						time_cnt=0;
					}				
				}
				else
				{
					//清空标志位
					g_rtc_alarm_event=0;
					
					//清零蜂鸣器鸣响的总时间值
					time_sum=0;
					
					//清零1毫秒时间计数值
					time_cnt=0;
					
				}	
			}		
			
			//串口1事件
			if(g_usart1_event)
			{
				
				//判断接收到的字符串为DATE SET
				//示例：DATE SET-2017-10-12-4#
				if(strstr((char *)g_usart1_recv_buf,"DATE SET"))
				{
					//以等号分割字符串
					strtok((char *)g_usart1_recv_buf,"-");
					
					//获取年
					p=strtok(NULL,"-");
					
					
					//2017-2000=17 
					i = atoi(p)-2000;
					//转换为16进制 17 ->0x17
					i= (i/10)*16+i%10; 
					RTC_DateStructure.RTC_Year = i;
					
					//获取月
					//10 -> 0x10
					p=strtok(NULL,"-");
					i=atoi(p);
					//转换为16进制
					i= (i/10)*16+i%10;						
					RTC_DateStructure.RTC_Month=i;


					//获取日
					p=strtok(NULL,"-");
					i=atoi(p);
					
					
					//转换为16进制
					i= (i/10)*16+i%10;		
					RTC_DateStructure.RTC_Date = i;
					
					//获取星期
					p=strtok(NULL,"-");
					i=atoi(p);
					//转换为16进制
					i= (i/10)*16+i%10;						
					RTC_DateStructure.RTC_WeekDay = i;
					
					//设置日期
					RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
				
				}	



				//判断接收到的字符串为TIME SET
				//示例：TIME SET-14-20-10#
				if(strstr((char *)g_usart1_recv_buf,"TIME SET"))
				{
					//以等号分割字符串
					strtok((char *)g_usart1_recv_buf,"-");
					
					//获取时
					p=strtok(NULL,"-");
					i = atoi(p);
					
					//通过时，判断是AM还是PM
					if(i<12)
						RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
					else
						RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
						
					//转换为16进制
					i= (i/10)*16+i%10;
					RTC_TimeStructure.RTC_Hours   = i;
					
					//获取分
					p=strtok(NULL,"-");
					i = atoi(p);						
					//转换为16进制
					i= (i/10)*16+i%10;	
					RTC_TimeStructure.RTC_Minutes = i;
					
					//获取秒
					p=strtok(NULL,"-");
					i = atoi(p);						
					//转换为16进制
					i= (i/10)*16+i%10;					
					RTC_TimeStructure.RTC_Seconds = i; 					
					
					RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure); 
				}	

				//判断接收到的字符串为ALARM SET
				//示例：ALARM SET-14-20-10#
				if(strstr((char *)g_usart1_recv_buf,"ALARM SET"))
				{
					//以等号分割字符串
					strtok((char *)g_usart1_recv_buf,"-");
					
					//获取时
					p=strtok(NULL,"-");
					i = atoi(p);
					
					//通过时，判断是AM还是PM
					if(i<12)
						RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
					else
						RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_PM;
						
					//转换为16进制
					i= (i/10)*16+i%10;
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = i;
					
					//获取分
					p=strtok(NULL,"-");
					i = atoi(p);						
					//转换为16进制
					i= (i/10)*16+i%10;	
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = i;
					
					//获取秒
					p=strtok(NULL,"-");
					i = atoi(p);						
					//转换为16进制
					i= (i/10)*16+i%10;					
					RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = i; 	
					
					#if 1	//每天闹钟都会生效
						//RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;								//若RTC_AlarmDateWeekDaySel_Date，当前为日期，数值范围是0x1~0x31
																									//若RTC_AlarmDateWeekDaySel_WeekDay,当前为星期几，数值范围：0x1~0x7
						//RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;	//选择日期
						RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;				//屏蔽星期/日期，也就说闹钟每天都会生效
					#else	//指定闹钟在某一天生效
						RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;								//若RTC_AlarmDateWeekDaySel_Date，当前为日期，数值范围是0x1~0x31
																									//若RTC_AlarmDateWeekDaySel_WeekDay,当前为星期几，数值范围：0x1~0x7
						RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;	//选择日期
						RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;						//不屏蔽星期/日期，也就说闹钟会指定某一天会生效	
					#endif		
					
					rtc_alarm_set(RTC_AlarmStructure); 
					printf("%s ok,please wait rtc alarm...\r\n",g_usart1_recv_buf);
				}
				
				//清空串口1数据计数值
				g_usart1_recv_cnt = 0;			
			
				//清空串口1接收数据事件
				g_usart1_event = 0;
			
				//清空串口1接收数据缓冲区
				memset((char *)g_usart1_recv_buf,0,sizeof g_usart1_recv_buf);
			
			}

	}
}


void RTC_WKUP_IRQHandler(void)
{

	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		g_rtc_wakeup_event = 1;
		
		RTC_ClearITPendingBit(RTC_IT_WUT);
	} 
	
	EXTI_ClearITPendingBit(EXTI_Line22);
}


void USART1_IRQHandler(void)                				//串口1中断服务程序
{

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//接收中断
	{
		//从串口1接收数据
		g_usart1_recv_buf[g_usart1_recv_cnt]=USART_ReceiveData(USART1);	
		
		//记录多少个数据
		g_usart1_recv_cnt++;
		
		//检测到'#'符或接收的数据满的时候则发送数据
		if(g_usart1_recv_buf[g_usart1_recv_cnt-1]=='#' || g_usart1_recv_cnt>=(sizeof g_usart1_recv_buf))
		{
			g_usart1_event = 1;
		
		}
	} 
} 


void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
	{
		//添加用户代码
		g_rtc_alarm_event=1;
		
		printf("rtc alarm trig\r\n");
		
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		EXTI_ClearITPendingBit(EXTI_Line17);
	} 
}
