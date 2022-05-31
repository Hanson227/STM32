#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"



static GPIO_InitTypeDef  	GPIO_InitStructure;
static USART_InitTypeDef 	USART_InitStructure;
static NVIC_InitTypeDef 	NVIC_InitStructure;	
static EXTI_InitTypeDef  	EXTI_InitStructure;

static RTC_TimeTypeDef  	RTC_TimeStructure;
static RTC_InitTypeDef  	RTC_InitStructure;
static RTC_DateTypeDef 		RTC_DateStructure;

uint32_t uwStartSector = 0;
uint32_t uwEndSector = 0;
uint32_t uwSectorCounter = 0;

static volatile uint8_t  g_usart1_recv_buf[128]={0};
static volatile uint32_t g_usart1_recv_cnt = 0;
static volatile uint32_t g_usart1_event=0;

static volatile uint32_t g_rtc_wakeup_event=0;


#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */


//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);  
	
	return ch;
}   

void delay_us(uint32_t nus)
{

	SysTick->CTRL = 0; 								// 关闭系统定时器之后才能对寄存器进行配置
	SysTick->LOAD = SystemCoreClock/8/1000000*nus;	// 设置计数值，用于决定当前的定时时间[*]
	SysTick->VAL = 0; 								// 清空标志位
	//SysTick->CTRL = 5; 								// 使能定时器开始计数，并且时钟源来源于PLL的168MHz
	SysTick->CTRL = 1; 						// 使能定时器开始计数，并且时钟源来源于PLL的168MHz再进行8分频的时钟=21MHz
	while ((SysTick->CTRL & 0x00010000)==0);		// 等待计数值计数完毕，就是定时时间已经到达
	SysTick->CTRL = 0; 								// 关闭系统定时器	
}


void delay_ms(uint32_t nms)
{

	while(nms--)
	{
		SysTick->CTRL = 0; 						// 关闭系统定时器之后才能对寄存器进行配置
		SysTick->LOAD = SystemCoreClock/8/1000;	// 设置计数值，用于决定当前的定时时间[*]
		SysTick->VAL = 0; 						// 清空标志位
		//SysTick->CTRL = 5; 						// 使能定时器开始计数，并且时钟源来源于PLL的168MHz
		SysTick->CTRL = 1; 						// 使能定时器开始计数，并且时钟源来源于PLL的168MHz再进行8分频的时钟=21MHz
		while ((SysTick->CTRL & 0x00010000)==0);// 等待计数值计数完毕，就是定时时间已经到达
		SysTick->CTRL = 0; 						// 关闭系统定时器	
	}

}


void LED_Init(void)
{    	 
  
	//使能GPIOE，GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);			

	//GPIOF9,F10初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;		//LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			    	//普通输出模式，
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出，驱动LED需要电流驱动
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		    	//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				    //上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);							//初始化GPIOF，把配置的数据写入寄存器						


	//GPIOE13,PE14初始化设置 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;		//LED2和LED3对应IO口
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;					//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;					//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);							//初始化GPIOE，把配置的数据写入寄存器

	GPIO_SetBits(GPIOF,GPIO_Pin_9  | GPIO_Pin_10);			    	//GPIOF9,PF10设置高，灯灭
	GPIO_SetBits(GPIOE,GPIO_Pin_13 | GPIO_Pin_14);		
}


void USART1_Init(uint32_t baud)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 							//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);							//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 						//GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 						//GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 						//GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;									//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;								//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 									//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 									//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 											//初始化PA9，PA10

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = baud;										//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//收发模式
	USART_Init(USART1, &USART_InitStructure); 										//初始化串口1
	
	USART_Cmd(USART1, ENABLE);  													//使能串口1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//开启相关中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;								//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;							//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;								//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器
}


void dht11_outputmode(void)
{

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
}

void dht11_data_mode(uint32_t mode)
{
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				//第9号引脚
	GPIO_InitStructure.GPIO_Mode = mode;					//输出/入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
}


