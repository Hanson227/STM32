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
	SysTick->CTRL = 0; 			// Disable SysTick，关闭系统定时器
	SysTick->LOAD = (168*n)-1; // 配置计数值(168*n)-1 ~ 0
	SysTick->VAL  = 0; 		// Clear current value as well as count flag
	SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
	while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	SysTick->CTRL = 0; 		// Disable SysTick	
}

void delay_ms(uint32_t n)
{
	while(n--)
	{
		SysTick->CTRL = 0; 				// Disable SysTick，关闭系统定时器
		SysTick->LOAD = (168000)-1; 	// 配置计数值(168000)-1 ~ 0
		SysTick->VAL  = 0; 		// Clear current value as well as count flag
		SysTick->CTRL = 5; 		// Enable SysTick timer with processor clock
		while ((SysTick->CTRL & 0x10000)==0);// Wait until count flag is set
	}
	
	SysTick->CTRL = 0; 		// Disable SysTick	

}

void usart1_init(uint32_t baud)
{
	
	//打开PA硬件时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	

	//打开串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//第9 10根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;	//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOA,&GPIO_InitStructure);


	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART1, &USART_InitStructure);
	
	
	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	//使能串口1工作
	USART_Cmd(USART1, ENABLE);
}

void dht11_init(void)
{

	//端口G硬件时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);	
	
	
	//配置PG9为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);		
	
	
	//PG9初始状态为高电平，看时序图
	PGout(9)=1;
}

int32_t dht11_read(uint8_t *pbuf)
{
	uint32_t t=0;
	int32_t i=0,j=0;
	uint8_t d=0;
	uint8_t *p=pbuf;
	uint32_t check_sum=0;
	
	//PG9设置为输出模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	
	
	PGout(9)=0;
	delay_ms(20);
	
	PGout(9)=1;
	delay_us(30);	
	
	
	//PG9设置为输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOG,&GPIO_InitStructure);	

	//等待低电平出现
	t=0;
	while(PGin(9))
	{
		t++;
		delay_us(1);
		if(t>=4000)
			return -1;
	}
	
	//测量低电平合法性
	t=0;
	while(PGin(9)==0)
	{
		t++;
		delay_us(1);
		if(t>=1000)
			return -2;
	}

	//测量高电平合法性
	t=0;
	while(PGin(9))
	{
		t++;	
		delay_us(1);
		if(t>=1000)
			return -3;
	}

	for(j=0; j<5; j++)
	{
		d=0;
		//一个字节的接收，从最高有效位开始接收
		for(i=7; i>=0; i--)
		{
		
			//等待数据0/数据1的前置低电平持续完毕
			t=0;
			while(PGin(9)==0)
			{
				t++;
				delay_us(1);			
				if(t>=1000)
					return -4;
			
			}

			//延时40us
			delay_us(40);
		
			//判断当前PG9是否为高电平还是低电平
			//若是高电平，则为数据1；
			//若是低电平，则为数据0；
			if(PGin(9))
			{
				d|=1<<i;//将变量d对应的比特位置1，如i=7，d|=1<<7就是将变量d的bit7置1
			
				//等待高电平持续完毕
				t=0;
				while(PGin(9))
				{
					t++;
					delay_us(1);
					if(t>=1000)
						return -5;
				}		
			}
		}
		
		p[j]=d;	
	}
	
	//校验和
	check_sum = (p[0]+p[1]+p[2]+p[3])&0xFF;
	
	if(p[4] != check_sum)
		return -6;
	
	return 0;
}

void adc_init(void)
{
	//打开ADC的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//打开PA的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	
	
	//配置PA6引脚为模拟信号引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//模拟信号模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//adc常规初始化
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;					//独立工作模式，只使用一个ADC
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;					//ADC硬件时钟=84MHz/2=42MHz,Tadc=1/42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;		//不需要映射数据，直接读取寄存器就可以得到结果值
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//5*Tadc
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;						//12位精度
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;								//单个通道，否则为多个通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//不断的进行转换
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;	//不需要内部脉冲触发adc工作
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//右对齐存储
	ADC_InitStructure.ADC_NbrOfConversion = 1;									//只有单个通道
	ADC_Init(ADC1, &ADC_InitStructure);

	//将通道6和ADC关联起来，优先级为1，采样点的时间为5*Tadc
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_3Cycles);
	
	//使能ADC1工作
	ADC_Cmd(ADC1, ENABLE);
}

void dac_init(void)
{
	//使能DAC硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
	
	
	//配置PA4引脚为模拟信号引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//模拟信号模式
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;		//引脚高速工作，收到指令立即工作；缺点：功耗高
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;		//增加输出电流的能力
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;		//不需要上下拉电阻	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	//配置DAC相关参数
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;					//不需要脉冲触发工作
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;		//不生成波形
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//允许输出电压
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	//使能DAC通道1工作
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

int main(void)
{
	uint16_t adc_val,adc_vol, adc_tmp;
	uint32_t sum=0;
	uint32_t i=0;
	uint16_t temp=0;
	uint16_t dac_vol;
	int32_t rt=0;
	uint8_t buf[5]={0};
	
	//使能(打开)端口F的硬件时钟，就是对端口F供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	//串口1波特率:115200bps
	usart1_init(115200);	

	//初始化GPIO引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//第9根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	//温湿度模块初始化
	dht11_init();
	
	
	//ADC初始化
	adc_init();
	
	//DAC初始化
	dac_init();
	
	
	
	//启动adc1的转换
	ADC_SoftwareStartConv(ADC1);
	
	printf("This is adc test\r\n");
	
	while(1)
	{
		rt = dht11_read(buf);
		
		if(rt == 0)
		{
			//printf("T:%d.%d,H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
			temp=buf[2];		// + buf[3]*0.1;
			//printf("temp:%d\r\n",temp);
			
			//公式转换，将温度转换成电压值
			dac_vol = temp*60*4095/3300;
			
			//dac通道1输出电压值dac_vol
			DAC_SetChannel1Data(DAC_Align_12b_R, dac_vol);
			delay_ms(100);
		}
		
		//adc读取到的100次电压累积值
		sum=0;
		for(i=0; i<100; i++)
		{
			//等待ADC1转换完毕
			while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
			ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
			
			
			//得到结果值
			adc_val=ADC_GetConversionValue(ADC1);
			
			//将结果值转换为电压值
			adc_vol = adc_val * 3300 /4095;	

			sum+=adc_vol;
		
		}
		//取平均值
		adc_vol=(sum/100);		//可以在后面添加-8，就是数据补偿
		//printf("adc_val:%d\r\n",adc_val);
		printf("adc_vol:%dmv\r\n",adc_vol);		
		
		//将电压值转换成温度
		adc_tmp = adc_vol/60;
		printf("adc_tmp:%d℃\r\n",adc_tmp);
		
		delay_ms(1000);
		
	}
}



void USART1_IRQHandler(void)
{
	uint8_t d;
	
	//检测标志位
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//接收数据
		d=USART_ReceiveData(USART1);
		
		
	
		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}

}




