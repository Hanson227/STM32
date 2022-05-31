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

void w25qxx_init(void)
{
	//PBӲ��ʱ��ʹ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	//����PB3 PB5 PB14Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_14;		//��3 4 5������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	//��ʱ��ͼ��������ģʽ3��ʱ��������PB3����ʼ��ƽΪ�ߵ�ƽ
	PBout(3)=1;
	
	//��ʱ��ͼ��Ƭѡ����PB14����ʼ��ƽΪ�ߵ�ƽ	
	PBout(14)=1;	
	
	
	//PB4��������ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;		//��4������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOB,&GPIO_InitStructure);		
	

	
}


uint8_t spi1_send_byte(uint8_t byte)
{
	int32_t i=0;
	uint8_t d=0;
	
	
	for(i=7; i>=0; i--)
	{
		//��byteÿ��bit�����ж�
		if(byte & (1<<i))
		{
			//MOSI��������ߵ�ƽ
			PBout(5)=1;
		}
		else
		{
		
			//MOSI��������͵�ƽ
			PBout(5)=0;		
		
		}
		
		//ʱ��������͵�ƽ
		PBout(3)=0;
		
		//��ʱһ�ᣬMOSI�����Ѿ����͵��Է�
		delay_us(1);
		
	
		//ʱ��������ߵ�ƽ
		PBout(3)=1;
		
		//��ʱһ��
		delay_us(1);	
	
		//��ȡMISO���ŵ�ƽ
		if(PBin(4))
			d|=1<<i;
	
	}
	
	
	return d;
	
	
}


void w25qxx_read_id(uint8_t *m_id,uint8_t *d_id)
{
	//Ƭѡ��������͵�ƽ
	PBout(14)=0;

	//����90h����
	spi1_send_byte(0x90);

	//����24bit��ַ������ֵȫΪ0
	spi1_send_byte(0x00);
	spi1_send_byte(0x00);
	spi1_send_byte(0x00);	
	
	//���������������ȡ����id
	*m_id=spi1_send_byte(0xFF);
	
	//���������������ȡ�豸id
	*d_id=spi1_send_byte(0xFF);	
	
	//Ƭѡ��������ߵ�ƽ
	PBout(14)=1;	
}


int main(void)
{
	int32_t d=0;
	uint8_t m_id,d_id;
	
	
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
	
	//��ʼ��SPI1���ӵ�w25qxx
	w25qxx_init();
	w25qxx_read_id(&m_id,&d_id);
	
	
	printf("m_id=%x,d_id=%x\r\n",m_id,d_id);
	
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




