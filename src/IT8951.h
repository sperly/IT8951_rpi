#ifndef _IT8951_H_
#define _IT8951_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define debug_print(fmt, ...) \
            do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

//Pin definitions
#define CS 				            8
#define HRDY 	                    24
#define RESET 	                    17
#define VCOM			            1500 //e.g. -1.53 = 1530 = 0x5FA

//Built in I80 Command Code
#define IT8951_TCON_SYS_RUN         0x0001
#define IT8951_TCON_STANDBY         0x0002
#define IT8951_TCON_SLEEP           0x0003
#define IT8951_TCON_REG_RD          0x0010
#define IT8951_TCON_REG_WR          0x0011
#define IT8951_TCON_MEM_BST_RD_T    0x0012
#define IT8951_TCON_MEM_BST_RD_S    0x0013
#define IT8951_TCON_MEM_BST_WR      0x0014
#define IT8951_TCON_MEM_BST_END     0x0015
#define IT8951_TCON_LD_IMG          0x0020
#define IT8951_TCON_LD_IMG_AREA     0x0021
#define IT8951_TCON_LD_IMG_END      0x0022

//I80 User defined command code
#define USDEF_I80_CMD_DPY_AREA      0x0034
#define USDEF_I80_CMD_GET_DEV_INFO  0x0302
#define USDEF_I80_CMD_DPY_BUF_AREA  0x0037
#define USDEF_I80_CMD_VCOM		    0x0039

//Panel
#define IT8951_PANEL_WIDTH          800
#define IT8951_PANEL_HEIGHT         600

//Rotate mode
#define IT8951_ROTATE_0             0
#define IT8951_ROTATE_90            1
#define IT8951_ROTATE_180           2
#define IT8951_ROTATE_270           3

//Pixel mode , BPP - Bit per Pixel
#define IT8951_2BPP                 0
#define IT8951_3BPP                 1
#define IT8951_4BPP                 2
#define IT8951_8BPP                 3

//Waveform Mode
#define IT8951_MODE_0               0
#define IT8951_MODE_1               1
#define IT8951_MODE_2               2
#define IT8951_MODE_3               3
#define IT8951_MODE_4               4

//Endian Type
#define IT8951_LDIMG_L_ENDIAN       0
#define IT8951_LDIMG_B_ENDIAN       1

//Auto LUT
#define IT8951_DIS_AUTO_LUT         0
#define IT8951_EN_AUTO_LUT          1

//LUT Engine Status
#define IT8951_ALL_LUTE_BUSY        0xFFFF

//System Registers
#define SYS_REG_BASE                0x0000

//Address of System Registers
#define I80CPCR                     (SYS_REG_BASE + 0x04)

//Register Base Address
#define DISPLAY_REG_BASE            0x1000

//Base Address of Basic LUT Registers
#define LUT0EWHR                    (DISPLAY_REG_BASE + 0x00)   //LUT0 Engine Width Height Reg
#define LUT0XYR                     (DISPLAY_REG_BASE + 0x40)   //LUT0 XY Reg
#define LUT0BADDR                   (DISPLAY_REG_BASE + 0x80)   //LUT0 Base Address Reg
#define LUT0MFN                     (DISPLAY_REG_BASE + 0xC0)   //LUT0 Mode and Frame number Reg
#define LUT01AF                     (DISPLAY_REG_BASE + 0x114)  //LUT0 and LUT1 Active Flag Reg

//Update Parameter Setting Register
#define UP0SR                       (DISPLAY_REG_BASE + 0x134)  //Update Parameter0 Setting Reg
#define UP1SR                       (DISPLAY_REG_BASE + 0x138)  //Update Parameter1 Setting Reg
#define LUT0ABFRV                   (DISPLAY_REG_BASE + 0x13C)  //LUT0 Alpha blend and Fill rectangle Value
#define UPBBADDR                    (DISPLAY_REG_BASE + 0x17C)  //Update Buffer Base Address
#define LUT0IMXY                    (DISPLAY_REG_BASE + 0x180)  //LUT0 Image buffer X/Y offset Reg
#define LUTAFSR                     (DISPLAY_REG_BASE + 0x224)  //LUT Status Reg (status of All LUT Engines)
#define BGVR                        (DISPLAY_REG_BASE + 0x250)  //Bitmap (1bpp) image color table

//Memory Converter Registers
#define MCSR_BASE_ADDR              0x0200
#define MCSR                        (MCSR_BASE_ADDR  + 0x0000)
#define LISAR                       (MCSR_BASE_ADDR + 0x0008)

typedef struct IT8951LdImgInfo
{
    uint16_t usEndianType;
    uint16_t usPixelFormat;
    uint16_t usRotate;
    uint32_t ulStartFBAddr;
    uint32_t ulImgBufBaseAddr;
    
}IT8951LdImgInfo;

typedef struct IT8951AreaImgInfo
{
    uint16_t usX;
    uint16_t usY;
    uint16_t usWidth;
    uint16_t usHeight;
}IT8951AreaImgInfo;

typedef struct
{
    uint16_t usPanelW;
    uint16_t usPanelH;
    uint16_t usImgBufAddrL;
    uint16_t usImgBufAddrH;
    uint16_t usFWVersion[8];    //16 Bytes String
    uint16_t usLUTVersion[8];   //16 Bytes String
}IT8951DevInfo;

uint8_t IT8951_Init(void);
void IT8951_Cancel(void);

void IT8951_GetSystemInfo(void* pBuf);

//Host Commands
void IT8951_SystemRun(void);
void IT8951_StandBy(void);
void IT8951_Sleep(void);
uint16_t IT8951_ReadReg(uint16_t usRegAddr);
void IT8951_WriteReg(uint16_t usRegAddr,uint16_t usValue);
void IT8951_MemBurstReadTrigger(uint32_t ulMemAddr , uint32_t ulReadSize);
void IT8951_MemBurstReadStart(void);
void IT8951_MemBurstWrite(uint32_t ulMemAddr , uint32_t ulWriteSize);
void IT8951_MemBurstEnd(void);
void IT8951_LoadImgStart(IT8951LdImgInfo* pstLdImgInfo);
void IT8951_LoadImgAreaStart(IT8951LdImgInfo* pstLdImgInfo ,IT8951AreaImgInfo* pstAreaImgInfo);
void IT8951_LoadImgEnd(void)

//Commands
void IT8951_MemBurstWriteProc(uint32_t ulMemAddr , uint32_t ulWriteSize, uint16_t* pSrcBuf );
void IT8951_MemBurstReadProc(uint32_t ulMemAddr , uint32_t ulReadSize, uint16_t* pDestBuf );

void IT8951_SetImgBufBaseAddr(uint32_t ulImgBufAddr);
void IT8951_WaitForDisplayReady(void);

void IT8951_HostAreaPackedPixelWrite(IT8951LdImgInfo* pstLdImgInfo,IT8951AreaImgInfo* pstAreaImgInfo);

void IT8951_DisplayArea(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode);
void IT8951_DisplayArea1bpp(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode, uint8_t ucBGGrayVal, uint8_t ucFGGrayVal);
void IT8951_DisplayAreaBuf(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, uint16_t usDpyMode, uint32_t ulDpyBufAddr);

void IT8951_DisplayClear(void);

uint16_t IT8951_GetVCOM(void);
void IT8951_SetVCOM(uint16_t vcom);

#endif //_IT8951_H_
