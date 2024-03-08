// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

typedef struct {
    int placeholder;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    (void)Scene; // Silence unused variable warning
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    NE_InitConsole();

    while (1)
    {
        NE_WaitForVBL(0);
        NE_ProcessArg(Draw3DScene, &Scene);

        scanKeys();

        // Your code goes here
    }

    return 0;
}
