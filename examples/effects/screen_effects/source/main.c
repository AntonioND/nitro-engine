
#include <NEMain.h>
#include "nitrocat_bin.h"

NE_Camera * Camera; //Create pointers to objects...
NE_Model * Model;


void Draw3DScene(void)
{
	NE_ClearColorSet(NE_Red, 31,63); //Set rear plane color

	NE_CameraUse(Camera);
	NE_ModelDraw(Model);
}

void Draw3DScene2(void)
{
	NE_ClearColorSet(NE_Green, 31,63); //Set rear plane color

	NE_CameraUse(Camera);
	NE_ModelDraw(Model);
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Needed for special effects
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_InitDual3D();  //Init dual 3D mode and console
	NE_InitConsole();
	
	Model = NE_ModelCreate(NE_Static); //Create space for objects...
	Camera = NE_CameraCreate();
	
	NE_CameraSet(Camera, -8,0,0, 0,0,0, 0,1,0); //Set camera
	
	NE_ModelLoadStaticMesh(Model,(u32*)nitrocat_bin); //Load model...
	
	NE_LightSet(0,NE_White,-0.5,-0.5,-0.5); //Set light color and direction
	
	NE_ShadingEnable(true); //Enable shading...
	
	//NE_SpecialEffectNoiseConfig(31);

	//NE_SpecialEffectSineConfig(3,8);
	
	while(1) 
	{
		scanKeys();
		int keys = keysHeld(); //Get keys information
		int kdown = keysDown();
		
		printf("\x1b[0;0HPad: Rotate.\nA: Sine effect.\nB: Noise effect.\nX: Deactivate effects.");
		printf("\nL/R: Pause/Unpause.");
		
		if(keys & KEY_UP) NE_ModelRotate(Model, 0, 0, 2);
		if(keys & KEY_DOWN) NE_ModelRotate(Model, 0, 0, -2); //Rotate model
		if(keys & KEY_RIGHT) NE_ModelRotate(Model, 0, 2, 0);
		if(keys & KEY_LEFT) NE_ModelRotate(Model, 0, -2, 0);
		
		if(kdown & KEY_B) NE_SpecialEffectSet(NE_NOISE); //Activate effects
		if(kdown & KEY_A) NE_SpecialEffectSet(NE_SINE);
		if(kdown & KEY_X) NE_SpecialEffectSet(0);	 //Deactivate effects
		
		if(kdown & KEY_L) NE_SpecialEffectPause(true);
		if(kdown & KEY_R) NE_SpecialEffectPause(false);
		
		NE_ProcessDual(Draw3DScene, Draw3DScene2); //Draw 3D scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
