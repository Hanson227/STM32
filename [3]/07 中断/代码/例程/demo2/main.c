#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 	GPIO_InitStructure;
static EXTI_InitTypeDef   	EXTI_InitStructure;
static NVIC_InitTypeDef   	NVIC_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;
	
	while(i--);
}


void exti0_init(void)
{
	//ʹ��(��)�˿�A��Ӳ��ʱ�ӣ����ǶԶ˿�A����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	//ʹ��ϵͳ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//����PA0����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//��0������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	


	//��PA0��EXTI0������һ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	//�ⲿ�жϵ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;		//�ⲿ�ж�0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//�½��ش���������ʶ�𰴼��İ��£������ش��������ڼ�ⰴ�����ɿ���  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0������ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�0x02
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//��Ӧ���ȼ�0x03
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܸ�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
}

void exti2_init(void)
{
	//ʹ��(��)�˿�E��Ӳ��ʱ�ӣ����ǶԶ˿�E����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	
	//ʹ��ϵͳ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	
	
	//����PE2����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;		//��2������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOE,&GPIO_InitStructure);	


	//��PE2��EXTI2������һ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);

	//�ⲿ�жϵ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;		//�ⲿ�ж�2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//�½��ش���������ʶ�𰴼��İ��£������ش��������ڼ�ⰴ�����ɿ���  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ⲿ�ж�2������ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�0x03
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//��Ӧ���ȼ�0x03
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܸ�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
}

int main(void)
{
	
		
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	PFout(10)=1;

	
	//�ж����ȼ��������ã�ѡ��ڶ���
	//���ȼ�����ֵԽС�����ȼ���Խ��
	//֧��4����ռ���ȼ�:0~3
	//֧��4����Ӧ���ȼ�:0~3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	//exti0��ʼ��
	exti0_init();
	
	//exit2��ʼ��
	exti2_init();
	
	while(1)
	{
		

	}
}

void EXTI0_IRQHandler(void)
{

	//�ж��Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		//����LED1
		PFout(9)=0;
		
		//��ʱһ��
		delay();
		
		//Ϩ��LED1
		PFout(9)=1;
		
		//��ʱһ��
		delay();		
	
		/* ��ձ�־λ������CPU���Ѿ���ɵ�ǰ�жϴ���������Ӧ�µ��ж�����*/
		EXTI_ClearITPendingBit(EXTI_Line0);
	}

}

void EXTI2_IRQHandler(void)
{

	//�ж��Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		//����LED2
		PFout(10)=0;
		
		//��ʱһ��
		delay();
		
		//Ϩ��LED2
		PFout(10)=1;
		
		//��ʱһ��
		delay();		
	
		/* ��ձ�־λ������CPU���Ѿ���ɵ�ǰ�жϴ���������Ӧ�µ��ж����� */
		EXTI_ClearITPendingBit(EXTI_Line2);
	}

}
