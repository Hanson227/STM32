#ifndef __LED__H
#define __LED__H

#include "stm32f4xx.h"
#define BEEP_ON			PFout(8) = 1;
#define BEEP_OFF		PFout(8) = 0;

static GPIO_InitTypeDef		GPIO_InitStructure;

void led_init(void);
void beep_init(void);
#endif

