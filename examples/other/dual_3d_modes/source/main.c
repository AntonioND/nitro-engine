// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2024
//
// This file is part of Nitro Engine

// The special screen effects of this is demo don't work on DesMuME. It works on
// melonDS and hardware.
//
// Dual 3D:
// - When the CPU hangs, the last drawn screen is shown in both screens.
// - The debug console works.
// - Special effects work normally.
//
// Dual 3D FB:
// - When the CPU hangs, the output is stable.
// - The debug console doesn't work.
// - Special effects don't work properly.
//
// Dual 3D DMA:
// - When the CPU hangs, the output is stable.
// - The debug console works.
// - Special effects work normally.
// - The CPU performance is lower. It requires more DMA and CPU copies.

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

void init_all(SceneData *Scene)
{
    // Allocate objects...
    Scene->Teapot = NE_ModelCreate(NE_Static);
    Scene->Sphere = NE_ModelCreate(NE_Static);
    Scene->Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene->Camera,
                 0, 0, -2,
                 0, 0, 0,
                 0, 1, 0);

    // Load models
    NE_ModelLoadStaticMesh(Scene->Teapot, teapot_bin);
    NE_ModelLoadStaticMesh(Scene->Sphere, sphere_bin);

    // Set light color and direction
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init dual 3D mode and console
    NE_InitDual3D();
    NE_InitConsole();

    init_all(&Scene);

    bool console = true;

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scenes
        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();
        uint32_t kdown = keysDown();

        if (console)
        {
            printf("\x1b[0;0H"
                   "START: Lock CPU until released\n"
                   "A: Sine effect.\n"
                   "B: Deactivate effect.\n"
                   "SELECT: Dual 3D DMA mode\n"
                   "X: Dual 3D FB mode (no console)\n"
                   "Y: Dual 3D mode\n"
                   "Pad: Rotate.\n");
        }

        // Lock CPU in an infinite loop to simulate a drop in framerate
        while (keys & KEY_START)
        {
            scanKeys();
            keys = keysHeld();
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

        // Deactivate effect
        if (kdown & KEY_B)
            NE_SpecialEffectSet(0);
        // Activate effect
        if (kdown & KEY_A)
            NE_SpecialEffectSet(NE_SINE);

        if (kdown & KEY_Y)
        {
            NE_SpecialEffectSet(0);
            NE_InitDual3D();
            NE_InitConsole();
            init_all(&Scene);
            console = true;
        }
        if (kdown & KEY_X)
        {
            NE_SpecialEffectSet(0);
            NE_InitDual3D_FB();
            init_all(&Scene);
            console = false;
        }
        if (kdown & KEY_SELECT)
        {
            NE_SpecialEffectSet(0);
            NE_InitDual3D_DMA();
            NE_InitConsole();
            init_all(&Scene);
            console = true;
        }
    }

    return 0;
}
