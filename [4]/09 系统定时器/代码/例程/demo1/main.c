#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 	GPIO_InitStructure;





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
	
	
	//系统定时器的时钟源168MHz
	//只要系统定时器进行168000000个计数，就代表1秒钟时间的到达
	//只要系统定时器进行168000000/10个计数，就代表1秒/10钟时间的到达	
	//只要系统定时器进行168000000/100个计数，就代表1秒/100钟时间的到达	
#if 0	
	//配置系统定时器触发1000Hz的中断
	//周期时间T=1/f=1/1000Hz=1ms
	
	SysTick_Config(SystemCoreClock / 1000);
#endif

#if 0
	//配置系统定时器触发100Hz的中断
	//周期时间T=1/f=1/100Hz=10ms
	//SysTick_Config(SystemCoreClock / 100);
	SysTick_Config(1680000);
#endif

	//配置系统定时器触发10Hz的中断，有问题，灯是不亮
	//周期时间T=1/f=1/10Hz=100ms
	SysTick_Config(SystemCoreClock / 10);
	
	while(1)
	{
		

	}
}

void SysTick_Handler(void)
{
static 
	uint32_t i=0;

	i++;

	if(i>=5)
	{
		i=0;
		
		//灯状态的翻转
		PFout(9)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}



}
