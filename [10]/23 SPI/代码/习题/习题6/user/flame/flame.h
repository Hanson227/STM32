#ifndef __FLAME__H
#define __FLAME__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef	  GPIO_InitStructure;
static EXTI_InitTypeDef   EXTI_InitStructure;
static NVIC_InitTypeDef   NVIC_InitStructure;

static ADC_InitTypeDef       ADC_InitStructure;
static ADC_CommonInitTypeDef ADC_CommonInitStructure;

void flame_init(void);

#endif

