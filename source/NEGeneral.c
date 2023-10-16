// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022-2023 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <nds/arm9/background.h>
#include <nds/arm9/postest.h>
#include <nds/dma.h>

#include "NEMain.h"
#include "NEMath.h"

/// @file NEGeneral.c

const char NE_VersionString[] =
    "Nitro Engine - Version " NITRO_ENGINE_VERSION_STRING " - "
    "(C) 2008-2011, 2019, 2022-2023 Antonio Nino Diaz";

static bool NE_UsingConsole;
bool NE_TestTouch;
static int NE_screenratio;
static uint32_t NE_viewport;
static u8 NE_Screen; // 1 = main screen, 0 = sub screen

static NE_ExecutionModes ne_execution_mode = NE_ModeUninitialized;

NE_Input ne_input;

static SpriteEntry *NE_Sprites; // 2D sprites used for Dual 3D mode

static int ne_znear, ne_zfar;
static int fov;

static int ne_main_screen = 1; // 1 = top, 0 = bottom

static uint32_t ne_dma_enabled = 0;
static uint32_t ne_dma_src = 0;
static uint32_t ne_dma_dst = 0;
static uint32_t ne_dma_cr = 0;

NE_ExecutionModes NE_CurrentExecutionMode(void)
{
    return ne_execution_mode;
}

void NE_End(void)
{
    if (ne_execution_mode == NE_ModeUninitialized)
        return;

    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);

    switch (ne_execution_mode)
    {
        case NE_ModeSingle3D:
        {
            videoSetMode(0);

            if (GFX_CONTROL & GL_CLEAR_BMP)
                NE_ClearBMPEnable(false);

            if (NE_UsingConsole)
                vramSetBankF(VRAM_F_LCD);

            break;
        }

        case NE_ModeDual3D:
        {
            videoSetMode(0);
            videoSetModeSub(0);

            vramSetBankC(VRAM_C_LCD);
            vramSetBankD(VRAM_D_LCD);

            free(NE_Sprites);

            if (NE_UsingConsole)
                vramSetBankF(VRAM_F_LCD);

            break;
        }

        case NE_ModeDual3D_FB:
        {
            videoSetMode(0);
            videoSetModeSub(0);

            vramSetBankC(VRAM_C_LCD);
            vramSetBankD(VRAM_D_LCD);
            break;
        }

        case NE_ModeDual3D_DMA:
        {
            ne_dma_enabled = 0;

#ifdef NE_BLOCKSDS
            dmaStopSafe(2);
#else
            DMA_CR(2) = 0;
#endif

            videoSetMode(0);
            videoSetModeSub(0);

            vramSetBankC(VRAM_C_LCD);
            vramSetBankD(VRAM_D_LCD);

            // A pseudo framebuffer and the debug console go here
            vramSetBankI(VRAM_I_LCD);
            break;
        }

        default:
            break;
    }

    NE_UsingConsole = false;

    vramSetBankE(VRAM_E_LCD); // Palettes

    NE_GUISystemEnd();
    NE_SpriteSystemEnd();
    NE_PhysicsSystemEnd();
    NE_ModelSystemEnd();
    NE_AnimationSystemEnd();
    NE_TextResetSystem();
    NE_TextureSystemEnd();
    NE_CameraSystemEnd();
    NE_SpecialEffectSet(0);

    //Power off 3D hardware
    powerOff(POWER_3D_CORE | POWER_MATRIX);

    NE_DebugPrint("Nitro Engine disabled");

    ne_execution_mode = NE_ModeUninitialized;
}

