#include "led.h"
#include "sys.h"

void led_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF 
						  |RCC_AHB1Periph_GPIOE, ENABLE);	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	//����Ӳ��������GPIO���˿�F����9 10������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	//����Ӳ��������GPIO���˿�E����13 14������	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13 | GPIO_Pin_14;	
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	PFout(9) = 1;
	PFout(10) = 1;
	PEout(13) = 1;
	PEout(14) = 1;
	
}

void beep_init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;				//��8������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	//PFout(8) = 1;
}

