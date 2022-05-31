#ifndef __TIM__H
#define __TIM__H

#include "stm32f4xx.h"


static uint32_t tim13_cnt=0;
static GPIO_InitTypeDef		    GPIO_InitStructure;
static TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
static TIM_OCInitTypeDef  		TIM_OCInitStructure;


void tim13_pwm_init(int freq);
void tim13_set_freq(uint32_t freq);
void tim13_set_duty(uint32_t duty);
void tim1_pwm_init(int freq);
void tim14_pwm_init(int freq);
#endif

