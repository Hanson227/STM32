#include "spi.h"
#include "sys.h"
#include <stdio.h>
#include "./user/delay/delay.h"
static SPI_InitTypeDef 		SPI_InitStructure;
static GPIO_InitTypeDef		GPIO_InitStruct;


void w25qxx_init(void)
{
	
	//使能端口B的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//使能SPI1硬件时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	
	//配置端口B PB3~PB5为复用功能模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
		
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);
	
	//配置端口B PB14为输出模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	

	//【*】看时序图，PB14的初始状态要输出高电平
	
	PBout(14) = 1;
	
	//配置SPI1相关的参数
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//【*】看时序图，主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//【*】看时序图，8个数据位
	
	//看时序图模式3
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//【*】看时序图，时钟线空闲的时候是什么电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							//【*】看时序图，MISO引脚采集数据是在时钟的第几个跳边沿
	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//由于片选引脚工作时间不固定，需要软件代码控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//看芯片手册是否支持该频率，SPI1的硬件时钟频率=84MHz/8=10.5MHz
	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//【*】看时序图，最高有效位优先
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	//使能SPI1硬件
	SPI_Cmd(SPI1, ENABLE);
	
}
	
//硬件---全双工同步通信
uint8_t spi1_send_byte(uint8_t txd)
{

	SPI_I2S_SendData(SPI1,txd);
	
	//等待发送完毕
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);


	//用于是全双工同步通信，等待接收完成
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	
	
	return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}



uint16_t w25qxx_read_id(void)
{
	//片选引脚输出低电平，通信的开始
	uint16_t id = 0;
	PBout(14) = 0;
	
	//通信的过程

	//1)发送0x90
		spi1_send_byte(0x90);
	
	//2)发送24bit，可以拆分为3个字节发送
		spi1_send_byte(0x00);	
		spi1_send_byte(0x00);
		spi1_send_byte(0x00);	
	
	//读取厂商id，参数可以为任意参数
	id = spi1_send_byte(0xFF)<<8;
	
	//读取设备id
	id|= spi1_send_byte(0xFF);
	
	//片选引脚输出高电平，通信的结束

	PBout(14) = 1;

	return id;
}
//写数据
void write_to_w25q128(uint32_t addr,uint8_t len,uint8_t *buf)
{
	uint8_t *arr = buf;
	//擦除扇区
	Sector_Erase(addr);
	//解锁写保护,写使能//////////////////////////////
	//片选输出低电平
	PBout(14) = 0;
	//使能写保护
	spi1_send_byte(0x06);
	//片选输出高电平
	PBout(14) = 1;
	
	
	//写入数据///////////////////////////////////////////
	//片选引脚输出低电平，通信的开始
	PBout(14)=0;
	//写入指令02
	spi1_send_byte(0x02);
	//写入要修改数据的地址
	spi1_send_byte((addr >> 16)&0xff);
	spi1_send_byte((addr >> 8)&0xff);
	spi1_send_byte(addr & 0xff);
	//写入数据
	while(len--)
		spi1_send_byte(*arr++);	
	//片选引脚输出高电平，通信的结束
	PBout(14)=1;
	
	
	//检查是否已经写入完毕////////////////////////////
	sFLASH_WaitForWriteEnd();
	
	
	//写失能//////////////////////////////////////////
	//失能写保护	
	PBout(14) = 0;
	spi1_send_byte(0x04);
	PBout(14) = 1;
}

//读数据
void w25qxx_read_data(uint32_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	//片选引脚输出低电平，通信的开始
	PBout(14)=0;

	//通信的过程
	//1)发送0x03
	spi1_send_byte(0x03);
	
	//2)发送24bit，可以拆分为3个字节发送
	//addr=0x125566
	spi1_send_byte((addr>>16)&0xFF);//0x12
	spi1_send_byte((addr>>8)&0xFF);//0x55
	spi1_send_byte( addr&0xFF);//0x66	
	
	
	//3）读取数据,看时序图，发现MOSI引脚的电平是忽略，参数随意
	while(len--)
		*p++=spi1_send_byte(0xFF);	

	//片选引脚输出高电平，通信的结束
	PBout(14)=1;

}

//扇区擦除
void Sector_Erase(uint32_t addr)
{
	//解锁写保护,写使能//////////////////////////////
	//片选输出低电平
	PBout(14) = 0;
	//使能写保护
	spi1_send_byte(0x06);
	//片选输出高电平
	PBout(14) = 1;
	
	//进行清除扇区//////////////////////////////////
	//片选输出低电平
	PBout(14) = 0;
	//发送20h指令
	spi1_send_byte(0x20);
	//写入清除地址
	addr = (addr / 4096 )*4096;
	spi1_send_byte((addr >> 16)&0xff);
	spi1_send_byte((addr >> 8)&0xff);
	spi1_send_byte(addr & 0xff);
	//片选输出高电平
	PBout(14) = 1;
	
	//检查是否已经擦除完毕///////////////////////////////
	sFLASH_WaitForWriteEnd();
	
	//写失能//////////////////////////////////////////
	//失能写保护	
	PBout(14) = 0;
	spi1_send_byte(0x04);
	PBout(14) = 1;
}


//检查是否擦除完毕
void sFLASH_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    //片选引脚输出低电平，通信的开始
	PBout(14)=0;

    //发送0x05
    spi1_send_byte(0x05);

    //检查是否已经擦除完毕
    do
    {
      flashstatus = spi1_send_byte(0xA5);
	 }
    while ((flashstatus & 0x01) == SET); 

    //片选引脚输出高电平，通信的结束
	PBout(14)=1;
}


//模拟SPI--全双工同步接收
uint8_t simulate_spi1_send_byte(uint8_t txd)
{
	int32_t i=0;
	
	uint8_t d=0;
	for(i=7; i>=0; i--)
	{
		//判断txd变量对应的bit位是否1
		if(txd & (1<<i))
			PBout(5)=1;
		else
			PBout(5)=0;
		
		
		//触发一个下降沿
		PBout(3)=0; 
		delay_us(2);
		
		//读取MISO引脚的电平
		if(PBin(4))
			d|=1<<i;
			
		//触发一个上升沿，然后告诉从机，读取MISO引脚电平完毕
		PBout(3)=1; 
		delay_us(2);		
	
	}
	
	return d;
}

//模拟spi初始化
void simulate_w25qxx_init(void)
{
	
	//使能端口B的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//配置端口B PB3（SCLK） PB5(MOSI) PB14（SS）为输出模式模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_14;			//第3 5 14个引脚
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;							//输出模式
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;							//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;							//增加输出电流的能力
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;							//不需要上下拉电阻
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//配置端口B PB4(MISO)为输入模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;			//第4个引脚
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;		//输出模式

	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//【*】看时序图，只要是输出模式，初始状态要输出对应的电平
	PBout(14)=1;	//SS引脚
	PBout(3)=1;     //SCLK引脚
	PBout(5)=1;		//MOSI引脚，随意，因为时序图没有告诉实际上的电平，模式0~模式3没有描述MOSI引脚相关
	
	
}

