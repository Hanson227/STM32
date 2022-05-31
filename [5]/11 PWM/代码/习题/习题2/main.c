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
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;	//����ֵ��0~99���������Ƶ��Ϊ100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 4200-1;		//8400-1+1=8400,����8400��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

	
	/* ͨ��1������PWM1ģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��/�ر��������
	TIM_OCInitStructure.TIM_Pulse = 100;							 //�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //��Ч״̬Ϊ�ߵ�ƽ������Ч״̬Ϊ�͵�ƽ
	
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);
	
	
	
	//ʹ�ܶ�ʱ��14����
	TIM_Cmd(TIM14, ENABLE);
}


void tim1_init(void)
{
	//ʹ�ܶ�ʱ��1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//���ö�ʱ��1��Ƶֵ������ֵ��
	//��ʱ��1Ӳ��ʱ��Ƶ��=��84MHz * 2��/TIM_Prescaler��16800��=10000Hz
	TIM_TimeBaseStructure.TIM_Period = (10000/100)-1;	//����ֵ��0~99���������Ƶ��Ϊ100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 16800-1;		//8400-1+1=8400,����8400��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* ͨ��1������PWM1ģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��/�ر��������
	TIM_OCInitStructure.TIM_Pulse = 100;							 //�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //��Ч״̬Ϊ�ߵ�ƽ������Ч״̬Ϊ�͵�ƽ
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	

	//ʹ�ܶ�ʱ��1����
	TIM_Cmd(TIM1, ENABLE);
	
	//����PWM�������
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
}


int main(void)
{
	
	int32_t pwm_cmp=0;
	
	//ʹ��(��)�˿�E��Ӳ��ʱ�ӣ����ǶԶ˿�E����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//���ù���ģʽ�������Ž�������Ӳ���Զ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;		//��13������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//���ù���ģʽ�������Ž�������Ӳ���Զ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	//��PF9�������ӵ���ʱ��14
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	//��PE13�������ӵ���ʱ��13
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);	
	
	//��ʱ��14�ĳ�ʼ��
	tim14_init();
	
	//��ʱ��1�ĳ�ʼ��	
	tim1_init();
	
	while(1)
	{
		//�ƽ���
		for(pwm_cmp=0; pwm_cmp<=100; pwm_cmp++)
		{
			//���ö�ʱ��14ͨ��1�Ƚ�ֵ
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			//���ö�ʱ��1ͨ��3�Ƚ�ֵ
			TIM_SetCompare3(TIM1,pwm_cmp);			
			
			delay_ms(20);
		}
		//�ƽ���		
		for(pwm_cmp=100; pwm_cmp>=0; pwm_cmp--)
		{
			//���ö�ʱ��14ͨ��1�Ƚ�ֵ		
			TIM_SetCompare1(TIM14,pwm_cmp);
			
			//���ö�ʱ��1ͨ��3�Ƚ�ֵ
			TIM_SetCompare3(TIM1,pwm_cmp);				
			
			delay_ms(20);
		}		
		
	}
}

