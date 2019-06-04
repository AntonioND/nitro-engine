//    _   _ _ _               ______             _              _____   ____   _____ 
//   | \ | (_) |  AntonioND  |  ____|           (_)            |  __ \ / __ \ / ____|
//   |  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___  | |__) | |  | | |     
//   | . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \ |  ___/| |  | | |     
//   | |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/ | |    | |__| | |____ 
//   |_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___| |_|     \____/ \_____|
//                                          __/ |                                    
//                                         |___/                                     
//      _                ___ ____  _____    _______        _  ___                _ 
//     | |        ______|  _|___ \|  __ \  |__   __|      | ||_  |______        | |
//     | |  _____|______| |   __) | |  | |    | | ___  ___| |_ | |______|_____  | |
//     | | |______|_____| |  |__ <| |  | |    | |/ _ \/ __| __|| |_____|______| | |
//     | |       |______| |  ___) | |__| |    | |  __/\__ \ |_ | |______|       | |
//     |_|              | |_|____/|_____/     |_|\___||___/\__|| |              |_|
//                      |___|                                |___|                 
// ________             ____________      ________              _____   __              
// ___  __ )____  __    ___  __/__(_)     ___  __ \_____ _      ___  | / /____________ _
// __  __  |_  / / /    __  /  __  /________  /_/ /  __ `/________   |/ /_  __ \_  __ `/
// _  /_/ /_  /_/ /     _  /   _  /_/_____/  _, _// /_/ /_/_____/  /|  / / /_/ /  /_/ / 
// /_____/ _\__, /      /_/    /_/        /_/ |_| \__,_/        /_/ |_/  \____/_\__, /  
//         /____/                                                              /____/   



//#define FAT_MESH_TEXT

#include <NEMain.h>
//#include <sys/dir.h>
#include <fat.h>
#include <time.h>



#ifndef FAT_MESH_TEXT
#include "cubo_bin.h"
#include "esfera_bin.h"
#include "texcubo_bin.h"
#include "texesfera_bin.h"
#endif

#define NUM_BALLS 50       // Set the initial number of balls/spheres
#define SCENE2_BALLS 10    // Set the initial number of balls/spheres for the second scene
#define MAX_NUM_BALLS 255  // Set the maxium number of balls/spheres (more than 255 will crash)
#define MIN_NUM_BALLS 0    // Set the minium number of balls/spheres (less than 0 will crash) (obviously xD)

u8 NUM = (int)NUM_BALLS;
u8 a;
NE_Camera * Camara, * Camara2, * Camara3, * Camara4;
NE_Model * Esfera[MAX_NUM_BALLS], * Cubo, * Cubo2;
NE_Material * Material;
NE_Material * Material2;
NE_Material * Material3;
char archivo[200];
int nc = 0;
int sc = 0;
int i;
int mode0 = 0;
int mode1 = 0;
int mode2 = 0;
int mode3 = 0;
int mode4 = 0;
int mode5 = 0;
int mode6 = 0;
int min = -2;
int max = 2;
int sumx = 3;
int sumy = 1;
int sumz = 2;

typedef struct { // The balls struct
	float x,y,z;
	float vx,vy,vz;
} _BALL_;

_BALL_ Ball[MAX_NUM_BALLS];

int newanglex, newangley, newanglez;
int posx = 0;
int posy = 0;
int posz = 0;
int posx2 = 0;
int posy2 = 0;
int posz2 = 0;

