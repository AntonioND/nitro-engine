
#include <NEMain.h>

NE_Camera * Camera; //Pointer to camera.

void Draw3DScene(void)
{
	NE_CameraUse(Camera); //Use camera and draw polygon.

	NE_PolyBegin(GL_QUAD); //Begin drawing
		
		NE_PolyColor(NE_Red); // Set next vertices color
		NE_PolyVertex(-1, 1, 0); // Put new vertex
		
		NE_PolyColor(NE_Green);
		NE_PolyVertex(-1,-1, 0);
		
		NE_PolyColor(NE_Yellow);
		NE_PolyVertex( 1,-1, 0);
		
		NE_PolyColor(NE_Blue);
		NE_PolyVertex( 1, 1, 0);
		
	NE_PolyEnd(); //This is completely USELESS. 
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); 

	Camera = NE_CameraCreate(); //Create space for the camera
	
	NE_CameraSet(Camera, 0,0,2,  0,0,0,  0,1,0); //Set camera.
	
	while(1) 
	{
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
