#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 	GPIO_InitStructure;





int main(void)
{
	
	//ʹ��(��)�˿�E��Ӳ��ʱ�ӣ����ǶԶ˿�E����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		//��13 14������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOE,&GPIO_InitStructure);	
	
	PFout(9)=1;
	
	
	//ϵͳ��ʱ����ʱ��Դ168MHz
	//ֻҪϵͳ��ʱ������168000000���������ʹ���1����ʱ��ĵ���
	//ֻҪϵͳ��ʱ������168000000/10���������ʹ���1��/10��ʱ��ĵ���	
	//ֻҪϵͳ��ʱ������168000000/100���������ʹ���1��/100��ʱ��ĵ���	
#if 1	
	//����ϵͳ��ʱ������1000Hz���ж�
	//����ʱ��T=1/f=1/1000Hz=1ms
	
	SysTick_Config(SystemCoreClock / 1000);
#endif

#if 0
	//����ϵͳ��ʱ������100Hz���ж�
	//����ʱ��T=1/f=1/100Hz=10ms
	//SysTick_Config(SystemCoreClock / 100);
	SysTick_Config(1680000);
	

#endif
	while(1)
	{
		

	}
}

void SysTick_Handler(void)
{
static uint32_t led1_cnt=0;
static uint32_t led2_cnt=0;
static uint32_t led3_cnt=0;	
static uint32_t led4_cnt=0;	

	
	led1_cnt++;
	led2_cnt++;	
	led3_cnt++;	
	led4_cnt++;		
	
	if(led1_cnt >= 100)
	{
		led1_cnt=0;
		
		//��״̬�ķ�ת
		PFout(9)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}

	if(led2_cnt >= 330)
	{
		led2_cnt=0;
		
		//��״̬�ķ�ת
		PFout(10)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}
	
	
	if(led3_cnt >= 1500)
	{
		led3_cnt=0;
		
		//��״̬�ķ�ת
		PEout(13)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}	
	
	if(led4_cnt >= 2200)
	{
		led4_cnt=0;
		
		//��״̬�ķ�ת
		PEout(14)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}		

}
