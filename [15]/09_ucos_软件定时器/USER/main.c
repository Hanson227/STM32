#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"

#define TASK1_STK_SIZE 4096

//任务1控制块
OS_TCB Task1_TCB;			//线程id

void task1(void *parg);

CPU_STK task1_stk[TASK1_STK_SIZE];		//任务1的任务堆栈，大小为4096字，也就是4096*4字节



OS_TMR			g_timer;

void  timer_callback (OS_TMR *p_tmr, void *p_arg);


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
					TASK1_STK_SIZE/10,							//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					TASK1_STK_SIZE,								//任务堆栈大小			
					0,											//禁止任务消息队列
					0,											//默认是抢占式内核															
					0,											//不需要补充用户存储区
					OS_OPT_TASK_NONE,							//没有任何选项
					&err										//返回的错误码
				);

	//启动定时器后延迟10*10ms执行，定时周期为1S，			
	OSTmrCreate(&g_timer,"g_timer",10,100,OS_OPT_TMR_PERIODIC,timer_callback,NULL,&err);
	
	
	OSTmrStart(&g_timer,&err);

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

		delay_ms(3000);
	}
}


void  timer_callback (OS_TMR *p_tmr, void *p_arg)
{

	

	delay_ms(5000);
	
	printf("timer_callback\r\n");

}



