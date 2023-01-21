// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "robot_dsm_bin.h"
#include "robot_wave_dsa_bin.h"
#include "texture_tex_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Animation *Animation;
NE_Material *Texture;

void Draw3DScene(void)
{
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_BACK, 0);

    NE_CameraUse(Camera);

    NE_ModelDraw(Model);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_InitConsole();

    Camera = NE_CameraCreate();
    Model = NE_ModelCreate(NE_Animated);
    Animation = NE_AnimationCreate();

    NE_AnimationLoad(Animation, robot_wave_dsa_bin);
    NE_ModelLoadDSM(Model, robot_dsm_bin);
    NE_ModelSetAnimation(Model, Animation);
    NE_ModelAnimStart(Model, NE_ANIM_LOOP, floattof32(0.1));

    NE_CameraSet(Camera,
                 6, 3, -4,
                 0, 3, 0,
                 0, 1, 0);

    Texture = NE_MaterialCreate();
    NE_MaterialTexLoad(Texture, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                       (void *)texture_tex_bin);

    NE_ModelSetMaterial(Model, Texture);

    NE_LightSet(0, NE_White, -0.9, 0, 0);
    NE_ClearColorSet(NE_Black, 31, 63);

    while (1)
    {
        NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

        scanKeys();
        uint32 keys = keysHeld();

        if (keys & KEY_RIGHT)
            NE_ModelRotate(Model,0,2,0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Model,0,-2,0);
        if (keys & KEY_UP)
            NE_ModelRotate(Model,0,0,2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Model,0,0,-2);

        printf("\x1b[0;0H"
               "CPU%%: %d  \nFrame: %.3f  ",
               NE_GetCPUPercent(), f32tofloat(NE_ModelAnimGetFrame(Model)));

        NE_Process(Draw3DScene);
    }

    return 0;
}
