#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static GPIO_InitTypeDef		GPIO_InitStructure;
static USART_InitTypeDef	USART_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  TIM_OCInitStructure;

static volatile uint8_t  g_usart1_buf[32]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;
static volatile uint32_t g_iwdg_rest=0;

void delay_us(uint32_t nus)
{
	uint32_t t = nus;

	SysTick->CTRL = 0; 					// Disable SysTick
	SysTick->LOAD = (nus*21)-1; 		// Count from (nus*21)-1 to 0 ((nus*21) cycles)
	SysTick->VAL = 0; 					// Clear current value as well as count flag
	SysTick->CTRL = 1; 					// Enable SysTick timer with processor clock/8
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 					// Disable SysTick		
	

}


void delay_ms(uint32_t nms)
{
	uint32_t t = nms;


	while(t--)
	{
		SysTick->CTRL = 0; 			// Disable SysTick
		SysTick->LOAD = 21000-1; 	// Count from 20999 to 0 (21000 cycles)
		SysTick->VAL = 0; 			// Clear current value as well as count flag
		SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}	
	
	
	SysTick->CTRL = 0; // Disable SysTick	
}

void usart1_init(uint32_t baud)
{
	//使能串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//使能端口A的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//配置PA9 PA10为复用功能模式
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//第9 10个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//复用功能模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA9和PA10连接到串口1硬件
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		

	//配置串口1：波特率，校验位、数据位、停止位
	USART_InitStructure.USART_BaudRate = baud;						//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//取消流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//能够收发数据
	USART_Init(USART1,&USART_InitStructure);
	
	
	//配置串口1的中断触发方式：接收完字节触发中断
	USART_ITConfig(USART1,USART_IT_RXNE , ENABLE);
	
	
	//使能串口1工作	
	USART_Cmd(USART1, ENABLE);
	
	
	//配置NVIC管理串口1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//串口1的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVIC使能串口1中断请求通道
	NVIC_Init(&NVIC_InitStructure);	
}

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{

	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	
	return(ch);
}

void tim3_init(void)
{

	//使能定时器3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//配置定时器3参数：定时时间
	//定时器3的硬件时钟=84MHz/8400=10000Hz
	//只要定时器3进行10000次计数，就是1秒时间的到达
	//只要定时器3进行10次计数，就是1毫秒时间的到达
	TIM_TimeBaseStructure.TIM_Period = 10000-1;					//计数值
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//预分频值，也可以理解为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//F407是没有时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		
	
	//配置定时器3的中断

	TIM_ITConfig(TIM3,TIM_IT_Update , ENABLE);
	
	//启动定时器3
	TIM_Cmd(TIM3, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		//定时器3的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVIC使能定时器3中断请求通道
	NVIC_Init(&NVIC_InitStructure);	
  

}
/*
void tim3_pwm_init(void)
{

	//使能端口A的硬件时钟，端口A才能工作，说白了就是对端口A上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	

	//使能定时器3的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	
	//配置硬件，配置GPIO，端口A，第6,7个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//第6个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//复用功能模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//将PA6引脚配置为定时器3的PWM功能
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);	
	

	
	
	//配置定时器3参数：输出频率100Hz
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;			//计数值
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//预分频值，也可以理解为第一次分频
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//F407是没有时钟分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	//占空比
	//配置通道1工作在PWM1模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//工作在模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//输出开关
	TIM_OCInitStructure.TIM_Pulse = 100;								//比较值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//有效状态：高电平输出；无效状态：低电平输出
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	
	//启动定时器3
	TIM_Cmd(TIM3, ENABLE);
}
*/
void dht11_init(void)
{
	//使能端口G的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	

	
	//配置PG9为输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//第9个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOG,&GPIO_InitStructure);	


	//PG9初始电平为高电平
	PGout(9)=1;
}


void dht11_pin_mode(GPIOMode_TypeDef gpio_mode)
{
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//第9个引脚
	GPIO_InitStructure.GPIO_Mode=gpio_mode;			//输出/输入模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOG,&GPIO_InitStructure);	

}



int32_t dht11_read(uint8_t *pdht_data)
{
	uint32_t t=0;
	
	uint8_t d;
	
	int32_t i=0;
	int32_t j=0;
	uint32_t check_sum=0;
	
	//保证引脚为输出模式
	dht11_pin_mode(GPIO_Mode_OUT);	
	
	PGout(9)=0;
	
	delay_ms(20);
	
	PGout(9)=1;	
	
	delay_us(30);
	
	//保证引脚为输入模式
	dht11_pin_mode(GPIO_Mode_IN);

	
	//等待DHT11响应，等待低电平出现
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 4000)
			return -1;
	}
	
	//若低电平超出100us
	t=0;
	while(PGin(9)==0)
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -2;
	}
	
	
	//若高电平超出100us
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -3;
	}
	
	//连续接收5个字节
	for(j=0; j<5; j++)
	{
		d = 0;
		//完成8个bit数据的接收，高位优先
		for(i=7; i>=0; i--)
		{
			//等待低电平持续完毕
			t=0;
			while(PGin(9)==0)
			{
			
				t++;
				
				delay_us(1);
				
				if(t >= 100)
					return -4;
			}	
			
			
			delay_us(40);
			
			if(PGin(9))
			{
				d|=1<<i;
				
				//等待数据1的高电平时间持续完毕
				t=0;
				while(PGin(9))
				{
				
					t++;
					
					delay_us(1);
					
					if(t >= 100)
						return -5;
				}			
			
			}
		}	
	
		pdht_data[j] = d;
	}
	
	
	//通信的结束
	delay_us(100);
	
	//计算校验和
	check_sum=pdht_data[0]+pdht_data[1]+pdht_data[2]+pdht_data[3];
	
	
	
	check_sum = check_sum & 0xFF;
	
	if(check_sum != pdht_data[4])
		return -6;
	
	return 0;
}


