#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include "MFRC522.h"

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

//MFRC522������
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8  card_readbuf[18];


//MFRC522���Ժ���
void MFRC522Test(void)
{
	u8 i,status,card_size;
	//
	status=MFRC522_Request(0x52, card_pydebuf);			//Ѱ��
	//
	if(status==0)		//���������
	{
		status=MFRC522_Anticoll(card_numberbuf);			//��ײ����			
		card_size=MFRC522_SelectTag(card_numberbuf);	//ѡ��
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//�鿨
		status=MFRC522_Write(4, card_writebuf);				//д����д��ҪС�ģ��ر��Ǹ����Ŀ�3��
		status=MFRC522_Read(4, card_readbuf);					//����
		
		//MFRC522_Halt();															//ʹ����������״̬
		//��������ʾ
		
		printf("card_pydebuf:%02X %02X\r\n",card_pydebuf[0],card_pydebuf[1]);
		
		
		
		//�����к��ԣ����һ�ֽ�Ϊ����У����
		printf("card_numberbuf:%02X %02X %02X %02X %02X\r\n",
					card_numberbuf[0],
					card_numberbuf[1],
					card_numberbuf[2],
					card_numberbuf[3],					
					card_numberbuf[4]);		
		

		//��������ʾ����λΪKbits
		printf("card_size:%d Kbits\r\n",card_size);
		
		//����״̬��ʾ������Ϊ0

		printf("status:%d \r\n",status);		
		
		//��һ�����������ʾ
		printf("card_readbuf:");
		for(i=0;i<18;i++)		
		{
			printf("%02X ",card_readbuf[i]);
		}
		printf("\r\n");

		PFout(8)=1;PFout(9)=0;delay_ms(80);
		PFout(8)=0;PFout(9)=1;delay_ms(80);		
		PFout(8)=1;PFout(9)=0;delay_ms(80);
		PFout(8)=0;PFout(9)=1;delay_ms(80);				
		
	}	
}
int main(void)
{
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//����1������:115200bps
	usart1_init(115200);	

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;		//��8 9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	PFout(8)=0;	
	PFout(9)=1;
	
	//��ʼ��MFRC522
	MFRC522_Initializtion();			

	
	while(1)
	{
		//��ʼ��MFRC522
		MFRC522_Initializtion();		
		MFRC522Test();
		delay_ms(1000);
		
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




