// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2024
//
// This file is part of Nitro Engine

// Important: This example won't work on devkitPro. GRF loading functions are
// only supported in BlocksDS.

#include <filesystem.h>
#include <NEMain.h>

typedef struct {
    NE_Material *Material1;
} SceneData1;

typedef struct {
    NE_Material *Material2;
} SceneData2;

void Draw3DScene(void *arg)
{
    SceneData1 *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(0, 0,
                          256, 192,
                          0, Scene->Material1);
}

void Draw3DScene2(void *arg)
{
    SceneData2 *Scene = arg;

    NE_2DViewInit();

    NE_2DDrawTexturedQuad(64, 32,
                          64 + 128, 32 + 128,
                          0, Scene->Material2);
}

__attribute__((noreturn)) void WaitLoop(void)
{
    printf("Press START to exit");
    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysHeld() & KEY_START)
            exit(0);
    }
}

int main(int argc, char *argv[])
{
    SceneData1 Scene1 = { 0 };
    SceneData2 Scene2 = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    consoleDemoInit();

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\n");
        WaitLoop();
    }

    // Init 3D mode
    NE_InitDual3D();
    NE_InitConsole();

    // Allocate objects
    Scene1.Material1 = NE_MaterialCreate();
    Scene2.Material2 = NE_MaterialCreate();

    if (NE_MaterialTexLoadGRF(Scene1.Material1, NULL, NE_TEXGEN_TEXCOORD,
                              "a1rgb5_png.grf") == 0)
    {
        printf("Failed to load GRF 1\n");
        WaitLoop();
    }

    // This material has a palette, but we don't care about it. By setting the
    // pointer to NULL it will be marked to be autoedeleted when the material is
    // deleted.
    if (NE_MaterialTexLoadGRF(Scene2.Material2, NULL, NE_TEXGEN_TEXCOORD,
                              "a3pal32_png.grf") == 0)
    {
        printf("Failed to load GRF 2\n");
        WaitLoop();
    }

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessDualArg(Draw3DScene, Draw3DScene2, &Scene1, &Scene2);
    }

    return 0;
}