void NE_Viewport(int x1, int y1, int x2, int y2)
{
    // Start calculating screen ratio in f32 format
    ne_div_start((x2 - x1 + 1) << 12, (y2 - y1 + 1));

    // Save viewport
    NE_viewport = x1 | (y1 << 8) | (x2 << 16) | (y2 << 24);
    GFX_VIEWPORT = NE_viewport;

    MATRIX_CONTROL = GL_PROJECTION; // New projection matix for this viewport
    MATRIX_IDENTITY = 0;

    int fovy = fov * DEGREES_IN_CIRCLE / 360;
    NE_screenratio = ne_div_result();
    gluPerspectivef32(fovy, NE_screenratio, ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;
}

void NE_MainScreenSetOnTop(void)
{
    ne_main_screen = 1;
}

void NE_MainScreenSetOnBottom(void)
{
    ne_main_screen = 0;
}

int NE_MainScreenIsOnTop(void)
{
    return ne_main_screen;
}

void NE_SwapScreens(void)
{
    ne_main_screen ^= 1;
}

void NE_SetFov(int fovValue)
{
    fov = fovValue;
}

static void ne_systems_end_all(void)
{
    NE_GUISystemEnd();
    NE_SpriteSystemEnd();
    NE_PhysicsSystemEnd();
    NE_ModelSystemEnd();
    NE_AnimationSystemEnd();
    NE_TextResetSystem();
    NE_TextureSystemEnd();
    NE_CameraSystemEnd();
    NE_SpecialEffectSet(0);
}

static int ne_systems_reset_all(NE_VRAMBankFlags vram_banks)
{
    // Default number of objects for everyting.
    if (NE_CameraSystemReset(0) != 0)
        goto cleanup;
    if (NE_PhysicsSystemReset(0) != 0)
        goto cleanup;
    if (NE_SpriteSystemReset(0) != 0)
        goto cleanup;
    if (NE_GUISystemReset(0) != 0)
        goto cleanup;
    if (NE_ModelSystemReset(0) != 0)
        goto cleanup;
    if (NE_AnimationSystemReset(0) != 0)
        goto cleanup;
    if (NE_TextureSystemReset(0, 0, vram_banks) != 0)
        goto cleanup;

    NE_TextPriorityReset();

    return 0;

cleanup:
    ne_systems_end_all();
    return -1;
}

static void ne_init_registers(void)
{
    // This function is usually called when the program boots. We don't know
    // which time in the frame exactly. In order to make the behaviour
    // consistent across emulators and hardware, it is required to synchronize
    // to the LCD refresh here.

    swiWaitForVBlank();

    // Power all 3D and 2D. Hide 3D screen during init
    powerOn(POWER_ALL);

    videoSetMode(0);

    vramSetBankE(VRAM_E_TEX_PALETTE);

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Clear the FIFO
    GFX_STATUS |= (1 << 29);

    GFX_FLUSH = 0;
    GFX_FLUSH = 0;

    NE_MainScreenSetOnTop();
    lcdMainOnTop();

    glResetMatrixStack();

    GFX_CONTROL = GL_TEXTURE_2D | GL_ANTIALIAS | GL_BLEND;

    GFX_ALPHA_TEST = 0;

    NE_ClearColorSet(NE_Black, 31, 63);
    NE_FogEnableBackground(false);

    GFX_CLEAR_DEPTH = GL_MAX_DEPTH;

    MATRIX_CONTROL = GL_TEXTURE;
    MATRIX_IDENTITY = 0;

    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_IDENTITY = 0;

    // Shininess table used for specular lighting
    NE_ShininessTableGenerate(NE_SHININESS_CUBIC);

    // setup default material properties
    NE_MaterialSetDefaultPropierties(RGB15(20, 20, 20), RGB15(16, 16, 16),
                                     RGB15(8, 8, 8), RGB15(5, 5, 5),
                                     false, true);

    // Turn off some things...
    for (int i = 0; i < 4; i++)
        NE_LightOff(i);

    GFX_COLOR = 0;
    GFX_POLY_FORMAT = 0;

    for (int i = 0; i < 8; i++)
        NE_OutliningSetColor(i, 0);

    ne_znear = floattof32(0.1);
    ne_zfar = floattof32(40.0);
    fov = 70;
    NE_Viewport(0, 0, 255, 191);

    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_IDENTITY = 0;

    // Make sure that this function is left always at the same time regardless
    // of whether it runs on hardware or emulators (which can be more or less
    // accurate). If not, the output of the screens in dual 3D mode may be
    // switched initially (and change once there is a framerate drop when
    // loading assets, for example).

    swiWaitForVBlank();

    // Ready!!

    videoSetMode(MODE_0_3D);
}

void NE_UpdateInput(void)
{
    ne_input.kdown = keysDown();
    ne_input.kheld = keysHeld();
    ne_input.kup = keysUp();

    if (ne_input.kheld & KEY_TOUCH)
        touchRead(&ne_input.touch);
}

int NE_Init3D(void)
{
    NE_End();

    if (ne_systems_reset_all(NE_VRAM_ABCD) != 0)
        return -1;

    NE_DisplayListSetDefaultFunction(NE_DL_DMA_GFX_FIFO);

    NE_UpdateInput();

    ne_init_registers();

    ne_execution_mode = NE_ModeSingle3D;

    NE_DebugPrint("Nitro Engine initialized in normal 3D mode");

    return 0;
}

static void ne_setup_sprites(void)
{
    // Reset sprites
    for (int i = 0; i < 128; i++)
        NE_Sprites[i].attribute[0] = ATTR0_DISABLED;

    int i = 0;
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            NE_Sprites[i].attribute[0] = ATTR0_BMP | ATTR0_SQUARE | (64 * y);
            NE_Sprites[i].attribute[1] = ATTR1_SIZE_64 | (64 * x);
            NE_Sprites[i].attribute[2] = ATTR2_ALPHA(1) | (8 * 32 * y)
                                       | (8 * x);
            i++;
        }
    }
}

