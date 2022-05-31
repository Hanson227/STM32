#include "adc.h"


static ADC_InitTypeDef       ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

void adc1_init(void)
{
	//使能ADC1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	//使能GPIOA的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	//将PA5引脚配置为模拟信号输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	
	/* ADC通用初始化 */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADC硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//关闭DMA直接存储访问
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样点的间隔5*（1/42MHz）
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC1的初始化 */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//输出12bit数据							
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//扫描1个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//不断地进行转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//不需要外部脉冲触发ADC工作
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//右对齐存储数据
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//告诉ADC1有多少个输入通道进行转换
	
	ADC_Init(ADC1, &ADC_InitStructure);
		/* 配置ADC1的通道5，优先级为1，对某个点采样的时间=3*（1/42MHz）*/
	ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_3Cycles);
	
	//使能ADC
	ADC_Cmd(ADC1,ENABLE);

}

void adc3_init(void)
{
	//使能ADC1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3,ENABLE);
	
	//使能GPIOA的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	//将PF7引脚配置为模拟信号输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Init(GPIOF,&GPIO_InitStructure);	
	
	//配置ADC相关的参数
	/* ADC通用初始化 */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//独立工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;						//ADC硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;			//关闭DMA直接存储访问
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;	//两个采样点的间隔5*（1/42MHz）
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	/* ADC3的初始化 */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//输出12bit数据							
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//扫描1个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;								//不断地进行转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//不需要外部脉冲触发ADC工作
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//右对齐存储数据
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//告诉ADC1有多少个输入通道进行转换
	
	
	ADC_Init(ADC3, &ADC_InitStructure);
	
	/* 配置ADC3的通道5，优先级为1，对某个点采样的时间=3*（1/42MHz）*/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_3Cycles);
	
	//使能ADC
	ADC_Cmd(ADC3,ENABLE);
}

	
