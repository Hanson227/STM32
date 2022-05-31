#include "rtc.h"
#include "sys.h"
#include <stdio.h>
#include <string.h> 
#include "./user/wenshi/wenshi.h"

char flash_data[100][50] = {0};
static uint8_t flash_number = 0;
void rtc_inits(void)
{
	//������Դ����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//�����Ա�����ķ���
	PWR_BackupAccessCmd(ENABLE);
	
#if 0
	//ʹ��LSE��ʱ��
	RCC_LSEConfig(RCC_LSE_ON);
	
	//���LSE��ʱ���Ƿ����
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	//ѡ��LSE��ΪRTC��ʱ��Դ
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

#else  //��LSE�޷������������ڲ�LSI
	/* ʹ��LSI*/
	RCC_LSICmd(ENABLE);
	
	/* ����LSI�Ƿ���Ч*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* ѡ��LSI��ΪRTC��Ӳ��ʱ��Դ*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif	
	
	
	
	//ʹ��RTCӲ��ʱ�ӣ�RTC����
	RCC_RTCCLKCmd(ENABLE);
	
	//�ȴ�RTC��ؼĴ���
	RTC_WaitForSynchro();
	
	//����RTC�ķ�Ƶֵ��24Сʱ��ʽ������ʼ��
	RTC_InitStructure.RTC_AsynchPrediv = 127;
	RTC_InitStructure.RTC_SynchPrediv = 255;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	//�����������ܣ�������
	RTC_DateStructure.RTC_Year = RTC_YEARS;
	RTC_DateStructure.RTC_Month = RTC_Month_December;
	RTC_DateStructure.RTC_Date = RTC_DATES;
	RTC_DateStructure.RTC_WeekDay = RTC_WEEKDAYS;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	//����ʱ���룬������
	RTC_TimeStructure.RTC_H12 = RTC_H12S;
	RTC_TimeStructure.RTC_Hours = RTC_HOURS;
	RTC_TimeStructure.RTC_Minutes = RTC_MINUTESS;
	RTC_TimeStructure.RTC_Seconds = RTC_SECONDS;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
	
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���û��Ѽ���ֵΪ�Զ����أ�д��ֵĬ��Ϊ0
	RTC_SetWakeUpCounter(0);
	
	//���rtc�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_WakeUpCmd(ENABLE);
	
	//����rtc�жϵĴ�����ʽ��ʱ�份���ж�
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//�����ⲿ�ж�22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//�����ж�22���ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
void rtc_backup_inits(void)
{
	//������Դ����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//�����Ա�����ķ���
	PWR_BackupAccessCmd(ENABLE);
	
	//��LSE�޷������������ڲ�LSI
	/* ʹ��LSI*/
	RCC_LSICmd(ENABLE);
	
	/* ����LSI�Ƿ���Ч*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* ѡ��LSI��ΪRTC��Ӳ��ʱ��Դ*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	//ʹ��RTCӲ��ʱ�ӣ�RTC����
	RCC_RTCCLKCmd(ENABLE);
	
	//�ȴ�RTC��ؼĴ���
	RTC_WaitForSynchro();
	
	
	
	//�رջ��ѹ���
	RTC_WakeUpCmd(DISABLE);
	
	//Ϊ���ѹ���ѡ��RTC���úõ�ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//���û��Ѽ���ֵΪ�Զ����أ�д��ֵĬ��Ϊ0
	RTC_SetWakeUpCounter(0);
	
	//���rtc�����жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//ʹ��RTC�����ж�
	RTC_WakeUpCmd(ENABLE);
	
	//����rtc�жϵĴ�����ʽ��ʱ�份���ж�
	RTC_ITConfig(RTC_IT_WUT,ENABLE);

	//�����ⲿ�ж�22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//�����ж�22���ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//���ӳ�ʼ��
void rtc_alarm_init(void)
{	
	//����rtc����A�ж�
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	//����жϱ�־λ
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	
	//�����ⲿ�ж�17�����������ж�
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//�������ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
}
/*
	DateWeekDay---��ʾҪ���õ����ڻ�������

	cycle      ---1��ʾÿ������
			   ---2��ʾ��������
			   ---3��ʾ��������
	am_pm	   ---��ʾ������
	hour       ---ʱ
	min		   ---��
	secc	   ---��
*/
//��������ʱ��
void rtc_alarm_set(uint8_t DateWeekDay,uint8_t am_pm,uint8_t hour,uint8_t min,uint8_t sec,uint8_t cycle)
{
	//�ر������жϣ������޷�����ʱ��
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//��������Aʱ��
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = am_pm;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = hour;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = min;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = sec;
	switch(cycle)
	{
		case 1://ÿ������
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_DateWeekDay;		//����
				break;
		case 2://ָ����������
				RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = DateWeekDay;						//����
				RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_WeekDay;	//��������
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_None;				//������
				break;
		case 3://ָ����������
				RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = DateWeekDay;						//����
				RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date;	//��������
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_None;				//������
	}
	
	
	
	 //����RTC��A���ӣ�ע��RTC���������������ֱ�Ϊ����A������B 
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	//ʹ������A
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
}
//���ʱ��
void get_datatime(void)
{
	//��ȡʱ��
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);

	//��ȡ����
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	
	//������ʱ���ӡ
	printf("%02x:%02x:%02x\r\n",
			RTC_TimeStructure.RTC_Hours,
			RTC_TimeStructure.RTC_Minutes,
			RTC_TimeStructure.RTC_Seconds);

	printf("20%02x/%02x/%02x Week:%x\r\n",
			RTC_DateStructure.RTC_Year,
			RTC_DateStructure.RTC_Month,
			RTC_DateStructure.RTC_Date,
			RTC_DateStructure.RTC_WeekDay);			
}
uint8_t DEC2BCD(uint8_t dec)  
{  
    return (dec+(dec/10)*6);
} 

