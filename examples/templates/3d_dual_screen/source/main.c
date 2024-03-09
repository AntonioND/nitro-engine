// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

typedef struct {
    int placeholder;
} SceneData1;

typedef struct {
    int placeholder;
} SceneData2;

void Draw3DScene1(void *arg)
{
    SceneData1 *Scene = arg;

    (void)Scene; // Silence unused variable warning
}

void Draw3DScene2(void *arg)
{
    SceneData2 *Scene = arg;

    (void)Scene; // Silence unused variable warning
}

int main(int argc, char *argv[])
{
    SceneData1 Scene1 = { 0 };
    SceneData2 Scene2 = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_VBLANK, NE_HBLFunc);

    NE_InitDual3D_DMA();
    NE_InitConsole();

    while (1)
    {
        NE_WaitForVBL(0);
        NE_ProcessDualArg(Draw3DScene1, Draw3DScene2, &Scene1, &Scene2);

        scanKeys();

        // Your code goes here
    }

    return 0;
}
