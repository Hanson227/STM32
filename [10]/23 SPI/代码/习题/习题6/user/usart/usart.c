#include "usart.h"
#include "sys.h"
#include <stdio.h>


int fputc(int ch,FILE *file)
{
	USART_SendData(USART1,ch);
	//等待发送完毕
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	return ch;
}

void usart1_init(uint32_t baud)
{

	//端口A硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//串口1硬件时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	


	//配置硬件，配置GPIO，端口A，第9 10引脚
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//第9 10引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//复用功能模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9引脚连接到串口1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);

	//将PA10引脚连接到串口1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	USART_InitStructure.USART_BaudRate = baud;					//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不需要流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//允许串口接收和发送数据
	USART_Init(USART1, &USART_InitStructure);

	//接收数据后触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* 配置串口1 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	USART_Cmd(USART1, ENABLE);
}

void usart3_init(uint32_t baud)
{

	//打开串口3的硬件时钟，就是供电
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	//打开端口B的硬件时钟，就是供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//配置引脚
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10|GPIO_Pin_11; 		//第10 11根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;					//复用功能模式，将引脚交给其他硬件进行自动管理
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;				//增加输出电流
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;			//引脚的响应速度为100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;				//不添加上下拉电阻
	
	//初始化GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//将PB10和PB11连接到USART1硬件
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	

	//初始化串口
	USART_InitStructure.USART_BaudRate = baud;						//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //允许接收和发送
	
	USART_Init(USART3,&USART_InitStructure);
	
	
	//配置串口接收中断，也就是接收到字节就触发中断，通知CPU紧急处理
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	//中断优先级的配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	//使能串口工作
	USART_Cmd(USART3, ENABLE);


}


void usart3_send_str(const char *pstr)
{
	const char *p = pstr;
	
	//检测当前p的指针有效性
	//*p检测是否为非0
	while(p && *p)
	{
	
		//发送一个字节
		USART_SendData(USART3,*p);
	
		//等待发送完成
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);	
		
		//地址偏移1个字节
		p++;
		
	
	}
}

