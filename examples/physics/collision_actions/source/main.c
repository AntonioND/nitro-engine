// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "model_bin.h"

NE_Camera *Camara;
NE_Model *Model[6];
NE_Physics *Physics[6];

void Draw3DScene(void)
{
	NE_CameraUse(Camara);

	// The first 3 boxes will be affected by one light and 3 last boxes by
	// another one

	NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_BACK, 0);
	for (int i = 0; i < 3; i++)
		NE_ModelDraw(Model[i]);

	NE_PolyFormat(31, 0, NE_LIGHT_1, NE_CULL_BACK, 0);
	for (int i = 3; i < 6; i++)
		NE_ModelDraw(Model[i]);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	NE_Init3D();

	Camara = NE_CameraCreate();
	NE_CameraSet(Camara,
		     -4, 3, 1,
		     0, 2, 0,
		     0, 1, 0);

	// Create objects
	for (int i = 0; i < 6; i++) {
		Model[i] = NE_ModelCreate(NE_Static);
		Physics[i] = NE_PhysicsCreate(NE_BoundingBox);

		NE_ModelLoadStaticMesh(Model[i], (u32 *)model_bin);

		NE_PhysicsSetModel(Physics[i], // Physics object
		                   (void *)Model[i]); // Model assigned to it

		NE_PhysicsEnable(Physics[i], false);
		NE_PhysicsSetSize(Physics[i], 1, 1, 1);
	}

	// Enable only the ones we will move
	NE_PhysicsEnable(Physics[0], true);
	NE_PhysicsEnable(Physics[1], true);
	NE_PhysicsEnable(Physics[2], true);

	// Object coordinates
	NE_ModelSetCoord(Model[0], 0, 4, 1);
	NE_ModelSetCoord(Model[1], 0, 4, 0);
	NE_ModelSetCoord(Model[2], 0, 4, -1);
	NE_ModelSetCoord(Model[3], 0, 0, 1);
	NE_ModelSetCoord(Model[4], 0, 0, 0);
	NE_ModelSetCoord(Model[5], 0, 0, -1);

	// Set gravity
	NE_PhysicsSetGravity(Physics[0], 0.001);
	NE_PhysicsSetGravity(Physics[1], 0.001);
	NE_PhysicsSetGravity(Physics[2], 0.001);

	// Tell the engine what to do if there is a collision
	NE_PhysicsOnCollition(Physics[0], NE_ColBounce);
	NE_PhysicsOnCollition(Physics[1], NE_ColStop);
	NE_PhysicsOnCollition(Physics[2], NE_ColNothing);

	// Set percent of energy kept after a bounce
	// Default is 50, 100 = no energy lost.
	NE_PhysicsSetBounceEnergy(Physics[0],100);
	NE_PhysicsSetBounceEnergy(Physics[1],75);
	NE_PhysicsSetBounceEnergy(Physics[2],50);

	// Lights
	NE_LightSet(0, NE_Green, -1, -1, 0);
	NE_LightSet(1, NE_Blue, -1, -1, 0);

	// Background
	NE_ClearColorSet(NE_Red, 31, 63);

	while(1) {
		NE_Process(Draw3DScene);
		NE_WaitForVBL(NE_UPDATE_PHYSICS);
	}

	return 0;
}
