// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2024
//
// This file is part of Nitro Engine

//  TODO

#include <NEMain.h>

#include "sphere_bin.h"

typedef struct {
    NE_Camera *Camera;
    NE_Model *ModelWithoutMatrix;
    NE_Model *ModelWithMatrix;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_CameraUse(Scene->Camera);

    NE_ModelDraw(Scene->ModelWithoutMatrix);
    NE_ModelDraw(Scene->ModelWithMatrix);
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine.
    NE_Init3D();
    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    // Allocate space for everything.
    Scene.ModelWithoutMatrix = NE_ModelCreate(NE_Static);
    Scene.ModelWithMatrix = NE_ModelCreate(NE_Static);

    Scene.Camera = NE_CameraCreate();

    // Setup camera
    NE_CameraSet(Scene.Camera,
                 0, 0, 3,
                 0, 0, 0,
                 0, 1, 0);

    // Load models
    NE_ModelLoadStaticMesh(Scene.ModelWithoutMatrix, sphere_bin);
    NE_ModelLoadStaticMesh(Scene.ModelWithMatrix, sphere_bin);

    // Set up light
    NE_LightSet(0, NE_Yellow, 0, -0.5, -0.5);

    // Set start coordinates/rotation of the models
    NE_ModelSetCoord(Scene.ModelWithoutMatrix, -1, 0, 0);

    // Transformation matrix we are going to use for a model. Note that this
    // matrix is transposed compared to what most 3D documentation describes.
    m4x3 matrix = {{
        // 3x3 transformation
        inttof32(1), 0,                     0,
        0,           inttof32(1),           0,
        0,                     0, inttof32(1),
        // Translation vector
        inttof32(2),           0,           0
    }};

    int translation = inttof32(2);

    printf("The right ball uses a matrix\n"
           "assigned by the user, the left\n"
           "one has rotation managed by\n"
           "Nitro Engine.");

    while (1)
    {
        NE_WaitForVBL(0);

        // Rotate the first model
        NE_ModelRotate(Scene.ModelWithoutMatrix, -1, 2, 1);

        // Update matrix manually
        translation += floattof32(0.05);
        if (translation > inttof32(3))
            translation = inttof32(1);
        matrix.m[9] = translation;

        // Assign matrix again
        NE_ModelSetMatrix(Scene.ModelWithMatrix, &matrix);

        // Draw scene
        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
