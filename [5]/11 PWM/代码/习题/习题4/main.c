#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 		GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;


void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick���ر�ϵͳ��ʱ��
		SysTick->LOAD = (168000)-1; 	// ���ü���ֵ(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
		SysTick->CTRL = 0; 		// Disable SysTick

	}	
}

static uint16_t tim13_period=0;

void tim13_init(uint32_t freq)
{
	//ʹ�ܶ�ʱ��13��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	
	//���ö�ʱ��13��Ƶֵ������ֵ��
	TIM_TimeBaseStructure.TIM_Period= (10000/freq)-1;			//����ֵ
	
	tim13_period = TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;		//8400-1+1=8400,����8400��Ԥ��Ƶֵ�����е�һ�η�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ����ķ���
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	
	  /* ͨ��1������PWM1ģʽ��*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//��/�ر��������
	TIM_OCInitStructure.TIM_Pulse = 0;							//�Ƚ�ֵ����ռ�ձ�  Խ���Խ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//��Ч״̬Ϊ�ߵ�ƽ������Ч״̬Ϊ�͵�ƽ

	TIM_OC1Init(TIM13, &TIM_OCInitStructure);

	
	//ʹ�ܶ�ʱ��13����
	TIM_Cmd(TIM13, ENABLE);

}

//���ö�ʱ��13��PWMռ�ձ�
//0%~100%  0.0 ~ 1
void TIM13_Set_duty(uint16_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_period+1)*duty/100;//�Ƚ�ֵ
	
	TIM_SetCompare1(TIM13,cmp);
}



int main(void)
{
	uint16_t freq[4]={523,587,659,740};
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;		//��8������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//����ģʽ/�๦��ģʽ�������������Ź���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//��PF8�������ӵ���ʱ��13
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_TIM13);
	
	
	while(1)
	{
		uint32_t i;
		for(i = 0;i<4;i++)
		{
			tim13_init(freq[i]);
			TIM13_Set_duty(25);
			delay_ms(1000);
		}
	
	}
	
}
