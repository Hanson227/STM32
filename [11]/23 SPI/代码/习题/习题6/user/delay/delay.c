#include "delay.h"

void delay_us(uint32_t nus)
{

	SysTick->CTRL = 0;							// 关闭系统定时器
	SysTick->LOAD = nus*(SystemCoreClock/1000000); 	// nus *1us延时，这个计数值
	SysTick->VAL = 0; 							// 清空标志位
	SysTick->CTRL = 5; 							// 使能系统定时器工作，时钟源为系统时钟168MHz
	while ((SysTick->CTRL & 0x00010000)==0);	// 检查CTRL寄存器的16bit是否置1，若置1，就代表计数完毕
	SysTick->CTRL = 0; 							// 关闭系统定时器	
}

void delay_ms(uint32_t nms)
{

	while(nms--)
	{
	
		SysTick->CTRL = 0;							// 关闭系统定时器
		SysTick->LOAD = (SystemCoreClock/1000); 	// 1ms延时，这个计数值
		SysTick->VAL = 0; 							// 清空标志位
		SysTick->CTRL = 5; 							// 使能系统定时器工作，时钟源为系统时钟168MHz
		while ((SysTick->CTRL & 0x00010000)==0);	// 检查CTRL寄存器的16bit是否置1，若置1，就代表计数完毕
		SysTick->CTRL = 0; 							// 关闭系统定时器	
	
	}
}

