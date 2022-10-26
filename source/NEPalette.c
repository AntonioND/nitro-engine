// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"
#include "NEAlloc.h"

/// @file NEPalette.c

typedef struct {
    u16 *pointer;
    int format;
} ne_palinfo_t;

static ne_palinfo_t *NE_PalInfo = NULL;
static NE_Palette **NE_UserPalette = NULL;

static NEChunk *NE_PalAllocList; // See NEAlloc.h

static bool ne_palette_system_inited = false;

static int NE_MAX_PALETTES;

NE_Palette *NE_PaletteCreate(void)
{
    if (!ne_palette_system_inited)
        return NULL;

    for (int i = 0; i < NE_MAX_PALETTES; i++)
    {
        if (NE_UserPalette[i] != NULL)
            continue;

        NE_Palette *ptr = malloc(sizeof(NE_Palette));
        NE_AssertPointer(ptr, "Not enough memory");
        ptr->index = NE_NO_PALETTE;
        NE_UserPalette[i] = ptr;
        return ptr;
    }

    NE_DebugPrint("No free slots");

    return NULL;
}

int NE_PaletteLoadFAT(NE_Palette *pal, char *path, int format)
{
    if (!ne_palette_system_inited)
        return 0;

    NE_AssertPointer(pal, "NULL palette pointer");
    NE_AssertPointer(path, "NULL path pointer");

    u32 size = NE_FATFileSize(path);
    if (size < 1)
    {
        NE_DebugPrint("Couldn't obtain file size");
        return 0;
    }

    char *ptr = NE_FATLoadData(path);
    NE_AssertPointer(ptr, "Couldn't load file from FAT");
    int ret = NE_PaletteLoad(pal, (u16 *) ptr, size >> 1, format);
    free(ptr);

    return ret;
}

int NE_PaletteLoad(NE_Palette *pal, u16 *pointer, u16 numcolor, int format)
{
    if (!ne_palette_system_inited)
        return 0;

    NE_AssertPointer(pal, "NULL pointer");

    if (pal->index != NE_NO_PALETTE)
    {
        NE_DebugPrint("Palette already loaded");
        NE_PaletteDelete(pal);
    }

    int slot = NE_NO_PALETTE;

    for (int i = 0; i < NE_MAX_PALETTES; i++)
    {
        if (NE_PalInfo[i].pointer == NULL)
        {
            slot = i;
            break;
        }
    }

    if (slot == NE_NO_PALETTE)
    {
        NE_DebugPrint("No free lots");
        return 0;
    }

    NE_PalInfo[slot].pointer = NE_Alloc(NE_PalAllocList, numcolor << 1);
    // Aligned to 16 bytes (except 8 bytes for NE_PAL4).

    if (NE_PalInfo[slot].pointer == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return 0;
    }

    NE_PalInfo[slot].format = format;

    pal->index = slot;

    // Allow CPU writes to VRAM_E
    vramSetBankE(VRAM_E_LCD);
    swiCopy(pointer, NE_PalInfo[slot].pointer, (numcolor / 2) | COPY_MODE_WORD);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    return 1;
}

void NE_PaletteDelete(NE_Palette *pal)
{
    if (!ne_palette_system_inited)
        return;

    NE_AssertPointer(pal, "NULL pointer");

    // If there is an asigned palette...
    if (pal->index != NE_NO_PALETTE)
    {
        NE_Free(NE_PalAllocList, (void *)NE_PalInfo[pal->index].pointer);
        NE_PalInfo[pal->index].pointer = NULL;
    }

    for (int i = 0; i < NE_MAX_PALETTES; i++)
    {
        if (NE_UserPalette[i] == pal)
        {
            NE_UserPalette[i] = NULL;
            free(pal);
            return;
        }
    }

    NE_DebugPrint("Material not found");
}

void NE_PaletteUse(const NE_Palette *pal)
{
    NE_AssertPointer(pal, "NULL pointer");
    NE_Assert(pal->index != NE_NO_PALETTE, "No asigned palette");
    unsigned int shift = 4 - (NE_PalInfo[pal->index].format == NE_PAL4);
    GFX_PAL_FORMAT = (uintptr_t)NE_PalInfo[pal->index].pointer >> shift;
}

