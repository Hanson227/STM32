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
	//ʹ�ܴ���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//ʹ�ܶ˿�A��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//����PA9 PA10Ϊ���ù���ģʽ
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//��9 10������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//��PA9��PA10���ӵ�����1Ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		

	//���ô���1�������ʣ�У��λ������λ��ֹͣλ
	USART_InitStructure.USART_BaudRate = baud;						//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ȡ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�ܹ��շ�����
	USART_Init(USART1,&USART_InitStructure);
	
	
	//���ô���1���жϴ�����ʽ���������ֽڴ����ж�
	USART_ITConfig(USART1,USART_IT_RXNE , ENABLE);
	
	
	//ʹ�ܴ���1����	
	USART_Cmd(USART1, ENABLE);
	
	
	//����NVIC������1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//����1���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVICʹ�ܴ���1�ж�����ͨ��
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




//������
int main(void)
{
	uint32_t d;
	uint32_t i=0;


	
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	


	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	
	//����1������Ϊ115200bps
	usart1_init(115200);
	
	printf("This is flash test\r\n");
	
	//���Խ�������ĵ���stm32f4xx_dsp_stdperiph_lib_um.chm��
	
	//���д����������FLASH
	FLASH_Unlock();
	
	//��������4
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

	
	//���д����������FLASH
	FLASH_Lock();
	
	for(i=0; i<64; i++)
	{
	
		//��ȡFLASH���ݣ���֤д��������Ƿ���ȷ
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

	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		
		//��������
		d = USART_ReceiveData(USART1);
		
		//��ձ�־λ	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}
