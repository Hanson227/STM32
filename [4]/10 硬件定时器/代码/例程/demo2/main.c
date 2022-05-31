#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统定时器
	SysTick->LOAD = (168*n)-1; // 配置计数值(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (168000)-1; 	// 配置计数值(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

}


void tim1_init(void)
{
	//使能定时器1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//配置定时器1分频值、计数值等
	//定时器的硬件时钟频率=168MHz/8400=20000Hz
	TIM_TimeBaseStructure.TIM_Period = (20000/2)-1;		//计数值，0~4999，决定定时时间为1/4秒
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//16800-1+1=16800,进行16800的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	
	//配置定时器1中断的触发方式：时间更新
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	
	//配置定时器1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能定时器1工作
	TIM_Cmd(TIM1, ENABLE);

}


int main(void)
{
	
		
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	
	//定时器1的初始化
	tim1_init();
	
	while(1)
	{
				
			
	}
}


void TIM1_UP_TIM10_IRQHandler(void)
{
	//判断标志位
	if(SET==TIM_GetITStatus(TIM1, TIM_IT_Update))
	{
		PFout(9)^=1;
	
	
		//清空标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
	//判断标志位
//	if(SET==TIM_GetITStatus(TIM10, TIM_IT_Update))
//	{
//		PFout(9)^=1;
//	
//	
//		//清空标志位
//		TIM_ClearITPendingBit(TIM10,TIM_IT_Update);
//	}	
	
}

