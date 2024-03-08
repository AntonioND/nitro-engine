// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "texture.h"

typedef struct {
    NE_Camera *Camera;
    NE_Material *Material;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    NE_CameraUse(Scene->Camera);

    // This set material's color to drawing color (default = white)
    NE_MaterialUse(Scene->Material);

    // In general you should avoid using the functions below for drawing models
    // because they have a much lower performance than precompiled models.

    // Begin drawing
    NE_PolyBegin(GL_QUAD);

        NE_PolyColor(NE_Red);    // Set next vertices color
        NE_PolyTexCoord(0, 0);   // Texture coordinates
        NE_PolyVertex(-1, 1, 0); // Send new vertex

        NE_PolyColor(NE_Blue);
        NE_PolyTexCoord(0, 64);
        NE_PolyVertex(-1, -1, 0);

        NE_PolyColor(NE_Green);
        NE_PolyTexCoord(64, 64);
        NE_PolyVertex(1, -1, 0);

        NE_PolyColor(NE_Yellow);
        NE_PolyTexCoord(64, 0);
        NE_PolyVertex(1, 1, 0);

    NE_PolyEnd();
}

int main(int argc, char *argv[])
{
    SceneData Scene = { 0 };

    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    Scene.Camera = NE_CameraCreate();
    Scene.Material = NE_MaterialCreate();

    NE_CameraSet(Scene.Camera,
                 0, 0, 2,
                 0, 0, 0,
                 0, 1, 0);

    NE_MaterialTexLoad(Scene.Material, NE_A1RGB5, 128, 128, NE_TEXGEN_TEXCOORD,
                       textureBitmap);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
