
#include <NEMain.h>

//------------------------------------------------------------------------------------
// DON'T FORGET TO COMPILE NITRO ENGINE WITH NE_DEBUG DEFINED OR THIS WON'T WORK!!!!!
//------------------------------------------------------------------------------------

void Draw3DScene(void)
{
	NE_LightOff(100);  // Let's generate some error messages... :P
	NE_CameraSetI(NULL,1,1,1,1,1,1,1,1,1);
	NE_PolyFormat(100,120,0,0,0);
}

void error_handler(const char * text)
{
	printf(text); // Simple handler, you could write this to a file, for example.
	printf("\n");
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); 
	
	NE_DebugSetHandler(error_handler); //Sets a custom error handler
	
	//NE_DebugSetHandlerConsole(); -> This would call NE_InitConsole and output all debug
	//                                messages to the console.
	
	while(1)
	{
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
