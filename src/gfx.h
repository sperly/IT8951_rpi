#ifndef _GFX_H_
#define _GFX_H_

#include <stdint.h>

typedef struct 
{
  uint16_t X;
  uint16_t Y;
}Point, *pPoint; 

typedef struct
{
	uint16_t cfType;
	uint32_t cfSize;
	uint32_t cfReserved;
	uint32_t cfoffBits;
}__attribute__((packed)) BITMAPFILEHEADER;
 
typedef struct
{
	uint32_t ciSize;
	uint32_t ciWidth;
	uint32_t ciHeight;
	uint16_t ciPlanes;
	uint16_t ciBitCount;
	uint32_t ciCompress;
	uint32_t ciSizeImage;
	uint32_t ciXPelsPerMeter;
	uint32_t ciYPelsPerMeter;
	uint32_t ciClrUsed;
	uint32_t ciClrImportant;ª
}__attribute__((packed)) BITMAPINFOHEADER;

typedef struct
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t reserved;
}__attribute__((packed)) PIXEL;

#define ABS(X)    ((X) > 0 ? (X) : -(X))     

void GFX_Clear(uint8_t Color);
void GFX_DrawPixel(uint16_t x0,uint16_t y0,uint8_t color);
void GFX_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color);
void GFX_DrawRect(uint16_t Xpos,uint16_t Ypos,uint16_t Width,uint16_t Height,uint8_t color);
void GFX_DrawCircle(uint16_t Xpos,uint16_t Ypos,uint16_t Radius,uint8_t color);
void GFX_DrawPolygon(pPoint Points,uint16_t PointCount,uint8_t color);
void GFX_DrawEllipse(uint16_t Xpos,uint16_t Ypos,uint16_t XRadius,uint16_t YRadius,uint8_t color);
void GFX_FillRect(uint16_t Xpos,uint16_t Ypos,uint16_t Width,uint16_t Height,uint8_t color);
void GFX_FillCircle(uint16_t Xpos,uint16_t Ypos,uint16_t Radius,uint8_t color);
void GFX_PutChar(uint16_t Xpos,uint16_t Ypos,uint8_t ASCI,uint8_t charColor,uint8_t bkColor);
void GFX_Text(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint8_t Color,uint8_t bkColor);
void GFX_DrawBitmap(uint16_t Xpos, uint16_t Ypos,uint16_t *bmp);
void GFX_DrawMatrix(uint16_t Xpos, uint16_t Ypos,uint16_t Width, uint16_t High,const uint16_t* Matrix);

#endif //_GFX_H_
