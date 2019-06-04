
#include <NEMain.h>
#include "BMP24BIT_bin.h"

NE_Material * Texture;

void Draw3DScene(void)
{
	NE_2DViewInit();

	NE_2DDrawTexturedQuad(0, 0, 128, 128, 0, Texture);
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	lcdMainOnBottom();
	
	Texture = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGBA(Texture,(void*)BMP24BIT_bin,1); //Load texture
	
	scanKeys();NE_WaitForVBL(0); //Wait a bit...
	
	while(1) 
	{
		scanKeys(); 
		touchPosition touch;
		
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
		
		if(keysHeld() & KEY_TOUCH) //Update stylus coordinates when screen is pressed
		{
			touchRead(&touch); 
			NE_TextureDrawingStart(Texture);
			NE_TexturePutPixelRGBA(touch.px>>1, touch.py>>1, RGB15(0,0,31) | BIT(15));
			NE_TextureDrawingEnd();
		}
	}
	
	return 0;
}
