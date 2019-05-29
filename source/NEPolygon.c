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

/*! \file   NEPolygon.c */


inline void NE_PolyColor(u32 color)
{
	GFX_COLOR = color;
}

inline void NE_LightOff(int num)
{
	NE_AssertMinMax(0,num,3,"NE_LightOff: Unexistent light number %d.",num);
	num = (num & 3) << 30;
	GFX_LIGHT_VECTOR = num;
	GFX_LIGHT_COLOR = num;
}

inline void NE_LightSetColor(int num, u32 color) 
{
	NE_AssertMinMax(0,num,3,"NE_LightSetColor: Unexistent light number %d.",num);
	GFX_LIGHT_COLOR = ((num & 3) << 30) | (vuint32)color;
}

inline void NE_LightSetI(int num, u32 color, int x, int y, int z) 
{
	NE_AssertMinMax(0,num,3,"NE_LightSetI: Unexistent light number %d.",num);
	num = (num & 3) << 30;
	GFX_LIGHT_VECTOR = num | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10)
						   | (x & 0x3FF);
	GFX_LIGHT_COLOR = num | (vuint32)color;
}

inline void NE_PolyBegin(int mode)
{
	GFX_BEGIN = mode;
}

inline void NE_PolyEnd(void)
{
	GFX_END = 0; 
}

inline void NE_PolyNormalI(int x, int y, int z)
{
	GFX_NORMAL = NORMAL_PACK(x,y,z);
}

inline void NE_PolyVertexI(int x, int y, int z)
{
	GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
	GFX_VERTEX16 = (uint32)(uint16)(z);
}

inline void NE_PolyTexCoord(int u, int v)
{
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(u),inttot16(v));
}

inline void NE_PolyFormat(u8 alpha, u8 id, NE_LIGHT_ENUM lights, NE_CULLING_ENUM culling, NE_OTHER_FORMAT_ENUM other)
{
	NE_AssertMinMax(0,alpha,31,"NE_PolyFormat: Wrong alpha value %d.",alpha);
	NE_AssertMinMax(0,id,63,"NE_PolyFormat: Wrong polygon ID %d.", id);

	GFX_POLY_FORMAT = POLY_ALPHA(alpha) | POLY_ID(id) | lights | culling | other;
}

inline void NE_OutliningEnable(bool value)
{
	if(value) GFX_CONTROL |= GL_OUTLINE;
	else  GFX_CONTROL &= ~GL_OUTLINE;
}


inline void NE_OutliningSetColor(u8 num, u32 color)
{
	NE_AssertMinMax(0,num,7,"NE_OutliningSetColor: Wrong outlining color index %d.",num);

	glSetOutlineColor(num,color);
}

void NE_ShadingEnable(bool value)
{
	if(value)
	{
		glSetToonTableRange( 0, 15, RGB15(8,8,8) );
		glSetToonTableRange( 16, 31, RGB15(24,24,24) );
	}
	else
	{
		glSetToonTableRange( 0, 31, 0 );
	}
}

inline void NE_ToonHighlightEnable(bool value)
{
	if(value) GFX_CONTROL |= GL_TOON_HIGHLIGHT;
	else GFX_CONTROL &= ~GL_TOON_HIGHLIGHT;
}


void NE_FogEnable(u8 shift, u32 color, u8 alpha, int mass, int depth)
{
	NE_FogDisable();

	GFX_CONTROL |= GL_FOG | ((shift & 0xF)<<8); //0-15

	GFX_FOG_COLOR = color | ((alpha)<<16); 
	GFX_FOG_OFFSET = depth;

	//int i; for (i = 0; i < 32; i++) { GFX_FOG_TABLE[i] = i<<2; }

	int32 density = -(mass << 1); // ... because we need a 0 in the first fog table entry!
	u8 i; for(i = 0; i < 32; i++)
	{
		density += mass << 1;
		density = ((density > 127) ? 127 : density); // entries are 7bit, so cap the density to 127
		GFX_FOG_TABLE[i] = density;
	}
}

static u32 ne_clearcolor = 0;

inline void NE_FogEnableBackground(bool value)
{
	if(value) ne_clearcolor |= BIT(15);
	else ne_clearcolor &= ~BIT(15);
	GFX_CLEAR_COLOR = ne_clearcolor;
}

inline void NE_FogDisable(void)
{
	GFX_CONTROL &= ~(GL_FOG | (15<<8));
}



inline void NE_ClearColorSet(u32 color, u8 alpha, u8 id)
{
	NE_AssertMinMax(0,alpha,31,"NE_ClearColorSet: Wrong alpha value %d.", alpha);
	NE_AssertMinMax(0,id,63,"NE_ClearColorSet: Wrong polygon ID %d.", id);

	ne_clearcolor &= BIT(15);

	ne_clearcolor |= 0x7FFF & color;
	ne_clearcolor |= (0x1F & alpha) << 16;
	ne_clearcolor |= (0x3F & id) << 24;

	GFX_CLEAR_COLOR = ne_clearcolor;
}


extern bool NE_Dual;


inline void NE_ClearBMPEnable(bool value)
{
	if(NE_Dual) 
	{
		NE_DebugPrint("NE_ClearBMPEnable: Not available in dual 3D mode.");
		return; //It needs two banks that are used for the display capture
	}

	REG_CLEARBMPOFFSET = 0; //Scroll = 0

	if(value) 
	{
		vramSetBankC(VRAM_C_TEXTURE_SLOT2); //Slot 2 = clear color
		vramSetBankD(VRAM_D_TEXTURE_SLOT3); //Slot 3 = clear depth
		GFX_CONTROL |= GL_CLEAR_BMP;
	}
	else 
	{
		GFX_CONTROL &= ~GL_CLEAR_BMP;
//		vramSetBankC(VRAM_C_LCD);
//		vramSetBankD(VRAM_D_LCD);
	}
}


inline void NE_ClearBMPScroll(u8 x, u8 y)
{
	REG_CLEARBMPOFFSET = x | (((u16)y)<<8);
}

