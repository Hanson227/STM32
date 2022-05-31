#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

#define TASK1_STK_SIZE 4096

//����1���ƿ�
OS_TCB Task1_TCB;			//�߳�id

void task1(void *parg);

CPU_STK task1_stk[TASK1_STK_SIZE];		//����1�������ջ����СΪ4096�֣�Ҳ����4096*4�ֽ�



OS_TMR			g_timer;

void  timer_callback (OS_TMR *p_tmr, void *p_arg);


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
					TASK1_STK_SIZE/10,							//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					TASK1_STK_SIZE,								//�����ջ��С			
					0,											//��ֹ������Ϣ����
					0,											//Ĭ������ռʽ�ں�															
					0,											//����Ҫ�����û��洢��
					OS_OPT_TASK_NONE,							//û���κ�ѡ��
					&err										//���صĴ�����
				);

	//������ʱ�����ӳ�10*10msִ�У���ʱ����Ϊ1S��			
	OSTmrCreate(&g_timer,"g_timer",10,100,OS_OPT_TMR_PERIODIC,timer_callback,NULL,&err);
	
	
	OSTmrStart(&g_timer,&err);

	//����OS�������������
	OSStart(&err);
	
					
	printf("never run.......\r\n");
					
	while(1);
	
}


void task1(void *parg)
{
	OS_ERR err;
	

	
	printf("task1 is create ok\r\n");

	while(1)
	{

		printf("task1 is running ...\r\n");

		delay_ms(3000);
	}
}


void  timer_callback (OS_TMR *p_tmr, void *p_arg)
{

	

	delay_ms(5000);
	
	printf("timer_callback\r\n");

}



