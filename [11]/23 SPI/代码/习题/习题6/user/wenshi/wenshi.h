#ifndef __WEISHI__H
#define __WEISHI__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef		GPIO_InitStructure;

void dht11_init(void);
void dht11_pin_mode(GPIOMode_TypeDef pin_mode);
int32_t dht11_read(uint8_t *pbuf);

#endif

