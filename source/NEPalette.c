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
#include "NEAlloc.h"

/*! \file   NEPalette.c */

typedef struct {
	u16 * pointer;
	u8 format;
} _NE_PalInfo_;
	
static _NE_PalInfo_ * NE_PalInfo = NULL;
static NE_Palette ** NE_UserPalette = NULL;

static NEChunk * NE_PalAllocList; //See NEAlloc.h

static bool ne_palette_system_inited = false;

static int NE_MAX_PALETTES;

NE_Palette * NE_PaletteCreate(void)
{
	if(!ne_palette_system_inited) return NULL;

	int a;
	for(a = 0; a < NE_MAX_PALETTES; a++) if(NE_UserPalette[a] == NULL) 
	{
		NE_Palette * ptr = malloc(sizeof(NE_Palette));
		NE_AssertPointer(ptr,"NE_PaletteCreate: Couldn't allocate palette information.");
		ptr->index = NE_NO_PALETTE;
		NE_UserPalette[a] = ptr;
		return ptr;
	}

	NE_DebugPrint("NE_PaletteCreate: No free slots...");

	return NULL;
}

int NE_PaletteLoadFAT(NE_Palette * pal, char * path, u8 format)
{
	if(!ne_palette_system_inited) return 0;

	NE_AssertPointer(pal,"NE_PaletteLoadFAT: NULL palette pointer.");
	NE_AssertPointer(path,"NE_PaletteLoadFAT: NULL path pointer.");

	u32 size = NE_FATFileSize(path);
	if(size < 1) 
	{
		NE_DebugPrint("NE_PaletteLoadFAT: Couldn't read file size.");
		return 0;
	}

	char * ptr = NE_FATLoadData(path);
	NE_AssertPointer(ptr,"NE_PaletteLoadFAT: Couldn't load data from FAT.");
	int a = NE_PaletteLoad(pal, (u16*) ptr, size>>1, format);
	free(ptr);
	return a;
}


int NE_PaletteLoad(NE_Palette * pal, u16* pointer, u16 numcolor, u8 format) 
{
	if(!ne_palette_system_inited) return 0;

	NE_AssertPointer(pal,"NE_PaletteLoad: NULL pointer.");

	if(pal->index != NE_NO_PALETTE) 
	{
		NE_DebugPrint("NE_PaletteLoad: Another palette loaded.");
		NE_PaletteDelete(pal);
	}

	int slot = NE_NO_PALETTE;

	int a;
	for(a = 0; a < NE_MAX_PALETTES; a++)
	{
		if(NE_PalInfo[a].pointer == NULL) 
		{
			pal->index = a;
			slot = a;
			a = NE_MAX_PALETTES;
		}
	}

	if(slot == NE_NO_PALETTE) 
	{
		NE_DebugPrint("NE_PaletteLoad: No free palette slots...");
		return 0;
	}

	NE_PalInfo[slot].pointer = NE_Alloc(NE_PalAllocList,numcolor << 1,1<<(4-(format==GL_RGB4)));
	if(NE_PalInfo[slot].pointer == NULL) 
	{
		NE_DebugPrint("NE_PaletteLoad: Not enough free space...");
		return 0;
	}

	NE_PalInfo[slot].format = format;
	vramSetBankE(VRAM_E_LCD);
	swiCopy(pointer, NE_PalInfo[slot].pointer, (numcolor>>1) | COPY_MODE_WORD);
	vramSetBankE(VRAM_E_TEX_PALETTE);
	return 1;
}


void NE_PaletteDelete(NE_Palette * pal)
{
	if(!ne_palette_system_inited) return;
	NE_AssertPointer(pal,"NE_PaletteDelete: NULL pointer.");

	if(pal->index != NE_NO_PALETTE) //If there is an asigned palette...
	{
		NE_Free(NE_PalAllocList,(void*)NE_PalInfo[pal->index].pointer);
		NE_PalInfo[pal->index].pointer = NULL;
	}

	int a;
	for(a = 0; a < NE_MAX_PALETTES; a++) if(NE_UserPalette[a] == pal) 
	{
		NE_UserPalette[a] = NULL;
		free(pal);
		return;
	}

	NE_DebugPrint("NE_PaletteDelete: Material not found in array.");
} 


void NE_PaletteUse(NE_Palette * pal) 
{
	NE_AssertPointer(pal,"NE_PaletteUse: NULL pointer.");
	NE_Assert(pal->index != NE_NO_PALETTE,"NE_PaletteUse: No asigned palette.");
	GFX_PAL_FORMAT = (int)NE_PalInfo[pal->index].pointer>>(4-(NE_PalInfo[pal->index].format==GL_RGB4));
}

