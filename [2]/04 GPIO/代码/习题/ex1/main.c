#include "stm32f4xx.h"


static GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;
	
	while(i--);
}

int main(void)
{
	//led1:PF9 led2:PF10 led3:PE13 led4:PE14
	
	//使能(打开)端口E的硬件时钟，就是对端口E供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;				//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//没有使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		//第13 14根引脚
	GPIO_Init(GPIOE,&GPIO_InitStructure);

	//所有led熄灭
	GPIO_SetBits(GPIOF,GPIO_Pin_9|GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13|GPIO_Pin_14);	
	
	while(1)
	{
		//PF9引脚输出低电平
		GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		delay();	
		
		GPIO_SetBits(GPIOF,GPIO_Pin_9);	
		delay();	


		GPIO_ResetBits(GPIOF,GPIO_Pin_10);
		delay();	
		
		GPIO_SetBits(GPIOF,GPIO_Pin_10);	
		delay();	


		GPIO_ResetBits(GPIOE,GPIO_Pin_13);
		delay();	
		
		GPIO_SetBits(GPIOE,GPIO_Pin_13);	
		delay();


		GPIO_ResetBits(GPIOE,GPIO_Pin_14);
		delay();	
		
		GPIO_SetBits(GPIOE,GPIO_Pin_14);	
		delay();
	}



}
