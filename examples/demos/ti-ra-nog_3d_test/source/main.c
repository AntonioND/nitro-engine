// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008, Ti-Ra-Nog
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

// 3D Test by Ti-Ra-Nog

//#define FAT_MESH_TEXT

#include <NEMain.h>

//#include <sys/dir.h>
#include <fat.h>
#include <time.h>

#ifndef FAT_MESH_TEXT
#include "cube_bin.h"
#include "sphere_bin.h"
#include "texcube_bin.h"
#include "texsphere_bin.h"
#endif

// Set the initial number of balls/spheres
#define NUM_BALLS	50
// Set the initial number of balls/spheres for the second scene
#define SCENE2_BALLS	10
// Set the maxium number of balls/spheres
#define MAX_NUM_BALLS	255
// Set the minium number of balls/spheres
#define MIN_NUM_BALLS	0

int NUM = NUM_BALLS;

NE_Camera *Camara, *Camara2, *Camara3, *Camara4;
NE_Model *Sphere[MAX_NUM_BALLS], *Cube, *Cube2;
NE_Material *Material;
NE_Material *Material2;
NE_Material *Material3;

bool camera_swap = false;

#define SUMX	3
#define SUMY	1
#define SUMZ	2

typedef struct {
	float x, y, z;
	float vx, vy, vz;
} ball_t;

ball_t Ball[MAX_NUM_BALLS];

int posx = 0;
int posy = 0;
int posz = 0;
int posx2 = 0;
int posy2 = 0;
int posz2 = 0;

void Draw3DScene(void)
{
	if (camera_swap)
		NE_CameraUse(Camara);
	else
		NE_CameraUse(Camara2);

	NE_ViewRotate(posx, posy, posz);

	// Set the culling to none
	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_NONE, 0);
	NE_ModelDraw(Cube);
	NE_ModelDraw(Cube2);

	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

	// Draw all spheres
	for (int i = 0; i < SCENE2_BALLS; i++)
		NE_ModelDraw(Sphere[i]);

	printf("\x1b[19;1HPolygon RAM1: %d   \n Vertex RAM1: %d   ",
	       NE_GetPolygonCount(), NE_GetVertexCount());
}

void Draw3DScene2(void)
{
	if (camera_swap)
		NE_CameraUse(Camara2);
	else
		NE_CameraUse(Camara);

	NE_ViewRotate(posx2, posy2, posz2);

	// Set the culling to none
	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_NONE, 0);
	NE_ModelDraw(Cube);
	NE_ModelDraw(Cube2);

	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_BACK, 0);

	// Draw all spheres
	for (int i = SCENE2_BALLS; i < NUM; i++)
		NE_ModelDraw(Sphere[i]);

	printf("\x1b[21;1HPolygon RAM2: %d   \n Vertex RAM2: %d   ",
	       NE_GetPolygonCount(), NE_GetVertexCount());
}

