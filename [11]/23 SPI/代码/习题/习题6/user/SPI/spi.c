#include "spi.h"
#include "sys.h"
#include <stdio.h>
#include "./user/delay/delay.h"
static SPI_InitTypeDef 		SPI_InitStructure;
static GPIO_InitTypeDef		GPIO_InitStruct;


void w25qxx_init(void)
{
	
	//ʹ�ܶ˿�B��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	//ʹ��SPI1Ӳ��ʱ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	
	//���ö˿�B PB3~PB5Ϊ���ù���ģʽ
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
	
	//���ö˿�B PB14Ϊ���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	

	//��*����ʱ��ͼ��PB14�ĳ�ʼ״̬Ҫ����ߵ�ƽ
	
	PBout(14) = 1;
	
	//����SPI1��صĲ���
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//��*����ʱ��ͼ������ģʽ
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//��*����ʱ��ͼ��8������λ
	
	//��ʱ��ͼģʽ3
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;								//��*����ʱ��ͼ��ʱ���߿��е�ʱ����ʲô��ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;							//��*����ʱ��ͼ��MISO���Ųɼ���������ʱ�ӵĵڼ���������
	
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//����Ƭѡ���Ź���ʱ�䲻�̶�����Ҫ����������
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//��оƬ�ֲ��Ƿ�֧�ָ�Ƶ�ʣ�SPI1��Ӳ��ʱ��Ƶ��=84MHz/8=10.5MHz
	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//��*����ʱ��ͼ�������Чλ����
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	//ʹ��SPI1Ӳ��
	SPI_Cmd(SPI1, ENABLE);
	
}
	
//Ӳ��---ȫ˫��ͬ��ͨ��
uint8_t spi1_send_byte(uint8_t txd)
{

	SPI_I2S_SendData(SPI1,txd);
	
	//�ȴ��������
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);


	//������ȫ˫��ͬ��ͨ�ţ��ȴ��������
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==RESET);
	
	
	return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}



uint16_t w25qxx_read_id(void)
{
	//Ƭѡ��������͵�ƽ��ͨ�ŵĿ�ʼ
	uint16_t id = 0;
	PBout(14) = 0;
	
	//ͨ�ŵĹ���

	//1)����0x90
		spi1_send_byte(0x90);
	
	//2)����24bit�����Բ��Ϊ3���ֽڷ���
		spi1_send_byte(0x00);	
		spi1_send_byte(0x00);
		spi1_send_byte(0x00);	
	
	//��ȡ����id����������Ϊ�������
	id = spi1_send_byte(0xFF)<<8;
	
	//��ȡ�豸id
	id|= spi1_send_byte(0xFF);
	
	//Ƭѡ��������ߵ�ƽ��ͨ�ŵĽ���

	PBout(14) = 1;

	return id;
}
//д����
void write_to_w25q128(uint32_t addr,uint8_t len,uint8_t *buf)
{
	uint8_t *arr = buf;
	//��������
	Sector_Erase(addr);
	//����д����,дʹ��//////////////////////////////
	//Ƭѡ����͵�ƽ
	PBout(14) = 0;
	//ʹ��д����
	spi1_send_byte(0x06);
	//Ƭѡ����ߵ�ƽ
	PBout(14) = 1;
	
	
	//д������///////////////////////////////////////////
	//Ƭѡ��������͵�ƽ��ͨ�ŵĿ�ʼ
	PBout(14)=0;
	//д��ָ��02
	spi1_send_byte(0x02);
	//д��Ҫ�޸����ݵĵ�ַ
	spi1_send_byte((addr >> 16)&0xff);
	spi1_send_byte((addr >> 8)&0xff);
	spi1_send_byte(addr & 0xff);
	//д������
	while(len--)
		spi1_send_byte(*arr++);	
	//Ƭѡ��������ߵ�ƽ��ͨ�ŵĽ���
	PBout(14)=1;
	
	
	//����Ƿ��Ѿ�д�����////////////////////////////
	sFLASH_WaitForWriteEnd();
	
	
	//дʧ��//////////////////////////////////////////
	//ʧ��д����	
	PBout(14) = 0;
	spi1_send_byte(0x04);
	PBout(14) = 1;
}

