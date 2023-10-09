// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

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

    NE_InitDual3D_DMA();
    NE_InitConsole();

    while (1)
    {
        NE_WaitForVBL(0);
        NE_ProcessDual(Draw3DScene, Draw3DScene2);

        scanKeys();

        // Your code goes here
    }

    return 0;
}
