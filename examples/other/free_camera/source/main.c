// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "robot_bin.h"
#include "texture.h"

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

    // Allocate objects
    Scene.Model = NE_ModelCreate(NE_Static);
    Scene.Camera = NE_CameraCreate();
    NE_Material *Material = NE_MaterialCreate();

    // Set coordinates for the camera
    NE_CameraSet(Scene.Camera,
                 -8, 3, 0,  // Position
                  0, 3, 0,  // Look at
                  0, 1, 0); // Up direction

    // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(Scene.Model, robot_bin);
    // Load a RGB texture from RAM and assign it to "Material".
    NE_MaterialTexLoad(Material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD,
                       textureBitmap);

    // Assign texture to model...
    NE_ModelSetMaterial(Scene.Model, Material);

    // We set up a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

    int angle = 0;

    while (1)
    {
        NE_WaitForVBL(0);

        // Get keys information
        scanKeys();
        uint32_t keys = keysHeld();

        printf("\x1b[0;0HPad: Rotate.\nA/B: Move forward/back.");

        if (keys & KEY_UP && angle < 92)
        {
            angle += 3;
            NE_CameraRotateFree(Scene.Camera, 3, 0, 0);
        }
        else if (keys & KEY_DOWN && angle > -92)
        {
            angle -= 3;
            NE_CameraRotateFree(Scene.Camera, -3, 0, 0);
        }

        if (keys & KEY_LEFT)
            NE_CameraRotateFree(Scene.Camera, 0, -3, 0);
        else if (keys & KEY_RIGHT)
            NE_CameraRotateFree(Scene.Camera, 0, 3, 0);

        if (keys & KEY_A)
            NE_CameraMoveFree(Scene.Camera, 0.05, 0, 0);
        else if (keys & KEY_B)
            NE_CameraMoveFree(Scene.Camera, -0.05, 0, 0);

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
