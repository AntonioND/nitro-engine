// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "grill_idx_bin.h"
#include "grill_pal_bin.h"
#include "grill_tex_bin.h"
#include "landscape_idx_bin.h"
#include "landscape_pal_bin.h"
#include "landscape_tex_bin.h"

NE_Material *Material1, *Material2;
NE_Palette *Palette1, *Palette2;

void Draw3DScene1(void)
{
    NE_ClearColorSet(RGB15(5, 5, 10), 31, 63);

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(32, 32,
                          32 + 128, 32 + 128,
                          0, Material1);
}

void Draw3DScene2(void)
{
    NE_ClearColorSet(RGB15(10, 5, 0), 31, 63);

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 0,
                          64 + 128, 0 + 128,
                          0, Material2);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_InitDual3D();

    // Allocate objects
    Material1 = NE_MaterialCreate();
    Material2 = NE_MaterialCreate();
    Palette1 = NE_PaletteCreate();
    Palette2 = NE_PaletteCreate();

    NE_MaterialTex4x4Load(Material1, 128, 128, NE_TEXGEN_TEXCOORD,
                          (u8 *)grill_tex_bin,
                          (u8 *)grill_idx_bin);
    NE_PaletteLoadSize(Palette1, (u16 *)grill_pal_bin, grill_pal_bin_size,
                       NE_TEX4X4);
    NE_MaterialSetPalette(Material1, Palette1);

    NE_MaterialTex4x4Load(Material2, 128, 128, NE_TEXGEN_TEXCOORD,
                          (u8 *)landscape_tex_bin,
                          (u8 *)landscape_idx_bin);
    NE_PaletteLoadSize(Palette2, (u16 *)landscape_pal_bin,
                       landscape_pal_bin_size, NE_TEX4X4);
    NE_MaterialSetPalette(Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDual(Draw3DScene1, Draw3DScene2);
    }

    return 0;
}
