// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2011, 2019, 2022
//
// This file is part of Nitro Engine

#include <NEMain.h>

#include "button_tex_bin.h"
#include "button_pal_bin.h"
#include "empty_tex_bin.h"
#include "empty_pal_bin.h"
#include "true_tex_bin.h"
#include "true_pal_bin.h"

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

    // Load textures
    NE_Material *ButtonImg, *TrueImg, *EmptyImg;
    NE_Palette *ButtonPal, *TruePal, *EmptyPal;

    ButtonImg = NE_MaterialCreate();
    TrueImg = NE_MaterialCreate();
    EmptyImg = NE_MaterialCreate();

    ButtonPal = NE_PaletteCreate();
    TruePal = NE_PaletteCreate();
    EmptyPal = NE_PaletteCreate();

    NE_MaterialTexLoad(ButtonImg, NE_PAL256, 64, 64,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                       (void *)button_tex_bin);
    NE_PaletteLoad(ButtonPal, (void *)button_pal_bin, 256, NE_PAL256);
    NE_MaterialSetPalette(ButtonImg, ButtonPal);

    NE_MaterialTexLoad(EmptyImg, NE_PAL256, 64, 64,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                       (void *)empty_tex_bin);
    NE_PaletteLoad(EmptyPal, (void *)empty_pal_bin, 256, NE_PAL256);
    NE_MaterialSetPalette(EmptyImg, EmptyPal);

    NE_MaterialTexLoad(TrueImg, NE_PAL256, 64, 64,
                       NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                       (void *)true_tex_bin);
    NE_PaletteLoad(TruePal, (void *)true_pal_bin, 256, NE_PAL256);
    NE_MaterialSetPalette(TrueImg, TruePal);


    // Create one button
    // -----------------

    NE_GUIObj *Button = NE_GUIButtonCreate(116, 16,  // Upper-left pixel
                                           180, 80); // Down-right pixel
    NE_GUIButtonConfig(Button,
                       // Appearance when pressed (texture, color, alpha)
                       ButtonImg, NE_White, 31,
                       //Appearance when not pressed
                       ButtonImg, NE_Blue, 25);

    // Create one check box
    // --------------------

    NE_GUIObj *ChBx = NE_GUICheckBoxCreate(16, 16, 80, 80, // Coordinates
                                           true); // Initial value
    NE_GUICheckBoxConfig(ChBx,
                         TrueImg,  // Texture when value is true
                         EmptyImg, // Texture when value is false
                         // Appearance when pressed (color, alpha)
                         NE_White, 31,
                         // Appearance when not pressed
                         NE_Yellow, 15);

    // Create three radio buttons
    // --------------------------

    NE_GUIObj *RaBtn1 = NE_GUIRadioButtonCreate(16, 116, 56, 156, // Coordinates
                                                0, // Button group
                                                false); // Initial value

    // Same arguments as check boxes.
    NE_GUIRadioButtonConfig(RaBtn1,
                            TrueImg, EmptyImg,
                            NE_White, 31,
                            NE_Gray, 31);

    NE_GUIObj *RaBtn2 = NE_GUIRadioButtonCreate(72, 116, 112, 156,
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

    // This function decides if the slide bar is vertical or horizontal based on
    // the size.
    NE_GUIObj *SldBar1 = NE_GUISlideBarCreate(255 - 10 - 20, 10, // Coordinates
                                              255 - 10, 192 - 10,
                                              0, 100, // Min. and max. values
                                              50); // Initial value

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

    while (1)
    {
        scanKeys(); // This function is needed for the GUI

        printf("\x1b[0;0H");
        printf("Slide bar 1: %d  \n", NE_GUISlideBarGetValue(SldBar1));
        printf("Slide bar 2: %d  \n", NE_GUISlideBarGetValue(SldBar2));
        printf("\n");
        printf("Radio button 1: %d \n", NE_GUIRadioButtonGetValue(RaBtn1));
        printf("Radio button 2: %d \n", NE_GUIRadioButtonGetValue(RaBtn2));
        printf("Radio button 3: %d \n", NE_GUIRadioButtonGetValue(RaBtn3));
        printf("\n");
        printf("Check box: %d \n", NE_GUICheckBoxGetValue(ChBx));
        printf("\n");
        printf("Button event: %d ", NE_GUIObjectGetEvent(Button));

        // Draw things...
        NE_Process(Draw3DScene);

        // Update GUI, input and wait for vertical blank. You have to
        // call scanKeys() each frame for this to work.
        NE_WaitForVBL(NE_UPDATE_GUI);
    }

    return 0;
}