void NE_PaletteSystemReset(int palette_number)
{
	if(ne_palette_system_inited) NE_PaletteSystemEnd();

	if(palette_number < 1) NE_MAX_PALETTES = NE_DEFAULT_PALETTES;
	else NE_MAX_PALETTES = palette_number;

	NE_PalInfo = malloc(NE_MAX_PALETTES * sizeof(_NE_PalInfo_));
	NE_AssertPointer(NE_PalInfo,"NE_PaletteSystemReset: Not enough memory to allocate palette array.");
	NE_UserPalette = malloc(NE_MAX_PALETTES * sizeof(NE_UserPalette));
	NE_AssertPointer(NE_UserPalette,"NE_PaletteSystemReset: Not enough memory to allocate control array.");

	NE_AllocInit(&NE_PalAllocList,(void*)VRAM_E,(void*)VRAM_F);

	memset(NE_PalInfo,0,NE_MAX_PALETTES * sizeof(_NE_PalInfo_));
	memset(NE_UserPalette,0,NE_MAX_PALETTES * sizeof(NE_UserPalette));

	ne_palette_system_inited = true;

	GFX_PAL_FORMAT = 0;
}

inline int NE_PaletteFreeMem(void)
{
	if(!ne_palette_system_inited) return 0;

	NEMemInfo Info;
	NE_MemGetInformation(NE_PalAllocList,&Info);

	return Info.Free;
}

inline int NE_PaletteFreeMemPercent(void)
{
	if(!ne_palette_system_inited) return 0;

	NEMemInfo Info;
	NE_MemGetInformation(NE_PalAllocList,&Info);

	return Info.FreePercent;
}


void NE_PaletteDefragMem(void)
{
	NE_Assert(0,"NE_PaletteDefragMem doesn't work.\n");
	return;
	/*
	if(!ne_palette_system_inited) return;

	vramSetBankE(VRAM_E_LCD);
	bool ok = false;
	while(!ok)
	{
		ok = true;
		int a;
		for(a = 0; a < NE_MAX_PALETTES; a++)
		{
			int size = NE_GetSize(NE_PalAllocList,(void*)NE_PalInfo[a].pointer);
			
			NE_Free(NE_PalAllocList,(void*)NE_PalInfo[a].pointer);
			void * pointer = NE_Alloc(NE_PalAllocList,size,1<<(4-(NE_PalInfo[a].format==GL_RGB4)));
			
			NE_AssertPointer(pointer,"NE_PaletteDefragMem: Couldn't reallocate palette.");
			
			if((int)pointer != (int)NE_PalInfo[a].pointer)
			{
				dmaCopy( (void*)NE_PalInfo[a].pointer, pointer, size ); 
				
				NE_PalInfo[a].pointer = (void*)pointer;
				ok = false;
			}
		}
	}
	vramSetBankE(VRAM_E_TEX_PALETTE);
	*/
}


void NE_PaletteSystemEnd(void)
{
	if(!ne_palette_system_inited) return;

	NE_AllocEnd(NE_PalAllocList);

	free(NE_PalInfo);
		
	int a;
	for(a = 0; a < NE_MAX_PALETTES; a++) if(NE_UserPalette[a]) 
		free(NE_UserPalette[a]);

	free(NE_UserPalette);

	ne_palette_system_inited = false;
}

//-----------------------------------------------------------------------------------------

static u16 * palette_adress = NULL;
static int palette_format;

void * NE_PaletteModificationStart(NE_Palette * pal)
{
	NE_AssertPointer(pal,"NE_PaletteModificationStart: NULL pointer.");
	NE_Assert(pal->index != NE_NO_PALETTE,"NE_PaletteModificationStart: No asigned palette.");

	NE_Assert(palette_adress == NULL,"NE_PaletteModificationStart: Another palette enabled for drawing.");

	palette_adress = NE_PalInfo[pal->index].pointer;
	palette_format = NE_PalInfo[pal->index].format;

	vramSetBankE(VRAM_E_LCD);

	return palette_adress;
}

void NE_PaletteRGB256SetColor(u8 colorindex, u16 color)
{
	NE_AssertPointer(palette_adress,"NE_PaletteRGB256SetColor: No palette enabled for modifying.");
	NE_Assert(palette_format == GL_RGB256,"NE_PaletteRGB256SetColor: Enabled palette isn't GL_RGB256.");

	palette_adress[colorindex] = color;
}

void NE_PaletteModificationEnd(void)
{
	NE_Assert(palette_adress != NULL,"NE_PaletteModificationEnd: No palette enabled for modifying.");

	vramSetBankE(VRAM_E_TEX_PALETTE);

	palette_adress = NULL;
}

