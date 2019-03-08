#ifndef _IT8951_HW_H_
#define _IT8951_HW_H_

void IT8951_HW_Init(void);
void IT8951_HW_WaitForReady(void);
void IT8951_HW_WriteCmdCode(uint16_t usCmdCode);
void IT8951_HW_WriteData(uint16_t usData);
void IT8951_HW_WriteNData(uint16_t* pwBuf, uint32_t ulSizeWordCnt);
uint16_t IT8951_HW_ReadData(void);
void IT8951_HW_ReadNData(uint16_t* pwBuf, uint32_t ulSizeWordCnt);
void IT8951_HW_SendCmdArg(uint16_t usCmdCode,uint16_t* pArg, uint16_t usNumArg);
void IT8951_HW_Close(void);

#endif _IT8951_HW_H_