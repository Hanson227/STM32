#include "stm32f4xx.h"


static GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;
	
	while(i--);
}

/*
#define GPIOF               	((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOF_BASE            	(AHB1PERIPH_BASE + 0x1400)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define PERIPH_BASE           ((uint32_t)0x40000000)

端口F的ODR寄存器地址 = 0x40000000+0x20000+0x1400+0x14=0x40021414;

映射地址，别名区域地址=0x42000000+(0x40021414-0x40000000)*32+9*4=0x404282A4

*/



#define PEout(n)	*(volatile uint32_t *)(0x42000000+((uint32_t)&GPIOE->ODR - 0x40000000)*32+n*4)
#define PFout(n)	*(volatile uint32_t *)(0x42000000+((uint32_t)&GPIOF->ODR - 0x40000000)*32+n*4)

#define PAin(n)		*(volatile uint32_t *)(0x42000000+((uint32_t)&GPIOA->IDR - 0x40000000)*32+n*4)
	

static uint32_t 	g_cnt=0;

int main(void)
{
	
	uint32_t t=0x12345678;
	
	//使能(打开)端口A的硬件时钟，就是对端口A供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//第0根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//输入模式
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	PFout(9)=1;
	
	
	
	while(1)
	{
		
		//判断PA0引脚的电平
		if(PAin(0) == 0)
		{
			PFout(9)=0;
			
			g_cnt++;
		}
		else
		{
			PFout(9)=1;		
		}
		

		
	}



}
