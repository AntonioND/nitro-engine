// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

// This is an example of passing arguments to the functions passed to
// NE_ProcessDualArg(). This example only passes a color to be used as clear
// color, but the code could pass a pointer to a struct with the information to
// render in that screen, or anything that the developer wants.

#include <NEMain.h>

#include "teapot_bin.h"
#include "sphere_bin.h"

NE_Camera *Camera;
NE_Model *Teapot, *Sphere;

void Draw3DScene(void *arg)
{
    NE_ClearColorSet(*(uint16_t *)arg, 31, 63);

    NE_CameraUse(Camera);
    NE_ModelDraw(Teapot);
}

void Draw3DScene2(void *arg)
{
    NE_ClearColorSet(*(uint16_t *)arg, 31, 63);

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
    NE_ModelLoadStaticMesh(Teapot, teapot_bin);
    NE_ModelLoadStaticMesh(Sphere, sphere_bin);

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

    uint16_t red = NE_Red;
    uint16_t green = NE_Green;

    while (1)
    {
        NE_WaitForVBL(0);

        // Draw 3D scenes
        NE_ProcessDualArg(Draw3DScene, Draw3DScene2,
                          (void *)&red, (void *)&green);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();
        uint32_t kdown = keysDown();

        printf("\x1b[0;0H"
               "A: Dual 3D DMA mode\n"
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

        if (kdown & KEY_Y)
        {
            NE_InitDual3D();
            NE_InitConsole();
            init_all();
        }
        if (kdown & KEY_X)
        {
            NE_InitDual3D_FB();
            init_all();
        }
        if (kdown & KEY_A)
        {
            NE_InitDual3D_DMA();
            NE_InitConsole();
            init_all();
        }
    }

    return 0;
}
