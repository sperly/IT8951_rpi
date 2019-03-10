#include <string.h>
//#include <fcntl.h>
//#include <stdio.h>

//#include <stdlib.h>
//#include <unistd.h>
//#include <linux/fb.h>
//#include <sys/mman.h>
//#include <sys/ioctl.h>
#include "IT8951.h" 
#include "gfx.h"
#include "AsciiLib.h"

uint16_t frameBufSize_x;
uint16_t frameBufSize_y;

uint8_t* gpFrameBuf;

void GFX_Init(uint16_t size_x, uint16_t size_y)
{
	frameBufSize_x = size_x;
	frameBufSize_y = size_y;
	gpFrameBuf = malloc(frameBufSize_x * frameBufSize_y);
	if (!gpFrameBuf)
	{
		perror("malloc error!\n");
		return 1;
	}
}

void GFX_Clear(uint8_t Color)
{
	memset(gpFrameBuf, Color, frameBufSize_x * frameBufSize_y);
}

void GFX_wPixel(uint16_t x0, uint16_t y0, uint8_t color)
{
	if(x0 < 0 || x0 >= frameBufSize_x || y0 < 0 || y0 >= frameBufSize_y)
		return ;	
	
	gpFrameBuf[y0*frameBufSize_x + x0] = color;
}

void GFX_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint8_t color)
{
	  uint32_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	  yinc1 = 0, yinc2 = 0, den = 0, num = 0, num_add = 0, num_pixels = 0, 
	  curpixel = 0;
	  
	  deltax = ABS(x2 - x1);        /* The difference between the x's */
	  deltay = ABS(y2 - y1);        /* The difference between the y's */
	  x = x1;                       /* Start x off at the first pixel */
	  y = y1;                       /* Start y off at the first pixel */
	  
	  if (x2 >= x1)                 /* The x-values are increasing */
	  {
		    xinc1 = 1;
		    xinc2 = 1;
	  }
	  else                          /* The x-values are decreasing */
	  {
		    xinc1 = -1;
		    xinc2 = -1;
	  }
	  
	  if (y2 >= y1)                 /* The y-values are increasing */
	  {
		    yinc1 = 1;
		    yinc2 = 1;
	  }
	  else                          /* The y-values are decreasing */
	  {
		    yinc1 = -1;
		    yinc2 = -1;
	  }
	  
	  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
	  {
		    xinc1 = 0;              /* Don't change the x when numerator >= denominator */
		    yinc2 = 0;              /* Don't change the y for every iteration */
		    den = deltax;
		    num = deltax / 2;
		    num_add = deltay;
		    num_pixels = deltax;    /* There are more x-values than y-values */
	  }
	  else                          /* There is at least one y-value for every x-value */
	  {
		    xinc2 = 0;              /* Don't change the x for every iteration */
		    yinc1 = 0;              /* Don't change the y when numerator >= denominator */
		    den = deltay;
		    num = deltay / 2;
		    num_add = deltax;
		    num_pixels = deltay;    /* There are more y-values than x-values */
	  }
	  
	  for (curpixel = 0; curpixel <= num_pixels; curpixel++)
	  {
		    GFX_DrawPixel(x, y, color);          /* Draw the current pixel */
		    num += num_add;                      /* Increase the numerator by the top of the fraction */
		    if (num >= den)                      /* Check if numerator >= denominator */
		    {
			      num -= den;                    /* Calculate the new numerator value */
			      x += xinc1;                    /* Change the x as appropriate */
			      y += yinc1;                    /* Change the y as appropriate */
		    }
		    x += xinc2;                          /* Change the x as appropriate */
		    y += yinc2;                          /* Change the y as appropriate */
	  }
}

void GFX_DrawRect(uint16_t Xpos,uint16_t Ypos,uint16_t Width,uint16_t Height,uint8_t color)
{
	GFX_DrawLine(Xpos, Ypos, Xpos+Width, Ypos, color);
	GFX_DrawLine(Xpos, Ypos, Xpos, Ypos+Height, color);
	GFX_DrawLine(Xpos, Ypos+Height, Xpos+Width, Ypos+Height, color);
	GFX_DrawLine(Xpos+Width, Ypos, Xpos+Width, Ypos+Height, color);
}


void GFX_DrawCircle(uint16_t Xpos,uint16_t Ypos,uint16_t Radius,uint8_t color)
{
	  int   decision;    	/* Decision Variable */ 
	  uint32_t  current_x;   	/* Current X Value */
	  uint32_t  current_y;   	/* Current Y Value */
	  
	  decision = 3 - (Radius << 1);
	  current_x = 0;
	  current_y = Radius;
	  
	  while (current_x <= current_y)
	  {
		    GFX_DrawPixel((Xpos + current_x), (Ypos - current_y), color);
		    
		    GFX_DrawPixel((Xpos - current_x), (Ypos - current_y), color);
		    
		    GFX_DrawPixel((Xpos + current_y), (Ypos - current_x), color);
		    
		    GFX_DrawPixel((Xpos - current_y), (Ypos - current_x), color);
		    
		    GFX_DrawPixel((Xpos + current_x), (Ypos + current_y), color);
		    
		    GFX_DrawPixel((Xpos - current_x), (Ypos + current_y), color);
		    
		    GFX_DrawPixel((Xpos + current_y), (Ypos + current_x), color);
		    
		    GFX_DrawPixel((Xpos - current_y), (Ypos + current_x), color);
		    
		    if (decision < 0)
		    { 
				decision += (current_x << 2) + 6;
		    }
		    else
		    {
			      decision += ((current_x - current_y) << 2) + 10;
			      current_y--;
		    }
		    current_x++;
	  } 
}

