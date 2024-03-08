// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2024
//
// This file is part of Nitro Engine

// This demo shows how to use dual 3D DMA mode, and how a drop in framerate
// doesn't affect the video output. It remains stable even after droping several
// frames. This doesn't happen in regular dual 3D mode, where the same image
// would be shown on both screens.

#include <NEMain.h>

#include "teapot_bin.h"
#include "sphere_bin.h"

typedef struct {
    NE_Camera *Camera;
    NE_Model *Teapot, *Sphere;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_ClearColorSet(NE_Red, 31, 63);

    NE_CameraUse(Scene->Camera);
    NE_ModelDraw(Scene->Teapot);
}

void Draw3DScene2(void *arg)
{
    SceneData *Scene = arg;

    NE_ClearColorSet(NE_Green, 31, 63);

    NE_CameraUse(Scene->Camera);
    NE_ModelDraw(Scene->Sphere);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init dual 3D mode and console
    NE_InitDual3D_DMA();
    NE_InitConsole();

    // Allocate objects...
    Scene.Teapot = NE_ModelCreate(NE_Static);
    Scene.Sphere = NE_ModelCreate(NE_Static);
    Scene.Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 0, 0, -2,
                 0, 0, 0,
                 0, 1, 0);

    // Load models
    NE_ModelLoadStaticMesh(Scene.Teapot, teapot_bin);
    NE_ModelLoadStaticMesh(Scene.Sphere, sphere_bin);

    // Set light color and direction
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scenes
        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();

        printf("\x1b[0;0H"
               "START: Lock CPU for 20 frames\n"
               "Pad: Rotate.\n");

        // Lock CPU in an infinite loop to simulate a drop in framerate
        if (keys & KEY_START)
        {
            for (int i = 0; i < 20; i++)
                swiWaitForVBlank();
        }

        // Rotate model
        if (keys & KEY_UP)
        {
            NE_ModelRotate(Scene.Sphere, 0, 0, 2);
            NE_ModelRotate(Scene.Teapot, 0, 0, 2);
        }
        if (keys & KEY_DOWN)
        {
            NE_ModelRotate(Scene.Sphere, 0, 0, -2);
            NE_ModelRotate(Scene.Teapot, 0, 0, -2);
        }
        if (keys & KEY_RIGHT)
        {
            NE_ModelRotate(Scene.Sphere, 0, 2, 0);
            NE_ModelRotate(Scene.Teapot, 0, 2, 0);
        }
        if (keys & KEY_LEFT)
        {
            NE_ModelRotate(Scene.Sphere, 0, -2, 0);
            NE_ModelRotate(Scene.Teapot, 0, -2, 0);
        }
    }

    return 0;
}
