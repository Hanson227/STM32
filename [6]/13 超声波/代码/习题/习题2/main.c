#include <stdio.h>
#include "stm32f4xx.h"
#include "sys.h"

GPIO_InitTypeDef   GPIO_InitStructure;

EXTI_InitTypeDef   EXTI_InitStructure;

NVIC_InitTypeDef   NVIC_InitStructure;


void delay_us(uint32_t t)
{

	SysTick->CTRL = 0;			 			// Disable SysTick
	SysTick->LOAD = SystemCoreClock/1000000*t; // 1us*t
	SysTick->VAL = 0; 						// Clear current value as well as count flag
	SysTick->CTRL = 5; 						// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 						// Disable SysTick



}

void delay_ms(uint32_t t)
{
	while(t--)
	{
		SysTick->CTRL = 0;			 			// Disable SysTick
		SysTick->LOAD = SystemCoreClock/1000; 	// 1ms
		SysTick->VAL = 0; 						// Clear current value as well as count flag
		SysTick->CTRL = 5; 						// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; 						// Disable SysTick
	
	}
}

void usart1_init(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;

	/* ʹ�ܶ˿�A��ʱ�ӣ��ö˿�A���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);


	/* ����1ʱ��ʹ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


	/* ��PA9 �� PA10�������ӵ�����1 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);


	/* ����PA9��PA10����Ϊ���ã��ڶ����ܡ��๦�ܣ�����ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	//���ô��ڵ�����
	USART_InitStructure.USART_BaudRate = baud;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//����λΪ8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//ֹͣλ1��
	USART_InitStructure.USART_Parity = USART_Parity_No;								//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//֧�ַ���/����
	USART_Init(USART1, &USART_InitStructure);


	/* ���ô���1�����ȼ� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;						//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;								//��Ӧ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* ʹ�ܴ���1���� */
	USART_Cmd(USART1, ENABLE);
	
	/* ʹ�ܴ���1�Ľ����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	

}


void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{
	while(len--)
	{
		//��������
		USART_SendData(USART1,*pbuf++);
		
		//����Ƿ������
		while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	
	}
}

void usart1_send_str(uint8_t *pbuf)
{
	while(*pbuf!='\0')
	{
		//��������
		USART_SendData(USART1,*pbuf++);
		
		//����Ƿ������
		while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	
	}
}

//�ض���fput���������printf��������õ�
int fputc(int ch,FILE *f)
{
	//��������
	USART_SendData(USART1,ch);
	
	//����Ƿ������
	while(RESET==USART_GetFlagStatus(USART1,USART_FLAG_TXE));	
	
	return ch;

}

void sr04_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE, ENABLE);

	/* ����PB6����Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//��6������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//�������ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	/* ����PE6����Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;					//��6������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;				//�������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	PBout(6)=0;
}


int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	PBout(6)=1;
	delay_us(20);
	PBout(6)=0;
	
	//�ȴ������źű�Ϊ�ߵ�ƽ
	while(PEin(6)==0)
	{
		t++;
		delay_us(1);
		
		//�����ʱ���ͷ���һ��������
		if(t>=1000000)
			return -1;
	}
	
	t=0;
	//�����ߵ�ƽ������ʱ��
	while(PEin(6))
	{
	
		//��ʱ9us,����3mm�Ĵ������
		delay_us(9);
		
		t++;
		
		
		//�����ʱ���ͷ���һ��������
		if(t>=1000000)
			return -2;	
	}
	
	//��ǰ�Ĵ������
	return 3*(t/2);



}

uint32_t tim13_cnt=0;

void tim13_init(void)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* ��ʱ��13��ʱ��ʹ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	

	/*��ʱ���Ļ������ã��������ö�ʱ������������Ƶ��Ϊ100Hz */
	TIM_TimeBaseStructure.TIM_Period = (40000/100)-1;					//���ö�ʱ�����Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 2100-1;						//��һ�η�Ƶ�����ΪԤ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//�ڶ��η�Ƶ,��ǰʵ��1��Ƶ��Ҳ���ǲ���Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	tim13_cnt=TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);

	/* ����PF8 ����Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;					//��8������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;				//���ø���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	
	/* �ö�ʱ��14 PWM ��ͨ�� 1 ������ģʽ 1*/
	 //PWM ģʽ 1�� �ڵ���ģʽ�£� ֻҪTIMx_CNT < TIMx_CCR1�� ͨ�� 1 ��Ϊ��Ч״̬���ߵ�ƽ���� ����Ϊ��Ч״̬���͵�ƽ����
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//��Ч��ʱ�� ����ߵ�ƽ
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);
	
	/*  ʹ�ܶ�ʱ�� 13 ���� */
	TIM_Cmd(TIM13, ENABLE);
}

