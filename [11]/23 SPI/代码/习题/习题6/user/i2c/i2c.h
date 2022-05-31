#ifndef __I2C__H
#define __I2C__H

#include "stm32f4xx.h"



void at24cxx_init(void);

void sda_pin_mode(GPIOMode_TypeDef pin_mode);

void i2c_start(void);

void i2c_stop(void);

void i2c_send_byte(uint8_t txd);

uint8_t i2c_wait_ack(void);

int32_t at24cxx_write(uint8_t addr,uint8_t *pbuf,uint8_t len);

int32_t at24cxx_read(uint8_t addr,uint8_t *pbuf,uint8_t len);
#endif

