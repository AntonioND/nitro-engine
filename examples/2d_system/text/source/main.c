// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "text_tex_bin.h"
#include "text2_tex_bin.h"

NE_Material *Text, *Text2;

touchPosition touch;

void Draw3DScene(void)
{
    NE_2DViewInit(); // Init 2D view

    scanKeys(); // Update stylus coordinates when screen is pressed

    if (keysHeld() & KEY_TOUCH)
        touchRead(&touch);

    // Print text on columns/rows
    NE_TextPrint(0, // Font slot
                 0, 0, // Coordinates
                 NE_White, // Color
                 "abcdefghijklmnopqrstuvwxyz012345\n6789\n"
                 "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
                 "12345678901234567890123456789012");
    NE_TextPrint(1, 1, 4, NE_Red, "AntonioND\n            rules!!!");
    NE_TextPrint(1, 8, 8, NE_Yellow, "Yeah!!!");

    // Print text on any coordinates
    NE_TextPrintFree(1, touch.px, touch.py, NE_Blue, "Test");
    NE_TextPrintBoxFree(1,
                        touch.px + 32, touch.py + 32,
                        touch.px + 96, touch.py + 64,
                        NE_Green, -1, "This is a\ntest!!");

    // Print formated text
    char text[64];
    sprintf(text, "Polys: %d\nVertices: %d\nCPU: %d%%",
            NE_GetPolygonCount(), NE_GetVertexCount(), NE_GetCPUPercent());
    NE_TextPrint(0, 0, 13, NE_White, text);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    memset(&touch, 0, sizeof(touchPosition));

    NE_Init3D();
    // Move 3D screen to lower screen
    NE_SwapScreens();

    Text = NE_MaterialCreate();
    Text2 = NE_MaterialCreate();
    NE_MaterialTexLoad(Text, NE_A1RGB5, 256, 64, NE_TEXGEN_TEXCOORD,
                       (void *)text_tex_bin);
    NE_MaterialTexLoad(Text2, NE_A1RGB5, 512, 128, NE_TEXGEN_TEXCOORD,
                       (void *)text2_tex_bin);

    NE_TextInit(0,     // Font slot
                Text,  // Image
                8, 8); // Size of one character (x, y)
    NE_TextInit(1, Text2, 12, 16);

    while (1)
    {
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
