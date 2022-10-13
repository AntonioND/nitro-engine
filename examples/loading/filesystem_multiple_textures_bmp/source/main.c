// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <fat.h>

#include <NEMain.h>

// To use this example you have to patch the NDS ROM with DLDI and copy
// nitro-engine folder to the root of your card.
//
// It will load the two BMPs and you will be able to take a screenshot.

NE_Camera *Camera;
NE_Model *Model;
NE_Material *OpaqueMaterial, *TransparentMaterial;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	NE_PolyFormat(31, 0, NE_LIGHT_0, NE_CULL_NONE, 0);
	NE_ModelDraw(Model);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init FAT, Nitro Engine and console
	fatInitDefault();
	NE_Init3D();
	// libnds uses VRAM_C for the text console, reserve A and B only
	NE_TextureSystemReset(0, 0, NE_VRAM_AB);
	// Init console in non-3D screen
	consoleDemoInit();

	// Allocate space for objects...
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate();
	OpaqueMaterial = NE_MaterialCreate();
	TransparentMaterial = NE_MaterialCreate();

	// Setup camera
	NE_CameraSet(Camera,
		     -2, -2, -2,
		     0, 0, 0,
		     0, 1, 0);

	// Load things from FAT...
	NE_ModelLoadStaticMeshFAT(Model, "nitro-engine/cube.bin");
	NE_FATMaterialTexLoadBMPtoRGBA(TransparentMaterial,
				       "nitro-engine/bmp16bit_x1rgb5.bmp", 1);
	NE_FATMaterialTexLoadBMPtoRGBA(OpaqueMaterial,
				       "nitro-engine/bmp24bit.bmp", 0);

	// Assign material to model
	NE_ModelSetMaterial(Model, TransparentMaterial);

	// Set up light
	NE_LightSet(0, NE_White, 0, -1, -1);

	// Background color...
	NE_ClearColorSet(NE_Gray, 31, 63);

	// Reduce size of the cube, the original model is 7.5 x 7.5 x 7.5
	NE_ModelScale(Model, 0.3, 0.3, 0.3);

	while (1) {
		scanKeys(); //Get keys information...
		uint32 keys = keysDown();

		// Change material if pressed
		if (keys & KEY_B)
			NE_ModelSetMaterial(Model, OpaqueMaterial);
		if (keys & KEY_A)
			NE_ModelSetMaterial(Model, TransparentMaterial);

		// Take screenshot of 3D screen
		if (keys & KEY_START)
			NE_ScreenshotBMP("NitroEngine/Screenshot.bmp");

		printf("\x1b[0;0HA/B: Change material.\n\nStart: Screenshot.");

		// Increase rotation, you can't get the rotation angle after
		// this. If you want to know always the angle, you should use
		// NE_ModelSetRot().
		NE_ModelRotate(Model, 1, 2, 0);

		// Draw scene...
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
