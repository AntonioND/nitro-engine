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

/*! \file   NEFormats.c */

//----------------------------------------------------------------------------------
static int lastx = 0, lasty = 0; // INTERNAL USE
static u32 numcolors = 0;
static void * __NE_ConvertBMPtoRGBA(void * pointer, bool transpcolor);
static void * __NE_ConvertBMPtoRGB256(void * pointer, u16 * palettebuffer);
//-------------------------------------------------------------------------------

int NE_FATMaterialTexLoadBMPtoRGBA(NE_Material * tex, char * filename, bool transpcolor)
{
	NE_AssertPointer(tex,"NE_FATMaterialTexLoadBMPtoRGBA: NULL material pointer.");
	NE_AssertPointer(filename,"NE_FATMaterialTexLoadBMPtoRGBA: NULL filename pointer.");

	void * pointer = NE_FATLoadData(filename); 
	int a = NE_MaterialTexLoadBMPtoRGBA(tex, pointer, transpcolor);
	free(pointer);

	return a;
}


int NE_FATMaterialTexLoadBMPtoRGB256(NE_Material * tex, NE_Palette * pal, char * filename, bool transpcolor)
{
	NE_AssertPointer(tex,"NE_FATMaterialTexLoadBMPtoRGB256: NULL material pointer.");
	NE_AssertPointer(pal,"NE_FATMaterialTexLoadBMPtoRGB256: NULL palette pointer.");
	NE_AssertPointer(filename,"NE_FATMaterialTexLoadBMPtoRGB256: NULL filename pointer.");

	char * pointer = NE_FATLoadData(filename); 
	int a = NE_MaterialTexLoadBMPtoRGB256(tex, pal,pointer, transpcolor);
	free(pointer);

	return a;
}


int NE_MaterialTexLoadBMPtoRGBA(NE_Material * tex, void * pointer, bool transpcolor)
{
	NE_AssertPointer(tex,"NE_MaterialTexLoadBMPtoRGBA: NULL material pointer.");
	NE_AssertPointer(pointer,"NE_MaterialTexLoadBMPtoRGBA: NULL data pointer.");

	void * datapointer = __NE_ConvertBMPtoRGBA(pointer,transpcolor);
	if(datapointer == NULL) return 0;
	int a = NE_MaterialTexLoad(tex,GL_RGBA,lastx,lasty,TEXGEN_TEXCOORD,(u8*)datapointer);
	free(datapointer);
	if(a == 0) return 0;
	return 1;
}

int NE_MaterialTexLoadBMPtoRGB256(NE_Material * tex, NE_Palette * pal, void * pointer, bool transpcolor)
{
	NE_AssertPointer(tex,"NE_MaterialTexLoadBMPtoRGB256: NULL material pointer.");
	NE_AssertPointer(pal,"NE_MaterialTexLoadBMPtoRGB256: NULL palette pointer.");
	NE_AssertPointer(pointer,"NE_MaterialTexLoadBMPtoRGB256: NULL data pointer.");

	u16 * palettebuffer = malloc(256 * sizeof(u16));
	NE_AssertPointer(palettebuffer,"NE_MaterialTexLoadBMPtoRGB256: Couldn't allocate temporal palette buffer.");
	if(palettebuffer == NULL) return 0;

	void * datapointer = __NE_ConvertBMPtoRGB256(pointer, palettebuffer);
	NE_AssertPointer(datapointer,"NE_MaterialTexLoadBMPtoRGB256: Couldn't convert BMP file to GL_RGB256 format.");
	if(datapointer == NULL) 
	{
		free(palettebuffer);
		return 0;
	}
		
	int transp = transpcolor ? (1<<29) : 0;

	int a = NE_MaterialTexLoad(tex,GL_RGB256,lastx,lasty,TEXGEN_TEXCOORD | transp,(u8*)datapointer);
	free(datapointer);

	if(a == 0) 
	{
		NE_DebugPrint("NE_MaterialTexLoadBMPtoRGB256: Error while loading texture...");
		free(palettebuffer);
		return 0;
	}
	a = NE_PaletteLoad(pal, palettebuffer, numcolors, GL_RGB256);
	free(palettebuffer);

	if(a == 0)
	{
		NE_DebugPrint("NE_MaterialTexLoadBMPtoRGB256: Error while loading palette...");
		NE_MaterialDelete(tex);
		return 0;
	}

	NE_MaterialTexSetPal(tex,pal);
	return 1;
}



