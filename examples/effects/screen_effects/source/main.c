// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "nitrocat_bin.h"

// Create pointers to objects...
NE_Camera *Camera;
NE_Model *Model;

void Draw3DScene(void)
{
	// Set rear plane color
	NE_ClearColorSet(NE_Red, 31, 63);

	NE_CameraUse(Camera);
	NE_ModelDraw(Model);
}

void Draw3DScene2(void)
{
	// Set rear plane color
	NE_ClearColorSet(NE_Green, 31, 63);

	NE_CameraUse(Camera);
	NE_ModelDraw(Model);
}

int main(void)
{
	// This is needed for special screen effects
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init dual 3D mode and console
	NE_InitDual3D();
	NE_InitConsole();

	// Allocate objects...
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate();

	// Setup camera
	NE_CameraSet(Camera,
		     -8, 0, 0,
		     0, 0, 0,
		     0, 1, 0);

	// Load model...
	NE_ModelLoadStaticMesh(Model, (u32 *)nitrocat_bin);

	// Set light color and direction
	NE_LightSet(0,NE_White,-0.5,-0.5,-0.5);

	// Enable shading...
	NE_ShadingEnable(true);

	// Other test configurations
	//NE_SpecialEffectNoiseConfig(31);
	//NE_SpecialEffectSineConfig(3, 8);

	while (1) {
		// Get keys information
		scanKeys();
		uint32 keys = keysHeld();
		uint32 kdown = keysDown();

		printf("\x1b[0;0H"
		       "Pad: Rotate.\nA: Sine effect.\nB: Noise effect.\n"
		       "X: Deactivate effects.\nL/R: Pause/Unpause.");

		// Rotate model
		if (keys & KEY_UP)
			NE_ModelRotate(Model, 0, 0, 2);
		if (keys & KEY_DOWN)
			NE_ModelRotate(Model, 0, 0, -2);
		if (keys & KEY_RIGHT)
			NE_ModelRotate(Model, 0, 2, 0);
		if (keys & KEY_LEFT)
			NE_ModelRotate(Model, 0, -2, 0);

		// Activate effects
		if (kdown & KEY_B)
			NE_SpecialEffectSet(NE_NOISE);
		if (kdown & KEY_A)
			NE_SpecialEffectSet(NE_SINE);
		// Deactivate effects
		if (kdown & KEY_X)
			NE_SpecialEffectSet(0);

		// Pause effects
		if (kdown & KEY_L)
			NE_SpecialEffectPause(true);
		if (kdown & KEY_R)
			NE_SpecialEffectPause(false);

		// Draw 3D scenes
		NE_ProcessDual(Draw3DScene, Draw3DScene2);
		NE_WaitForVBL(0);
	}

	return 0;
}
