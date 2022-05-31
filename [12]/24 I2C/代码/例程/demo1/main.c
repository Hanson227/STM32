#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;


#pragma import(__use_no_semihosting_swi)
struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	USART_ClearFlag(USART1,USART_FLAG_TXE);
	
	
	return ch;
}
void _sys_exit(int return_code) {

}

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

void usart1_init(uint32_t baud)
{
	
	//��PAӲ��ʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//�򿪴���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//����PA9��PA10Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//�๦��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//��PA9��PA10�������ӵ�����1��Ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//���ô���1��ز����������ʡ���У��λ��8λ����λ��1��ֹͣλ......
	USART_InitStructure.USART_BaudRate = baud;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����շ�����
	USART_Init(USART1, &USART_InitStructure);
	
	
	//���ô���1���жϴ�������������һ���ֽڴ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//���ô���1���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//ʹ�ܴ���1����
	USART_Cmd(USART1, ENABLE);
}
#define SCL_W	PBout(8)
#define SDA_W	PBout(9)
#define SDA_R	PBin(9)


void at24c02_init(void)
{
	//PBӲ��ʱ��ʹ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	
	//����PB8 PB9Ϊ���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;		//��8 9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	//��ʱ��ͼ
	PBout(8)=1;
	
	//��ʱ��ͼ	
	PBout(9)=1;	
}

void sda_pin_mode(GPIOMode_TypeDef pin_mode)
{

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= pin_mode;	//���/����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

void i2c_start(void)
{
	//��֤SDA����Ϊ���ģʽ
	sda_pin_mode(GPIO_Mode_OUT);
	
	SCL_W=1;
	SDA_W=1;
	delay_us(5);

	SDA_W=0;
	delay_us(5);
	
	
	SCL_W=0;
	delay_us(5);		//ǯסI2C����
}

void i2c_stop(void)
{
	//��֤SDA����Ϊ���ģʽ
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=1;
	SDA_W=0;
	delay_us(5);

	SDA_W=1;
	delay_us(5);
}


void i2c_send_byte(uint8_t byte)
{
	int32_t i;
	
	//��֤SDA����Ϊ���ģʽ
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=0;
	SDA_W=0;
	delay_us(5);

	for(i=7; i>=0; i--)
	{
		//�ȸ���SDA���ŵĵ�ƽ
		if(byte & (1<<i))
		{
			SDA_W=1;
		
		}
		else
		{
			SDA_W=0;
		}
		
		delay_us(5);
		
		SCL_W=1;
		delay_us(5);
		
		
		SCL_W=0;
		delay_us(5);	
	}
}

void i2c_ack(uint8_t ack)
{

	
	//��֤SDA����Ϊ���ģʽ
	sda_pin_mode(GPIO_Mode_OUT);	
	
	SCL_W=0;
	SDA_W=0;
	delay_us(5);


	//�ȸ���SDA���ŵĵ�ƽ
	if(ack)
	{
		SDA_W=1;
	
	}
	else
	{
		SDA_W=0;
	}
	
	delay_us(5);
	
	SCL_W=1;
	delay_us(5);
	
	
	SCL_W=0;
	delay_us(5);	
}

uint8_t i2c_wait_ack(void)
{
	uint8_t ack=0;
	
	//��֤SDA����Ϊ��INģʽ
	sda_pin_mode(GPIO_Mode_IN);	
	SCL_W=1;
	delay_us(5);
	
	if(SDA_R)
		ack=1;	//��Ӧ���ź�
	else
		ack=0;	//��Ӧ���ź�

	SCL_W=0;	//������æ��״̬
	delay_us(5);
	
	return ack;
}

uint8_t i2c_recv_byte(void)
{
	uint8_t d=0;
	
	int32_t i;
	
	//��֤SDA����Ϊ��INģʽ
	sda_pin_mode(GPIO_Mode_IN);	
	
	for(i=7; i>=0; i--)
	{
		SCL_W=1;
		delay_us(5);
		
		if(SDA_R)
			d|=1<<i;	
		

		SCL_W=0;	
		delay_us(5);	
	
	}

	
	return d;
}


int32_t at24c02_write(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack;
	uint8_t *p = pbuf;

	//������ʼ�źţ�ͨ�ŵĿ�ʼ
	i2c_start();
	
	//�����豸��ַ������Ѱַ
	i2c_send_byte(0xA0);

	//�ȴ�Ӧ���ź�
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address fail\r\n");
		
		return -1;
	}


	//����Ҫд�����ݵĵ�ַ
	i2c_send_byte(addr);

	//�ȴ�Ӧ���ź�
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}
	
	
	while(len--)
	{
	
		//����Ҫд������
		i2c_send_byte(*p++);

		//�ȴ�Ӧ���ź�
		ack = i2c_wait_ack();
		
		if(ack)
		{
			printf("data fail\r\n");
			
			return -3;
		}	
	}
	
	
	//����ֹͣ�ź�
	i2c_stop();
	
	printf("write suceess\r\n");
	
	return 0;
}

int32_t at24c02_read(uint8_t addr,uint8_t *pbuf,uint8_t len)
{
	uint8_t ack;
	uint8_t *p = pbuf;

	//������ʼ�źţ�ͨ�ŵĿ�ʼ
	i2c_start();
	
	//�����豸��ַ������Ѱַ
	i2c_send_byte(0xA0);

	//�ȴ�Ӧ���ź�
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address write access fail\r\n");
		
		return -1;
	}


	//����Ҫ��ȡ���ݵĵ�ַ
	i2c_send_byte(addr);

	//�ȴ�Ӧ���ź�
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("word address fail\r\n");
		
		return -2;
	}
	
	//���·�����ʼ�źţ��µ�ͨ�ſ�ʼ
	i2c_start();
	
	//�����豸��ַ������Ѱַ����ǰ�Ƕ�����
	i2c_send_byte(0xA1);

	//�ȴ�Ӧ���ź�
	ack = i2c_wait_ack();
	
	if(ack)
	{
		printf("device address read access fail\r\n");
		
		return -3;
	}	
	
	len = len-1;
	
	while(len--)
	{
	
		//��������
		*p=i2c_recv_byte();
		
		p++;

		//����Ӧ���źŸ��ӻ������ߴӻ��Ѿ������Լ��ɹ�
		i2c_ack(0);
	}
	
	//�������һ������
	*p=i2c_recv_byte();	
	
	//������Ӧ���źŸ��ӻ������ߴӻ����ٽ�������
	i2c_ack(1);	
	
	//����ֹͣ�ź�
	i2c_stop();
	
	printf("read suceess\r\n");
	
	return 0;
}

int main(void)
{
	int32_t i=0;
	uint8_t buf[8]={0};
	
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//����1������:115200bps
	usart1_init(115200);	

	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	
	//at240c2��ʼ��
	at24c02_init();
	
	
	//��0��ַд��8���ֽڵ�����
	memset(buf,0x88,8);
	
	at24c02_write(0,buf,8);
	
	delay_ms(100);
	
	memset(buf,0,8);
	at24c02_read(0,buf,8);
	
	for(i=0; i<8; i++)
	{
		printf("%02X ",buf[i]);
	
	}
	printf("\r\n");

	
	while(1)
	{
		
		
	}
}



void USART1_IRQHandler(void)
{
	uint8_t d;
	
	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//��������
		d=USART_ReceiveData(USART1);
		
		
	
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}




