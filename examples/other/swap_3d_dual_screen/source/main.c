// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a3pal32.h"
#include "a5pal8.h"

typedef struct {
    NE_Material *Material1, *Material2;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(0, 0,
                          256, 192,
                          0, Scene->Material1);
}

void Draw3DScene2(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 32,
                          64 + 128, 32 + 128,
                          0, Scene->Material2);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_InitDual3D();
    NE_InitConsole();

    // Allocate objects
    Scene.Material1 = NE_MaterialCreate();
    Scene.Material2 = NE_MaterialCreate();
    NE_Palette *Palette1 = NE_PaletteCreate();
    NE_Palette *Palette2 = NE_PaletteCreate();

    // Load part of the texture ignoring some of its height. You can't do
    // this with width because of how textures are laid out in VRAM.
    NE_MaterialTexLoad(Scene.Material1, NE_A3PAL32, 256, 192,
                       NE_TEXGEN_TEXCOORD, a3pal32Bitmap);

    // Load complete texture
    NE_MaterialTexLoad(Scene.Material2, NE_A5PAL8, 256, 256,
                       NE_TEXGEN_TEXCOORD, a5pal8Bitmap);

    NE_PaletteLoad(Palette1, a3pal32Pal, 32, NE_A3PAL32);
    NE_PaletteLoad(Palette2, a5pal8Pal, 32, NE_A5PAL8);

    NE_MaterialSetPalette(Scene.Material1, Palette1);
    NE_MaterialSetPalette(Scene.Material2, Palette2);

    printf("UP:   Set top screen as main\n"
           "DOWN: Set bottom screen as main\n"
           "A:    Swap screens");

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        uint16_t keys = keysDown();

        if (keys & KEY_A)
            NE_SwapScreens();
        if (keys & KEY_UP)
            NE_MainScreenSetOnTop();
        if (keys & KEY_DOWN)
            NE_MainScreenSetOnBottom();

        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);
    }

    return 0;
}
