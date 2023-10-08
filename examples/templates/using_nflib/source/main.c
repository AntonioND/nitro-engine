// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
// SPDX-FileContributor: NightFox, 2009-2011
//
// This file is part of Nitro Engine

// NightFox’s Lib (NFlib) is a library that can supports the 2D hardware of the
// NDS. It's ideal to complement Nitro Engine, as it only supports the 3D
// hardware. The latest version can be found at the following link:
//
//     https://github.com/knightfox75/nds_nflib
//
// A few notes:
//
// - Remember to avoid using 3D features of NFlib.
//
// - Don't use the dual 3D mode in Nitro Engine.
//
// - You are free to use any features of NFlib in the secondary screen, but you
//   need to assign VRAM C and D to NFlib to get space for sprites and
//   backgrounds.
//
// - You could load 2D background and sprites in the same screen as the 3D
//   rendering, but you will have to give up VRAM banks A and B. If you also use
//   banks C and D for the secondary screen, it will leave you without any VRAM
//   for textures.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>
#include <nf_lib.h>

NE_Camera *Camera;
NE_Model *Model;
NE_Animation *Animation;
NE_Material *Material;

void Draw3DScene(void)
{
    NE_CameraUse(Camera);

    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
    NE_ModelDraw(Model);
}

void WaitLoop(void)
{
    while(1)
        swiWaitForVBlank();
}

void LoadAndSetupGraphics3D(void)
{
    // When using nflib for the 2D sub screen engine, banks C and H are used for
    // backgrounds and banks D and I are used for sprites. Nitro Engine only
    // uses bank E for palettes, so the only thing we need to do is to tell
    // Nitro Engine to only use banks A and B and leave C and D unused.

    NE_Init3D();
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);

    // Create objects

    Model = NE_ModelCreate(NE_Animated);
    Camera = NE_CameraCreate();
    Material = NE_MaterialCreate();
    Animation = NE_AnimationCreate();

    // Load assets from the filesystem

    if (NE_ModelLoadDSMFAT(Model, "robot.dsm") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load model...");
        WaitLoop();
    }

    if (NE_AnimationLoadFAT(Animation, "robot_wave.dsa") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load animation...");
        WaitLoop();
    }

    if (NE_MaterialTexLoadFAT(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                              "texture_tex.bin") == 0)
    {
        consoleDemoInit();
        printf("Couldn't load texture...");
        WaitLoop();
    }

    // Assign material to the model
    NE_ModelSetMaterial(Model, Material);

    // Assign animation to the model and start it
    NE_ModelSetAnimation(Model, Animation);
    NE_ModelAnimStart(Model, NE_ANIM_LOOP, floattof32(0.1));

    // Setup light
    NE_LightSet(0, NE_White, 0, -1, -1);

    // Setup background color
    NE_ClearColorSet(NE_Black, 31, 63);

    // Setup camera
    NE_CameraSet(Camera,
                 6, 3, -4,
                 0, 3, 0,
                 0, 1, 0);
}

void LoadAndSetupGraphics2D(void)
{
    // Initialize sub 2D engine
    NF_Set2D(1, 0);

    // Initialize sprites for sub screen (it uses VRAM D and I)
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(1);

    // Load assets from filesystem to RAM
    NF_LoadSpriteGfx("sprite/personaje", 1, 64, 64);
    NF_LoadSpritePal("sprite/personaje", 1);

    // Copy all frames to VRAM
    NF_VramSpriteGfx(1, 1, 0, false);
    NF_VramSpritePal(1, 1, 0);

    // Display sprite on the screen
    NF_CreateSprite(1, 0, 0, 0, 0, 0);

    // Initialize tiled backgrounds and text systems for the 2D sub engine (it
    // uses VRAM C and H)
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1);
    NF_InitTextSys(1);

    // Load assets from filesystem to RAM
    NF_LoadTiledBg("bg/bg3", "capa_3", 256, 256);
    NF_LoadTiledBg("bg/bg1", "capa_1", 1536, 256);
    NF_LoadTiledBg("bg/bg0", "capa_0", 2048, 256);
    NF_LoadTextFont("fnt/default", "normal", 256, 256, 0);

    // Create tiled backgrounds
    NF_CreateTiledBg(1, 3, "capa_3");
    NF_CreateTiledBg(1, 2, "capa_1");
    NF_CreateTiledBg(1, 1, "capa_0");

    // Create text layer
    NF_CreateTextLayer(1, 0, 0, "normal");
}

