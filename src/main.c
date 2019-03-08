#include "IT8951.h"

int main (int argc, char *argv[])
{
	if(IT8951_Init())
	{
		printf("IT8951_Init error \n");
		return 1;
	}
	
	IT8951LdImgInfo stLdImgInfo;
	IT8951AreaImgInfo stAreaImgInfo;

	
	printf("Clearing display\n");fflush(stdout);
	EPD_Clear(0xff);
	printf("Cleared display\n");fflush(stdout);
		
	Show_bmp(0,0,"Current.bmp");
	printf("Drawed image\n");fflush(stdout);


	IT8951WaitForDisplayReady();
	printf("Display Ready\n");fflush(stdout);

	IT8951DevInfo* info = IT8951GetDevInfo();
	printf("Info retrieved\n");fflush(stdout);

	//uint8_t* gpFrameBuf;
	//memset(gpFrameBuf, 0xff, info->usPanelW * info->usPanelH);

	//Setting Load image information
	stLdImgInfo.ulStartFBAddr    = 0;
	stLdImgInfo.usEndianType     = IT8951_LDIMG_L_ENDIAN;
	stLdImgInfo.usPixelFormat    = IT8951_8BPP; 
	stLdImgInfo.usRotate         = IT8951_ROTATE_0;
	stLdImgInfo.ulImgBufBaseAddr = info->usImgBufAddrL | (info->usImgBufAddrH << 16);
	//Set Load Area
	stAreaImgInfo.usX      = 0;
	stAreaImgInfo.usY      = 0;
	stAreaImgInfo.usWidth  = info->usPanelW;//gstI80DevInfo.usPanelW;
	stAreaImgInfo.usHeight = info->usPanelH;//gstI80DevInfo.usPanelH;

	printf("Writing pixels (%p) (%p)\n",stLdImgInfo.ulStartFBAddr  ,stLdImgInfo.ulImgBufBaseAddr);fflush(stdout);
	
	//Load Image from Host to IT8951 Image Buffer
	IT8951HostAreaPackedPixelWrite(&stLdImgInfo, &stAreaImgInfo);//Display function 2
	//Display Area ?V (x,y,w,h) with mode 2 for fast gray clear mode - depends on current waveform 
	


	EPD_DrawRect(300,0,2,599,1);	

	IT8951DisplayArea(0,0, info->usPanelW, info->usPanelH, 2);
	
	IT8951_Cancel();

	return 0;
}


