// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

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

    NE_CameraUse(Scene->Camera);

    for (int i = 0; i < NUM_MODELS; i++)
        NE_ModelDraw(Scene->Model[i]);

    // Get some information after drawing but before returning from the
    // function
    printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
           NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate space for everything
    for (int i = 0; i < NUM_MODELS; i++)
        Scene.Model[i] = NE_ModelCreate(NE_Static);

    Scene.Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 -3.5, 1.5, 1.25,
                    0, 1.5, 1.25,
                    0, 1, 0);

    // Load model
    for (int i = 0; i < NUM_MODELS; i++)
        NE_ModelLoadStaticMesh(Scene.Model[i], sphere_bin);

    // Setup light
    NE_LightSet(0, NE_Yellow, 0, -0.5, -0.5);

    // Set start coordinates/rotation of models
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
