#ifndef __RTC_H__
#define __RTC_H__

#include "delay.h"
#include "usart.h"

volatile uint32_t 		g_rtc_wakeup_event=0;
volatile uint32_t 		g_rtc_clock_event=0;
//int volatile flag = 0;

static USART_InitTypeDef 		USART_InitStructure;
static EXTI_InitTypeDef 		EXTI_InitStructure;
static NVIC_InitTypeDef			NVIC_InitStructure;
static RTC_TimeTypeDef  		RTC_TimeStructure;
static RTC_InitTypeDef  		RTC_InitStructure;
static RTC_DateTypeDef 			RTC_DateStructure;

void rtc_init(void);
void rtc_alarm_init(void);
void rtc_alarm_set(RTC_AlarmTypeDef RTC_AlarmStructure);
void RTC_WKUP_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);



#endif


