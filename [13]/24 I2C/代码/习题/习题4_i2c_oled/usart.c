#include "stm32f4xx.h"
#include "sys.h"
#include "stdio.h"

//GPIO初始化的结构体
static GPIO_InitTypeDef  GPIO_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;

void usart1_init(uint32_t baud)
{
	//GPIOA硬件时钟使能，就是让GPIOA进行工作
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//使能串口1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	/* 将PA9 PA10引脚连接到串口1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	
	//使能PA9和PA10引脚的多功能模式
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9                ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10                ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baud;										//波特率，串口通信的速度
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//发送和接收数据的长度为8个bit，也就是一个字节
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	
	/*使能串口1接收中断*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

}


void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{

	while(len--)
	{
		//发送数据
		USART_SendData(USART1,*pbuf++);
		
		//检查数据是否发送完毕
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	
	}
}


int fputc(int ch,FILE *f)
{
	//发送数据
	USART_SendData(USART1,ch);
	
	//检查数据是否发送完毕
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	return ch;

}


void USART1_IRQHandler(void)
{
	uint8_t d=0;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		//获取接收到的数据
		d =USART_ReceiveData(USART1);
		
		//返发接收到的数据
		usart1_send_bytes(&d,1);
	}
}

