#ifndef __SPI_H_
#define __SPI_H_

#include "stm32f4xx.h"

void w25qxx_init(void);
uint8_t spi1_send_byte(uint8_t txd);
uint16_t w25qxx_read_id(void);
void write_to_w25q128(uint32_t addr,uint8_t len,uint8_t *buf);
void w25qxx_read_data(uint32_t addr,uint8_t *buf,uint32_t len);

void Sector_Erase(uint32_t addr);
void sFLASH_WaitForWriteEnd(void);

void simulate_w25qxx_init(void);
uint8_t simulate_spi1_send_byte(uint8_t txd);


#endif



