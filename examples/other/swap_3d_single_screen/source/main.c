// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a5pal8.h"
#include "a5pal8.h"

typedef struct {
    NE_Material *Material;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 32,
                          64 + 128, 32 + 128,
                          0, Scene->Material);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine in normal 3D mode
    NE_Init3D();

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate objects
    Scene.Material = NE_MaterialCreate();
    NE_Palette *Palette = NE_PaletteCreate();

    NE_MaterialTexLoad(Scene.Material, NE_A5PAL8, 256, 256, NE_TEXGEN_TEXCOORD,
                       a5pal8Bitmap);

    NE_PaletteLoad(Palette, a5pal8Pal, 32, NE_A5PAL8);

    NE_MaterialSetPalette(Scene.Material, Palette);

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

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
