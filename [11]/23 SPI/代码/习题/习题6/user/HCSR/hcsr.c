#include "hcsr.h"
#include "sys.h"
#include <stdio.h>
#include "./user/delay/delay.h"
#include "./user/tim/tim.h"



//超声波hc_sr04初始化
void sr04_init(void)
{

	//使能端口B的硬件时钟，端口B才能工作，说白了就是对端口B上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//使能端口E的硬件时钟，端口E才能工作，说白了就是对端口E上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	

	//配置硬件，配置GPIO，端口B，第6个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//第6个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//配置硬件，配置GPIO，端口E，第6个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//第6个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			//输入模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);


	//关于输出模式的引脚，必须给予初始的电平，该电平一定要看时序图【*】
	PBout(6)=0;
}

//测距函数
int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	PBout(6)=1;
	//持续高电平时间必须10us以上，当前填写20us
	delay_us(20);
	PBout(6)=0;

	//等待回响引号引脚输出高电平，就代表说有信号返回
	while(PEin(6)==0)
	{
		t++;
		delay_us(1);
		//假如等待时间超过100ms，则认为当前测量有异常
		if(t >=100000)
			return -1;
	}

	//测量高电平的持续时间
	t=0;
	while(PEin(6))
	{
		t++;
		delay_us(9); //3mm
		//假如等待时间超过900ms，则认为当前测量有异常
		if(t >=100000)
			return -2;
	
	}
	//因为当前传输的时间=发送到返回的时间
	t=t/2;

	return 3*t;

}

void check_dis(uint32_t d)
{
//对距离的有效性进行判断
	if(d >=20 && d<=4000)
	{	
		printf("distance:%dmm\r\n",d);
		
		//＞45cm：理想安全区，所有LED灯熄灭
		if(d>=450)
		{
			PFout(9) = 1;
			PFout(10) = 1;
			PEout(13) = 1;
			PEout(14) = 1;	

			//关闭蜂鸣器
			tim13_set_duty(0);
			
		}
		//35~45cm：非常安全区，亮1盏LED灯
		else if(d>=350)
		{
			PFout(9) = 0;
			PFout(10) = 1;
			PEout(13) = 1;
			PEout(14) = 1;		
			//关闭蜂鸣器
			tim13_set_duty(0);							
		}
		//25~35cm：安全区，亮2盏LED灯
		else if(d>=250)
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 1;
			PEout(14) = 1;	
			//蜂鸣器开始工作
			tim13_set_freq(2);
			tim13_set_duty(voice);				

		}
		//15~25cm：警告区，亮3盏LED灯
		else if(d>=150)
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 0;
			PEout(14) = 1;			
			//蜂鸣器开始工作
			tim13_set_freq(4);
			tim13_set_duty(voice);	
		}
		//＜15cm：危险区，亮4盏LED灯
		else
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 0;
			PEout(14) = 0;				
			//蜂鸣器开始工作
			tim13_set_freq(10);
			tim13_set_duty(voice);	
		}
	}
	
}
