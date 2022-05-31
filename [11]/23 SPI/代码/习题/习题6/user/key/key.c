#include "key.h"
#include "sys.h"


void key_init(void)
{
	//使能端口A的硬件时钟，端口A才能工作，说白了就是对端口A上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//使能端口E的硬件时钟，端口E才能工作，说白了就是对端口E上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		
	
	//使能系统配置时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//配置硬件，配置GPIO，端口A，第0个引脚
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;			//第0 个引脚
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;			//输入模式
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStruct);		
	
	//配置硬件，配置GPIO，端口E，第2 3 4个引脚
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	//第2 3 4 个引脚
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;			//输入模式
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStruct);		


	//将PA0引脚连接到外部中断0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	//将PE2引脚连接到外部中断2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	
	//将PE3引脚连接到外部中断3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	
	//将PE4引脚连接到外部中断4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* 配置外部中断0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;				//指定配置外部中断0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发，识别按键的松开动作。一旦识别到上升沿，就会向CPU申请中断请求 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能
	EXTI_Init(&EXTI_InitStructure);

	/* 配置外部中断2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;				//指定配置外部中断2
	//使能
	EXTI_Init(&EXTI_InitStructure);
	
	/* 配置外部中断3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;				//指定配置外部中断3
	//使能
	EXTI_Init(&EXTI_InitStructure);	
	
	/* 配置外部中断4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;				//指定配置外部中断4
	//使能
	EXTI_Init(&EXTI_InitStructure);		
	
	/*配置外部中断0的优先级*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//外部中断0中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级0x2	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//响应（子）优先级0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能
	NVIC_Init(&NVIC_InitStructure);
	
	
	/*配置外部中断2的优先级*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//外部中断2中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;//抢占优先级0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//响应（子）优先级0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能
	NVIC_Init(&NVIC_InitStructure);
	
	/*配置外部中断3的优先级*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//外部中断3中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;//抢占优先级0x0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//响应（子）优先级0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能
	NVIC_Init(&NVIC_InitStructure);	
	
	/*配置外部中断4的优先级*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//外部中断4中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;//抢占优先级0xF
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应（子）优先级0xF
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能
	NVIC_Init(&NVIC_InitStructure);	
}

