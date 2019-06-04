
#include <NEMain.h>
#include "model_bin.h"

NE_Camera * Camera; //Pointers to objects...
NE_Model * Model[16];

void Draw3DScene(void)
{
	NE_CameraUse(Camera); //Use camera and draw all objects.
	u8 a; for(a = 0; a < 16; a++) NE_ModelDraw(Model[a]);

	//Get some information AFTER drawing but BEFORE returning from the function.
	printf("\x1b[0;0HPolygon RAM: %d   \nVertex RAM: %d   ",NE_GetPolygonCount(), NE_GetVertexCount());
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit();
	
	u8 a; for(a = 0; a < 16; a++) Model[a] = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate(); //Create space for everything.
	
	NE_CameraSet(Camera, -3.5,1.5,1.25,  0,1.5,1.25,  0,1,0); //Set camera.
	
	for(a = 0; a < 16; a++) NE_ModelLoadStaticMesh(Model[a],(u32*)model_bin); //Load model.
	
	//Set up light
	NE_LightSet(0,NE_Yellow,0,-0.5,-0.5);
	
	NE_ShadingEnable(true); //Enable shading
	
	for(a = 0; a < 16; a++) //Set start coordinates/rotation for models using random formules...
	{
		NE_ModelSetRot(Model[a], a, a*30, a* 20);
		NE_ModelSetCoord(Model[a], 0, a % 4, a / 4);
	}
	
	while(1) 
	{
		//Rotate every model using random formules :P
		for(a = 0; a < 16; a++) NE_ModelRotate(Model[a], -a, a%5, 5-a);
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
