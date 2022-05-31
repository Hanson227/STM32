#include "wenshi.h"
#include "sys.h"
#include "./user/usart/usart.h"
#include "./user/delay/delay.h"



void dht11_init(void)
{
	//��ʪ��ʹ�õ�����ΪG9
	//�򿪶˿�G9��Ӳ��ʱ�ӣ����ǶԶ˿�G����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//����GPIOӲ����Ϣ
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9; 		//��9������
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;	//�����������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;//���ŵ���Ӧ�ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//���������������
	
	//��ʼ��GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	//���ŵĳ�ʼ�ĵ�ƽΪ�ߵ�ƽ����ʱ��ͼ��*��
	PGout(9)=1;
}
//ʵ���������ģʽ���л�
void dht11_pin_mode(GPIOMode_TypeDef pin_mode)
{
	//����GPIOӲ����Ϣ
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9; 		//��9������
	GPIO_InitStructure.GPIO_Mode	= pin_mode;			//����/���ģʽ
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;	//�����������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;//���ŵ���Ӧ�ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//���������������
	
	//��ʼ��GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
}
//��ȡ�¶ȸ�ʪ��
int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t check_sum;
	
	//��֤��ǰΪ���ģʽ
	dht11_pin_mode(GPIO_Mode_OUT);

	PGout(9)=0;
	
	delay_ms(20);

	PGout(9)=1;

	delay_us(30);


	//��֤��ǰΪ����ģʽ
	dht11_pin_mode(GPIO_Mode_IN);
	
	//�ȴ��͵�ƽ����
	t=0;	
	while(PGin(9))
	{
		t++;
		delay_us(1);	
		if(t > 5000)
			return -1;
	}

	//���͵�ƽ����Ч�ԣ�����80us
	t=0;	
	while(PGin(9)==0)
	{
		t++;	
		delay_us(1);
		//�ó�ʱʱ��������80
		if(t > 1000)
			return -2;
	}

	//���ߵ�ƽ����Ч�ԣ�����80us
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		//�ó�ʱʱ��������80		
		if(t > 1000)
			return -3;
	}
	//�ظ�5�ν����ֽڵĹ���
	for(j=0; j<5; j++)
	{
		//����1���ֽ�
		for(i=7; i>=0; i--)
		{
			//���͵�ƽ����Ч�ԣ�����50us
			t=0;
			while(PGin(9)==0)
			{
				t++;
				delay_us(1);		
				//�ó�ʱʱ��������50
				if(t > 1000)
					return -4;
			}
			//��ʱ40us
			delay_us(40);
			if(PGin(9))
			{
				//�ͽ���Ӧ��bitλ��1
				d|=1<<i;
				
				//�ȴ��ߵ�ƽ�������
				t=0;
				while(PGin(9))
				{
					t++;
					delay_us(1);
					//�ó�ʱʱ��������70		
					if(t > 1000)
						return -5;
				}			
			}
		
		}
		pbuf[j]=d;
		d=0;	
	}
	
	//ͨ�ŵĽ���
	delay_us(100);
	
	//����У��͵ļ���
	check_sum=(pbuf[0]+pbuf[1]+pbuf[2]+pbuf[3])&0xFF;
	
	//�ж�У��͵�׼ȷ��
	if(check_sum != pbuf[4])
		return -6;

	return 0;

}



