#ifndef __BLUE__H
#define __BLUE__H

#include "stm32f4xx.h"


extern char buf[30];			//�����������͵�ָ��
extern uint32_t voice; 			//����������
void ble_set_config(void);
void blue_change_beep(void);
void blue_change_time(void);
#endif

