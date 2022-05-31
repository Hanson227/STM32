#include "stm32f4xx.h" 
#include "sys.h"
#include "delay.h"
#include "includes.h"

static GPIO_InitTypeDef 		GPIO_InitStructure;

void dht11_init(void)
{

	//端口G硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	
	
	//配置PG9为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);		
	
	
	//PG9初始状态为高电平，看时序图
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	uint32_t check_sum=0;
	
	//PG9设置为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	
	
	PGout(9)=0;
	delay_ms(20);
	
	PGout(9)=1;
	delay_us(30);	
	
	
	//PG9设置为输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	//等待低电平出现
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		
		if(t>=4000)
			return -1;

	}
	
	
	//测量低电平合法性
	t=0;
	while(PGin(9)==0)
	{
		t++;
		
		delay_us(1);
		
		if(t>=1000)
			return -2;
	
	}

	//测量高电平合法性
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
		//一个字节的接收，从最高有效位开始接收
		for(i=7; i>=0; i--)
		{
		
			//等待数据0/数据1的前置低电平持续完毕
			t=0;
			while(PGin(9)==0)
			{
				t++;
				
				delay_us(1);
				
				if(t>=1000)
					return -4;
			
			}

			//延时40us
			delay_us(40);
		
			//判断当前PG9是否为高电平还是低电平
			//若是高电平，则为数据1；
			//若是低电平，则为数据0；
			if(PGin(9))
			{
				d|=1<<i;//将变量d对应的比特位置1，如i=7，d|=1<<7就是将变量d的bit7置1
			
			
				//等待高电平持续完毕
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
	
	//校验和
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	
	return 0;
	
}