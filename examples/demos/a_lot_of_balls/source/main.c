// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008, Ti-Ra-Nog
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

// Nitro Engine NDS Benchmark by Ti-Ra-Nog

#include <time.h>

#include <NEMain.h>

#include "model_bin.h"
#include "cubo_bin.h"
#include "texcubo_bin.h"
#include "texesfera_bin.h"

// Set the initial number of balls/spheres
#define NUM_BALLS	50
// Set the maxium number of balls/spheres
#define MAX_NUM_BALLS	255
// Set the minium number of balls/spheres
#define MIN_NUM_BALLS	0

int NUM = NUM_BALLS;

// Pointers to objects...
NE_Camera *Camera;
NE_Model *Sphere[MAX_NUM_BALLS], *Cube;
NE_Material *Material, *Material2;

float mov;

typedef struct {
	float x,y,z;
	float vx,vy,vz;
} _BALL_;

_BALL_ Ball[MAX_NUM_BALLS];

void Draw3DScene(void)
{
	scanKeys(); // Get keys information
	int keys = keysHeld(); // Keys continously pressed

	mov += 0.5; // If B is pressed, increase camera rotation speed
	NE_CameraUse(Camera); //Use camera and draw all objects.

	// Rotate the camara every frame if the B Button is pressed (slowly)
	if (!(keys & KEY_B))
		NE_ViewRotate(0, 0, mov);

	// Draw the cube
	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_NONE, 0);
	NE_ModelDraw(Cube);

	// Draw every Sphere
	NE_PolyFormat(31, 0, NE_LIGHT_ALL,NE_CULL_BACK, 0);
	for (int i = 0; i < NUM; i++)
		NE_ModelDraw(Sphere[i]);

	// Get some information AFTER drawing but BEFORE returning from the
	// function.
	printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",
	       NE_GetPolygonCount(), NE_GetVertexCount());
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init Nitro Engine 3D rendering in one screen.
	NE_Init3D();
	// libnds uses VRAM_C for the text console, reserve A and B only
	NE_TextureSystemReset(0, 0, NE_VRAM_AB);
	// Init console in non-3D screen
	consoleDemoInit();

	// Allocate objects
	Cube = NE_ModelCreate(NE_Static); // Cube model
	Material = NE_MaterialCreate();   // Material for the cube
	Material2 = NE_MaterialCreate();  // Material fot the spheres

	for (int i = 0; i < MAX_NUM_BALLS; i++) // Create all spheres
		Sphere[i] = NE_ModelCreate(NE_Static);

	// Create and setup camera
	Camera = NE_CameraCreate();
	NE_CameraSet(Camera,
		     6, 6, 6,
		     0, 0, 0,
		     0, 1, 0);

	// Load sphere texture to its material
	NE_MaterialTexLoad(Material2, GL_RGB, 64, 64,TEXGEN_TEXCOORD,
			   (u8 *) texesfera_bin);

	// Loop until it arrives to the max ball number
	for (int i = 0; i < MAX_NUM_BALLS; i++) {
		//Load every sphere model
		NE_ModelLoadStaticMesh(Sphere[i], (u32 *)model_bin);
		// Set Material2 to every Sphere
		NE_ModelSetMaterial(Sphere[i],Material2);
	}

	// Load cube texture to its material
	NE_MaterialTexLoad(Material, GL_RGB, 64, 64, TEXGEN_TEXCOORD,
			   (u8 *) texcubo_bin);
	// Load the cube mesh
	NE_ModelLoadStaticMesh(Cube,(u32*) cubo_bin);
	// Set the cube material
	NE_ModelSetMaterial(Cube,Material);
	// Resize the cube (it's originally 7x7x7, now it's 21x21x21)
	NE_ModelScale(Cube, 3, 3, 3);

	// Set up the white light
	NE_LightSet(0,NE_White,0,1,0);

	//Enable shading
	NE_ShadingEnable(true);

	// Set cube coordinates to (0, 0, 0)
	NE_ModelSetCoord(Cube, 0, 0, 0);

	// Set start coordinates/rotation for models using random formules...
	for (int i = 0; i < MAX_NUM_BALLS; i++) {
		// Set the speed for each axis
		Ball[i].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 10;
		Ball[i].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 10;
		Ball[i].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 10;

		// Randomly invert the speeds
		if (rand() & 1)
			Ball[i].vx *= -1;
		if (rand() & 1)
			Ball[i].vy *= -1;
		if (rand() & 1)
			Ball[i].vz *= -1;
	}

	// Initialize some variables
	int fpscount = 0;
	int oldsec = 0;
	int seconds = 0;

	while (1) {
		// Time Variables/Structs
		time_t unixTime = time(NULL);
		struct tm* timeStruct = gmtime((const time_t *)&unixTime);
		seconds = timeStruct->tm_sec;

		// Have we moved to a new second?
		if (seconds != oldsec) {
			oldsec = seconds; // old second = new second
			printf("\x1b[0;20HFPS: %d", fpscount);
			fpscount = 0; // Reset FPS count for next second
		}

		// Get keys information
		scanKeys();
		int keys = keysHeld(); // Keys Continously pressed
		int keysd = keysDown(); // Keys NOW pressed (only this frame)

		// Set the model rotation for every Sphere
		for (int i = 0; i < NUM; i++)
			NE_ModelRotate(Sphere[i], 25 / i, -25 / i, 25 / i);

		// Calculate the model position for every model
		for (int i = 0; i < NUM; i++) {
			// If the ball crashes with one of the faces of the cube
			// invert the speed of the corresponding axis.
			if ((Ball[i].x >= 10.5) || (Ball[i].x <= -10.5))
				Ball[i].vx *= -1;
			if ((Ball[i].y >= 9.5) || (Ball[i].y <= -9.0))
				Ball[i].vy *= -1;
			if ((Ball[i].z >= 10.5) || (Ball[i].z <= -10.5))
				Ball[i].vz *= -1;

			// Add speed to the position to calculate the new
			// position
			Ball[i].x += Ball[i].vx;
			Ball[i].y += Ball[i].vy;
			Ball[i].z += Ball[i].vz;

			// Update position
			NE_ModelSetCoord(Sphere[i],
					 Ball[i].x, Ball[i].y, Ball[i].z);
		}

		// Set all balls to (0, 0, 0) position.
		if (keys & KEY_Y) {
			for (int i = 0; i < NUM; i++) {
				Ball[i].x = 0;
				Ball[i].y = 0;
				Ball[i].z = 0;
			}
		}

		printf("\x1b[3;0HPolygon count: %d      ", NUM * 48);

		printf("\x1b[6;0HUp: Increase Ball Number.");
		printf("\x1b[7;0HDown: Decrease Ball Number.");
		printf("\x1b[8;0HR: Increase Ball Number by one.");
		printf("\x1b[9;0HL: Decrease Ball Number by one.");
		printf("\x1b[10;0HB: Stop camera rotation.");
		printf("\x1b[11;0HY: Set all balls to 0 position.");

		// Draw scene
		NE_Process(Draw3DScene);

		// Press UP: Increase the balls number
		if ((keys & KEY_UP) && (NUM != MAX_NUM_BALLS))
			NUM++;
		// Press DOWN: Decrease the balls number
		if ((keys & KEY_DOWN) && (NUM != MIN_NUM_BALLS))
			NUM--;
		// Press R: Increase the balls number by one
		if ((keysd & KEY_R) && (NUM != MAX_NUM_BALLS))
			NUM++;
		// Press L: Decrease the balls number by one
		if ((keysd & KEY_L) && (NUM != MIN_NUM_BALLS))
			NUM--;

		printf("\x1b[2;0HBalls Number: %d    ", NUM);

		// Wait for next frame
		NE_WaitForVBL(0);
		fpscount++; // Increase the fps count
	}

	return 0;
}
