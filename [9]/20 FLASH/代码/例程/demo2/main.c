#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


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
	SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳ��ʱ��
	SysTick->LOAD = (168*n)-1; // ���ü���ֵ(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick���ر�ϵͳ��ʱ��
		SysTick->LOAD = (168000)-1; 	// ���ü���ֵ(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

}

void usart1_init(uint32_t baud)
{
	
	//��PAӲ��ʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//�򿪴���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//����PA9��PA10Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//�๦��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//��PA9��PA10�������ӵ�����1��Ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//���ô���1��ز����������ʡ���У��λ��8λ����λ��1��ֹͣλ......
	USART_InitStructure.USART_BaudRate = baud;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����շ�����
	USART_Init(USART1, &USART_InitStructure);
	
	
	//���ô���1���жϴ�������������һ���ֽڴ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//���ô���1���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//ʹ�ܴ���1����
	USART_Cmd(USART1, ENABLE);
}



int main(void)
{
	int32_t d=0;
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//����1������:115200bps
	usart1_init(115200);	

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	
	
	printf("This is flash test\r\n");
	
	
	//���д����
	FLASH_Unlock();
	
	
	//��һ�еı�־����
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	
	//��������4��������С��Ԫ�ٶ�Ϊ4���ֽ�
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3))
	{
		printf("FLASH_EraseSector sector 4 fail\r\n");
		
		while(1);
	
	}
	
	//������4�׵�ַд��һ��32λ������
	if(FLASH_COMPLETE!=FLASH_ProgramWord(0x8010000, 0x1688))
	{
		printf("FLASH_ProgramWord at addr 0x8010000 fail\r\n");
		while(1);
	}
	
	if(FLASH_COMPLETE!=FLASH_ProgramWord(0x8010000, 0x1111))
	{
		printf("FLASH_ProgramWord at addr 0x8010000 fail\r\n");
		while(1);
	}
		
	
	//���д����
	FLASH_Lock();
	
	//��ȡ����4���׵�ַ�����ݣ���ȡ4���ֽھ���32bit������
	d = *(volatile uint32_t*)0x8010000;
	
	printf("read addr at 0x8010000 data is 0x%X\r\n",d);
	
	
	
	while(1)
	{
		
		
	}
}



void USART1_IRQHandler(void)
{
	uint8_t d;
	
	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//��������
		d=USART_ReceiveData(USART1);
		
		
	
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}




