// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

NE_Camera *Camera;

void Draw3DScene(void)
{
    // Use camera and draw polygon.
    NE_CameraUse(Camera);

    // Begin drawing
    NE_PolyBegin(GL_QUAD);

        NE_PolyColor(NE_Red);    // Set next vertices color
        NE_PolyVertex(-1, 1, 0); // Send vertex

        NE_PolyColor(NE_Green);
        NE_PolyVertex(-1, -1, 0);

        NE_PolyColor(NE_Yellow);
        NE_PolyVertex(1, -1, 0);

        NE_PolyColor(NE_Blue);
        NE_PolyVertex(1, 1, 0);

    // This seems to not be needed
    NE_PolyEnd();
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    Camera = NE_CameraCreate();

    NE_CameraSet(Camera,
                 0, 0, 2,
                 0, 0, 0,
                 0, 1, 0);

    while (1)
    {
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
