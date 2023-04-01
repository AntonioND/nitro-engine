// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "robot_dsm_bin.h"
#include "robot_walk_dsa_bin.h"
#include "robot_wave_dsa_bin.h"
#include "texture_tex_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Animation *Animation[2];
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
    Animation[0] = NE_AnimationCreate();
    Animation[1] = NE_AnimationCreate();

    NE_AnimationLoad(Animation[0], robot_walk_dsa_bin);
    NE_AnimationLoad(Animation[1], robot_wave_dsa_bin);
    NE_ModelLoadDSM(Model, robot_dsm_bin);
    NE_ModelSetAnimation(Model, Animation[0]);
    NE_ModelSetAnimationSecondary(Model, Animation[1]);
    NE_ModelAnimStart(Model, NE_ANIM_LOOP, floattof32(0.1));
    NE_ModelAnimSecondaryStart(Model, NE_ANIM_LOOP, floattof32(0.1));

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

    int32_t blend = floattof32(0.5);

    printf("\x1b[0;0H"
           "L/R: Remove one animation\n"
           "A/B: Blend factor\n"
           "Pad: Rotate\n");

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

        if (keys & KEY_A)
        {
            blend += floattof32(0.02);
            if (blend > inttof32(1))
                blend = inttof32(1);
        }
        if (keys & KEY_B)
        {
            blend -= floattof32(0.02);
            if (blend < 0)
                blend = 0;
        }

        if (keys & KEY_L)
            NE_ModelAnimSecondaryClear(Model, true);
        if (keys & KEY_R)
            NE_ModelAnimSecondaryClear(Model, false);

        NE_ModelAnimSecondarySetFactor(Model, blend);

        printf("\x1b[20;0H"
               "CPU%%: %d  \n"
               "Blend factor:      %.3f  \n"
               "Frame (main):      %.3f  \n"
               "Frame (secondary): %.3f  ",
               NE_GetCPUPercent(), f32tofloat(blend),
               f32tofloat(NE_ModelAnimGetFrame(Model)),
               f32tofloat(NE_ModelAnimSecondaryGetFrame(Model)));

        NE_Process(Draw3DScene);
    }

    return 0;
}