//----------------------------------------------------------------------------------
// INTERNAL USE - THIS CONVERTS THE BMPs INTO TEXTURES

static void * __NE_ConvertBMPtoRGBA(void * pointer, bool transpcolor)
{
	NE_BMP_HEADER * header = (void*)pointer;
	NE_INFO_BMP_HEADER * infoheader = (void*)((u8*)header + sizeof(NE_BMP_HEADER));
	u8 * IMAGEDATA = (void*)((u8*)infoheader + sizeof(NE_INFO_BMP_HEADER));
	if(header->type != 0x4D42) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGBA: Not a BMP file.");
		return NULL; //Not a bmp file
	}
	int sizex = infoheader->width;
	int sizey = infoheader->height;
	if(sizex>1024||sizey>1024) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGBA: BMP file too big (%d,%d).",sizex,sizey);
		return NULL; //Wrong size
	}

	if(infoheader->compression != 0) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGBA: Compressed BMP not supported.");
		return NULL; //Compressed bmp not supported
	}
	if(infoheader->bits != 16 && infoheader->bits != 24) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGBA: Unsuported depth for GL_RGBA conversion (%d).",infoheader->bits);
		return NULL; //Unsuported depth
	}

	//Decode
	u16 * buffer = malloc(2 * sizex * sizey);
	NE_AssertPointer(buffer,"__NE_ConvertBMPtoRGBA: Couldn't allocate temporal buffer.");

	u8 transr = 0, transb = 0, transg = 0;
	u16 transcolor16bit = 0;

	if(transpcolor)
	{
		if(infoheader->bits == 16) //X1RGB5
		{
			u16 red, green, blue;
			transcolor16bit = (u16)IMAGEDATA[2*sizey*(sizex-1)] | (((u16)IMAGEDATA[2*sizey*(sizex-1) + 1]) << 8);
			red = (transcolor16bit & 0x7C00) >> 10;
			green = (transcolor16bit & 0x3E0); //don't move it, it's in the right position
			blue = (transcolor16bit & 0x1F);
			transcolor16bit = red | green | (blue << 10);
		}
		else //24 bits
		{
			transr = IMAGEDATA[3*sizey*(sizex-1) + 2];
			transg = IMAGEDATA[3*sizey*(sizex-1) + 1];
			transb = IMAGEDATA[3*sizey*(sizex-1) + 0];
		}
	}
	//IMAGEDATA -> buffer

	int disalign = ((sizex * infoheader->bits) >> 3) & 3;
	if(disalign) disalign = 4 - disalign;

	if(infoheader->bits == 16) //X1RGB5
	{
		int y, x;
		for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
		{
			u16 red, green, blue;
			
			int base_pos = (sizex*(sizey-y-1)+x)<<1;
			
			if(disalign)
			{
				base_pos += disalign * (sizey-y-1);
			}
			
			u16 color = (u16)IMAGEDATA[base_pos] | (IMAGEDATA[base_pos + 1] << 8);		
			
			red = (color & 0x7C00) >> 10;
			green = (color & 0x3E0); //don't move it, it's in the right position
			blue = (color & 0x1F);
			color = red | green | (blue << 10);
			
			if(!(transpcolor&&color==transcolor16bit))
				buffer[y*sizex+x] = color | BIT(15);
			else
				buffer[y*sizex+x] = 0;
		}
	}
	else //24 bits
	{
		int y, x;
		for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
		{
			u8 r,g,b;
			
			int base_pos = 3*(sizex*(sizey-y-1)+x);
			
			if(disalign)
			{
				base_pos += disalign * (sizey-y-1);
			}
			
			r = IMAGEDATA[base_pos + 2];
			g = IMAGEDATA[base_pos + 1];
			b = IMAGEDATA[base_pos + 0];
			
			if(!(transpcolor&&r==transr&&g==transg&&b==transb))
				buffer[y*sizex+x] = RGB15((r >> 3) & 31 ,(g >> 3) & 31,(b >> 3) & 31) | BIT(15);
			else
				buffer[y*sizex+x] = 0;
		}
	}

	lasty = sizey;
	lastx = sizex;

	return (void*)buffer;
}



