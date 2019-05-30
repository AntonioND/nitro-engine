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

/*! \file   NEText.c */

typedef struct {
	int sizex, sizey;
	NE_Material * material;
} _NE_TextInfo_;
	
static _NE_TextInfo_ NE_TextInfo[NE_MAX_TEXT_FONTS];

static int NE_TEXT_PRIORITY = 0;

inline void NE_TextPrioritySet(int priority)
{
	NE_TEXT_PRIORITY = priority;
}

inline void NE_TextPriorityReset(void)
{
	NE_TEXT_PRIORITY = 0;
}

void NE_TextInit(int slot, NE_Material * mat, int sizex, int sizey)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextInit: Text slot %d unavailable.",slot);
	NE_AssertPointer(mat,"NE_TextInit: NULL pointer.");

	NE_TextInfo[slot].sizex = sizex;
	NE_TextInfo[slot].sizey = sizey;
	NE_TextInfo[slot].material = mat;
}

void NE_TextEnd(int slot)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextEnd: Text slot %d unavailable.",slot);

	NE_TextInfo[slot].sizex = 0;
	NE_TextInfo[slot].sizey = 0;
	NE_TextInfo[slot].material = NULL;
}


void NE_TextResetSystem(void)
{
	int a;
	for(a = 0; a < NE_MAX_TEXT_FONTS; a++)
	{
		NE_TextInfo[a].sizex = 0;
		NE_TextInfo[a].sizey = 0;
		NE_TextInfo[a].material = NULL;
	}
}


static inline void _ne_texturecuadprint(int xcrd1, int ycrd1, int xcrd2, int ycrd2, int xtx1, int ytx1, int xtx2, int ytx2)
{
	//------------ INTERNAL USE  ----------------------------
	GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx1));
	GFX_VERTEX16 = (ycrd1 << 16) | (xcrd1 & 0xFFFF);
	GFX_VERTEX16 = NE_TEXT_PRIORITY;

	GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx1), inttot16(ytx2));
	GFX_VERTEX_XY = (ycrd2 << 16) | (xcrd1 & 0xFFFF);

	GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx2));
	GFX_VERTEX_XY = (ycrd2 << 16) | (xcrd2 & 0xFFFF);

	GFX_TEX_COORD = TEXTURE_PACK(inttot16(xtx2), inttot16(ytx1));
	GFX_VERTEX_XY = (ycrd1 << 16) | (xcrd2 & 0xFFFF);
}

static inline void _ne_charprint(_NE_TextInfo_ * textinfo, int xcrd1, int ycrd1, char character)
{
	//------------ INTERNAL USE  ----------------------------
	//Texture coords
	int xcoord = ((character & 31) * textinfo->sizex);
	int xcoord2 = (xcoord + textinfo->sizex); 
		
	int ycoord = ((character >> 5) * textinfo->sizey);
	int ycoord2 = ycoord + textinfo->sizey;

	_ne_texturecuadprint(xcrd1,ycrd1, xcrd1+textinfo->sizex,ycrd1+textinfo->sizey,  
			xcoord,ycoord, xcoord2,ycoord2);
}



int NE_TextPrint(int slot, int x, int y, u32 color, const char * text)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextPrint: Text slot %d unavailable.",slot);

	_NE_TextInfo_ * textinfo = &NE_TextInfo[slot];

	if(textinfo->material == NULL) return -1;

	NE_MaterialUse(textinfo->material);
	GFX_COLOR = color;

	int count = 0;
	int x_ = x * textinfo->sizex, y_ = y * textinfo->sizey;

	GFX_BEGIN = GL_QUADS;

	while(1)
	{
		if(text[count] == '\0') break;
		else if(text[count] == '\n') 
			{ y_ += textinfo->sizey; x_ = 0; count++;}	
		else
		{
			if(x_ > 255) { y_ += textinfo->sizey; x_ = 0; }
			if(y_ > 191) break;
			
			_ne_charprint(textinfo, x_, y_, text[count]);
			
			count++;
			x_ += textinfo->sizex;
		}
	}
	return count;
}



int NE_TextPrintBox(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextPrintBox: Text slot %d unavailable.",slot);

	_NE_TextInfo_ * textinfo = &NE_TextInfo[slot];

	if(textinfo->material == NULL) return -1;

	NE_MaterialUse(textinfo->material);
	GFX_COLOR = color;

	int count = 0;
	int x_ = x * textinfo->sizex, y_ = y * textinfo->sizey;
	int xlimit = endx * textinfo->sizex, ylimit = endy * textinfo->sizey;
	ylimit = (ylimit > 191) ? 191 : ylimit;

	if(charnum < 0) charnum = 0x0FFFFFFF;

	GFX_BEGIN = GL_QUADS;

	while(1)
	{
		if(charnum <= count) break;
		else if(text[count] == '\0') break;
		else if(text[count] == '\n') { y_ += textinfo->sizey; x_ = x * textinfo->sizex; count++;}	
		else
		{
			if(x_ > xlimit) { y_ += textinfo->sizey; x_ = x * textinfo->sizex; }
			if(y_ > ylimit) break;
			
			_ne_charprint(textinfo, x_, y_, text[count]);
			
			count++;
			x_ += textinfo->sizex;
		}
	}
	return count;
}


int NE_TextPrintFree(int slot, int x, int y, u32 color, const char * text)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextPrintFree: Text slot %d unavailable.",slot);

	_NE_TextInfo_ * textinfo = &NE_TextInfo[slot];

	if(textinfo->material == NULL) return -1;

	NE_MaterialUse(textinfo->material);
	GFX_COLOR = color;

	int count = 0;
	int x_ = x, y_ = y;

	GFX_BEGIN = GL_QUADS;

	while(1)
	{
		if(text[count] == '\0') break;
		else
		{
			if(x_ > 255) break;
			
			_ne_charprint(textinfo, x_, y_, text[count]);
			
			count++;
			x_ += textinfo->sizex;
		}
	}
	return count;
}


int NE_TextPrintBoxFree(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text)
{
	NE_AssertMinMax(0,slot,NE_MAX_TEXT_FONTS,"NE_TextPrintBoxFree: Text slot %d unavailable.",slot);

	_NE_TextInfo_ * textinfo = &NE_TextInfo[slot];

	if(textinfo->material == NULL) return -1;

	NE_MaterialUse(textinfo->material);
	GFX_COLOR = color;

	int count = 0;
	int x_ = x, y_ = y;
	int xlimit = endx;
	int ylimit = (endy > 191) ? 191 : endy; 

	if(charnum < 0) charnum = 0x0FFFFFFF;

	GFX_BEGIN = GL_QUADS;

	while(1)
	{
		if(charnum <= count) break;
		else if(text[count] == '\0') break;
		else if(text[count] == '\n') { y_ += textinfo->sizey; x_ = x; count++;}	
		else
		{
			if(x_ > xlimit) { y_ += textinfo->sizey; x_ = x; }
			if(y_ > ylimit) break;
			
			_ne_charprint(textinfo, x_, y_, text[count]);
			
			count++;
			x_ += textinfo->sizex;
		}
	}
	return count;
}




