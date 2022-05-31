#ifndef __USART__H
#define __USART__H

#include "stm32f4xx.h"

static GPIO_InitTypeDef		GPIO_InitStructure;
static NVIC_InitTypeDef		NVIC_InitStructure;
static USART_InitTypeDef 	USART_InitStructure;


void usart1_init(uint32_t baud);
void usart3_init(uint32_t baud);
void usart3_send_str(const char *pstr);

#endif