void NE_PaletteSystemReset(int max_palettes)
{
    if (ne_palette_system_inited)
        NE_PaletteSystemEnd();

    if (max_palettes < 1)
        NE_MAX_PALETTES = NE_DEFAULT_PALETTES;
    else
        NE_MAX_PALETTES = max_palettes;

    NE_PalInfo = malloc(NE_MAX_PALETTES * sizeof(ne_palinfo_t));
    NE_AssertPointer(NE_PalInfo, "Not enough memory");
    NE_UserPalette = malloc(NE_MAX_PALETTES * sizeof(NE_UserPalette));
    NE_AssertPointer(NE_UserPalette, "Not enough memory");

    NE_AllocInit(&NE_PalAllocList, (void *)VRAM_E, (void *)VRAM_F);

    memset(NE_PalInfo, 0, NE_MAX_PALETTES * sizeof(ne_palinfo_t));
    memset(NE_UserPalette, 0, NE_MAX_PALETTES * sizeof(NE_UserPalette));

    ne_palette_system_inited = true;

    GFX_PAL_FORMAT = 0;
}

int NE_PaletteFreeMem(void)
{
    if (!ne_palette_system_inited)
        return 0;

    NEMemInfo info;
    NE_MemGetInformation(NE_PalAllocList, &info);

    return info.free;
}

int NE_PaletteFreeMemPercent(void)
{
    if (!ne_palette_system_inited)
        return 0;

    NEMemInfo info;
    NE_MemGetInformation(NE_PalAllocList, &info);

    return info.free_percent;
}

void NE_PaletteDefragMem(void)
{
    NE_Assert(0, "This function doesn't work");
    return;

    // TODO: Fix

    /*
    if (!ne_palette_system_inited)
        return;

    vramSetBankE(VRAM_E_LCD);
    bool ok = false;
    while (!ok)
    {
        ok = true;
        for (int i = 0; i < NE_MAX_PALETTES; i++)
        {
            int size = NE_GetSize(NE_PalAllocList, (void*)NE_PalInfo[i].pointer);

            NE_Free(NE_PalAllocList, (void*)NE_PalInfo[i].pointer);
            void *pointer = NE_Alloc(NE_PalAllocList, size);
            // Aligned to 16 bytes (except 8 bytes for NE_PAL4).

            NE_AssertPointer(pointer, "Couldn't reallocate palette");

            if ((int)pointer != (int)NE_PalInfo[i].pointer)
            {
                dmaCopy((void*)NE_PalInfo[i].pointer, pointer, size);

                NE_PalInfo[i].pointer = (void*)pointer;
                ok = false;
            }
        }
    }
    vramSetBankE(VRAM_E_TEX_PALETTE);
    */
}

void NE_PaletteSystemEnd(void)
{
    if (!ne_palette_system_inited)
        return;

    NE_AllocEnd(&NE_PalAllocList);

    free(NE_PalInfo);

    for (int i = 0; i < NE_MAX_PALETTES; i++)
    {
        if (NE_UserPalette[i])
            free(NE_UserPalette[i]);
    }

    free(NE_UserPalette);

    ne_palette_system_inited = false;
}

static u16 *palette_adress = NULL;
static int palette_format;

void *NE_PaletteModificationStart(const NE_Palette *pal)
{
    NE_AssertPointer(pal, "NULL pointer");
    NE_Assert(pal->index != NE_NO_PALETTE, "No asigned palette");
    NE_Assert(palette_adress == NULL, "Another palette already active");

    palette_adress = NE_PalInfo[pal->index].pointer;
    palette_format = NE_PalInfo[pal->index].format;

    // Enable CPU accesses to VRAM_E
    vramSetBankE(VRAM_E_LCD);

    return palette_adress;
}

void NE_PaletteRGB256SetColor(u8 colorindex, u16 color)
{
    NE_AssertPointer(palette_adress, "No active palette");
    NE_Assert(palette_format == NE_PAL256, "Active palette isn't NE_PAL256");

    palette_adress[colorindex] = color;
}

void NE_PaletteModificationEnd(void)
{
    NE_Assert(palette_adress != NULL, "No active palette");

    // Disable CPU accesses to VRAM_E
    vramSetBankE(VRAM_E_TEX_PALETTE);

    palette_adress = NULL;
}
