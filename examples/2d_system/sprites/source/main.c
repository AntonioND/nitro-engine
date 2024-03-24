// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "assets.h"
#include "icon.h"

NE_Sprite *Sprite[6];

void Draw3DScene1(void)
{
    NE_2DViewInit();

    NE_ClearColorSet(NE_White, 31, 63);

    NE_SpriteDraw(Sprite[3]);
    NE_SpriteDraw(Sprite[4]);
    NE_SpriteDraw(Sprite[5]);
}

void Draw3DScene2(void)
{
    NE_2DViewInit();

    NE_ClearColorSet(NE_Gray, 31, 63);

    NE_SpriteDraw(Sprite[0]);
    NE_SpriteDraw(Sprite[1]);
    NE_SpriteDraw(Sprite[2]);
}

int main(int argc, char *argv[])
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_InitDual3D();
    NE_InitConsole();

    NE_Material *MaterialIcon = NE_MaterialCreate();
    NE_Palette *PaletteIcon = NE_PaletteCreate();

    NE_MaterialTexLoad(MaterialIcon, NE_PAL16, 128, 128,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                       iconBitmap);
    NE_PaletteLoad(PaletteIcon, iconPal, iconPalLen / 2, NE_PAL16);
    NE_MaterialSetPalette(MaterialIcon, PaletteIcon);

    NE_Material *MaterialAssets = NE_MaterialCreate();
    NE_Palette *PaletteAssets = NE_PaletteCreate();

    NE_MaterialTexLoad(MaterialAssets, NE_PAL256, 512, 256,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                       assetsBitmap);
    NE_PaletteLoad(PaletteAssets, assetsPal, assetsPalLen / 2, NE_PAL16);
    NE_MaterialSetPalette(MaterialAssets, PaletteAssets);

    Sprite[0] = NE_SpriteCreate();
    Sprite[1] = NE_SpriteCreate();
    Sprite[2] = NE_SpriteCreate();
    Sprite[3] = NE_SpriteCreate();
    Sprite[4] = NE_SpriteCreate();
    Sprite[5] = NE_SpriteCreate();

    // Sprite with the same size as the texture
    NE_SpriteSetMaterial(Sprite[0], MaterialIcon);
    NE_SpriteSetPos(Sprite[0], 10, 40);
    NE_SpriteSetPriority(Sprite[0], 10);

    // Sprite with a different size than the texture (scaled down) and a
    // different color
    NE_SpriteSetMaterial(Sprite[1], MaterialIcon);
    NE_SpriteSetPos(Sprite[1], 114, 32);
    NE_SpriteSetPriority(Sprite[1], 5);
    NE_SpriteSetParams(Sprite[1], 15, 1, NE_Green);
    NE_SpriteSetSize(Sprite[1], 56, 56);

    // Sprite with a different size than the texture (scaled down), and with
    // transparency.
    NE_SpriteSetMaterial(Sprite[2], MaterialIcon);
    NE_SpriteSetPos(Sprite[2], 100, 50);
    NE_SpriteSetPriority(Sprite[2], 1);
    NE_SpriteSetParams(Sprite[2], 15, 2, NE_White);
    NE_SpriteSetSize(Sprite[2], 56, 56);

    // The following sprites will only use a small part of the texture

    NE_SpriteSetMaterial(Sprite[3], MaterialAssets);
    NE_SpriteSetPos(Sprite[3], 50, 60);
    NE_SpriteSetMaterialCanvas(Sprite[3], 384, 0, 484, 118);
    NE_SpriteSetSize(Sprite[3], 484 - 384, 118 - 0);

    NE_SpriteSetMaterial(Sprite[4], MaterialAssets);
    NE_SpriteSetPos(Sprite[4], 0, 0);
    NE_SpriteSetMaterialCanvas(Sprite[4], 73, 0, 152, 75);
    NE_SpriteSetSize(Sprite[4], 152 - 73, 75 - 0);

    NE_SpriteSetMaterial(Sprite[5], MaterialAssets);
    NE_SpriteSetPos(Sprite[5], 170, 20);
    NE_SpriteSetMaterialCanvas(Sprite[5], 0, 77, 72, 175);
    NE_SpriteSetSize(Sprite[5], 72 - 0, 175 - 77);

    int rot = 0;
    int x = 100, y = 50;

    printf("PAD:   Move\n");
    printf("START: Exit to loader\n");

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        uint32_t keys = keysHeld();

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

        if (keys & KEY_START)
            break;

        NE_SpriteSetPos(Sprite[2], x, y);

        NE_ProcessDual(Draw3DScene1, Draw3DScene2);
    }

    return 0;
}
