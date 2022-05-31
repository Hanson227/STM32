#include "rc522.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>
#include "./user/delay/delay.h"
#include "./user/led/led.h"
#include "./user/wenshi/wenshi.h"

GPIO_InitTypeDef GPIO_InitStruct;
SPI_InitTypeDef  SPI_InitStructure;

//MFRC522数据区
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8  card_writebuf[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
u8  card_readbuf[18];

//数字的ASCII码
uc8 numberascii[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//显示缓冲区
u8  dispnumber5buf[6];
u8  dispnumber3buf[4];
u8  dispnumber2buf[3];
//MFRC522数据区
u8  mfrc552pidbuf[18];
u8  card_pydebuf[2];
u8  card_numberbuf[5];
u8  card_readbuf[18];



void rc522_init(void) 
{ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	
	//pins: CS = PD6,SCK=PD7, MOSI=PC6，RST=PC11,MISO=PC8
	
	//配置片选线cs，时钟线sck
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStruct);

								//MOSI		//RST
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStruct);

	//配置主入从出为输入模式
	// MISO
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed=GPIO_High_Speed;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	//模式3
	PDout(6) = 1;//ss时序图，片选信号为高电平
	PDout(7) = 1;//sck时序图，时钟信号为高电平
	PCout(6) = 1;//mosi主出从入，随意高低信号
}

//发送数据，并接收数据
uint8_t SPI3_Send(u8 val)  
{ 
	//pins: CS = PD6,SCK=PD7, MOSI=PC6，RST=PC11,MISO=PC8
	int32_t i = 0;
	uint8_t d = 0;	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!必须是32位
	
	for(i=7;i>=0;i--)
	{
		//准备数据，放入mosi口
		if((val & (1<<i)) )
			PCout(6) = 1;
		else
			PCout(6) = 0;
		
		//拉低电平，准备发送
		PDout(7) = 0;
		delay_us(2);
		
		//发出数据
		PDout(7) = 1;
		delay_us(2);
		
		//同步接收数据
		if(PCin(8))
			d|= 1<<i;
	}
	return d;
}




//功能描述向MFRC522的某一寄存器写一个字节数据
//输入参数addr--寄存器地址val--要写入的值
void Write_MFRC522(u8 addr, u8 val) 
{
	//地址格式0XXXXXX0  
	MFRC522_CS(0);   
	SPI3_Send((addr<<1)&0x7E);  
	SPI3_Send(val);    
	MFRC522_CS(1); 
}
//功能描述从MFRC522的某一寄存器读一个字节数据
//输入参数addr--寄存器地址
//返 回 值返回读取到的一个字节数据 
u8 Read_MFRC522(u8 addr) 
{  
	u8 val;
	//地址格式1XXXXXX0   
	MFRC522_CS(0);     
	SPI3_Send(((addr<<1)&0x7E)|0x80);   
	val = SPI3_Send(0x00);	
	MFRC522_CS(1); 
	//   
	return val;  
}



									//下面两个函数只对能读写位有效
//功能描述置RC522寄存器位
//输入参数reg--寄存器地址;mask--置位值
void SetBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;
	//     
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp|mask);  // set bit mask 
}

//功能描述清RC522寄存器位
//输入参数reg--寄存器地址;mask--清位值
void ClearBitMask(u8 reg, u8 mask)   
{     
	u8 tmp=0;
	//     
	tmp=Read_MFRC522(reg);     
	Write_MFRC522(reg,tmp&(~mask));  //clear bit mask 
}



//功能描述开启天线,每次启动或关闭天线发射之间应至少有1ms的间隔
void AntennaOn(void) 
{  
	u8 temp;
	//   
	temp=Read_MFRC522(TxControlReg);  
	if ((temp&0x03)==0)  
	{   
		SetBitMask(TxControlReg,0x03);  
	}
}
//功能描述关闭天线,每次启动或关闭天线发射之间应至少有1ms的间隔
void AntennaOff(void) 
{  
	ClearBitMask(TxControlReg,0x03);
}



//功能描述复位MFRC522
void MFRC522_Reset(void) 
{ 
	//外复位可以不用
	MFRC522_Rst(1);
	delay_us(1);
	MFRC522_Rst(0);
	delay_us(1);
	MFRC522_Rst(1);
	delay_us(1);
	//内复位   
	Write_MFRC522(CommandReg, PCD_RESETPHASE); 
}

