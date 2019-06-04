
#include <NEMain.h>
#include <fat.h> //INCLUDE LIBFAT!!

NE_Camera * Camera;
NE_Model * Model;  //Pointers...
NE_Material * OpaqueMaterial, * TransparentMaterial;


void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	NE_PolyFormat(31,0,NE_LIGHT_0,NE_CULL_NONE,0);
	NE_ModelDraw(Model);  //Set camera and draw model...
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); // Used for special effects...
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	fatInitDefault(); //Init FAT, console and Nitro Engine
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit();
	
	Model = NE_ModelCreate(NE_Static);
	Camera = NE_CameraCreate(); //Create space for objects...
	OpaqueMaterial = NE_MaterialCreate();
	TransparentMaterial = NE_MaterialCreate();
	
	NE_CameraSet(Camera, -2,-2,-2, 0,0,0, 0,1,0); //Set camera...
	
	NE_ModelLoadStaticMeshFAT(Model,"NitroEngine/cube.bin"); //Load things from FAT...
	NE_FATMaterialTexLoadBMPtoRGBA(TransparentMaterial, "NitroEngine/BMP16BIT_X1RGB5.bmp", 1);
	NE_FATMaterialTexLoadBMPtoRGBA(OpaqueMaterial, "NitroEngine/BMP24BIT.bmp", 0);
	NE_ModelSetMaterial(Model,TransparentMaterial); //Assign ONE material to the model.
	
	//Set up light
	NE_LightSet(0,NE_White,0,-1,-1);
	
	//Background color...
	NE_ClearColorSet(NE_Gray,31,63);
	
	NE_ModelScale(Model, 0.3, 0.3, 0.3); //Reduce size of the cube,
	                                      //original is 7.5 x 7.5 x 7.5.
	
	while(1) 
	{
		scanKeys(); //Get keys information...
		int keys = keysDown();
		
		if(keys & KEY_B) NE_ModelSetMaterial(Model,OpaqueMaterial); //Change material if
		if(keys & KEY_A) NE_ModelSetMaterial(Model,TransparentMaterial); //pressed
		
		//Make screenshot of 3D screen
		if(keys & KEY_START) NE_ScreenshotBMP("NitroEngine/Screenshot.BMP"); 
		
		printf("\x1b[0;0HA/B: Change material.\n\nStart: Screenshot.");
		
		//Increase rotation, you can't get the rotation angle after this. If you want to know 
		//always the angle, you should use NE_ModelSetRot.
		NE_ModelRotate(Model, 1, 2, 0);
		
		NE_Process(Draw3DScene); //Draw scene...
		NE_WaitForVBL(0);
	}
	
	return 0;
}
