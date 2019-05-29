
#include <NEMain.h>
#include "texture_bin.h"
#include "model_bin.h"

NE_Camera * Camera;  //Pointers to the objects...
NE_Model * Model, * Model2, * Model3;
NE_Material * Material;


void Draw3DScene(void)
{
	NE_CameraUse(Camera); //Set camera view and draw objects...

	//Have to use this to use fog...
	NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_BACK,NE_USE_FOG);

	NE_ModelDraw(Model);
	NE_ModelDraw(Model2);
	NE_ModelDraw(Model3);
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_VBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); //Init console and Nitro Engine
	
	Model = NE_ModelCreate(NE_Static);
	Model2 = NE_ModelCreate(NE_Static);
	Model3 = NE_ModelCreate(NE_Static);  //Create space for objects
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();
	
	NE_CameraSet(Camera, -1,2,-1, 1,1,1, 0,1,0); //Set camera coordinates
	
	NE_ModelLoadStaticMesh(Model,(u32*)model_bin);
	NE_ModelLoadStaticMesh(Model2,(u32*)model_bin);  //Load objects...
	NE_ModelLoadStaticMesh(Model3,(u32*)model_bin);
	
	//Load texture
	NE_MaterialTexLoad(Material, GL_RGB, 64, 64,TEXGEN_TEXCOORD,(u8*)texture_bin);
	
	NE_ModelSetMaterial(Model,Material);
	NE_ModelSetMaterial(Model2,Material); //Assign the same material to every model object.
	NE_ModelSetMaterial(Model3,Material);
	
	NE_LightSet(0,NE_White,0,-1,-1); //Set light and vector of light 0
	
	NE_ModelSetCoord(Model, 1, 0, 1);
	NE_ModelSetCoord(Model2, 3, 1, 3); //Set position of every object
	NE_ModelSetCoord(Model3, 7, 2, 7);
	
	u32 Color = NE_Black; //Set color to black (will be used for fog)
	
	u16 depth = 0x7800;
	u8 shift = 3; //Some parameters
	u8 mass = 1;
	
	while(1) 
	{
		scanKeys();
		int keys = keysDown(); //Get keys information
		
		if(keys & KEY_UP) shift ++; if(keys & KEY_DOWN) shift --;
		if(keys & KEY_X)  mass ++; if(keys & KEY_B)  mass --; //Change parameters
		if(keysHeld() & KEY_R)  depth += 0x20; if(keysHeld() & KEY_L)  depth -= 0x20;
		
		shift &= 0xF;
		mass &= 7; //Limit parameters
		depth = (depth & 0x0FFF) + 0x7000;
		
		if(keys & KEY_START) Color = NE_Black; //Change color
		if(keys & KEY_SELECT) Color = NE_White;
		
		NE_FogEnable(shift,Color,31,mass,depth); //Enable/update fog
		
		printf("\x1b[0;0HUp/Down - Shift: %d \nX/B - Mass: %d  \nL/R - Depth: 0x%x   \nSelect/Start - Change color.",
				shift, mass, depth);
		
		NE_Process(Draw3DScene); //Draw scene...
		NE_WaitForVBL(0);
	}
	
	return 0;
}
