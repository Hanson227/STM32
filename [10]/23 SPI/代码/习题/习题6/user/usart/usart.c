#include "usart.h"
#include "sys.h"
#include <stdio.h>


int fputc(int ch,FILE *file)
{
	USART_SendData(USART1,ch);
	//�ȴ��������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

	return ch;
}

void usart1_init(uint32_t baud)
{

	//�˿�AӲ��ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);	
	
	//����1Ӳ��ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	


	//����Ӳ��������GPIO���˿�A����9 10����
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;//��9 10����
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;			//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//��PA9�������ӵ�����1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);

	//��PA10�������ӵ�����1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	USART_InitStructure.USART_BaudRate = baud;					//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//����Ҫ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�����ڽ��պͷ�������
	USART_Init(USART1, &USART_InitStructure);

	//�������ݺ󴥷��ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* ���ô���1 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	USART_Cmd(USART1, ENABLE);
}

void usart3_init(uint32_t baud)
{

	//�򿪴���3��Ӳ��ʱ�ӣ����ǹ���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	
	//�򿪶˿�B��Ӳ��ʱ�ӣ����ǹ���
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	
	
	//��������
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10|GPIO_Pin_11; 		//��10 11������
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF;					//���ù���ģʽ�������Ž�������Ӳ�������Զ�����
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;				//�����������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_100MHz;			//���ŵ���Ӧ�ٶ�Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;				//���������������
	
	//��ʼ��GPIO
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//��PB10��PB11���ӵ�USART1Ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);	

	//��ʼ������
	USART_InitStructure.USART_BaudRate = baud;						//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //������պͷ���
	
	USART_Init(USART3,&USART_InitStructure);
	
	
	//���ô��ڽ����жϣ�Ҳ���ǽ��յ��ֽھʹ����жϣ�֪ͨCPU��������
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	//�ж����ȼ�������
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	//ʹ�ܴ��ڹ���
	USART_Cmd(USART3, ENABLE);


}


void usart3_send_str(const char *pstr)
{
	const char *p = pstr;
	
	//��⵱ǰp��ָ����Ч��
	//*p����Ƿ�Ϊ��0
	while(p && *p)
	{
	
		//����һ���ֽ�
		USART_SendData(USART3,*p);
	
		//�ȴ��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);	
		
		//��ַƫ��1���ֽ�
		p++;
		
	
	}
}

