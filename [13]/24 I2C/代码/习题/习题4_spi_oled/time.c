#include "time.h"


volatile uint8_t countdown = 0;

void tim1_init(void)
{
	//使能定时器3硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//配置定时器3分频值、计数值等
	TIM_TimeBaseStructure.TIM_Period = 10000-1;			//计数值，0~4999，决定定时时间为1/2秒
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;		//8400-1+1=8400,进行8400的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	//配置定时器3中断的触发方式：时间更新
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	//配置定时器3的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	//使能定时器3工作
	TIM_Cmd(TIM1, ENABLE);
}


void TIM1_UP_TIM10_IRQHandler(void)
{
	//判断标志位
	if(SET==TIM_GetITStatus(TIM1, TIM_IT_Update))
	{
		countdown++;
	
	
		//清空标志位
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