//功能描述初始化MFRC522
void MFRC522_Initializtion(void) 
{
	rc522_init();  
	MFRC522_Reset();         
	//Timer: TPrescaler*TreloadVal/6.78MHz = 0xD3E*0x32/6.78=25ms     
	Write_MFRC522(TModeReg,0x8D);				//TAuto=1为自动计数模式，受通信协议影向。低4位为预分频值的高4位
	//Write_MFRC522(TModeReg,0x1D);				//TAutoRestart=1为自动重载计时，0x0D3E是0.5ms的定时初值//test    
	Write_MFRC522(TPrescalerReg,0x3E); 			//预分频值的低8位     
	Write_MFRC522(TReloadRegL,0x32);			//计数器的低8位                
	Write_MFRC522(TReloadRegH,0x00);			//计数器的高8位       
	Write_MFRC522(TxAutoReg,0x40); 				//100%ASK     
	Write_MFRC522(ModeReg,0x3D); 				//CRC初始值0x6363
	Write_MFRC522(CommandReg,0x00);				//启动MFRC522  
	//Write_MFRC522(RFCfgReg, 0x7F);   			 //RxGain = 48dB调节卡感应距离      
	AntennaOn();          						//打开天线 
}

//功能描述RC522和ISO14443卡通讯
//输入参数command--MF522命令字
//					sendData--通过RC522发送到卡片的数据
//					sendLen--发送的数据长度
//					BackData--接收到的卡片返回数据
//					BackLen--返回数据的位长度
//返 回 值成功返回MI_O
u8 MFRC522_ToCard(u8 command, u8 *sendData, u8 sendLen, u8 *backData, u16 *backLen) 
{
	u8  status=MI_ERR;
	u8  irqEn=0x00;
	u8  waitIRq=0x00;
	u8  lastBits;
	u8  n;
	u16 i;
	//根据命预设中断参数
	switch (command)     
	{         
		case PCD_AUTHENT:  		//认证卡密   
			irqEn 	= 0x12;			//    
			waitIRq = 0x10;			//    
			break;
		case PCD_TRANSCEIVE: 	//发送FIFO中数据      
			irqEn 	= 0x77;			//    
			waitIRq = 0x30;			//    
			break;      
		default:    
			break;     
	}
	//
	Write_MFRC522(ComIEnReg, irqEn|0x80);		//允许中断请求     
	ClearBitMask(ComIrqReg, 0x80);  				//清除所有中断请求位               	
	SetBitMask(FIFOLevelReg, 0x80);  				//FlushBuffer=1, FIFO初始化
	Write_MFRC522(CommandReg, PCD_IDLE); 		//使MFRC522空闲   
	//向FIFO中写入数据     
	for (i=0; i<sendLen; i++)
		Write_MFRC522(FIFODataReg, sendData[i]);
	//执行命令
	Write_MFRC522(CommandReg, command);
	//天线发送数据     
	if (command == PCD_TRANSCEIVE)					//如果是卡片通信命令，MFRC522开始向天线发送数据      
		SetBitMask(BitFramingReg, 0x80);  		//StartSend=1,transmission of data starts      
	//等待接收数据完成     
	i = 10000; //i根据时钟频率调整操作M1卡最大等待时间25ms     
	do      
	{        
		n = Read_MFRC522(ComIrqReg);
		//irq_regdata=n;	//test         
		i--;
		//wait_count=i;		//test		     
	}while ((i!=0) && !(n&0x01) && !(n&waitIRq));	//接收完就退出n=0x64
	//停止发送
	ClearBitMask(BitFramingReg, 0x80);   		//StartSend=0
	//如果在25ms内读到卡
	if (i != 0)     
	{            
		if(!(Read_MFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr         
		{            
			if (n & irqEn & 0x01)			//                  
				status = MI_NOTAGERR;		//
			//
			if (command == PCD_TRANSCEIVE)             
			{                 
				n = Read_MFRC522(FIFOLevelReg);		//n=0x02                
				lastBits = Read_MFRC522(ControlReg) & 0x07;	//lastBits=0               
				if (lastBits!=0)                         
					*backLen = (n-1)*8 + lastBits; 
				else
					*backLen = n*8;									//backLen=0x10=16
				//
				if (n == 0)                         
				 	n = 1;                        
				if (n > MAX_LEN)         
				 	n = MAX_LEN;
				//
				for (i=0; i<n; i++)                 
					backData[i] = Read_MFRC522(FIFODataReg); 
			}
			//
			status = MI_OK;		
		}
		else
			status = MI_ERR;
	}	
	//
	Write_MFRC522(ControlReg,0x80);				//timer stops     
	Write_MFRC522(CommandReg, PCD_IDLE);	//
	//
	return status;
}

//功能描述寻卡读取卡类型号
//输入参数reqMode--寻卡方式
//					TagType--返回卡片类型
//					0x4400 = Mifare_UltraLight
//					0x0400 = Mifare_One(S50)
//					0x0200 = Mifare_One(S70)
//					0x0800 = Mifare_Pro(X)
//					0x4403 = Mifare_DESFire
//返 回 值成功返回MI_OK	
u8 MFRC522_Request(u8 reqMode, u8 *TagType)
{  
	u8  status;    
	u16 backBits;   //接收到的数据位数
	//   
	Write_MFRC522(BitFramingReg, 0x07);  //TxLastBists = BitFramingReg[2..0]   
	TagType[0] = reqMode;  
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits); 
	// 
	if ((status != MI_OK) || (backBits != 0x10))  
	{       
		status = MI_ERR;
	}
	//  
	return status; 
}

//功能描述防冲突检测读取选中卡片的卡序列号
//输入参数serNum--返回4字节卡序列号,第5字节为校验字节
//返 回 值成功返回MI_OK
u8 MFRC522_Anticoll(u8 *serNum) 
{     
	u8  status;     
	u8  i;     
	u8  serNumCheck=0;     
	u16 unLen;
	//           
	ClearBitMask(Status2Reg, 0x08);  			//TempSensclear     
	ClearBitMask(CollReg,0x80);   				//ValuesAfterColl  
	Write_MFRC522(BitFramingReg, 0x00);  	    //TxLastBists = BitFramingReg[2..0]
	serNum[0] = PICC_ANTICOLL1;     
	serNum[1] = 0x20;     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
	//      
	if (status == MI_OK)
	{   
		//校验卡序列号   
		for(i=0;i<4;i++)   
			serNumCheck^=serNum[i];
		//
		if(serNumCheck!=serNum[i])        
			status=MI_ERR;
	}
	SetBitMask(CollReg,0x80);  //ValuesAfterColl=1
	//      
	return status;
}

//功能描述用MF522计算CRC
//输入参数pIndata--要读数CRC的数据len--数据长度pOutData--计算的CRC结果
void CalulateCRC(u8 *pIndata, u8 len, u8 *pOutData) 
{     
	u16 i;
	u8  n;
	//      
	ClearBitMask(DivIrqReg, 0x04);   			//CRCIrq = 0     
	SetBitMask(FIFOLevelReg, 0x80);   		//清FIFO指针     
	Write_MFRC522(CommandReg, PCD_IDLE);   
	//向FIFO中写入数据      
	for (i=0; i<len; i++)
		Write_MFRC522(FIFODataReg, *(pIndata+i));
	//开始RCR计算
	Write_MFRC522(CommandReg, PCD_CALCCRC);
	//等待CRC计算完成     
	i = 1000;     
	do      
	{         
		n = Read_MFRC522(DivIrqReg);         
		i--;     
	}while ((i!=0) && !(n&0x04));   //CRCIrq = 1
	//读取CRC计算结果     
	pOutData[0] = Read_MFRC522(CRCResultRegL);     
	pOutData[1] = Read_MFRC522(CRCResultRegH);
	Write_MFRC522(CommandReg, PCD_IDLE);
}
//功能描述选卡读取卡存储器容量
//输入参数serNum--传入卡序列号
//返 回 值成功返回卡容量
u8 MFRC522_SelectTag(u8 *serNum) 
{     
	u8  i;     
	u8  status;     
	u8  size;     
	u16 recvBits;     
	u8  buffer[9];
	//     
	buffer[0] = PICC_ANTICOLL1;	//防撞码1     
	buffer[1] = 0x70;
	buffer[6] = 0x00;						     
	for (i=0; i<4; i++)					
	{
		buffer[i+2] = *(serNum+i);	//buffer[2]-buffer[5]为卡序列号
		buffer[6]  ^=	*(serNum+i);	//卡校验码
	}
	//
	CalulateCRC(buffer, 7, &buffer[7]);	//buffer[7]-buffer[8]为RCR校验码
	ClearBitMask(Status2Reg,0x08);
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
	//
	if ((status == MI_OK) && (recvBits == 0x18))    
		size = buffer[0];     
	else    
		size = 0;
	//	     
	return size; 
}
//功能描述验证卡片密码
//输入参数authMode--密码验证模式
//					0x60 = 验证A密钥
//					0x61 = 验证B密钥
//					BlockAddr--块地址
//					Sectorkey--扇区密码
//					serNum--卡片序列号4字节
//返 回 值成功返回MI_OK
u8 MFRC522_Auth(u8 authMode, u8 BlockAddr, u8 *Sectorkey, u8 *serNum) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[12];    
	//验证模式+块地址+扇区密码+卡序列号     
	buff[0] = authMode;		//验证模式     
	buff[1] = BlockAddr;	//块地址     
	for (i=0; i<6; i++)
		buff[i+2] = *(Sectorkey+i);	//扇区密码
	//
	for (i=0; i<4; i++)
		buff[i+8] = *(serNum+i);		//卡序列号
	//
	status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
	//      
	if ((status != MI_OK) || (!(Read_MFRC522(Status2Reg) & 0x08)))
		status = MI_ERR;
	//
	return status;
}
//功能描述读块数据
//输入参数blockAddr--块地址;recvData--读出的块数据
//返 回 值成功返回MI_OK
u8 MFRC522_Read(u8 blockAddr, u8 *recvData) 
{     
	u8  status;     
	u16 unLen;
	//      
	recvData[0] = PICC_READ;     
	recvData[1] = blockAddr;     
	CalulateCRC(recvData,2, &recvData[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
	//
	if ((status != MI_OK) || (unLen != 0x90))
		status = MI_ERR;
	//
	return status;
}
//功能描述写块数据
//输入参数blockAddr--块地址;writeData--向块写16字节数据
//返 回 值成功返回MI_OK
u8 MFRC522_Write(u8 blockAddr, u8 *writeData) 
{     
	u8  status;     
	u16 recvBits;     
	u8  i;  
	u8  buff[18];
	//           
	buff[0] = PICC_WRITE;     
	buff[1] = blockAddr;     
	CalulateCRC(buff, 2, &buff[2]);     
	status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
	//
	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		status = MI_ERR;
	//
	if (status == MI_OK)     
	{         
		for (i=0; i<16; i++)  //向FIFO写16Byte数据                     
			buff[i] = *(writeData+i);
		//                     
		CalulateCRC(buff, 16, &buff[16]);         
		status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);           
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))               
			status = MI_ERR;         
	}          
	return status;
}
//功能描述命令卡片进入休眠状态
void MFRC522_Halt(void) 
{    
	u16 unLen;     
	u8  buff[4];
	//       
	buff[0] = PICC_HALT;     
	buff[1] = 0;     
	CalulateCRC(buff, 2, &buff[2]);       
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}


