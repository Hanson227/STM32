#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

static uint32_t iwdg_flag=1;

#pragma import(__use_no_semihosting_swi)
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	
	return ch;
}
void _sys_exit(int return_code) {

}

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
	TIM_TimeBaseStructure.TIM_Period = (10000/2)-1;			//计数值，10000相当于1s，0~4999，决定定时时间为1/2秒
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//8400-1+1=8400,进行8400的预分频值，进行第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//在F407是不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数的方法
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	//配置定时器3中断的触发方式：时间更新
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	//配置定时器3的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//使能定时器3工作
	TIM_Cmd(TIM3, ENABLE);
}

void usart1_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口1工作
	USART_Cmd(USART1, ENABLE);
}

void dht11_init(void)
{

	//端口G硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	
	
	//配置PG9为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);		
	
	
	//PG9初始状态为高电平，看时序图
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	uint32_t check_sum=0;
	
	//PG9设置为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	
	
	PGout(9)=0;
	delay_ms(20);
	
	PGout(9)=1;
	delay_us(30);	
	
	
	//PG9设置为输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	//等待低电平出现
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		if(t>=4000)
			return -1;

	}
	
	
	//测量低电平合法性
	t=0;
	while(PGin(9)==0)
	{
		t++;
		
		delay_us(1);
		
		if(t>=1000)
			return -2;
	
	}

	//测量高电平合法性
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		if(t>=1000)
			return -3;
	}

	for(j=0; j<5; j++)
	{
		d=0;
		//一个字节的接收，从最高有效位开始接收
		for(i=7; i>=0; i--)
		{
		
			//等待数据0/数据1的前置低电平持续完毕
			t=0;
			while(PGin(9)==0)
			{
				t++;
				
				delay_us(1);
				
				if(t>=1000)
					return -4;
			
			}

			//延时40us
			delay_us(40);
		
			//判断当前PG9是否为高电平还是低电平
			//若是高电平，则为数据1；
			//若是低电平，则为数据0；
			if(PGin(9))
			{
				d|=1<<i;//将变量d对应的比特位置1，如i=7，d|=1<<7就是将变量d的bit7置1
			
			
				//等待高电平持续完毕
				t=0;
				while(PGin(9))
				{
					t++;
					
					delay_us(1);
					
					if(t>=1000)
						return -5;
				}		
			}
		}
		
		p[j]=d;	
	}
	
	//校验和
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	
	return 0;
	
}

void iwdg_init(void)
{
	//允许访问独立看门狗
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//配置独立看门狗的分频值为256
	//独立看门狗的硬件时钟=32KHz/256=125Hz
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	//设置独立看门狗的计数值124
	//计数范围：0~124，共进行125次计数，就是1秒时间的到达
	IWDG_SetReload(125-1);
	
	//重载计数值==刷新计数值
	IWDG_ReloadCounter();
	
	
	//使能独立看门狗工作
	IWDG_Enable();
}

int main(void)
{
	int32_t rt=0;
	uint8_t buf[5]={0};
	
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//串口1波特率:115200bps
	usart1_init(115200);	

	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	//定时器3初始化
	tim3_init();
	
	//温湿度模块初始化
	dht11_init();
	
	//若检测到由独立看门狗导致的复位
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
	{
		/* IWDGRST flag set */
		printf("iwdg reset cpu\r\n");
	}
	else
	{
		/* IWDGRST flag is not set */
		printf("normal reset cpu\r\n");
	}
	
	/* Clear reset flags */
	RCC_ClearFlag();	
	
	
	//独立看门狗的初始化
	iwdg_init();
	
	while(1)
	{
		rt = dht11_read(buf);
		
		if(rt == 0)
		{
			printf("温度:%d.%d,湿度:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
			if(buf[2] >= 29)
			{
				iwdg_flag=0;
			}
		}
		else
		{
			//printf("dht11 error code %d\r\n",rt);
		}
		
		delay_ms(4000);
	}
}

void TIM3_IRQHandler(void)
{
	//判断标志位
	if(SET==TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		if(iwdg_flag)
		{
			//每500ms喂一次狗，CPU不复位
			IWDG_ReloadCounter();//刷新计数值 == 喂狗，防止计数值减到0就触发复位
		}
		//清空标志位
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//接收数据
		d=USART_ReceiveData(USART1);
		
		
	
		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}




