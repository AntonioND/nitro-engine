// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "a1rgb5_tex_bin.h"

NE_Material *Material;

void Draw3DScene(void)
{
    NE_2DViewInit();
    NE_2DDrawTexturedQuad(0, 0, 256, 256, 0, Material);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_MainScreenSetOnBottom();

    Material = NE_MaterialCreate();
    NE_MaterialTexLoad(Material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                       (u8 *)a1rgb5_tex_bin);

    // Wait a bit...
    scanKeys();
    NE_WaitForVBL(0);

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        touchPosition touch;

        if (keysHeld() & KEY_TOUCH)
        {
            // Update stylus coordinates when screen is pressed
            touchRead(&touch);
            NE_TextureDrawingStart(Material);

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

        NE_Process(Draw3DScene);
    }

    return 0;
}
