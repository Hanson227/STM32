#include "led.h"
#include "sys.h"

void led_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF 
						  |RCC_AHB1Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	//配置硬件，配置GPIO，端口F，第9 10个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	//配置硬件，配置GPIO，端口E，第13 14个引脚	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14;	
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	PFout(9) = 1;
	PFout(10) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
	
}

void beep_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;				//第8个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//PFout(8) = 1;
}

