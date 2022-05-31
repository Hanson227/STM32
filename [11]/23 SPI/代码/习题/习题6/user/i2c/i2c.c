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

//��ʼ�����ſ�
void at24cxx_init(void)
{
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//���ö˿�B PB8 PB9Ϊ���ģʽģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;			//��8 9������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;				//���ģʽ
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;				//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;				//�����������������
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;				//����Ҫ����������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	SCL_W = 1;
	SDA_W = 1;
	
		
}
//���������ͬģʽ������
void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{
	//����GPIOӲ����Ϣ
	GPIO_InitStruct.GPIO_Pin 	= GPIO_Pin_9; 		//��9������
	GPIO_InitStruct.GPIO_Mode	= pin_mode;			//����/���ģʽ
	GPIO_InitStruct.GPIO_OType 	= GPIO_OType_PP;	//�����������
	GPIO_InitStruct.GPIO_Speed 	= GPIO_Speed_100MHz;//���ŵ���Ӧ�ٶ�Ϊ100MHz
	GPIO_InitStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;	//���������������
	
	//��ʼ��GPIO
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}
	

//��ʼ�ź�
void i2c_start(void)
{
	sda_pin_mode(GPIO_Mode_OUT);
	//��ʼǰ��Ϊ�ߵ�ƽ
	SCL_W=1;
	SDA_W=1;
	delay_us(5);
	//�������ɸ߱�ͱ�ʾ��ʼ
	SDA_W=0;
	delay_us(5);

	SCL_W=0;
	delay_us(5);
}

//ֹͣ�ź�
void i2c_stop(void)
{
	sda_pin_mode(GPIO_Mode_OUT);		//����Ϊ���ģʽ
	//ֹͣǰ��Ϊ�͵�ƽ
	SCL_W=0;
	SDA_W=0;
	
	delay_us(5);
	
	//ʱ�������ø�
	SCL_W=1;
	
	delay_us(5);
	
	//�������ɵ����ߣ���ʾ�ź�ֹͣ
	SDA_W=1;
	
	delay_us(5);
}
//��������/��ַ
void i2c_send_byte(uint8_t txd)
{
	int32_t i=0;
	
	sda_pin_mode(GPIO_Mode_OUT);	//����Ϊ���ģʽ
	
	for(i=7; i>=0; i--)
	{
		//��ȡ����
		if(txd & (1<<i))
			SDA_W = 1;
		else
			SDA_W = 0;	
		
		//�Ƚ��������øߵ�ƽ
		SCL_W = 1;
		delay_us(5);	
		//�ٽ��������õ͵�ƽ�����һ��λ�Ĵ���
		SCL_W = 0;
		delay_us(5);	
	}
}
//�ȴ�Ӧ���ź�
uint8_t i2c_wait_ack(void)
{
	uint8_t  ack=0;
	
	sda_pin_mode(GPIO_Mode_IN);	//sda�����߱���Ϊ����ģʽ�����մӻ���Ӧ���ź�
	
	//���������øߵ�ƽ
	SCL_W = 1;
	delay_us(5);	
	
	if(SDA_R)
		ack=1;	//��Ӧ��
	else
		ack=0;	//��Ӧ��

	SCL_W=0;
	delay_us(5);	
	
	return ack;
}
//��ģʽʱ������Ӧ���ź�
void i2c_ack(uint8_t ack)
{
	sda_pin_mode(GPIO_Mode_OUT);	//����Ϊ���ģʽ

	//��ȡ����
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
	
	
	//���������øߵ�ƽ
	for(i=7; i>=0; i--)
	{
		sda_pin_mode(GPIO_Mode_IN);//sda�����߱���Ϊ����ģʽ�����մӻ���Ӧ���ź�
		SCL_W=1;
		delay_us(5);	
		
		if(SDA_R)
			d|=1<<i;

		SCL_W=0;
		delay_us(5);	
	}

	return d;
}

//д����
int32_t at24cxx_write(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	uint8_t *p=pbuf;
	uint8_t len_old=len;
	
	i2c_start();			//���Ϳ�ʼ�ź�
	
	i2c_send_byte(0xA0);	//�����豸Ѱַ��E2PROM��at24c02����ַ0x1010 0000
	
	ack = i2c_wait_ack();	//�ȴ�Ӧ���ź�
	
	if(ack)					//��Ӧ������ֹ�ź�
	{
		i2c_stop();
		printf("device address with write access fail\r\n");
		return -1;
	}
	
	i2c_send_byte(addr);	//����Ҫд��ĵ�ַ
	
	ack=i2c_wait_ack();		//��������Ƿ��дӻ�������Ӧ���ź�
	
	if(ack)	//��Ӧ��
	{
		i2c_stop();
		printf("work address fail\r\n");
		return -2;
	}
	
	while(len--)			//д������
	{
		//����Ҫд�������
		i2c_send_byte(*p++);

		ack = i2c_wait_ack();	//����Ƿ���Ӧ��
		
		if(ack)
		{
			i2c_stop();
			printf("data  fail,have write %d bytes\r\n",len_old-len);
			return -3;
		}
	}
	
	i2c_stop();			//����ֹͣ�ź�
		
	return 0;
		
}


int32_t at24cxx_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack=0;
	uint8_t *p=pbuf;
	
	//������ʼ�ź�
	i2c_start();
	
	//�����豸Ѱַ��Ѱ��at24c02���豸
	i2c_send_byte(0xA0);
	
	//��������Ƿ��дӻ�������Ӧ���ź�
	ack=i2c_wait_ack();
	
	if(ack)	//��Ӧ��
	{
		i2c_stop();
		
		printf("device address with write access fail\r\n");
		return -1;
	}
	
	//����Ҫ��ȡ���ݵ��׵�ַ
	i2c_send_byte(addr);
	
	//��������Ƿ��дӻ�������Ӧ���ź�
	ack=i2c_wait_ack();
	
	if(ack)	//��Ӧ��
	{
		i2c_stop();
		
		printf("word address fail\r\n");
		return -2;
	}
	
	
	//��һ�η�����ʼ�ź�-----------------------------------��ģʽ
	i2c_start();
	
	//���ж������豸Ѱַ��Ѱ��at24c02���豸
	i2c_send_byte(0xA1);
	
	//��������Ƿ��дӻ�������Ӧ���ź�
	ack=i2c_wait_ack();
	
	if(ack)	//��Ӧ��
	{
		i2c_stop();
		
		printf("device address with read access fail\r\n");
		
		return -3;

	}	
	
	
	//������ȡ����
	len = len-1;
	
	while(len--)
	{
		//��ȡ����
		*p++=i2c_recv_byte();
		
		//ÿ��ȡһ���ֽڣ�������Ӧ���źŸ����ӻ�
		i2c_ack(0);
	}
	
	//��ȡ��������
	*p=i2c_recv_byte();
		
	//������Ӧ���źŸ����ӻ�
	i2c_ack(1);	
	
	
	//ͨ�Ž���
	i2c_stop();
	
	
	return 0;

}


