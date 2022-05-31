#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>
#include "oled.h"
#include "bmp.h"

static GPIO_InitTypeDef		GPIO_InitStructure;
static USART_InitTypeDef	USART_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;


void delay_us(uint32_t nus)
{
	SysTick->CTRL = 0; 					// Disable SysTick
	SysTick->LOAD = (nus*21)-1; 		// Count from (nus*21)-1 to 0 ((nus*21) cycles)
	SysTick->VAL = 0; 					// Clear current value as well as count flag
	SysTick->CTRL = 1; 					// Enable SysTick timer with processor clock/8
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 					// Disable SysTick		
	

}


void delay_ms(uint32_t nms)
{
	uint32_t t = nms;


	while(t--)
	{
		SysTick->CTRL = 0; 			// Disable SysTick
		SysTick->LOAD = 21000-1; 	// Count from 20999 to 0 (21000 cycles)
		SysTick->VAL = 0; 			// Clear current value as well as count flag
		SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock/8
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}	
	
	
	SysTick->CTRL = 0; // Disable SysTick	
}

void usart1_init(uint32_t baud)
{
	//ʹ�ܴ���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	
	//ʹ�ܶ˿�A��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//����PA9 PA10Ϊ���ù���ģʽ
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;	//��9 10������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;		//���ù���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;	//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;	//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//��PA9��PA10���ӵ�����1Ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);		

	//���ô���1�������ʣ�У��λ������λ��ֹͣλ
	USART_InitStructure.USART_BaudRate = baud;						//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ȡ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�ܹ��շ�����
	USART_Init(USART1,&USART_InitStructure);
	
	
	//���ô���1���жϴ�����ʽ���������ֽڴ����ж�
	USART_ITConfig(USART1,USART_IT_RXNE , ENABLE);
	
	
	//ʹ�ܴ���1����	
	USART_Cmd(USART1, ENABLE);
	
	
	//����NVIC������1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//����1���жϺ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//��ռ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 0x2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//NVICʹ�ܴ���1�ж�����ͨ��
	NVIC_Init(&NVIC_InitStructure);	
}

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) 
{

	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	
	return(ch);
}



//������
int main(void)
{

	u8 t;

	
	//ʹ�ܶ˿�F��Ӳ��ʱ�ӣ��˿�F���ܹ�����˵���˾��ǶԶ˿�F�ϵ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);	


	
	//����Ӳ��������GPIO���˿�F����9������
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;			//��9 ������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	
	//����1������Ϊ115200bps
	usart1_init(115200);
	
	printf("This is spi oled test\r\n");
	
	//���Խ�������ĵ���stm32f4xx_dsp_stdperiph_lib_um.chm��
	OLED_Init();			//��ʼ��OLED  
	OLED_Clear()  	; 
		/*
		 OLED_ShowString(30,0,"OLED TEST");
		OLED_ShowString(8,2,"ZHONGJINGYUAN");  
		OLED_ShowString(20,4,"2014/05/01");  
		OLED_ShowString(0,6,"ASCII:");  
		OLED_ShowString(63,6,"CODE:");  
		t=' '; 
		OLED_ShowCHinese(0,0,0);//��
		OLED_ShowCHinese(18,0,1);//��
		OLED_ShowCHinese(36,0,2);//԰
		OLED_ShowCHinese(54,0,3);//��
		OLED_ShowCHinese(72,0,4);//��
		OLED_ShowCHinese(90,0,5);//��
		OLED_ShowCHinese(108,0,6);//��
		*/	
	t=' ';
	while(1) 
	{		
		OLED_Clear();
		OLED_ShowCHinese(0,0,0);//��
		OLED_ShowCHinese(18,0,1);//��
		OLED_ShowCHinese(36,0,2);//԰
		OLED_ShowCHinese(54,0,3);//��
		OLED_ShowCHinese(72,0,4);//��
		OLED_ShowCHinese(90,0,5);//��
		OLED_ShowCHinese(108,0,6);//��
		OLED_ShowString(0,3,"1.3' OLED TEST");
		//OLED_ShowString(8,2,"ZHONGJINGYUAN");  
	 //	OLED_ShowString(20,4,"2014/05/01");  
		OLED_ShowString(0,6,"ASCII:");  
		OLED_ShowString(63,6,"CODE:");  
		OLED_ShowChar(48,6,t);//��ʾASCII�ַ�	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,6,t,3,16);//��ʾASCII�ַ�����ֵ 	
			
		
		delay_ms(8000);
		OLED_Clear();
		delay_ms(8000);
		OLED_DrawBMP(0,0,128,8,BMP1);  //ͼƬ��ʾ(ͼƬ��ʾ���ã����ɵ��ֱ�ϴ󣬻�ռ�ý϶�ռ䣬FLASH�ռ�8K��������)
		delay_ms(8000);
		OLED_DrawBMP(0,0,128,8,BMP2);
		delay_ms(8000);
	}	  
}

void  USART1_IRQHandler(void)
{
	uint8_t d;

	//����־λ
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		
		//��������
		d = USART_ReceiveData(USART1);
		
		//��ձ�־λ	
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}
