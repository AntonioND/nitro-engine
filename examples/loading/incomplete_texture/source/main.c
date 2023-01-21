// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a3pal32_pal_bin.h"
#include "a3pal32_tex_bin.h"
#include "pal4_pal_bin.h"
#include "pal4_tex_bin.h"

NE_Material *Material, *Material2;
NE_Palette *Palette, *Palette2;

// This is an example to show how Nitro Engine can load textures of any height.
// Internally, the NDS thinks that the texture is bigger, but Nitro Engine only
// uses the parts that the user has loaded.
//
// The width needs to be a power of two because:
//
// - Supporting them complicates the loading code a lot.
//
// - Compressed textures can't really be expanded because they are composed
//   by many 4x4 subimages.
//
// - They don't save space in VRAM.
//
// - They save space in the final ROM, but you can achieve the same effect
//   compressing them with LZSS compression, for example.

void Draw3DScene(void)
{
    NE_2DViewInit();

    NE_2DDrawTexturedQuad(40, 10,
                          40 + 32, 10 + 100,
                          0, Material);

    NE_2DDrawTexturedQuad(128, 10,
                          128 + 64, 10 + 100,
                          0, Material2);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_Init3D();

    // Allocate objects for a material
    Material = NE_MaterialCreate();
    Palette = NE_PaletteCreate();

    NE_MaterialTexLoad(Material,
                       NE_A3PAL32, // Texture type
                       64, 200,    // Width, height (in pixels)
                       NE_TEXGEN_TEXCOORD, (u8 *)a3pal32_tex_bin);
    NE_PaletteLoad(Palette, (u16 *)a3pal32_pal_bin, 32, NE_A3PAL32);
    NE_MaterialSetPalette(Material, Palette);

    // Allocate objects for another material
    Material2 = NE_MaterialCreate();
    Palette2 = NE_PaletteCreate();

    NE_MaterialTexLoad(Material2, NE_PAL4, 64, 100, NE_TEXGEN_TEXCOORD,
                       (u8 *)pal4_tex_bin);
    NE_PaletteLoad(Palette2, (u16 *)pal4_pal_bin, 4, NE_PAL4);
    NE_MaterialSetPalette(Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
