// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <time.h>

#include <NEMain.h>

#include "nitrocat_bin.h"
#include "texture_bin.h"

// Note: There are better ways to do this, for example, using the VBL interrupt
// to check if the second has changed.

NE_Camera *Camera;
NE_Model *Model;
NE_Material *Material;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);
	NE_ModelDraw(Model);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init Nitro Engine in normal 3D mode
	NE_Init3D();
	// libnds uses VRAM_C for the text console, reserve A and B only
	NE_TextureSystemReset(0, 0, NE_VRAM_AB);
	// Init console in non-3D screen
	consoleDemoInit();

	// Allocate space for objects
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();

	// Set coordinates for the camera
	NE_CameraSet(Camera,
		     -8, 0, 0, // Position
		     0, 0, 0,  // Look at
		     0, 1, 0); // Up direction

	// Load mesh from RAM and assign it to the object "Model".
	NE_ModelLoadStaticMesh(Model, (u32 *)nitrocat_bin);

	// Load a RGB texture from RAM and assign it to "Material".
	NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD,
			   (u8 *) texture_bin);

	// Assign texture to model...
	NE_ModelSetMaterial(Model, Material);

	// We set a light and its color
	NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

	int fpscount = 0;

	// This is used to see if second has changed
	int oldsec = 0;
	int seconds = 0;

	while (1) {
		// Get time
		time_t unixTime = time(NULL);
		struct tm* timeStruct = gmtime((const time_t *)&unixTime);
		seconds = timeStruct->tm_sec;

		// If new second
		if (seconds != oldsec) {
			// Reset fps count and print current
			oldsec = seconds;
			printf("\x1b[10;0HFPS: %d", fpscount);
			fpscount = 0;
		}

		// Get keys information
		scanKeys();
		uint32 keys = keysHeld();

		printf("\x1b[0;0HPad: Rotate.");

		// Rotate model using the pad
		if (keys & KEY_UP)
			NE_ModelRotate(Model, 0, 0, 2);
		if (keys & KEY_DOWN)
			NE_ModelRotate(Model, 0, 0, -2);
		if (keys & KEY_RIGHT)
			NE_ModelRotate(Model, 0, 2, 0);
		if (keys & KEY_LEFT)
			NE_ModelRotate(Model, 0, -2, 0);

		NE_Process(Draw3DScene);
		// Wait for next frame
		NE_WaitForVBL(0);
		// Increase frame count
		fpscount++;
	}

	return 0;
}
