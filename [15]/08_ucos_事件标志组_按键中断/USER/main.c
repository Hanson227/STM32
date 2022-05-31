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


OS_FLAG_GRP				g_flag_grp;//事件标志组


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
				
	//创建事件标志组
	//初值为0
	OSFlagCreate(&g_flag_grp,"OS_FLAG_GRP",0,&err);
				
	

	//启动OS，进行任务调度
	OSStart(&err);
	
					
	printf("never run.......\r\n");
					
	while(1);
	
}

static GPIO_InitTypeDef 	GPIO_InitStructure;
static EXTI_InitTypeDef   	EXTI_InitStructure;
static NVIC_InitTypeDef   	NVIC_InitStructure;

void exti0_init(void)
{
	//使能(打开)端口A的硬件时钟，就是对端口A供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	
	//使能系统配置时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//配置PA0引脚为输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;		//第0根引脚
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN;		//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	//没有使能上下拉电阻
	GPIO_Init(GPIOA,&GPIO_InitStructure);	


	//将PA0和EXTI0连接在一起
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	//外部中断的配置
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;		//外部中断0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发，用于识别按键的按下；上升沿触发，用于检测按键的松开；  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//外部中断0的请求通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;		//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能该通道
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

		//等待事件标志组
		//等待bit0
		flags=OSFlagPend(&g_flag_grp,
					0x01,
					0,
					OS_OPT_PEND_BLOCKING+OS_OPT_PEND_FLAG_SET_ANY+OS_OPT_PEND_FLAG_CONSUME,
					0,
					&err);
		
		//检测bit0是否置位
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
	
	
	//判断是否有中断请求
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		//设置事件标志组
		//设置bit0为1
		OSFlagPost(&g_flag_grp,0x01,OS_OPT_POST_FLAG_SET,&err);

		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	
	
	OSIntExit();	

}


