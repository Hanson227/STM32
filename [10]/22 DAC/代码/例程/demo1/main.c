#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>


static GPIO_InitTypeDef 		GPIO_InitStructure;
static NVIC_InitTypeDef 		NVIC_InitStructure;
static USART_InitTypeDef 		USART_InitStructure;


static ADC_InitTypeDef       ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

static DAC_InitTypeDef       DAC_InitStructure;

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

void adc_init(void)
{
	//��ADC��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//��PA��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	
	
	//����PA6����Ϊģ���ź�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//adc�����ʼ��
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					//��������ģʽ��ֻʹ��һ��ADC
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;					//ADCӲ��ʱ��=84MHz/2=42MHz,Tadc=1/42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;		//����Ҫӳ�����ݣ�ֱ�Ӷ�ȡ�Ĵ����Ϳ��Եõ����ֵ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//5*Tadc
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						//12λ����
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//����ͨ��������Ϊ���ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//���ϵĽ���ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//����Ҫ�ڲ����崥��adc����
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//�Ҷ���洢
	ADC_InitStructure.ADC_NbrOfConversion = 1;									//ֻ�е���ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);

	//��ͨ��6��ADC�������������ȼ�Ϊ1���������ʱ��Ϊ5*Tadc
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_3Cycles);
	
	//ʹ��ADC1����
	ADC_Cmd(ADC1, ENABLE);
}

void dac_init(void)
{
	//ʹ��DACӲ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	
	
	//����PA4����Ϊģ���ź�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//���Ÿ��ٹ������յ�ָ������������ȱ�㣺���ĸ�
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//�����������������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//����Ҫ����������	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//����DAC��ز���
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;					//����Ҫ���崥������
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;		//�����ɲ���
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//���������ѹ
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	//ʹ��DACͨ��1����
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

int main(void)
{
	uint16_t adc_val,adc_vol;
	uint32_t sum=0;
	uint32_t i=0;
	
	
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
	
	
	
	printf("This is adc test\r\n");
	
	
	adc_init();
	
	dac_init();
	
	//dacͨ��1�����ѹֵ1650mv
	DAC_SetChannel1Data(DAC_Align_12b_R, 1800*4095/3300);
	delay_ms(100);
	
	//����adc1��ת��
	ADC_SoftwareStartConv(ADC1);
	
	
	while(1)
	{
		
		sum=0;
		for(i=0; i<100; i++)
		{
			//�ȴ�ADC1ת�����
			while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
			ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
			
			
			//�õ����ֵ
			adc_val=ADC_GetConversionValue(ADC1);
			
			//�����ֵת��Ϊ��ѹֵ
			adc_vol = adc_val * 3300 /4095;	

			sum+=adc_vol;
		
		}
		
		adc_vol=(sum/100)-8;//-8�������ݲ���
		
		
		//printf("adc_val:%d\r\n",adc_val);
		printf("adc_vol:%dmv\r\n",adc_vol);		
		
		delay_ms(1000);
		
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




