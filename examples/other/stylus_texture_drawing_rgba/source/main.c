// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a1rgb5.h"

typedef struct {
    NE_Material *Material;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_2DViewInit();
    NE_2DDrawTexturedQuad(0, 0, 256, 256, 0, Scene->Material);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_MainScreenSetOnBottom();

    Scene.Material = NE_MaterialCreate();
    NE_MaterialTexLoad(Scene.Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                       a1rgb5Bitmap);

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        touchPosition touch;

        if (keysHeld() & KEY_TOUCH)
        {
            // Update stylus coordinates when screen is pressed
            touchRead(&touch);
            NE_TextureDrawingStart(Scene.Material);

            // Draw blue pixels with no transparency. The function
            // NE_TexturePutPixelRGBA() makes sure to not draw outside of the
            // function, so we don't have to check here.
            uint16_t color = RGB15(0, 0, 31) | BIT(15);
            NE_TexturePutPixelRGBA(touch.px, touch.py, color);
            NE_TexturePutPixelRGBA(touch.px + 1, touch.py, color);
            NE_TexturePutPixelRGBA(touch.px, touch.py + 1, color);
            NE_TexturePutPixelRGBA(touch.px + 1, touch.py + 1, color);

            NE_TextureDrawingEnd();
        }

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