void dual(void)
{
	char file[200];
	bool noise_effect = false;
	bool sine_effect = false;
	bool recording = false;
	bool auto_rotate = false;
	bool hide_text = false;

	// Screenshot number count
	int nc = 0;
	int sc = 0;

	// Allocate all needed objects
	for (int i = 0; i < MAX_NUM_BALLS; i++)
		Sphere[i] = NE_ModelCreate(NE_Static);

	Cube = NE_ModelCreate(NE_Static);
	Cube2 = NE_ModelCreate(NE_Static);
	Camara = NE_CameraCreate();
	Camara2 = NE_CameraCreate();
	Material = NE_MaterialCreate();
	Material2 = NE_MaterialCreate();

#ifdef FAT_MESH_TEXT
	NE_MaterialTexLoadFAT(Material, GL_RGB, 64, 64, TEXGEN_TEXCOORD,
			      "texsphere.bin");

	for (int i = 0; i < MAX_NUM_BALLS; i++) {
		// Load sphere model. Note that this is just to test. This is a
		// really inefficient way to load the same model several times.
		// Ideally, you'd load it once and then create models by cloning
		// the first one.
		NE_ModelLoadStaticMeshFAT(Sphere[i], "sphere.bin");
		// Set Material to every Sphere
		NE_ModelSetMaterial(Sphere[i], Material);
	}

	NE_MaterialTexLoadFAT(Material2, GL_RGB, 64, 64, TEXGEN_TEXCOORD,
			      "texcube.bin");
	NE_ModelLoadStaticMeshFAT(Cube, "cube.bin");
	NE_ModelSetMaterial(Cube, Material2);

	NE_ModelLoadStaticMeshFAT(Cube2, "cube.bin");
	NE_ModelSetMaterial(Cube2, Material2);
#else
	NE_MaterialTexLoad(Material, GL_RGB, 64, 64, TEXGEN_TEXCOORD,
			   (u8 *) texsphere_bin);

	for (int i = 0; i < MAX_NUM_BALLS; i++) {
		// Load sphere model
		NE_ModelLoadStaticMesh(Sphere[i], (u32 *)sphere_bin);
		// Set material to every sphere
		NE_ModelSetMaterial(Sphere[i], Material);
	}

	NE_MaterialTexLoad(Material2, GL_RGB, 64, 64, TEXGEN_TEXCOORD,
			   (u8 *) texcube_bin);
	NE_ModelLoadStaticMesh(Cube, (u32 *) cube_bin);
	NE_ModelSetMaterial(Cube, Material2);

	NE_ModelLoadStaticMesh(Cube2, (u32 *) cube_bin);
	NE_ModelSetMaterial(Cube2, Material2);
#endif

	NE_ModelScale(Cube2, 3, 3, 3);

	NE_LightSet(0, NE_White, 0, -1, 0);

	if (camera_swap) {
		NE_CameraSet(Camara2,
			     -6, 6, -6,
			     0, 0, 0,
			     0, 1, 0);
		NE_CameraSet(Camara,
			     -2, 2, -2,
			     0, 0, 0,
			     0, 1, 0);
	} else {
		NE_CameraSet(Camara,
			     -6, 6, -6,
			     0, 0, 0,
			     0, 1, 0);
		NE_CameraSet(Camara2,
			     -2, 2, -2,
			     0, 0, 0,
			     0, 1, 0);
	}

	NE_SetConsoleColor(NE_Red);

	// Clear screen and move cursor to the top
	printf("\x1b[2J");

	printf("\x1b[1;1HTi-Ra-Nog 3D Test\n =================");
	printf("\x1b[4;1HR: Save Video (So Sloooow).");
	printf("\x1b[5;1HL: Save screenshot.");
	printf("\x1b[6;1HA: Move camera (Held Button).");
	printf("\x1b[7;1HB: Sine Effect.");
	printf("\x1b[8;1HX: Noise Effect.");
	printf("\x1b[9;1HY: Show/hide the text.");
	printf("\x1b[10;1HStart: Move mesh (on/off).");
	printf("\x1b[11;1HSelect: LCD Swap.");
	printf("\x1b[12;1HUP: Scene mode 1.");
	printf("\x1b[13;1HDown: Scene mode 2.");
	printf("\x1b[23;8HPowered By Nitro Engine");

	for (int i = 0; i < NUM; i++)
		NE_ModelScale(Sphere[i], 0.5, 0.5, 0.5);

	// Set start coordinates/rotation for models using random formules...
	for (int i = 0; i < MAX_NUM_BALLS; i++) {
		// Set the absolute initial speed
		Ball[i].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 30;
		Ball[i].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 30;
		Ball[i].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 30;

		// Randomly invert the speed
		if (rand() & 1)
			Ball[i].vx *= -1;
		if (rand() & 1)
			Ball[i].vy *= -1;
		if (rand() & 1)
			Ball[i].vz *= -1;
	}

	// Set start coordinates/rotation for models using random formules
	for (int i = MAX_NUM_BALLS; i < NUM; i++) {
		// Set the absolute initial speed
		Ball[i].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 1;
		Ball[i].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 1;
		Ball[i].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 1;

		// Randomly invert the speed
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

		// Has the second changed?
		if (seconds != oldsec) {
			oldsec = seconds; // old second = new second
			printf("\x1b[1;24HFPS: %d", fpscount);
			fpscount = 0; // Reset FPS count for next second
		}

		scanKeys();
		uint32 keysd = keysDown();
		uint32 keysh = keysHeld();

		// Set rotation for every sphere
		for (int i = 0; i < NUM; i++)
			NE_ModelRotate(Sphere[i], 25 / i, -25 / i, 25 / i);

		// Calculate the model position for every model based on its
		// current position and speed
		for (int i = 0; i < SCENE2_BALLS; i++) {
			// If the ball crashes with one of the faces of the cube
			// invert the speed of the corresponding axis.
			if ((Ball[i].x >= 2.5) || (Ball[i].x <= -2.5))
				Ball[i].vx *= -1;
			if ((Ball[i].y >= 2.5) || (Ball[i].y <= -2.5))
				Ball[i].vy *= -1;
			if ((Ball[i].z >= 2.5) || (Ball[i].z <= -2.5))
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

		// Calculate the model position for every model based on its
		// current position and speed
		for (int i = SCENE2_BALLS; i < NUM; i++) {
			// If the ball crashes with one of the faces of the cube
			// invert the speed of the corresponding axis.
			if ((Ball[i].x >= 9.5) || (Ball[i].x <= -9.5))
				Ball[i].vx *= -1;
			if ((Ball[i].y >= 9.5) || (Ball[i].y <= -9.5))
				Ball[i].vy *= -1;
			if ((Ball[i].z >= 9.5) || (Ball[i].z <= -9.5))
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

		if (keysd & KEY_UP)
			camera_swap = false;

		if (keysd & KEY_DOWN)
			camera_swap = true;

		if (keysh & KEY_A) {
			posx += SUMX;
			posy += SUMY;
			posz += SUMZ;

			posx2 += SUMX;
			posy2 += SUMY;
			posz2 += SUMZ;
		}

		NE_ProcessDual(Draw3DScene, Draw3DScene2);
		fpscount++;
		swiWaitForVBlank();

		// If SELECT is pressed swap top and bottom screens
		if (keysd & KEY_SELECT)
			lcdSwap();

		if (keysd & KEY_L) {
			// Generate file name
			sprintf(file, "screen/screenshot_%04d.bmp", sc);
			// Save the screenshot
			NE_ScreenshotBMP(file);
			// Increase the Screenshot number
			sc++;
			// Wait for next frame (this is necessary for not making
			// artifacts in the next screenshot)
			NE_WaitForVBL(0);
		}

		if (keysd & KEY_START) {
			if (!auto_rotate)
				auto_rotate = true;
			else
				auto_rotate = false;
		}

		if (auto_rotate) {
			posx += SUMX;
			posy += SUMY;
			posz += SUMZ;

			posx2 += SUMX;
			posy2 += SUMY;
			posz2 += SUMZ;
		}

		if (keysd & KEY_R) {
			if (!recording)
				recording = true;
			else
				recording = false;
		}

		if (recording) {
			// Generate file name
			sprintf(file,"video/vid/video_%04d.bmp",nc);
			// Save the screenshot
			NE_ScreenshotBMP(file);
			// Increase the Screenshot number
			nc++;
			// Wait for next frame (this is needed for not making
			// artifacs in the next screenshot)
			NE_WaitForVBL(0);
		}

		if (keysd & KEY_Y) {
			if (!hide_text) {
				hide_text = true;

				// Clear screen and move cursor to the top
				printf("\x1b[2J");

				printf("\x1b[1;1H"
				       "Ti-Ra-Nog 3D Test\n"
				       " =================");
				printf("\x1b[23;8HPowered By Nitro Engine");
			} else {
				hide_text = false;
				printf("\x1b[4;1HR: Save Video (So Sloooow).");
				printf("\x1b[5;1HL: Save screenshot.");
				printf("\x1b[6;1HA: Move camera (Held Button).");
				printf("\x1b[7;1HB: Sine Effect.");
				printf("\x1b[8;1HX: Noise Effect.");
				printf("\x1b[9;1HY: Show/hide the text.");
				printf("\x1b[10;1HStart: Move mesh (on/off).");
				printf("\x1b[11;1HSelect: LCD Swap.");
				printf("\x1b[12;1HUP: Scene mode 1.");
				printf("\x1b[13;1HDown: Scene mode 2.");
			}
		}

		// If B is pressed use the sine effect. Stop if pressed again
		if (keysd & KEY_B) {
			if (!sine_effect) {
				sine_effect = true;
				NE_SpecialEffectSet(NE_SINE);
			} else {
				sine_effect = false;
				NE_SpecialEffectSet(0);
			}
		}

		if (keysd & KEY_X) {
			if (!noise_effect) {
				noise_effect = true;
				NE_SpecialEffectSet(NE_NOISE);
			} else {
				noise_effect = false;
				NE_SpecialEffectSet(0);
			}
		}
	}
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	fatInitDefault();

	NE_InitDual3D();
	NE_InitConsole();

	NE_WaitForVBL(0);

	dual();

	return 0;
}
