// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "nitrocat_bin.h"
#include "texture_bin.h"

// We use pointers to use less ram if we are not using 3D, but you can create
// the structs directly and you won't have to use Create/Delete functions.
NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

int shading, alpha, id;

void Draw3DScene(void)
{
	//Set camera
	NE_CameraUse(Camera);

	//Set polygon format
	NE_PolyFormat(alpha, id, NE_LIGHT_0, NE_CULL_BACK, shading);

	//Draw model...
	NE_ModelDraw(Model);
}

int main(void)
{
	// This is needed for special screen effects
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_VBLANK, NE_HBLFunc);

	// Init console and Nitro Engine
	NE_Init3D();
	// libnds uses VRAM_C for the demo text console
	NE_TextureSystemReset(0, 0, NE_VRAM_AB);
	// This is needed to print text
	consoleDemoInit();

	// Allocate the things we will use
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();

	// Set coordinates for the camera
	NE_CameraSet(Camera,
		     -8, 0, 0,
		     0, 0, 0,
		     0, 1, 0);

	// Load mesh from RAM and assign it to the object "Model".
	NE_ModelLoadStaticMesh(Model, (u32 *)nitrocat_bin);
	// Load texture from RAM and assign it to "Material".
	NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD,
			   (u8 *) texture_bin);

	// Assign texture to model...
	NE_ModelSetMaterial(Model, Material);

	//Set some propierties to Material
	NE_MaterialSetPropierties(Material,
				  RGB15(24,24,24), //diffuse
				  RGB15(8,8,8),    // ambient
				  RGB15(0,0,0),    // specular
				  RGB15(0,0,0),    // emission
				  false, false);   // vtxcolor, useshininess

	// We set a light and its color
	NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

	// This enables shading (you can choose normal or toon).
	NE_ShadingEnable(true);
	// This enables outlining in all polygons, so be careful
	NE_OutliningEnable(true);

	// We set the second outlining color to red.
	// This will be used by polygons with ID 8 - 15.
	NE_OutliningSetColor(1, NE_Red);

	while(1) {
		// Get keys information
		scanKeys();
		uint32 keys = keysHeld();

		printf("\x1b[0;0H"
		       "Pad: Rotate.\nA: Toon shading.\n"
		       "B: Change alpha value.\nY: Wireframe mode (alpha = 0)\n"
		       "X: Outlining.");

		// Rotate model using the pad
		if (keys & KEY_UP)
			NE_ModelRotate(Model, 0, 0, 2);
		if (keys & KEY_DOWN)
			NE_ModelRotate(Model, 0, 0, -2);
		if (keys & KEY_RIGHT)
			NE_ModelRotate(Model, 0, 2, 0);
		if (keys & KEY_LEFT)
			NE_ModelRotate(Model, 0, -2, 0);

		// Change shading type
		if (keys & KEY_A)
			shading = NE_TOON_SHADING;
		else
			shading = NE_MODULATION;

		if (keys & KEY_B)
			alpha = 15; // Transparent
		else if (keys & KEY_Y)
			alpha = 0;  // Wireframe
		else
			alpha = 31; // Opaque

		// Change id co change outlining color
		if (keys & KEY_X)
			id = 8;
		else
			id = 0;

		// Draw scene
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
