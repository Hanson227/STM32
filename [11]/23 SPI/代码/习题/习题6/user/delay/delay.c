#include "delay.h"

void delay_us(uint32_t nus)
{

	SysTick->CTRL = 0;							// �ر�ϵͳ��ʱ��
	SysTick->LOAD = nus*(SystemCoreClock/1000000); 	// nus *1us��ʱ���������ֵ
	SysTick->VAL = 0; 							// ��ձ�־λ
	SysTick->CTRL = 5; 							// ʹ��ϵͳ��ʱ��������ʱ��ԴΪϵͳʱ��168MHz
	while ((SysTick->CTRL & 0x00010000)==0);	// ���CTRL�Ĵ�����16bit�Ƿ���1������1���ʹ���������
	SysTick->CTRL = 0; 							// �ر�ϵͳ��ʱ��	
}

void delay_ms(uint32_t nms)
{

	while(nms--)
	{
	
		SysTick->CTRL = 0;							// �ر�ϵͳ��ʱ��
		SysTick->LOAD = (SystemCoreClock/1000); 	// 1ms��ʱ���������ֵ
		SysTick->VAL = 0; 							// ��ձ�־λ
		SysTick->CTRL = 5; 							// ʹ��ϵͳ��ʱ��������ʱ��ԴΪϵͳʱ��168MHz
		while ((SysTick->CTRL & 0x00010000)==0);	// ���CTRL�Ĵ�����16bit�Ƿ���1������1���ʹ���������
		SysTick->CTRL = 0; 							// �ر�ϵͳ��ʱ��	
	
	}
}

