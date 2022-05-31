#include "tim.h"
#include "sys.h"

void tim13_pwm_init(int freq)
{
	//ʹ�ܶ�ʱ��13��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13, ENABLE);
	
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//���ö�ʱ��14�����Ƶ��
	//��ǰʱ��Ƶ��Ϊ10000Hz�����Ƶ��Ϊ100Hz������ֵΪ99.
	
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//����ֵ��������ǰ�����Ƶ�ʣ���ǰƵ��Ϊ100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = (8400-1);					//Ԥ��Ƶֵ8400��Ҳ��֮Ϊ��һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//ʱ�ӷ�Ƶ,���ǵڶ��η�Ƶ������F407��û�еڶ��η�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ����ķ���
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;				//��8������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//��PF8���Ÿ��õ���ʱ��13
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource8,GPIO_AF_TIM13);	
	
	//����PWM���ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//����PWM�Ĺ�����ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�����������
	//TIM_OCInitStructure.TIM_Pulse = CCR1_Val;						//�Ƚ�ֵ�����ã���ʵ����ʹ������һ������ר������TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��Ч״̬���������Ϊ�ߵ�ƽ
	
	//��ʱ��13ͨ��1�ĳ�ʼ��
	TIM_OC1Init(TIM13, &TIM_OCInitStructure);
	
	
	//ʹ�ܶ�ʱ��13����

	TIM_Cmd(TIM13, ENABLE);	
}




void tim13_set_freq(uint32_t freq)
{
	/*��ʱ���Ļ������ã��������ö�ʱ������������Ƶ��Ϊ freq Hz */
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1; //���ö�ʱ�����Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1; //��һ�η�Ƶ�����ΪԤ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	tim13_cnt= TIM_TimeBaseStructure.TIM_Period;
	
	TIM_TimeBaseInit(TIM13, &TIM_TimeBaseStructure);
}


void tim13_set_duty(uint32_t duty)
{
	uint32_t cmp=0;
	
	cmp = (tim13_cnt+1) * duty/100;
	
	TIM_SetCompare1(TIM13,cmp);
}

void tim14_pwm_init(int freq)
{
	//ʹ�ܶ�ʱ��14��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//���ö�ʱ��14�����Ƶ��
	//��ǰʱ��Ƶ��Ϊ10000Hz�����Ƶ��Ϊ100Hz������ֵΪ99.
	
	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//����ֵ��������ǰ�����Ƶ�ʣ���ǰƵ��Ϊ100Hz
	TIM_TimeBaseStructure.TIM_Prescaler = (8400-1);					//Ԥ��Ƶֵ8400��Ҳ��֮Ϊ��һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//ʱ�ӷ�Ƶ,���ǵڶ��η�Ƶ������F407��û�еڶ��η�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ����ķ���
	
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//��PF9�������ӵ���ʱ��14
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14);	
	
	//����PWM���ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//����PWM�Ĺ�����ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�����������
	TIM_OCInitStructure.TIM_Pulse = 100;						//�Ƚ�ֵ�����ã���ʵ����ʹ������һ������ר������TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��Ч״̬���������Ϊ�ߵ�ƽ
	
	//��ʱ��14ͨ��1�ĳ�ʼ��
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);

	//ʹ�ܶ�ʱ��14����
	TIM_Cmd(TIM14, ENABLE);		
}

void tim1_pwm_init(int freq)
{
	//ʹ�ܶ�ʱ��1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//ʹ�ܶ˿�E��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	
	
	//����Ӳ��������GPIO���˿�E����13��14������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14;			//��13 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;						//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;					//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;					//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;					//����Ҫ����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	
	//���ö�ʱ��1�����Ƶ��
	//��ǰʱ��Ƶ��Ϊ10000Hz�����Ƶ��Ϊ100Hz������ֵΪ99.

	TIM_TimeBaseStructure.TIM_Period = (10000/freq)-1;				//����ֵ��������ǰ�����Ƶ�ʣ���ǰƵ��Ϊ50Hz	
	TIM_TimeBaseStructure.TIM_Prescaler = (16800-1);				//Ԥ��Ƶֵ8400��Ҳ��֮Ϊ��һ�η�Ƶ
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0;					//ʱ�ӷ�Ƶ,���ǵڶ��η�Ƶ������F407��û�еڶ��η�Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//���ϼ����ķ���
	//��ʼ����ʱ��1
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	//����PWM���ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;				//����PWM�Ĺ�����ģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�����������
	TIM_OCInitStructure.TIM_Pulse = 100;						//�Ƚ�ֵ�����ã���ʵ����ʹ������һ������ר������TIMx_SetCompare
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//��Ч״̬���������Ϊ�ߵ�ƽ
	
	//��PF9�������ӵ���ʱ��13��14
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);	
	
	//��ʱ��14ͨ��3��4�ĳ�ʼ��
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);
	
	//ʹ�ܶ�ʱ��14����
	TIM_Cmd(TIM1, ENABLE);	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //pwm���
}

