#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;

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


int main(void)
{
	
	int32_t pwm_cmp=0;
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//���ù���ģʽ�������Ž�������Ӳ���Զ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//��PF9�������ӵ���ʱ��14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	
	//��ʱ��14�ĳ�ʼ��
	tim14_init();
	
	while(1)
	{
		//�ƽ���
		for(pwm_cmp=0; pwm_cmp<=100; pwm_cmp++)
		{
		
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			delay_ms(20);
		}
		//�ƽ���		
		for(pwm_cmp=100; pwm_cmp>=0; pwm_cmp--)
		{
		
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			delay_ms(20);
		}		
		
	}
}


void TIM3_IRQHandler(void)
{
	//�жϱ�־λ
	if(SET==TIM_GetITStatus(TIM3, TIM_IT_Update))
	{
		PFout(9)^=1;
	
	
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
}

