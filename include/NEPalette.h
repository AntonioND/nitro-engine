// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_PALETTE_H__
#define NE_PALETTE_H__

#include <nds.h>

/// @file   NEPalette.h
/// @brief  Functions for loading, using and deleting palettes.

/// @defgroup palette_system Palette system
///
/// Functions to load and manipulate texture palettes.
///
/// @{

#define NE_DEFAULT_PALETTES 64 /// Default max number of palettes

/// Holds information of a palette.
typedef struct {
    int index; /// Index to internal palette object
} NE_Palette;

/// Creates a new palette object.
///
/// @return Pointer to the newly created palette.
NE_Palette *NE_PaletteCreate(void);

/// Loads a palette from the filesystem into a palette object.
///
/// @param pal Pointer to the palette object.
/// @param path Path of the palette.
/// @param format Format of the palette.
/// @return It returns 1 on success, 0 on error.
int NE_PaletteLoadFAT(NE_Palette *pal, char *path, int format);

/// Assign a palette in RAM to a palette object.
///
/// @param pal Pointer to the palette object.
/// @param pointer Pointer to the palette in RAM.
/// @param numcolor Number of colors of the palette.
/// @param format Format of the palette.
/// @return It returns 1 on success, 0 on error.
int NE_PaletteLoad(NE_Palette *pal, u16 *pointer, u16 numcolor, int format);

/// Deletes a palette object.
///
/// @param pal Pointer to the palette object.
void NE_PaletteDelete(NE_Palette *pal);

/// Tells the GPU to use the palette in the specified object.
///
/// @param pal Pointer to the palette object.
void NE_PaletteUse(NE_Palette *pal);

/// Resets the palette system and sets the new max number of palettes.
///
/// @param max_palettes Max number of palettes. If lower than 1, it will
///                     create space for NE_DEFAULT_PALETTES.
void NE_PaletteSystemReset(int max_palettes);

/// Returns the available free memory for palettes.
///
/// Note that, even if it is all available, it may not be contiguous, so you may
/// not be able to load a palette because there isn't enough space in any free
/// gap.
///
/// @return Returns the available memory in bytes.
int NE_PaletteFreeMem(void);

/// Returns the percentage of available free memory for palettes.
///
/// @return Returns the percentage of available memory (0-100).
int NE_PaletteFreeMemPercent(void);

/// Defragment memory used for palettes.
///
/// WARNING: This function is currently not working.
void NE_PaletteDefragMem(void);

/// End palette system and free all memory used by it.
void NE_PaletteSystemEnd(void);

/// Enables modification of the specified palette.
///
/// It unlocks VRAM so that you can modify the palette. It also returns a
/// pointer to the base address of the palette in VRAM.
///
/// Use this during VBL. Remember to use NE_PaletteModificationEnd() when you
/// finish. If you don't, the GPU won't be able to render textures to the
/// screen.
///
/// @param pal Palette to modify.
/// @return Returns a pointer to the base address of the palette in VRAM.
void *NE_PaletteModificationStart(NE_Palette *pal);

/// Set the desired entry of a palette to a new color.
///
/// Use this during VBL.
///
/// @param colorindex Color index to change.
/// @param color New color.
void NE_PaletteRGB256SetColor(u8 colorindex, u16 color);

/// Disables modification of palettes.
///
/// Use this during VBL.
void NE_PaletteModificationEnd(void);

/// @}

#endif // NE_PALETTE_H__
