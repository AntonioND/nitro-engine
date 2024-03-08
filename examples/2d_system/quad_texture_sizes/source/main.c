// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

// This example shows that Nitro Engine fixes the strange texture mapping of the
// DS of any size of texture.
//
// For example, a naive call to glTexCoord2t16(inttot16(32), inttot16(64)) will
// cause strange issues next to the vertex. Nitro Engine helpers compensate for
// this effect.

#include <NEMain.h>

#include "s8.h"
#include "s16.h"
#include "s64.h"
#include "s256.h"

typedef struct {
    NE_Material *Material_s8, *Material_s16, *Material_s64, *Material_s256;
    NE_Palette *Palette_s8, *Palette_s16, *Palette_s64, *Palette_s256;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_ClearColorSet(NE_DarkGray, 31, 63);

    NE_2DViewInit();

    // Texture scaled from 64x64 to 128x128
    NE_2DDrawTexturedQuad(10, 10,
                          10 + 128, 10 + 128,
                          2, Scene->Material_s64);

    // Texture scaled from 8x8 to 32x32
    NE_2DDrawTexturedQuad(150, 10,
                          150 + 32, 10 + 32,
                          0, Scene->Material_s8);

    // Texture not scaled
    NE_2DDrawTexturedQuad(160, 50,
                          160 + 64, 50 + 64,
                          10, Scene->Material_s64);


    // Texture scaled from 16x16 to 64x64
    NE_2DDrawTexturedQuad(150, 120,
                          150 + 64, 120 + 64,
                          1, Scene->Material_s16);
}

void Draw3DScene2(void *arg)
{
    SceneData *Scene = arg;

    NE_ClearColorSet(NE_Magenta, 31, 63);

    NE_2DViewInit();

    // Texture not scaled
    NE_2DDrawTexturedQuad(-10, -70,
                          -10 + 256, -70 + 256,
                          3, Scene->Material_s256);
}

int main(void)
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_InitDual3D();

    Scene.Material_s8 = NE_MaterialCreate();
    Scene.Material_s16 = NE_MaterialCreate();
    Scene.Material_s64 = NE_MaterialCreate();
    Scene.Material_s256 = NE_MaterialCreate();

    Scene.Palette_s8 = NE_PaletteCreate();
    Scene.Palette_s16 = NE_PaletteCreate();
    Scene.Palette_s64 = NE_PaletteCreate();
    Scene.Palette_s256 = NE_PaletteCreate();

    NE_MaterialTexLoad(Scene.Material_s8, NE_PAL16, 8, 8,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                       s8Bitmap);
    NE_MaterialTexLoad(Scene.Material_s16, NE_PAL16, 16, 16,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                       s16Bitmap);
    NE_MaterialTexLoad(Scene.Material_s64, NE_PAL16, 64, 64,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                       s64Bitmap);
    NE_MaterialTexLoad(Scene.Material_s256, NE_PAL16, 256, 256,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_WRAP_S | NE_TEXTURE_WRAP_T,
                       s256Bitmap);

    NE_PaletteLoad(Scene.Palette_s8, s8Pal, 16, NE_PAL16);
    NE_PaletteLoad(Scene.Palette_s16, s16Pal, 16, NE_PAL16);
    NE_PaletteLoad(Scene.Palette_s64, s64Pal, 16, NE_PAL16);
    NE_PaletteLoad(Scene.Palette_s256, s256Pal, 16, NE_PAL16);

    NE_MaterialSetPalette(Scene.Material_s8, Scene.Palette_s8);
    NE_MaterialSetPalette(Scene.Material_s16, Scene.Palette_s16);
    NE_MaterialSetPalette(Scene.Material_s64, Scene.Palette_s64);
    NE_MaterialSetPalette(Scene.Material_s256, Scene.Palette_s256);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);
    }

    return 0;
}
