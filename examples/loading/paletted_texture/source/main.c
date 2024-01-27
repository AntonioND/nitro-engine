// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a5pal8.h"
#include "a3pal32.h"

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
                       a3pal32Bitmap);

    // Load complete texture
    NE_MaterialTexLoad(Material2, NE_A5PAL8, 256, 256, NE_TEXGEN_TEXCOORD,
                       a5pal8Bitmap);

    NE_PaletteLoad(Palette1, a3pal32Pal, 32, NE_A3PAL32);
    NE_PaletteLoad(Palette2, a5pal8Pal, 32, NE_A5PAL8);

    NE_MaterialSetPalette(Material1, Palette1);
    NE_MaterialSetPalette(Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDual(Draw3DScene, Draw3DScene2);
    }

    return 0;
}
