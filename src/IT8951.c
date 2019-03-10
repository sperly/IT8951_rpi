#include "IT8951.h"
#include "IT8951_HW.h"

//Global variable
IT8951DevInfo gstI80DevInfo;
uint8_t* gpFrameBuf; //Host Source Frame buffer
uint32_t gulImgBufAddr; //IT8951 Image buffer address

uint8_t IT8951_Init()
{
	IT8951_HW_Init();

	//Get Device Info
	IT8951_GetSystemInfo(&gstI80DevInfo);
	
	
	
 	gulImgBufAddr = gstI80DevInfo.usImgBufAddrL | (gstI80DevInfo.usImgBufAddrH << 16);
 	
 	//Set to Enable I80 Packed mode
 	IT8951_WriteReg(I80CPCR, 0x0001);

	if (VCOM != IT8951GetVCOM())
	{
		IT8951_SetVCOM(VCOM);
		debug_print("VCOM = -%.02fV\n",(float)IT8951_GetVCOM()/1000);
	}
	
	return 0;
}

void IT8951_Cancel()
{
	free(gpFrameBuf);

	IT8951_HW_Close();
}

void IT8951_GetSystemInfo(void* pBuf)
{
	uint16_t* pusWord = (uint16_t*)pBuf;
	IT8951DevInfo* pstDevInfo;

	//Send I80 CMD
	IT8951_HW_WriteCmdCode(USDEF_I80_CMD_GET_DEV_INFO);
 
	//Burst Read Request for SPI interface only
	IT8951_HW_ReadNData(pusWord, sizeof(IT8951DevInfo)/2);//Polling HRDY for each words(2-bytes) if possible
	
	//Show Device information of IT8951
	pstDevInfo = (IT8951DevInfo*)pBuf;
	debug_print("Panel(W,H) = (%d,%d)\r\n", 	pstDevInfo->usPanelW, pstDevInfo->usPanelH );
	debug_print("Image Buffer Address = %X\r\n", pstDevInfo->usImgBufAddrL | (pstDevInfo->usImgBufAddrH << 16));
	//Show Firmware and LUT Version
	debug_print("FW Version = %s\r\n", (uint8_t*)pstDevInfo->usFWVersion);
	debug_print("LUT Version = %s\r\n", (uint8_t*)pstDevInfo->usLUTVersion);
}


//-----------------------------------------------------------
//Host Cmd 1---SYS_RUN
//-----------------------------------------------------------
void IT8951_SystemRun()
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_SYS_RUN);
}

//-----------------------------------------------------------
//Host Cmd 2---STANDBY
//-----------------------------------------------------------
void IT8951_StandBy()
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_STANDBY);
}

//-----------------------------------------------------------
//Host Cmd 3---SLEEP
//-----------------------------------------------------------
void IT8951_Sleep()
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_SLEEP);
}

//-----------------------------------------------------------
//Host Cmd 4---REG_RD
//-----------------------------------------------------------
uint16_t IT8951_ReadReg(uint16_t usRegAddr)
{
	uint16_t usData;
	
	//Send Cmd and Register Address
	IT8951_HW_WriteCmdCode(IT8951_TCON_REG_RD);
	IT8951_HW_WriteData(usRegAddr);
	//Read data from Host Data bus
	usData = IT8951_HW_ReadData();
	return usData;
}
//-----------------------------------------------------------
//Host Cmd 5---REG_WR
//-----------------------------------------------------------
void IT8951_WriteReg(uint16_t usRegAddr,uint16_t usValue)
{
	//Send Cmd , Register Address and Write Value
	IT8951_HW_WriteCmdCode(IT8951_TCON_REG_WR);
	IT8951_HW_WriteData(usRegAddr);
	IT8951_HW_WriteData(usValue);
}

