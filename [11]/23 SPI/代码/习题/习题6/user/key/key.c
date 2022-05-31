#include "key.h"
#include "sys.h"


void key_init(void)
{
	//ʹ�ܶ˿�A��Ӳ��ʱ�ӣ��˿�A���ܹ�����˵���˾��ǶԶ˿�A�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//ʹ�ܶ˿�E��Ӳ��ʱ�ӣ��˿�E���ܹ�����˵���˾��ǶԶ˿�E�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		
	
	//ʹ��ϵͳ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//����Ӳ��������GPIO���˿�A����0������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;			//��0 ������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;			//����ģʽ
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStruct);		
	
	//����Ӳ��������GPIO���˿�E����2 3 4������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	//��2 3 4 ������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;			//����ģʽ
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOE,&GPIO_InitStruct);		


	//��PA0�������ӵ��ⲿ�ж�0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	//��PE2�������ӵ��ⲿ�ж�2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	
	//��PE3�������ӵ��ⲿ�ж�3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	
	//��PE4�������ӵ��ⲿ�ж�4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* �����ⲿ�ж�0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;				//ָ�������ⲿ�ж�0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//�����ش�����ʶ�𰴼����ɿ�������һ��ʶ�������أ��ͻ���CPU�����ж����� 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//ʹ��
	EXTI_Init(&EXTI_InitStructure);

	/* �����ⲿ�ж�2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;				//ָ�������ⲿ�ж�2
	//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	/* �����ⲿ�ж�3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;				//ָ�������ⲿ�ж�3
	//ʹ��
	EXTI_Init(&EXTI_InitStructure);	
	
	/* �����ⲿ�ж�4 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;				//ָ�������ⲿ�ж�4
	//ʹ��
	EXTI_Init(&EXTI_InitStructure);		
	
	/*�����ⲿ�ж�0�����ȼ�*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ�0x2	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//��Ӧ���ӣ����ȼ�0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	
	/*�����ⲿ�ж�2�����ȼ�*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ⲿ�ж�2�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x05;//��ռ���ȼ�0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//��Ӧ���ӣ����ȼ�0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	/*�����ⲿ�ж�3�����ȼ�*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//�ⲿ�ж�3�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;//��ռ���ȼ�0x0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//��Ӧ���ӣ����ȼ�0x3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
	
	/*�����ⲿ�ж�4�����ȼ�*/	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//�ⲿ�ж�4�жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0E;//��ռ���ȼ�0xF
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//��Ӧ���ӣ����ȼ�0xF
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��
	NVIC_Init(&NVIC_InitStructure);	
}