//RFID模块测试
void MFRC522Test(void)
{
	u8 i,j,status,card_size;
	//
	status=MFRC522_Request(0x52, card_pydebuf);			//寻卡
	//
	if(status==0)		//如果读到卡
	{
		status=MFRC522_Anticoll(card_numberbuf);			//防撞处理			
		card_size=MFRC522_SelectTag(card_numberbuf);	//选卡
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//验卡
		status=MFRC522_Write(4, card_writebuf);				//写卡（写卡要小心，特别是各区的块3）
		status=MFRC522_Read(4, card_readbuf);					//读卡
		//MFRC522_Halt();															//使卡进入休眠状态
		//卡类型显示
		printf("卡类型显示[0]:%x\n",card_pydebuf[0]);
		printf("卡类型显示[1]:%x\n",card_pydebuf[1]);
		
		
		//卡序列号显，最后一字节为卡的校验码
		for(i=0;i<5;i++)
		{
			
			printf("%x",card_pydebuf[i]);
			if(i==4)		//显示校验码
				printf("校验码:%x",card_numberbuf[i]);
		}
		printf("\n");
		//卡容量显示，单位为Kbits
		printf("卡容量:%d\n",card_size);
		

		
		//读卡状态显示，正常为0
		printf("状态:%x\n",status);
		

		
		//读一个块的数据显示
		for(i=0;i<2;i++)		//分两行显示
		{
			for(j=0;j<9;j++)	//每行显示8个
			{
				printf("%x",card_readbuf[j+i*9]);
			}
		}

		
		BEEP_ON;
		delay_ms(100);
		BEEP_OFF;
		delay_ms(100);
		BEEP_ON;
		delay_ms(100);
		BEEP_OFF;
		delay_ms(100);
		
	}	
}

