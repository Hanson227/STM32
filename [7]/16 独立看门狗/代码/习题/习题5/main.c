#include <stdio.h>
#include "stm32f4xx.h"
#include "sys.h"
#include "string.h"
#include "stdlib.h"


GPIO_InitTypeDef   GPIO_InitStructure;

EXTI_InitTypeDef   EXTI_InitStructure;

NVIC_InitTypeDef   NVIC_InitStructure;

USART_InitTypeDef USART_InitStructure;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;


static volatile uint32_t g_iwdg_reset=0;

static volatile uint32_t g_wwdg_reset=0;

static volatile uint8_t  g_usart1_buf[128]={0};
static volatile uint32_t g_usart1_cnt=0;
static volatile uint32_t g_usart1_event=0;


//�ض���(�ض���)fputc����

int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	
	
	return ch;
}


void delay_us(uint32_t nus)
{

	SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳʱ�Ӻ�������üĴ���
	SysTick->LOAD = SystemCoreClock/8/1000000*nus; 		//���ü���ֵ
	SysTick->VAL = 0; 			// Clear current value as well as count flag����յ�ǰֵ���б�־λ
	SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock��ʹ��ϵͳ��ʱ����ʼ���㣬��ʹ��8��Ƶ��ʱ��
	while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set���ȴ��������
	SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳʱ�Ӵ���˵���ٽ��ж�ʱ����	

}

void delay_ms(uint32_t nms)
{
	while(nms --)
	{
		SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳʱ�Ӻ�������üĴ���
		SysTick->LOAD = SystemCoreClock/8/1000; 		// ���ü���ֵ
		SysTick->VAL = 0; 			// Clear current value as well as count flag����յ�ǰֵ���б�־λ
		SysTick->CTRL = 1; 			// Enable SysTick timer with processor clock��ʹ��ϵͳ��ʱ����ʼ���㣬��ʹ��8��Ƶ��ʱ��
		while ((SysTick->CTRL & 0x00010000)==0);// Wait until count flag is set���ȴ��������
		SysTick->CTRL = 0; 			// Disable SysTick���ر�ϵͳʱ�Ӵ���˵���ٽ��ж�ʱ����	
	
	}

}
void tim3_init(void)
{
	/* TIM3 clock enable ����ʱ��3��ʱ��ʹ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Enable the TIM3 gloabal Interrupt ��ʹ�ܶ�ʱ��3��ȫ���ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);



	/* Time base configuration����ʱ���Ļ������ã��������ö�ʱ�����ж�Ƶ��Ϊ1000Hz��Ҳ����˵�೤ʱ�����һ���жϣ���ǰ1ms����һ���ж� */
	TIM_TimeBaseStructure.TIM_Period = (10000/1000)-1;					//���ö�ʱ��Ƶ��
	TIM_TimeBaseStructure.TIM_Prescaler = 8400-1;						//��һ�η�Ƶ�����ΪԤ��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;				//�ڶ��η�Ƶ,��ǰʵ��1��Ƶ��Ҳ���ǲ���Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* TIM Interrupts enable,ʹ�ܶ�ʱ��3�����ж��¼���Ҳ����˵��ʱ�Ѿ�������¼� */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter��ʹ�ܶ�ʱ��3���� */
	TIM_Cmd(TIM3, ENABLE);	
}


void USART1_Init(uint32_t baud)
{

	/* Enable GPIO clock ��ʹ�ܶ�Ӧ��GPIOAʱ��*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//ʹ�ܴ���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* ����1��Ӧ���Ÿ��� */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//����Ϊ����ģʽ��Ҳ����˵ʹ�����ŵĵڶ�����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
				
	USART_InitStructure.USART_BaudRate = baud;						//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λΪ8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);

	
	/* Enable the USART1 Interrupt ��ʹ�ܴ���1�ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USART ��ʹ�ܴ���1����*/
	USART_Cmd(USART1, ENABLE);	
	
	
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);


}

