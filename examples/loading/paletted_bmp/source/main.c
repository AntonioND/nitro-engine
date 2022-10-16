// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

// Include for Nitro Engine
#include <NEMain.h>

// Include model and textures
#include "bmp8bit_bin.h"
#include "bmp4bit_bin.h"
#include "model_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material4, *Material8;
NE_Palette *Palette4, *Palette8;

void Draw3DScene(void)
{
    // Set camera position and vector
    NE_CameraUse(Camera);

    // Draw model
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
    NE_ModelDraw(Model);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine, normal 3D mode
    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate objects
    Model = NE_ModelCreate(NE_Static);
    Camera = NE_CameraCreate();
    Material8 = NE_MaterialCreate();
    Material4 = NE_MaterialCreate();
    Palette8 = NE_PaletteCreate();
    Palette4 = NE_PaletteCreate();

    // Setup camera
    NE_CameraSet(Camera,
                 -2, -2, -2,
                  0, 0, 0,
                  0, 1, 0);

    // Load model
    NE_ModelLoadStaticMesh(Model, (u32 *)model_bin);

    // Load textures, this one's color 0 is transparent
    NE_MaterialTexLoadBMPtoRGB256(Material4, Palette4, (void *)bmp4bit_bin, 1);
    // This one is completely opaque
    NE_MaterialTexLoadBMPtoRGB256(Material8, Palette8, (void *)bmp8bit_bin, 0);

    // Assign material to model
    NE_ModelSetMaterial(Model, Material8);

    // Set light 0 color and vector
    NE_LightSet(0, NE_White, 0, -1, -1);

    // The original model of the cube is too big to fit, scale it down
    NE_ModelScale(Model, 0.3, 0.3, 0.3);

    printf("Press A/B to change texture");

    while (1)
    {
        scanKeys();

        // Rotate model a bit
        NE_ModelRotate(Model, 1, 2, 0);

        // Change material
        if (keysHeld() & KEY_A)
            NE_ModelSetMaterial(Model, Material8);
        else if (keysHeld() & KEY_B)
            NE_ModelSetMaterial(Model, Material4);

        // Draw 3D scene
        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
