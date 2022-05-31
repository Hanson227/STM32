#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

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


void tim1_init(void)
{
	//ʹ�ܶ�ʱ��1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//���ö�ʱ��1��Ƶֵ������ֵ��
	//��ʱ����Ӳ��ʱ��Ƶ��=168MHz/8400=20000Hz
	TIM_TimeBaseStructure.TIM_Period = (20000/2)-1;		//����ֵ��0~4999��������ʱʱ��Ϊ1/4��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//16800-1+1=16800,����16800��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	
	//���ö�ʱ��1�жϵĴ�����ʽ��ʱ�����
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	
	//���ö�ʱ��1���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//ʹ�ܶ�ʱ��1����
	TIM_Cmd(TIM1, ENABLE);

}


int main(void)
{
	
		
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	
	//��ʱ��1�ĳ�ʼ��
	tim1_init();
	
	while(1)
	{
				
			
	}
}


void TIM1_UP_TIM10_IRQHandler(void)
{
	//�жϱ�־λ
	if(SET==TIM_GetITStatus(TIM1, TIM_IT_Update))
	{
		PFout(9)^=1;
	
	
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
	
	//�жϱ�־λ
//	if(SET==TIM_GetITStatus(TIM10, TIM_IT_Update))
//	{
//		PFout(9)^=1;
//	
//	
//		//��ձ�־λ
//		TIM_ClearITPendingBit(TIM10,TIM_IT_Update);
//	}	
	
}

