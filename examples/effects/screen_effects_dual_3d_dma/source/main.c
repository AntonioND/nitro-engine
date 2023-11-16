// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

// This demo doesn't work on DesMuME. It works on melonDS and hardware.

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

int main(void)
{
    // This is needed for special screen effects
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init dual 3D mode and console
    NE_InitDual3D_DMA();
    NE_InitConsole();

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

    // Other test configurations
    //NE_SpecialEffectNoiseConfig(31);
    //NE_SpecialEffectSineConfig(3, 8);

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scenes
        NE_ProcessDual(Draw3DScene, Draw3DScene2);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();
        uint32_t kdown = keysDown();

        printf("\x1b[0;0H"
               "START: Lock CPU until released\n"
               "Pad: Rotate.\nA: Sine effect.\nB: Noise effect.\n"
               "X: Deactivate effects.\nL/R: Pause/Unpause.");

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

        // Activate effects
        if (kdown & KEY_B)
            NE_SpecialEffectSet(NE_NOISE);
        if (kdown & KEY_A)
            NE_SpecialEffectSet(NE_SINE);
        // Deactivate effects
        if (kdown & KEY_X)
            NE_SpecialEffectSet(0);

        // Pause effects
        if (kdown & KEY_L)
            NE_SpecialEffectPause(true);
        if (kdown & KEY_R)
            NE_SpecialEffectPause(false);
    }

    return 0;
}