void GFX_DrawPolygon(pPoint Points,uint16_t PointCount,uint8_t color)
{
  uint16_t x = 0, y = 0;
  
  if(PointCount < 2)
  {
    return;
  }
  
  GFX_DrawLine(Points->X, Points->Y, (Points+PointCount-1)->X, (Points+PointCount-1)->Y,color);
  
  while(--PointCount)
  {
    x = Points->X;
    y = Points->Y;
    Points++;
    GFX_DrawLine(x, y, Points->X, Points->Y,color);
  }
}


void GFX_DrawEllipse(uint16_t Xpos, uint16_t Ypos, uint16_t XRadius, uint16_t YRadius, uint8_t color)
{
  int32_t x = 0, y = -YRadius, err = 2-2*XRadius, e2;
  float k = 0, rad1 = 0, rad2 = 0;
  
  rad1 = XRadius;
  rad2 = YRadius;
  
  k = (float)(rad2/rad1);  
  
  do { 
    GFX_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos+y), color);
    GFX_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos+y), color);
    GFX_DrawPixel((Xpos+(uint16_t)(x/k)), (Ypos-y), color);
    GFX_DrawPixel((Xpos-(uint16_t)(x/k)), (Ypos-y), color);      
    
    e2 = err;
    if (e2 <= x) {
      err += ++x*2+1;
      if (-y == x && e2 <= y) e2 = 0;
    }
    if (e2 > y) err += ++y*2+1;     
  }
  while (y <= 0);
}

void GFX_FillRect(uint16_t Xpos,uint16_t Ypos,uint16_t Width,uint16_t Height,uint8_t color)
{
	uint16_t i;
	for (i=0;i<Height;i++)
	{
		GFX_DrawLine(Xpos, Ypos+i,Xpos+Width,Ypos+i,color);
	}
  
}

void GFX_FillCircle(uint16_t Xpos,uint16_t Ypos,uint16_t Radius,uint8_t color)
{
  int32_t  decision;     /* Decision Variable */ 
  uint32_t  current_x;   /* Current X Value */
  uint32_t  current_y;   /* Current Y Value */
  
  decision = 3 - (Radius << 1);
  
  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y)
  {
    if(current_y > 0) 
    {
	GFX_DrawLine(Xpos - current_y, Ypos + current_x,Xpos +current_y,Ypos + current_x,color);
	GFX_DrawLine(Xpos - current_y, Ypos - current_x,Xpos + current_y, Ypos - current_x,color);
    }
    
    if(current_x > 0) 
    {
	GFX_DrawLine(Xpos - current_x, Ypos - current_y,Xpos+current_x,Ypos - current_y,color);
 	GFX_DrawLine(Xpos - current_x, Ypos + current_y,Xpos+current_x,Ypos + current_y,color);
    }
    if (decision < 0)
    { 
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
  
 	GFX_DrawCircle(Xpos, Ypos, Radius,color);
}

void GFX_PutChar(uint16_t Xpos,uint16_t Ypos,uint8_t ASCI,uint8_t charColor,uint8_t bkColor )
{
	uint16_t i, j;
	uint8_t buffer[16], tmp_char;
	GetASCIICode(buffer,ASCI);
	for(i=0; i<16; i++)
	{
		tmp_char = buffer[i];
		for(j=0; j<8; j++)
		{
			if(((tmp_char >> (7-j)) & 0x01) == 0x01)
			{
				GFX_DrawPixel(Xpos+j, Ypos+i, charColor);
			}
			else
			{
				GFX_DrawPixel(Xpos+j, Ypos+i, bkColor);
			}
		}
	}
}


void GFX_Text(uint16_t Xpos,uint16_t Ypos,uint8_t *str,uint8_t Color,uint8_t bkColor)
{
	uint8_t TempChar;
	do
	{
		TempChar = *str++;  
		GFX_PutChar(Xpos, Ypos, TempChar, Color, bkColor);    
		if(Xpos < frameBufSize_x - 8)
		{
			Xpos += 8;
		} 
		else if (Ypos < frameBufSize_y - 16)
		{
			Xpos = 0;
			Ypos += 16;
		}   
	else
	{
		Xpos = 0;
		Ypos = 0;
	}    
    }
    while (*str != 0);
}

extern uint16_t bmp01[];

void GFX_DrawBitmap(uint16_t Xpos, uint16_t Ypos,uint16_t *bmp)
{
	uint16_t i,j;
	uint16_t R,G,B,temp;
	double Gray;

	for (j=0;j<412;j++)
	{
		for (i=0;i<550;i++)
		{
			temp = bmp[j*550+i];
			R = (temp >> 11)<<3;
			G = ((temp&0x07E0) >> 5)<<2;
			B = (temp&0x01F)<<3;
			Gray = (R*299 + G*587 + B*114 + 500) / 1000;
			GFX_DrawPixel(i, j,(uint8_t)Gray);
		}	
	}
}

void GFX_DrawMatrix(uint16_t Xpos, uint16_t Ypos,uint16_t Width, uint16_t High,const uint16_t* Matrix)
{
	uint16_t i,j,x,y;
	uint16_t R,G,B,temp;
	double Gray;
	
	for (y=0,j=Ypos;y<High;y++,j++)
	{
 		for (x=0,i=Xpos;x<Width;x++,i++)
		{
			temp = Matrix[y*550+x];
			R = (temp >> 11)<<3;
			G = ((temp&0x07E0) >> 5)<<2;
			B = (temp&0x001F) <<3;
			Gray = (R*299 + G*587 + B*114 + 500) / 1000;
			GFX_DrawPixel(i, j, Gray);
		}
	}
}





