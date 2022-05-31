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
//GPIO��ʼ���Ľṹ��
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
	
	float pitch,roll,yaw; 		//ŷ����
		

	
	/* GPIOE GPIOFӲ��ʱ��ʹ�ܣ�������GPIOE GPIOF���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
	
	
	/* ����PE13 PE14Ϊ���ģʽ����������ž�������ߵ͵�ƽ�Ĺ��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;	//��13 14������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	
	/* ����PF9 PF10Ϊ���ģʽ����������ž�������ߵ͵�ƽ�Ĺ��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;	//��9 10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	

	PFout(9)=PFout(10)=1;
	PEout(13)=PEout(14)=1;
	
	
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//rtc��ʼ��
	rtc_init();
	
	//����1��ʼ�����Ҳ�����Ϊ115200bps
	usart1_init(115200);
	//��ʼ��OLED 
	OLED_Init();			 
	OLED_Clear();
	//��ʼ��MPU6050	

	MPU_Init();
	
	
	while(mpu_dmp_init())
	{
		printf("MPU6050 ERROR \r\n");
		delay_ms(500);
	}

	//��ʼ����ʱ��
	tim1_init();
	while(1)
	{
		//OLED_Clear();
		if(g_rtc_wakeup_event)
		{
			//��ȡʱ��
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStruct);
			//��ȡ����
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStruct);
			
			memset(date,0,64);
			memset(time,0,64);
			//printf��ӡ����ʱ��
			sprintf(date,"20%02x-%02x-%02x",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date);
			sprintf(time,"%02x:%02x:%02x",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
			//sprintf(week,"Date:%x",RTC_DateStruct.RTC_WeekDay);
			g_rtc_wakeup_event=0;
			//��ȡ���ڼ�
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
			//ͨ��LED�Ʊ�ʾ�����
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






