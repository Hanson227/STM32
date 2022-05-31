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

void ir_init(void)
{

	//�˿�AӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	
	//����PA8Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		//��8������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOA,&GPIO_InitStructure);		
	
	
}

int32_t ir_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	
	
	//����ʼ��ƽ�Ƿ�Ϸ�
	if(PAin(8))
		return -1;
	
	
	//�����͵�ƽ�Ϸ���
	t=0;
	while(PAin(8)==0)
	{
		t++;
		
		delay_us(10);
		
		//10ms�ĳ�ʱʱ��
		if(t>=1000)
			return -2;
	
	}

	//�����ߵ�ƽ�Ϸ���
	t=0;
	while(PAin(8))
	{
		t++;
		
		delay_us(10);
		
		//6ms�ĳ�ʱʱ��
		if(t>=600)
			return -3;
	}

	for(j=0; j<4; j++)
	{
		d=0;
		//һ���ֽڵĽ��գ��������Чλ��ʼ����
		for(i=0; i<8; i++)
		{
		
			//�ȴ�����0/����1��ǰ�õ͵�ƽ�������
			t=0;
			while(PAin(8)==0)
			{
				t++;
				
				delay_us(10);
				
				//1ms�ĳ�ʱʱ��
				if(t>=100)
					return -4;
			
			}

			//��ʱ700us
			delay_us(700);
		
			//�жϵ�ǰPG9�Ƿ�Ϊ�ߵ�ƽ���ǵ͵�ƽ
			//���Ǹߵ�ƽ����Ϊ����1��
			//���ǵ͵�ƽ����Ϊ����0��
			if(PAin(8))
			{
				d|=1<<i;//������d��Ӧ�ı���λ��1����i=7��d|=1<<7���ǽ�����d��bit7��1
			
			
				//�ȴ��ߵ�ƽ�������
				t=0;
				while(PAin(8))
				{
					t++;
					
					delay_us(10);
					
					//��ʱ2ms�ļ��
					if(t>=2000)
						return -5;
				}		
			}
		}
		
		p[j]=d;	
	}
	
	//����У��
	if(p[0]+p[1]==255)
		if(p[2]+p[3]==255)
			return 0;
	
	
	
	return -6;
}



int main(void)
{
	int32_t rt=0;
	uint8_t buf[5]={0};
	
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
	
	printf("This is ir test\r\n");
	
	//�����ʼ��
	ir_init();
	
	
	while(1)
	{

		rt = ir_read(buf);
		
		if(rt == 0)
		{
			printf("%02X %02X  %02X  %02X\r\n",buf[0],buf[1],buf[2],buf[3]);
		
		}
		
		//oled��ʾ
		
		
		//��ʪ�ȶ�ȡ
		
		//......
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




