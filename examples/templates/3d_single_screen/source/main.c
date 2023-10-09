// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2023
//
// This file is part of Nitro Engine

#include <NEMain.h>

void Draw3DScene(void)
{

}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_InitConsole();

    while (1)
    {
        NE_WaitForVBL(0);
        NE_Process(Draw3DScene);

        scanKeys();

        // Your code goes here
    }

    return 0;
}