uint32_t dht11_start(void)
{
	uint32_t t=0;
	//PG9引脚为输出模式
	dht11_data_mode(GPIO_Mode_OUT);
	
	
	PGout(9)=0;
	
	delay_ms(20);


	PGout(9)=1;
	
	delay_us(30);

	//PG9引脚为输入模式
	dht11_data_mode(GPIO_Mode_IN);
	
	//检测是否有低电平产生，若没有低电平产生，则执行超时处理，超时时间为1ms
	t=0;
	while(1)
	{
		delay_us(1);
		
		t++;
		
		//若出现低电平，则跳出该循环，DHT11有响应的了
		if(PGin(9)==0)
			break;
		
		//若dht11在1ms内没有做出响应，则视为超时
		if(t >=1000)
			return 1;
	}
	
	//等待低电平持续完毕
	while(PGin(9)==0);
	
	//等待高电平持续完毕
	while(PGin(9));
	
	return 0;
}

uint8_t dht11_read_byte(void)
{
	uint32_t i=0;
	uint8_t d=0;
	
	
	for(i=0; i<8; i++)
	{
		//检测到低电平，然后等待高电平的出现
		while(PGin(9)==0);
		
		//延时40us
		delay_us(40);
		
		//判断PG9引脚的电平,当前是高位先出
		if(PGin(9))
		{
			d|=1<<(7-i);
			
			//让高电平持续完毕
			while(PGin(9));
		}
	}

	
	return d;
}


uint32_t dht11_read_data(uint8_t *pbuf)
{
	uint32_t i=0;
	uint8_t check_sum=0;
	
	//等待建立启动信号成功
	if(0!=dht11_start())
		return 1;

	//连续读取5个字节，就是连续读取40bit的数据
	for(i=0; i<5; i++)
		pbuf[i]=dht11_read_byte();

	//通信的结束，延时100us
	delay_us(100);
	
	//检验和，检查当前的数据是否正确
	check_sum=pbuf[0]+pbuf[1]+pbuf[2]+pbuf[3];
	
	if(check_sum == pbuf[4])
		return 0;
	
	return 1;
	
}



static uint32_t g_dht11_start=0;

void flash_erase_record(void);

uint32_t flash_write_record(char *pbuf,uint32_t record_count)
{
	uint32_t addr_start=ADDR_FLASH_SECTOR_4+record_count*64;
	uint32_t addr_end  =addr_start+64;

	uint32_t i=0;
	
	while (addr_start < addr_end)
	{
		//每次写入数据是4个字节
		if (FLASH_ProgramWord(addr_start, *(uint32_t *)&pbuf[i]) == FLASH_COMPLETE)
		{
			//地址每次偏移4个字节
			addr_start +=4;
			
			i+=4;
		}
		else
		{ 
			printf("flash write record fail,now goto erase sector!\r\n");
			
			//重新擦除扇区
			flash_erase_record();

			return 1;
		}
	}
	
	return 0;
}

void flash_read_record(char *pbuf,uint32_t record_count)
{
	uint32_t addr_start=ADDR_FLASH_SECTOR_4+record_count*64;
	uint32_t addr_end  =addr_start+64;

	uint32_t i=0;
	
	while (addr_start < addr_end)
	{
		*(uint32_t *)&pbuf[i] = *(__IO uint32_t*)addr_start;

		addr_start+=4;
		
		i = i + 4;
	}

}

void flash_erase_record(void)
{

	
	/* 如果不擦除，写入会失败，读取的数据都为0 */
	printf("FLASH_EraseSector start\r\n");
	

	if (FLASH_EraseSector(FLASH_Sector_4, VoltageRange_3) != FLASH_COMPLETE)
	{ 
			printf("Erase error\r\n");
			return;
	}

	printf("FLASH_EraseSector ends\r\n");
}


