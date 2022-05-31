#include "stm32f4xx.h"
#include "sys.h"
#include "string.h"
#include "stdio.h"
#include "delay.h"
#include "usart.h"
#include "i2c.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"
#include "oled.h"
#include "bmp.h"


extern volatile uint32_t 		g_rtc_wakeup_event;
extern volatile uint8_t countdown;
//GPIO初始化的结构体
static GPIO_InitTypeDef  GPIO_InitStructure;
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_AlarmTypeDef RTC_AlarmStructure;

int main(void)
{
	char day[7][20] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
	
	uint8_t res;
	
	char date[64]={0};
	char time[64]={0};
	char week[64]={0};
	
	float pitch,roll,yaw; 		//欧拉角
		

	
	/* GPIOE GPIOF硬件时钟使能，就是让GPIOE GPIOF工作 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
	
	
	/* 配置PE13 PE14为输出模式，让这根引脚具有输出高低电平的功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;	//第13 14号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	
	/* 配置PF9 PF10为输出模式，让这根引脚具有输出高低电平的功能 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;	//第9 10号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	

	PFout(9)=PFout(10)=1;
	PEout(13)=PEout(14)=1;
	
	
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//rtc初始化
	rtc_init();
	
	//串口1初始化，且波特率为115200bps
	usart1_init(115200);
	//初始化OLED 
	OLED_Init();			 
	OLED_Clear();
	//初始化MPU6050	

	MPU_Init();
	
	
	while(mpu_dmp_init())
	{
		printf("MPU6050 ERROR \r\n");
		delay_ms(500);
	}

	//初始化定时器
	tim1_init();
	while(1)
	{
		//OLED_Clear();
		if(g_rtc_wakeup_event)
		{
			//获取时间
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStruct);
			//获取日期
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStruct);
			
			memset(date,0,64);
			memset(time,0,64);
			//printf打印日期时间
			sprintf(date,"20%02x-%02x-%02x",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
			sprintf(time,"%02x:%02x:%02x",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
			//sprintf(week,"Date:%x",RTC_DateStruct.RTC_WeekDay);
			g_rtc_wakeup_event=0;
			//获取星期几
			switch(RTC_DateStruct.RTC_WeekDay)
			{
				case 1:
					sprintf(week,"Date:%s",day[0]);
					break;
				case 2:
					sprintf(week,"Date:%s",day[1]);
					break;
				case 3:
					sprintf(week,"Date:%s",day[2]);
					break;
				case 4:
					sprintf(week,"Date:%s",day[3]);
					break;
				case 5:
					sprintf(week,"Date:%s",day[4]);
					break;
				case 6:
					sprintf(week,"Date:%s",day[5]);
					break;
				case 7:
					sprintf(week,"Date:%s",day[6]);
					break;
			}
				
			OLED_ShowString(30,0,time,16);
			OLED_ShowString(24,2,date,16);
			OLED_ShowString(20,6,week,16);
			
		}
		if(countdown >= 5)
		{
			countdown = 0;
			OLED_Display_Off();
			TIM_Cmd(TIM1, DISABLE);
		}
		
		
		res=mpu_dmp_get_data(&pitch,&roll,&yaw);
		if(res==0)
		{ 

			printf("pitch=%f,roll=%f,yaw=%f\r\n",pitch,roll,yaw);
			
			delay_ms(100);
			//通过LED灯表示横滚角
			if(roll<5)
			{
				PFout(9)=PFout(10)=1;
				PEout(13)=PEout(14)=1;
			}
			else if(roll<35)
			{
				countdown = 0;
				OLED_Display_On();	
				TIM_Cmd(TIM1, ENABLE);				
			}
					
			else
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=0;			
			}				
		}	

	}

}






