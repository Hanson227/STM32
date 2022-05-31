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


void tim3_init(void)
{
	//ʹ�ܶ�ʱ��3Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//���ö�ʱ��3��Ƶֵ������ֵ��
	TIM_TimeBaseStructure.TIM_Period = (10000/20)-1;	//����ֵ��0~499���������Ƶ��Ϊ20Hz
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//4200-1+1=4200,����4200��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;		//��F407�ǲ�֧��
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	
	/* ͨ��1������PWM1ģʽ */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//��/�ر��������
	TIM_OCInitStructure.TIM_Pulse = 250;						 //�Ƚ�ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	 //��Ч״̬Ϊ�ߵ�ƽ������Ч״̬Ϊ�͵�ƽ
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	
	//ʹ�ܶ�ʱ��3����
	TIM_Cmd(TIM3, ENABLE);

}


int main(void)
{
	
	int32_t pwm_cmp=0;
	
	//ʹ��(��)�˿�C��Ӳ��ʱ�ӣ����ǶԶ˿�C����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;		//��6������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;		//���ù���ģʽ�������Ž�������Ӳ���Զ�����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	//��PC6�������ӵ���ʱ��3
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	
	
	//��ʱ��3�ĳ�ʼ��
	tim3_init();
	
	while(1)
	{

		
	}
}

