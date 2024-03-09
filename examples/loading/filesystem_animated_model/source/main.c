// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <stdbool.h>
#include <stdio.h>

#include <filesystem.h>

#include <NEMain.h>

typedef struct {
    NE_Camera *Camera;
    NE_Model *Model;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_CameraUse(Scene->Camera);

    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
    NE_ModelDraw(Scene->Model);
}

void WaitLoop(void)
{
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            return;
    }
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

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\nPress START to exit");
        WaitLoop();
        return 0;
    }

    // Allocate space for objects...
    Scene.Model = NE_ModelCreate(NE_Animated);
    Scene.Camera = NE_CameraCreate();
    NE_Material *Material = NE_MaterialCreate();
    NE_Animation *Animation = NE_AnimationCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 6, 3, -4,
                 0, 3, 0,
                 0, 1, 0);

    if (NE_ModelLoadDSMFAT(Scene.Model, "robot.dsm") == 0)
    {
        printf("Couldn't load model...");
        WaitLoop();
        return 0;
    }

    if (NE_AnimationLoadFAT(Animation, "robot_wave.dsa") == 0)
    {
        printf("Couldn't load animation...");
        WaitLoop();
        return 0;
    }

    if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                              "texture.img.bin") == 0)
    {
        printf("Couldn't load texture...");
        WaitLoop();
        return 0;
    }

    // Assign material to the model
    NE_ModelSetMaterial(Scene.Model, Material);

    NE_ModelSetAnimation(Scene.Model, Animation);
    NE_ModelAnimStart(Scene.Model, NE_ANIM_LOOP, floattof32(0.1));

    NE_LightSet(0, NE_White, 0, -1, -1);

    NE_ClearColorSet(NE_Black, 31, 63);

    printf("\x1b[0;0HPad: Rotate\nSTART: Exit");

    while (1)
    {
        NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

        scanKeys();
        uint32_t keys = keysHeld();

        if (keys & KEY_START)
            break;

        if (keys & KEY_RIGHT)
            NE_ModelRotate(Scene.Model, 0, 2, 0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Scene.Model, 0, -2, 0);
        if (keys & KEY_UP)
            NE_ModelRotate(Scene.Model, 0, 0, 2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Scene.Model, 0, 0, -2);

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
