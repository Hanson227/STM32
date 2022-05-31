#ifndef __KEY__H
#define __KEY__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef	  GPIO_InitStruct;
static EXTI_InitTypeDef   EXTI_InitStructure;
static NVIC_InitTypeDef   NVIC_InitStructure;

void key_init(void);

#endif

