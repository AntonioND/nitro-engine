// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>
#include <math.h>

#include "text_tex_bin.h"
#include "text2_tex_bin.h"

NE_Camera *Camera;
NE_Material *Text, *Text2;

float angle;
float yPos;

void Draw3DScene(void)
{
    NE_CameraUse(Camera);
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);

    char angleText[30];
    sprintf(angleText, "Angle: %0.0f",
            angle);

    NE_TextPrint3D(0,                   // Slot
                   0, yPos - 2, 0,      // Position
                   angle, angle, angle, // Rotation
                   1,                   // Scale
                   NE_White, "Hello World!");
    NE_TextPrint3D(1,           // Slot
                   0, 1, yPos,  // Position
                   0, angle, 0, // Rotation
                   1,           // Scale
                   NE_Red, angleText);

    yPos = cosf(angle / 10);
    angle += 0.5;
}

int main()
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

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

    // Allocate objects
    Camera = NE_CameraCreate();

    // Set coordinates for the camera
    NE_CameraSet(Camera,
                 0, 0, 8,  // Position
                 0, 0, 0,  // Look at
                 0, 1, 0); // Up direction

    while (1)
    {
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
