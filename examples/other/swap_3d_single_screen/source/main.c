// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a5pal8.h"
#include "a5pal8.h"

NE_Material *Material;
NE_Palette *Palette;

void Draw3DScene(void)
{
    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 32,
                          64 + 128, 32 + 128,
                          0, Material);
}

int main(void)
{
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
    Material = NE_MaterialCreate();
    Palette = NE_PaletteCreate();

    NE_MaterialTexLoad(Material, NE_A5PAL8, 256, 256, NE_TEXGEN_TEXCOORD,
                       a5pal8Bitmap);

    NE_PaletteLoad(Palette, a5pal8Pal, 32, NE_A5PAL8);

    NE_MaterialSetPalette(Material, Palette);

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

        NE_Process(Draw3DScene);
    }

    return 0;
}
