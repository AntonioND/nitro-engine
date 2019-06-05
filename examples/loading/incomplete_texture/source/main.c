// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "test_tex_bin.h"
#include "test_pal_bin.h"
#include "test2_tex_bin.h"
#include "test2_pal_bin.h"

NE_Material *Material, *Material2;
NE_Palette *Palette, *Palette2;

// This is an example to show how Nitro Engine can load textures of any size.
// If the width is not a power of 2, it will take much more time than normal to
// load. The height doesn't matter.
//
// Nitro Engine resizes the width to the nearest power of two (if needed) when
// loading a texture. This means:
//
// - It needs more time than normal to load because it needs time to resize it.
// - It won't save space in VRAM, the DS still needs to see the full width in
//   VRAM. It will just save some space in the ROM.
//
// If the height is different it will actually save some space in VRAM (apart
// from saving space in the ROM).

void Draw3DScene(void)
{
	NE_2DViewInit();

	NE_2DDrawTexturedQuad(10, 10,
			      10 + 57, 10 + 100,
			      0, Material);

	NE_2DDrawTexturedQuad(118, 10,
			      246, 138,
			      0, Material2);
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	// Init 3D mode
	NE_Init3D();

	// Allocate objects for a material
	Material = NE_MaterialCreate();
	Palette = NE_PaletteCreate();

	NE_MaterialTexLoad(Material,
			   GL_RGB32_A3, // Texture type
			   57, 100,     // Width, height (in pixels)
			   TEXGEN_TEXCOORD, (u8 *) test_tex_bin);
	NE_PaletteLoad(Palette, (u16 *)test_pal_bin, 32, GL_RGB32_A3);
	NE_MaterialTexSetPal(Material, Palette);

	// Allocate objects for another material
	Material2 = NE_MaterialCreate();
	Palette2 = NE_PaletteCreate();

	NE_MaterialTexLoad(Material2, GL_RGB4, 100, 100, TEXGEN_TEXCOORD,
			   (u8 *) test2_tex_bin);
	NE_PaletteLoad(Palette2, (u16 *)test2_pal_bin, 32, GL_RGB4);
	NE_MaterialTexSetPal(Material2, Palette2);

	while (1) {
		// Draw 3D scene
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
