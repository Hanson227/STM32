#ifndef __INFRARED__H
#define __INFRARED__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef GPIO_InitStructure;

int infrared_act(uint8_t *pbuf);
void infrared_init(void);
void infrared_order(uint8_t oder);

#endif

