
#include <NEMain.h>
#include "bird_final_nea_bin.h"
#include "bird_final_bmp_bin.h"

NE_Camera * Camara;
NE_Model * Model;
NE_Material * Texture;

void Draw3DScene(void)
{
	NE_PolyFormat(31,0,NE_LIGHT_0,NE_CULL_BACK,0);

	NE_CameraUse(Camara);

	NE_ModelDraw(Model);
}

int main()
{	
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_InitConsole();
	
	Camara = NE_CameraCreate();
	Model = NE_ModelCreate(NE_Animated);
		
	NE_ModelAnimStart(Model, 0, 0, 20, NE_ANIM_LOOP, 3);
	NE_ModelLoadNEA(Model, (u32*)bird_final_nea_bin);
	NE_CameraSet(Camara, 7,2,-4, 0,0,0, 0,1,0);
	Texture = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGBA(Texture,(void*)bird_final_bmp_bin,0); //Load texture
	
	NE_ModelSetMaterial(Model,Texture);
	
	NE_LightSet(0,NE_White,0,-0.9,0);
	NE_ClearColorSet(NE_Black,31,63);
	
	while(1) 
	{
		scanKeys();
		int keys = keysHeld();
		
		if(keys & KEY_A) NE_ModelAnimInterpolate(Model, true);
		if(keys & KEY_B) NE_ModelAnimInterpolate(Model, false);
		
		if(keys & KEY_RIGHT) NE_ModelRotate(Model,0,2,0);
		if(keys & KEY_LEFT) NE_ModelRotate(Model,0,-2,0);
		if(keys & KEY_UP) NE_ModelRotate(Model,0,0,2);
		if(keys & KEY_DOWN) NE_ModelRotate(Model,0,0,-2);
		
		printf("\x1b[0;0HA/B: Interpolation.\nCPU%%: %d  \nFrame: %d  ",NE_GetCPUPercent(),NE_ModelAnimGetFrame(Model));
		
		NE_Process(Draw3DScene);
		NE_WaitForVBL(NE_UPDATE_ANIMATIONS);
	}
	
	return 0;
}




