// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <stdbool.h>

#include <filesystem.h>

#include <NEMain.h>

typedef struct {
    NE_Camera *Camera;
    NE_Model *Model;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_CameraUse(Scene->Camera);

    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
    NE_ModelDraw(Scene->Model);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    if (!nitroFSInit(NULL))
    {
        printf("nitroFSInit failed.\nPress START to exit");
        while (1)
        {
            swiWaitForVBlank();
            scanKeys();
            if (keysHeld() & KEY_START)
                return 0;
        }
    }

    // Allocate space for objects...
    Scene.Model = NE_ModelCreate(NE_Static);
    Scene.Camera = NE_CameraCreate();
    NE_Material *MaterialBlue = NE_MaterialCreate();
    NE_Material *MaterialRed = NE_MaterialCreate();
    NE_Palette *PaletteBlue = NE_PaletteCreate();
    NE_Palette *PaletteRed = NE_PaletteCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 -1, -1, -1,
                  0, 0, 0,
                  0, 1, 0);

    // Load things from FAT
    NE_ModelLoadStaticMeshFAT(Scene.Model, "cube.bin");

    NE_MaterialTexLoadFAT(MaterialBlue, NE_A3PAL32, 64, 64, NE_TEXGEN_TEXCOORD,
                          "spiral_blue_pal32.img.bin");
    NE_MaterialTexLoadFAT(MaterialRed, NE_A3PAL32, 64, 64, NE_TEXGEN_TEXCOORD,
                          "spiral_red_pal32.img.bin");

    NE_PaletteLoadFAT(PaletteBlue, "spiral_blue_pal32.pal.bin", NE_A3PAL32);
    NE_PaletteLoadFAT(PaletteRed, "spiral_red_pal32.pal.bin", NE_A3PAL32);

    NE_MaterialSetPalette(MaterialBlue, PaletteBlue);
    NE_MaterialSetPalette(MaterialRed, PaletteRed);

    // Assign material to model
    NE_ModelSetMaterial(Scene.Model, MaterialBlue);

    // Set up light
    NE_LightSet(0, NE_White, 0, -1, -1);

    // Background color
    NE_ClearColorSet(NE_Gray, 31, 63);

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys(); //Get keys information...
        uint32_t keys = keysDown();

        if (keys & KEY_START)
            return 0;

        // Change material if pressed
        if (keys & KEY_B)
            NE_ModelSetMaterial(Scene.Model, MaterialBlue);
        if (keys & KEY_A)
            NE_ModelSetMaterial(Scene.Model, MaterialRed);

        printf("\x1b[0;0HA/B: Change material.\n\nSTART: Exit.");

        // Increase rotation, you can't get the rotation angle after
        // this. If you want to know always the angle, you should use
        // NE_ModelSetRot().
        NE_ModelRotate(Scene.Model, 1, 2, 0);

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
