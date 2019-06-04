
#include <NEMain.h>
#include "BMP24BIT_bin.h"


NE_Camera * Camera; //Pointer to camera.
NE_Material * Texture;

void Draw3DScene(void)
{
	NE_CameraUse(Camera); //Use camera and draw polygon.

	NE_MaterialUse(Texture); //This set material's color to drawing color (default = white).

	NE_PolyBegin(GL_QUAD); //Begin drawing
		
		NE_PolyColor(NE_Red);    // Set next vertices color   	| This is a bit messy, but it works.
		NE_PolyTexCoord(0, 0);  // Texture coordinates       	| In fact, you shouldn't use this 
		NE_PolyVertex(-1, 1, 0); // Put new vertex            	| functions to draw.
		
		NE_PolyColor(NE_Blue);
		NE_PolyTexCoord(0, 64);
		NE_PolyVertex(-1,-1, 0);
		
		NE_PolyColor(NE_Green);
		NE_PolyTexCoord(64, 64);
		NE_PolyVertex( 1,-1, 0);
		
		NE_PolyColor(NE_Yellow);
		NE_PolyTexCoord(64, 0);
		NE_PolyVertex( 1, 1, 0);
		
	NE_PolyEnd(); //This is completely USELESS. 
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	
	Camera = NE_CameraCreate(); //Create space for the camera
	Texture = NE_MaterialCreate();
	
	NE_CameraSet(Camera, 0,0,2,  0,0,0,  0,1,0); //Set camera.
	
	NE_MaterialTexLoadBMPtoRGBA(Texture,(void*)BMP24BIT_bin,1); //Load texture
	
	while(1) 
	{
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
