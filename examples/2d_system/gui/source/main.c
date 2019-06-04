// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "Button_bin.h"
#include "Empty_bin.h"
#include "True_bin.h"

void Draw3DScene(void)
{
	NE_2DViewInit();
	NE_GUIDraw();
}

int main(void)
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);
	irqSet(IRQ_HBLANK, NE_HBLFunc);

	NE_Init3D();

	// Move 3D screen to lower screen
	NE_SwapScreens();
	// libnds uses VRAM_C for the text console, reserve A and B only
	NE_TextureSystemReset(0, 0, NE_VRAM_AB);
	// Init console in non-3D screen
	consoleDemoInit();

	// Set bg color for 3D screen
	NE_ClearColorSet(RGB15(15, 15, 15), 31, 63);

	// Pointers to the textures
	NE_Material *ButtonImg, *TrueImg, *EmptyImg;

	// Load textures
	ButtonImg = NE_MaterialCreate();
	TrueImg = NE_MaterialCreate();
	EmptyImg = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGBA(ButtonImg,(void*)Button_bin, true);
	NE_MaterialTexLoadBMPtoRGBA(TrueImg,(void*)True_bin, true);
	NE_MaterialTexLoadBMPtoRGBA(EmptyImg,(void*)Empty_bin, true);

	// Create one button
	// -----------------
	NE_GUIObj *Button = NE_GUIButtonCreate(116,16,  // Upper-left pixel
					       180,80); // Down-right pixel
	NE_GUIButtonConfig(Button,
			   // Appearance when pressed (texture, color, alpha)
			   ButtonImg, NE_White, 31,
			   //Appearance when not pressed
	                   ButtonImg, NE_Blue, 25);

	// Create one check box
	// --------------------
	NE_GUIObj *ChBx = NE_GUICheckBoxCreate(16,16,80,80, // Coordinates
					       true); // Initial value
	NE_GUICheckBoxConfig(ChBx,
			     TrueImg,  // Texture when value is true
			     EmptyImg, // Texture when value is false
			     // Appearance when pressed (color, alpha)
			     NE_White, 31,
			     // Appearance when not pressed
			     NE_Yellow, 15);

	// Create three radio buttons
	// ---------------------
	NE_GUIObj *RaBtn1 =
		NE_GUIRadioButtonCreate(16, 116, 56, 156, // Coordinates
					0, // Button group
					false); // Initial value

	// Same arguments as check boxes.
	NE_GUIRadioButtonConfig(RaBtn1,
				TrueImg, EmptyImg,
				NE_White, 31,
				NE_Gray, 31);

	NE_GUIObj *RaBtn2 =
		NE_GUIRadioButtonCreate(72, 116, 112, 156,
					0,
	                                true);
	// When creating a radio button, if the initial value is true, all
	// other buttons of the same group will be set to false.

	NE_GUIRadioButtonConfig(RaBtn2, TrueImg, EmptyImg,
				NE_White, 31, NE_Gray, 31);

	NE_GUIObj *RaBtn3 = NE_GUIRadioButtonCreate(128, 116, 168, 156,
						    0, false);
	NE_GUIRadioButtonConfig(RaBtn3, TrueImg, EmptyImg,
				NE_White, 31, NE_Gray, 31);

	// Create two slide bars
	// ---------------------
	NE_GUIObj *SldBar1 =
		NE_GUISlideBarCreate(255 - 10 - 20, 10, // Coordinates.
				     255 - 10, 192 - 10,
				     // This function decides if the slide bar
				     // is vertical or horizontal based on the
				     // coordinates.
				     0, 100, // Min. and max. values.
				     50); // Initial value.

	NE_GUISlideBarConfig(SldBar1,
			     EmptyImg, // Buttons' texture
			     EmptyImg, // Sliding button's texture
			     NULL, // Bar texture (NULL = No image...)
			     // Buttons' pressed/not pressed colors.
			     NE_White, NE_Yellow,
			     NE_Black, // Bar color
			     31, 29, // Buttons' pressed/not pressed alpha.
			     15); // Bar alpha

	NE_GUIObj *SldBar2 = NE_GUISlideBarCreate(10, 192 - 10 - 20,
						  255 - 50, 192 - 10,
						  -30, 20,
						  0);
	NE_GUISlideBarConfig(SldBar2,
			     EmptyImg, NULL, NULL,
			     NE_Green, RGB15(25, 31, 0), NE_Yellow,
			     31, 20, 31);

	while(1) {
		scanKeys(); // This function is needed for the GUI

		printf("\x1b[0;0HSlide bar 1: %d  ",
		       NE_GUISlideBarGetValue(SldBar1));
		printf("\nSlide bar 2: %d  ",
		       NE_GUISlideBarGetValue(SldBar2));

		printf("\n\nRadio button 1: %d ",
		       NE_GUIRadioButtonGetValue(RaBtn1));
		printf("\nRadio button 2: %d ",
		       NE_GUIRadioButtonGetValue(RaBtn2));
		printf("\nRadio button 3: %d ",
		       NE_GUIRadioButtonGetValue(RaBtn3));

		printf("\n\nCheck box: %d ",
		       NE_GUICheckBoxGetValue(ChBx));

		printf("\n\nButton event: %d ",
		       NE_GUIObjectGetEvent(Button));

		// Draw things...
		NE_Process(Draw3DScene);

		// Update GUI, input and wait for vertical blank. You have to
		// call scanKeys() each frame for this to work.
		NE_WaitForVBL(NE_UPDATE_GUI);
	}

	return 0;
}
