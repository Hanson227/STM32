#include "rtc.h"
#include "sys.h"
#include <stdio.h>
#include <string.h> 
#include "./user/wenshi/wenshi.h"

char flash_data[100][50] = {0};
static uint8_t flash_number = 0;
void rtc_inits(void)
{
	//开启电源管理时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//启动对备份域的访问
	PWR_BackupAccessCmd(ENABLE);
	
#if 0
	//使用LSE振荡时钟
	RCC_LSEConfig(RCC_LSE_ON);
	
	//检查LSE振荡时钟是否就绪
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

	//选中LSE作为RTC的时钟源
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	

#else  //若LSE无法工作，可用内部LSI
	/* 使能LSI*/
	RCC_LSICmd(ENABLE);
	
	/* 检查该LSI是否有效*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 选择LSI作为RTC的硬件时钟源*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
#endif	
	
	
	
	//使能RTC硬件时钟，RTC工作
	RCC_RTCCLKCmd(ENABLE);
	
	//等待RTC相关寄存器
	RTC_WaitForSynchro();
	
	//配置RTC的分频值和24小时格式，并初始化
	RTC_InitStructure.RTC_AsynchPrediv = 127;
	RTC_InitStructure.RTC_SynchPrediv = 255;
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_Init(&RTC_InitStructure);
	
	//配置年月日周，并设置
	RTC_DateStructure.RTC_Year = RTC_YEARS;
	RTC_DateStructure.RTC_Month = RTC_Month_December;
	RTC_DateStructure.RTC_Date = RTC_DATES;
	RTC_DateStructure.RTC_WeekDay = RTC_WEEKDAYS;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	//设置时分秒，并设置
	RTC_TimeStructure.RTC_H12 = RTC_H12S;
	RTC_TimeStructure.RTC_Hours = RTC_HOURS;
	RTC_TimeStructure.RTC_Minutes = RTC_MINUTESS;
	RTC_TimeStructure.RTC_Seconds = RTC_SECONDS;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
	
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认为0
	RTC_SetWakeUpCounter(0);
	
	//清除rtc唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_WakeUpCmd(ENABLE);
	
	//配置rtc中断的触发方式：时间唤醒中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//配置外部中断22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//配置中断22优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
void rtc_backup_inits(void)
{
	//开启电源管理时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	//启动对备份域的访问
	PWR_BackupAccessCmd(ENABLE);
	
	//若LSE无法工作，可用内部LSI
	/* 使能LSI*/
	RCC_LSICmd(ENABLE);
	
	/* 检查该LSI是否有效*/  
	while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

	/* 选择LSI作为RTC的硬件时钟源*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	//使能RTC硬件时钟，RTC工作
	RCC_RTCCLKCmd(ENABLE);
	
	//等待RTC相关寄存器
	RTC_WaitForSynchro();
	
	
	
	//关闭唤醒功能
	RTC_WakeUpCmd(DISABLE);
	
	//为唤醒功能选择RTC配置好的时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	//设置唤醒计数值为自动重载，写入值默认为0
	RTC_SetWakeUpCounter(0);
	
	//清除rtc唤醒中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//使能RTC唤醒中断
	RTC_WakeUpCmd(ENABLE);
	
	//配置rtc中断的触发方式：时间唤醒中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);

	//配置外部中断22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//配置中断22优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//闹钟初始化
void rtc_alarm_init(void)
{	
	//允许rtc闹钟A中断
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);
	//清除中断标志位
	RTC_ClearFlag(RTC_FLAG_ALRAF);
	
	//配置外部中断17，用于闹钟中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	//配置优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
}
/*
	DateWeekDay---表示要设置的星期或者日期

	cycle      ---1表示每天闹钟
			   ---2表示星期闹钟
			   ---3表示日期闹钟
	am_pm	   ---表示上下午
	hour       ---时
	min		   ---分
	secc	   ---秒
*/
//设置闹钟时间
void rtc_alarm_set(uint8_t DateWeekDay,uint8_t am_pm,uint8_t hour,uint8_t min,uint8_t sec,uint8_t cycle)
{
	//关闭闹钟中断，否则无法设置时间
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	
	//配置闹钟A时间
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = am_pm;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = hour;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = min;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = sec;
	switch(cycle)
	{
		case 1://每天闹钟
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_DateWeekDay;		//屏蔽
				break;
		case 2://指定星期闹钟
				RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = DateWeekDay;						//星期
				RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_WeekDay;	//星期闹钟
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_None;				//不屏蔽
				break;
		case 3://指定日期闹钟
				RTC_AlarmStructure.RTC_AlarmDateWeekDay		 = DateWeekDay;						//日期
				RTC_AlarmStructure.RTC_AlarmDateWeekDaySel	 = RTC_AlarmDateWeekDaySel_Date;	//日期闹钟
				RTC_AlarmStructure.RTC_AlarmMask 			 = RTC_AlarmMask_None;				//不屏蔽
	}
	
	
	
	 //配置RTC的A闹钟，注：RTC的闹钟有两个，分别为闹钟A与闹钟B 
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
	
	//使能闹钟A
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	
}
//获得时间
void get_datatime(void)
{
	//获取时间
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);

	//获取日期
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	
	//将日期时间打印
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
	
	//获取时间
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);

	//获取日期
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);

	//每次记录温湿度
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
		
		//解锁FLASH，才能允许访问
		FLASH_Unlock();
		
		//清空相关标志位
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
								
		//擦除扇区4
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

		//锁定FLASH，不允许修改FLASH
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
	//清空相关标志位
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
	//锁定FLASH，不允许修改FLASH
	FLASH_Lock(); 

}

void clean_flash4_data(void)
{
	FLASH_Unlock();
	//清空相关标志位
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
							
	//擦除扇区4
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


