// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "test_tex_bin.h"
#include "test_pal_bin.h"

NE_Material *Material, *Material2;
NE_Palette *Palette;

void Draw3DScene(void)
{
	NE_2DViewInit();

	NE_2DDrawTexturedQuad(0, 0,
			      256, 192,
			      0, Material);
}

void Draw3DScene2(void)
{
	NE_2DViewInit();

	NE_2DDrawTexturedQuad(64, 32,
			      64 + 128, 32 + 128,
			      0, Material2);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init 3D mode
	NE_InitDual3D();

	// Allocate objects
	Material = NE_MaterialCreate();
	Material2 = NE_MaterialCreate();
	Palette = NE_PaletteCreate();

	// Load part of the texture ignoring some of its height. You can't do
	// this with width because of how textures are laid out in VRAM.
	NE_MaterialTexLoad(Material, GL_RGB32_A3, 128, 96, TEXGEN_TEXCOORD,
			   (u8 *) test_tex_bin);

	// Load complete texture
	NE_MaterialTexLoad(Material2, GL_RGB32_A3, 128, 128, TEXGEN_TEXCOORD,
			   (u8 *) test_tex_bin);

	NE_PaletteLoad(Palette, (u16 *)test_pal_bin, 32, GL_RGB32_A3);

	NE_MaterialTexSetPal(Material, Palette);
	NE_MaterialTexSetPal(Material2, Palette);

	while(1) {
		NE_ProcessDual(Draw3DScene, Draw3DScene2);
		NE_WaitForVBL(0);
	}

	return 0;
}
