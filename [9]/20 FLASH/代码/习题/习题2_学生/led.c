#include "led.h"



//beep初始化
void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能(打开)端口E、F的硬件时钟，就是对端口供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//指定引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //没有使能上拉或下拉电阻	
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	//将PF8置为低电平
	PFOut(8) = 0;
}

//led初始化
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能(打开)端口E、F的硬件时钟，就是对端口供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//指定引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //没有使能上拉或下拉电阻	
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//指定引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出模式
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	//将PF10置为高电平
	PFOut(9) = 1;
	PFOut(10) = 1;
	PEOut(13) = 1;
	PEOut(14) = 1;

}