//���ö�ʱ��13��PWMƵ��
void tim13_set_freq(uint32_t freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	/*��ʱ���Ļ������ã��������ö�ʱ������������Ƶ��Ϊfreq Hz */
	TIM_TimeBaseStructure.TIM_Period = (40000/freq)-1;					//���ö�ʱ�����Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 2100-1;						//��һ�η�Ƶ�����ΪԤ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//�ڶ��η�Ƶ,��ǰʵ��1��Ƶ��Ҳ���ǲ���Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	tim13_cnt= TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);

}

//���ö�ʱ��13��PWMռ�ձ�0%~100%
void tim13_set_duty(uint32_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_cnt+1) * duty/100;

	TIM_SetCompare1(TIM13,cmp);
}


void beep_init(void)
{
	tim13_init();
	
	
	//����������
	tim13_set_duty(0);
}


void key_init(void)
{
	/* ʹ�ܶ˿�A��ʱ�ӣ��ö˿�A���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	/* ʹ�ܶ˿�E��ʱ�ӣ��ö˿�E���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	
	/* ����PA0����Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//��0������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//����ģʽ
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//���������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//���ŵĹ�������ٶȣ�100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û������������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* ����PE2 PE3 PE4����Ϊ����ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;				//��2 3 4������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


int main(void)
{
	uint32_t distance=0;
	uint32_t duty=10;
	uint32_t distance_safe=1;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);

	/* Configure PF9 PF10 in output pushpull mode������PF9 PF10����Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9����10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//�������ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	
	/* Configure PE13 PE14 in output pushpull mode������PE13 PE14����Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		//��13����14������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//�������ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	


	//�����ж����ȼ��ķ���
	//����֧��4����ռ���ȼ���֧��4����Ӧ���ȼ�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	

	PFout(9)=1;
	PFout(10)=1;	
	PEout(13)=1;
	PEout(14)=1;
	
	//����1��ʼ����������115200bps
	usart1_init(115200);

	//������ģ���ʼ��
	sr04_init();
	
	//��ӷ�������ʼ��
	beep_init();
	
	
	//�����ĳ�ʼ��
	key_init();
	
	while(1)
	{
		distance=sr04_get_distance();
		
		if(distance < 0)
		{
			//ģ���п���û�����Ӻã����Ӵ����Ż�Ű����ɶ�
			printf("sr04 is error\r\n");
		}
		else if(distance>=20 && distance<=4000)
		{
			//��������ȷ��
			printf("distance=%dmm\r\n",distance);		
		
			if(distance>=450)
			{
				PFout(9)=1;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;	
				
				distance_safe=1;
			}
			else if(distance>=350 && distance<450)
			{
				PFout(9)=0;
				PFout(10)=1;
				PEout(13)=1;
				PEout(14)=1;	

				
				distance_safe=1;
			}	
			else if(distance>=250 && distance<350)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=1;
				PEout(14)=1;	
				
				distance_safe=0;
				
				tim13_set_freq(2);

			}				
			else if(distance>=150 && distance<250)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=1;	
				
				distance_safe=0;
				
				tim13_set_freq(4);

			}	

			else if(distance<150)
			{
				PFout(9)=0;
				PFout(10)=0;
				PEout(13)=0;
				PEout(14)=0;	
				
				tim13_set_freq(10);
				
				distance_safe=0;
			}	

			//���ǰ�ȫ���룬�����������
			if(distance_safe)
				tim13_set_duty(0);
			//�����ǰ�ȫ���룬����������ݶ�Ӧ��ռ�ձȽ�������
			else
				tim13_set_duty(duty);
				
		}
		else
		{
			//�������쳣��
			printf("sr04 get distance is error\r\n");			
		
		}
		
		//��Ӱ���������
		if(PAin(0)==0)
		{
			//ռ�ձȼ�5��������С
			if(duty>0)
			{
				duty-=5;
				
				tim13_set_duty(duty);
			
			}
				
		
		}
		
		if(PEin(2)==0)
		{
			//ռ�ձȼ�5���������
			if(duty<30)
			{
				duty+=5;
				
				tim13_set_duty(duty);
			
			}		
		}
		
		//��ʱ500ms
		delay_ms(500);
	}

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	//����Ƿ��д��ڽ����жϲ���
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		//��������
		d = USART_ReceiveData(USART1);
		
		
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