void MFRC522_humiture(void)
{
	static uint32_t Whether_first = 0;
	
	char rc522_card_num[10] = {0};
	uint8_t arr[5] = {0};
	u8 i=0,j=0,status=0,card_size=0;
	
	status=MFRC522_Request(0x52, card_pydebuf);			//寻卡
	
	if(status==0)		//如果读到卡
	{
		status=MFRC522_Anticoll(card_numberbuf);						//防撞处理			
		card_size=MFRC522_SelectTag(card_numberbuf);					//选卡
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);	//验卡
		
		memset(card_writebuf,0,sizeof(card_writebuf));
		
		//字符串内容的格式化
		//63 44 2B 3E -> "63442b3e"
		sprintf(rc522_card_num,"%02x%02x%02x%02x",card_numberbuf[0],card_numberbuf[1],card_numberbuf[2],card_numberbuf[3]);
		
		
		printf("find card num:%s\n",rc522_card_num);
		
		//字符串比较，匹配成功则为有效卡
		//【*】测试的时候记得修改有效卡的号码
		if((strcmp(rc522_card_num,"63442b3e")) == 0)
		{
			//常亮LED1
			PFout(9) = 0;
			
			printf("卡类型显示[0]:%x\n",card_pydebuf[0]);
			printf("卡类型显示[1]:%x\n",card_pydebuf[1]);
			
		
			//卡容量显示，单位为Kbits
			printf("卡容量:%d\n",card_size);

			//读卡状态显示，正常为0
			printf("状态:%x\n",status);
			
			if(Whether_first != 0)
			{
				status=MFRC522_Read(4, card_readbuf);							//读卡
				
				printf("年---月-日-周-----时-分-秒-温--湿\n");
				//读一个块的数据显示
				for(i=0;i<12;i++)		//分两行显示
				{
					if(i>=0 && i<=6)
					{
						if(i==0) printf("20%02x-",card_readbuf[i]);
						else if(i==3) printf("week:%02x-",card_readbuf[i]);
						else printf("%02x-",card_readbuf[i]);
					}
					else if(i>6&&i<=11)
					{
						if(i==11)	printf("%c",card_readbuf[i]);
						else if(i%2)	printf("%d.",card_readbuf[i]);
						else	printf("%d-",card_readbuf[i]);
					}
				}
				printf("\n");
				
			}
			else
			{
				
				printf("No data at present\n ");
				Whether_first = 1;
				
			}
			
			dht11_read(arr);
			delay_ms(2000);	
		
			//获取时间
			RTC_GetTime(RTC_Format_BCD,&RTC_TimeStructure);
		
			//获取日期
			RTC_GetDate(RTC_Format_BCD,&RTC_DateStructure);		
		
			//填充日期、时间、温度值、湿度值、有效记录的标记
			card_writebuf[0] = RTC_DateStructure.RTC_Year;
			card_writebuf[1] = 	RTC_DateStructure.RTC_Month;
			card_writebuf[2] = 	RTC_DateStructure.RTC_Date;
			card_writebuf[3] = 	RTC_DateStructure.RTC_WeekDay;
			card_writebuf[4] =  RTC_TimeStructure.RTC_Hours;
			card_writebuf[5] =  RTC_TimeStructure.RTC_Minutes;
			card_writebuf[6] = 	RTC_TimeStructure.RTC_Seconds;
			card_writebuf[7] = 	arr[2];
			card_writebuf[8] = 	arr[3];
			card_writebuf[9] = 	arr[0];
			card_writebuf[10] = arr[1];
		
			card_writebuf[11] = 35;								//有效记录的标记
		
			status=MFRC522_Write(4, card_writebuf);							//写卡（写卡要小心，特别是各区的块3）
			//printf("ss:%d",status);
			//写入成功，蜂鸣器滴一声示意
			if(status == MI_OK)
			{
				BEEP_ON;
				delay_ms(100);
				BEEP_OFF;
			}
			//写入失败，蜂鸣器滴两声示意
			else
			{
				BEEP_ON;
				delay_ms(100);
				BEEP_OFF;
				delay_ms(100);
				BEEP_ON;
				delay_ms(100);
				BEEP_OFF;
			}
		}
		//字符串比较，匹配失败则为非法卡
		else
		{

			BEEP_ON;
			printf("find illegal card\r\n");

		}
		
		//等待卡离开
		while(1)
		{
			//重新初始化rc522
			MFRC522_Initializtion();

			//寻卡
			status=MFRC522_Request(0x52, card_pydebuf);			
		
			//找不到卡则卡离开有效的识别距离
			if(status != MI_OK)
			{
				//卡已离开
				printf("the card has left\r\n");
				
				return;
			}
			else
			{
				//卡还在
				printf("the card is still there\r\n");			
			}
			
			delay_ms(1000);
		
		}
		
	}	
	//寻卡失败：1）卡离开有效的识别距离；2）那张卡不是M1卡
	else	
	{
	
		PFout(9) = 1;
		BEEP_OFF;
	}
}


