#include "flash.h"

void flash_init(void)
{
	uint8_t d;
	//解锁FLASH，才能允许访问
	FLASH_Unlock();
	

	//清空相关标志位
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
							
	//擦除扇区4
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3))
	{
		printf("FLASH_EraseSector fail\r\n");
		while(1);
	}

	
	//向扇区4首地址0x8010000写入数据0x12345678
	if(FLASH_COMPLETE!=FLASH_ProgramWord(0x8010000,0xAABBCCDD))
	{
		printf("FLASH_ProgramWord fail\r\n");
		while(1);
	}
	
	//锁定FLASH，不允许修改FLASH
	FLASH_Lock(); 
	
	d = *(volatile uint32_t *)0x8010000;

	printf("read addr at 0x08010000  data is :0x%08X\r\n",d);
	
	//读取扇区4首地址的数据
}