//-----------------------------------------------------------
//Host Cmd 6---MEM_BST_RD_T
//-----------------------------------------------------------
void IT8951_MemBurstReadTrigger(uint32_t ulMemAddr , uint32_t ulReadSize)
{
    uint16_t usArg[4];
    //Setting Arguments for Memory Burst Read
    usArg[0] = (uint16_t)(ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (uint16_t)( (ulMemAddr >> 16) & 0x0000FFFF ); //addr[25:16]
    usArg[2] = (uint16_t)(ulReadSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (uint16_t)( (ulReadSize >> 16) & 0x0000FFFF ); //Cnt[25:16]
    //Send Cmd and Arg
    IT8951_HW_SendCmdArg(IT8951_TCON_MEM_BST_RD_T , usArg , 4);
}
//-----------------------------------------------------------
//Host Cmd 7---MEM_BST_RD_S
//-----------------------------------------------------------
void IT8951_MemBurstReadStart()
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_MEM_BST_RD_S);
}
//-----------------------------------------------------------
//Host Cmd 8---MEM_BST_WR
//-----------------------------------------------------------
void IT8951_MemBurstWrite(uint32_t ulMemAddr , uint32_t ulWriteSize)
{
    uint16_t usArg[4];
    //Setting Arguments for Memory Burst Write
    usArg[0] = (uint16_t)(ulMemAddr & 0x0000FFFF); //addr[15:0]
    usArg[1] = (uint16_t)( (ulMemAddr >> 16) & 0x0000FFFF ); //addr[25:16]
    usArg[2] = (uint16_t)(ulWriteSize & 0x0000FFFF); //Cnt[15:0]
    usArg[3] = (uint16_t)( (ulWriteSize >> 16) & 0x0000FFFF ); //Cnt[25:16]
    //Send Cmd and Arg
    IT8951_HW_SendCmdArg(IT8951_TCON_MEM_BST_WR , usArg , 4);
}

//-----------------------------------------------------------
//Host Cmd 9---MEM_BST_END
//-----------------------------------------------------------
void IT8951_MemBurstEnd(void)
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_MEM_BST_END);
}

//-----------------------------------------------------------
//Host Cmd 10---LD_IMG
//-----------------------------------------------------------
void IT8951_LoadImgStart(IT8951LdImgInfo* pstLdImgInfo)
{
    uint16_t usArg;
    //Setting Argument for Load image start
    usArg = (pstLdImgInfo->usEndianType << 8 )
    |(pstLdImgInfo->usPixelFormat << 4)
    |(pstLdImgInfo->usRotate);
    //Send Cmd
    IT8951_HW_WriteCmdCode(IT8951_TCON_LD_IMG);
    //Send Arg
    IT8951_HW_WriteData(usArg);
}

//-----------------------------------------------------------
//Host Cmd 11---LD_IMG_AREA
//-----------------------------------------------------------
void IT8951_LoadImgAreaStart(IT8951LdImgInfo* pstLdImgInfo ,IT8951AreaImgInfo* pstAreaImgInfo)
{
    uint16_t usArg[5];
    //Setting Argument for Load image start
    usArg[0] = (pstLdImgInfo->usEndianType << 8 )
    |(pstLdImgInfo->usPixelFormat << 4)
    |(pstLdImgInfo->usRotate);
    usArg[1] = pstAreaImgInfo->usX;
    usArg[2] = pstAreaImgInfo->usY;
    usArg[3] = pstAreaImgInfo->usWidth;
    usArg[4] = pstAreaImgInfo->usHeight;
    //Send Cmd and Args
    IT8951_HW_SendCmdArg(IT8951_TCON_LD_IMG_AREA , usArg , 5);
}

//-----------------------------------------------------------
//Host Cmd 12---LD_IMG_END
//-----------------------------------------------------------
void IT8951_LoadImgEnd(void)
{
    IT8951_HW_WriteCmdCode(IT8951_TCON_LD_IMG_END);
}

