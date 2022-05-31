#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;


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
#define SCL_W	PBout(8)
#define SDA_W	PBout(9)
#define SDA_R	PBin(9)


void at24c02_init(void)
{
	//PB硬件时钟使能	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	//配置PB8 PB9为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;		//第8 9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	//看时序图
	PBout(8)=1;
	
	//看时序图	
	PBout(9)=1;	
}

void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= pin_mode;	//输出/输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void i2c_start(void)
{
	//保证SDA引脚为输出模式
	sda_pin_mode(GPIO_Mode_OUT);
	
	SCL_W=1;
	SDA_W=1;
	delay_us(5);

	SDA_W=0;
	delay_us(5);
	
	
	SCL_W=0;
	delay_us(5);		//钳住I2C总线
}

void i2c_stop(void)
{
	//保证SDA引脚为输出模式
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=1;
	SDA_W=0;
	delay_us(5);

	SDA_W=1;
	delay_us(5);
}


void i2c_send_byte(uint8_t byte)
{
	int32_t i;
	
	//保证SDA引脚为输出模式
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=0;
	SDA_W=0;
	delay_us(5);

	for(i=7; i>=0; i--)
	{
		//先更改SDA引脚的电平
		if(byte & (1<<i))
		{
			SDA_W=1;
		
		}
		else
		{
			SDA_W=0;
		}
		
		delay_us(5);
		
		SCL_W=1;
		delay_us(5);
		
		
		SCL_W=0;
		delay_us(5);	
	}
}

void i2c_ack(uint8_t ack)
{

	
	//保证SDA引脚为输出模式
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=0;
	SDA_W=0;
	delay_us(5);


	//先更改SDA引脚的电平
	if(ack)
	{
		SDA_W=1;
	
	}
	else
	{
		SDA_W=0;
	}
	
	delay_us(5);
	
	SCL_W=1;
	delay_us(5);
	
	
	SCL_W=0;
	delay_us(5);	
}

uint8_t i2c_wait_ack(void)
{
	uint8_t ack=0;
	
	//保证SDA引脚为输IN模式
	sda_pin_mode(GPIO_Mode_IN);	
	SCL_W=1;
	delay_us(5);
	
	if(SDA_R)
		ack=1;	//无应答信号
	else
		ack=0;	//有应答信号

	SCL_W=0;	//总线在忙的状态
	delay_us(5);
	
	return ack;
}

uint8_t i2c_recv_byte(void)
{
	uint8_t d=0;
	
	int32_t i;
	
	//保证SDA引脚为输IN模式
	sda_pin_mode(GPIO_Mode_IN);	
	
	for(i=7; i>=0; i--)
	{
		SCL_W=1;
		delay_us(5);
		
		if(SDA_R)
			d|=1<<i;	
		

		SCL_W=0;	
		delay_us(5);	
	
	}

	
	return d;
}


int32_t at24c02_write(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack;
	uint8_t *p = pbuf;

	//发送起始信号，通信的开始
	i2c_start();
	
	//发送设备地址，就是寻址
	i2c_send_byte(0xA0);

	//等待应答信号
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address fail\r\n");
		
		return -1;
	}


	//发送要写入数据的地址
	i2c_send_byte(addr);

	//等待应答信号
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}
	
	
	while(len--)
	{
	
		//发送要写入数据
		i2c_send_byte(*p++);

		//等待应答信号
		ack = i2c_wait_ack();
		
		if(ack)
		{
			printf("data fail\r\n");
			
			return -3;
		}	
	}
	
	
	//发送停止信号
	i2c_stop();
	
	printf("write suceess\r\n");
	
	return 0;
}

int32_t at24c02_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack;
	uint8_t *p = pbuf;

	//发送起始信号，通信的开始
	i2c_start();
	
	//发送设备地址，就是寻址
	i2c_send_byte(0xA0);

	//等待应答信号
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address write access fail\r\n");
		
		return -1;
	}


	//发送要读取数据的地址
	i2c_send_byte(addr);

	//等待应答信号
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}
	
	//重新发送起始信号，新的通信开始
	i2c_start();
	
	//发送设备地址，就是寻址，当前是读访问
	i2c_send_byte(0xA1);

	//等待应答信号
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address read access fail\r\n");
		
		return -3;
	}	
	
	len = len-1;
	
	while(len--)
	{
	
		//接收数据
		*p=i2c_recv_byte();
		
		p++;

		//发送应答信号给从机，告诉从机已经接收自己成功
		i2c_ack(0);
	}
	
	//接收最后一个数据
	*p=i2c_recv_byte();	
	
	//发送无应答信号给从机，告诉从机不再接收数据
	i2c_ack(1);	
	
	//发送停止信号
	i2c_stop();
	
	printf("read suceess\r\n");
	
	return 0;
}

int main(void)
{
	int32_t i=0;
	uint8_t buf[8]={0};
	
	
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
	
	
	//at240c2初始化
	at24c02_init();
	
	
	//向0地址写入8个字节的数据
	memset(buf,0x88,8);
	
	at24c02_write(0,buf,8);
	
	delay_ms(100);
	
	memset(buf,0,8);
	at24c02_read(0,buf,8);
	
	for(i=0; i<8; i++)
	{
		printf("%02X ",buf[i]);
	
	}
	printf("\r\n");

	
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




