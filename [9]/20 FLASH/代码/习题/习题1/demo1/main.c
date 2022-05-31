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




//主函数
int main(void)
{
	uint32_t d;
	uint32_t i=0;


	
	//使能端口F的硬件时钟，端口F才能工作，说白了就是对端口F上电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	


	
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
	
	printf("This is flash test\r\n");
	
	//尝试借鉴帮助文档《stm32f4xx_dsp_stdperiph_lib_um.chm》
	
	//解除写保护，解锁FLASH
	FLASH_Unlock();
	
	//擦除扇区4
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3))
	{
		printf("FLASH_EraseSector fail\r\n");
		while(1);
		
	
	}
	
	
	for(i=0; i<64; i++)
	{
		
		if(FLASH_COMPLETE!=FLASH_ProgramWord(0x08010000+i*4,i+1))
		{
			printf("FLASH_ProgramWord fail\r\n");
			while(1);	
		
		}	
	
	}

	
	//添加写保护，锁定FLASH
	FLASH_Lock();
	
	for(i=0; i<64; i++)
	{
	
		//读取FLASH数据，验证写入的数据是否正确
		d = *(volatile uint32_t *)(0x08010000+i*4);
		printf("read flash addr at 0x%08X,data is 0x%x\r\n",(0x08010000+i*4),d);
	}

	
	
	
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
