// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "background_tex_bin.h"
#include "depth_tex_bin.h"
#include "model_bin.h"

NE_Camera *Camera;
NE_Model *Model;

void Draw3DScene(void)
{
    NE_CameraUse(Camera);

    NE_PolyFormat(31, 0, NE_LIGHT_01, NE_CULL_BACK, 0);
    NE_ModelDraw(Model);

    NE_2DViewInit();
    NE_2DDrawQuad(0, 0,
                  100, 100,
                  0, NE_White);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    // The clear bitmap is placed in VRAM_C and VRAM_D, so it is needed to
    // preserve them.
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);

    // Copy data into VRAM
    vramSetBankC(VRAM_C_LCD);
    dmaCopy(background_tex_bin, VRAM_C, background_tex_bin_size);
    vramSetBankD(VRAM_D_LCD);
    dmaCopy(depth_tex_bin, VRAM_D, depth_tex_bin_size);

    NE_ClearBMPEnable(true);

    Camera = NE_CameraCreate();
    NE_CameraSet(Camera,
                 1, 1, 1,
                 0, 0, 0,
                 0, 1, 0);

    Model = NE_ModelCreate(NE_Static);
    NE_ModelLoadStaticMesh(Model, (u32 *)model_bin);

    NE_LightSet(0, NE_Yellow, -1, -1, 0);
    NE_LightSet(1, NE_Red, -1, 1, 0);

    NE_ClearColorSet(0,       // Color not used when clear BMP
                     31, 63); // ID and alpha are used

    u8 scrollx = 0, scrolly = 0;
    while (1)
    {
        scanKeys();
        uint32 keys = keysHeld();

        NE_ModelRotate(Model, 0, 2, 1);

        if (keys & KEY_A)
            NE_ClearBMPEnable(true);
        if (keys & KEY_B)
            NE_ClearBMPEnable(false);

        NE_ClearBMPScroll(scrollx,scrolly);

        if (keys & KEY_UP)
            scrolly --;
        if (keys & KEY_DOWN)
            scrolly ++;
        if (keys & KEY_RIGHT)
            scrollx ++;
        if (keys & KEY_LEFT)
            scrollx --;

        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
