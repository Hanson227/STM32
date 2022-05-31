#include "infrared.h"
#include "sys.h"
#include <stdio.h>
#include "./user/delay/delay.h"
#include "./user/wenshi/wenshi.h"
#include "./user/tim/tim.h"


//红外线初始化
void infrared_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}
//获取红外线的按键代码
int infrared_act(uint8_t *pbuf)
{
	uint32_t t= 0;
	uint8_t i,j;
	uint8_t d=0;
	while(PAin(8))
	{
		t++;
		delay_us(1);
		if(t>5000)
			return -7;
	}
	
	while(PAin(8)==0)
	{
		t++;
		delay_us(10);
		if(t>1000)
			return -1;
	}
	
	t = 0;
	while(PAin(8))
	{
		t++;
		delay_us(10);
		if(t>5000)
			return -2;
	}
	
	for(j=0;j<4;j++)
	{
		for(i=0;i<8;i++)
		{
			t=0;
			while(PAin(8)==0)
			{
				t++;
				delay_us(10);
				if(t > 100)
				return -3;
			}
			delay_us(600);
			if(PAin(8))
			{
				d|=1<<i;
				t=0;
				
				while(PAin(8))
				{
					t++;
					delay_us(10);
					if(t>200)
						return -4;
				}
			}
			
		}
		pbuf[j] = d;
		d=0;
	}
	delay_us(600);
	if(pbuf[0]+pbuf[1] !=255||pbuf[2]+pbuf[3] !=255)
	{
		return -5;
	}	
	return 0;
}
//红外线指令实现灯，蜂鸣器，温湿度指令
void infrared_order(uint8_t oder)
{
	uint8_t beep_fre = 0;
	uint8_t beep_voice = 0;
	uint8_t str[5] = {0};
	static int flag1=0,flag3=0,flag4=0;
	static uint8_t fre1 = 2;
	static uint8_t num = 0;
	static uint8_t voice1 = 10;
	static uint8_t light1 = 60;
	static uint8_t light3 = 60;
	static uint8_t light4 = 60;
	switch(oder)
	{
		case 12://1
				if(flag1 == 0)
				{
					TIM_SetCompare1(TIM14,light1);
					flag1++;
				}
				else if(flag1 ==1)
				{
					TIM_SetCompare1(TIM14,100);
					flag1=0;
				}
				beep_fre  = 0;
				beep_voice = 0;
				break;
		case 24://2
				PFout(10) ^=1;
				beep_fre  = 0;
				beep_voice = 0;
				break;
		case 94://3
				if(flag3 == 0)
				{
					TIM_SetCompare3(TIM1,light3);
					flag3++;
				}
				else if(flag3 ==1)
				{
					TIM_SetCompare3(TIM1,100);
					flag3=0;
				}
				PEout(13) ^=1;
				beep_fre  = 0;
				beep_voice = 0;
				break;
		case 8://4
				if(flag4 == 0)
				{
					TIM_SetCompare4(TIM1,light4);
					flag4++;
				}
				else if(flag4 ==1)
				{
					TIM_SetCompare4(TIM1,100);
					flag4=0;
				}
				beep_fre  = 0;
				beep_voice = 0;
				break;
		case 28://5
				beep_fre  = 1;
				beep_voice = 0;
				break;
		case 90://6
				beep_voice = 1;
				beep_fre  = 0;
				break;
		case 66://7
				beep_fre  = 0;
				beep_voice = 0;
				dht11_read(str);
				printf("T:%d.%d,H:%d.%d\r\n",str[2],str[3],str[0],str[1]);
				break;
		case 82://8
				break;
		case 74://9
				break;
		case 7://-
				if(beep_fre ==1)
				{
					if(fre1-2>0)
						fre1-=2;
					tim13_set_freq(fre1);
					tim13_set_duty(voice1);	
				}
				else if(beep_voice == 1)
				{
					if(voice1-2>0)
						voice1-=2;
					
					tim13_set_duty(voice1);	
				}
				else if(flag1 == 1 &&flag3 == 0 &&flag4 == 0)
				{
					if(light1-30>0)
						light1-=30;
					TIM_SetCompare1(TIM14,light1);
				}
				else if(flag3 == 1 &&flag1 == 0 &&flag4 == 0)
				{
					if(light3-30>0)
						light3-=30;
					TIM_SetCompare3(TIM1,light3);
				}
				else if(flag4 == 1&&flag1 == 0 &&flag3 == 0)
				{
					if(light4-30>0)
					light4-=30;
					TIM_SetCompare4(TIM1,light4);
				}
				else if(flag1+flag3+flag4 > 1)
				{
					if(num==1)
					{
						if(light1-30>0)
						light1-=30;
						TIM_SetCompare1(TIM14,light1);
					}
					else if(num==2)
					{
						if(light3-30>0)
						light3-=30;
						TIM_SetCompare3(TIM1,light3);
					}
					else if(num==3)
					{
						if(light4-30>0)
						light4-=30;
						TIM_SetCompare4(TIM1,light4);
					}
				}
				break;
		case 21://+
				if(beep_fre ==1)
				{
					if(fre1+2<20)
						fre1+=2;
					tim13_set_freq(fre1);
					tim13_set_duty(voice1);	
				}
				else if(beep_voice == 1)
				{
					if(voice1+2<20)
						voice1+=2;
					
					tim13_set_duty(voice1);	
				}
				else if(flag1 == 1 &&flag3 == 0 &&flag4 == 0)
				{
					if(light1+30<100)
						light1+=30;
					TIM_SetCompare1(TIM14,light1);
				}
				else if(flag3 == 1 &&flag1 == 0 &&flag4 == 0)
				{
					if(light3+30<100)
						light3+=30;
					TIM_SetCompare3(TIM1,light3);
				}
				else if(flag4 == 1&&flag1 == 0 &&flag3 == 0)
				{
					if(light4+30<100)
					light4+=30;
					TIM_SetCompare4(TIM1,light4);
				}
				else if(flag1+flag3+flag4 > 1)
				{
					if(num==1)
					{
						if(light1+30<100)
						light1+=30;
						TIM_SetCompare1(TIM14,light1);
					}
					else if(num==2)
					{
						if(light3+30<100)
						light3+=30;
						TIM_SetCompare3(TIM1,light3);
					}
					else if(num==3)
					{
						if(light4+30<100)
						light4+=30;
						TIM_SetCompare4(TIM1,light4);
					}
				}
				break;
		case 22://0
				if(num+1<4)num++;
				else num = 1;
				break;
		case 69://CH-
				tim13_set_duty(0);	
				TIM_SetCompare1(TIM14,100);
				PFout(10) = 1;
				TIM_SetCompare3(TIM1,100);
				TIM_SetCompare4(TIM1,100);
				break;
		default:
				break;
	}
}


