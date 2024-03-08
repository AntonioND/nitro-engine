// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "pal256.h"

typedef struct {
    NE_Material *Material;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();
    NE_2DDrawTexturedQuad(0, 0,
                          256, 256,
                          0, Scene->Material);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine, normal 3D mode, and move the 3D screen to the
    // bottom screen
    NE_Init3D();
    NE_SwapScreens();

    // Allocate objects
    Scene.Material = NE_MaterialCreate();
    NE_Palette *Palette = NE_PaletteCreate();

    // Load texture
    NE_MaterialTexLoad(Scene.Material, NE_PAL256, 256, 256, NE_TEXGEN_TEXCOORD,
                       pal256Bitmap);
    NE_PaletteLoad(Palette, pal256Pal, 32, NE_PAL256);
    NE_MaterialSetPalette(Scene.Material, Palette);

    // Modify color 254 of the palette so that we can use it to draw with a
    // known color
    NE_PaletteModificationStart(Palette);
    NE_PaletteRGB256SetColor(254, RGB15(0, 0, 31));
    NE_PaletteModificationEnd();

    touchPosition touch;

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        touchRead(&touch);

        if (keysHeld() & KEY_TOUCH)
        {
            NE_TextureDrawingStart(Scene.Material);

            // The function NE_TexturePutPixelRGB256() makes sure to not draw
            // outside of the function, so we don't have to check here.
            NE_TexturePutPixelRGB256(touch.px, touch.py, 254);
            NE_TexturePutPixelRGB256(touch.px + 1, touch.py, 254);
            NE_TexturePutPixelRGB256(touch.px, touch.py + 1, 254);
            NE_TexturePutPixelRGB256(touch.px + 1, touch.py + 1, 254);

            NE_TextureDrawingEnd();
        }

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
