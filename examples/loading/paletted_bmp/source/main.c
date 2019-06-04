
#include <NEMain.h> // Include for Nitro Engine

#include "BMP8BIT_bin.h" //Include model and textures
#include "BMP4BIT_bin.h"
#include "model_bin.h"

NE_Camera * Camera;
NE_Model * Model; //Pointer to objects
NE_Material * Material4, * Material8;
NE_Palette * Palette4, * Palette8;

void Draw3DScene(void)
{
	NE_PolyFormat(31,0,NE_LIGHT_0,NE_CULL_NONE,0);

	NE_CameraUse(Camera); //Set camera position and vector
	NE_ModelDraw(Model); //Draw model
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); // This is needed for some things
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D(); //Init Nitro Engine, normal 3D mode.
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit();
	
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate(); //Create space for every object
	Material8 = NE_MaterialCreate();
	Material4 = NE_MaterialCreate();
	Palette8 = NE_PaletteCreate();
	Palette4 = NE_PaletteCreate();

	NE_CameraSet(Camera, -2,-2,-2, 0,0,0, 0,1,0); //Set camera coordinates
	
	NE_ModelLoadStaticMesh(Model,(u32*)model_bin); //Load model
	
	//Load textures, this one's color 0 is transparent
	NE_MaterialTexLoadBMPtoRGB256(Material4, Palette4,(void*)BMP4BIT_bin, 1); 
	//This is completely opaque
	NE_MaterialTexLoadBMPtoRGB256(Material8, Palette8,(void*)BMP8BIT_bin, 0);

	NE_ModelSetMaterial(Model,Material8); //Assign material to model
	
	NE_LightSet(0,NE_White,0,-1,-1); //Set light 0 color and vector
	
	NE_ModelScale(Model, 0.3, 0.3, 0.3); //Original cube is too big to fit, we scale it down.
	
	printf("Press A/B to change texture");
	
	while(1) 
	{
		scanKeys();
		
		NE_ModelRotate(Model, 1, 2, 0); //Increase model rotation
		
		if(keysHeld() & KEY_A) NE_ModelSetMaterial(Model,Material8);
		else if(keysHeld() & KEY_B) NE_ModelSetMaterial(Model,Material4);
		
		NE_Process(Draw3DScene); //Draw 3D scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
