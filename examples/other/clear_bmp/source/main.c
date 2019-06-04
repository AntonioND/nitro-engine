
#include <NEMain.h>
#include "model_bin.h"
#include "background_tex_bin.h"
#include "depth_tex_bin.h"

NE_Camera * Camera;
NE_Model * Model;

int keys;

void Draw3DScene(void)
{
	NE_CameraUse(Camera);

	NE_PolyFormat(31, 0, NE_LIGHT_01, NE_CULL_BACK, 0);
	NE_ModelDraw(Model);

	NE_2DViewInit();
	NE_2DDrawQuad(0,0,100,100,0,NE_White);
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	NE_Init3D();

	// THIS IS NEEDED IF YOU DON'T WANT NITRO ENGINE TO OVERWRITE YOUR CLEAR BMP WITH TEXTURES!!!!!
	NE_TextureSystemReset(0,0,NE_VRAM_AB); 
	
	//For clear bitmap. Copy data into VRAM
	vramSetBankC(VRAM_C_LCD);
	dmaCopy(background_tex_bin,VRAM_C,background_tex_bin_size);
	vramSetBankD(VRAM_D_LCD);
	dmaCopy(depth_tex_bin,VRAM_D,depth_tex_bin_size);

	NE_ClearBMPEnable(true);
	
	Camera = NE_CameraCreate();
	NE_CameraSet(Camera, 1,1,1, 0,0,0, 0,1,0);

	Model = NE_ModelCreate(NE_Static);
	NE_ModelLoadStaticMesh(Model, (u32*)model_bin);

	NE_LightSet(0,NE_Yellow,-1,-1,0);
	NE_LightSet(1,NE_Red,-1,1,0);
	
	
	NE_ClearColorSet(/*Color not used when clear BMP.*/ 0 ,
					/*Id and alpha used.*/31,63);
	
	u8 scrollx = 0, scrolly = 0;
	while(1) 
	{
		scanKeys();
		keys = keysHeld();
		
		NE_ModelRotate(Model,0,2,1);
		
		if(keys & KEY_A) NE_ClearBMPEnable(true);
		if(keys & KEY_B) NE_ClearBMPEnable(false);
		NE_ClearBMPScroll(scrollx,scrolly);
	
		if(keys & KEY_UP) scrolly --;
		if(keys & KEY_DOWN) scrolly ++;
		if(keys & KEY_RIGHT) scrollx ++;
		if(keys & KEY_LEFT) scrollx --;
		
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}
	
	return 0;
}
