#include "rtc.h"


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
	RTC_DateStructure.RTC_Date = 0x27;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
	/* Set the time to 16h 12mn 00s PM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_PM;
	RTC_TimeStructure.RTC_Hours   = 0x23;
	RTC_TimeStructure.RTC_Minutes = 0x59;
	RTC_TimeStructure.RTC_Seconds = 0x50; 
	
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
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	
	/* ��ձ�־λ */
	RTC_ClearFlag(RTC_FLAG_ALRAF);

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

void rtc_alarm_set(RTC_AlarmTypeDef RTC_AlarmStructure)
{
	/* �ر����ӣ������رգ��������Ӵ������ж���BUG��������ô���ã�ֻҪ��00�룬�򴥷��ж�*/
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
	
	/* ����RTC��A���ӣ�ע��RTC���������������ֱ�Ϊ����A������B */
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	/* ��RTC������A����*/
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}



void RTC_WKUP_IRQHandler(void)
{

	//����־λ
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		//printf("RTC_WKUP_IRQHandler\r\n");
		
		g_rtc_wakeup_event=1;
		
		//��ձ�־λ
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		EXTI_ClearITPendingBit(EXTI_Line22);
	}


}


void RTC_Alarm_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_ALRA) == SET)
	{
		g_rtc_clock_event = 1;
		RTC_ClearITPendingBit(RTC_IT_ALRA);
	}
	EXTI_ClearITPendingBit(EXTI_Line17);  //����ⲿ�ж�17
	
	
}

