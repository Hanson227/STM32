#include "handler.h"
#include "sys.h"
//����1�ж�
void USART1_IRQHandler(void)
{
	uint8_t d;
	//��鴮��1�Ƿ���յ�����
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{	
		
		//��ȡ����
		d= USART_ReceiveData(USART1);
		
		//ATָ��ģʽ
		//USART_SendData(USART3,d);
	
		//��ձ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		
	}
}

void EXTI0_IRQHandler(void)
{
	//����Ƿ����жϴ���
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
//		if(voice < 100)
//			voice += 10;
//		tim13_set_duty(0);	
//		printf("voice:%d\r\n",voice);
		
		//delay_ms(10);
		KEY1 = 1;

		EXTI_ClearITPendingBit(EXTI_Line0);		
	}
}

void EXTI2_IRQHandler(void)
{
	//����Ƿ����жϴ���
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
//		if(voice > 10)
//			voice -= 10;
//		printf("voice:%d\r\n",voice);
	
		//delay_ms(20);
		KEY2 = 1;
		KEY1 = 0;
		if(PAin(2))
		{
			PFout(10) = 0;			//���洫��������
		}
		else if(PAin(2) == 0)
			PFout(10) = 1;			//���洫��������
		
		EXTI_ClearITPendingBit(EXTI_Line2);		
	}
	
	
}

void EXTI3_IRQHandler(void)
{
	//����Ƿ����жϴ���
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
	{
//		if(voice > 10)
//			voice -= 10;
//		printf("voice:%d\r\n",voice);
		
		//delay_ms(20);
		KEY3 = 1;
		KEY1 = 0;	

		EXTI_ClearITPendingBit(EXTI_Line3);		
	}
	
}

void EXTI4_IRQHandler(void)
{
	//����Ƿ����жϴ���
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
	{
//		if(voice > 10)
//			voice -= 10;
//		printf("voice:%d\r\n",voice);
		
		//delay_ms(20);
		KEY4 = 1;
		KEY1 = 0;		
		
		EXTI_ClearITPendingBit(EXTI_Line4);		
	}
	
}




