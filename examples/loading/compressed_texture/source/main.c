// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a5pal8_tex_bin.h"
#include "a5pal8_pal_bin.h"

// This has been taken from a libnds example:
//
// https://github.com/devkitPro/nds-examples/tree/6afa09b2054c9f47685514c32873b3905721c9ee/Graphics/3D/Paletted_Cube/data
//
// When img2ds supports compressed textures it will be replaced by a PNG file.
#include "texture10_COMP_pal_bin.h"
#include "texture10_COMP_tex_bin.h"
#include "texture10_COMP_texExt_bin.h"

NE_Material *Material1, *Material2;
NE_Palette *Palette1, *Palette2;

void Draw3DScene(void)
{
    NE_2DViewInit();

    NE_2DDrawTexturedQuad(0, 0,
                          64, 64,
                          0, Material2);

    NE_2DDrawTexturedQuad(64, 0,
                          64 + 128, 0 + 128,
                          0, Material1);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_Init3D();

    // Allocate objects
    Material1 = NE_MaterialCreate();
    Material2 = NE_MaterialCreate();
    Palette1 = NE_PaletteCreate();
    Palette2 = NE_PaletteCreate();

    NE_MaterialTex4x4Load(Material1, 128, 128, NE_TEXGEN_TEXCOORD,
                          (u8 *)texture10_COMP_tex_bin,
                          (u8 *)texture10_COMP_texExt_bin);
    NE_PaletteLoadSize(Palette1, (u16 *)texture10_COMP_pal_bin,
                       texture10_COMP_pal_bin_size, NE_TEX4X4);
    NE_MaterialSetPalette(Material1, Palette1);

    NE_MaterialTexLoad(Material2, NE_A5PAL8, 256, 256, NE_TEXGEN_TEXCOORD,
                       (u8 *)a5pal8_tex_bin);
    NE_PaletteLoadSize(Palette2, (u16 *)a5pal8_pal_bin, a5pal8_pal_bin_size,
                       NE_A5PAL8);
    NE_MaterialSetPalette(Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_Process(Draw3DScene);
    }

    return 0;
}
