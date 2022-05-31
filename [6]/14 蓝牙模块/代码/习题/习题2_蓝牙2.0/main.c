#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


static GPIO_InitTypeDef		GPIO_InitStructure;
static USART_InitTypeDef	USART_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;


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

void led1_init()		//led1灯初始化配置
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
}	

void led2_init()		//led2灯初始化配置
{
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
}	

void led3_init()		//led3灯初始化配置
{
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOE,&GPIO_InitStructure);
}	

void led4_init()		//led4灯初始化配置
{
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOE,&GPIO_InitStructure);
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



void usart3_init(uint32_t baud)
{
	//使能串口3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	
	//使能端口B的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//配置PB10 PB11为复用功能模式
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;	//第10 11个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//复用功能模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PB10和PB11连接到串口1硬件
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);		

	//配置串口3：波特率，校验位、数据位、停止位
	USART_InitStructure.USART_BaudRate = baud;						//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//取消流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//能够收发数据
	USART_Init(USART3,&USART_InitStructure);
	
	
	//配置串口3的中断触发方式：接收完字节触发中断
	USART_ITConfig(USART3,USART_IT_RXNE , ENABLE);
	
	
	//使能串口3工作	
	USART_Cmd(USART3, ENABLE);
	
	
	//配置NVIC管理串口3
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//串口1的中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//响应优先级 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVIC使能串口1中断请求通道
	NVIC_Init(&NVIC_InitStructure);	
}


void usart3_send_str(char *str)
{
	char *p = str;
	
	while(p && (*p!='\0'))
	{
	
		USART_SendData(USART3,*p);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);	
		p++;
	}
}


void bluetooth_config(void)
{
	//使能端口E的硬件时钟，端口E才能工作，说白了就是对端口E上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	


	
	//配置硬件，配置GPIO，端口E，第6个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//第6 个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	
    //PE6引脚输出高电平，即EN引脚为高电平，进入AT指令模式
	PEout(6)=1;
	delay_ms(500);

	delay_ms(500);
	
	usart3_send_str("AT\r\n");
	delay_ms(500);	
	
	usart3_send_str("AT\r\n");
	delay_ms(500);	
	
	usart3_send_str("AT+NAME=TWon\r\n");
	delay_ms(500);
	
	
	//复位模块
	usart3_send_str("AT+RESET\r\n");
	delay_ms(500);
	
	
	//PE6引脚输出低电平，即EN引脚为低电平，退出AT指令模式
	PEout(6)=0;
	
	delay_ms(1000);	

}



//主函数
int main(void)
{
	int32_t d;

	
	//使能端口F的硬件时钟，端口F才能工作，说白了就是对端口F上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	
	
	
	led1_init();
	led2_init();
	led3_init();
	led4_init();

	PFout(9)=1;
	PFout(10)=1;
	PEout(13)=1;
	PEout(14)=1;


	
	//配置硬件，配置GPIO，端口F，第9个引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//第9 个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	
	//串口1波特率为115200bps
	usart1_init(115200);
	
	printf("This is bluetooth test\r\n");
	
	//串口3波特率为38400bps，有部分蓝牙2.0模块是9600bps
	usart3_init(9600);	
	
	//蓝牙模块配置
	bluetooth_config();
	
	while(1)
	{

		
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
		
		//清空标志位	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}


void  USART3_IRQHandler(void)
{
	uint8_t d;

	//检测标志位
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		
		//接收数据
		d = USART_ReceiveData(USART3);
		
		
		//发送到串口1->PC
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		
		if(d == '1')
		{
			PFout(9)=0;//打开LED1
		}
		if(d == 'a')
		{
			PFout(9)=1;//熄灭LED1
		}
		if(d == '2')
		{
			PFout(10)=0;//打开LED2
		}
		if(d == 'b')
		{
			PFout(10)=1;//熄灭LED2
		}
		if(d == '3')
		{
			PEout(13)=0;//打开LED3
		}
		if(d == 'c')
		{
			PEout(13)=1;//熄灭LED3
		}
		if(d == '4')
		{
			PEout(14)=0;//打开LED4
		}
		if(d == 'd')
		{
			PEout(14)=1;//熄灭LED4
		}
		
		//清空标志位	
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}

}