int NE_InitDual3D(void)
{
    NE_End();

    NE_Sprites = calloc(128, sizeof(SpriteEntry));
    if (NE_Sprites == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -1;
    }

    if (ne_systems_reset_all(NE_VRAM_AB) != 0)
    {
        free(NE_Sprites);
        return -2;
    }

    ne_setup_sprites();

    NE_DisplayListSetDefaultFunction(NE_DL_DMA_GFX_FIFO);

    NE_UpdateInput();

    ne_init_registers();

    videoSetModeSub(0);

    REG_BG2CNT_SUB = BG_BMP16_256x256;
    REG_BG2PA_SUB = 1 << 8;
    REG_BG2PB_SUB = 0;
    REG_BG2PC_SUB = 0;
    REG_BG2PD_SUB = 1 << 8;
    REG_BG2X_SUB = 0;
    REG_BG2Y_SUB = 0;

    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);

    videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_SPR_ACTIVE |
                    DISPLAY_SPR_2D_BMP_256);

    ne_execution_mode = NE_ModeDual3D;

    NE_Screen = 0;

    NE_DebugPrint("Nitro Engine initialized in dual 3D mode");

    return 0;
}

int NE_InitDual3D_FB(void)
{
    NE_End();

    NE_Sprites = calloc(128, sizeof(SpriteEntry));
    if (NE_Sprites == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -1;
    }

    if (ne_systems_reset_all(NE_VRAM_AB) != 0)
        return -2;

    ne_setup_sprites();

    NE_DisplayListSetDefaultFunction(NE_DL_DMA_GFX_FIFO);

    NE_UpdateInput();

    ne_init_registers();

    videoSetModeSub(0);

    REG_BG2CNT = BG_BMP16_256x256;
    REG_BG2PA = 1 << 8;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 1 << 8;
    REG_BG2X = 0;
    REG_BG2Y = 0;

    REG_BG2CNT_SUB = BG_BMP16_256x256;
    REG_BG2PA_SUB = 1 << 8;
    REG_BG2PB_SUB = 0;
    REG_BG2PC_SUB = 0;
    REG_BG2PD_SUB = 1 << 8;
    REG_BG2X_SUB = 0;
    REG_BG2Y_SUB = 0;

    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);

    videoSetMode(0);
    videoSetModeSub(0);

    ne_execution_mode = NE_ModeDual3D_FB;

    NE_Screen = 0;

    NE_DebugPrint("Nitro Engine initialized in dual 3D FB mode");

    return 0;
}

int NE_InitDual3D_DMA(void)
{
    NE_End();

    if (ne_systems_reset_all(NE_VRAM_AB) != 0)
        return -1;

    NE_DisplayListSetDefaultFunction(NE_DL_CPU);

    NE_UpdateInput();

    ne_init_registers();

    videoSetMode(0);
    videoSetModeSub(0);

    ne_execution_mode = NE_ModeDual3D_DMA;

    NE_Screen = 0;

    NE_DebugPrint("Nitro Engine initialized in dual 3D DMA mode");

    return 0;
}

