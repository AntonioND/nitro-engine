// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

// Cloning models will avoid loading into memory the same mesh many times. This
// is really useful when you want to draw lots of the same model in different
// locations, with different animations, etc. You won't need to store multiple
// copies of the mesh in RAM.
//
// If you clone an animated model you will be able to set different animations
// for each model.

#include <NEMain.h>

#include "sphere_bin.h"

#define NUM_MODELS 16

typedef struct {
    NE_Camera *Camera;
    NE_Model *Model[NUM_MODELS];
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    // Setup camera and draw all objects.
    NE_CameraUse(Scene->Camera);

    for (int i = 0; i < NUM_MODELS; i++)
        NE_ModelDraw(Scene->Model[i]);

    // Get some information AFTER drawing but BEFORE returning from the
    // function.
    printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
           NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

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
        Scene.Model[i] = NE_ModelCreate(NE_Static);

    Scene.Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 -3.5, 1.5, 1.25,
                    0, 1.5, 1.25,
                    0, 1, 0);

    // Load model once
    NE_ModelLoadStaticMesh(Scene.Model[0], sphere_bin);

    // Clone model to the test of the objects
    for (int i = 1; i < NUM_MODELS; i++)
    {
        NE_ModelClone(Scene.Model[i],  // Destination
                      Scene.Model[0]); // Source model
    }

    // Set up light
    NE_LightSet(0, NE_Yellow, 0, -0.5, -0.5);

    // Set start coordinates/rotation of the models
    for (int i = 0; i < NUM_MODELS; i++)
    {
        NE_ModelSetRot(Scene.Model[i], i, i * 30, i * 20);
        NE_ModelSetCoord(Scene.Model[i], 0, i % 4, i / 4);
    }

    while (1)
    {
        NE_WaitForVBL(0);

        // Rotate every model
        for (int i = 0; i < NUM_MODELS; i++)
            NE_ModelRotate(Scene.Model[i], -i, i % 5, 5 - i);

        // Draw scene
        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
