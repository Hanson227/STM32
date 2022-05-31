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
	
	
	//SPI1Ӳ��ʱ��ʹ��
	

	
	//����PB3~PB5Ϊ���ù���ģʽ
	
	
	//PB3~PB5���ӵ�SPI1Ӳ��
	
	
	//PB14�������ģʽ
	
	
	//��ʱ��ͼ��PB14����һ����ʼ��ƽ,�ߵ�ƽ
	
	
	//����SPI1�Ĳ���
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					 	//����ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;				//�������ֲᣬ8λ����λ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						//�������ֲᣬSPI FLASH�������øߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					//�������ֲᣬMISO������ʱ���ߵڶ����ز�������
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						//Ƭѡ�����ɴ������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//�������ֲᣬSPI1��Ӳ��ʱ��ԴΪAPB2��SPI1_CLK=84MHz/8=10.5MHz
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				//�������ֲᣬ�����Чλ���ȴ���
	
	SPI_Init(SPI1, &SPI_InitStructure);
	
	/*ʹ��SPI1Ӳ������  */
	SPI_Cmd(sFLASH_SPI, ENABLE);
}


uint8_t spi1_send_byte(uint8_t byte)
{
	//���SPI1�ķ��ͻ������Ƿ�Ϊ��
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	
	//ͨ��SPI1Ӳ����������
	SPI_I2S_SendData(SPI1, byte);
	
	//�ȴ��ֽڽ���
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	//���ؽ��յ�������
	return SPI_I2S_ReceiveData(SPI1);
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
	
	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//��������
		d=USART_ReceiveData(USART1);
		
		
	
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}




