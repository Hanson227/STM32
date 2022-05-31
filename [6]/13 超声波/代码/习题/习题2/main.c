#include <stdio.h>
#include "stm32f4xx.h"
#include "sys.h"

GPIO_InitTypeDef   GPIO_InitStructure;

EXTI_InitTypeDef   EXTI_InitStructure;

NVIC_InitTypeDef   NVIC_InitStructure;


void delay_us(uint32_t t)
{

	SysTick->CTRL = 0;			 			// Disable SysTick
	SysTick->LOAD = SystemCoreClock/1000000*t; // 1us*t
	SysTick->VAL = 0; 						// Clear current value as well as count flag
	SysTick->CTRL = 5; 						// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 						// Disable SysTick



}

void delay_ms(uint32_t t)
{
	while(t--)
	{
		SysTick->CTRL = 0;			 			// Disable SysTick
		SysTick->LOAD = SystemCoreClock/1000; 	// 1ms
		SysTick->VAL = 0; 						// Clear current value as well as count flag
		SysTick->CTRL = 5; 						// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; 						// Disable SysTick
	
	}
}

void usart1_init(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;

	/* 使能端口A的时钟，让端口A工作 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


	/* 串口1时钟使能 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


	/* 将PA9 和 PA10引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);


	/* 配置PA9和PA10引脚为复用（第二功能、多功能）功能模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//配置串口的属性
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//数据位为8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//停止位1个
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//支持发送/接收
	USART_Init(USART1, &USART_InitStructure);


	/* 配置串口1的优先级 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;						//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能串口1工作 */
	USART_Cmd(USART1, ENABLE);
	
	/* 使能串口1的接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	

}


void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{
	while(len--)
	{
		//发送数据
		USART_SendData(USART1,*pbuf++);
		
		//检查是否发送完成
		while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	
	}
}

void usart1_send_str(uint8_t *pbuf)
{
	while(*pbuf!='\0')
	{
		//发送数据
		USART_SendData(USART1,*pbuf++);
		
		//检查是否发送完成
		while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	
	}
}

//重定向fput这个函数，printf函数会调用到
int fputc(int ch,FILE *f)
{
	//发送数据
	USART_SendData(USART1,ch);
	
	//检查是否发送完成
	while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));	
	
	return ch;

}

void sr04_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);

	/* 配置PB6引脚为输出模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//第6根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* 配置PE6引脚为输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//第6根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;				//设置输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	PBout(6)=0;
}


int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	PBout(6)=1;
	delay_us(20);
	PBout(6)=0;
	
	//等待回响信号变为高电平
	while(PEin(6)==0)
	{
		t++;
		delay_us(1);
		
		//如果超时，就返回一个错误码
		if(t>=1000000)
			return -1;
	}
	
	t=0;
	//测量高电平持续的时间
	while(PEin(6))
	{
	
		//延时9us,就是3mm的传输距离
		delay_us(9);
		
		t++;
		
		
		//如果超时，就返回一个错误码
		if(t>=1000000)
			return -2;	
	}
	
	//当前的传输距离
	return 3*(t/2);



}

uint32_t tim13_cnt=0;

void tim13_init(void)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* 定时器13的时钟使能*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	

	/*定时器的基本配置，用于配置定时器的输出脉冲的频率为100Hz */
	TIM_TimeBaseStructure.TIM_Period = (40000/100)-1;					//设置定时脉冲的频率
	TIM_TimeBaseStructure.TIM_Prescaler = 2100-1;						//第一次分频，简称为预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//第二次分频,当前实现1分频，也就是不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	tim13_cnt=TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);

	/* 配置PF8 引脚为复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;					//第8根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//设置复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	
	/* 让定时器14 PWM 的通道 1 工作在模式 1*/
	 //PWM 模式 1， 在递增模式下， 只要TIMx_CNT < TIMx_CCR1， 通道 1 便为有效状态（高电平）， 否则为无效状态（低电平）。
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //允许输出
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//有效的时候， 输出高电平
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);
	
	/*  使能定时器 13 工作 */
	TIM_Cmd(TIM13, ENABLE);
}

