// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

// Don't forget to compile Nitro Engine with NE_DEBUG defined or this won't work

void Draw3DScene(void)
{
	// Let's generate some error messages...
	NE_LightOff(100);
	NE_CameraSetI(NULL,
		      1, 1, 1,
		      1, 1, 1,
		      1, 1, 1);
	NE_PolyFormat(100, 120, 0, 0, 0);
}

void error_handler(const char * text)
{
	// Simple handler. You could write this to a file instead, for example.
	printf(text);
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

	// Set a custom error handler
	NE_DebugSetHandler(error_handler);

	// In order to use the default handler again it is needed to call
	// NE_DebugSetHandlerConsole(). After that, all messages will be printed
	// to the default console

	while (1) {
		NE_Process(Draw3DScene);
		NE_WaitForVBL(0);
	}

	return 0;
}
