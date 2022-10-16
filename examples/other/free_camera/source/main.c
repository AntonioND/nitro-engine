// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "nitrocat_bin.h"
#include "texture_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

void Draw3DScene(void)
{
    NE_CameraUse(Camera);
    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
    NE_ModelDraw(Model);
}

int main()
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate objects
    Model = NE_ModelCreate(NE_Static);
    Camera = NE_CameraCreate();
    Material = NE_MaterialCreate();

    // Set coordinates for the camera
    NE_CameraSet(Camera,
                 -10, 0, 0, // Position
                   0, 0, 0,   // Look at
                   0, 1, 0);  // Up direction

    // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(Model, (u32 *)nitrocat_bin);
    // Load a RGB texture from RAM and assign it to "Material".
    NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD,
                       (u8 *)texture_bin);

    // Assign texture to model...
    NE_ModelSetMaterial(Model, Material);

    NE_ModelScale(Model, 5, 5, 5);

    // We set up a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    int angle = 0;

    while (1)
    {
        // Get keys information
        scanKeys();
        uint32 keys = keysHeld();

        printf("\x1b[0;0HPad: Rotate.\nA/B: Move forward/back.");

        if (keys & KEY_UP && angle < 92)
        {
            angle += 3;
            NE_CameraRotateFree(Camera, 3, 0, 0);
        }
        else if (keys & KEY_DOWN && angle > -92)
        {
            angle -= 3;
            NE_CameraRotateFree(Camera, -3, 0, 0);
        }

        if (keys & KEY_LEFT)
            NE_CameraRotateFree(Camera, 0, -3, 0);
        else if (keys & KEY_RIGHT)
            NE_CameraRotateFree(Camera, 0, 3, 0);

        if (keys & KEY_A)
            NE_CameraMoveFree(Camera, 0.2, 0, 0);
        else if (keys & KEY_B)
            NE_CameraMoveFree(Camera, -0.2, 0, 0);

        NE_Process(Draw3DScene);
        NE_WaitForVBL(0);
    }

    return 0;
}
