#include "stm32f4xx.h"
#include "sys.h"

void delay_us(uint32_t t)
{

	SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器
	SysTick->LOAD = (SystemCoreClock/8/1000000)*t; 	// 168000000/1000000=1us,决定了延时的时间[*]
	SysTick->VAL = 0; 								// Clear current value as well as count flag，清空标志位
	SysTick->CTRL = 1; 								// Enable SysTick timer with processor clock，使能系统定时器开始工作，选择8分频后的时钟频率为21MHz
	while ((SysTick->CTRL & 0x00010000)==0);		// Wait until count flag is set，等待计数完毕
	SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器
}



void delay_ms(uint32_t t)
{
	uint32_t m,n;
	
	
	m = t/500;			
	
	n = t%500;			
	
	while(m--)
	{
		SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (SystemCoreClock/8/1000)*500; 	// 168000000/1000=1ms,决定了延时的时间[*]
		SysTick->VAL = 0; 								// Clear current value as well as count flag，清空标志位
		SysTick->CTRL = 1; 								// Enable SysTick timer with processor clock，使能系统定时器开始工作，选择8分频后的时钟频率为21MHz
		while ((SysTick->CTRL & 0x00010000)==0);		// Wait until count flag is set，等待计数完毕
		SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器	
	}
	

	if(n)
	{
		SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (SystemCoreClock/8/1000)*n; 	// 168000000/1000=1ms,决定了延时的时间[*]
		SysTick->VAL = 0; 								// Clear current value as well as count flag，清空标志位
		SysTick->CTRL = 1; 								// Enable SysTick timer with processor clock，使能系统定时器开始工作，选择8分频后的时钟频率为21MHz
		while ((SysTick->CTRL & 0x00010000)==0);		// Wait until count flag is set，等待计数完毕
		SysTick->CTRL = 0; 								// Disable SysTick，关闭系统定时器		
	
	}

}
