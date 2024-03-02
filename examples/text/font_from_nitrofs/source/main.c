// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

NE_Sprite *TextSprite;

void Draw3DScene(void)
{
    NE_ClearColorSet(RGB15(0, 7, 7), 31, 63);

    NE_2DViewInit();

    NE_RichTextRender3D(3, "VAWATa\ntajl", 0, 0);

    NE_RichTextRender3DAlpha(2, "Text with alpha", 10, 80,
                             POLY_ALPHA(20) | POLY_CULL_BACK, 30);
}

void Draw3DScene2(void)
{
    NE_ClearColorSet(RGB15(7, 0, 7), 31, 63);

    NE_2DViewInit();

    NE_SpriteSetPos(TextSprite, 16, 32);
    NE_SpriteDraw(TextSprite);
}

__attribute__((noreturn)) void WaitLoop(void)
{
    printf("Press START to exit");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(0);
    }
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_InitDual3D();
    NE_InitConsole();

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\n");
        WaitLoop();
    }

    // Load a 256-color font to be used for rendering text as quads

    NE_RichTextInit(2);
    NE_RichTextMetadataLoadFAT(2, "fonts/font.fnt");
    NE_RichTextMaterialLoadGRF(2, "fonts/font_16_png.grf");

    // Load a 16-color font to be used for rendering text as quads

    NE_RichTextInit(3);
    NE_RichTextMetadataLoadFAT(3, "fonts/font.fnt");
    NE_RichTextMaterialLoadGRF(3, "fonts/font_256_png.grf");

    // Load a 16-color font to be used for rendering text to textures.

    NE_RichTextInit(5);
    NE_RichTextMetadataLoadFAT(5, "fonts/font.fnt");
    NE_RichTextBitmapLoadGRF(5, "fonts/font_16_png.grf");

    // Render text to a texture using the last font we've loaded

    // We don't care about the palette, passing NULL will mark the palette
    // to be autodeleted when the material is deleted.
    NE_Material *Material = NULL;
    NE_RichTextRenderMaterial(5,
                "Sample: AWAV.\nÿ_ßðñÑü(o´Áá)|\nInvalid char: ŋ",
                &Material, NULL);

    // Create a sprite to be used to render the texture we've rendered

    TextSprite = NE_SpriteCreate();
    NE_SpriteSetMaterial(TextSprite, Material);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDual(Draw3DScene, Draw3DScene2);

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    NE_SpriteDelete(TextSprite);
    NE_MaterialDelete(Material);

    NE_RichTextEnd(2);
    NE_RichTextEnd(3);
    NE_RichTextEnd(5);

    return 0;
}
