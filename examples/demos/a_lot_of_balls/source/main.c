// Nitro Engine NDS Benchmark by Ti-Ra-Nog

#include <time.h>

#include <NEMain.h>

#include "model_bin.h"
#include "cubo_bin.h"
#include "texcubo_bin.h"
#include "texesfera_bin.h"

#define NUM_BALLS 50       // Set the initial number of balls/spheres
#define MAX_NUM_BALLS 255  // Set the maxium number of balls/spheres (more than 255 will crash)
#define MIN_NUM_BALLS 0    // Set the minium number of balls/spheres (less than 0 will crash) (obviously xD)

int NUM = NUM_BALLS;

NE_Camera *Camera; // Pointers to objects...
NE_Model *Esfera[MAX_NUM_BALLS], *Cubo;
NE_Material *Material, *Material2;

u8 a;
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

	mov += 0.5; // if the b button is pressed, increase camera rotation speed
	NE_CameraUse(Camera); //Use camera and draw all objects.

	// Rotate the camara every frame if the B Button is pressed(slowly)
	if (!(keys & KEY_B))
		NE_ViewRotate(0, 0, mov);

	// Draw the cube
	NE_PolyFormat(31, 0, NE_LIGHT_ALL, NE_CULL_NONE, 0);
	NE_ModelDraw(Cubo);

	// Draw every Sphere
	NE_PolyFormat(31, 0, NE_LIGHT_ALL,NE_CULL_BACK, 0);
	for (int i = 0; i < NUM; i++)
		NE_ModelDraw(Esfera[i]);

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
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); //Init text.
	
	Cubo = NE_ModelCreate(NE_Static);   // Create the cube model (allocate memory)
	Material = NE_MaterialCreate();  // Create the material for the cube (allocate memory)
	Material2 = NE_MaterialCreate(); // Create the material fot the balls/spheres (allocate memory)
	
	for(a = 0; a < MAX_NUM_BALLS; a++) Esfera[a] = NE_ModelCreate(NE_Static); // Create every Sphere
	Camera = NE_CameraCreate(); //Create space for everything.
	
	NE_CameraSet(Camera, 6,6,6,  0,0,0,  0,1,0); //Set camera.
	
	NE_MaterialTexLoad(Material2, GL_RGB, 64, 64,TEXGEN_TEXCOORD,(u8*) texesfera_bin); // Load sphere texture on ram
	
	for(a = 0; a < MAX_NUM_BALLS; a++) // loop until it arrived to the max ball number
	{ 
		NE_ModelLoadStaticMesh(Esfera[a],(u32*)model_bin); //Load every sphere model.
		NE_ModelSetMaterial(Esfera[a],Material2); // Set Material2 to every Sphere
	}
	NE_MaterialTexLoad(Material, GL_RGB, 64, 64,TEXGEN_TEXCOORD,(u8*) texcubo_bin);	// Load Cube texture on ram	
	NE_ModelLoadStaticMesh(Cubo,(u32*) cubo_bin); // Load the cube mesh
	NE_ModelSetMaterial(Cubo,Material);	   // Set the cube material
	NE_ModelScale(Cubo, 3, 3, 3);           // Resize the cube (it's originally 7x7x7, now it's 21x21x21)
	
	NE_LightSet(0,NE_White,0,1,0);            // Set up the white light
	
	NE_ShadingEnable(true); //Enable shading
	NE_ModelSetCoord(Cubo, 0,0,0); // Set the cube to (0, 0, 0) position (not needed really... But it's... hmmmm... I don't know because I put it xD, it's just quite cool :O)
	
	for(a = 0; a < MAX_NUM_BALLS; a++) //Set start coordinates/rotation for models using random formules...
	{
		Ball[a].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 10; // Set the speed for X axis
		Ball[a].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 10; // Set the speed for Y axis
		Ball[a].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 10; // set the speed for Z axis
		if(rand()&1)Ball[a].vx *= -1;                                // If te ball is odd invest his direction for X axis("odd" and "invest" words by google translator... I think you understood me :P)
		if(rand()&1)Ball[a].vy *= -1;                                // If te ball is odd invest his direction for y axis
		if(rand()&1)Ball[a].vz *= -1;                                // If te ball is odd invest his direction for z axis
	}
	
	u8 fpscount = 0; // initialize some variables
	u8 oldsec = 0;
	u8 seconds = 0;
	
	while(1) {
		// Time Variables/Structs
		time_t unixTime = time(NULL);
		struct tm* timeStruct = gmtime((const time_t *)&unixTime);
		seconds = timeStruct->tm_sec;
		
		if(seconds != oldsec) // is currently a new second?
		{
			oldsec = seconds; // old second = new second
			printf("\x1b[0;20HFPS: %d", fpscount);
			fpscount = 0; // put frame anothertime to 0
		}
		scanKeys();  //Get keys information
		int keys = keysHeld(); // Keys Continously pressed
		int keysd = keysDown(); // Keys NOW pressed (only this frame)
		
		for(a = 0; a < NUM; a++) NE_ModelRotate(Esfera[a], 25/a, -25/a, 25/a); // Set the model rotation for every Sphere
		for(a = 0; a < NUM; a++) // Calculate the model position for every model
		{
			if((Ball[a].x>=10.5) || (Ball[a].x<=-10.5)) // If the ball/sphere have crashed with the supposed cube face in the x axis then invest the speed
			{
				Ball[a].vx *= -1;
			}
			if((Ball[a].y>=9.5) || (Ball[a].y<=-9.0)) // If the ball/sphere have crashed with the supposed cube face in the y axis then invest the speed
			{
				Ball[a].vy *= -1;
			}
			if((Ball[a].z>=10.5) || (Ball[a].z<=-10.5)) // If the ball/sphere have crashed with the supposed cube face in the z axis then invest the speed
			{
				Ball[a].vz *= -1;
			}	
			Ball[a].x += Ball[a].vx; // Sphere/ball current X axis position + current sphere/ball X axis speed
			Ball[a].y += Ball[a].vy; // Sphere/ball current y axis position + current sphere/ball y axis speed
			Ball[a].z += Ball[a].vz; // Sphere/ball current z axis position + current sphere/ball z axis speed
			
			NE_ModelSetCoord(Esfera[a], Ball[a].x, Ball[a].y, Ball[a].z); // Update the ball/sphere position
		}
		if(keys & KEY_Y) { for(a = 0; a < NUM; a++) NE_ModelSetCoord(Esfera[a], 0, 0, 0); }// Set all balls to (0, 0, 0) position.
		
		printf("\x1b[3;0HPolygon count: %d      ", NUM*48);
		
		printf("\x1b[6;0HUp: Increase Ball Number.");
		printf("\x1b[7;0HDown: Decrease Ball Number.");
		printf("\x1b[8;0HR: Increase Ball Number by one.");
		printf("\x1b[9;0HL: Decrease Ball Number by one.");
		printf("\x1b[10;0HB: Stop camera rotation.");
		printf("\x1b[11;0HY: Set all balls to 0 position.");
		NE_Process(Draw3DScene); //Draw scene
		
		if((keys & KEY_UP) && (NUM != MAX_NUM_BALLS)) NUM++;   // If you pressed the up button then increase the balls number
		if((keys & KEY_DOWN) && (NUM != MIN_NUM_BALLS)) NUM--; // If you pressed the down button then decrease the balls number
		if((keysd & KEY_R) && (NUM != MAX_NUM_BALLS)) NUM++;   // If you pressed the up button then increase the balls number by one
		if((keysd & KEY_L) && (NUM != MIN_NUM_BALLS)) NUM--; // If you pressed the down button then decrease the balls number by one
		
		printf("\x1b[2;0HBalls Number: %d    ", NUM);
		NE_WaitForVBL(0); // Wait for vbl (whait for next frame)
		fpscount++; // Increase the fps count
	}
	
	return 0;
}
