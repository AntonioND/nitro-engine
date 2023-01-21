// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

// This example shows that Nitro Engine fixes the strange texture mapping of the
// DS of any size of texture.
//
// For example, a naive call to glTexCoord2t16(inttot16(32), inttot16(64)) will
// cause strange issues next to the vertex. Nitro Engine helpers compensate for
// this effect.

#include <NEMain.h>

#include "big_tex_bin.h"
#include "medium_tex_bin.h"
#include "small_tex_bin.h"

NE_Material *Material_small, *Material_medium, *Material_big;

void Draw3DScene(void)
{
    NE_2DViewInit();

    // Texture scaled from 8x8 to 32x32
    NE_2DDrawTexturedQuad(150, 10,
                          150 + 32, 10 + 32,
                          0, Material_small);

    // Texture scaled from 16x16 to 64x64
    NE_2DDrawTexturedQuad(150, 100,
                          150 + 64, 100 + 64,
                          1, Material_medium);

    // Texture scaled from 64x64 to 128x128
    NE_2DDrawTexturedQuad(10, 10,
                          10 + 128, 10 + 128,
                          2, Material_big);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    Material_small = NE_MaterialCreate();
    Material_medium = NE_MaterialCreate();
    Material_big = NE_MaterialCreate();

    NE_MaterialTexLoad(Material_small, NE_A1RGB5, 8, 8, NE_TEXGEN_TEXCOORD,
                       (void *)small_tex_bin);
    NE_MaterialTexLoad(Material_medium, NE_A1RGB5, 16, 16, NE_TEXGEN_TEXCOORD,
                       (void *)medium_tex_bin);
    NE_MaterialTexLoad(Material_big, NE_A1RGB5, 64, 64, NE_TEXGEN_TEXCOORD,
                       (void *)big_tex_bin);

    NE_ClearColorSet(NE_DarkGray, 31, 63);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_Process(Draw3DScene);
    }

    return 0;
}
