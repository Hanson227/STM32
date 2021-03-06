#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;
static SPI_InitTypeDef  		SPI_InitStructure;

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

void w25qxx_init(void)
{
	//PB硬件时钟使能	
	
	
	//SPI1硬件时钟使能
	

	
	//配置PB3~PB5为复用功能模式
	
	
	//PB3~PB5连接到SPI1硬件
	
	
	//PB14配置输出模式
	
	
	//看时序图，PB14会有一个初始电平,高电平
	
	
	//配置SPI1的参数
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					 	//主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				//看数据手册，8位数据位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						//看数据手册，SPI FLASH可以配置高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					//看数据手册，MISO引脚在时钟线第二边沿采样数据
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						//片选引脚由代码控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//看数据手册，SPI1的硬件时钟源为APB2，SPI1_CLK=84MHz/8=10.5MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				//看数据手册，最高有效位优先传输
	
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/*使能SPI1硬件工作  */
	SPI_Cmd(sFLASH_SPI, ENABLE);
}


uint8_t spi1_send_byte(uint8_t byte)
{
	//检查SPI1的发送缓冲区是否为空
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	
	//通过SPI1硬件发送数据
	SPI_I2S_SendData(SPI1, byte);
	
	//等待字节接收
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	//返回接收到的数据
	return SPI_I2S_ReceiveData(SPI1);
}


void w25qxx_read_id(uint8_t *m_id,uint8_t *d_id)
{
	//片选引脚输出低电平
	PBout(14)=0;

	//发送90h命令
	spi1_send_byte(0x90);

	//发送24bit地址，该数值全为0
	spi1_send_byte(0x00);
	spi1_send_byte(0x00);
	spi1_send_byte(0x00);	
	
	//传递任意参数，读取厂商id
	*m_id=spi1_send_byte(0xFF);
	
	//传递任意参数，读取设备id
	*d_id=spi1_send_byte(0xFF);	
	
	//片选引脚输出高电平
	PBout(14)=1;	
}


int main(void)
{
	int32_t d=0;
	uint8_t m_id,d_id;
	
	
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
	
	//初始化SPI1连接的w25qxx
	w25qxx_init()
	w25qxx_read_id(&m_id,&d_id);
	
	
	printf("m_id=%x,d_id=%s\r\n",m_id,d_id);
	
	while(1)
	{
		
		
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




