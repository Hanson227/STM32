#include <stdio.h>
#include "stm32f4xx.h"
#include "sys.h"	
#include <stdlib.h>
#include <string.h>
#include "./user/led/led.h"
#include "./user/blue/blue.h"
#include "./user/delay/delay.h"
#include "./user/iwdg/iwdg.h"
#include "./user/hcsr/hcsr.h"
#include "./user/wenshi/wenshi.h"
#include "./user/tim/tim.h"
#include "./user/usart/usart.h"
#include "./user/key/key.h"
#include "./user/infrared/infrared.h"
#include "./user/rtc/rtc.h"
#include "./user/ADC/adc.h"
#include "./user/flame/flame.h"
#include "./user/DAC/dac.h"
#include "./user/steering/steering.h"
#include "./user/SPI/spi.h"
#include "./user/rc522/rc522.h"
#include "./user/i2c/i2c.h"

#define 	N   12

uint32_t fre = 2;		//蜂鸣器频率
uint32_t voice = 20;	//蜂鸣器声音
	
uint8_t KEY1=0;			//按键1未按下
uint8_t KEY2=0;			//按键2未按下
uint8_t KEY3=0;			//按键3未按下
uint8_t KEY4=0;			//按键4未按下

char buf[30];										//接收蓝牙发送的指令
static volatile uint32_t g_rtc_wakeup_event=0;		//唤醒指令
static volatile uint32_t usart3_get_data = 0;		//蓝牙指令的标志位（usart3）

uint32_t g_steering_pwm=0;			//舵机pwm

//中值平均滤波
uint32_t filter(uint32_t *adc_val_buf)
{
	uint32_t *buf = adc_val_buf;
	
	uint32_t count,i,j;
	uint32_t Value_buf[N] = {0};
	uint32_t sum=0;
	uint32_t temp=0;
	for(count=0;count<N;count++)
	{
		Value_buf[count]= buf[count];
		//printf("第%d次温度---%d\n",count,buf[count]);
	}
	for(j=0;j<(N-1);j++)
	{
		for(i=0;i<(N-j);i++)
		{
			if(Value_buf[i]>Value_buf[i+1])
			{
				 temp = Value_buf[i];
				 Value_buf[i]= Value_buf[i+1];
				  Value_buf[i+1]=temp;
			}		
		
		
		}
	}
	for(count =1;count<N-1;count++)
		sum += Value_buf[count];
	
	return (sum/(N-2));
}

int main(void)
{


	//初始化led
	led_init();
	
	//按键初始化
	key_init();
	
	//蜂鸣器初始化
	beep_init();

//	//串口1波特率为115200bps
	usart1_init(115200);
	
	
	dht11_init();
	
	rtc_inits();

	printf("This is spi rfid test\r\n");
	
    while(1)
	{
	
		/***************************RFID模块******************************/
	
		MFRC522_Initializtion();
		MFRC522_humiture();
		delay_ms(1000);
	}

}

void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) == SET)
	{
		//printf("RTC_WKUP_IRQHandler\n");
		//触发每秒事件
		g_rtc_wakeup_event=1;
		EXTI_ClearFlag(EXTI_Line22);
		RTC_ClearITPendingBit(RTC_IT_WUT);
	}
}



//串口3中断
void USART3_IRQHandler(void)
{
	uint8_t d;
	static int num=0;

	//检查是否有数据接收
	if(USART_GetITStatus(USART3, USART_IT_RXNE) ==SET)
	{
		//接收数据
		d = USART_ReceiveData(USART3);
		//USART_SendData(USART1,d);
	
		if(d!='#')
		{
			buf[num++] = d;
		}
		else
		{
			num = 0;
			usart3_get_data =1;			
		}
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	
	}
}