void NE_InitConsole(void)
{
    if (ne_execution_mode == NE_ModeUninitialized)
        return;

    switch (ne_execution_mode)
    {
        case NE_ModeSingle3D:
        case NE_ModeDual3D:
        {
            videoBgEnable(1);

            vramSetBankF(VRAM_F_MAIN_BG);

            BG_PALETTE[255] = 0xFFFF;

            // Use BG 1 for text, set to highest priority
            REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);

            // Set BG 0 (3D background) to be a lower priority than BG 1
            REG_BG0CNT = BG_PRIORITY(1);

            consoleInit(0, 1, BgType_Text4bpp, BgSize_T_256x256, 4, 0, true, true);

            break;
        }

        case NE_ModeDual3D_FB:
        {
            NE_Assert(false, "Debug console not supported in ModeDual3D_FB");
            break;
        }

        case NE_ModeDual3D_DMA:
        {
            BG_PALETTE[255] = 0xFFFF;

            vramSetBankF(VRAM_F_LCD);
            vramSetBankG(VRAM_G_LCD);
            vramSetBankH(VRAM_H_LCD);

            // Main engine - VRAM_C:
            //     BMP base 0 (256x192): 0x06008000 - 0x06020000 (96 KB)
            //     Tile base 0: 0x06000000 - 0x06001000 (4 KB)
            //     Map base 8: 0x06004000 - 0x06004800 (2 KB)
            vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
            consoleInit(0, 1, BgType_Text4bpp, BgSize_T_256x256, 8, 0, true, true);

            // We have just called consoleInit() to initialize the background
            // and to load the graphics. The next call to consoleInit() will
            // deinitialize the console from VRAM C, but it will leave the
            // graphics that have been loaded, which is what we wanted.
            //
            // VRAM I is always setup as sub background RAM, so we can rely on
            // the console always being mapped at the same address, while VRAM C
            // will alternate between LCD and main BG. We use libnds to write to
            // VRAM I, and copy the map to VRAM C whenever VRAM C is set as main
            // BG RAM.

            // Sub engine - VRAM_I:
            //     Available memory: 0x06208000 - 0x0620C000 (16 KB)
            //     Tile base 2: 0x06208000 - 0x06209000 (4 KB)
            //     Framebuffer (one line): 0x0620B000 - 0x0620B200 (512 B)
            //     Map base 23: 0x0620B800 - 0x0620C000 (2 KB)
            //
            // There is an overlap between the framebuffer and the tileset.
            // Tileset slots are 16 KB in size, so there is only one tileset
            // available in VRAM I. However, bitmap slots are also 16 KB in
            // size, so both the bitmap and tileset need to share the same
            // space.
            //
            // Luckily, the tileset of the console doesn't use the whole 16 KB.
            // The current console of libnds uses up to 128 characters. The
            // absolute maximum would be 256 characters, which requires (for a 4
            // bpp tileset) 8 KB of VRAM. This means that anything in the second
            // half of the bank (VRAM I is 16 KB in size) can be used for maps
            // or to store the pseudo framebuffer line.
            //
            // 8 KB is the same size as 16 lines in a 16-bit background. We can
            // setup our 16-bit bitmap as if it started at the same base as the
            // tileset, and we can store our line bitmap at some point between
            // 16 and 32 lines.
            //
            // Now, remember that the map uses the last 2 KB of the VRAM bank,
            // and that uses as much memory as 8 lines of a 16-bit bitmap. The
            // free lines of the bitmap are actually 16 to 24.
            //
            // Nitro Engine uses line 20, located at offset 0x3000 from the
            // start of the bank.
            vramSetBankI(VRAM_I_SUB_BG_0x06208000);
            consoleInit(0, 1, BgType_Text4bpp, BgSize_T_256x256, 23, 2, false, true);

            break;
        }

        default:
        {
            break;
        }
    }

    NE_UsingConsole = true;
}

void NE_SetConsoleColor(u32 color)
{
    BG_PALETTE[255] = color;
}