//-----------------------------------------------------------
//Initial function 2---Set Image buffer base address
//-----------------------------------------------------------
void IT8951_SetImgBufBaseAddr(uint32_t ulImgBufAddr)
{
	uint16_t usWordH = (uint16_t)((ulImgBufAddr >> 16) & 0x0000FFFF);
	uint16_t usWordL = (uint16_t)( ulImgBufAddr & 0x0000FFFF);
	//Write LISAR Reg
	IT8951_WriteReg(LISAR + 2 ,usWordH);
	IT8951_WriteReg(LISAR ,usWordL);
}

//-----------------------------------------------------------
// 3.6. Display Functions
//-----------------------------------------------------------

uint16_t IT8951_GetVCOM(void)
{
	uint16_t vcom;
	
	IT8951_HW_WriteCmdCode(USDEF_I80_CMD_VCOM);
	IT8951_HW_WriteData(0);
	//Read data from Host Data bus
	vcom = IT8951_HW_ReadData();
	return vcom;
}

void IT8951_SetVCOM(uint16_t vcom)
{
	IT8951_HW_WriteCmdCode(USDEF_I80_CMD_VCOM);
	IT8951_HW_WriteData(1);
	//Read data from Host Data bus
	IT8951_HW_WriteData(vcom);
}

//-----------------------------------------------------------
//Example of Memory Burst Write
//-----------------------------------------------------------
// ****************************************************************************************
// Function name: IT8951MemBurstWriteProc( )
//
// Description:
//   IT8951 Burst Write procedure
//      
// Arguments:
//      uint32_t ulMemAddr: IT8951 Memory Target Address
//      uint32_t ulWriteSize: Write Size (Unit: Word)
//      uint8_t* pDestBuf - Buffer of Sent data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************
void IT8951_MemBurstWriteProc(uint32_t ulMemAddr , uint32_t ulWriteSize, uint16_t* pSrcBuf )
{
    
    uint32_t i;
 
    //Send Burst Write Start Cmd and Args
    IT8951_MemBurstWrite(ulMemAddr , ulWriteSize);
 
    //Burst Write Data
    for(i=0;i<ulWriteSize;i++)
    {
        IT8951_HW_WriteData(pSrcBuf[i]);
    }
 
    //Send Burst End Cmd
    IT8951_MemBurstEnd();
}

// ****************************************************************************************
// Function name: IT8951MemBurstReadProc( )
//
// Description:
//   IT8951 Burst Read procedure
//      
// Arguments:
//      uint32_t ulMemAddr: IT8951 Read Memory Address
//      uint32_t ulReadSize: Read Size (Unit: Word)
//      uint8_t* pDestBuf - Buffer for storing Read data
// Return Values:
//   NULL.
// Note:
//
// ****************************************************************************************
void IT8951_MemBurstReadProc(uint32_t ulMemAddr , uint32_t ulReadSize, uint16_t* pDestBuf )
{
    //Send Burst Read Start Cmd and Args
    IT8951_MemBurstReadTrigger(ulMemAddr , ulReadSize);
          
    //Burst Read Fire
    IT8951_MemBurstReadStart();
    
    //Burst Read Request for SPI interface only
    IT8951_HW_ReadNData(pDestBuf, ulReadSize);

    //Send Burst End Cmd
    IT8951_MemBurstEnd(); //the same with IT8951MemBurstEnd()
}

//-----------------------------------------------------------
//Display function 1---Wait for LUT Engine Finish
//                     Polling Display Engine Ready by LUTNo
//-----------------------------------------------------------
void IT8951_WaitForDisplayReady()
{
	//Check IT8951 Register LUTAFSR => NonZero Busy, 0 - Free
	while(IT8951_ReadReg(LUTAFSR));
}