void iwdg_init(void)
{
	//解除看门狗的写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);


	//设置分频值，才能得到独立看门狗的硬件时钟
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	
	//配置独立看门狗的计数值，配置为超时时间为2S，2秒后会复位芯片
	IWDG_SetReload(125*2-1);
	
	
	//刷新计数值
	IWDG_ReloadCounter();
	
	//使能独立看门狗工作
	IWDG_Enable();
}

//主函数
int main(void)
{
	int32_t rt; 
	
	uint8_t buf[5];

	uint8_t duty;
	
	char *p = NULL;
	
	char buffer[64]={0};
	//使能端口F的硬件时钟，端口F才能工作，说白了就是对端口F上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	


	
	//配置硬件，配置GPIO，端口F，第9个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//第9 个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	PAout(6)=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
	//串口1波特率为115200bps
	usart1_init(115200);
		//判断复位原因
   if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
   {
   	/* IWDGRST flag set */
   	printf("iwdg reset cpu\r\n");
   
   	/* Clear reset flags */
   	RCC_ClearFlag();
   }
   else
   {
   	/* IWDGRST flag is not set */
   	printf("normal reset cpu\r\n");
   }
	tim3_init();
	
	//温湿度模块初始化
	dht11_init();
	
	
	
	iwdg_init();
	printf("我复位了\n");

	while(1)
	{
	
		//获取温湿度
	
		rt = dht11_read(buf);
		
		if(rt < 0)
		{
			printf("dht11 read fail,error code = %d\r\n",rt);
		
		}
		else
		{
			printf("T:%d.%d,H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
		
		}
		if(buf[2] > 20)
		{
			printf("danger!!!\n");
			g_iwdg_rest = 1;
		}
		
		
		delay_ms(6000);
		
		
	}
}

void  USART1_IRQHandler(void)
{
	uint8_t d;

	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		
		//接收数据
		d = USART_ReceiveData(USART1);
		g_usart1_buf[g_usart1_cnt] = d;
		g_usart1_cnt++;
		if(d == '#' || g_usart1_cnt >= sizeof(g_usart1_buf))
		{
			g_usart1_event = 1;
		}
		
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
		
		//清空标志位	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
			
		if(g_iwdg_rest == 0)
		{
			IWDG_ReloadCounter();
		}
		else
		{
			while(1);
		}
    }
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
