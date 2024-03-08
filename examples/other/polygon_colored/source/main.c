// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

typedef struct {
    NE_Camera *Camera;
} SceneData;

void Draw3DScene(void *arg)
{
    SceneData *Scene = arg;

    // Use camera and draw polygon.
    NE_CameraUse(Scene->Camera);

    // Begin drawing
    NE_PolyBegin(GL_QUAD);

        NE_PolyColor(NE_Red);    // Set next vertices color
        NE_PolyVertex(-1, 1, 0); // Send vertex

        NE_PolyColor(NE_Green);
        NE_PolyVertex(-1, -1, 0);

        NE_PolyColor(NE_Yellow);
        NE_PolyVertex(1, -1, 0);

        NE_PolyColor(NE_Blue);
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

    NE_CameraSet(Scene.Camera,
                 0, 0, 2,
                 0, 0, 0,
                 0, 1, 0);

    while (1)
    {
        NE_WaitForVBL(0);

        NE_ProcessArg(Draw3DScene, &Scene);
    }

    return 0;
}
