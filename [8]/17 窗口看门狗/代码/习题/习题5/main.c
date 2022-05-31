#include <stdio.h>
#include "stm32f4xx.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"


GPIO_InitTypeDef   GPIO_InitStructure;

EXTI_InitTypeDef   EXTI_InitStructure;

NVIC_InitTypeDef   NVIC_InitStructure;

USART_InitTypeDef USART_InitStructure;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


static volatile uint32_t g_iwdg_reset=0;

static volatile uint32_t g_wwdg_reset=0;

static volatile uint8_t  g_usart1_buf[128]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;


//重定义(重定向)fputc函数

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	
	
	return ch;
}


void delay_us(uint32_t nus)
{

	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统时钟后才能设置寄存器
	SysTick->LOAD = SystemCoreClock/8/1000000*nus; 		//设置计数值
	SysTick->VAL = 0; 			// Clear current value as well as count flag，清空当前值还有标志位
	SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock，使能系统定时器开始计算，且使用8分频的时钟
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set，等待计数完成
	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统时钟代表说不再进行定时计数	

}

void delay_ms(uint32_t nms)
{
	while(nms --)
	{
		SysTick->CTRL = 0; 			// Disable SysTick，关闭系统时钟后才能设置寄存器
		SysTick->LOAD = SystemCoreClock/8/1000; 		// 设置计数值
		SysTick->VAL = 0; 			// Clear current value as well as count flag，清空当前值还有标志位
		SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock，使能系统定时器开始计算，且使用8分频的时钟
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set，等待计数完成
		SysTick->CTRL = 0; 			// Disable SysTick，关闭系统时钟代表说不再进行定时计数	
	
	}

}
void tim3_init(void)
{
	/* TIM3 clock enable ，定时器3的时钟使能*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Enable the TIM3 gloabal Interrupt ，使能定时器3的全局中断*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



	/* Time base configuration，定时器的基本配置，用于配置定时器的中断频率为1000Hz，也就是说多长时间出发一次中断，当前1ms触发一次中断 */
	TIM_TimeBaseStructure.TIM_Period = (10000/1000)-1;					//设置定时的频率
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;						//第一次分频，简称为预分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//第二次分频,当前实现1分频，也就是不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* TIM Interrupts enable,使能定时器3更新中断事件，也代表说定时已经到达的事件 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter，使能定时器3工作 */
	TIM_Cmd(TIM3, ENABLE);	
}


