#include "blue.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "./user/usart/usart.h"
#include "./user/delay/delay.h"
#include "./user/wenshi/wenshi.h"
#include "./user/tim/tim.h"
#include "./user/rtc/rtc.h"
static RTC_DateTypeDef	  RTC_DateStructure;
static RTC_TimeTypeDef	  RTC_TimeStructure;

void ble_set_config(void)
{
	//设置模块名字
	usart3_send_str("AT+NAMESONG\r\n");
	delay_ms(500);
	
//	usart3_send_str("AT+LADDR\r\n");
//	delay_ms(500);
	
	//复位模块
	usart3_send_str("AT+RESET\r\n");
	delay_ms(1000);
}
//根据不同蓝牙指令实现效果
void blue_change_beep(void)
{
	uint8_t pwm = 0;
	char *str;
	const char flag[2] = {"="};
	uint8_t buf1[5]={0};	//接收温湿度数据数组
	if(strlen(buf) > 4 && strlen(buf) <=30)
	{
		str = strtok(buf,flag);
		if(str!=NULL)
		{
			//printf("str:%s\n",str);
			if(strcmp(str,"duty") ==0)
			{
				str = strtok(NULL,flag);
				pwm = atoi(str);
				TIM_SetCompare1(TIM14,pwm);
				printf("duty set %d ok\r\n",pwm);
			}
			else if(strcmp(str,"beep") ==0)
			{
				str = strtok(NULL,flag);
				if(strcmp(str,"on") ==0)
				{
					tim13_set_freq(2);
					tim13_set_duty(voice);	
					printf("beep on ok\r\n");
				}
				else if(strcmp(str,"off") ==0)
				{
					tim13_set_duty(0);	
					printf("beep off ok\r\n");
				}		
			}
			else 
			{
				str = strtok(buf,"?");
				 if(strcmp(str,"temp") ==0)
				{
					dht11_read(buf1);
					printf("T:%d.%d\r\n",buf1[2],buf1[3]);
				}
				else if(strcmp(str,"humi") ==0)
				{
					dht11_read(buf1);
					printf("H:%d.%d\r\n",buf1[0],buf1[1]);
				}	
			}
			
			
		}
		memset(buf,0,sizeof(buf));
		memset(buf1,0,sizeof(buf1));
	}
}

void blue_change_time(void)
{
	uint32_t year=0,month=0,day=0,weekday=0;
	uint32_t hour=0,min=0,sec=0,cycle = 0;
	char *str;
	uint8_t buf1[5]={0};	//接收温湿度数据数组
	const char flag[2] = {"-"};
	if(strlen(buf) > 4 && strlen(buf) <=30)
	{
		str = strtok(buf,flag);
		if(strcmp(str,"DATE SET") ==0)
		{	
			str = strtok(NULL,flag);
			year = atoi(str);
			if(year>=2000)				//判断年份范围
				year -= 2000;
			else if(year >=1970 && year <2000)
				year -= 1900;
			
			str = strtok(NULL,flag);
			month = atoi(str);
			
			str = strtok(NULL,flag);
			day = atoi(str);
			
			str = strtok(NULL,flag);
			weekday = atoi(str);
			
			year = DEC2BCD(year);
			month = DEC2BCD(month);
			day = DEC2BCD(day);
			weekday = DEC2BCD(weekday);
			printf("y:%x",year);
			//设置年月日
			RTC_DateStructure.RTC_Year = year;
			RTC_DateStructure.RTC_Month = month;
			RTC_DateStructure.RTC_Date = day;			
			RTC_DateStructure.RTC_WeekDay = weekday;
			
			RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
		}
		else if(strcmp(str,"TIME SET") ==0)
		{	
			str = strtok(NULL,flag);
			hour = atoi(str);

			str = strtok(NULL,flag);
			min = atoi(str);
			
			str = strtok(NULL,flag);
			sec = atoi(str);
			
			//设置时间
			RTC_TimeStructure.RTC_Hours = hour;
			RTC_TimeStructure.RTC_Minutes = min;
			RTC_TimeStructure.RTC_Seconds = sec;			
			RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
		}
		else if(strcmp(str,"ALARM SET") ==0)
		{	
			str = strtok(NULL,flag);
			hour = atoi(str);

			str = strtok(NULL,flag);
			min = atoi(str);
			
			str = strtok(NULL,flag);
			sec = atoi(str);
			//闹钟日期/星期
			str = strtok(NULL,flag);
			weekday = atoi(str);
			 //闹钟周期
			str = strtok(NULL,flag);
			cycle = atoi(str);
			//设置闹钟
			rtc_alarm_set(weekday,PM,hour,min,sec,cycle);	//第一个参数为星期/日期，最后一个参数表示闹钟周期
		}
		
	}
	
}