//������
void w25qxx_read_data(uint32_t addr,uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	//Ƭѡ��������͵�ƽ��ͨ�ŵĿ�ʼ
	PBout(14)=0;

	//ͨ�ŵĹ���
	//1)����0x03
	spi1_send_byte(0x03);
	
	//2)����24bit�����Բ��Ϊ3���ֽڷ���
	//addr=0x125566
	spi1_send_byte((addr>>16)&0xFF);//0x12
	spi1_send_byte((addr>>8)&0xFF);//0x55
	spi1_send_byte( addr&0xFF);//0x66	
	
	
	//3����ȡ����,��ʱ��ͼ������MOSI���ŵĵ�ƽ�Ǻ��ԣ���������
	while(len--)
		*p++=spi1_send_byte(0xFF);	

	//Ƭѡ��������ߵ�ƽ��ͨ�ŵĽ���
	PBout(14)=1;

}

//��������
void Sector_Erase(uint32_t addr)
{
	//����д����,дʹ��//////////////////////////////
	//Ƭѡ����͵�ƽ
	PBout(14) = 0;
	//ʹ��д����
	spi1_send_byte(0x06);
	//Ƭѡ����ߵ�ƽ
	PBout(14) = 1;
	
	//�����������//////////////////////////////////
	//Ƭѡ����͵�ƽ
	PBout(14) = 0;
	//����20hָ��
	spi1_send_byte(0x20);
	//д�������ַ
	addr = (addr / 4096 )*4096;
	spi1_send_byte((addr >> 16)&0xff);
	spi1_send_byte((addr >> 8)&0xff);
	spi1_send_byte(addr & 0xff);
	//Ƭѡ����ߵ�ƽ
	PBout(14) = 1;
	
	//����Ƿ��Ѿ��������///////////////////////////////
	sFLASH_WaitForWriteEnd();
	
	//дʧ��//////////////////////////////////////////
	//ʧ��д����	
	PBout(14) = 0;
	spi1_send_byte(0x04);
	PBout(14) = 1;
}


//����Ƿ�������
void sFLASH_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;

    //Ƭѡ��������͵�ƽ��ͨ�ŵĿ�ʼ
	PBout(14)=0;

    //����0x05
    spi1_send_byte(0x05);

    //����Ƿ��Ѿ��������
    do
    {
      flashstatus = spi1_send_byte(0xA5);
	 }
    while ((flashstatus & 0x01) == SET); 

    //Ƭѡ��������ߵ�ƽ��ͨ�ŵĽ���
	PBout(14)=1;
}


//ģ��SPI--ȫ˫��ͬ������
uint8_t simulate_spi1_send_byte(uint8_t txd)
{
	int32_t i=0;
	
	uint8_t d=0;
	for(i=7; i>=0; i--)
	{
		//�ж�txd������Ӧ��bitλ�Ƿ�1
		if(txd & (1<<i))
			PBout(5)=1;
		else
			PBout(5)=0;
		
		
		//����һ���½���
		PBout(3)=0; 
		delay_us(2);
		
		//��ȡMISO���ŵĵ�ƽ
		if(PBin(4))
			d|=1<<i;
			
		//����һ�������أ�Ȼ����ߴӻ�����ȡMISO���ŵ�ƽ���
		PBout(3)=1; 
		delay_us(2);		
	
	}
	
	return d;
}

//ģ��spi��ʼ��
void simulate_w25qxx_init(void)
{
	
	//ʹ�ܶ˿�B��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//���ö˿�B PB3��SCLK�� PB5(MOSI) PB14��SS��Ϊ���ģʽģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_14;			//��3 5 14������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;							//���ģʽ
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;							//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;							//�����������������
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;							//����Ҫ����������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//���ö˿�B PB4(MISO)Ϊ����ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_4;			//��4������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;		//���ģʽ

	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	
	//��*����ʱ��ͼ��ֻҪ�����ģʽ����ʼ״̬Ҫ�����Ӧ�ĵ�ƽ
	PBout(14)=1;	//SS����
	PBout(3)=1;     //SCLK����
	PBout(5)=1;		//MOSI���ţ����⣬��Ϊʱ��ͼû�и���ʵ���ϵĵ�ƽ��ģʽ0~ģʽ3û������MOSI�������
	
	
}

