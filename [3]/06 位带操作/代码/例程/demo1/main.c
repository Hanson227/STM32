#include "stm32f4xx.h"


static GPIO_InitTypeDef GPIO_InitStructure;

void delay(void)
{
	uint32_t i=0x2000000;
	
	while(i--);
}

/*
#define GPIOF               	((GPIO_TypeDef *) GPIOF_BASE)
#define GPIOF_BASE            	(AHB1PERIPH_BASE + 0x1400)
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
#define PERIPH_BASE           ((uint32_t)0x40000000)

�˿�F��ODR�Ĵ�����ַ = 0x40000000+0x20000+0x1400+0x14=0x40021414;

ӳ���ַ�����������ַ=0x42000000+(0x40021414-0x40000000)*32+9*4=0x404282A4

*/

#define PEout(n)	*(uint32_t *)(0x42000000+((uint32_t)&GPIOE->ODR - 0x40000000)*32+n*4)
#define PFout(n)	*(uint32_t *)(0x42000000+((uint32_t)&GPIOF->ODR - 0x40000000)*32+n*4)



int main(void)
{
	
	//uint32_t *pf9 = (uint32_t *)(0x42000000+((uint32_t)&GPIOF->ODR - 0x40000000)*32+9*4);
	
	//ʹ��(��)�˿�F��Ӳ��ʱ�ӣ����ǶԶ˿�F����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);


	//��ʼ��GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_OUT;	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOF,&GPIO_InitStructure);
	
	
	PFout(9)=1;
	PFout(10)=1;
	
	
	while(1)
	{
		
		PFout(9)=0;		
		delay();		

		PFout(9)=1;
		delay();
		
		PFout(10)=0;		
		delay();
		
		PFout(10)=1;
		delay();
		

		
	}



}