//设置定时器13的PWM频率
void tim13_set_freq(uint32_t freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/*定时器的基本配置，用于配置定时器的输出脉冲的频率为freq Hz */
	TIM_TimeBaseStructure.TIM_Period = (40000/freq)-1;					//设置定时脉冲的频率
	TIM_TimeBaseStructure.TIM_Prescaler = 2100-1;						//第一次分频，简称为预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//第二次分频,当前实现1分频，也就是不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	tim13_cnt= TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);

}

//设置定时器13的PWM占空比0%~100%
void tim13_set_duty(uint32_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_cnt+1) * duty/100;

	TIM_SetCompare1(TIM13,cmp);
}


void beep_init(void)
{
	tim13_init();
	
	
	//蜂鸣器禁鸣
	tim13_set_duty(0);
}


void key_init(void)
{
	/* 使能端口A的时钟，让端口A工作 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	/* 使能端口E的时钟，让端口E工作 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	
	/* 配置PA0引脚为输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//第0号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//输入模式
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增加驱动能力
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的工作最大速度：100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有上下拉电阻
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 配置PE2 PE3 PE4引脚为输入模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;				//第2 3 4号引脚
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


int main(void)
{
	uint32_t distance=0;
	uint32_t duty=10;
	uint32_t distance_safe=1;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);

	/* Configure PF9 PF10 in output pushpull mode，配置PF9 PF10引脚为输出模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9根和10根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	
	/* Configure PE13 PE14 in output pushpull mode，配置PE13 PE14引脚为输出模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		//第13根和14根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//设置输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽模式，增加驱动电流
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//设置IO的速度为100MHz，频率越高性能越好，频率越低，功耗越低
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//不需要上拉电阻
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	


	//配置中断优先级的分组
	//配置支持4个抢占优先级，支持4个响应优先级
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	

	PFout(9)=1;
	PFout(10)=1;	
	PEout(13)=1;
	PEout(14)=1;
	
	//串口1初始化，波特率115200bps
	usart1_init(115200);

	//超声波模块初始化
	sr04_init();
	
	//添加蜂鸣器初始化
	beep_init();
	
	
	//按键的初始化
	key_init();
	
	while(1)
	{
		distance=sr04_get_distance();
		
		if(distance < 0)
		{
			//模块有可能没有连接好，连接错引脚或杜邦线松动
			printf("sr04 is error\r\n");
		}
		else if(distance>=20 && distance<=4000)
		{
			//数据是正确的
			printf("distance=%dmm\r\n",distance);		
		
			if(distance>=450)
			{
				PFout(9)=1;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;	
				
				distance_safe=1;
			}
			else if(distance>=350 && distance<450)
			{
				PFout(9)=0;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;	

				
				distance_safe=1;
			}	
			else if(distance>=250 && distance<350)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=1;
				PEout(14)=1;	
				
				distance_safe=0;
				
				tim13_set_freq(2);

			}				
			else if(distance>=150 && distance<250)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=1;	
				
				distance_safe=0;
				
				tim13_set_freq(4);

			}	

			else if(distance<150)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=0;	
				
				tim13_set_freq(10);
				
				distance_safe=0;
			}	

			//若是安全距离，则蜂鸣器禁鸣
			if(distance_safe)
				tim13_set_duty(0);
			//若不是安全距离，则蜂鸣器根据对应的占空比进行鸣响
			else
				tim13_set_duty(duty);
				
		}
		else
		{
			//数据是异常的
			printf("sr04 get distance is error\r\n");			
		
		}
		
		//添加按键检测代码
		if(PAin(0)==0)
		{
			//占空比减5，声音变小
			if(duty>0)
			{
				duty-=5;
				
				tim13_set_duty(duty);
			
			}
				
		
		}
		
		if(PEin(2)==0)
		{
			//占空比加5，声音变大
			if(duty<30)
			{
				duty+=5;
				
				tim13_set_duty(duty);
			
			}		
		}
		
		//延时500ms
		delay_ms(500);
	}

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	//检查是否有串口接收中断产生
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		//接收数据
		d = USART_ReceiveData(USART1);
		
		
		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

