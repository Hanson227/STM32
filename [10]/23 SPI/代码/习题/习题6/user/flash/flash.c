#include "flash.h"

void flash_init(void)
{
	uint8_t d;
	//����FLASH�������������
	FLASH_Unlock();
	

	//�����ر�־λ
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
				FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
							
	//��������4
	if(FLASH_COMPLETE!=FLASH_EraseSector(FLASH_Sector_4,VoltageRange_3))
	{
		printf("FLASH_EraseSector fail\r\n");
		while(1);
	}

	
	//������4�׵�ַ0x8010000д������0x12345678
	if(FLASH_COMPLETE!=FLASH_ProgramWord(0x8010000,0xAABBCCDD))
	{
		printf("FLASH_ProgramWord fail\r\n");
		while(1);
	}
	
	//����FLASH���������޸�FLASH
	FLASH_Lock(); 
	
	d = *(volatile uint32_t *)0x8010000;

	printf("read addr at 0x08010000  data is :0x%08X\r\n",d);
	
	//��ȡ����4�׵�ַ������
}


