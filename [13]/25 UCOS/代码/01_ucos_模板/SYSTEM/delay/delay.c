#include "delay.h"
#include "sys.h"

#include "includes.h"							//ucos ʹ��	  



static uint16_t fac_us=0;						//us��ʱ������			   
static uint16_t fac_ms=0;						//ms��ʱ������,��os��,����ÿ�����ĵ�ms��
	
//systick�жϷ�����,ʹ��OSʱ�õ�
void SysTick_Handler(void)
{	
	if(OSRunning==OS_STATE_OS_RUNNING)			//OS��ʼ����,��ִ�������ĵ��ȴ���
	{
		OSIntEnter();							//�����ж�
		OSTimeTick();       					//����ucos��ʱ�ӷ������               
		OSIntExit();       	 					//���������л����ж�
	}
}

			   
//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
void systick_init(void)
{

	uint32_t reload;

 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//SYSTICKʹ���ⲿʱ��Դ	 
	
	fac_us=SystemCoreClock/8/1000000;					//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��

	reload=SystemCoreClock/8/1000000;					//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/OSCfg_TickRate_Hz;					//����OSCfg_TickRate_Hz�趨���ʱ��
														//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	fac_ms=1000/OSCfg_TickRate_Hz;						//����OS������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;			//����SYSTICK�ж�
	SysTick->LOAD=reload; 								//ÿ1/OSCfg_TickRate_Hz���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; 			//����SYSTICK
}								    

    								   
void delay_us(uint32_t nus)
{		
	OS_ERR err; 
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						//��Ҫ�Ľ����� 
	
	OSSchedLock(&err);						//��ֹOS���ȣ���ֹ���us��ʱ
	
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	
	while(1)
	{
		tnow=SysTick->VAL;					//��ȡ��ǰ����ֵ
		
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;			//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else 
				tcnt+=reload-tnow+told;
			
			told=tnow;
			
			if(tcnt>=ticks)
				break;						//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}
	
	OSSchedUnlock(&err);					//�ָ�OS����											    
}  

void delay_ms(uint16_t nms)
{	
	OS_ERR err; 
	
	if(OSRunning&&OSIntNestingCtr==0)		//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if(nms>=fac_ms)						//��ʱ��ʱ�����OS������ʱ������ 
			OSTimeDly(nms/fac_ms,OS_OPT_TIME_PERIODIC,&err);
		
		nms%=fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	
	delay_us((uint32_t)(nms*1000));			//��ͨ��ʽ��ʱ
}





