int main(void)
{
    // Initialize nitroFS before doing anything else
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    consoleDemoInit();
    printf("Starting nitroFS...\n");
    if (!nitroFSInit(NULL))
    {
        printf("Failed to start nitroFS\n");
        printf("Press START to exit\n");

        while (1)
        {
            swiWaitForVBlank();
            scanKeys();
            if (keysHeld() & KEY_START)
                return -1;
        }
    }

    swiWaitForVBlank();

    // Set the root folder to the nitroFS filesystem
    NF_SetRootFolder("NITROFS");

    // Setup interrupt handlers
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Load and setup graphics
    LoadAndSetupGraphics3D();
    LoadAndSetupGraphics2D();

    // Initialize variables to control the sprite
    int pj_x = 0;
    int pj_y = 127;
    unsigned int pj_frame = 0;
    unsigned int pj_anim_ticks = 0;
    int pj_speed = 1;

    // Initialize variables to control the backgrounds
    int bg_x[4] = {0, 0, 0};

    // Print instructions for the user
    NF_WriteText(1, 0, 1, 1, "PAD: Rotate model");
    NF_WriteText(1, 0, 1, 2, "L/R: Scroll background");
    NF_WriteText(1, 0, 1, 3, "START: Exit");
    NF_UpdateTextLayers();

    while (1)
    {
        NE_WaitForVBL(NE_UPDATE_ANIMATIONS);

        // At this point we are in the vertical blank period. This is where 2D
        // elements have to be updated to avoid flickering.

        // Update the scroll of the backgrounds
        for (int n = 0; n < 3; n ++)
                NF_ScrollBg(1, n + 1, bg_x[n], 0);

        // Copy shadow OAM copy to the OAM of the 2D sub engine
        oamUpdate(&oamSub);

        // Start processing a new frame after the 2D elements have been updated.

        scanKeys();
        uint32 keys = keysHeld();

        if (keys & KEY_START)
            break;

        if (keys & KEY_RIGHT)
            NE_ModelRotate(Model, 0, 2, 0);
        if (keys & KEY_LEFT)
            NE_ModelRotate(Model, 0, -2, 0);
        if (keys & KEY_UP)
            NE_ModelRotate(Model, 0, 0, 2);
        if (keys & KEY_DOWN)
            NE_ModelRotate(Model, 0, 0, -2);

        if (keys & KEY_L)
        {
            bg_x[0] -= 2;
            if (bg_x[0] < 0)
                bg_x[0] = 0;
        }

        if (keys & KEY_R)
        {
            bg_x[0] += 2;
            if (bg_x[0] > 1152)
                bg_x[0] = 1152;
        }

        // For the parallax effect, calculate the scroll of the second layer
        // based on the scroll of the first one.
        bg_x[1] = bg_x[0] / 2;

        // Move sprite
        pj_x += pj_speed;
        if ((pj_x < 0) || (pj_x > 191))
        {
            pj_speed = -pj_speed;
            if (pj_speed > 0)
                NF_HflipSprite(1, 0, false);
            else
                NF_HflipSprite(1, 0, true);
        }
        NF_MoveSprite(1, 0, pj_x, pj_y);

        // Animate sprite
        pj_anim_ticks++;
        if (pj_anim_ticks > 5)
        {
            pj_anim_ticks = 0;
            pj_frame++;
            if (pj_frame > 11)
                pj_frame = 0;
            NF_SpriteFrame(1, 0, pj_frame);
        }

        // Refresh shadow OAM copy
        NF_SpriteOamSet(1);

        // Draw 3D scene
        NE_Process(Draw3DScene);
    }

    return 0;
}