void NE_Process(NE_Voidfunc drawscene)
{
    NE_UpdateInput();

    if (ne_main_screen == 1)
        lcdMainOnTop();
    else
        lcdMainOnBottom();

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    GFX_VIEWPORT = NE_viewport;

    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_IDENTITY = 0;
    gluPerspectivef32(fov * DEGREES_IN_CIRCLE / 360, NE_screenratio,
                      ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_IDENTITY = 0;

    NE_AssertPointer(drawscene, "NULL function pointer");
    drawscene();

    GFX_FLUSH = GL_TRANS_MANUALSORT;
}

static void ne_process_dual_3d(NE_Voidfunc mainscreen, NE_Voidfunc subscreen)
{
    NE_UpdateInput();

    if (NE_Screen == ne_main_screen)
        lcdMainOnTop();
    else
        lcdMainOnBottom();

    if (NE_Screen == 1)
    {
        if (NE_UsingConsole)
        {
            REG_BG1CNT = BG_MAP_BASE(4) | BG_PRIORITY(0);
            REG_BG0CNT = BG_PRIORITY(1);
        }

        vramSetBankC(VRAM_C_SUB_BG);
        vramSetBankD(VRAM_D_LCD);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_D)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
                       | DCAP_ENABLE;
    }
    else
    {
        if (NE_UsingConsole)
        {
            REG_BG1CNT = BG_PRIORITY(1);
            REG_BG0CNT = BG_PRIORITY(0);
        }

        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_SUB_SPRITE);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_C)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
                       | DCAP_ENABLE;
    }

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    NE_Viewport(0, 0, 255, 191);

    MATRIX_IDENTITY = 0;

    if (NE_Screen == 1)
        mainscreen();
    else
        subscreen();

    GFX_FLUSH = GL_TRANS_MANUALSORT;

    dmaCopy(NE_Sprites, OAM_SUB, 128 * sizeof(SpriteEntry));

    NE_Screen ^= 1;
}

#define NE_DUAL_DMA_3D_LINES_OFFSET 20

static void ne_process_dual_3d_fb(NE_Voidfunc mainscreen, NE_Voidfunc subscreen)
{
    NE_UpdateInput();

    if (NE_Screen == ne_main_screen)
        lcdMainOnTop();
    else
        lcdMainOnBottom();

    if (NE_Screen == 1)
    {
        videoSetMode(MODE_FB3);
        videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

        vramSetBankC(VRAM_C_SUB_BG);
        vramSetBankD(VRAM_D_LCD);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_D)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_3DONLY)
                       | DCAP_ENABLE;
    }
    else
    {
        videoSetMode(MODE_FB2);
        videoSetModeSub(MODE_5_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_2D_BMP_256);

        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_SUB_SPRITE);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_C)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_3DONLY)
                       | DCAP_ENABLE;
    }

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    NE_Viewport(0, 0, 255, 191);

    MATRIX_IDENTITY = 0;

    if (NE_Screen == 1)
        mainscreen();
    else
        subscreen();

    GFX_FLUSH = GL_TRANS_MANUALSORT;

    dmaCopy(NE_Sprites, OAM_SUB, 128 * sizeof(SpriteEntry));

    NE_Screen ^= 1;
}

static void ne_do_dma(void)
{
    // BlocksDS has a safe way to start DMA copies that doesn't involve writing
    // to registers directly. It's safer to call the functions directly. The
    // libnds of devkitPro doesn't have this functionality.
#ifdef NE_BLOCKSDS
    dmaStopSafe(2);

    dmaSetParams(2, (const void *)ne_dma_src, (void *)ne_dma_dst, ne_dma_cr);
#else
    DMA_CR(2) = 0;

    DMA_SRC(2) = ne_dma_src;
    DMA_DEST(2) = ne_dma_dst;
    DMA_CR(2) = ne_dma_cr;
#endif
}

