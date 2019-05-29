
#include <NEMain.h>


void Draw3DScene(void)
{

}

void Draw3DScene2(void)
{

}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_VBLANK, NE_HBLFunc);

	NE_InitDual3D();
	
	while(1) 
	{
		scanKeys();
		
		
		NE_ProcessDual(Draw3DScene, Draw3DScene2);
		NE_WaitForVBL(0);
	}
	
	return 0;
}
