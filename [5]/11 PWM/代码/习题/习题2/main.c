#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;

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


void tim14_init(void)
{
	//使能定时器14硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//配置定时器14分频值、计数值等
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;	//计数值，0~99，决定输出频率为100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 4200-1;		//8400-1+1=8400,进行8400的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	
	/* 通道1工作在PWM1模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//打开/关闭脉冲输出
	TIM_OCInitStructure.TIM_Pulse = 100;							 //比较值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //有效状态为高电平，则无效状态为低电平
	
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	
	
	//使能定时器14工作
	TIM_Cmd(TIM14, ENABLE);
}


void tim1_init(void)
{
	//使能定时器1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//配置定时器1分频值、计数值等
	//定时器1硬件时钟频率=（84MHz * 2）/TIM_Prescaler（16800）=10000Hz
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;	//计数值，0~99，决定输出频率为100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;		//8400-1+1=8400,进行8400的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* 通道1工作在PWM1模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//打开/关闭脉冲输出
	TIM_OCInitStructure.TIM_Pulse = 100;							 //比较值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //有效状态为高电平，则无效状态为低电平
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	

	//使能定时器1工作
	TIM_Cmd(TIM1, ENABLE);
	
	//控制PWM的主输出
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


int main(void)
{
	
	int32_t pwm_cmp=0;
	
	//使能(打开)端口E的硬件时钟，就是对端口E供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//复用功能模式，该引脚交给其他硬件自动管理
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		//第13根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//复用功能模式，该引脚交给其他硬件自动管理
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	//将PF9引脚连接到定时器14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	//将PE13引脚连接到定时器13
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);	
	
	//定时器14的初始化
	tim14_init();
	
	//定时器1的初始化	
	tim1_init();
	
	while(1)
	{
		//灯渐灭
		for(pwm_cmp=0; pwm_cmp<=100; pwm_cmp++)
		{
			//设置定时器14通道1比较值
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			//设置定时器1通道3比较值
			TIM_SetCompare3(TIM1,pwm_cmp);			
			
			delay_ms(20);
		}
		//灯渐亮		
		for(pwm_cmp=100; pwm_cmp>=0; pwm_cmp--)
		{
			//设置定时器14通道1比较值		
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			//设置定时器1通道3比较值
			TIM_SetCompare3(TIM1,pwm_cmp);				
			
			delay_ms(20);
		}		
		
	}
}

