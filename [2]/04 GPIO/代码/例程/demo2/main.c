#include "stm32f4xx.h"


static GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;
	
	while(i--);
}

int main(void)
{
	//led1:PF9 led2:PF10 led3:PE13 led4:PE14
	
	//ʹ��(��)�˿�A��Ӳ��ʱ�ӣ����ǶԶ˿�A����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			//��0������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;			//����ģʽ
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������,���ʹ���ڲ�������GPIO_PuPd_UP
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	while(1)
	{
		//������ŵ�ƽ��״̬
		if(Bit_RESET == GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))
		{
			//led1����
			GPIO_ResetBits(GPIOF,GPIO_Pin_9);
		
		}
		else
		{
			//led1Ϩ��
			GPIO_SetBits(GPIOF,GPIO_Pin_9);		
		}
		
	}



}
