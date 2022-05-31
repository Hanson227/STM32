#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

//任务1控制块
OS_TCB Task1_TCB;			//线程id

void task1(void *parg);

CPU_STK task1_stk[128];		//任务1的任务堆栈，大小为128字，也就是512字节



//任务2控制块
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];		//任务2的任务堆栈，大小为128字，也就是512字节


OS_MUTEX					g_mutex;


//主函数
int main(void)
{
	OS_ERR err;

	systick_init();  													//时钟初始化
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置
	
	usart_init(9600);  				 									//串口初始化
	
	LED_Init();         												//LED初始化	


	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);


	//创建任务1，等同于创建Linux 线程1
	OSTaskCreate(	&Task1_TCB,									//任务控制块，等同于线程id
					"Task1",									//任务的名字，名字可以自定义的
					task1,										//任务函数，等同于线程函数
					0,											//传递参数，等同于线程的传递参数
					6,											//任务的优先级6		
					task1_stk,									//任务堆栈基地址
					128/10,										//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核															
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);


	//创建任务2，等同于创建Linux 线程2
	OSTaskCreate(	&Task2_TCB,									//任务控制块
					"Task2",									//任务的名字
					task2,										//任务函数
					0,												//传递参数
					6,											 	//任务的优先级6		
					task2_stk,									//任务堆栈基地址
					128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,												//默认是抢占式内核																
					0,												//不需要补充用户存储区
					OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
				
	//创建互斥锁	
	OSMutexCreate(&g_mutex,"g_mutex",&err);

	//启动OS，进行任务调度
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

		//一直阻塞等待互斥锁
		OSMutexPend(&g_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		
		printf("task1 is running ...\r\n");
		
		
		//立即释放互斥锁，其他等待锁的任务（最高优先级且就绪）得到立即执行
		OSMutexPost(&g_mutex,OS_OPT_POST_NONE,&err);
		
		delay_ms(20);
	}
}

void task2(void *parg)
{

	OS_ERR err;
	
	printf("task2 is create ok\r\n");

	while(1)
	{


		//一直阻塞等待互斥锁
		OSMutexPend(&g_mutex,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		
		printf("task2 is running ...\r\n");
		
		//立即释放互斥锁，其他等待锁的任务（最高优先级且就绪）得到立即执行
		OSMutexPost(&g_mutex,OS_OPT_POST_NONE,&err);
		
		


		delay_ms(30);
		
	}
}







