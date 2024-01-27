// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

// The depth bitmap is a bit complicated... It is quite difficult to regulate
// the depth so that the image and the result make sense.
//
// With img2ds, the color you have to use to set the depth is blue. The clear
// bitmap is in its farthest point if blue is 255.
//
// If you want to hide everything, including the 2D projection, use red (any
// value overrides the value in the blue channel).
//
// Check assets.sh to see how to convert the images.

#include <NEMain.h>

#include "background.h"
#include "depth_tex_bin.h"
#include "cube_bin.h"

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
    dmaCopy(backgroundBitmap, VRAM_C, backgroundBitmapLen * 2);
    vramSetBankD(VRAM_D_LCD);
    dmaCopy(depth_tex_bin, VRAM_D, depth_tex_bin_size);

    NE_ClearBMPEnable(true);

    Camera = NE_CameraCreate();
    NE_CameraSet(Camera,
                 1, 1, 1,
                 0, 0, 0,
                 0, 1, 0);

    Model = NE_ModelCreate(NE_Static);
    NE_ModelLoadStaticMesh(Model, cube_bin);

    NE_LightSet(0, NE_Yellow, -1, -1, 0);
    NE_LightSet(1, NE_Red, -1, 1, 0);

    NE_ClearColorSet(0,       // Color not used when clear BMP
                     31, 63); // ID and alpha are used

    int scrollx = 0, scrolly = 0;

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        uint32_t keys = keysHeld();

        NE_ModelRotate(Model, 0, 2, 1);

        if (keys & KEY_A)
            NE_ClearBMPEnable(true);
        if (keys & KEY_B)
            NE_ClearBMPEnable(false);

        NE_ClearBMPScroll(scrollx, scrolly);

        if (keys & KEY_UP)
            scrolly--;
        if (keys & KEY_DOWN)
            scrolly++;
        if (keys & KEY_RIGHT)
            scrollx++;
        if (keys & KEY_LEFT)
            scrollx--;

        NE_Process(Draw3DScene);
    }

    return 0;
}
