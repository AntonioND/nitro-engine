// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "ball_bin.h"
#include "texture_bin.h"

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

bool wireframe;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	if (wireframe)
		NE_PolyFormat(0, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);
	else
		NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

	NE_ModelDraw(Model);

	printf("\x1b[5;0HPolygon count: %d   \nVertex count: %d   ",
	       NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(void)
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

	// Setup camera
	NE_CameraSet(Camera,
		     2, 0, 0,
		     0, 0, 0,
		     0, 1, 0);

	// Load model...
	NE_ModelLoadStaticMesh(Model,(u32*)ball_bin);
	NE_MaterialTexLoadBMPtoRGBA(Material, (void*) texture_bin, 0);
	NE_ModelSetMaterial(Model, Material);

	// Set some propierties to Material
	NE_MaterialSetPropierties(Material,
				 RGB15(31, 31, 31), // diffuse
				 RGB15(0, 0, 0),    // ambient
				 RGB15(0, 0, 0),    // specular
				 RGB15(0, 0, 0),    // emission
				 false, false);     // vtxcolor, useshininess

	// Set light color and direction
	NE_LightSet(0, NE_White, 0, 1, 0);
	NE_LightSet(1, NE_Blue, 0, -1, 0);
	NE_LightSet(2, NE_Red, 1, 0, 0);
	NE_LightSet(3, NE_Green, -1, 0, 0);

	while (1) {
		// Get keys information
		scanKeys();
		uint32 keys = keysHeld();

		printf("\x1b[0;0HPad: Rotate.\nA: Set wireframe mode.");

		// Rotate model
		if (keys & KEY_UP)
			NE_ModelRotate(Model, 0, 0, 2);
		if (keys & KEY_DOWN)
			NE_ModelRotate(Model, 0, 0, -2);
		if (keys & KEY_RIGHT)
			NE_ModelRotate(Model, 0, 2, 0);
		if (keys & KEY_LEFT)
			NE_ModelRotate(Model, 0, -2, 0);

		if (keys & KEY_A)
			wireframe = true;
		else
			wireframe = false;

		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
