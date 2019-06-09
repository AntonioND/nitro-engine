// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <fat.h>

#include <NEMain.h>

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

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
	Model = NE_ModelCreate(NE_Animated);
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();

	// Setup camera
	NE_CameraSet(Camera,
		     -8, -8, -8,
		     0, 0, 0,
		     0, 1, 0);

	if (NE_ModelLoadNEAFAT(Model, "nitro-engine/model.nea") == 0) {
		printf("Couldn't load model...");
		while(1) swiWaitForVBlank();
	}

	if (NE_FATMaterialTexLoadBMPtoRGBA(Material,
					   "nitro-engine/texture.bmp",
					   true) == 0) {
		printf("Couldn't load texture...");
		while(1) swiWaitForVBlank();
	}

	// Assign material to the model
	NE_ModelSetMaterial(Model, Material);

	NE_LightSet(0, NE_White, 0, -1, -1);

	NE_ClearColorSet(NE_Black, 31, 63);

	int maxframe = 0;
	NE_ModelAnimStart(Model, 0, 0, maxframe, NE_ANIM_LOOP, 32);

	float scale = 1;

	while (1) {
		printf("\x1b[0;0H"
		       "Pad: Rotate.\nR/L: Change max frame: %d  \n"
		       "A/B: Scale.\nStart: Screenshot.", maxframe);

		scanKeys();
		uint32 keys = keysHeld();
		uint32 keysdown = keysDown();

		if (keys & KEY_START)
			NE_ScreenshotBMP("NitroEngine/Screenshot.bmp");

		if (keys & KEY_A)
			scale += 0.1;
		if (keys & KEY_B)
			scale -= 0.1;

		NE_ModelScale(Model, scale, scale, scale);

		if (keysdown & KEY_R)
			NE_ModelAnimStart(Model, 0, 0, ++maxframe,
					  NE_ANIM_LOOP, 32);
		if (keysdown & KEY_L)
			NE_ModelAnimStart(Model, 0, 0, --maxframe,
					  NE_ANIM_LOOP, 32);

		if (keys & KEY_RIGHT)
			NE_ModelRotate(Model, 0, 2, 0);
		if (keys & KEY_LEFT)
			NE_ModelRotate(Model, 0, -2, 0);
		if (keys & KEY_UP)
			NE_ModelRotate(Model, 0, 0, 2);
		if (keys & KEY_DOWN)
			NE_ModelRotate(Model, 0, 0, -2);

		// Draw scene...
		NE_Process(Draw3DScene);
		NE_WaitForVBL(NE_UPDATE_ANIMATIONS);
	}

	return 0;
}
