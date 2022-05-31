#include "handler.h"
#include "sys.h"
//串口1中断
void USART1_IRQHandler(void)
{
	uint8_t d;
	//检查串口1是否接收到数据
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{	
		
		//读取数据
		d= USART_ReceiveData(USART1);
		
		//AT指令模式
		//USART_SendData(USART3,d);
	
		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);		
	}
}

void EXTI0_IRQHandler(void)
{
	//检查是否有中断触发
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
	//检查是否有中断触发
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
			PFout(10) = 0;			//火焰传感器触发
		}
		else if(PAin(2) == 0)
			PFout(10) = 1;			//火焰传感器触发
		
		EXTI_ClearITPendingBit(EXTI_Line2);		
	}
	
	
}

void EXTI3_IRQHandler(void)
{
	//检查是否有中断触发
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
	//检查是否有中断触发
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




