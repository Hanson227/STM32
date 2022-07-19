#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;
static EXTI_InitTypeDef 		EXTI_InitStructure;

static RTC_TimeTypeDef  		RTC_TimeStructure;
static RTC_InitTypeDef  		RTC_InitStructure;
static RTC_DateTypeDef 			RTC_DateStructure;

static RTC_AlarmTypeDef 		RTC_AlarmStructure;//���ӽṹ��


static volatile uint32_t 		g_rtc_wakeup_event=0;//��ʱ�жϱ�־ֵ
static volatile uint32_t 		g_rtc_alarm_a_event=0;//���ӻ��ѱ�־ֵ

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

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳ��ʱ��
	SysTick->LOAD = (168*n)-1; // ���ü���ֵ(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick���ر�ϵͳ��ʱ��
		SysTick->LOAD = (168000)-1; 	// ���ü���ֵ(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

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
	RTC_DateStructure.RTC_Year = 0x22;
	RTC_DateStructure.RTC_Month = RTC_Month_July;
	RTC_DateStructure.RTC_Date = 0x17;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Sunday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 16h 12mn 00s PM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 0x14;
	RTC_TimeStructure.RTC_Minutes = 0x05;
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

void alarm_init(void)
{
	//�ر�����A�����ܽ�������
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);


	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_PM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x16;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x12;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x10;
	
#if 0 //ÿ����Ч
	//RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	//RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
#endif

#if 0 //ָ��ĳһ����Ч����ǰ��24����Ч
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x24;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;
#endif
	
#if 1 //ָ��ĳһ������Ч����ǰ����������Ч
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Friday;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_None;
#endif	
	
	
	/* ��������A*/
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	//������A
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);


	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	/* Enable the RTC Alarm Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



}



int main(void)
{
	int32_t rt=0;
	uint8_t buf[5]={0};
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//����1������:115200bps
	usart1_init(115200);	

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;		//��8 9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);

	PFout(8)=0;	
	PFout(9)=1;
	
	printf("This is rtc test\r\n");
	
	
	//rtc�ĳ�ʼ��
	rtc_init();
	
	//alarm�ĳ�ʼ��
	alarm_init();
	
	while(1)
	{
			if(g_rtc_wakeup_event)
			{
				//��ȡʱ��
				RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
				
				//9��19��5��    9:19:5     09:19:05
				printf("%02x:%02x:%02x\r\n",RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
				
				
				//��ȡ����
				RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
				printf("20%02x/%02x/%02x Week:%x\r\n",	RTC_DateStructure.RTC_Year,
														RTC_DateStructure.RTC_Month,
														RTC_DateStructure.RTC_Date,
														RTC_DateStructure.RTC_WeekDay);
				
				//printf��ӡ����ʱ��
			
			
				g_rtc_wakeup_event=0;
			}
			
			
			if(g_rtc_alarm_a_event)
			{
				PFout(8)=1;PFout(9)=0;
				delay_ms(80);
			
				PFout(8)=0;PFout(9)=1;
				delay_ms(80);	

				PFout(8)=1;PFout(9)=0;
				delay_ms(80);
			
				PFout(8)=0;PFout(9)=1;
				delay_ms(80);	
				
				PFout(8)=1;PFout(9)=0;
				delay_ms(80);
			
				PFout(8)=0;PFout(9)=1;
				delay_ms(80);	

				PFout(8)=1;PFout(9)=0;
				delay_ms(80);
			
				PFout(8)=0;PFout(9)=1;
				delay_ms(80);				
				
				g_rtc_alarm_a_event=0;
			}

	}
}



void USART1_IRQHandler(void)
{
	uint8_t d;
	
	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//��������
		d=USART_ReceiveData(USART1);
		
		
	
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}


void RTC_WKUP_IRQHandler(void)
{

	//����־λ
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		printf("RTC_WKUP_IRQHandler\r\n");
		
		g_rtc_wakeup_event=1;
		
		//��ձ�־λ
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}


}


void RTC_Alarm_IRQHandler(void)
{

	//����־λ
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET)
	{
		printf("RTC_Alarm_IRQHandler\r\n");
		
		g_rtc_alarm_a_event=1;
		
		//��ձ�־λ
		RTC_ClearITPendingBit(RTC_IT_ALRA);
		
		EXTI_ClearITPendingBit(EXTI_Line17);
	}


}
