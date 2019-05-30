/****************************************************************************
*****************************************************************************
****       _   _ _ _               ______             _                  ****
****      | \ | (_) |             |  ____|           (_)                 ****
****      |  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___       ****
****      | . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \      ****
****      | |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/      ****
****      |_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___|      ****
****                                             __/ |                   ****
****                                            |___/      V 0.6.1       ****
****                                                                     ****
****                     Copyright (C) 2008 - 2011 Antonio Niño Díaz     ****
****                                   All rights reserved.              ****
****                                                                     ****
*****************************************************************************
****************************************************************************/

/****************************************************************************
*                                                                           *
* Nitro Engine V 0.6.1 is licensed under the terms of <readme_license.txt>. *
* If you have any question, email me at <antonio_nd@hotmail.com>.           *
*                                                                           *
****************************************************************************/

#include "NEMain.h"

/*! \file   NEFAT.c */

char* NE_FATLoadData(char* filename) 
{
	char* buffer = NULL;
	FILE* datafile;
	u32 size = 0;

	datafile = fopen (filename, "rb+");
	if(datafile)
	{
		fseek (datafile , 0 , SEEK_END);
		size = ftell (datafile);
		rewind (datafile);
		buffer = malloc (size);
		if(buffer)
		{
			fread (buffer,1,size,datafile);
			fclose (datafile);
		}
		else
		{
			NE_DebugPrint("NE_FATLoadData: Not enought memory to load %s!", filename);
		}
	}
	else
	{
		NE_DebugPrint("NE_FATLoadData: %s could't be opened!", filename);
	}
	
	return buffer;
}

u32 NE_FATFileSize(char* filename) 
{
	FILE* datafile;
	u32 size = 0;

	datafile = fopen (filename, "rb+");
	if(datafile != NULL)
	{
		fseek (datafile , 0 , SEEK_END);
		size = ftell (datafile);
		fclose (datafile);
		return size;
	}
	
	NE_DebugPrint("NE_FATFileSize: %s could't be opened!", filename);

	return -1;
}

//----------------------------------------------------------------
//                      Screenshots
//----------------------------------------------------------------

inline void NE_write16(u16* address, u16 value) 
{
	u8* first=(u8*)address;
	u8* second=first+1;

	*first=value&0xff;
	*second=value>>8;
}

inline void NE_write32(u32* address, u32 value)
{
	u8* first=(u8*)address;
	u8* second=first+1;
	u8* third=first+2;
	u8* fourth=first+3;

	*first=value&0xff;
	*second=(value>>8)&0xff;
	*third=(value>>16)&0xff;
	*fourth=(value>>24)&0xff;
}

extern bool NE_Dual;

int NE_ScreenshotBMP(char * filename) 
{   
	FILE* file=fopen(filename, "wb");

	if(file == NULL) 
	{
		NE_DebugPrint("NE_ScreenshotBMP: %s could't be opened!", filename);
		return 0;
	}
	
	NE_SpecialEffectPause(true);
	
	if(!NE_Dual)
	{
		vramSetBankD(VRAM_D_LCD);
		
		REG_DISPCAPCNT=DCAP_BANK(3)|DCAP_ENABLE|DCAP_SIZE(3);
		while(REG_DISPCAPCNT & DCAP_ENABLE);
	}

	int ysize = 0;
	
	if(NE_Dual) ysize = 384;
	else ysize = 192;	
	
	u8* temp=(u8*)malloc(256*ysize*3+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER));

	NE_BMP_HEADER* header=(NE_BMP_HEADER*)temp;
	NE_INFO_BMP_HEADER* infoheader=(NE_INFO_BMP_HEADER*)(temp+sizeof(NE_BMP_HEADER));

	NE_write16(&header->type, 0x4D42);
	NE_write32(&header->size, 256*ysize*3+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER));
	NE_write32(&header->offset, sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER));
	NE_write16(&header->reserved1, 0);
	NE_write16(&header->reserved2, 0);

	NE_write16(&infoheader->bits, 24);
	NE_write32(&infoheader->size, sizeof(NE_INFO_BMP_HEADER));
	NE_write32(&infoheader->compression, 0);
	NE_write32(&infoheader->width, 256);
	NE_write32(&infoheader->height, ysize);
	NE_write16(&infoheader->planes, 1);
	NE_write32(&infoheader->imagesize, 256*ysize*3);
	NE_write32(&infoheader->xresolution, 0);
	NE_write32(&infoheader->yresolution, 0);
	NE_write32(&infoheader->importantcolors, 0);
	NE_write32(&infoheader->ncolors, 0);

	uint32 vramTemp = 0;
	if(NE_Dual) { vramTemp = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD); }

	int y, x;
	for(y=0;y<ysize;y++)
	{
		for(x=0;x<256;x++)
		{
			u16 color = 0;
			
			if(NE_Dual) 
			{
				if(y>191) color=VRAM_C[256*192-(y-192+1)*256+x];
				else color=VRAM_D[256*192-(y+1)*256+x];
			}
			else color=VRAM_D[256*192-(y+1)*256+x];
			
			u8 b=(color&31)<<3;
			u8 g=((color>>5)&31)<<3;
			u8 r=((color>>10)&31)<<3;
			
			temp[((y*256)+x)*3+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER)]=r;
			temp[((y*256)+x)*3+1+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER)]=g;
			temp[((y*256)+x)*3+2+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER)]=b;
		}
	}
	
	if(NE_Dual) vramRestorePrimaryBanks(vramTemp);
	
	DC_FlushAll();
	fwrite(temp, 1, 256*ysize*3+sizeof(NE_INFO_BMP_HEADER)+sizeof(NE_BMP_HEADER), file);
	fclose(file);
	free(temp);
	
	NE_SpecialEffectPause(false);
	
	return 1;
}