static void ne_process_dual_3d_dma(NE_Voidfunc mainscreen, NE_Voidfunc subscreen)
{
    NE_AssertPointer(mainscreen, "NULL function pointer (main screen)");
    NE_AssertPointer(subscreen, "NULL function pointer (sub screen)");

    if (NE_Screen == ne_main_screen)
        lcdMainOnBottom();
    else
        lcdMainOnTop();

    NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

    NE_Viewport(0, 0, 255, 191);

    MATRIX_IDENTITY = 0;

    if (NE_Screen == 1)
    {
        // DMA copies from VRAM C to VRAM I

        // Main engine: displays VRAM D as 16-bit BG
        // Sub engine: displays VRAM I as 16-bit BG

        videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);
        if (NE_UsingConsole)
            videoSetModeSub(MODE_5_2D | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE);
        else
            videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

        vramSetBankC(VRAM_C_LCD);
        vramSetBankD(VRAM_D_MAIN_BG_0x06000000);
        vramSetBankI(VRAM_I_SUB_BG_0x06208000);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_C)
                       | DCAP_OFFSET(1) // Write with an offset of 0x8000
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_3DONLY)
                       | DCAP_ENABLE;

        REG_BG2CNT = BG_BMP16_256x256 | BG_BMP_BASE(2) | BG_PRIORITY(2);
        REG_BG2PA = 1 << 8;
        REG_BG2PB = 0;
        REG_BG2PC = 0;
        REG_BG2PD = 1 << 8;
        REG_BG2X = 0;
        REG_BG2Y = -1 << 8;

        REG_BG2CNT_SUB = BG_BMP16_256x256 | BG_BMP_BASE(2) | BG_PRIORITY(2);
        REG_BG2PA_SUB = 1 << 8;
        REG_BG2PB_SUB = 0;
        REG_BG2PC_SUB = 0;
        REG_BG2PD_SUB = 0; // Scale first row to expand to the full screen
        REG_BG2X_SUB = 0;
        REG_BG2Y_SUB = NE_DUAL_DMA_3D_LINES_OFFSET << 8;

        ne_dma_enabled = 1;
        ne_dma_src = (uint32_t)VRAM_C + 0x8000;
        ne_dma_dst = ((uint32_t)BG_BMP_RAM_SUB(2))
                   + 256 * NE_DUAL_DMA_3D_LINES_OFFSET * 2;
        ne_dma_cr = DMA_COPY_WORDS | (256 * 2 / 4) |
                    DMA_START_HBL | DMA_REPEAT | DMA_SRC_INC | DMA_DST_RESET;

        ne_do_dma();

        mainscreen();
    }
    else
    {
        // DMA copies from VRAM D to VRAM I

        // Main engine: displays VRAM C as 16-bit sprites
        // Sub engine: displays VRAM I as 16-bit BG

        if (NE_UsingConsole)
            videoSetMode(MODE_5_2D | DISPLAY_BG1_ACTIVE | DISPLAY_BG2_ACTIVE);
        else
            videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE);
        videoSetModeSub(MODE_5_2D | DISPLAY_BG2_ACTIVE);

        vramSetBankC(VRAM_C_MAIN_BG_0x06000000);
        vramSetBankD(VRAM_D_LCD);
        vramSetBankI(VRAM_I_SUB_BG_0x06208000);

        REG_DISPCAPCNT = DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_BANK(DCAP_BANK_VRAM_D)
                       | DCAP_OFFSET(1) // Write with an offset of 0x8000
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_3DONLY)
                       | DCAP_ENABLE;

        REG_BG2CNT = BG_BMP16_256x256 | BG_BMP_BASE(2) | BG_PRIORITY(2);
        REG_BG2PA = 1 << 8;
        REG_BG2PB = 0;
        REG_BG2PC = 0;
        REG_BG2PD = 1 << 8;
        REG_BG2X = 0;
        REG_BG2Y = -1 << 8;

        REG_BG2CNT_SUB = BG_BMP16_256x256 | BG_BMP_BASE(2) | BG_PRIORITY(2);
        REG_BG2PA_SUB = 1 << 8;
        REG_BG2PB_SUB = 0;
        REG_BG2PC_SUB = 0;
        REG_BG2PD_SUB = 0; // Scale first row to expand to the full screen
        REG_BG2X_SUB = 0;
        REG_BG2Y_SUB = NE_DUAL_DMA_3D_LINES_OFFSET << 8;

        ne_dma_enabled = 1;
        ne_dma_src = (uint32_t)VRAM_D + 0x8000;
        ne_dma_dst = ((uint32_t)BG_BMP_RAM_SUB(2))
                   + 256 * NE_DUAL_DMA_3D_LINES_OFFSET * 2;
        ne_dma_cr = DMA_COPY_WORDS | (256 * 2 / 4) |
                    DMA_START_HBL | DMA_REPEAT | DMA_SRC_INC | DMA_DST_RESET;

        // Synchronize console of the main engine from the sub engine. Use a
        // channel other than 2, that one is used for HBL copies.
        if (NE_UsingConsole)
            dmaCopyWords(3, BG_MAP_RAM_SUB(23), BG_MAP_RAM(8), 32 * 32 * 2);

        ne_do_dma();

        subscreen();
    }

    GFX_FLUSH = GL_TRANS_MANUALSORT;

    NE_Screen ^= 1;

    NE_UpdateInput();
}

