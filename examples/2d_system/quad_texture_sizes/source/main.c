// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "BMP24BIT_big_bin.h"
#include "BMP24BIT_medium_bin.h"
#include "BMP24BIT_small_bin.h"

// This example shows that Nitro Engine fixes the strange texture mapping of the
// DS of any size of texture. The magic part is in NE_2DDrawXXXXXX() source. The
// bad thing is... that it does only work with those functions.

NE_Material *Material_small, *Material_medium, *Material_big;

void Draw3DScene(void)
{
	NE_2DViewInit();

	// Texture scaled from 8x8 to 32x32
	NE_2DDrawTexturedQuad(150, 10,
			      150 + 32, 10 + 32,
			      0, Material_small);

	// Texture scaled from 16x16 to 64x64
	NE_2DDrawTexturedQuad(150, 100,
			      150 + 64, 100 + 64,
			      1, Material_medium);

	// Texture scaled from 64x64 to 128x128
	NE_2DDrawTexturedQuad(10, 10,
			      10 + 128, 10 + 128,
			      2, Material_big);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	NE_Init3D();

	Material_small = NE_MaterialCreate();
	Material_medium = NE_MaterialCreate();
	Material_big = NE_MaterialCreate();

	NE_MaterialTexLoadBMPtoRGBA(Material_small,
				    (void*)BMP24BIT_small_bin, 0); // 8x8
	NE_MaterialTexLoadBMPtoRGBA(Material_medium,
				    (void*)BMP24BIT_medium_bin, 0); // 16x16
	NE_MaterialTexLoadBMPtoRGBA(Material_big,
				    (void*)BMP24BIT_big_bin, 0); // 64x64

	NE_ClearColorSet(NE_DarkGray, 31, 63);

	while (1) {
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