void Draw3DScene(void)
{	
	scanKeys();
	int keysd = keysHeld();
	int keys = keysDown();

	if(keysd & KEY_UP)
	{ 
		mode6=0;
	}

	if(keysd & KEY_DOWN)
	{ 
		mode6=1;
	}
		
	if(mode6==1)
	{
		NE_CameraUse(Camara);
	} else 
	{
		NE_CameraUse(Camara2);
	}

	if(mode0==1)
	{
		posx += sumx;
		posy += sumy;
		posz += sumz;

		NE_ViewRotate(posx, posy, posz);
	}

	if(keysd & KEY_A) 
	{
		posx += sumx;
		posy += sumy;
		posz += sumz;

		NE_ViewRotate(posx, posy, posz);
	}

	if(keys & KEY_START)
	{ 
		if(mode1==0) mode1=1;
		else mode1=0;
	}
	if(mode1==1) NE_ViewRotate(posx, posy, posz);


	NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_NONE,0); // Set the culling to none
	NE_ModelDraw(Cubo);
	NE_ModelDraw(Cubo2);

	NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_BACK,0);
	for(a = 0; a < SCENE2_BALLS; a++) NE_ModelDraw(Esfera[a]); // Draw every Sphere

	printf("\x1b[19;1HPolygon RAM1: %d   \n Vertex RAM1: %d   ",NE_GetPolygonCount(), NE_GetVertexCount());
}

void Draw3DScene2(void)
{
	scanKeys();
	int keysd = keysHeld();
	int keys = keysDown();

	if(mode6==1)
	{
		NE_CameraUse(Camara2);
	} else 
	{
		NE_CameraUse(Camara);
	}


	if(keysd & KEY_A) 
	{
		NE_ViewRotate(posx2, posy2, posz2);
		posx2 += sumx;
		posy2 += sumy;
		posz2 += sumz;
	}

	if(keys & KEY_START)
	{ 
		if(mode1==0)mode1=1; else mode1=0;
	}
	if(mode0==1) 
	{
		posx2 += sumx;
		posy2 += sumy;
		posz2 += sumz;
		NE_ViewRotate(posx2, posy2, posz2);
	}

	NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_NONE,0); // Set the culling to none
	NE_ModelDraw(Cubo);
	NE_ModelDraw(Cubo2);

	NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_BACK,0);
	for(a = SCENE2_BALLS; a < NUM; a++) NE_ModelDraw(Esfera[a]); // Draw every Sphere

	printf("\x1b[21;1HPolygon RAM2: %d   \n Vertex RAM2: %d   ",NE_GetPolygonCount(), NE_GetVertexCount());
}


