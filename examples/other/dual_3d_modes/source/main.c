// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
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

NE_Camera *Camera;
NE_Model *Teapot, *Sphere;

void Draw3DScene(void)
{
    NE_ClearColorSet(NE_Red, 31, 63);

    NE_CameraUse(Camera);
    NE_ModelDraw(Teapot);
}

void Draw3DScene2(void)
{
    NE_ClearColorSet(NE_Green, 31, 63);

    NE_CameraUse(Camera);
    NE_ModelDraw(Sphere);
}

void init_all(void)
{
    // Allocate objects...
    Teapot = NE_ModelCreate(NE_Static);
    Sphere = NE_ModelCreate(NE_Static);
    Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Camera,
                 0, 0, -2,
                 0, 0, 0,
                 0, 1, 0);

    // Load models
    NE_ModelLoadStaticMesh(Teapot, (u32 *)teapot_bin);
    NE_ModelLoadStaticMesh(Sphere, (u32 *)sphere_bin);

    // Set light color and direction
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    // Enable shading
    NE_ShadingEnable(true);
}

int main(void)
{
    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init dual 3D mode and console
    NE_InitDual3D();
    NE_InitConsole();

    init_all();

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scenes
        NE_ProcessDual(Draw3DScene, Draw3DScene2);

        // Refresh keys
        scanKeys();
        uint32 keys = keysHeld();
        uint32 kdown = keysDown();

        printf("\x1b[0;0H"
               "START: Lock CPU until released\n"
               "A: Sine effect.\n"
               "B: Deactivate effect.\n"
               "SELECT: Dual 3D DMA mode\n"
               "X: Dual 3D FB mode (no console)\n"
               "Y: Dual 3D mode\n"
               "Pad: Rotate.\n");

        // Lock CPU in an infinite loop to simulate a drop in framerate
        while (keys & KEY_START)
        {
            scanKeys();
            keys = keysHeld();
        }

        // Rotate model
        if (keys & KEY_UP)
        {
            NE_ModelRotate(Sphere, 0, 0, 2);
            NE_ModelRotate(Teapot, 0, 0, 2);
        }
        if (keys & KEY_DOWN)
        {
            NE_ModelRotate(Sphere, 0, 0, -2);
            NE_ModelRotate(Teapot, 0, 0, -2);
        }
        if (keys & KEY_RIGHT)
        {
            NE_ModelRotate(Sphere, 0, 2, 0);
            NE_ModelRotate(Teapot, 0, 2, 0);
        }
        if (keys & KEY_LEFT)
        {
            NE_ModelRotate(Sphere, 0, -2, 0);
            NE_ModelRotate(Teapot, 0, -2, 0);
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
            init_all();
        }
        if (kdown & KEY_X)
        {
            NE_SpecialEffectSet(0);
            NE_InitDual3D_FB();
            init_all();
        }
        if (kdown & KEY_SELECT)
        {
            NE_SpecialEffectSet(0);
            NE_InitDual3D_DMA();
            NE_InitConsole();
            init_all();
        }
    }

    return 0;
}