//-----------------------------------------------------------
//Display function 2---Load Image Area process
//-----------------------------------------------------------
void IT8951_HostAreaPackedPixelWrite(IT8951LdImgInfo* pstLdImgInfo,IT8951AreaImgInfo* pstAreaImgInfo)
{
	uint32_t i,j;
	//Source buffer address of Host
	uint16_t* pusFrameBuf = (uint16_t*)pstLdImgInfo->ulStartFBAddr;

	//Set Image buffer(IT8951) Base address
	IT8951_SetImgBufBaseAddr(pstLdImgInfo->ulImgBufBaseAddr);
	//Send Load Image start Cmd
	IT8951_LoadImgAreaStart(pstLdImgInfo , pstAreaImgInfo);
	//Host Write Data
	for(j=0;j< pstAreaImgInfo->usHeight;j++)
	{
		 for(i=0;i< pstAreaImgInfo->usWidth/2;i++)
			{
					//Write a Word(2-Bytes) for each time
					IT8951_HW_WriteData(*pusFrameBuf);
					pusFrameBuf++;
			}
	}
	//Send Load Img End Command
	IT8951_LoadImgEnd();
}

//-----------------------------------------------------------
//Display functions 3---Application for Display panel Area
//-----------------------------------------------------------
void IT8951_DisplayArea(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode)
{
	//Send I80 Display Command (User defined command of IT8951)
	IT8951_HW_WriteCmdCode(USDEF_I80_CMD_DPY_AREA); //0x0034
	//Write arguments
	IT8951_HW_WriteData(usX);
	IT8951_HW_WriteData(usY);
	IT8951_HW_WriteData(usW);
	IT8951_HW_WriteData(usH);
	IT8951_HW_WriteData(usDpyMode);
}

//Display Area with bitmap on EPD
//-----------------------------------------------------------
// Display Function 4---for Display Area for 1-bpp mode format
//   the bitmap(1bpp) mode will be enable when Display
//   and restore to Default setting (disable) after displaying finished
//-----------------------------------------------------------
void IT8951_DisplayArea1bpp(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode, uint8_t ucBGGrayVal, uint8_t ucFGGrayVal)
{
    //Set Display mode to 1 bpp mode - Set 0x18001138 Bit[18](0x1800113A Bit[2])to 1
    IT8951_WriteReg(UP1SR+2, IT8951_ReadReg(UP1SR+2) | (1<<2));
    
    //Set BitMap color table 0 and 1 , => Set Register[0x18001250]:
    //Bit[7:0]: ForeGround Color(G0~G15)  for 1
    //Bit[15:8]:Background Color(G0~G15)  for 0
    IT8951_WriteReg(BGVR, (ucBGGrayVal<<8) | ucFGGrayVal);
    
    //Display
    IT8951_DisplayArea( usX, usY, usW, usH, usDpyMode);
    IT8951_WaitForDisplayReady();
    
    //Restore to normal mode
    IT8951_WriteReg(UP1SR+2, IT8951_ReadReg(UP1SR+2) & ~(1<<2));
}

//-------------------------------------------------------------------------------------------------------------
// 	Command - 0x0037 for Display Base addr by User 
//  uint32_t ulDpyBufAddr - Host programmer need to indicate the Image buffer address of IT8951
//                                         In current case, there is only one image buffer in IT8951 so far.
//                                         So Please set the Image buffer address you got  in initial stage.
//                                         (gulImgBufAddr by Get device information 0x0302 command)
//
//-------------------------------------------------------------------------------------------------------------
void IT8951_DisplayAreaBuf(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode, uint32_t ulDpyBufAddr)
{
    //Send I80 Display Command (User defined command of IT8951)
    IT8951_HW_WriteCmdCode(USDEF_I80_CMD_DPY_BUF_AREA); //0x0037
    
    //Write arguments
    IT8951_HW_WriteData(usX);
    IT8951_HW_WriteData(usY);
    IT8951_HW_WriteData(usW);
    IT8951_HW_WriteData(usH);
    IT8951_HW_WriteData(usDpyMode);
    IT8951_HW_WriteData((uint16_t)ulDpyBufAddr);       //Display Buffer Base address[15:0]
    IT8951_HW_WriteData((uint16_t)(ulDpyBufAddr>>16)); //Display Buffer Base address[26:16]
}