int write_flash_data(void)
{
	uint8_t d;
    uint8_t number = 0;
	static uint8_t j=0;
	uint32_t flash_add = 0x8010000;
	uint8_t arr[5] = {0};
	char real_time_humiture[50] = {0};
	
	if(flash_number > 100)
	{
		printf("The number is full\n!");
		return -1;
	}
	
	//��ȡʱ��
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);

	//��ȡ����
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);

	//ÿ�μ�¼��ʪ��
	d = dht11_read(arr);
	//printf("	");
	if(d == 0)
	{
		sprintf(real_time_humiture,"[%03d]20%02x/%02x/%02x Week:%x %02x:%02x:%02x T:%d.%d,H:%d.%d\r\n",flash_number,
								RTC_DateStructure.RTC_Year,
								RTC_DateStructure.RTC_Month,
								RTC_DateStructure.RTC_Date,
								RTC_DateStructure.RTC_WeekDay,
								RTC_TimeStructure.RTC_Hours,
								RTC_TimeStructure.RTC_Minutes,
								RTC_TimeStructure.RTC_Seconds,
								arr[2],arr[3],arr[0],arr[1]
			);
		
		printf("%s",real_time_humiture);
		
		strcpy(flash_data[flash_number++],real_time_humiture);
		
		//����FLASH�������������
		FLASH_Unlock();
		
		//�����ر�־λ
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
								
		//��������4
		if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3))
		{
			printf("FLASH_EraseSector fail\r\n");
			while(1);
		}
		else{
				while(strlen(flash_data[number]))
				{	
					for(j=0;j<50;j++)
					{
						if(FLASH_COMPLETE!=FLASH_ProgramByte(flash_add,flash_data[number][j]))
						{
							printf("FLASH_ProgramWord fail:%d\r\n",flash_number);
							while(1);
						}
						flash_add += 4;
						//printf("%c",flash_data[number][j]);
					}
					
					number++;
				}
			}

		//����FLASH���������޸�FLASH
		FLASH_Lock(); 
		
		memset(arr,0,sizeof(arr));
		memset(real_time_humiture,0,sizeof(real_time_humiture));
	}
	
	return 0;

}

void read_flash4_data(void)
{
	uint8_t number = flash_number;
	uint32_t flash_addr =  0x8010000;
	uint8_t i = 0;
	char real_time_humiture[50] = {0};
	char d;
	FLASH_Unlock();
	//�����ر�־λ
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
	printf("show!The number of data is:%d\n",number);

	while(number)
	{
		i=0;
		while(i<50)
		{
			real_time_humiture[i++] = *(volatile uint32_t *)flash_addr;
			flash_addr +=4;
		}
		printf("%s",real_time_humiture);
		memset(real_time_humiture,0,50);
		number --;
	}
	printf("show is finish\n");
	//����FLASH���������޸�FLASH
	FLASH_Lock(); 

}

void clean_flash4_data(void)
{
	FLASH_Unlock();
	//�����ر�־λ
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
							
	//��������4
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3))
	{
		printf("FLASH_EraseSector fail\r\n");
		while(1);
	}
	flash_number = 0;
	memset(flash_data,0,sizeof(flash_data));
	FLASH_Lock(); 
	printf("Clean the flash data\n");
	
}


