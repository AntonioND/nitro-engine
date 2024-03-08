// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "teapot_bin.h"

typedef struct {
    NE_Camera *Camera;
    NE_Model *Model;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    // Set rear plane color
    NE_ClearColorSet(NE_Red, 31, 63);

    NE_CameraUse(Scene->Camera);
    NE_ModelDraw(Scene->Model);
}

void Draw3DScene2(void *arg)
{
    SceneData *Scene = arg;

    // Set rear plane color
    NE_ClearColorSet(NE_Green, 31, 63);

    NE_CameraUse(Scene->Camera);
    NE_ModelDraw(Scene->Model);
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

    // Allocate objects...
    Scene.Model = NE_ModelCreate(NE_Static);
    Scene.Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 0, 0, -3,
                 0, 0, 0,
                 0, 1, 0);

    // Load model
    NE_ModelLoadStaticMesh(Scene.Model, teapot_bin);

    // Set light color and direction
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    // Other test configurations
    //NE_SpecialEffectNoiseConfig(31);
    //NE_SpecialEffectSineConfig(3, 8);

    while (1)
    {
        NE_WaitForVBL(0);

        // Refresh keys
        scanKeys();
        uint32_t keys = keysHeld();
        uint32_t kdown = keysDown();

        printf("\x1b[0;0H"
               "Pad: Rotate.\nA: Sine effect.\nB: Noise effect.\n"
               "X: Deactivate effects.\nL/R: Pause/Unpause.");

        // Rotate model
        if (keys & KEY_UP)
            NE_ModelRotate(Scene.Model, 0, 0, 2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Scene.Model, 0, 0, -2);
        if (keys & KEY_RIGHT)
            NE_ModelRotate(Scene.Model, 0, 2, 0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Scene.Model, 0, -2, 0);

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

        // Draw 3D scenes
        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);
    }

    return 0;
}
