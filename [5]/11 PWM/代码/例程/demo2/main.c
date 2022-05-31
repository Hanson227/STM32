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


void tim3_init(void)
{
	//使能定时器3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//配置定时器3分频值、计数值等
	TIM_TimeBaseStructure.TIM_Period = (10000/20)-1;	//计数值，0~499，决定输出频率为20Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//4200-1+1=4200,进行4200的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	
	/* 通道1工作在PWM1模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//打开/关闭脉冲输出
	TIM_OCInitStructure.TIM_Pulse = 250;						 //比较值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //有效状态为高电平，则无效状态为低电平
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	
	//使能定时器3工作
	TIM_Cmd(TIM3, ENABLE);

}


int main(void)
{
	
	int32_t pwm_cmp=0;
	
	//使能(打开)端口C的硬件时钟，就是对端口C供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//第6根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//复用功能模式，该引脚交给其他硬件自动管理
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//将PC6引脚连接到定时器3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	
	
	//定时器3的初始化
	tim3_init();
	
	while(1)
	{

		
	}
}

