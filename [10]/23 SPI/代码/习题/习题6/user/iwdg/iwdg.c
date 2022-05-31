#include "iwdg.h"

void iwdg(void)
{
	//ʹ�����ڲ�ʱ��32khz
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//����д����
	
	//���ÿ��Ź��ķ�Ƶֵ���������Ź���ʱ��Ƶ��
	//�������Ź���ʱ��Ƶ�ʾ����˵�ǰ����ֵ
	//�������Ź���ʱ��Ƶ��= 32khz /256��Ƶ = 125hz��
	//ֻҪ����125�μ���������1����ʱ��
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	IWDG_SetReload(375-1);		//3��ιһ�ι�
	
	IWDG_ReloadCounter();		//���¼��ؼ���ֵ����������
	
	IWDG_Enable();				//ʹ�ܿ��Ź�
}

void wwdg(void)
{
	//�������ڿ��Ź�ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG,ENABLE);
	//���÷�Ƶϵ��
	//���ڿ��Ź���Ӳ��ʱ��Ƶ��=(42MHz/4096)/8��1281Hz
	//						��ʱ��Ƶ��/4096��/��Ƶϵ��
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	
	//���ô�������ֵ
	WWDG_SetWindowValue(80);
	
	//���ü���ֵ��ʹ�ܴ��ڿ��Ź�
	WWDG_Enable(127);
}
