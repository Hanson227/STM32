#include "humiture.h"

static GPIO_InitTypeDef GPIO_InitStructure;

void delay_us(uint32_t n)
{
	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统定时器
	SysTick->LOAD = (168*n)-1; // 配置计数值(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (168000)-1; 	// 配置计数值(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

}

void humi_init(void)
{
	//使能(打开)端口G的硬件时钟，就是对端口供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//指定引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  //没有使能上拉或下拉电阻	
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

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	PGOut(9) = 1;
	
	PGOut(9) = 0;
	delay_ms(20);	//主机拉低20ms
	
	PGOut(9) = 1;
	delay_us(40);	//主机拉高40us
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	//输出模式
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	while(PGIn(9))	//等待低电平出现
	{
		delay_us(1);
		t++;
		
		if(t>=4000)
			return -1;
	}
	t = 0;
	while(!PGIn(9))	//低电平合法性
	{
		t++;
		delay_us(1);
		
		if(t>=90)
			return -2;
	}
	t = 0;
	while(PGIn(9))	//高电平合法性
	{
		t++;
		delay_us(1);
		
		if(t>=1000)
			return -3;
	}
	
	for(j=7; j<12; j++)
	{
		d=0;
		//一个字节接收
		for(i=7; i>=0; i--)
		{
			//等待数据0/1前置低电平持续完毕
			t=0;
			while(!PGIn(9))
			{
				t++;
				delay_us(1);
				
				if(t>=1000)
					return -4;
			}
			
			//延时40us
			delay_us(40);
			if(PGIn(9))
			{
				d|=1<<i;
				
				t=0;
				while(PGIn(9))	//高电平合法性
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
	
	//校验和
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	return 0;
}




