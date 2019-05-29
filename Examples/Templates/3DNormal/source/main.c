
#include <NEMain.h>

void Draw3DScene(void)
{

}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	NE_Init3D();


	while(1) 
	{
		scanKeys();
		
		
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}
	
	return 0;
}
