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
	//使能DAC的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

	//使能GPIOA的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//将PA2引脚配置为模拟信号模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;			//第2个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;			//模拟信号模式
	//GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	

	//DAC配置
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;					//不需要脉冲触发输出电压
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;		//不输出波形，只通过代码输出可调电压
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//允许输出电压
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	//DAC通道1硬件使能
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void adc_init(void)
{
	//使能ADC1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	
	//使能GPIOA的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	//将PA3引脚配置为模拟信号模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;			//第3个引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;			//模拟信号模式
	//GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;	//不需要上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	
	//配置ADC相关的参数
	/* ADC通用初始化 */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADC硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//关闭DMA直接存储访问
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样点的间隔5*（1/42MHz）
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1的初始化 */
	
#if 1	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//输出12bit数据
#else
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;							//输出10bit数据
#endif

	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//扫描1个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//不断地进行转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//不需要外部脉冲触发ADC工作
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//右对齐存储数据
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//告诉ADC1有多少个输入通道进行转换
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* 配置ADC1的通道3，优先级为1，对某个点采样的时间=3*（1/42MHz）*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_3Cycles);
	
	
	//使能ADC工作
	ADC_Cmd(ADC1, ENABLE);	
}

