#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;


void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (168000)-1; 	// 配置计数值(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; 		// Disable SysTick

	}	
}

static uint16_t tim13_period=0;

void tim13_init(uint32_t freq)
{
	//使能定时器13的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	
	//配置定时器13分频值、计数值等
	TIM_TimeBaseStructure.TIM_Period= (10000/freq)-1;			//计数值
	
	tim13_period = TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//8400-1+1=8400,进行8400的预分频值，进行第一次分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	
	  /* 通道1工作在PWM1模式下*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//打开/关闭脉冲输出
	TIM_OCInitStructure.TIM_Pulse = 0;							//比较值决定占空比  越大灯越暗
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//有效状态为高电平，则无效状态为低电平

	TIM_OC1Init(TIM13, &TIM_OCInitStructure);

	
	//使能定时器13工作
	TIM_Cmd(TIM13, ENABLE);

}

//设置定时器13的PWM占空比
//0%~100%  0.0 ~ 1
void TIM13_Set_duty(uint16_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_period+1)*duty/100;//比较值
	
	TIM_SetCompare1(TIM13,cmp);
}



int main(void)
{
	uint16_t freq[4]={523,587,659,740};
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		//第8根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//复用模式/多功能模式，交给其他引脚管理
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//将PF8引脚连接到定时器13
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	
	while(1)
	{
		uint32_t i;
		for(i = 0;i<4;i++)
		{
			tim13_init(freq[i]);
			TIM13_Set_duty(25);
			delay_ms(1000);
		}
	
	}
	
}
