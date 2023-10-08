// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

// Note: You can also use NE_Init3D() and NE_ProcessDual(), but it is
// discouraged. You'll need to ensure that your game never drops frames. If it
// drops one frame, the image of one screen will be shown in both screens until
// a new frame is generated (screens are drawn in alternate frames).

#include <NEMain.h>

void Draw3DScene(void)
{

}

void Draw3DScene2(void)
{

}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    NE_InitSafeDual3D();

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();

        // Your code goes here

        NE_ProcessSafeDual3D(Draw3DScene, Draw3DScene2);
    }

    return 0;
}