void dual()
{
	for(a = 0; a < MAX_NUM_BALLS; a++) Esfera[a] = NE_ModelCreate(NE_Static); // Create every Sphere	
	
	Cubo = NE_ModelCreate(NE_Static);
	Cubo2 = NE_ModelCreate(NE_Static);
	Camara = NE_CameraCreate();
	Camara2 = NE_CameraCreate();
	Material = NE_MaterialCreate();
	Material2 = NE_MaterialCreate();
	
#ifdef FAT_MESH_TEXT
	NE_MaterialTexLoadFAT(Material, GL_RGB, 64, 64,TEXGEN_TEXCOORD,"texesfera.bin"); // Load the sphere texture in fat mode
	for(a = 0; a < MAX_NUM_BALLS; a++) // loop until it arrived to the max ball number
	{ 
		NE_ModelLoadStaticMeshFAT(Esfera[a],"esfera.bin"); //Load every sphere model.
		NE_ModelSetMaterial(Esfera[a],Material); // Set Material2 to every Sphere
	}
	
	
	NE_MaterialTexLoadFAT(Material2, GL_RGB, 64, 64,TEXGEN_TEXCOORD,"texcubo.bin"); // Load the cube texture in fat mode
	NE_ModelLoadStaticMeshFAT(Cubo,"cubo.bin");
	NE_ModelSetMaterial(Cubo,Material2);
	
	
	NE_ModelLoadStaticMeshFAT(Cubo2,"cubo.bin");
	NE_ModelSetMaterial(Cubo2,Material2);
	
#else
	NE_MaterialTexLoad(Material, GL_RGB, 64, 64,TEXGEN_TEXCOORD,(u8*) texesfera_bin); // Load the sphere texture
	for(a = 0; a < MAX_NUM_BALLS; a++) // loop until it arrived to the max ball number
	{ 
		NE_ModelLoadStaticMesh(Esfera[a],(u32*)esfera_bin); //Load every sphere model.
		NE_ModelSetMaterial(Esfera[a],Material); // Set Material2 to every Sphere
	}
	
	
	NE_MaterialTexLoad(Material2, GL_RGB, 64, 64,TEXGEN_TEXCOORD,(u8*) texcubo_bin); // Load the cube texture
	NE_ModelLoadStaticMesh(Cubo,(u32*) cubo_bin);
	NE_ModelSetMaterial(Cubo,Material2);
	
	
	NE_ModelLoadStaticMesh(Cubo2,(u32*) cubo_bin);
	NE_ModelSetMaterial(Cubo2,Material2);
#endif

	NE_ModelScale(Cubo2, 3, 3, 3);

	NE_LightSet(0,NE_White,0,-1,0);
	
		
	if(mode6==1)
	{
		NE_CameraSet(Camara2, -6,6,-6, 0,0,0, 0,1,0);
		NE_CameraSet(Camara, -2,2,-2, 0,0,0, 0,1,0);
	} else 
	{
		NE_CameraSet(Camara, -6,6,-6, 0,0,0, 0,1,0);
		NE_CameraSet(Camara2, -2,2,-2, 0,0,0, 0,1,0);
	}
	
	
	NE_SetConsoleColor(NE_Red);
	
	printf("\x1b[0;0H                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ");

	printf("\x1b[1;1HTi-Ra-Nog 3D Test\n =================");
	printf("\x1b[4;1HR: Save Video (So Sloooow).");
	printf("\x1b[5;1HL: Save ScreenShoot.");
	printf("\x1b[6;1HA: Move camera (Held Button).");
	printf("\x1b[7;1HB: Sine Effect.");
	printf("\x1b[8;1HX: Noise Effect.");
	printf("\x1b[9;1HY: Show/hide the text.");
	printf("\x1b[10;1HStart: Move mesh (on/off).");
	printf("\x1b[11;1HSelect: LCD Swap.");
	printf("\x1b[12;1HUP: Scene mode 1.");
	printf("\x1b[13;1HDown: Scene mode 2.");
	printf("\x1b[23;8HPowered By Nitro Engine");
	
	for(a = 0; a < NUM; a++) NE_ModelScale(Esfera[a], 0.5, 0.5, 0.5);
	
	for(a = 0; a < MAX_NUM_BALLS; a++) //Set start coordinates/rotation for models using random formules...
	{
		Ball[a].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 30; // Set the speed for X axis
		Ball[a].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 30; // Set the speed for Y axis
		Ball[a].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 30; // set the speed for Z axis
		if(rand()&1)Ball[a].vx *= -1;                                // If te ball is odd invest his direction for X axis("odd" and "invest" words by google translator... I think you understood me :P)
		if(rand()&1)Ball[a].vy *= -1;                                // If te ball is odd invest his direction for y axis
		if(rand()&1)Ball[a].vz *= -1;                                // If te ball is odd invest his direction for z axis
	}
	
	for(a = MAX_NUM_BALLS; a < NUM; a++) //Set start coordinates/rotation for models using random formules...
	{
		Ball[a].vx = (float)(1 + ((float)(rand() % 10) / 10)) / 1; // Set the speed for X axis
		Ball[a].vy = (float)(1 + ((float)(rand() % 10) / 10)) / 1; // Set the speed for Y axis
		Ball[a].vz = (float)(1 + ((float)(rand() % 10) / 10)) / 1; // set the speed for Z axis
		if(rand()&1)Ball[a].vx *= -1;                                // If te ball is odd invest his direction for X axis("odd" and "invest" words by google translator... I think you understood me :P)
		if(rand()&1)Ball[a].vy *= -1;                                // If te ball is odd invest his direction for y axis
		if(rand()&1)Ball[a].vz *= -1;                                // If te ball is odd invest his direction for z axis
	}
	
	u8 fpscount = 0; // initialize some variables
	u8 oldsec = 0;
	u8 seconds = 0;
	
	while(1) 
	{
		// Time Variables/Structs
		time_t unixTime = time(NULL);
		struct tm* timeStruct = gmtime((const time_t *)&unixTime);
		seconds = timeStruct->tm_sec;
		
		if(seconds != oldsec) // is currently a new second?
		{
			oldsec = seconds; // old second = new second
			printf("\x1b[1;24HFPS: %d", fpscount);
			fpscount = 0; // put frame anothertime to 0
		}
		
		scanKeys();
		int keysd = keysDown();
				
		for(a = 0; a < NUM; a++) NE_ModelRotate(Esfera[a], 25/a, -25/a, 25/a); // Set the model rotation for every Sphere
		
		for(a = 0; a < SCENE2_BALLS; a++) // Calculate the model position for every model
		{
			if((Ball[a].x>=2.5) || (Ball[a].x<=-2.5)) // If the ball/sphere have crashed with the supposed cube face in the x axis then invest the speed
			{
				Ball[a].vx *= -1;
			}
			if((Ball[a].y>=2.5) || (Ball[a].y<=-2.5)) // If the ball/sphere have crashed with the supposed cube face in the y axis then invest the speed
			{
				Ball[a].vy *= -1;
			}
			if((Ball[a].z>=2.5) || (Ball[a].z<=-2.5)) // If the ball/sphere have crashed with the supposed cube face in the z axis then invest the speed
			{
				Ball[a].vz *= -1;
			}
			Ball[a].x += Ball[a].vx; // Sphere/ball current X axis position + current sphere/ball X axis speed
			Ball[a].y += Ball[a].vy; // Sphere/ball current y axis position + current sphere/ball y axis speed
			Ball[a].z += Ball[a].vz; // Sphere/ball current z axis position + current sphere/ball z axis speed
		
			NE_ModelSetCoord(Esfera[a], Ball[a].x, Ball[a].y, Ball[a].z); // Update the ball/sphere position
		}
		
		for(a = SCENE2_BALLS; a < NUM; a++) // Calculate the model position for every model
		{
			if((Ball[a].x>=9.5) || (Ball[a].x<=-9.5)) // If the ball/sphere have crashed with the supposed cube face in the x axis then invest the speed
			{
				Ball[a].vx *= -1;
			}
			if((Ball[a].y>=9.5) || (Ball[a].y<=-9.5)) // If the ball/sphere have crashed with the supposed cube face in the y axis then invest the speed
			{
				Ball[a].vy *= -1;
			}
			if((Ball[a].z>=9.5) || (Ball[a].z<=-9.5)) // If the ball/sphere have crashed with the supposed cube face in the z axis then invest the speed
			{
				Ball[a].vz *= -1;
			}	
			
			Ball[a].x += Ball[a].vx; // Sphere/ball current X axis position + current sphere/ball X axis speed
			Ball[a].y += Ball[a].vy; // Sphere/ball current y axis position + current sphere/ball y axis speed
			Ball[a].z += Ball[a].vz; // Sphere/ball current z axis position + current sphere/ball z axis speed
			
			NE_ModelSetCoord(Esfera[a], Ball[a].x, Ball[a].y, Ball[a].z); // Update the ball/sphere position
		}
		
		NE_ProcessDual(Draw3DScene, Draw3DScene2);
		fpscount++;
		swiWaitForVBlank();
		
		if(keysd & KEY_SELECT) lcdSwap();
		
		if(keysd & KEY_L)
		{
			/*pdir=diropen("/screen"); 
			if (!pdir){
				mkdir("/screen/", 0755);
			}*/
			
			if(sc<10){ // if the screenshoot number is less than 10...
			sprintf(archivo,"screen/ScreenShoot_000%d.bmp",nc); // Put 3 zeros before the screenshoot number.
			}
			if((sc>9) && (sc<100)){ // if the screenshoot number is less than 100...
			sprintf(archivo,"screen/ScreenShoot_00%d.bmp",nc); // Put 2 zeros before the screenshoot number.
			}
			if((sc>99) && (sc<1000)){ // if the screenshoot number is less than 1000...
			sprintf(archivo,"screen/ScreenShoot_0%d.bmp",nc); // Put one zero before the screenshoot number.
			}
			
			NE_ScreenshotBMP(archivo); // Save the screenshoot
			sc++;                      // Increase the Screenshot number
			NE_WaitForVBL(0);          // Wait for next frame (this is necessary for not making artifacs on the next screenshoot)
		}
		
		if(keysd & KEY_START)
		{ 
			if(mode0==0) mode0=1;
			else mode0=0;
		}
		
		if(mode0==1)
		{
			//NE_StaticModelRotate(Cubo,1,1,-1); NE_StaticModelRotate(Cubo2,-1,1,1);
		}
		
		
		if(keysd & KEY_R)
		{ 
			if(mode2==0)mode2=1; else mode2=0;
		}
		
		if(mode2==1)
		{
			/*pdir=diropen("/video"); 
			if (!pdir){
				mkdir("/video/", 0755);
			}*/
			if(nc<99)
			{ // if the current screenshoot is less than 99 save on the first folder
				if(nc<10)
				{ // if the screenshoot number is less than 10...
					sprintf(archivo,"video/vid/Video_000%d.bmp",nc); // Put 3 zeros before the screenshoot number.
				}
				if((nc>9) && (nc<100))
				{ // if the screenshoot number is less than 100...
					sprintf(archivo,"video/vid/Video_00%d.bmp",nc); // Put 2 zeros before the screenshoot number.
				}
				if((nc>99) && (nc<1000))
				{ // if the screenshoot number is less than 1000...
					sprintf(archivo,"video/vid/Video_0%d.bmp",nc); // Put one zero before the screenshoot number.
				}	
			} else { // if not, save on the second one
				if(nc<10)
				{
					sprintf(archivo,"video/vid2/Video_000%d.bmp",nc);
				}
				if((nc>9) && (nc<100))
				{
					sprintf(archivo,"video/vid2/Video_00%d.bmp",nc);
				}
				if((nc>99) && (nc<1000))
				{
					sprintf(archivo,"video/vid2/Video_0%d.bmp",nc);
				}
			}
			
			NE_ScreenshotBMP(archivo); // Save the screenshoot
			nc++;                      // Increase the Screenshot number
			NE_WaitForVBL(0);          // Wait for next frame (this is necessary for not making artifacs on the next screenshoot)
		}
		

		if(keysd & KEY_Y)
		{ 
			if(mode3==0)
			{
				mode3=1;
				printf("\x1b[0;0H                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ");
				printf("\x1b[1;1HTi-Ra-Nog 3D Test\n =================");
				printf("\x1b[23;8HPowered By Nitro Engine");
			}
			else 
			{
				mode3=0;
				printf("\x1b[4;1HR: Save Video (So Sloooow).");
				printf("\x1b[5;1HL: Save ScreenShoot.");
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
		
			
		if(keysd & KEY_B)
		{  // if key B pressed use the sine effect; if key pressed another time
			if(mode4==0){ mode4=1; NE_SpecialEffectSet(NE_SINE); }
			else { mode4=0; NE_SpecialEffectSet(0);}
		}
		
		if(keysd & KEY_X)
		{ 
			if(mode5==0) {mode5=1; NE_SpecialEffectSet(NE_NOISE);}
			else { mode5=0; NE_SpecialEffectSet(0);}
		}
	}
}


int main()
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
