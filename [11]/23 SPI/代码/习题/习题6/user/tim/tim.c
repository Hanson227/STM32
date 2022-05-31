#include "tim.h"
#include "sys.h"

void tim13_pwm_init(int freq)
{
	//使能定时器13的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	
	//使能端口F的硬件时钟，端口F才能工作，说白了就是对端口F上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//配置定时器14的输出频率
	//当前时钟频率为10000Hz，输出频率为100Hz，计数值为99.
	
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//计数值，决定当前的输出频率，当前频率为100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = (8400-1);					//预分频值8400，也称之为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//时钟分频,它是第二次分频，但是F407是没有第二次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数的方法
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	
	//配置硬件，配置GPIO，端口F，第9个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;				//第8个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//将PF8引脚复用到定时器13
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);	
	
	//配置PWM输出模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//配置PWM的工作在模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//允许输出脉冲
	//TIM_OCInitStructure.TIM_Pulse = CCR1_Val;						//比较值的设置，其实可以使用另外一个函数专门设置TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//有效状态的输出极性为高电平
	
	//定时器13通道1的初始化
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);
	
	
	//使能定时器13工作

	TIM_Cmd(TIM13, ENABLE);	
}




void tim13_set_freq(uint32_t freq)
{
	/*定时器的基本配置，用于配置定时器的输出脉冲的频率为 freq Hz */
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1; //设置定时脉冲的频率
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //第一次分频，简称为预分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	tim13_cnt= TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
}


void tim13_set_duty(uint32_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_cnt+1) * duty/100;
	
	TIM_SetCompare1(TIM13,cmp);
}

void tim14_pwm_init(int freq)
{
	//使能定时器14的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//使能端口F的硬件时钟，端口F才能工作，说白了就是对端口F上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//配置定时器14的输出频率
	//当前时钟频率为10000Hz，输出频率为100Hz，计数值为99.
	
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//计数值，决定当前的输出频率，当前频率为100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = (8400-1);					//预分频值8400，也称之为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//时钟分频,它是第二次分频，但是F407是没有第二次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数的方法
	
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	//配置硬件，配置GPIO，端口F，第9个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//第9 个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//将PF9引脚连接到定时器14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);	
	
	//配置PWM输出模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//配置PWM的工作在模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 100;						//比较值的设置，其实可以使用另外一个函数专门设置TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//有效状态的输出极性为高电平
	
	//定时器14通道1的初始化
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);

	//使能定时器14工作
	TIM_Cmd(TIM14, ENABLE);		
}

void tim1_pwm_init(int freq)
{
	//使能定时器1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//使能端口E的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//配置硬件，配置GPIO，端口E，第13，14个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;			//第13 个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;						//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;					//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;					//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;					//不需要上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	//配置定时器1的输出频率
	//当前时钟频率为10000Hz，输出频率为100Hz，计数值为99.

	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//计数值，决定当前的输出频率，当前频率为50Hz	
	TIM_TimeBaseStructure.TIM_Prescaler = (16800-1);				//预分频值8400，也称之为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//时钟分频,它是第二次分频，但是F407是没有第二次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//向上计数的方法
	//初始化定时器1
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	//配置PWM输出模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//配置PWM的工作在模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//允许输出脉冲
	TIM_OCInitStructure.TIM_Pulse = 100;						//比较值的设置，其实可以使用另外一个函数专门设置TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//有效状态的输出极性为高电平
	
	//将PF9引脚连接到定时器13，14
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);	
	
	//定时器14通道3，4的初始化
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	//使能定时器14工作
	TIM_Cmd(TIM1, ENABLE);	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //pwm输出
}

