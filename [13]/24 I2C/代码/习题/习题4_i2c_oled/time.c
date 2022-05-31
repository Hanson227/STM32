#include "time.h"


volatile uint8_t countdown = 0;

void tim1_init(void)
{
	//ʹ�ܶ�ʱ��3Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//���ö�ʱ��3��Ƶֵ������ֵ��
	TIM_TimeBaseStructure.TIM_Period = 10000-1;			//����ֵ��0~4999��������ʱʱ��Ϊ1/2��
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;		//8400-1+1=8400,����8400��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	//���ö�ʱ��3�жϵĴ�����ʽ��ʱ�����
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	//���ö�ʱ��3���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	//ʹ�ܶ�ʱ��3����
	TIM_Cmd(TIM1, ENABLE);
}


void TIM1_UP_TIM10_IRQHandler(void)
{
	//�жϱ�־λ
	if(SET==TIM_GetITStatus(TIM1, TIM_IT_Update))
	{
		countdown++;
	
	
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}
}
