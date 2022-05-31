#include "stm32f4xx.h" 
#include "sys.h"
#include "delay.h"
#include "includes.h"

static GPIO_InitTypeDef 		GPIO_InitStructure;

void dht11_init(void)
{

	//�˿�GӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	
	
	//����PG9Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOG,&GPIO_InitStructure);		
	
	
	//PG9��ʼ״̬Ϊ�ߵ�ƽ����ʱ��ͼ
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	uint32_t check_sum=0;
	
	//PG9����Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOG,&GPIO_InitStructure);	
	
	PGout(9)=0;
	delay_ms(20);
	
	PGout(9)=1;
	delay_us(30);	
	
	
	//PG9����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	//�ȴ��͵�ƽ����
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		if(t>=4000)
			return -1;

	}
	
	
	//�����͵�ƽ�Ϸ���
	t=0;
	while(PGin(9)==0)
	{
		t++;
		
		delay_us(1);
		
		if(t>=1000)
			return -2;
	
	}

	//�����ߵ�ƽ�Ϸ���
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		if(t>=1000)
			return -3;
	}

	for(j=0; j<5; j++)
	{
		d=0;
		//һ���ֽڵĽ��գ��������Чλ��ʼ����
		for(i=7; i>=0; i--)
		{
		
			//�ȴ�����0/����1��ǰ�õ͵�ƽ�������
			t=0;
			while(PGin(9)==0)
			{
				t++;
				
				delay_us(1);
				
				if(t>=1000)
					return -4;
			
			}

			//��ʱ40us
			delay_us(40);
		
			//�жϵ�ǰPG9�Ƿ�Ϊ�ߵ�ƽ���ǵ͵�ƽ
			//���Ǹߵ�ƽ����Ϊ����1��
			//���ǵ͵�ƽ����Ϊ����0��
			if(PGin(9))
			{
				d|=1<<i;//������d��Ӧ�ı���λ��1����i=7��d|=1<<7���ǽ�����d��bit7��1
			
			
				//�ȴ��ߵ�ƽ�������
				t=0;
				while(PGin(9))
				{
					t++;
					
					delay_us(1);
					
					if(t>=1000)
						return -5;
				}		
			}
		}
		
		p[j]=d;	
	}
	
	//У���
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	
	return 0;
	
}