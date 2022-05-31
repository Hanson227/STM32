#include "hcsr.h"
#include "sys.h"
#include <stdio.h>
#include "./user/delay/delay.h"
#include "./user/tim/tim.h"



//������hc_sr04��ʼ��
void sr04_init(void)
{

	//ʹ�ܶ˿�B��Ӳ��ʱ�ӣ��˿�B���ܹ�����˵���˾��ǶԶ˿�B�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//ʹ�ܶ˿�E��Ӳ��ʱ�ӣ��˿�E���ܹ�����˵���˾��ǶԶ˿�E�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	

	//����Ӳ��������GPIO���˿�B����6������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//��6������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//����Ӳ��������GPIO���˿�E����6������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;			//��6������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			//����ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOE,&GPIO_InitStructure);


	//�������ģʽ�����ţ���������ʼ�ĵ�ƽ���õ�ƽһ��Ҫ��ʱ��ͼ��*��
	PBout(6)=0;
}

//��ຯ��
int32_t sr04_get_distance(void)
{
	uint32_t t=0;
	PBout(6)=1;
	//�����ߵ�ƽʱ�����10us���ϣ���ǰ��д20us
	delay_us(20);
	PBout(6)=0;

	//�ȴ�����������������ߵ�ƽ���ʹ���˵���źŷ���
	while(PEin(6)==0)
	{
		t++;
		delay_us(1);
		//����ȴ�ʱ�䳬��100ms������Ϊ��ǰ�������쳣
		if(t >=100000)
			return -1;
	}

	//�����ߵ�ƽ�ĳ���ʱ��
	t=0;
	while(PEin(6))
	{
		t++;
		delay_us(9); //3mm
		//����ȴ�ʱ�䳬��900ms������Ϊ��ǰ�������쳣
		if(t >=100000)
			return -2;
	
	}
	//��Ϊ��ǰ�����ʱ��=���͵����ص�ʱ��
	t=t/2;

	return 3*t;

}

void check_dis(uint32_t d)
{
//�Ծ������Ч�Խ����ж�
	if(d >=20 && d<=4000)
	{	
		printf("distance:%dmm\r\n",d);
		
		//��45cm�����밲ȫ��������LED��Ϩ��
		if(d>=450)
		{
			PFout(9) = 1;
			PFout(10) = 1;
			PEout(13) = 1;
			PEout(14) = 1;	

			//�رշ�����
			tim13_set_duty(0);
			
		}
		//35~45cm���ǳ���ȫ������1յLED��
		else if(d>=350)
		{
			PFout(9) = 0;
			PFout(10) = 1;
			PEout(13) = 1;
			PEout(14) = 1;		
			//�رշ�����
			tim13_set_duty(0);							
		}
		//25~35cm����ȫ������2յLED��
		else if(d>=250)
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 1;
			PEout(14) = 1;	
			//��������ʼ����
			tim13_set_freq(2);
			tim13_set_duty(voice);				

		}
		//15~25cm������������3յLED��
		else if(d>=150)
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 0;
			PEout(14) = 1;			
			//��������ʼ����
			tim13_set_freq(4);
			tim13_set_duty(voice);	
		}
		//��15cm��Σ��������4յLED��
		else
		{
			PFout(9) = 0;
			PFout(10) = 0;
			PEout(13) = 0;
			PEout(14) = 0;				
			//��������ʼ����
			tim13_set_freq(10);
			tim13_set_duty(voice);	
		}
	}
	
}