//���Ͷ���ֽ�����
void usart1_send_bytes(uint8_t *pbuf,uint32_t len)
{
	while(len--)
	{
		USART_SendData(USART1,*pbuf++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	}
}


//�����ַ���
void usart1_send_str(uint8_t *pbuf)
{
	while(pbuf && *pbuf)
	{
		USART_SendData(USART1,*pbuf++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	}
}


int main(void)
{

	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF, ENABLE);


	/* Configure PF9 PF10 in output pushpull mode������PF9 PF10����Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;		//��9����10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//�������ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	
	/* Configure PE13 PE14 in output pushpull mode������PE13 PE14����Ϊ���ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;		//��13����14������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//�������ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//����ģʽ��������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			//����IO���ٶ�Ϊ100MHz��Ƶ��Խ������Խ�ã�Ƶ��Խ�ͣ�����Խ��
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;			//����Ҫ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	


	//�����ж����ȼ��ķ���
	//����֧��4����ռ���ȼ���֧��4����Ӧ���ȼ�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	

	PFout(9)=1;
	PFout(10)=1;	
	PEout(13)=1;
	PEout(14)=1;


	USART1_Init(115200);
	
	
	printf("This is WDG Test\r\n");
	
	/* Check if the system has resumed from WWDG reset����鵱ǰϵͳ�Ƿ��ɿ��Ź����и�λ */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{
		/* WWDGRST flag set */
		printf("WWDG RESET OK\r\n");
		

	}
	/* Check if the system has resumed from IWDG reset����鵱ǰϵͳ�Ƿ��ɿ��Ź����и�λ */
	else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("IWDG RESET OK\r\n");
		

	}	
	else
	{
		printf("normal reset cpu\r\n");
	}

	delay_ms(500);
	/* Clear reset flags ����ո�λ��־*/
	RCC_ClearFlag();

	/* WWDG configuration */
	/* Enable WWDG clock ��ʹ�ܴ��ڿ��Ź���ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	
	/* WWDG clock counter = (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us) 
	   ������Ź��ļ���ֵ�ǱȽ����⣬����ֵÿ��1���㣬������ʱ��Ϊ780us

		����ֵ���ֵΪ127������ֵ0x40=64���м����64������ֵ��
	*/
	WWDG_SetPrescaler(WWDG_Prescaler_8);

	//���ô�������ֵ 80
	WWDG_SetWindowValue(80);
	
	
	//ʹ�ܿ��Ź�����ˢ�¼���ֵ==ι��
	WWDG_Enable(127);


	//WWDG NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;							//���ڿ��Ź��ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;					//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;						//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);											//����ָ���Ĳ�����ʼ��VIC�Ĵ���	
	
	
	//�����ǰ�����жϱ�־λ
	WWDG_ClearFlag();
	
	//ʹ����ǰ�����ж�
	WWDG_EnableIT();
	
	/* Enable write access to IWDG_PR and IWDG_RLR registers���������Ź��ļĴ��������ܱ����ģ����ڽ��н������� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: LSI/256 �����ÿ��Ź���ʱ��Ƶ�� = 32KHz/256=125Hz*/
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	//���ÿ��Ź���ι��ʱ�䣬Ҳ�����������ļ���ֵ
	//��ǰд125-1����ôι���ĳ�ʱʱ��Ϊ1�룬���û����1���ڽ���ι����ˢ�¼���ֵ�����ͻḴλCPU
	//��ǰд250-1����ôι���ĳ�ʱʱ��Ϊ2�룬���û����2���ڽ���ι����ˢ�¼���ֵ�����ͻḴλCPU
	//......
	IWDG_SetReload(125-1);
	
	/* Reload IWDG counter �����ؼ���ֵ��ˢ�¼���ֵ==ι��*/
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) ���ÿ��Ź�����*/
	IWDG_Enable();	
	
	//��ʱ��3��ʼ����ÿ1ms����һ���ж�
	tim3_init();


	while(1)
	{
		if(g_usart1_event)
		{
			//�ж��Ƿ���յ�IWDG RESET
			if(strstr((const char *)g_usart1_buf,"IWDG RESET"))
			{
				g_iwdg_reset=1;
			
			}
			
			//�ж��Ƿ���յ�WWDG RESET
			if(strstr((const char *)g_usart1_buf,"WWDG RESET"))
			{
				g_wwdg_reset=1;
			
			}	


			g_usart1_event=0;
			g_usart1_cnt=0;
			memset((uint8_t *)g_usart1_buf,0,sizeof g_usart1_buf);
		
		}
	}

}

void USART1_IRQHandler(void)
{
	uint8_t d;
	
	/* USART in Receiver mode ,����Ƿ���յ���������*/
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{

		/* Receive Transaction data���������� */
		g_usart1_buf[g_usart1_cnt] = (uint8_t)USART_ReceiveData(USART1);	
		
		g_usart1_cnt++;
		
		
		//��⵱ǰ�Ƿ������ɣ���־Ϊ���з������ü�鵱ǰ��������Ƿ����
		if(g_usart1_buf[g_usart1_cnt-1] == '#' || g_usart1_cnt>= sizeof(g_usart1_buf))
		{
			g_usart1_event=1;
		
		}
	}



}

void TIM3_IRQHandler(void)
{
	static int tim3_count=0;
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		tim3_count++;
		
		if(tim3_count>=500)
		{
			if(g_iwdg_reset == 0)
			{
				//ˢ�¼���ֵ���ü���ֵ��0==ι��
				IWDG_ReloadCounter();
			
				tim3_count=0;			
			}
			else
			{
				//һֱռ��CPU���ÿ��Ź���ʱ��λ
				while(1);
			}

		
		}
		//��ձ�־λ������CPU���Ѿ���ɶ�ʱ��3�ĸ����жϣ����Դ����µ�һ���ж���
	
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}


}

void WWDG_IRQHandler(void)
{
	//�ܹ�������жϷ�������Ҳ����˵��ǰ���ڿ��Ź��ļ���ֵ==0x40
	if(WWDG_GetFlagStatus() == SET)
	{
		if(g_wwdg_reset == 0)
		{
			/* Update WWDG counter ��ˢ�¼���ֵ==ι��*/
			WWDG_SetCounter(127);
	
			//�����ǰ���ѱ�־λ
			WWDG_ClearFlag();
		}
		else
		{
			while(1);
		}

	}
}

