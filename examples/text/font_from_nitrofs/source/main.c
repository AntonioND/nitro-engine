// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <filesystem.h>
#include <nds.h>
#include <NEMain.h>

typedef struct {
    NE_Sprite *TextSprite;
} SceneData;

void Draw3DScene1(void *arg)
{
    (void)arg;

    NE_ClearColorSet(RGB15(0, 7, 7), 31, 63);

    NE_2DViewInit();

    NE_RichTextRender3D(1, "VAWATa\ntajl", 0, 0);

    NE_RichTextRender3DAlpha(0, "Text with alpha", 10, 80,
                             POLY_ALPHA(20) | POLY_CULL_BACK, 30);
}

void Draw3DScene2(void *arg)
{
    SceneData *Scene = arg;

    NE_ClearColorSet(RGB15(7, 0, 7), 31, 63);

    NE_2DViewInit();

    NE_SpriteSetPos(Scene->TextSprite, 16, 32);
    NE_SpriteDraw(Scene->TextSprite);
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

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

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

    // Load a 16-color font to be used for rendering text as quads

    NE_RichTextInit(0);
    NE_RichTextMetadataLoadFAT(0, "fonts/font.fnt");
    NE_RichTextMaterialLoadGRF(0, "fonts/font_16_png.grf");

    // Load a 256-color font to be used for rendering text as quads

    NE_RichTextInit(1);
    NE_RichTextMetadataLoadFAT(1, "fonts/font.fnt");
    NE_RichTextMaterialLoadGRF(1, "fonts/font_256_png.grf");

    // Load a 16-color font to be used for rendering text to textures.

    NE_RichTextInit(2);
    NE_RichTextMetadataLoadFAT(2, "fonts/font.fnt");
    NE_RichTextBitmapLoadGRF(2, "fonts/font_16_png.grf");

    // Render text to a texture using the last font we've loaded

    // We don't care about managing the palette. Passing NULL will tell Nitro
    // Engine to delete the palete automatically when the material is deleted.
    NE_Material *Material = NULL;
    NE_RichTextRenderMaterial(2,
                "Sample: AWAV.\nÿ_ßðñÑü(o´Áá)|\nInvalid char: ŋ",
                &Material, NULL);

    // Create a sprite to be used to render the texture we've rendered

    Scene.TextSprite = NE_SpriteCreate();
    NE_SpriteSetMaterial(Scene.TextSprite, Material);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDualArg(Draw3DScene1, Draw3DScene2, &Scene, &Scene);

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    NE_SpriteDelete(Scene.TextSprite);
    NE_MaterialDelete(Material);

    NE_RichTextResetSystem();

    return 0;
}
