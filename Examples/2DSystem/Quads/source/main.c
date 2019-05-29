
#include <NEMain.h>

#define NUM_QUADS 40

typedef struct {
	bool enabled;
	int x1, y1,  x2, y2;
	int alpha, id;
	int color;
} _QUAD_;
	
_QUAD_ Quad[NUM_QUADS];

int kheld;

void UpdateQuads(void)
{
	int a;

	for(a = 0; a < NUM_QUADS; a++)
	{
		if(!Quad[a].enabled)
		{
			Quad[a].enabled = true;
			Quad[a].x1 = rand() & 255;
			Quad[a].x2 = rand() & 255;
			Quad[a].y1 = rand() % 192;
			Quad[a].y2 = rand() % 192;
			Quad[a].alpha = (rand() % 30) + 1;
			Quad[a].id = rand() & 63;
			Quad[a].color = rand() & 0xFFFF;
		}
		else if((rand() & 31) == 31) Quad[a].enabled = false;
	}
}


void Draw3DScene(void)
{
	NE_2DViewInit();

	NE_PolyFormat(31, 0,0,NE_CULL_BACK,0);

	int a;
	for(a = 0; a < NUM_QUADS; a++) if(Quad[a].enabled)
	{
		if(kheld & KEY_A) NE_PolyFormat(Quad[a].alpha,Quad[a].id,0,NE_CULL_NONE,0);
		NE_2DDrawQuad(Quad[a].x1, Quad[a].y1, Quad[a].x2, Quad[a].y2, a, Quad[a].color);
	}
}

int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc); //Used to control some things.
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit();	
	
	printf("A: Alpha");
	
	while(1) 
	{
		scanKeys();
		kheld = keysHeld();		
		
		UpdateQuads();
		
		NE_Process(Draw3DScene); //Draw scene
		NE_WaitForVBL(0);
	}
	
	return 0;
}