static void * __NE_ConvertBMPtoRGB256(void * pointer, u16 * palettebuffer)
{
	NE_BMP_HEADER * header = (void*)pointer;
	NE_INFO_BMP_HEADER * infoheader = (void*)((u8*)header + sizeof(NE_BMP_HEADER));
	if(header->type != 0x4D42) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGB256: Not a BMP file.");
		return NULL; //Not a bmp file
	}
	int sizex = infoheader->width;
	int sizey = infoheader->height;
	if(sizex>1024||sizey>1024) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGB256: BMP file too big (%d,%d).",sizex,sizey);
		return NULL; //Wrong size
	}

	if(infoheader->compression != 0) 
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGB256: Compressed BMP not supported.");
		return NULL; //Compressed bmp not supported
	}
	if(infoheader->bits != 8 && infoheader->bits != 4)
	{
		NE_DebugPrint("__NE_ConvertBMPtoRGB256: Unsuported depth for GL_RGB256 conversion (%d).",infoheader->bits);
		return NULL; //Unsuported depth
	}

	//Decode
	int colornumber = (infoheader->bits == 8) ? 256 : 16;
	u8 * PALETTEDATA = (void*)((u8*)infoheader + sizeof(NE_INFO_BMP_HEADER));
	u8 * IMAGEDATA = (u8*)header + header->offset;

	numcolors = colornumber; //numcolors is used by the other functions (look at the start of this file)

	int a = 0;
	while(a < numcolors) //First, we read the palette
	{
		u8 r,g,b;
		g = PALETTEDATA[(a<<2) + 1] & 0xFF;
		r = PALETTEDATA[(a<<2) + 2] & 0xFF;
		b = PALETTEDATA[(a<<2) + 0] & 0xFF;
		palettebuffer[a] = RGB15(r >> 3,g >> 3,b >> 3); 
		a++;
	}

	u8 * buffer = malloc(sizex * sizey);
	NE_AssertPointer(buffer,"__NE_ConvertBMPtoRGB256: Couldn't allocate temporal buffer.");

	int y, x; //Then, the image
	if(colornumber == 256)
	{
		int disalign = sizex & 3; //For bmps with width not X*4
		
		if(disalign) 
		{
			disalign = 4 - disalign;
			
			for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
			{
				buffer[y*sizex+x] = IMAGEDATA[(sizex*(sizey-y-1))+x  + (((disalign) * (sizey-y-1))*1)];
			}
			}
		else for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
		{
			buffer[y*sizex+x] = IMAGEDATA[(sizex*(sizey-y-1))+x];
		}
	}
	else //colornumber == 16
	{
		int disalign = sizex & 7; //For bmps with width not X*8
		
		if(disalign)
		{
			disalign = 8-disalign;
			for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
			{
				if(x&1)
					buffer[y*sizex+x] = IMAGEDATA[( (sizex*(sizey-y-1)+x) + (disalign * (sizey-y-1)) ) >> 1 ] & 0x0F;
				else
					buffer[y*sizex+x] = (IMAGEDATA[(  (sizex*(sizey-y-1)+x) + (disalign * (sizey-y-1)) ) >> 1 ] >> 4) & 0x0F;
			}
		}
		else
		{
			for(y=0;y<sizey;y++) for(x=0;x<sizex;x++)
			{
				if(x&1)
					buffer[y*sizex+x] = IMAGEDATA[(sizex*(sizey-y-1)+x)>>1] & 0x0F;
				else
					buffer[y*sizex+x] = (IMAGEDATA[(sizex*(sizey-y-1)+x)>>1] >> 4) & 0x0F;
			}
		}
	}


	lastx = sizex;
	lasty = sizey;

	return (void*)buffer;
}


//----------------------------------------------------------------------------------