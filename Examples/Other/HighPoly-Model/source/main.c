
#include <NEMain.h>
#include "ball_bin.h"
#include "texture_bin.h"

NE_Camera * Camera; //Create pointers to objects...
NE_Model * Model;
NE_Material * Material;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	int keys = keysHeld(); //Get keys information

	if(keys & KEY_A) NE_PolyFormat(0,0,NE_LIGHT_ALL,NE_CULL_BACK,0);
	else NE_PolyFormat(31,0,NE_LIGHT_ALL,NE_CULL_BACK,0);
		
	NE_ModelDraw(Model);

	printf("\x1b[5;0HPolygon count: %d   \nVertex count: %d   ",NE_GetPolygonCount(),NE_GetVertexCount());
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Needed for special effects
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();  //Init 3D mode.
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); 
	
	Model = NE_ModelCreate(NE_Static); //Create space for objects...
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();
	
	NE_CameraSet(Camera, 2,0,0, 0,0,0, 0,1,0); //Set camera
	
	NE_ModelLoadStaticMesh(Model,(u32*)ball_bin); //Load model...
	NE_MaterialTexLoadBMPtoRGBA(Material, (void*) texture_bin, 0);
	NE_ModelSetMaterial(Model, Material);
	
	NE_MaterialSetPropierties(Material, //Set some propierties to Material
						RGB15(31,31,31), //diffuse
						RGB15(0,0,0), //ambient
						RGB15(0,0,0), //specular
						RGB15(0,0,0), //emission
						false, false); //vtxcolor, useshininess);

	//Set light color and direction
	NE_LightSet(0,NE_White,0,1,0);
	NE_LightSet(1,NE_Blue,0,-1,0);
	NE_LightSet(2,NE_Red,1,0,0);
	NE_LightSet(3,NE_Green,-1,0,0);
	
	while(1) 
	{
		scanKeys();
		int keys = keysHeld(); //Get keys information
		
		printf("\x1b[0;0HPad: Rotate.\nA: Set wireframe mode.");
		
		if(keys & KEY_UP) NE_ModelRotate(Model, 0, 0, 2);
		if(keys & KEY_DOWN) NE_ModelRotate(Model, 0, 0, -2); //Rotate model
		if(keys & KEY_RIGHT) NE_ModelRotate(Model, 0, 2, 0);
		if(keys & KEY_LEFT) NE_ModelRotate(Model, 0, -2, 0);
		
		NE_Process(Draw3DScene); //Draw 3D scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
