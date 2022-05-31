#include "led.h" 

//��ʼ�� PF9��PF10��PE13��PE14 Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);			//ʹ��GPIOE��GPIOFʱ��

  //GPIOF9,F10��ʼ������ 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//LED0��LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    //��ͨ���ģʽ��
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������������LED��Ҫ��������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    //100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //����
  GPIO_Init(GPIOF, &GPIO_InitStructure);						//��ʼ��GPIOF�������õ�����д��Ĵ���
  
  //GPIOE13,PE14��ʼ������ 
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;	//LED2��LED3��ӦIO��
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;				//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//100MHz
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);						//��ʼ��GPIOE�������õ�����д��Ĵ���
	
  GPIO_SetBits(GPIOF,GPIO_Pin_9  | GPIO_Pin_10);			    //GPIOF9,PF10���øߣ�����
  GPIO_SetBits(GPIOE,GPIO_Pin_13 | GPIO_Pin_14);				//GPIOE13,PE14���øߣ�����
}






