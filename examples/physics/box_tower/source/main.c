
#include <NEMain.h>
#include "model_bin.h"

NE_Camera * Camara;
NE_Model * Model[6];
NE_Physics * Physics[6];

void Draw3DScene(void)
{
	NE_CameraUse(Camara);

	NE_PolyFormat(31, 0, NE_LIGHT_0,NE_CULL_BACK, 0);
	int a; for (a = 0; a < 5; a++) NE_ModelDraw(Model[a]);

	NE_PolyFormat(31, 0, NE_LIGHT_1,NE_CULL_BACK, 0);
	NE_ModelDraw(Model[5]);
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	
	Camara = NE_CameraCreate(); //New camera...
	NE_CameraSet(Camara, -9,7,5,  0,6,0,  0,1,0);
	
	int a;
	for (a = 0; a < 6; a++) //Create objects
	{
		Model[a] = NE_ModelCreate(NE_Static);
		Physics[a] = NE_PhysicsCreate(NE_BoundingBox);
		NE_ModelLoadStaticMesh(Model[a], (u32*)model_bin);
		NE_PhysicsSetModel(Physics[a], (void*)Model[a]);
		NE_PhysicsSetSize(Physics[a],1,1,1);
	}
	
	NE_PhysicsEnable(Physics[5],false); //Enable only the ones we will move
	
	NE_ModelSetCoord(Model[0],0,2,0);
	NE_ModelSetCoord(Model[1],0,4,0);
	NE_ModelSetCoord(Model[2],0,6,0); //Object coordinates
	NE_ModelSetCoord(Model[3],0,8,0);
	NE_ModelSetCoord(Model[4],0,10,0);
	NE_ModelSetCoord(Model[5],0,0,0);

	NE_PhysicsSetGravity(Physics[0], 0.001);
	NE_PhysicsSetGravity(Physics[1], 0.001); //Set gravity
	NE_PhysicsSetGravity(Physics[2], 0.001);
	NE_PhysicsSetGravity(Physics[3], 0.001);
	NE_PhysicsSetGravity(Physics[4], 0.001);
	
	NE_PhysicsOnCollition(Physics[0],NE_ColBounce); //Tell the engine what to do if
	NE_PhysicsOnCollition(Physics[1],NE_ColBounce); //there is a collition
	NE_PhysicsOnCollition(Physics[2],NE_ColBounce);
	NE_PhysicsOnCollition(Physics[3],NE_ColBounce);
	NE_PhysicsOnCollition(Physics[4],NE_ColBounce);
	
	NE_PhysicsSetBounceEnergy(Physics[0],100); //Set percent of energy kept after a bounce
	NE_PhysicsSetBounceEnergy(Physics[1],100); //Default is 50, 100 = no energy lost.
	NE_PhysicsSetBounceEnergy(Physics[2],100);
	NE_PhysicsSetBounceEnergy(Physics[3],100);
	NE_PhysicsSetBounceEnergy(Physics[4],100);
	
	//Light
	NE_LightSet(0,NE_Green,-1,-1,0);
	NE_LightSet(1,NE_Blue,-1,-1,0);
	
	//Background
	NE_ClearColorSet(NE_Red,31,63);
	
	while(1) 
	{
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(NE_UPDATE_PHYSICS);
	}
	
	return 0;
}
