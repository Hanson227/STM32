#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "dht11.h"
#include "includes.h"

//任务1控制块
OS_TCB Task1_TCB;			//线程id

void task1(void *parg);

CPU_STK task1_stk[128];			//任务1的任务堆栈，大小为128字，也就是512字节



//任务2控制块
OS_TCB Task2_TCB;

void task2(void *parg);

CPU_STK task2_stk[128];			//任务2的任务堆栈，大小为128字，也就是512字节


//任务3控制块
OS_TCB Task3_TCB;

void task3(void *parg);

CPU_STK task3_stk[128];			//任务3的任务堆栈，大小为128字，也就是512字节


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
				
				
	//创建任务3，等同于创建Linux 线程3
	OSTaskCreate(	&Task3_TCB,									//任务控制块
					"Task3",									//任务的名字
					task3,										//任务函数
					0,												//传递参数
					6,											 	//任务的优先级6		
					task3_stk,									//任务堆栈基地址
					128/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					128,										//任务堆栈大小			
					0,											//禁止任务消息队列
					0,												//默认是抢占式内核																
					0,												//不需要补充用户存储区
					OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);

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

		printf("task1 is running ...\r\n");

		delay_ms(1000);
	}
}

void task2(void *parg)
{

	
	printf("task2 is create ok\r\n");

	while(1)
	{

		printf("task2 is running ...\r\n");


		delay_ms(1000);
	}
}


void task3(void *parg)
{
	int32_t rt=0;
	uint8_t buf[5]={0};
	
	//温湿度模块初始化
	dht11_init();	
	
	printf("task3 is create ok\r\n");

	while(1)
	{
		rt = dht11_read(buf);
		
		if(rt == 0)
		{
			printf("T:%d.%d,H:%d.%d\r\n",buf[2],buf[3],buf[0],buf[1]);
		
		}
		else
		{
			printf("dht11 error code %d\r\n",rt);
		}
		
		delay_ms(6000);

	}
}





