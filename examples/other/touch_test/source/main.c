
#include <NEMain.h>
#include "model_bin.h"

NE_Camera * Camera; //Pointers to objects...
NE_Model * Model[10];

int distancetocamera[10];

int object_touched;

int rotz, roty;

int keys;

void Draw3DScene(void)
{
	NE_CameraUse(Camera); //Use camera and draw all objects.
	NE_ViewRotate(0,roty,rotz);

	u8 a;  //Draw everything
	for(a = 0; a < 10; a++) 
	{
		if(a == object_touched) NE_PolyFormat(31,0,NE_LIGHT_1,NE_CULL_BACK,0);
		else NE_PolyFormat(31,0,NE_LIGHT_0,NE_CULL_BACK,0);
		
		NE_ModelDraw(Model[a]);
	}

	if(keys & KEY_TOUCH)
	{
		//-----------------------------------------------------------------------
		NE_TouchTestStart(); //Get the information
		for (a = 0; a < 10; a++) 
			{
			NE_TouchTestObject(); //This isn't drawn, used for testing. You should use a less
			NE_ModelDraw(Model[a]); //detailed model than original to send less commands
			distancetocamera[a] = NE_TouchTestResult(); //to the GPU.
			}
		NE_TouchTestEnd();
		//-----------------------------------------------------------------------
	}
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_SwapScreens(); //3D screen to touch screen
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); 
	
	u32 a; for(a = 0; a < 10; a++) Model[a] = NE_ModelCreate(NE_Static);

	Camera = NE_CameraCreate(); //Create space for everything.
	NE_CameraSet(Camera, -4,0,0,  0,0,0,  0,1,0); //Set camera.
	for(a = 0; a < 10; a++) NE_ModelLoadStaticMesh(Model[a],(u32*)model_bin); //Load model.
	//Set up light
	NE_LightSet(0,NE_Yellow,0,-0.5,-0.5);
	NE_LightSet(1,NE_Red,0,-0.5,-0.5);
	
	NE_ShadingEnable(true); //Enable shading
	
	swiWaitForVBlank(); 
	swiWaitForVBlank(); 
	swiWaitForVBlank(); 
	swiWaitForVBlank();
	printf("Press any key to start...");
	
	u8 framecount = 0;
	
	while(1)
	{
		
		if(framecount < 30) printf("\x1b[1;0H_");
		else printf("\x1b[1;0H ");
		
		if(framecount == 60) framecount = 0;
		
		framecount++;
		
		
		scanKeys();
		
		for(a = 0; a < 10; a++) //Set start coordinates/rotation for models using random formules...
		{
			NE_ModelSetCoordI(Model[a], (rand() & (inttof32(3) - 1)) - floattof32(1.5), 
					(rand() & (inttof32(3) - 1)) - floattof32(1.5) , 
					(rand() & (inttof32(3) - 1)) - floattof32(1.5));
		}
		
		if(keysHeld()) break;
		
		swiWaitForVBlank();
	}
	
	printf("\x1b[0;0H                         ");
	printf("\x1b[1;0H ");
	
	printf("\x1b[0;0HNote: If two objects overlap,\nit may fail to diferenciate\nwhich is closer to the camera.");
	printf("\x1b[22;0HPAD: Rotate.");
	printf("\x1b[23;0HSTART: New positions.");
	
	while(1) 
	{
		scanKeys();
		keys = keysHeld();
		
		if(keys & KEY_RIGHT) roty --;
		if(keys & KEY_LEFT) roty ++; //Rotate view
		if(keys & KEY_UP) rotz ++;
		if(keys & KEY_DOWN) rotz --;		
		
		if(keysDown() & KEY_START)
		{
			for(a = 0; a < 10; a++) //Set start coordinates/rotation for models using random formules...
			{
				NE_ModelSetCoordI(Model[a], (rand() & (inttof32(3) - 1)) - floattof32(1.5), 
						(rand() & (inttof32(3) - 1)) - floattof32(1.5) , 
						(rand() & (inttof32(3) - 1)) - floattof32(1.5));
			}
		}
		
		object_touched = -1; //No object being touched, let's test if we're wrong ;)
		
		if(keys & KEY_TOUCH)
		{
			//-----------------------------------------------------------------------
			//This is the part where I check which is the object being touched
			
			int mindistance = GL_MAX_DEPTH; //Max distance
			
			int b;
			for(b = 0; b < 10; b++) if(distancetocamera[b] >= 0) //If touched
			{
				if(distancetocamera[b] < mindistance) //If it is near...
				{
					object_touched = b;
					mindistance = distancetocamera[b];
				}
			}
			//-----------------------------------------------------------------------
		}
		else
		{
			int b; //Reset distances if screen is not being touched
			for(b = 0; b < 10; b++) distancetocamera[b] = GL_MAX_DEPTH;
		}
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
