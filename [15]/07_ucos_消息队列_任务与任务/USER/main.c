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


OS_Q				g_queue_usart1;


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
				
	//������Ϣ���У�֧��16����Ϣ
	OSQCreate(&g_queue_usart1,"g_queue_usart1",16,&err)	;
				
	

	//����OS�������������
	OSStart(&err);
	
					
	printf("never run.......\r\n");
					
	while(1);
	
}


void task1(void *parg)
{
	OS_ERR err;
	
	uint32_t cnt=0;
	
	char buf[64]={0};
	
	printf("task1 is create ok\r\n");

	while(1)
	{
		cnt++;
		
		sprintf(buf,"task1 run %d times\r\n",cnt);
		
		OSQPost(&g_queue_usart1,buf,strlen(buf),OS_OPT_POST_FIFO,&err);
		
		printf("task1 is running ...\r\n");
		
		
		
		delay_ms(2000);
	}
}

void task2(void *parg)
{
	uint32_t i=0;
	OS_ERR err;
	
	OS_MSG_SIZE  msg_size;	//��Ϣ�Ĵ�С
	
	char *p=NULL;
	
	printf("task2 is create ok\r\n");

	while(1)
	{

		//�ȴ���Ϣ����-��Դ�ڴ�������
		p=OSQPend(&g_queue_usart1,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		//pָ����Ч����msg_size>0
		if(p && msg_size)
		{
			printf("OSQPend msg:");

			for(i=0; i<msg_size; i++)
			{
				printf("%c",p[i]);
			}
			
			printf("\r\n");
		
		}
		
		printf("task2 is running ...\r\n");

		

		
	}
}







