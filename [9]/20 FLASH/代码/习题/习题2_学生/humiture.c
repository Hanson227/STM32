#include "humiture.h"

static GPIO_InitTypeDef GPIO_InitStructure;

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳ��ʱ��
	SysTick->LOAD = (168*n)-1; // ���ü���ֵ(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick���ر�ϵͳ��ʱ��
		SysTick->LOAD = (168000)-1; 	// ���ü���ֵ(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

}

void humi_init(void)
{
	//ʹ��(��)�˿�G��Ӳ��ʱ�ӣ����ǶԶ˿ڹ���
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//ָ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //û��ʹ����������������	
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	PGOut(9) = 1;
}

int humi_start(uint8_t *buf)
{
	uint32_t t = 0;
	int32_t i=0,j=0;
	uint8_t d;
	uint8_t *p = buf;
	uint32_t check_sum=0;

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//���ģʽ
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	PGOut(9) = 1;
	
	PGOut(9) = 0;
	delay_ms(20);	//��������20ms
	
	PGOut(9) = 1;
	delay_us(40);	//��������40us
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	//���ģʽ
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	while(PGIn(9))	//�ȴ��͵�ƽ����
	{
		delay_us(1);
		t++;
		
		if(t>=4000)
			return -1;
	}
	t = 0;
	while(!PGIn(9))	//�͵�ƽ�Ϸ���
	{
		t++;
		delay_us(1);
		
		if(t>=90)
			return -2;
	}
	t = 0;
	while(PGIn(9))	//�ߵ�ƽ�Ϸ���
	{
		t++;
		delay_us(1);
		
		if(t>=1000)
			return -3;
	}
	
	for(j=7; j<12; j++)
	{
		d=0;
		//һ���ֽڽ���
		for(i=7; i>=0; i--)
		{
			//�ȴ�����0/1ǰ�õ͵�ƽ�������
			t=0;
			while(!PGIn(9))
			{
				t++;
				delay_us(1);
				
				if(t>=1000)
					return -4;
			}
			
			//��ʱ40us
			delay_us(40);
			if(PGIn(9))
			{
				d|=1<<i;
				
				t=0;
				while(PGIn(9))	//�ߵ�ƽ�Ϸ���
				{
					t++;
					delay_us(1);
					
					if(t>=1000)
						return -5;
				}
			}
			else
				d|=0<<i;
		}
		
		p[j] = d;
	}
	
	//У���
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	return 0;
}




