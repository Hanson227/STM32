#include "wenshi.h"
#include "sys.h"
#include "./user/usart/usart.h"
#include "./user/delay/delay.h"



void dht11_init(void)
{
	//温湿度使用的引脚为G9
	//打开端口G9的硬件时钟，就是对端口G供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//配置GPIO硬件信息
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9; 		//第9根引脚
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;	//增加输出电流
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;//引脚的响应速度为100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//不添加上下拉电阻
	
	//初始化GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	//引脚的初始的电平为高电平，看时序图【*】
	PGout(9)=1;
}
//实现输入输出模式的切换
void dht11_pin_mode(GPIOMode_TypeDef pin_mode)
{
	//配置GPIO硬件信息
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9; 		//第9根引脚
	GPIO_InitStructure.GPIO_Mode	= pin_mode;			//输入/输出模式
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;	//增加输出电流
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;//引脚的响应速度为100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//不添加上下拉电阻
	
	//初始化GPIO
	GPIO_Init(GPIOG,&GPIO_InitStructure);
}
//读取温度跟湿度
int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t check_sum;
	
	//保证当前为输出模式
	dht11_pin_mode(GPIO_Mode_OUT);

	PGout(9)=0;
	
	delay_ms(20);

	PGout(9)=1;

	delay_us(30);


	//保证当前为输入模式
	dht11_pin_mode(GPIO_Mode_IN);
	
	//等待低电平出现
	t=0;	
	while(PGin(9))
	{
		t++;
		delay_us(1);	
		if(t > 5000)
			return -1;
	}

	//检测低电平的有效性，持续80us
	t=0;	
	while(PGin(9)==0)
	{
		t++;	
		delay_us(1);
		//该超时时间必须大于80
		if(t > 1000)
			return -2;
	}

	//检测高电平的有效性，持续80us
	t=0;
	while(PGin(9))
	{
		t++;
		
		delay_us(1);
		//该超时时间必须大于80		
		if(t > 1000)
			return -3;
	}
	//重复5次接收字节的过程
	for(j=0; j<5; j++)
	{
		//接收1个字节
		for(i=7; i>=0; i--)
		{
			//检测低电平的有效性，持续50us
			t=0;
			while(PGin(9)==0)
			{
				t++;
				delay_us(1);		
				//该超时时间必须大于50
				if(t > 1000)
					return -4;
			}
			//延时40us
			delay_us(40);
			if(PGin(9))
			{
				//就将对应的bit位置1
				d|=1<<i;
				
				//等待高电平持续完毕
				t=0;
				while(PGin(9))
				{
					t++;
					delay_us(1);
					//该超时时间必须大于70		
					if(t > 1000)
						return -5;
				}			
			}
		
		}
		pbuf[j]=d;
		d=0;	
	}
	
	//通信的结束
	delay_us(100);
	
	//进行校验和的计算
	check_sum=(pbuf[0]+pbuf[1]+pbuf[2]+pbuf[3])&0xFF;
	
	//判断校验和的准确性
	if(check_sum != pbuf[4])
		return -6;

	return 0;

}



