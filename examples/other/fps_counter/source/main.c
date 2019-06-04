
#include <NEMain.h>
#include <time.h>  //Needed for getting the time
#include "nitrocat_bin.h"
#include "texture_bin.h"

//----------------------------------------------
// Note: There are better ways to do this, for
// example, using the VBL interrupt to check
// if the second has changed.
//----------------------------------------------


NE_Camera * Camera;     //We use pointers to waste less ram if we finish 3D mode.
NE_Model * Model; 
NE_Material * Material; 

void Draw3DScene(void)
{
	NE_CameraUse(Camera);   //Set camera
	NE_ModelDraw(Model); //Draw model...
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //This is needed for special screen effects
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();  //Init Nitro Engine in normal 3D mode
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); 

	Model = NE_ModelCreate(NE_Static);  //Create space for the things we will use.
	Camera = NE_CameraCreate();      //If you don't do this, the game will crash.
	Material = NE_MaterialCreate();
	
	//Set coordinates for the camera
	NE_CameraSet(Camera, -8,0,0, //Position
		                  0,0,0, //Look at
						  0,1,0);//Up direction
	
	//Load mesh from RAM and assign it to the object "Model".
	NE_ModelLoadStaticMesh(Model,(u32*)nitrocat_bin);
	//Load a RGB texture from RAM and assign it to "Material".
	NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD, (u8*) texture_bin);
	//Assign texture to model...
	NE_ModelSetMaterial(Model, Material);
	
	//We set a light and its color
	NE_LightSet(0,NE_White,-0.5,-0.5,-0.5);
	
	u8 fpscount = 0;  //To hold fps count
	u8 oldsec = 0; // This is used to see if second has changed
	u8 seconds = 0;
	
	while(1) 
	{
		time_t unixTime = time(NULL);  //Update time.
		struct tm* timeStruct = gmtime((const time_t *)&unixTime);
		seconds = timeStruct->tm_sec;
		
		if(seconds != oldsec) //If new second
		{
			oldsec = seconds; //Reset fps count and print current
			printf("\x1b[10;0HFPS: %d", fpscount);
			fpscount = 0;
		}
		
		scanKeys();  //Get keys information
		int keys = keysHeld();
		
		printf("\x1b[0;0HPad: Rotate."); //Print text.
		
		if(keys & KEY_UP) NE_ModelRotate(Model, 0, 0, 2);  //Rotate model using the pad
		if(keys & KEY_DOWN) NE_ModelRotate(Model, 0, 0, -2);
		if(keys & KEY_RIGHT) NE_ModelRotate(Model, 0, 2, 0);
		if(keys & KEY_LEFT) NE_ModelRotate(Model, 0, -2, 0);
		
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0); //Wait for next frame
		fpscount++; //Increase frame count
	}
	
	return 0;
}
