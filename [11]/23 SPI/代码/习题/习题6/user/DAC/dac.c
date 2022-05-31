#include "dac.h"
#include <stdio.h>
#include "sys.h"
#include "./user/delay/delay.h"

static GPIO_InitTypeDef		    GPIO_InitStructure;
static DAC_InitTypeDef       	DAC_InitStructure;

static ADC_InitTypeDef       ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

void dac_init(void)
{
	//ʹ��DAC��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	//ʹ��GPIOA��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//��PA2��������Ϊģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;			//��2������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;			//ģ���ź�ģʽ
	//GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	

	//DAC����
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;					//����Ҫ���崥�������ѹ
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;		//��������Σ�ֻͨ����������ɵ���ѹ
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//���������ѹ
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	//DACͨ��1Ӳ��ʹ��
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void adc_init(void)
{
	//ʹ��ADC1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	
	//ʹ��GPIOA��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	//��PA3��������Ϊģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;			//��3������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;			//ģ���ź�ģʽ
	//GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;	//����Ҫ����������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	//����ADC��صĲ���
	/* ADCͨ�ó�ʼ�� */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//��������
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADCӲ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//�ر�DMAֱ�Ӵ洢����
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//����������ļ��5*��1/42MHz��
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1�ĳ�ʼ�� */
	
#if 1	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//���12bit����
#else
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;							//���10bit����
#endif

	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//ɨ��1��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//���ϵؽ���ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//����Ҫ�ⲿ���崥��ADC����
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//�Ҷ���洢����
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//����ADC1�ж��ٸ�����ͨ������ת��
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ����ADC1��ͨ��3�����ȼ�Ϊ1����ĳ���������ʱ��=3*��1/42MHz��*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_3Cycles);
	
	
	//ʹ��ADC����
	ADC_Cmd(ADC1, ENABLE);	
}