void USART1_Init(uint32_t baud)
{

	/* Enable GPIO clock ，使能对应的GPIOA时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//使能串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* 串口1对应引脚复用 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//设置为复用模式，也就是说使能引脚的第二功能模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
				
	USART_InitStructure.USART_BaudRate = baud;						//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位为8个比特位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure);

	
	/* Enable the USART1 Interrupt ，使能串口1中断*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART ，使能串口1工作*/
	USART_Cmd(USART1, ENABLE);	
	
	
	/* 使能串口1接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


}

//发送多个字节数据
void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{
	while(len--)
	{
		USART_SendData(USART1,*pbuf++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	}
}


//发送字符串
void usart1_send_str(uint8_t *pbuf)
{
	while(pbuf && *pbuf)
	{
		USART_SendData(USART1,*pbuf++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	}
}


int main(void)
{

	
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


	USART1_Init(115200);
	
	
	printf("This is WDG Test\r\n");
	
	/* Check if the system has resumed from WWDG reset，检查当前系统是否由看门狗进行复位 */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{
		/* WWDGRST flag set */
		printf("WWDG RESET OK\r\n");
		

	}
	/* Check if the system has resumed from IWDG reset，检查当前系统是否由看门狗进行复位 */
	else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("IWDG RESET OK\r\n");
		

	}	
	else
	{
		printf("normal reset cpu\r\n");
	}

	delay_ms(500);
	/* Clear reset flags ，清空复位标志*/
	RCC_ClearFlag();

	/* WWDG configuration */
	/* Enable WWDG clock ，使能窗口看门狗的时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	
	/* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us) 
	   这个看门狗的计数值是比较特殊，计数值每减1运算，所花的时间为780us

		上限值最大值为127，下限值0x40=64，中间相隔64个计数值，
	*/
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	//设置窗口上限值 80
	WWDG_SetWindowValue(80);
	
	
	//使能看门狗，并刷新计数值==喂狗
	WWDG_Enable(127);


	//WWDG NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;							//窗口看门狗中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;					//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;						//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);											//根据指定的参数初始化VIC寄存器	
	
	
	//清空提前唤醒中断标志位
	WWDG_ClearFlag();
	
	//使能提前唤醒中断
	WWDG_EnableIT();
	
	/* Enable write access to IWDG_PR and IWDG_RLR registers，独立看门狗的寄存器都是受保护的，现在进行解锁动作 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: LSI/256 ，设置看门狗的时钟频率 = 32KHz/256=125Hz*/
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	//设置看门狗的喂狗时间，也就是设置它的计数值
	//当前写125-1，那么喂狗的超时时间为1秒，如果没有在1秒内进行喂狗（刷新计数值），就会复位CPU
	//当前写250-1，那么喂狗的超时时间为2秒，如果没有在2秒内进行喂狗（刷新计数值），就会复位CPU
	//......
	IWDG_SetReload(125-1);
	
	/* Reload IWDG counter ，重载计数值，刷新计数值==喂狗*/
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) ，让看门狗工作*/
	IWDG_Enable();	
	
	//定时器3初始化，每1ms触发一次中断
	tim3_init();


	while(1)
	{
		if(g_usart1_event)
		{
			//判断是否接收到IWDG RESET
			if(strstr((const char *)g_usart1_buf,"IWDG RESET"))
			{
				g_iwdg_reset=1;
			
			}
			
			//判断是否接收到WWDG RESET
			if(strstr((const char *)g_usart1_buf,"WWDG RESET"))
			{
				g_wwdg_reset=1;
			
			}	


			g_usart1_event=0;
			g_usart1_cnt=0;
			memset((uint8_t *)g_usart1_buf,0,sizeof g_usart1_buf);
		
		}
	}

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	/* USART in Receiver mode ,检查是否接收到串口数据*/
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{

		/* Receive Transaction data，接收数据 */
		g_usart1_buf[g_usart1_cnt] = (uint8_t)USART_ReceiveData(USART1);	
		
		g_usart1_cnt++;
		
		
		//检测当前是否接收完成，标志为换行符，还得检查当前数组接收是否溢出
		if(g_usart1_buf[g_usart1_cnt-1] == '#' || g_usart1_cnt>= sizeof(g_usart1_buf))
		{
			g_usart1_event=1;
		
		}
	}



}

void TIM3_IRQHandler(void)
{
	static int tim3_count=0;
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		tim3_count++;
		
		if(tim3_count>=500)
		{
			if(g_iwdg_reset == 0)
			{
				//刷新计数值，让计数值非0==喂狗
				IWDG_ReloadCounter();
			
				tim3_count=0;			
			}
			else
			{
				//一直占用CPU，让看门狗超时复位
				while(1);
			}

		
		}
		//清空标志位，告诉CPU，已经完成定时器3的更新中断，可以触发新的一次中断了
	
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}


}

void WWDG_IRQHandler(void)
{
	//能够进入该中断服务函数，也就是说当前窗口看门狗的计数值==0x40
	if(WWDG_GetFlagStatus() == SET)
	{
		if(g_wwdg_reset == 0)
		{
			/* Update WWDG counter ，刷新计数值==喂狗*/
			WWDG_SetCounter(127);
	
			//清空提前唤醒标志位
			WWDG_ClearFlag();
		}
		else
		{
			while(1);
		}

	}
}

