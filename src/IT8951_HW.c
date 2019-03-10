#include "IT8951_HW.h"

#include <bcm2835.h>

void IT8951_HW_Init()
{
	if (!bcm2835_init()) 
	{
		debug_print("bcm2835_init error \n");
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

	debug_print("****** IT8951 HW ******\n");
	
	bcm2835_gpio_write(RESET, LOW);
	bcm2835_delay(100);
	bcm2835_gpio_write(RESET, HIGH);
}

void IT8951_HW_WaitForReady()
{
	uint8_t ulData = bcm2835_gpio_lev(HRDY);
	while(ulData == 0)
	{
		ulData = bcm2835_gpio_lev(HRDY);
	}
}

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

uint16_t IT8951_HW_ReadData()
{
	uint16_t wRData; 
	
	uint16_t wPreamble = 0x1000;

	IT8951_HW_WaitForReady();

	bcm2835_gpio_write(CS,LOW);
		
	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);

	IT8951_HW_WaitForReady();
	
	wRData=bcm2835_spi_transfer(0x00);
	wRData=bcm2835_spi_transfer(0x00);
	
	IT8951_HW_WaitForReady();
	
	wRData = bcm2835_spi_transfer(0x00)<<8;
	wRData |= bcm2835_spi_transfer(0x00);
		
	bcm2835_gpio_write(CS,HIGH); 
		
	return wRData;
}

void IT8951_HW_ReadNData(uint16_t* pwBuf, uint32_t ulSizeWordCnt)
{
	uint32_t i;
	
	uint16_t wPreamble = 0x1000;

	IT8951_HW_WaitForReady();
	
	bcm2835_gpio_write(CS,LOW);

	bcm2835_spi_transfer(wPreamble>>8);
	bcm2835_spi_transfer(wPreamble);
	
	IT8951_HW_WaitForReady();
	
	pwBuf[0]=bcm2835_spi_transfer(0x00);
	pwBuf[0]=bcm2835_spi_transfer(0x00);
	
	IT8951_HW_WaitForReady();
	
	for(i=0;i<ulSizeWordCnt;i++)
	{
		pwBuf[i] = bcm2835_spi_transfer(0x00)<<8;
		pwBuf[i] |= bcm2835_spi_transfer(0x00);
	}
	
	bcm2835_gpio_write(CS,HIGH); 
}

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
