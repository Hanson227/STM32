#ifndef __HCSR__H
#define __HCSR__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef		GPIO_InitStructure;

extern uint32_t fre ;
extern uint32_t voice ;

void sr04_init(void);
int32_t sr04_get_distance(void);
void check_dis(uint32_t d);
#endif

