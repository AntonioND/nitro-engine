
#include <NEMain.h>
#include "nitrocat_bin.h"
#include "texture_bin.h"

NE_Camera * Camera;     //We use pointers to use less ram if we are not using 3D, but you can 
NE_Model * Model;       //create the structs directly and you won't have to use Create/Delete
NE_Material * Material; //functions.

int shading, alpha, id;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);   //Set camera

	NE_PolyFormat(alpha,id,NE_LIGHT_0,NE_CULL_BACK,shading);	//Set polygon format

	NE_ModelDraw(Model); //Draw model...
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //This is needed for special screen effects
	irqSet(IRQ_VBLANK, NE_HBLFunc);
	
	NE_Init3D();  //Init Nitro Engine in normal 3D mode
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); //This is needed to print text

	Model = NE_ModelCreate(NE_Static);  //Create space for the things we will use
	Camera = NE_CameraCreate();
	Material = NE_MaterialCreate();
	
	NE_CameraSet(Camera, -8,0,0, 0,0,0, 0,1,0); //Set coordinates for the camera
	
	//Load mesh from RAM and assign it to the object "Model".
	NE_ModelLoadStaticMesh(Model,(u32*)nitrocat_bin);
	//Load texture from RAM and assign it to "Material".
	NE_MaterialTexLoad(Material, GL_RGB, 128, 128, TEXGEN_TEXCOORD, (u8*) texture_bin);
	//Assign texture to model...
	NE_ModelSetMaterial(Model, Material);
	
	NE_MaterialSetPropierties(Material, //Set some propierties to Material
						RGB15(24,24,24), //diffuse
						RGB15(8,8,8), //ambient
						RGB15(0,0,0), //specular
						RGB15(0,0,0), //emission
						false, false); //vtxcolor, useshininess);

	NE_LightSet(0,NE_White,-0.5,-0.5,-0.5); //We set a light and its color
	
	NE_ShadingEnable(true); //This enables shading (you can choose normal or toon).
	NE_OutliningEnable(true); //This enables outlining in all polygons, so be careful

	//We set the second outlining color to red.
	//This will be used by polygons with ID 8 - 15.
	NE_OutliningSetColor(1, NE_Red);      
	
	while(1) 
	{
		scanKeys();  //Get keys information
		int keys = keysHeld();
		
		printf("\x1b[0;0HPad: Rotate.\nA: Toon shading.\nB: Change alpha value.\nY: Wireframe mode (alpha = 0)\nX: Outlining.");
		
		if(keys & KEY_UP) NE_ModelRotate(Model, 0, 0, 2);  //Rotate model using the pad
		if(keys & KEY_DOWN) NE_ModelRotate(Model, 0, 0, -2);
		if(keys & KEY_RIGHT) NE_ModelRotate(Model, 0, 2, 0);
		if(keys & KEY_LEFT) NE_ModelRotate(Model, 0, -2, 0);
		
		if(keys & KEY_A) shading = NE_TOON_SHADING; //Change shading type
		else shading = NE_MODULATION;
		
		if(keys & KEY_B) alpha = 15;  // Transparent
		else if(keys & KEY_Y) alpha = 0;  //Wireframe
		else alpha = 31; //Opaque
		
		if(keys & KEY_X) id = 8; //Change id co change outlining color
		else id = 0;
		
		NE_Process(Draw3DScene); //Draws scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
