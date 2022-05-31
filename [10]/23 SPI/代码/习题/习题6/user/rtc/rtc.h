#ifndef __RTC__H
#define __RTC__H

#include "stm32f4xx.h"

//年月日星期设置
#define		RTC_YEARS		  0x19
#define		RTC_MONTHS		  RTC_Month_December
#define		RTC_DATES		  0x19
#define		RTC_WEEKDAYS	  RTC_Weekday_Thursday
//时分秒上下午设置
#define 	RTC_H12S		  RTC_H12_PM	// RTC_H12_AM
#define		RTC_HOURS		  0x14
#define		RTC_MINUTESS	  0x03
#define		RTC_SECONDS 	  0x00

//闹钟设置上下午
#define 	AM				  RTC_H12_AM                    
#define 	PM				  RTC_H12_PM 

static EXTI_InitTypeDef   EXTI_InitStructure;
static NVIC_InitTypeDef   NVIC_InitStructure;
static RTC_InitTypeDef	  RTC_InitStructure;
static RTC_DateTypeDef	  RTC_DateStructure;
static RTC_TimeTypeDef	  RTC_TimeStructure;
static RTC_AlarmTypeDef	  RTC_AlarmStructure;



void rtc_inits(void);
void rtc_backup_inits(void);
void rtc_alarm_init(void);
void get_datatime(void);
void rtc_alarm_set(uint8_t DateWeekDay,uint8_t am_pm,uint8_t hour,uint8_t min,uint8_t sec,uint8_t cycle);
int write_flash_data(void);
void read_flash4_data(void);
void clean_flash4_data(void);

uint8_t DEC2BCD(uint8_t dec);

#endif

