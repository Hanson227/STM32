#include "stm32f4xx.h"
#include "sys.h"


static GPIO_InitTypeDef 	GPIO_InitStructure;





int main(void)
{
	
		
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;		//��9������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	PFout(9)=1;
	
	
	//ϵͳ��ʱ����ʱ��Դ168MHz
	//ֻҪϵͳ��ʱ������168000000���������ʹ���1����ʱ��ĵ���
	//ֻҪϵͳ��ʱ������168000000/10���������ʹ���1��/10��ʱ��ĵ���	
	//ֻҪϵͳ��ʱ������168000000/100���������ʹ���1��/100��ʱ��ĵ���	
#if 0	
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

	//����ϵͳ��ʱ������10Hz���жϣ������⣬���ǲ���
	//����ʱ��T=1/f=1/10Hz=100ms
	SysTick_Config(SystemCoreClock / 10);
	
	while(1)
	{
		

	}
}

void SysTick_Handler(void)
{
static 
	uint32_t i=0;

	i++;

	if(i>=5)
	{
		i=0;
		
		//��״̬�ķ�ת
		PFout(9)^=1;	//1^0=1 1^1=0 0^1=1 1^1=0 ...
	}



}