void NE_ProcessDual(NE_Voidfunc mainscreen, NE_Voidfunc subscreen)
{
    NE_AssertPointer(mainscreen, "NULL function pointer (main screen)");
    NE_AssertPointer(subscreen, "NULL function pointer (sub screen)");

    switch (ne_execution_mode)
    {
        case NE_ModeDual3D:
        {
            ne_process_dual_3d(mainscreen, subscreen);
            return;
        }

        case NE_ModeDual3D_FB:
        {
            ne_process_dual_3d_fb(mainscreen, subscreen);
            return;
        }

        case NE_ModeDual3D_DMA:
        {
            ne_process_dual_3d_dma(mainscreen, subscreen);
            return;
        }
        default:
        {
            return;
        }
    }
}

void NE_ClippingPlanesSetI(int znear, int zfar)
{
    NE_Assert(znear < zfar, "znear must be smaller than zfar");
    ne_znear = znear;
    ne_zfar = zfar;
}

void NE_AntialiasEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_ANTIALIAS;
    else
        GFX_CONTROL &= ~GL_ANTIALIAS;
}

int NE_GetPolygonCount(void)
{
    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    return GFX_POLYGON_RAM_USAGE;
}

int NE_GetVertexCount(void)
{
    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    return GFX_VERTEX_RAM_USAGE;
}

static int NE_Effect = NE_NONE;
static int NE_lastvbladd = 0;
static bool NE_effectpause;
#define NE_NOISEPAUSE_SIZE 512
static int *ne_noisepause;
static int ne_cpucount;
static int ne_noise_value = 0xF;
static int ne_sine_mult = 10, ne_sine_shift = 9;

void NE_VBLFunc(void)
{
    if (ne_execution_mode == NE_ModeUninitialized)
        return;

    if (ne_dma_enabled)
    {
        // The first line of the sub screen must be set to black during VBL
        // because the DMA transfer won't start until the first HBL, which
        // happens after the first line has been drawn.
        //
        // For consistency, in the main screen this is achieved by simply
        // scrolling the bitmap background by one pixel.
        dmaFillWords(0, BG_BMP_RAM_SUB(2) + 256 * NE_DUAL_DMA_3D_LINES_OFFSET,
                     256 * 2);

        ne_do_dma();
    }

    if (NE_Effect == NE_NOISE || NE_Effect == NE_SINE)
    {
        if (!NE_effectpause)
            NE_lastvbladd = (NE_lastvbladd + 1) & (NE_NOISEPAUSE_SIZE - 1);
    }
}

void NE_SpecialEffectPause(bool pause)
{
    if (NE_Effect == 0)
        return;

    if (pause)
    {
        ne_noisepause = malloc(sizeof(int) * NE_NOISEPAUSE_SIZE);
        if (ne_noisepause == NULL)
        {
            NE_DebugPrint("Not enough memory");
            return;
        }

        for (int i = 0; i < NE_NOISEPAUSE_SIZE; i++)
        {
            ne_noisepause[i] = (rand() & ne_noise_value)
                             - (ne_noise_value >> 1);
        }
    }
    else
    {
        if (ne_noisepause != NULL)
        {
            free(ne_noisepause);
            ne_noisepause = NULL;
        }
    }

    NE_effectpause = pause;
}

void NE_HBLFunc(void)
{
    if (ne_execution_mode == NE_ModeUninitialized)
        return;

    s16 angle;
    int val;

    // This counter is used to estimate CPU usage
    ne_cpucount++;

    // Fix a problem with the first line when using effects
    int vcount = REG_VCOUNT;
    if (vcount == 262)
        vcount = 0;

    switch (NE_Effect)
    {
        case NE_NOISE:
            if (NE_effectpause && ne_noisepause)
                val = ne_noisepause[vcount & (NE_NOISEPAUSE_SIZE - 1)];
            else
                val = (rand() & ne_noise_value) - (ne_noise_value >> 1);
            REG_BG0HOFS = val;
            break;

        case NE_SINE:
            angle = (vcount + NE_lastvbladd) * ne_sine_mult;
            REG_BG0HOFS = sinLerp(angle << 6) >> ne_sine_shift;
            break;

        default:
            break;
    }
}

