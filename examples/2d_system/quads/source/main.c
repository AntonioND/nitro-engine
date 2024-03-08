// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022, 2024
//
// This file is part of Nitro Engine

#include <NEMain.h>

#define NUM_QUADS 40

typedef struct {
   bool enabled;
   int x1, y1,  x2, y2;
   int alpha, id;
   int color;
} quad_t;

quad_t Quad[NUM_QUADS];

bool enable_alpha = false;

void UpdateQuads(void)
{
    for (int i = 0; i < NUM_QUADS; i++)
    {
        if (!Quad[i].enabled)
        {
            // Always recreate quads if not enabled
            Quad[i].enabled = true;
            Quad[i].x1 = rand() & 255;
            Quad[i].x2 = rand() & 255;
            Quad[i].y1 = rand() % 192;
            Quad[i].y2 = rand() % 192;
            Quad[i].alpha = (rand() % 30) + 1;
            Quad[i].id = rand() & 63;
            Quad[i].color = rand() & 0xFFFF;
        }
        else
        {
            // Disable quads randomly
            if ((rand() & 31) == 31)
                Quad[i].enabled = false;
        }
    }
}

void Draw3DScene(void)
{
    NE_2DViewInit();

    NE_PolyFormat(31, 0,0,NE_CULL_BACK,0);

    for (int i = 0; i < NUM_QUADS; i++)
    {
        if (!Quad[i].enabled)
            continue;

        if (enable_alpha)
            NE_PolyFormat(Quad[i].alpha, Quad[i].id, 0, NE_CULL_NONE, 0);

        NE_2DDrawQuad(Quad[i].x1, Quad[i].y1, Quad[i].x2, Quad[i].y2, i,
                      Quad[i].color);
    }
}

int main(void)
{
    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    // Init console in non-3D screen
    consoleDemoInit();

    printf("A: Alpha");

    while (1)
    {
        NE_WaitForVBL(0);

        scanKeys();
        uint16_t kheld = keysHeld();
        if (kheld & KEY_A)
            enable_alpha = true;
        else
            enable_alpha = false;

        UpdateQuads();

        NE_Process(Draw3DScene);
    }

    return 0;
}
