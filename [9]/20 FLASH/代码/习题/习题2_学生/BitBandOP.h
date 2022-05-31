/**
  ******************************************************************************
  * @file    Templates/usr/BitBandOP.h 
  * @author  YDF
  * @version V1.0
  * @date    2018-11-24
  * @brief   位带操作头文件
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BIT_BAND_OP_H
#define __BIT_BAND_OP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define  BitBand  //使用位带操作宏定义

#define  AddrMap(addr)			*(vu32 *)(addr)

#define  PAODR_addr_BAND		0x42000000+(GPIOA_BASE+0x14-0x40000000)*32
#define  PBODR_addr_BAND		0x42000000+(GPIOB_BASE+0x14-0x40000000)*32
#define  PCODR_addr_BAND		0x42000000+(GPIOC_BASE+0x14-0x40000000)*32
#define  PDODR_addr_BAND		0x42000000+(GPIOD_BASE+0x14-0x40000000)*32
#define  PEODR_addr_BAND		0x42000000+(GPIOE_BASE+0x14-0x40000000)*32
#define  PFODR_addr_BAND		0x42000000+(GPIOF_BASE+0x14-0x40000000)*32
#define  PGODR_addr_BAND		0x42000000+(GPIOG_BASE+0x14-0x40000000)*32

#define  PAIDR_addr_BAND		0x42000000+(GPIOA_BASE+0x10-0x40000000)*32
#define  PBIDR_addr_BAND		0x42000000+(GPIOB_BASE+0x10-0x40000000)*32
#define  PCIDR_addr_BAND		0x42000000+(GPIOC_BASE+0x10-0x40000000)*32
#define  PDIDR_addr_BAND		0x42000000+(GPIOD_BASE+0x10-0x40000000)*32
#define  PEIDR_addr_BAND		0x42000000+(GPIOE_BASE+0x10-0x40000000)*32
#define  PFIDR_addr_BAND		0x42000000+(GPIOF_BASE+0x10-0x40000000)*32
#define  PGIDR_addr_BAND		0x42000000+(GPIOG_BASE+0x10-0x40000000)*32


#define  PAOut(n)			AddrMap(PAODR_addr_BAND+ n*4)
#define  PBOut(n)			AddrMap(PBODR_addr_BAND+ n*4)
#define  PCOut(n)			AddrMap(PCODR_addr_BAND+ n*4)
#define  PDOut(n)			AddrMap(PDODR_addr_BAND+ n*4)
#define  PEOut(n)			AddrMap(PEODR_addr_BAND+ n*4)
#define  PFOut(n)			AddrMap(PFODR_addr_BAND+ n*4)
#define  PGOut(n)			AddrMap(PGODR_addr_BAND+ n*4)


#define  PAIn(n)			AddrMap(PAIDR_addr_BAND+ n*4)
#define  PBIn(n)			AddrMap(PBIDR_addr_BAND+ n*4)
#define  PCIn(n)			AddrMap(PCIDR_addr_BAND+ n*4)
#define  PDIn(n)			AddrMap(PDIDR_addr_BAND+ n*4)
#define  PEIn(n)			AddrMap(PEIDR_addr_BAND+ n*4)
#define  PFIn(n)			AddrMap(PFIDR_addr_BAND+ n*4)
#define  PGIn(n)			AddrMap(PGIDR_addr_BAND+ n*4)

/* Definations ------------------------------------------------------------------*/

/* Declarations ------------------------------------------------------------------*/


#endif /* __BIT_BAND_OP_H */

/********END OF FILE****/
