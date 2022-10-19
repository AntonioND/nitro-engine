// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "sphere_bin.h"

// Pointers to objects...
NE_Camera *Camera;
NE_Model *Model[16];

// Cloning models will avoid loading into memory the same data many times. This
// is really useful when you want to draw lots of animated models, they will use
// the memory of one!
//
// NOTE: Models use a few bytes for holding some information.
//
// NOTE 2: Be careful when using NE_ModelClone(). If you delete source model and
// try to draw a destination model game will eventually crash!
//
// NOTE 3: If you clone an animated model you will be able to set different
// animation patterns for each model.

void Draw3DScene(void)
{
    // Setup camera and draw all objects.
    NE_CameraUse(Camera);

    int i;
    for (i = 0; i < 16; i++)
        NE_ModelDraw(Model[i]);

    // Get some information AFTER drawing but BEFORE returning from the
    // function.
    printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
           NE_GetPolygonCount(), NE_GetVertexCount());
}

int main()
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine.
    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate space for everything.
    for (int i = 0; i < 16; i++)
        Model[i] = NE_ModelCreate(NE_Static);

    Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Camera,
                 -3.5, 1.5, 1.25,
                    0, 1.5, 1.25,
                    0, 1, 0);

    // Load model once
    NE_ModelLoadStaticMesh(Model[0], (u32 *)sphere_bin);

    // Clone model to the test of the objects
    for (int i = 1; i < 16; i++)
    {
        NE_ModelClone(Model[i],  // Destination
                      Model[0]); // Source model
    }

    // Set up light
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
        // Rotate every model using random formules :P
        for (int i = 0; i < 16; i++)
            NE_ModelRotate(Model[i], -i, i % 5, 5 - i);

        // Draw scene
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
