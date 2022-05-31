#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static GPIO_InitTypeDef		GPIO_InitStructure;
static USART_InitTypeDef	USART_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  TIM_OCInitStructure;

static volatile uint8_t  g_usart1_buf[32]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;
static volatile uint32_t g_iwdg_rest=0;

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

void tim3_init(void)
{

	//ʹ�ܶ�ʱ��3��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//���ö�ʱ��3��������ʱʱ��
	//��ʱ��3��Ӳ��ʱ��=84MHz/8400=10000Hz
	//ֻҪ��ʱ��3����10000�μ���������1��ʱ��ĵ���
	//ֻҪ��ʱ��3����10�μ���������1����ʱ��ĵ���
	TIM_TimeBaseStructure.TIM_Period = 10000-1;					//����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//Ԥ��Ƶֵ��Ҳ�������Ϊ��һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//F407��û��ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
		
	
	//���ö�ʱ��3���ж�

	TIM_ITConfig(TIM3,TIM_IT_Update , ENABLE);
	
	//������ʱ��3
	TIM_Cmd(TIM3, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;		//��ʱ��3���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVICʹ�ܶ�ʱ��3�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);	
  

}
/*
void tim3_pwm_init(void)
{

	//ʹ�ܶ˿�A��Ӳ��ʱ�ӣ��˿�A���ܹ�����˵���˾��ǶԶ˿�A�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	

	//ʹ�ܶ�ʱ��3��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	
	//����Ӳ��������GPIO���˿�A����6,7������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//��6������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//��PA6��������Ϊ��ʱ��3��PWM����
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);	
	

	
	
	//���ö�ʱ��3���������Ƶ��100Hz
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;			//����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;				//Ԥ��Ƶֵ��Ҳ�������Ϊ��һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//F407��û��ʱ�ӷ�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	//ռ�ձ�
	//����ͨ��1������PWM1ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//������ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�������
	TIM_OCInitStructure.TIM_Pulse = 100;								//�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��Ч״̬���ߵ�ƽ�������Ч״̬���͵�ƽ���
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	
	//������ʱ��3
	TIM_Cmd(TIM3, ENABLE);
}
*/
void dht11_init(void)
{
	//ʹ�ܶ˿�G��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	

	
	//����PG9Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOG,&GPIO_InitStructure);	


	//PG9��ʼ��ƽΪ�ߵ�ƽ
	PGout(9)=1;
}


void dht11_pin_mode(GPIOMode_TypeDef gpio_mode)
{
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_Mode=gpio_mode;			//���/����ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOG,&GPIO_InitStructure);	

}



int32_t dht11_read(uint8_t *pdht_data)
{
	uint32_t t=0;
	
	uint8_t d;
	
	int32_t i=0;
	int32_t j=0;
	uint32_t check_sum=0;
	
	//��֤����Ϊ���ģʽ
	dht11_pin_mode(GPIO_Mode_OUT);	
	
	PGout(9)=0;
	
	delay_ms(20);
	
	PGout(9)=1;	
	
	delay_us(30);
	
	//��֤����Ϊ����ģʽ
	dht11_pin_mode(GPIO_Mode_IN);

	
	//�ȴ�DHT11��Ӧ���ȴ��͵�ƽ����
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 4000)
			return -1;
	}
	
	//���͵�ƽ����100us
	t=0;
	while(PGin(9)==0)
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -2;
	}
	
	
	//���ߵ�ƽ����100us
	t=0;
	while(PGin(9))
	{
	
		t++;
		
		delay_us(1);
		
		if(t >= 100)
			return -3;
	}
	
	//��������5���ֽ�
	for(j=0; j<5; j++)
	{
		d = 0;
		//���8��bit���ݵĽ��գ���λ����
		for(i=7; i>=0; i--)
		{
			//�ȴ��͵�ƽ�������
			t=0;
			while(PGin(9)==0)
			{
			
				t++;
				
				delay_us(1);
				
				if(t >= 100)
					return -4;
			}	
			
			
			delay_us(40);
			
			if(PGin(9))
			{
				d|=1<<i;
				
				//�ȴ�����1�ĸߵ�ƽʱ��������
				t=0;
				while(PGin(9))
				{
				
					t++;
					
					delay_us(1);
					
					if(t >= 100)
						return -5;
				}			
			
			}
		}	
	
		pdht_data[j] = d;
	}
	
	
	//ͨ�ŵĽ���
	delay_us(100);
	
	//����У���
	check_sum=pdht_data[0]+pdht_data[1]+pdht_data[2]+pdht_data[3];
	
	
	
	check_sum = check_sum & 0xFF;
	
	if(check_sum != pdht_data[4])
		return -6;
	
	return 0;
}


void iwdg_init(void)
{
	//������Ź���д����
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);


	//���÷�Ƶֵ�����ܵõ��������Ź���Ӳ��ʱ��
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	
	//���ö������Ź��ļ���ֵ������Ϊ��ʱʱ��Ϊ2S��2���ḴλоƬ
	IWDG_SetReload(125*2-1);
	
	
	//ˢ�¼���ֵ
	IWDG_ReloadCounter();
	
	//ʹ�ܶ������Ź�����
	IWDG_Enable();
}

//������
int main(void)
{
	int32_t rt; 
	
	uint8_t buf[5];

	uint8_t duty;
	
	char *p = NULL;
	
	char buffer[64]={0};
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	


	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//��9 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	PAout(6)=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	
	//����1������Ϊ115200bps
	usart1_init(115200);
		//�жϸ�λԭ��
   if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
   {
   	/* IWDGRST flag set */
   	printf("iwdg reset cpu\r\n");
   
   	/* Clear reset flags */
   	RCC_ClearFlag();
   }
   else
   {
   	/* IWDGRST flag is not set */
   	printf("normal reset cpu\r\n");
   }
	tim3_init();
	
	//��ʪ��ģ���ʼ��
	dht11_init();
	
	
	
	iwdg_init();
	printf("�Ҹ�λ��\n");

	while(1)
	{
	
		//��ȡ��ʪ��
	
		rt = dht11_read(buf);
		
		if(rt < 0)
		{
			printf("dht11 read fail,error code = %d\r\n",rt);
		
		}
		else
		{
			printf("T:%d.%d,H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
		
		}
		if(buf[2] > 20)
		{
			printf("danger!!!\n");
			g_iwdg_rest = 1;
		}
		
		
		delay_ms(6000);
		
		
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
		g_usart1_buf[g_usart1_cnt] = d;
		g_usart1_cnt++;
		if(d == '#' || g_usart1_cnt >= sizeof(g_usart1_buf))
		{
			g_usart1_event = 1;
		}
		
		USART_SendData(USART1,d);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
		
		//��ձ�־λ	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
			
		if(g_iwdg_rest == 0)
		{
			IWDG_ReloadCounter();
		}
		else
		{
			while(1);
		}
    }
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
