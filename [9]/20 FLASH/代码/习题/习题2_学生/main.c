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
	
	//��PAӲ��ʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//�򿪴���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//����PA9��PA10Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//�๦��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//��PA9��PA10�������ӵ�����1��Ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//���ô���1��ز����������ʡ���У��λ��8λ����λ��1��ֹͣλ......
	USART_InitStructure.USART_BaudRate = baud;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����շ�����
	USART_Init(USART1, &USART_InitStructure);
	
	
	//���ô���1���жϴ�������������һ���ֽڴ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//���ô���1���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//ʹ�ܴ���1����
	USART_Cmd(USART1, ENABLE);
}

void rtc_init(void)
{

	/* Enable the PWR clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		
	
	/* Allow access to RTC */
	PWR_BackupAccessCmd(ENABLE);
	
	//ʹ��LSE��ʱ��
	RCC_LSEConfig(RCC_LSE_ON);

	//�ȴ��ⲿ����������ٷ�����Ҫ��ӳ�ʱ��
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//ѡ��LSE��ΪRTC���ⲿ��ʱ��Դ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable the RTC Clock��ʹ��RTC��Ӳ��ʱ�� */
	RCC_RTCCLKCmd(ENABLE);	
	
	//�ȴ����мĴ�������
	RTC_WaitForSynchro();
	
	/* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)/(uwSynchPrediv + 1)=1Hz*/
	
	
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;		//�첽��Ƶ
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;		//ͬ����Ƶ
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;		//24Сʱ��ʽ
	RTC_Init(&RTC_InitStructure);
	
	/* Set the date: Wednesday April 22th 2020�� */
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
	
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���û��Ѽ���ֵΪ�Զ����أ�д��ֵĬ����0
	RTC_SetWakeUpCounter(0);
	
	//���RTC�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//ʹ�ܻ��ѹ���
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
	/* ����RTC��A���Ӵ����ж� */
	RTC_ITConfig(RTC_IT_ALRB, ENABLE);
	
	/* ��ձ�־λ */
	RTC_ClearFlag(RTC_FLAG_ALRBF);

	/*ʹ���ⲿ�жϿ�����17���ж�*/
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/*ʹ�����ӵ��ж� */
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
	
	//����1������:115200bps
	usart1_init(115200);	
	humi_init();
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x1688)
	{
		//rtc�ĳ�ʼ��
		rtc_init();
		
		//���ñ��ݼĴ���0
		RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1688);
	}
	else
	{
		/* Enable the PWR clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		
		
		/* Allow access to RTC */
		PWR_BackupAccessCmd(ENABLE);
		
		//ʹ��LSE��ʱ��
		RCC_LSEConfig(RCC_LSE_ON);

		//�ȴ��ⲿ����������ٷ�����Ҫ��ӳ�ʱ��
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
		
		//ѡ��LSE��ΪRTC���ⲿ��ʱ��Դ
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		
		/* Enable the RTC Clock��ʹ��RTC��Ӳ��ʱ�� */
		RCC_RTCCLKCmd(ENABLE);	
		
		//�ȴ����мĴ�������
		RTC_WaitForSynchro();	
		
		//���RTC�����жϱ�־
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		//ʹ��RTC�����ж�
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		//ʹ�ܻ��ѹ���
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

	//���д����
	FLASH_Unlock();	
	
	//��һ�еı�־����
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	
	//��������4��������С��Ԫ�ٶ�Ϊ4���ֽ�
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
			//��ȡʱ��
			RTC_GetTime(RTC_Format_BCD,&Time);
			
			//��ȡ����
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
				
				//���д����
				FLASH_Unlock();	
				if(count<100)
				{
					for(i=0; i<11; i++)
					{	
						//д������
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
					printf("�洢����\r\n");
					humi_flag = 0;
				}
				
				//���д����
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
				
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);	//�޸�����
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
	
				RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);	//�޸�ʱ��
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
						//��ȡ����4������
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
				//���д����
				FLASH_Unlock();	
				
				//��һ�еı�־����
				FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
				
				//��������4��������С��Ԫ�ٶ�Ϊ4���ֽ�
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

	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{	
		if(USART_ReceiveData(USART1)=='#'||i>sizeof(recv_buf))
		{
			i = 0;
			recv_event = 1;	//�������
		}
		else
		{
			recv_buf[i] = USART_ReceiveData(USART1);
			i++;
		}
		
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}


void RTC_WKUP_IRQHandler(void)
{

	//����־λ
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{		
		g_rtc_wakeup_event=1;
		
		//��ձ�־λ
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}


}



