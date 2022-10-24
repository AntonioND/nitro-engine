// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/// @file NEPolygon.c

void NE_PolyColor(u32 color)
{
    GFX_COLOR = color;
}

void NE_LightOff(int index)
{
    NE_AssertMinMax(0, index, 3, "Invalid light index %d", index);

    GFX_LIGHT_VECTOR = (index & 3) << 30;
    GFX_LIGHT_COLOR = (index & 3) << 30;
}

void NE_LightSetColor(int index, u32 color)
{
    NE_AssertMinMax(0, index, 3, "Invalid light number %d", index);

    GFX_LIGHT_COLOR = ((index & 3) << 30) | color;
}

void NE_LightSetI(int index, u32 color, int x, int y, int z)
{
    NE_AssertMinMax(0, index, 3, "Invalid light number %d", index);

    GFX_LIGHT_VECTOR = ((index & 3) << 30)
                     | ((z & 0x3FF) << 20) | ((y & 0x3FF) << 10) | (x & 0x3FF);
    GFX_LIGHT_COLOR = ((index & 3) << 30) | color;
}

void NE_PolyBegin(int mode)
{
    GFX_BEGIN = mode;
}

void NE_PolyEnd(void)
{
    GFX_END = 0;
}

void NE_PolyNormalI(int x, int y, int z)
{
    GFX_NORMAL = NORMAL_PACK(x, y, z);
}

void NE_PolyVertexI(int x, int y, int z)
{
    GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
    GFX_VERTEX16 = (uint32) (uint16) (z);
}

void NE_PolyTexCoord(int u, int v)
{
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(u), inttot16(v));
}

void NE_PolyFormat(u32 alpha, u32 id, NE_LightEnum lights,
                   NE_CullingEnum culling, NE_OtherFormatEnum other)
{
    NE_AssertMinMax(0, alpha, 31, "Invalid alpha value %lu", alpha);
    NE_AssertMinMax(0, id, 63, "Invalid polygon ID %lu", id);

    GFX_POLY_FORMAT = POLY_ALPHA(alpha) | POLY_ID(id)
                    | lights | culling | other;
}

void NE_OutliningEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_OUTLINE;
    else
        GFX_CONTROL &= ~GL_OUTLINE;
}

void NE_OutliningSetColor(u32 index, u32 color)
{
    NE_AssertMinMax(0, index, 7, "Invalaid outlining color index %lu", index);

    GFX_EDGE_TABLE[index] = color;
}

void NE_ShadingEnable(bool value)
{
    if (value)
    {
        for (int i = 0; i < 16; i++)
            GFX_TOON_TABLE[i] = RGB15(8, 8, 8);
        for (int i = 16; i < 32; i++)
            GFX_TOON_TABLE[i] = RGB15(24, 24, 24);
    }
    else
    {
        for (int i = 0; i < 32; i++)
            GFX_TOON_TABLE[i] = 0;
    }
}

void NE_ToonHighlightEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_TOON_HIGHLIGHT;
    else
        GFX_CONTROL &= ~GL_TOON_HIGHLIGHT;
}

void NE_FogEnable(u32 shift, u32 color, u32 alpha, int mass, int depth)
{
    NE_FogDisable();

    GFX_CONTROL |= GL_FOG | ((shift & 0xF) << 8);

    GFX_FOG_COLOR = color | ((alpha) << 16);
    GFX_FOG_OFFSET = depth;

    // Another option:
    //for (int i = 0; i < 32; i++)
    //    GFX_FOG_TABLE[i] = i << 2;

    // We need a 0 in the first fog table entry!
    int32 density = -(mass << 1);

    for (int i = 0; i < 32; i++)
    {
        density += mass << 1;
        // Entries are 7 bit, so cap the density to 127
        density = ((density > 127) ? 127 : density);
        GFX_FOG_TABLE[i] = density;
    }
}

// The GFX_CLEAR_COLOR register is write-only. This holds a copy of its value in
// order for the code to be able to modify individual fields.
static u32 ne_clearcolor = 0;

void NE_FogEnableBackground(bool value)
{
    if (value)
        ne_clearcolor |= BIT(15);
    else
        ne_clearcolor &= ~BIT(15);

    GFX_CLEAR_COLOR = ne_clearcolor;
}

void NE_FogDisable(void)
{
    GFX_CONTROL &= ~(GL_FOG | (15 << 8));
}

void NE_ClearColorSet(u32 color, u32 alpha, u32 id)
{
    NE_AssertMinMax(0, alpha, 31, "Invalid alpha value %lu", alpha);
    NE_AssertMinMax(0, id, 63, "Invalid polygon ID %lu", id);

    ne_clearcolor &= BIT(15);

    ne_clearcolor |= 0x7FFF & color;
    ne_clearcolor |= (0x1F & alpha) << 16;
    ne_clearcolor |= (0x3F & id) << 24;

    GFX_CLEAR_COLOR = ne_clearcolor;
}

// From NE_General.c
extern bool NE_Dual;

void NE_ClearBMPEnable(bool value)
{
    if (NE_Dual)
    {
        // It needs two banks that are used for the display capture
        NE_DebugPrint("Not available in dual 3D mode");
        return;
    }

    REG_CLRIMAGE_OFFSET = 0; // Set horizontal and vertical scroll to 0

    if (value)
    {
        vramSetBankC(VRAM_C_TEXTURE_SLOT2); // Slot 2 = clear color
        vramSetBankD(VRAM_D_TEXTURE_SLOT3); // Slot 3 = clear depth
        GFX_CONTROL |= GL_CLEAR_BMP;
    }
    else
    {
        GFX_CONTROL &= ~GL_CLEAR_BMP;
        // Let the user decide if they are used for textures or something else.
        //vramSetBankC(VRAM_C_LCD);
        //vramSetBankD(VRAM_D_LCD);
    }
}

void NE_ClearBMPScroll(u32 x, u32 y)
{
    REG_CLRIMAGE_OFFSET = x | (y << 8);
}
