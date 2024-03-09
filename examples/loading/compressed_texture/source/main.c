// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "grill_idx_bin.h"
#include "grill_pal_bin.h"
#include "grill_tex_bin.h"
#include "landscape_idx_bin.h"
#include "landscape_pal_bin.h"
#include "landscape_tex_bin.h"

typedef struct {
    NE_Material *Material1;
} SceneData1;

typedef struct {
    NE_Material *Material2;
} SceneData2;

void Draw3DScene1(void *arg)
{
    SceneData1 *Scene = arg;

    NE_ClearColorSet(RGB15(5, 5, 10), 31, 63);

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(32, 32,
                          32 + 128, 32 + 128,
                          0, Scene->Material1);
}

void Draw3DScene2(void *arg)
{
    SceneData2 *Scene = arg;

    NE_ClearColorSet(RGB15(10, 5, 0), 31, 63);

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 0,
                          64 + 128, 0 + 128,
                          0, Scene->Material2);
}

int main(int argc, char *argv[])
{
    SceneData1 Scene1 = { 0 };
    SceneData2 Scene2 = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_InitDual3D();

    // Allocate objects
    Scene1.Material1 = NE_MaterialCreate();
    Scene2.Material2 = NE_MaterialCreate();

    NE_Palette *Palette1 = NE_PaletteCreate();
    NE_Palette *Palette2 = NE_PaletteCreate();

    NE_MaterialTex4x4Load(Scene1.Material1, 128, 128, NE_TEXGEN_TEXCOORD,
                          grill_tex_bin, grill_idx_bin);
    NE_PaletteLoadSize(Palette1, grill_pal_bin, grill_pal_bin_size,
                       NE_TEX4X4);
    NE_MaterialSetPalette(Scene1.Material1, Palette1);

    NE_MaterialTex4x4Load(Scene2.Material2, 128, 128, NE_TEXGEN_TEXCOORD,
                          landscape_tex_bin, landscape_idx_bin);
    NE_PaletteLoadSize(Palette2, landscape_pal_bin,
                       landscape_pal_bin_size, NE_TEX4X4);
    NE_MaterialSetPalette(Scene2.Material2, Palette2);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDualArg(Draw3DScene1, Draw3DScene2, &Scene1, &Scene2);
    }

    return 0;
}
