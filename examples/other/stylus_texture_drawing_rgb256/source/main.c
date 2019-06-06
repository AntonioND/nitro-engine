// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "bmp8bit_bin.h"

NE_Material *Material;
NE_Palette *Palette;

void Draw3DScene(void)
{
	NE_2DViewInit();
	NE_2DDrawTexturedQuad(0, 0,
			      128, 128,
			      0, Material);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init Nitro Engine, normal 3D mode, and move the 3D screen to the
	// bottom screen
	NE_Init3D();
	NE_SwapScreens();

	// Allocate objects
	Material = NE_MaterialCreate();
	Palette = NE_PaletteCreate();

	// Load texture
	NE_MaterialTexLoadBMPtoRGB256(Material, Palette, (void *) bmp8bit_bin,
				      1);

	// Modify color 254 of the palette so that we can use it to draw with a
	// known color
	NE_PaletteModificationStart(Palette);
	NE_PaletteRGB256SetColor(254, RGB15(0, 0, 31));
	NE_PaletteModificationEnd();

	touchPosition touch;

	while (1) {
		scanKeys();
		touchRead(&touch);

		NE_Process(Draw3DScene);

		NE_WaitForVBL(0);

		if (keysHeld() & KEY_TOUCH) {
			NE_TextureDrawingStart(Material);
			NE_TexturePutPixelRGB256(touch.px >> 1 ,touch.py >> 1 ,
						 254);
			NE_TextureDrawingEnd();
		}
	}

	return 0;
}