void rtc_date_init(void)
{

	/* Enable the PWR clock，使能电源时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	/* Allow access to RTC，允许访问RTC */
	PWR_BackupAccessCmd(ENABLE);
	
	//使能外部低速晶振，提高精度
	RCC_LSEConfig(RCC_LSE_ON);
	
	/* Wait till LSI is ready，等待外部低速震荡电路稳定 */  
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)

	/* Select the RTC Clock Source，选择RTC时钟源 */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	/* Enable the RTC Clock,使能RTC时钟 */
	RCC_RTCCLKCmd(ENABLE);
	
	//等待RTC相关寄存器就绪
	RTC_WaitForSynchro();
	
	/*  Configure the RTC data register and RTC prescaler 
	
		RTC时钟频率 = 32768Hz / 128 / 256 = 1Hz
	*/
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;					//异步分频系数128
	RTC_InitStructure.RTC_SynchPrediv = 0xFF;					//同步分频系数256
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;		//24小时格式
	RTC_Init(&RTC_InitStructure);
  
	/* Set the date: 2018/05/04 星期五 */
	RTC_DateStructure.RTC_Year = 0x18;
	RTC_DateStructure.RTC_Month = RTC_Month_May;
	RTC_DateStructure.RTC_Date = 0x04;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday;
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	 
	 /* Set the time to 11:11:11 AM */
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x11;
	RTC_TimeStructure.RTC_Minutes = 0x11;
	RTC_TimeStructure.RTC_Seconds = 0x11; 
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

	/* Indicator for the RTC configuration,写备份寄存器 */
	//RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
	
	//关闭WAKE UP
	RTC_WakeUpCmd(DISABLE);		

	//唤醒时钟选择RTC配置好的时钟
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);

	//设置WAKE UP自动重装载寄存器，写入零值默认是0+1=1
	RTC_SetWakeUpCounter(0);								
	
	
	//注意：必须添加清空RTC唤醒中断标志位，否则RTC不会触发
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	/* Enable RTC Wakeup Interrupt ，使能RTC唤醒中断*/
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	 
	/* Enable the Wakeup ，开启RTCWakeup定时器*/
	RTC_WakeUpCmd(ENABLE);
	

	/* EXTI configuration *******************************************************/
	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	 
	/* Enable the RTC Wakup Interrupt，使能RTC唤醒中断 */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	/* Indicator for the RTC configuration，写备份寄存器，用于建立判断标志是否要重置RTC的日期与时间 */
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x8888);	
}