void NE_SpecialEffectNoiseConfig(int value)
{
    ne_noise_value = value;
}

void NE_SpecialEffectSineConfig(int mult, int shift)
{
    ne_sine_mult = mult;
    ne_sine_shift = shift;
}

void NE_SpecialEffectSet(NE_SpecialEffects effect)
{
    NE_Effect = effect;

    if (effect == NE_NONE)
        REG_BG0HOFS = 0;
}

static int NE_CPUPercent;

void NE_WaitForVBL(NE_UpdateFlags flags)
{
    if (flags & NE_UPDATE_GUI)
        NE_GUIUpdate();
    if (flags & NE_UPDATE_ANIMATIONS)
        NE_ModelAnimateAll();
    if (flags & NE_UPDATE_PHYSICS)
        NE_PhysicsUpdateAll();

    NE_CPUPercent = div32(ne_cpucount * 100, 263);
    if (flags & NE_CAN_SKIP_VBL)
    {
        if (NE_CPUPercent > 100)
        {
            ne_cpucount = 0;
            return;

            // REG_DISPSTAT & DISP_IN_VBLANK
        }
    }

    swiWaitForVBlank();
    ne_cpucount = 0;
}

int NE_GetCPUPercent(void)
{
    return NE_CPUPercent;
}

bool NE_GPUIsRendering(void)
{
    if (REG_VCOUNT > 190 && REG_VCOUNT < 214)
        return false;

    return true;
}

#ifdef NE_DEBUG
static void (*ne_userdebugfn)(const char *) = NULL;

void __ne_debugoutputtoconsole(const char *text)
{
    printf(text);
}

void __NE_debugprint(const char *text)
{
    if (ne_execution_mode == NE_ModeUninitialized)
        return;

    if (ne_userdebugfn)
        ne_userdebugfn(text);
}

void NE_DebugSetHandler(void (*fn)(const char *))
{
    ne_userdebugfn = fn;
}

void NE_DebugSetHandlerConsole(void)
{
    NE_InitConsole();
    ne_userdebugfn = __ne_debugoutputtoconsole;
}
#endif

static int ne_vertexcount;

void NE_TouchTestStart(void)
{
    // Hide what we are going to draw
    GFX_VIEWPORT = 255 | (255 << 8) | (255 << 16) | (255 << 24);

    // Save current state
    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_PUSH = 0;
    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_PUSH = 0;

    // Setup temporary render environment
    MATRIX_IDENTITY = 0;

    int temp[4] = {
        NE_viewport & 0xFF,
        (NE_viewport >> 8) & 0xFF,
        (NE_viewport >> 16) & 0xFF,
        (NE_viewport >> 24) & 0xFF
    };

    // Render only what is below the cursor
    gluPickMatrix(ne_input.touch.px, 191 - ne_input.touch.py, 3, 3, temp);
    gluPerspectivef32(fov * DEGREES_IN_CIRCLE / 360, NE_screenratio,
                      ne_znear, ne_zfar);

    MATRIX_CONTROL = GL_MODELVIEW;

    NE_Assert(!NE_TestTouch, "Test already active");

    NE_TestTouch = true;
}

void NE_TouchTestObject(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    // Wait for the position test to finish
    while (PosTestBusy());

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Save the vertex ram count
    ne_vertexcount = NE_GetVertexCount();
}

int NE_TouchTestResult(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    // Wait for geometry engine operations to end
    while (GFX_STATUS & BIT(27));

    // Wait for the position test to finish
    while (PosTestBusy());

    // If a polygon was drawn
    if (NE_GetVertexCount() > ne_vertexcount)
        return PosTestWresult();

    return -1;
}

void NE_TouchTestEnd(void)
{
    NE_Assert(NE_TestTouch, "No active test");

    NE_TestTouch = false;

    // Reset the viewport
    GFX_VIEWPORT = NE_viewport;

    // Restore previous state
    MATRIX_CONTROL = GL_PROJECTION;
    MATRIX_POP = 1;
    MATRIX_CONTROL = GL_MODELVIEW;
    MATRIX_POP = 1;
}
