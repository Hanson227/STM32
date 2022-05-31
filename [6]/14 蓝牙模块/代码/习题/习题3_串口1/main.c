#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static GPIO_InitTypeDef			GPIO_InitStructure;
static USART_InitTypeDef		USART_InitStructure;
static NVIC_InitTypeDef			NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;


static volatile uint8_t 	g_usart_buf[32]={0};
static volatile uint32_t 	g_usart_event=0;

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



void usart3_init(uint32_t baud)
{
	//ʹ�ܴ���3Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	
	//ʹ�ܶ˿�B��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//����PB10 PB11Ϊ���ù���ģʽ
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;	//��10 11������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��PB10��PB11���ӵ�����1Ӳ��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);		

	//���ô���3�������ʣ�У��λ������λ��ֹͣλ
	USART_InitStructure.USART_BaudRate = baud;						//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ȡ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�ܹ��շ�����
	USART_Init(USART3,&USART_InitStructure);
	
	
	//���ô���3���жϴ�����ʽ���������ֽڴ����ж�
	USART_ITConfig(USART3,USART_IT_RXNE , ENABLE);
	
	
	//ʹ�ܴ���3����	
	USART_Cmd(USART3, ENABLE);
	
	
	//����NVIC������3
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		//����1���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVICʹ�ܴ���1�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);	
}


void usart3_send_str(char *str)
{
	char *p = str;
	
	while(p && (*p!='\0'))
	{
	
		USART_SendData(USART3,*p);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);	
		p++;
	}
}


void bluetooth_config(void)
{

	
    //PE6��������ߵ�ƽ����EN����Ϊ�ߵ�ƽ������ATָ��ģʽ
	PEout(6)=1;
	delay_ms(500);

	delay_ms(500);
	
	usart3_send_str("AT\r\n");
	delay_ms(500);	
	
	usart3_send_str("AT\r\n");
	delay_ms(500);	
	
	usart3_send_str("AT+NAME=TWon\r\n");
	delay_ms(500);
	
	
	//��λģ��
	usart3_send_str("AT+RESET\r\n");
	delay_ms(500);
	
	
	//PE6��������͵�ƽ����EN����Ϊ�͵�ƽ���˳�ATָ��ģʽ
	PEout(6)=0;
	
	delay_ms(1000);	

}

void tim14_init(void)
{
	//ʹ�ܶ�ʱ��14Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//���ö�ʱ��14��Ƶֵ������ֵ��
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;	//����ֵ��0~99���������Ƶ��Ϊ20Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//8400-1+1=8400,����8400��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	
	/* ͨ��1������PWM1ģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��/�ر��������
	TIM_OCInitStructure.TIM_Pulse = 250;							 //�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //��Ч״̬Ϊ�ߵ�ƽ������Ч״̬Ϊ�͵�ƽ
	
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	

	//ʹ�ܶ�ʱ��14����
	TIM_Cmd(TIM14, ENABLE);
}

//������
int main(void)
{
	int32_t d;
	
	char *p=NULL;
	
	uint8_t duty=0;
	
	char buf[32]={0};
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		

	
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	

	//ʹ�ܶ˿�E��Ӳ��ʱ�ӣ��˿�E���ܹ�����˵���˾��ǶԶ˿�E�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	


	
	//����Ӳ��������GPIO���˿�E����6������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//��6 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	//����ģ��2.0�Ͳ�����ATָ��ģʽ
	PEout(6)=0;
	
	
	//����Ӳ��������GPIO���˿�F����8������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;			//��8 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//�رշ�����
	PFout(8)=0;
	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//��PF9�������ӵ���ʱ��14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	
	
	//��ʱ��14�ĳ�ʼ��
	tim14_init();	
	
	
	//����1������Ϊ115200bps
	usart1_init(115200);
	
	printf("This is bluetooth test\r\n");
	
	//����3������Ϊ38400bps���в�������2.0ģ����9600bps
	usart3_init(38400);	
	
	//����ģ������
	//bluetooth_config();
	
	while(1)
	{
		
		if(g_usart_event)
		{
			//�ж��ַ�������Ч��
			//g_usart_buf:"duty=50#"
			if(strstr((const char *)g_usart_buf,"duty"))
			{
				//�ֽ��ַ���
				p=strtok((char *)g_usart_buf,"=");
				
				printf("%s\r\n",p);	//"duty"
				
				p=strtok(NULL,"=");
				
				printf("%s\r\n",p);	//"50#"

				//�ַ���ת��Ϊ����
				duty = atoi(p);			//"50#" -> 50
				
				//���ñȽ�ֵ
				TIM_SetCompare1(TIM14,duty);
			
				//��ʽ���ַ���
				sprintf(buf,"duty set %d ok\r\n",duty);
				
				//�����ֻ����Ѿ����óɹ�
				usart3_send_str(buf);
				
				printf(buf);
			
			}
			if(strstr((const char *)g_usart_buf,"beep"))
			{	
				if(strstr((const char *)g_usart_buf,"on"))
				{
					//�򿪷�����
					PFout(8)=1;
					
					printf("beep on ok\r\n");
				}
				if(strstr((const char *)g_usart_buf,"off"))
				{
					//�رշ�����
					PFout(8)=0;
					
					printf("beep off ok\r\n");					
					
				}
			}
			
			g_usart_event=0;
			
			memset((void *)g_usart_buf,0,sizeof g_usart_buf);

		}

		
	}
}

void  USART1_IRQHandler(void)
{
static 
	uint32_t i=0;

	uint8_t d;


	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		
		//��������
		d = USART_ReceiveData(USART1);
		
		
		g_usart_buf[i] = d;
		
		i++;
		
		//�ж��Ƿ��н�������#����ͬʱ�����û�г����������Ĵ�С
		if(d == '#' || i >=sizeof (g_usart_buf))
		{
			i=0;
			g_usart_event=1;
		}
#if 0		
		//���͵�����1->PC
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
#endif		

		
		//��ձ�־λ	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}


void  USART3_IRQHandler(void)
{
	
static 
	uint32_t i=0;

	uint8_t d;

	//����־λ
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		
		//��������
		d = USART_ReceiveData(USART3);
		
		
		g_usart_buf[i] = d;
		
		i++;
		
		//�ж��Ƿ��н�������#����ͬʱ�����û�г����������Ĵ�С
		if(d == '#' || i >=sizeof (g_usart_buf))
		{
			i=0;
			g_usart_event=1;
		}
#if 0		
		//���͵�����1->PC
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
#endif		

		//��ձ�־λ	
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}

}