int main(void)
{ 
	uint8_t 	dht11_data[4];
	uint32_t	dht11_rec_cnt=0;
	uint32_t	i=0;
	char 		buf[128]={0};
	
	LED_Init();		

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	//设置中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//串口1,波特率115200bps,开启接收中断
	USART1_Init(115200);

	//默认擦除记录
	//flash_erase_record();
	printf("This is flash test\r\n");
	
	/*解锁FLASH，允许操作FLASH*/
	FLASH_Unlock();

	/* 清空相应的标志位*/  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                   FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 	
	
	//尝试获取100条记录
	for(i=0;i<100;i++)
	{
		//获取存储的记录
		flash_read_record(buf,i);
		
		//检查记录是否存在换行符号，不存在则不打印输出
		if(strstr((const char *)buf,"\n")==0)
			break;		
		
	}	
	
	
	
	dht11_rec_cnt=i;	
	
	printf("data records count=%d\r\n",dht11_rec_cnt);
	
	//rtc初始化
	if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x8888)
	{  
		rtc_date_init();
	}
	else
	{
		/* Enable the PWR clock ，使能电源时钟*/
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
		
		/* Allow access to RTC，允许访问RTC */
		PWR_BackupAccessCmd(ENABLE);
		
		/* Wait for RTC APB registers synchronisation，等待所有的RTC寄存器就绪 */
		RTC_WaitForSynchro();	
		

		//关闭唤醒功能
		RTC_WakeUpCmd(DISABLE);
		
		//为唤醒功能选择RTC配置好的时钟源
		RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
		
		//设置唤醒计数值为自动重载，写入值默认是0，1->0
		RTC_SetWakeUpCounter(0);
		
		//清除RTC唤醒中断标志
		RTC_ClearITPendingBit(RTC_IT_WUT);
		
		//使能RTC唤醒中断
		RTC_ITConfig(RTC_IT_WUT, ENABLE);

		//使能唤醒功能
		RTC_WakeUpCmd(ENABLE);			


		/* 配置外部中断控制线22，实现RTC唤醒*/
		EXTI_ClearITPendingBit(EXTI_Line22);
		EXTI_InitStructure.EXTI_Line = EXTI_Line22;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* 使能RTC唤醒中断 */
		NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	
	while(1)
	{
		//rtc唤醒事件
		if(g_rtc_wakeup_event)
		{
			g_rtc_wakeup_event=0;
			
			//开始采集温湿度数据
			if(g_dht11_start)
			{
				//读取温湿度数据
				if(dht11_read_data(dht11_data) == 0)
				{
					//检查当前的数据记录是否小于100条
					if(dht11_rec_cnt<100)
					{
						// 获取时间
						RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);						
						
						// 获取日期
						RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);
						

						
						//格式化字符串,末尾添加\r\n作为一个结束标记，方便我们读取的时候进行判断
						sprintf((char *)buf,"[%03d]20%02x/%02x/%02x Week:%x %02x:%02x:%02x Temp=%d.%d Humi=%d.%d\r\n", \
										dht11_rec_cnt,\
										RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay,\
										RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds,\
										dht11_data[2],dht11_data[3],dht11_data[0],dht11_data[1]);
					
						//写入温湿度记录
						if(0==flash_write_record(buf,dht11_rec_cnt))
						{
							//显示
							printf(buf);					

							//记录自加1
							dht11_rec_cnt++;						
						}
						else
						{
							//数据记录清零，重头开始存储数据
							dht11_rec_cnt=0;
						}
						
					}
					else
					{
						//超过100条记录则打印
						printf("The record has reached 100 and cannot continue writing\r\n");
					}
				}			
			
			}		
		}

		//判断串口接收到的字符串
		//执行串口1事件
		if(g_usart1_event)
		{
			//判断接收到的字符串为start
			if(strstr((char *)g_usart1_recv_buf,"start"))
			{
				//开启温湿度数据采集
				g_dht11_start =1;
				
				printf("Open temperature and humidity data acquisition\r\n");
												
			}		
			
			//判断接收到的字符串为stop
			if(strstr((char *)g_usart1_recv_buf,"stop"))
			{
				//停止温湿度数据采集
				g_dht11_start =0;
				
				printf("Stop temperature and humidity data acquisition\r\n");
												
			}		

			//判断接收到的字符串为clear
			if(strstr((char *)g_usart1_recv_buf,"clear"))
			{
				//清空所有记录
				printf("Emptying all data records is being executed......\r\n");
				
				//扇区擦除
				flash_erase_record();
				
				printf("Empty all data records successfully\r\n");
				
				//清零记录计数值
				dht11_rec_cnt=0;
												
			}	

			//判断接收到的字符串为show
			if(strstr((char *)g_usart1_recv_buf,"show"))
			{
				printf("Execution temperature data record display:\r\n");
				
				//尝试获取100条记录
				for(i=0;i<100;i++)
				{
					//获取存储的记录
					flash_read_record(buf,i);
					
					//检查记录是否存在换行符号，不存在则不打印输出
					if(strstr(buf,"\n")==0)
						break;		
					
					//打印记录
					printf(buf);
					
				}
				
				//如果i等于0，代表没有一条记录
				if(i==0)
				{
					printf("There is no record\r\n");
				}
												
			}	

			//清空串口1事件
			g_usart1_event=0;
			
			//清空串口1接收数据缓冲区
			memset((uint8_t *)g_usart1_recv_buf,0,sizeof g_usart1_recv_buf);
			
			//清空串口1接收计数值
			g_usart1_recv_cnt=0;
		}	
	}
}

void RTC_WKUP_IRQHandler(void)
{

	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		g_rtc_wakeup_event = 1;
		
		RTC_ClearITPendingBit(RTC_IT_WUT);
	} 
	
	EXTI_ClearITPendingBit(EXTI_Line22);
}

void USART1_IRQHandler(void)                				//串口1中断服务程序
{
	uint8_t d;
	
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  	//接收中断
	{
		//从串口1接收数据
		g_usart1_recv_buf[g_usart1_recv_cnt]=USART_ReceiveData(USART1);	
		
		//记录多少个数据
		g_usart1_recv_cnt++;
		
		//检测到'#'符或接收的数据满的时候则发送数据
		if(g_usart1_recv_buf[g_usart1_recv_cnt-1]=='#' || g_usart1_recv_cnt>=(sizeof g_usart1_recv_buf)-1)
		{
			g_usart1_event = 1;
			
		}
	} 
} 
