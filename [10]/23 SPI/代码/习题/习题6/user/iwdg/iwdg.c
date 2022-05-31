#include "iwdg.h"

void iwdg(void)
{
	//使用是内部时钟32khz
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//允许写操作
	
	//设置看门狗的分频值，决定看门狗的时钟频率
	//独立看门狗的时钟频率决定了当前计数值
	//独立看门狗的时钟频率= 32khz /256分频 = 125hz；
	//只要进行125次计数，就是1秒钟时间
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	IWDG_SetReload(375-1);		//3秒喂一次狗
	
	IWDG_ReloadCounter();		//重新加载计数值到计数器中
	
	IWDG_Enable();				//使能看门狗
}

void wwdg(void)
{
	//开启窗口看门狗时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
	//配置分频系数
	//窗口看门狗的硬件时钟频率=(42MHz/4096)/8≈1281Hz
	//						（时钟频率/4096）/分频系数
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	
	//设置窗口上限值
	WWDG_SetWindowValue(80);
	
	//设置计数值并使能窗口看门狗
	WWDG_Enable(127);
}
