
#include <NEMain.h> // Include for Nitro Engine

#include "BMP8BIT_bin.h" //Include model and texture

NE_Material * Material;
NE_Palette * Palette;

void Draw3DScene(void)
{
	NE_2DViewInit();
	NE_2DDrawTexturedQuad(0,0,128,128,0,Material);
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); // This is needed for some things
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D(); //Init Nitro Engine, normal 3D mode.
	NE_SwapScreens();
	
	Material = NE_MaterialCreate();
	Palette = NE_PaletteCreate();
	
	NE_MaterialTexLoadBMPtoRGB256(Material, Palette,(void*)BMP8BIT_bin, 1); //Load texture
	
	NE_PaletteModificationStart(Palette);
	NE_PaletteRGB256SetColor(254,RGB15(0,0,31));
	NE_PaletteModificationEnd();
	
	touchPosition touch;
	
	while(1)   
	{
		scanKeys();
		touchRead(&touch);
		
		
		
		NE_Process(Draw3DScene); //Draw 3D scene
		
		NE_WaitForVBL(0);
		
		if(keysHeld() & KEY_TOUCH)
		{
			NE_TextureDrawingStart(Material);
			NE_TexturePutPixelRGB256(touch.px>>1 ,touch.py>>1 , 254);
			NE_TextureDrawingEnd();
		}
	}
	
	return 0;
}
