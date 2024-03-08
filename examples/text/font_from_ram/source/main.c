// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024

#include <stdio.h>

#include <nds.h>
#include <NEMain.h>

#include "font_fnt_bin.h"
#include "font_16.h"
#include "font_256.h"

typedef struct {
    NE_Sprite *TextSprite;
} SceneData;

void Draw3DScene(void *arg)
{
    (void)arg;

    NE_ClearColorSet(RGB15(0, 7, 7), 31, 63);

    NE_2DViewInit();

    NE_RichTextRender3D(3, "VAWATa\ntajl", 0, 0);

    NE_RichTextRender3DAlpha(2, "Text with alpha", 10, 80,
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

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init 3D mode
    NE_InitDual3D();
    NE_InitConsole();

    // Load a 16-color font to be used for rendering text as quads

    NE_RichTextInit(2);
    NE_RichTextMetadataLoadMemory(2, font_fnt_bin, font_fnt_bin_size);

    {
        NE_Material *Font16 = NE_MaterialCreate();
        NE_MaterialTexLoad(Font16, NE_PAL16, 256, 256,
                           NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                           font_16Bitmap);

        NE_Palette *Pal16 = NE_PaletteCreate();
        NE_PaletteLoad(Pal16, font_16Pal, 16, NE_PAL16);

        NE_MaterialSetPalette(Font16, Pal16);

        // The material and palette will be deleted when the rich text font is
        // deleted.
        NE_RichTextMaterialSet(2, Font16, Pal16);
    }

    // Load a 256-color font to be used for rendering text as quads

    NE_RichTextInit(3);
    NE_RichTextMetadataLoadMemory(3, font_fnt_bin, font_fnt_bin_size);

    {
        NE_Material *Font256 = NE_MaterialCreate();
        NE_MaterialTexLoad(Font256, NE_PAL256, 256, 256,
                           NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                           font_256Bitmap);

        NE_Palette *Pal256 = NE_PaletteCreate();
        NE_PaletteLoad(Pal256, font_256Pal, 256, NE_PAL256);

        NE_MaterialSetPalette(Font256, Pal256);

        // The material and palette will be deleted when the rich text font is
        // deleted.
        NE_RichTextMaterialSet(3, Font256, Pal256);
    }

    // Load a 16-color font to be used for rendering text to textures.

    NE_RichTextInit(5);
    NE_RichTextMetadataLoadMemory(5, font_fnt_bin, font_fnt_bin_size);
    NE_RichTextBitmapSet(5, font_16Bitmap, 256, 256, NE_PAL16,
                         font_16Pal, font_16PalLen);

    // Render text to a texture using the last font we've loaded

    // We don't care about the palette, passing NULL will mark the palette
    // to be autodeleted when the material is deleted.
    NE_Material *Material = NULL;
    NE_RichTextRenderMaterial(5,
                "Sample: AWAV.\nÿ_ßðñÑü(o´Áá)|\nInvalid char: ŋ",
                &Material, NULL);

    // Create a sprite to be used to render the texture we've rendered

    Scene.TextSprite = NE_SpriteCreate();
    NE_SpriteSetMaterial(Scene.TextSprite, Material);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene, &Scene);

        scanKeys();
        if (keysHeld() & KEY_START)
            break;
    }

    NE_SpriteDelete(Scene.TextSprite);
    NE_MaterialDelete(Material);

    NE_RichTextEnd(2);
    NE_RichTextEnd(3);
    NE_RichTextEnd(5);

    return 0;
}
