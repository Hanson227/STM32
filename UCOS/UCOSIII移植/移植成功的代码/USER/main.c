#include "sys.h"
#include "delay.h"
#include "includes.h"

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define TASK1_TASK_PRIO		4
//�����ջ��С	
#define TASK1_STK_SIZE 		128
//������ƿ�
OS_TCB Task1_TaskTCB;
//�����ջ	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

//�������ȼ�
#define TASK2_TASK_PRIO		5
//�����ջ��С	
#define TASK2_STK_SIZE 		128
//������ƿ�
OS_TCB Task2_TaskTCB;
//�����ջ	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//������
void task2_task(void *p_arg);

static GPIO_InitTypeDef   GPIO_InitStruct;
static NVIC_InitTypeDef 		NVIC_InitStruct;
static USART_InitTypeDef 		USART_InitStruct;

void usart1_init(uint32_t baud)
{
	
	//��PAӲ��ʱ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//�򿪴���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//����PA9��PA10Ϊ���ù���ģʽ
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9 10������
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF;	//�๦��ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	//��������������������������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//������Ӧ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;	//û��ʹ������������

	GPIO_Init(GPIOA,&GPIO_InitStruct);


	//��PA9��PA10�������ӵ�����1��Ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);	
	
	
	
	//���ô���1��ز����������ʡ���У��λ��8λ����λ��1��ֹͣλ......
	USART_InitStruct.USART_BaudRate = baud;										//������
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;						//8λ����λ
	USART_InitStruct.USART_StopBits = USART_StopBits_1;							//1��ֹͣλ
	USART_InitStruct.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ��������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����շ�����
	USART_Init(USART1, &USART_InitStruct);
	
	
	//���ô���1���жϴ�������������һ���ֽڴ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
	//���ô���1���ж����ȼ�
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	
	
	//ʹ�ܴ���1����
	USART_Cmd(USART1, ENABLE);


}

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	
	return ch;
}
void Led_Init(void)
{
    
    
    // ����AHB����
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin         = GPIO_Pin_6 | GPIO_Pin_7; // ���õ�����
    GPIO_InitStruct.GPIO_Mode        = GPIO_Mode_OUT; // ����GPIO�˿�ģʽ����� GPIOF
    GPIO_InitStruct.GPIO_OType       = GPIO_OType_PP; // ����GPIO�˿����ͣ��������
    GPIO_InitStruct.GPIO_Speed       = GPIO_Fast_Speed; // ����GPIO�˿��ٶȣ�50M
    GPIO_InitStruct.GPIO_PuPd        = GPIO_PuPd_UP;    // ����GPIO�˿�������������
    
    // ������ɳ�ʼ��
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // ��ʼ�����
    PAout(6) = 0;  
    PAout(7) = 1;    

}

//������
int main(void)
{
	OS_ERR err;

	usart1_init(115200);
	delay_init(168);  								//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
	//uart_init(115200);  				 			//���ڳ�ʼ��
	Led_Init();         							//LED��ʼ��	

	
	OSInit(&err);		   	 						//��ʼ��UCOSIII
			 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     	//�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,		//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,			//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,						//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,						//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ

	OSStart(&err);      //����UCOSIII
}


//��ʼ����������
void start_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;					//��ֹ���뾯��

	//CPU_Init();

	//����TASK1����
	OSTaskCreate((OS_TCB 	* )&Task1_TaskTCB,		
				 (CPU_CHAR	* )"Task1 task", 		
                 (OS_TASK_PTR )task1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK1_TASK_PRIO,     
                 (CPU_STK   * )&TASK1_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK1_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����TASK2����
	OSTaskCreate((OS_TCB 	* )&Task2_TaskTCB,		
				 (CPU_CHAR	* )"task2 task", 		
                 (OS_TASK_PTR )task2_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )TASK2_TASK_PRIO,     	
                 (CPU_STK   * )&TASK2_TASK_STK[0],	
                 (CPU_STK_SIZE)TASK2_STK_SIZE/10,	
                 (CPU_STK_SIZE)TASK2_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);			 

	OSTaskDel((OS_TCB*)0,&err);	//ɾ��start_task��������
}


//task1������
void task1_task(void *p_arg)
{
	u8 task1_num=0;
	OS_ERR err;

	p_arg = p_arg;
	
	while(1)
	{
		task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
		PAout(6) ^= 1;
		printf("����1�Ѿ�ִ�У�%d��\r\n",task1_num);
		if(task1_num==5) 
		{
			OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//����1ִ��5�˺�ɾ��������2
			printf("����1ɾ��������2!\r\n");
		}

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
		
	}
}

//task2������
void task2_task(void *p_arg)
{
	u8 task2_num=0;
	OS_ERR err;

	p_arg = p_arg;
	

	while(1)
	{
		task2_num++;	//����2ִ�д�����1 ע��task1_num2�ӵ�255��ʱ������㣡��
		PAout(7) ^= 1;
		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}

