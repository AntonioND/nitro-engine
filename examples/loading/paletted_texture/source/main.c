// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a5pal8_tex_bin.h"
#include "a5pal8_pal_bin.h"

#include "a3pal32_tex_bin.h"
#include "a3pal32_pal_bin.h"

NE_Material *Material1, *Material2;
NE_Palette *Palette1, *Palette2;

void Draw3DScene(void)
{
    NE_2DViewInit();

    NE_2DDrawTexturedQuad(0, 0,
                          256, 192,
                          0, Material1);
}

void Draw3DScene2(void)
{
    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 32,
                          64 + 128, 32 + 128,
                          0, Material2);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_InitDual3D();

    // Allocate objects
    Material1 = NE_MaterialCreate();
    Material2 = NE_MaterialCreate();
    Palette1 = NE_PaletteCreate();
    Palette2 = NE_PaletteCreate();

    // Load part of the texture ignoring some of its height. You can't do
    // this with width because of how textures are laid out in VRAM.
    NE_MaterialTexLoad(Material1, NE_A3PAL32, 256, 192, NE_TEXGEN_TEXCOORD,
                       (u8 *)a3pal32_tex_bin);

    // Load complete texture
    NE_MaterialTexLoad(Material2, NE_A5PAL8, 256, 256, NE_TEXGEN_TEXCOORD,
                       (u8 *)a5pal8_tex_bin);

    NE_PaletteLoad(Palette1, (u16 *)a3pal32_pal_bin, 32, NE_A3PAL32);
    NE_PaletteLoad(Palette2, (u16 *)a5pal8_pal_bin, 32, NE_A5PAL8);

    NE_MaterialTexSetPal(Material1, Palette1);
    NE_MaterialTexSetPal(Material2, Palette2);

    while (1)
    {
        NE_ProcessDual(Draw3DScene, Draw3DScene2);
        NE_WaitForVBL(0);
    }

    return 0;
}
