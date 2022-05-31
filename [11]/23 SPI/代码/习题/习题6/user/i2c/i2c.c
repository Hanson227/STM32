#include "i2c.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include "./user/delay/delay.h"

static GPIO_InitTypeDef		GPIO_InitStruct;
//pin:B8 SCL		B9 SDA

#define		SCL_W			PBout(8)	
#define		SDA_W			PBout(9)	
#define		SDA_R			PBin(9)	

//初始化引脚口
void at24cxx_init(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//配置端口B PB8 PB9为输出模式模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;			//第8 9个引脚
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;				//输出模式
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;				//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;				//增加输出电流的能力
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;				//不需要上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	SCL_W = 1;
	SDA_W = 1;
	
		
}
//输入输出不同模式的配置
void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{
	//配置GPIO硬件信息
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_9; 		//第9根引脚
	GPIO_InitStruct.GPIO_Mode	= pin_mode;			//输入/输出模式
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;	//增加输出电流
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_100MHz;//引脚的响应速度为100MHz
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//不添加上下拉电阻
	
	//初始化GPIO
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
	

//开始信号
void i2c_start(void)
{
	sda_pin_mode(GPIO_Mode_OUT);
	//开始前都为高电平
	SCL_W=1;
	SDA_W=1;
	delay_us(5);
	//数据线由高变低表示开始
	SDA_W=0;
	delay_us(5);

	SCL_W=0;
	delay_us(5);
}

//停止信号
void i2c_stop(void)
{
	sda_pin_mode(GPIO_Mode_OUT);		//配置为输出模式
	//停止前都为低电平
	SCL_W=0;
	SDA_W=0;
	
	delay_us(5);
	
	//时钟线先置高
	SCL_W=1;
	
	delay_us(5);
	
	//数据线由低至高，表示信号停止
	SDA_W=1;
	
	delay_us(5);
}
//发送数据/地址
void i2c_send_byte(uint8_t txd)
{
	int32_t i=0;
	
	sda_pin_mode(GPIO_Mode_OUT);	//配置为输出模式
	
	for(i=7; i>=0; i--)
	{
		//获取数据
		if(txd & (1<<i))
			SDA_W = 1;
		else
			SDA_W = 0;	
		
		//先将数据线置高电平
		SCL_W = 1;
		delay_us(5);	
		//再将数据线置低电平，完成一个位的传输
		SCL_W = 0;
		delay_us(5);	
	}
}
//等待应答信号
uint8_t i2c_wait_ack(void)
{
	uint8_t  ack=0;
	
	sda_pin_mode(GPIO_Mode_IN);	//sda数据线必须为输入模式，接收从机的应答信号
	
	//数据线先置高电平
	SCL_W = 1;
	delay_us(5);	
	
	if(SDA_R)
		ack=1;	//无应答
	else
		ack=0;	//有应答

	SCL_W=0;
	delay_us(5);	
	
	return ack;
}
//读模式时，发送应答信号
void i2c_ack(uint8_t ack)
{
	sda_pin_mode(GPIO_Mode_OUT);	//配置为输出模式

	//获取数据
	if(ack)
		SDA_W=1;
	else
		SDA_W=0;

	SCL_W=1;
	delay_us(5);	


	SCL_W=0;
	delay_us(5);	
	
}
//
uint8_t i2c_recv_byte(void)
{
	uint8_t d = 0;
	int32_t i = 0;
	
	
	//数据线先置高电平
	for(i=7; i>=0; i--)
	{
		sda_pin_mode(GPIO_Mode_IN);//sda数据线必须为输入模式，接收从机的应答信号
		SCL_W=1;
		delay_us(5);	
		
		if(SDA_R)
			d|=1<<i;

		SCL_W=0;
		delay_us(5);	
	}

	return d;
}

//写数据
int32_t at24cxx_write(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	uint8_t *p=pbuf;
	uint8_t len_old=len;
	
	i2c_start();			//发送开始信号
	
	i2c_send_byte(0xA0);	//进行设备寻址，E2PROM（at24c02）地址0x1010 0000
	
	ack = i2c_wait_ack();	//等待应答信号
	
	if(ack)					//非应答则中止信号
	{
		i2c_stop();
		printf("device address with write access fail\r\n");
		return -1;
	}
	
	i2c_send_byte(addr);	//发送要写入的地址
	
	ack=i2c_wait_ack();		//检测总线是否有从机发出的应答信号
	
	if(ack)	//无应答
	{
		i2c_stop();
		printf("work address fail\r\n");
		return -2;
	}
	
	while(len--)			//写入数据
	{
		//发送要写入的数据
		i2c_send_byte(*p++);

		ack = i2c_wait_ack();	//检测是否有应答
		
		if(ack)
		{
			i2c_stop();
			printf("data  fail,have write %d bytes\r\n",len_old-len);
			return -3;
		}
	}
	
	i2c_stop();			//正常停止信号
		
	return 0;
		
}


int32_t at24cxx_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	uint8_t *p=pbuf;
	
	//发送起始信号
	i2c_start();
	
	//进行设备寻址，寻找at24c02的设备
	i2c_send_byte(0xA0);
	
	//检测总线是否有从机发出的应答信号
	ack=i2c_wait_ack();
	
	if(ack)	//无应答
	{
		i2c_stop();
		
		printf("device address with write access fail\r\n");
		return -1;
	}
	
	//发送要读取数据的首地址
	i2c_send_byte(addr);
	
	//检测总线是否有从机发出的应答信号
	ack=i2c_wait_ack();
	
	if(ack)	//无应答
	{
		i2c_stop();
		
		printf("word address fail\r\n");
		return -2;
	}
	
	
	//再一次发送起始信号-----------------------------------读模式
	i2c_start();
	
	//进行读访问设备寻址，寻找at24c02的设备
	i2c_send_byte(0xA1);
	
	//检测总线是否有从机发出的应答信号
	ack=i2c_wait_ack();
	
	if(ack)	//无应答
	{
		i2c_stop();
		
		printf("device address with read access fail\r\n");
		
		return -3;

	}	
	
	
	//连续读取数据
	len = len-1;
	
	while(len--)
	{
		//读取数据
		*p++=i2c_recv_byte();
		
		//每读取一个字节，发送有应答信号给到从机
		i2c_ack(0);
	}
	
	//读取最后的数据
	*p=i2c_recv_byte();
		
	//发送无应答信号给到从机
	i2c_ack(1);	
	
	
	//通信结束
	i2c_stop();
	
	
	return 0;

}


