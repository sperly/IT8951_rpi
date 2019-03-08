#include "IT8951_HW.h"

void IT8951_HW_Init()
{
	if (!bcm2835_init()) 
	{
		printf("bcm2835_init error \n");
		return 1;
	}
	
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);   		//default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);               		//default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);		//default
	
	bcm2835_gpio_fsel(CS, BCM2835_GPIO_FSEL_OUTP);  
	bcm2835_gpio_fsel(HRDY, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RESET, BCM2835_GPIO_FSEL_OUTP);
	
	bcm2835_gpio_write(CS, HIGH);

	printf("****** IT8951 HW ******\n");
	
	bcm2835_gpio_write(RESET, LOW);
	bcm2835_delay(100);
	bcm2835_gpio_write(RESET, HIGH);
}

//-----------------------------------------------------------
//Host controller function 1---Wait for host data Bus Ready
//-----------------------------------------------------------
void IT8951_HW_WaitForReady()
{
	uint8_t ulData = bcm2835_gpio_lev(HRDY);
	while(ulData == 0)
	{
		ulData = bcm2835_gpio_lev(HRDY);
	}
}

//-----------------------------------------------------------
//Host controller function 2---Write command code to host data Bus
//-----------------------------------------------------------
void IT8951_HW_WriteCmdCode(uint16_t usCmdCode)
{
	//Set Preamble for Write Command
	uint16_t wPreamble = 0x6000; 
	
	IT8951_HW_WaitForReady();	

	bcm2835_gpio_write(CS,LOW);
	
	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);
	
	IT8951_HW_WaitForReady();	
	
	bcm2835_spi_transfer(usCmdCode>>8);
	bcm2835_spi_transfer(usCmdCode);
	
	bcm2835_gpio_write(CS,HIGH); 
}

//-----------------------------------------------------------
//Host controller function 3---Write Data to host data Bus
//-----------------------------------------------------------
void IT8951_HW_WriteData(uint16_t usData)
{
	//Set Preamble for Write Data
	uint16_t wPreamble	= 0x0000;

	IT8951_HW_WaitForReady();

	bcm2835_gpio_write(CS,LOW);

	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);
	
	IT8951_HW_WaitForReady();
			
	bcm2835_spi_transfer(usData>>8);
	bcm2835_spi_transfer(usData);
	
	bcm2835_gpio_write(CS,HIGH); 
}

void IT8951_HW_WriteNData(uint16_t* pwBuf, uint32_t ulSizeWordCnt)
{
	uint32_t i;

	uint16_t wPreamble	= 0x0000;

	IT8951_HW_WaitForReady();

	bcm2835_gpio_write(CS,LOW);
	
	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);
	
	IT8951_HW_WaitForReady();

	for(i=0;i<ulSizeWordCnt;i++)
	{
		bcm2835_spi_transfer(pwBuf[i]>>8);
		bcm2835_spi_transfer(pwBuf[i]);
	}
	
	bcm2835_gpio_write(CS,HIGH); 
}  

//-----------------------------------------------------------
//Host controller function 4---Read Data from host data Bus
//-----------------------------------------------------------
uint16_t IT8951_HW_ReadData()
{
	uint16_t wRData; 
	
	uint16_t wPreamble = 0x1000;

	IT8951_HW_WaitForReady();

	bcm2835_gpio_write(CS,LOW);
		
	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);

	IT8951_HW_WaitForReady();
	
	wRData=bcm2835_spi_transfer(0x00);//dummy
	wRData=bcm2835_spi_transfer(0x00);//dummy
	
	IT8951_HW_WaitForReady();
	
	wRData = bcm2835_spi_transfer(0x00)<<8;
	wRData |= bcm2835_spi_transfer(0x00);
		
	bcm2835_gpio_write(CS,HIGH); 
		
	return wRData;
}

//-----------------------------------------------------------
//  Read Burst N words Data
//-----------------------------------------------------------
void IT8951_HW_ReadNData(uint16_t* pwBuf, uint32_t ulSizeWordCnt)
{
	uint32_t i;
	
	uint16_t wPreamble = 0x1000;

	IT8951_HW_WaitForReady();
	
	bcm2835_gpio_write(CS,LOW);

	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);
	
	IT8951_HW_WaitForReady();
	
	pwBuf[0]=bcm2835_spi_transfer(0x00);//dummy
	pwBuf[0]=bcm2835_spi_transfer(0x00);//dummy
	
	IT8951_HW_WaitForReady();
	
	for(i=0;i<ulSizeWordCnt;i++)
	{
		pwBuf[i] = bcm2835_spi_transfer(0x00)<<8;
		pwBuf[i] |= bcm2835_spi_transfer(0x00);
	}
	
	bcm2835_gpio_write(CS,HIGH); 
}

//-----------------------------------------------------------
//Host controller function 5---Write command to host data Bus with aruments
//-----------------------------------------------------------
void IT8951_HW_SendCmdArg(uint16_t usCmdCode,uint16_t* pArg, uint16_t usNumArg)
{
     uint16_t i;
     //Send Cmd code
     IT8951_HW_WriteCmdCode(usCmdCode);
     //Send Data
     for(i=0;i<usNumArg;i++)
     {
         IT8951_HW_WriteData(pArg[i]);
     }
}

void IT8951_HW_Close()
{
	bcm2835_spi_end();
	bcm2835_close();
}
