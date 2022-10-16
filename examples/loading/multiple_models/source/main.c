// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "model_bin.h"

NE_Camera *Camera;
NE_Model *Model[16];

void Draw3DScene(void)
{
    NE_CameraUse(Camera);

    for (int i = 0; i < 16; i++)
        NE_ModelDraw(Model[i]);

    // Get some information after drawing but before returning from the
    // function
    printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
           NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate space for everything
    for (int i = 0; i < 16; i++)
        Model[i] = NE_ModelCreate(NE_Static);

    Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Camera,
                 -3.5, 1.5, 1.25,
                    0, 1.5, 1.25,
                    0, 1, 0);

    // Load model
    for (int i = 0; i < 16; i++)
        NE_ModelLoadStaticMesh(Model[i], (u32 *)model_bin);

    // Setup light
    NE_LightSet(0, NE_Yellow, 0, -0.5, -0.5);

    // Enable shading
    NE_ShadingEnable(true);

    // Set start coordinates/rotation for models using random formules...
    for (int i = 0; i < 16; i++)
    {
        NE_ModelSetRot(Model[i], i, i * 30, i * 20);
        NE_ModelSetCoord(Model[i], 0, i % 4, i / 4);
    }

    while (1)
    {
        // Rotate every model using random formules
        for (int i = 0; i < 16; i++)
            NE_ModelRotate(Model[i], -i, i % 5, 5 - i);

        // Draw scene
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
