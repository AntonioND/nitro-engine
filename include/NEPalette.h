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

#ifndef __NE_PALETTE_H__
#define __NE_PALETTE_H__

#include <nds.h>

/*! \file   NEPalette.h
 *  \brief  Functions for loading, using and deleting palettes. */

/*! @defgroup palette_system Palette system.

	Functions to manipulate texture palettes.
	@{
*/

/*! \def   #define NE_DEFAULT_PALETTES 512 */
#define NE_DEFAULT_PALETTES 512

/*! \struct NE_TextPalette
 *  \brief  Holds information for a palette. */
typedef struct {
	int index;
} NE_Palette;

/*! \fn    NE_TextPalette * NE_PaletteCreate(void);
 *  \brief Returns a pointer to a NE_TextPalette struct. */
NE_Palette * NE_PaletteCreate(void);

/*! \fn    int NE_PaletteLoadFAT(NE_Palette * pal, char * path, u8 format);
 *  \brief Load a palette from FAT and assign it to a NE_TextPalette struct. Returns 1 if OK. 
 *  \param pal Pointer to the palette.
 *  \param path Path of the palette.
 *  \param format Format of the palette. */
int NE_PaletteLoadFAT(NE_Palette * pal, char * path, u8 format);

/*! \fn    int NE_PaletteLoad(NE_Palette * pal, u16* pointer, u16 numcolor, u8 format);
 *  \brief Assign a palette in RAM to a NE_TextPalette struct. Returns 1 if OK. 
 *  \param pal Pointer to the palette.
 *  \param pointer Pointer to the palette.
 *  \param numcolor Number of colors of the palette.
 *  \param format Format of the palette. */
int  NE_PaletteLoad(NE_Palette * pal, u16* pointer, u16 numcolor, u8 format);

/*! \fn    void NE_PaletteDelete(NE_Palette * pal);
 *  \brief Delete a NE_Palette struct. 
 *  \param pal Pointer to the palette. */
void NE_PaletteDelete(NE_Palette * pal);

/*! \fn    void NE_PaletteUse(NE_Palette * pal);
 *  \brief Use a palette assigned to a NE_TextPalette struct. 
 *  \param pal Pointer to the palette struct. */
void NE_PaletteUse(NE_Palette * pal);

/*! \fn    void NE_PaletteSystemReset(int palette_number);
 *  \brief Resets the palette system and sets the new number of palettes.
 *  \param palette_number Max number of palettes. If lower than 1, it will 
           create space for NE_DEFAULT_PALETTES. */
void NE_PaletteSystemReset(int palette_number);

/*! \fn    int NE_PaletteFreeMem(void);
 *  \brief Returns total free mem for palettes. */
int NE_PaletteFreeMem(void);

/*! \fn    int NE_PaletteFreeMemPercent(void);
 *  \brief Returns percent of total free mem for palettes. */
int NE_PaletteFreeMemPercent(void);

/*! \fn    void NE_PaletteDefragMem(void);
 *  \brief Defragment memory used for palettes. 

 NOT WORKING */
void NE_PaletteDefragMem(void);

/*! \fn    void NE_PaletteSystemEnd(void);
 *  \brief Terminate palette system, frees memory used by it and prevent any palette from loading. */
void NE_PaletteSystemEnd(void);

/*! \fn    void * NE_PaletteModificationStart(NE_Palette * pal);
 *  \brief Enables modification of given palette. Returns a pointer to the data.
 *  \param pal Palette to modify. 
 
Use this DURING VBL. YOU MUST USE NE_PaletteModificationEnd() WHEN YOU FINISH. 
IF YOU DON'T, GPU WON'T BE ABLE TO RENDER THE TEXTURES TO SCREEN. */
void * NE_PaletteModificationStart(NE_Palette * pal);

/*! \fn    void NE_PaletteRGB256SetColor(u8 colorindex, u16 color);
 *  \brief Set color number "colorindex" to "color". 
 *  \param colorindex Index to change.
 *  \param color New color. 

Use this DURING VBL. */
void NE_PaletteRGB256SetColor(u8 colorindex, u16 color);

/*! \fn    void NE_PaletteModificationEnd(void);
 *  \brief Ends modification of palettes.

Use this DURING VBL. */
void NE_PaletteModificationEnd(void);

/*! @} */ //palette_system

#endif //__NE_PALETTE_H__
