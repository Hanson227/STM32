#ifndef _HUMITURE_H
#define _HUMITURE_H

#include "stm32f4xx.h"
#include "BitBandOP.h"
#include "led.h"

void delay_us(uint32_t n);

void delay_ms(uint32_t n);

void humi_init(void);

int humi_start(uint8_t *buf);







#endif
