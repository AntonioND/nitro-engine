// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "icon_tex_bin.h"
#include "icon_pal_bin.h"

NE_Material *Material;
NE_Palette *Palette;
NE_Sprite *Sprite[3];

void Draw3DScene(void)
{
    NE_2DViewInit();
    NE_SpriteDrawAll();
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_SwapScreens();

    Material = NE_MaterialCreate();
    Palette = NE_PaletteCreate();

    NE_MaterialTexLoad(Material, NE_PAL16, 128, 128, NE_TEXGEN_TEXCOORD,
                       (void *)icon_tex_bin);
    NE_PaletteLoad(Palette, (void *)icon_pal_bin, 32, NE_PAL16);
    NE_MaterialTexSetPal(Material, Palette);


    NE_ClearColorSet(NE_Gray, 31, 63);

    Sprite[0] = NE_SpriteCreate();
    Sprite[1] = NE_SpriteCreate();
    Sprite[2] = NE_SpriteCreate();

    NE_SpriteSetPos(Sprite[0], 0, 0);
    NE_SpriteSetSize(Sprite[0], 128, 128);
    NE_SpriteSetPriority(Sprite[0], 10);
    NE_SpriteSetMaterial(Sprite[0], Material);

    NE_SpriteSetPos(Sprite[1], 114, 32);
    NE_SpriteSetSize(Sprite[1], 56, 56);
    NE_SpriteSetPriority(Sprite[1], 5);
    NE_SpriteSetParams(Sprite[1], 15, 1, NE_Green);
    NE_SpriteSetMaterial(Sprite[1], Material);

    NE_SpriteSetPos(Sprite[2], 100, 50);
    NE_SpriteSetSize(Sprite[2], 56, 56);
    NE_SpriteSetPriority(Sprite[2], 1);
    NE_SpriteSetMaterial(Sprite[2], Material);
    NE_SpriteSetParams(Sprite[2], 15, 2, NE_White);

    int rot = 0;
    int x = 100, y = 50;

    while (1)
    {
        scanKeys();
        uint32 keys = keysHeld();

        rot = (rot + 2) & 511;
        NE_SpriteSetRot(Sprite[2], rot);

        if (keys & KEY_UP)
            y--;
        if (keys & KEY_DOWN)
            y++;
        if (keys & KEY_RIGHT)
            x++;
        if (keys & KEY_LEFT)
            x--;

        NE_SpriteSetPos(Sprite[2], x, y);

        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
