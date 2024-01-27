// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "cube_bin.h"

NE_Camera *Camara;
NE_Model *Model[6];
NE_Physics *Physics[6];

void Draw3DScene(void)
{
    NE_CameraUse(Camara);

    NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_BACK, 0);
    for (int i = 0; i < 5; i++)
        NE_ModelDraw(Model[i]);

    NE_PolyFormat(31, 0, NE_LIGHT_1, NE_CULL_BACK, 0);
    NE_ModelDraw(Model[5]);
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    Camara = NE_CameraCreate();
    NE_CameraSet(Camara,
                 -9, 7, 5,
                  0, 6, 0,
                  0, 1, 0);

    // Create objects
    for (int i = 0; i < 6; i++)
    {
        Model[i] = NE_ModelCreate(NE_Static);
        Physics[i] = NE_PhysicsCreate(NE_BoundingBox);

        NE_ModelLoadStaticMesh(Model[i], cube_bin);

        NE_PhysicsSetModel(Physics[i], Model[i]);

        NE_PhysicsSetSize(Physics[i], 1, 1, 1);
    }

    NE_PhysicsEnable(Physics[5],false);

    // Object coordinates
    NE_ModelSetCoord(Model[0], 0, 2, 0);
    NE_ModelSetCoord(Model[1], 0, 4, 0);
    NE_ModelSetCoord(Model[2], 0, 6, 0);
    NE_ModelSetCoord(Model[3], 0, 8, 0);
    NE_ModelSetCoord(Model[4], 0, 10, 0);
    NE_ModelSetCoord(Model[5], 0, 0, 0);

    // Set gravity
    NE_PhysicsSetGravity(Physics[0], 0.001);
    NE_PhysicsSetGravity(Physics[1], 0.001);
    NE_PhysicsSetGravity(Physics[2], 0.001);
    NE_PhysicsSetGravity(Physics[3], 0.001);
    NE_PhysicsSetGravity(Physics[4], 0.001);

    // Tell the engine what to do if there is a collision
    NE_PhysicsOnCollision(Physics[0], NE_ColBounce);
    NE_PhysicsOnCollision(Physics[1], NE_ColBounce);
    NE_PhysicsOnCollision(Physics[2], NE_ColBounce);
    NE_PhysicsOnCollision(Physics[3], NE_ColBounce);
    NE_PhysicsOnCollision(Physics[4], NE_ColBounce);

    // Set percent of energy kept after a bounce
    // Default is 50, 100 = no energy lost
    NE_PhysicsSetBounceEnergy(Physics[0], 100);
    NE_PhysicsSetBounceEnergy(Physics[1], 100);
    NE_PhysicsSetBounceEnergy(Physics[2], 100);
    NE_PhysicsSetBounceEnergy(Physics[3], 100);
    NE_PhysicsSetBounceEnergy(Physics[4], 100);

    // Lights
    NE_LightSet(0, NE_Green, -1, -1, 0);
    NE_LightSet(1, NE_Blue, -1, -1, 0);

    // Background
    NE_ClearColorSet(NE_Red, 31, 63);

    while (1)
    {
        NE_WaitForVBL(NE_UPDATE_PHYSICS);

        NE_Process(Draw3DScene);
    }

    return 0;
}
