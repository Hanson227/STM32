#include "led.h"



//beep��ʼ��
void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ��(��)�˿�E��F��Ӳ��ʱ�ӣ����ǶԶ˿ڹ���
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//ָ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //û��ʹ����������������	
	GPIO_Init(GPIOF, &GPIO_InitStructure);	

	//��PF8��Ϊ�͵�ƽ
	PFOut(8) = 0;
}

//led��ʼ��
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ��(��)�˿�E��F��Ӳ��ʱ�ӣ����ǶԶ˿ڹ���
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//ָ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //û��ʹ����������������	
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//ָ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	//��PF10��Ϊ�ߵ�ƽ
	PFOut(9) = 1;
	PFOut(10) = 1;
	PEOut(13) = 1;
	PEOut(14) = 1;

}


