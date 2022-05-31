#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

//����1���ƿ�
OS_TCB Task1_TCB;			//�߳�id

void task1(void *parg);

CPU_STK task1_stk[128];		//����1�������ջ����СΪ128�֣�Ҳ����512�ֽ�



//����2���ƿ�
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];		//����2�������ջ����СΪ128�֣�Ҳ����512�ֽ�


OS_FLAG_GRP				g_flag_grp;//�¼���־��


//������
int main(void)
{
	OS_ERR err;

	systick_init();  													//ʱ�ӳ�ʼ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������
	
	usart_init(9600);  				 									//���ڳ�ʼ��
	
	LED_Init();         												//LED��ʼ��	


	//OS��ʼ�������ǵ�һ�����еĺ���,��ʼ�����ֵ�ȫ�ֱ����������ж�Ƕ�׼����������ȼ����洢��
	OSInit(&err);


	//��������1����ͬ�ڴ���Linux �߳�1
	OSTaskCreate(	&Task1_TCB,									//������ƿ飬��ͬ���߳�id
					"Task1",									//��������֣����ֿ����Զ����
					task1,										//����������ͬ���̺߳���
					0,											//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					6,											//��������ȼ�6		
					task1_stk,									//�����ջ����ַ
					128/10,										//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					128,										//�����ջ��С			
					0,											//��ֹ������Ϣ����
					0,											//Ĭ������ռʽ�ں�															
					0,											//����Ҫ�����û��洢��
					OS_OPT_TASK_NONE,							//û���κ�ѡ��
					&err										//���صĴ�����
				);


	//��������2����ͬ�ڴ���Linux �߳�2
	OSTaskCreate(	&Task2_TCB,									//������ƿ�
					"Task2",									//���������
					task2,										//������
					0,												//���ݲ���
					6,											 	//��������ȼ�6		
					task2_stk,									//�����ջ����ַ
					128/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					128,										//�����ջ��С			
					0,											//��ֹ������Ϣ����
					0,												//Ĭ������ռʽ�ں�																
					0,												//����Ҫ�����û��洢��
					OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
				
	//�����¼���־��
	//��ֵΪ0
	OSFlagCreate(&g_flag_grp,"OS_FLAG_GRP",0,&err);
				
	

	//����OS�������������
	OSStart(&err);
	
					
	printf("never run.......\r\n");
					
	while(1);
	
}

static GPIO_InitTypeDef 	GPIO_InitStructure;
static EXTI_InitTypeDef   	EXTI_InitStructure;
static NVIC_InitTypeDef   	NVIC_InitStructure;

void exti0_init(void)
{
	//ʹ��(��)�˿�A��Ӳ��ʱ�ӣ����ǶԶ˿�A����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	//ʹ��ϵͳ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//����PA0����Ϊ����ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//��0������
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������
	GPIO_Init(GPIOA,&GPIO_InitStructure);	


	//��PA0��EXTI0������һ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	//�ⲿ�жϵ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;		//�ⲿ�ж�0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//�½��ش���������ʶ�𰴼��İ��£������ش��������ڼ�ⰴ�����ɿ���  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//ʹ��
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0������ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ�ܸ�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
}

void task1(void *parg)
{
	OS_ERR err;
	
	
	printf("task1 is create ok\r\n");

	while(1)
	{

		
		printf("task1 is running ...\r\n");
		
		
		
		delay_ms(2000);
	}
}

void task2(void *parg)
{

	OS_ERR err;
	
	OS_FLAGS flags=0;
	
	printf("task2 is create ok\r\n");
	
	exti0_init();

	while(1)
	{

		//�ȴ��¼���־��
		//�ȴ�bit0
		flags=OSFlagPend(&g_flag_grp,
					0x01,
					0,
					OS_OPT_PEND_BLOCKING+OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
					0,
					&err);
		
		//���bit0�Ƿ���λ
		if(flags & 0x01)
		{
			printf("OSFlagPend bit0 set\r\n");
		
		}

		
		printf("task2 is running ...\r\n");

		

		
	}
}




void EXTI0_IRQHandler(void)
{
	OS_ERR err;

	
	OSIntEnter();
	
	
	//�ж��Ƿ����ж�����
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		//�����¼���־��
		//����bit0Ϊ1
		OSFlagPost(&g_flag_grp,0x01,OS_OPT_POST_FLAG_SET,&err);

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	
	
	OSIntExit();	

}


