#include "adc.h"


static ADC_InitTypeDef       ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

void adc1_init(void)
{
	//ʹ��ADC1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	//ʹ��GPIOA��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//��PA5��������Ϊģ���ź�����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ADCͨ�ó�ʼ�� */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADCӲ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//�ر�DMAֱ�Ӵ洢����
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//����������ļ��5*��1/42MHz��
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1�ĳ�ʼ�� */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//���12bit����							
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//ɨ��1��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//���ϵؽ���ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//����Ҫ�ⲿ���崥��ADC����
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//�Ҷ���洢����
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//����ADC1�ж��ٸ�����ͨ������ת��
	
	ADC_Init(ADC1, &ADC_InitStructure);
		/* ����ADC1��ͨ��5�����ȼ�Ϊ1����ĳ���������ʱ��=3*��1/42MHz��*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_3Cycles);
	
	//ʹ��ADC
	ADC_Cmd(ADC1,ENABLE);

}

void adc3_init(void)
{
	//ʹ��ADC1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3,ENABLE);
	
	//ʹ��GPIOA��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	//��PF7��������Ϊģ���ź�����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	
	//����ADC��صĲ���
	/* ADCͨ�ó�ʼ�� */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADCӲ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//�ر�DMAֱ�Ӵ洢����
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//����������ļ��5*��1/42MHz��
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC3�ĳ�ʼ�� */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//���12bit����							
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//ɨ��1��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//���ϵؽ���ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//����Ҫ�ⲿ���崥��ADC����
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//�Ҷ���洢����
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//����ADC1�ж��ٸ�����ͨ������ת��
	
	
	ADC_Init(ADC3, &ADC_InitStructure);
	
	/* ����ADC3��ͨ��5�����ȼ�Ϊ1����ĳ���������ʱ��=3*��1/42MHz��*/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	//ʹ��ADC
	ADC_Cmd(ADC3,ENABLE);
}

	
