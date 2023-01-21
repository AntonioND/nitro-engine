// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "sphere_bin.h"

#define NUM_MODELS 16

NE_Camera *Camera;
NE_Model *Model[NUM_MODELS];

// Cloning models will avoid loading into memory the same mesh many times. This
// is really useful when you want to draw lots of the same model in different
// locations, with different animations, etc. You won't need to store multiple
// copies of the mesh in RAM.
//
// If you clone an animated model you will be able to set different animations
// for each model.

void Draw3DScene(void)
{
    // Setup camera and draw all objects.
    NE_CameraUse(Camera);

    for (int i = 0; i < NUM_MODELS; i++)
        NE_ModelDraw(Model[i]);

    // Get some information AFTER drawing but BEFORE returning from the
    // function.
    printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
           NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(void)
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
    for (int i = 0; i < NUM_MODELS; i++)
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
    for (int i = 1; i < NUM_MODELS; i++)
    {
        NE_ModelClone(Model[i],  // Destination
                      Model[0]); // Source model
    }

    // Set up light
    NE_LightSet(0, NE_Yellow, 0, -0.5, -0.5);

    // Enable shading
    NE_ShadingEnable(true);

    // Set start coordinates/rotation of the models
    for (int i = 0; i < NUM_MODELS; i++)
    {
        NE_ModelSetRot(Model[i], i, i * 30, i * 20);
        NE_ModelSetCoord(Model[i], 0, i % 4, i / 4);
    }

    while (1)
    {
        NE_WaitForVBL(0);

        // Rotate every model
        for (int i = 0; i < NUM_MODELS; i++)
            NE_ModelRotate(Model[i], -i, i % 5, 5 - i);

        // Draw scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
