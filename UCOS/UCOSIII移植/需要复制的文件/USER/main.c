#include "sys.h"
#include "delay.h"
#include "includes.h"

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define TASK1_TASK_PRIO		4
//任务堆栈大小	
#define TASK1_STK_SIZE 		128
//任务控制块
OS_TCB Task1_TaskTCB;
//任务堆栈	
CPU_STK TASK1_TASK_STK[TASK1_STK_SIZE];
void task1_task(void *p_arg);

//任务优先级
#define TASK2_TASK_PRIO		5
//任务堆栈大小	
#define TASK2_STK_SIZE 		128
//任务控制块
OS_TCB Task2_TaskTCB;
//任务堆栈	
CPU_STK TASK2_TASK_STK[TASK2_STK_SIZE];
//任务函数
void task2_task(void *p_arg);

GPIO_InitTypeDef   GPIO_InitStruct;


void Led_Init(void)
{
    
    
    // 配置AHB总线
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOE, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin         = GPIO_Pin_9 | GPIO_Pin_10; // 配置的引脚
    GPIO_InitStruct.GPIO_Mode        = GPIO_Mode_OUT; // 配置GPIO端口模式：输出 GPIOF
    GPIO_InitStruct.GPIO_OType       = GPIO_OType_PP; // 配置GPIO端口类型：推挽输出
    GPIO_InitStruct.GPIO_Speed       = GPIO_Fast_Speed; // 配置GPIO端口速度：50M
    GPIO_InitStruct.GPIO_PuPd        = GPIO_PuPd_UP;    // 配置GPIO端口上下拉：上拉
    
    // 配置完成初始化
    GPIO_Init(GPIOF, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin         = GPIO_Pin_13 | GPIO_Pin_14; // 配置的引脚
    GPIO_InitStruct.GPIO_Mode        = GPIO_Mode_OUT; // 配置GPIO端口模式：输出 GPIOF
    GPIO_InitStruct.GPIO_OType       = GPIO_OType_PP; // 配置GPIO端口类型：推挽输出
    GPIO_InitStruct.GPIO_Speed       = GPIO_Fast_Speed; // 配置GPIO端口速度：50M
    GPIO_InitStruct.GPIO_PuPd        = GPIO_PuPd_UP;    // 配置GPIO端口上下拉：上拉
    
    // 配置完成初始化
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    // 初始化灭灯
    PFout(9) = 0;  
    PFout(10) = 1;    
    PEout(13) = 1; 
    PEout(14) = 1; 
}

//主函数
int main(void)
{
	OS_ERR err;

	
	delay_init(168);  								//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
	//uart_init(115200);  				 			//串口初始化
	Led_Init();         							//LED初始化	

	
	OSInit(&err);		   	 						//初始化UCOSIII
			 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     	//任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,		//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,			//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,						//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,						//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值

	OSStart(&err);      //开启UCOSIII
}


//开始任务任务函数
void start_task(void *p_arg)
{
	OS_ERR err;

	p_arg = p_arg;					//防止编译警告

	//CPU_Init();

	//创建TASK1任务
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
				 
	//创建TASK2任务
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

	OSTaskDel((OS_TCB*)0,&err);	//删除start_task任务自身
}


//task1任务函数
void task1_task(void *p_arg)
{
	u8 task1_num=0;
	OS_ERR err;

	p_arg = p_arg;
	
	while(1)
	{
		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
		PFout(9) ^= 1;
		//printf("任务1已经执行：%d次\r\n",task1_num);
		if(task1_num==5) 
		{
			OSTaskDel((OS_TCB*)&Task2_TaskTCB,&err);	//任务1执行5此后删除掉任务2
			//printf("任务1删除了任务2!\r\n");
		}

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
		
	}
}

//task2任务函数
void task2_task(void *p_arg)
{
	u8 task2_num=0;
	OS_ERR err;

	p_arg = p_arg;
	

	while(1)
	{
		task2_num++;	//任务2执行次数加1 注意task1_num2加到255的时候会清零！！
		PFout(10) ^= 1;
		//printf("任务2已经执行：%d次\r\n",task2_num);

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	}
}

