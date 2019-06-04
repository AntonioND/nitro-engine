
#include <NEMain.h>
#include "Button_bin.h"
#include "Empty_bin.h"
#include "True_bin.h"

void Draw3DScene(void)
{
	NE_2DViewInit(); 
	NE_APIDraw();   //Easy, isn't it? ;)
}


int main()
{
	irqEnable(IRQ_HBLANK);
	irqSet(IRQ_VBLANK, NE_VBLFunc);  //Init Nitro Engine...
	irqSet(IRQ_HBLANK, NE_HBLFunc);
	
	NE_Init3D();
	
	NE_SwapScreens(); //3D screen to lower screen
	NE_TextureSystemReset(0,0,NE_VRAM_AB); // libnds uses VRAM_C for the text console
	consoleDemoInit(); //Init text in non-3D screen
	
	NE_ClearColorSet(RGB15(15,15,15),31,63); //Set bg color
	
	NE_Material * ButtonImg, * TrueImg, * EmptyImg; //Pointers to the textures
	
	ButtonImg = NE_MaterialCreate();
	TrueImg = NE_MaterialCreate();  //Load textures
	EmptyImg = NE_MaterialCreate();
	NE_MaterialTexLoadBMPtoRGBA(ButtonImg,(void*)Button_bin, true);
	NE_MaterialTexLoadBMPtoRGBA(TrueImg,(void*)True_bin, true);
	NE_MaterialTexLoadBMPtoRGBA(EmptyImg,(void*)Empty_bin, true);

	
	//----------------Create one button---------------------
	NE_APIObj * Button = NE_APIButtonCreate(116,16,  //Upper-left pixel
	                                        180,80); //Down-right pixel
							
							//Appearance when pressed (texture, color, alpha)
	NE_APIButtonConfig(Button, ButtonImg, NE_White, 31,  
							//Appearance when not pressed
	                          ButtonImg, NE_Blue, 25);

	//----------------Create one check box---------------------
	NE_APIObj * ChBx = NE_APICheckBoxCreate(16,16,80,80, //Coordinates
	                                               true); //Start value
	NE_APICheckBoxConfig(ChBx,  TrueImg, //Texture when value is true
	                            EmptyImg, //Texture when value is false
								NE_White, 31,   //Appearance when pressed (color, alpha)
								NE_Yellow, 15); //Appearance when not pressed

	//----------------Create three radio buttons---------------------
	NE_APIObj * RaBtn1 = NE_APIRadioButtonCreate(16,116,56,156, //Coordinates
	                                                          0, //Button group
													     false); //Initial value
								//Same as check boxes.
	NE_APIRadioButtonConfig(RaBtn1, TrueImg, EmptyImg, NE_White, 31, NE_Gray, 31); 
	
	
	NE_APIObj * RaBtn2 = NE_APIRadioButtonCreate(72,116,112,156, 0, //Coordinates and group
	                                                          true); //If true, buttons of same
																	 //group will be set to false.
	NE_APIRadioButtonConfig(RaBtn2, TrueImg, EmptyImg, NE_White, 31, NE_Gray, 31);
	
	NE_APIObj * RaBtn3 = NE_APIRadioButtonCreate(128,116,168,156, 0,false);
	NE_APIRadioButtonConfig(RaBtn3, TrueImg, EmptyImg, NE_White, 31, NE_Gray, 31);
	
	//----------------Create two slide bars---------------------
	NE_APIObj * SldBar1 = NE_APISlideBarCreate(255 - 10 - 20,10, //Coordinates. This function 
	                                          255 - 10, 192 - 10, //will decide if slide bar is 
															      //vertical  or horizontal.
											              0, 100, //Min. and max. values.
														      50);//Initial value.
	NE_APISlideBarConfig(SldBar1, EmptyImg, //Buttons' texture
	                              EmptyImg, //Sliding button's texture
							          NULL, //Bar texture (NULL = No image...)
					   NE_White, NE_Yellow, //Buttons' pressed/not pressed colors.
					               NE_Black, //Bar color
							         31, 29, //Buttons' pressed/not pressed alpha.
									    15); //Bar alpha

	NE_APIObj * SldBar2 = NE_APISlideBarCreate(10,192-10-20,255 - 50,192 - 10, -30, 20, 0);
	NE_APISlideBarConfig(SldBar2, EmptyImg, NULL, NULL, NE_Green, RGB15(25,31,0), NE_Yellow, 31,20,31);
	
	while(1) 
	{
		scanKeys(); //Needed
		
		//Print information
		printf("\x1b[0;0HSlide bar 1: %d  ",NE_APISlideBarGetValue(SldBar1));
		printf("\nSlide bar 2: %d  ",NE_APISlideBarGetValue(SldBar2));
		
		printf("\n\nRadio button 1: %d ",NE_APIRadioButtonGetValue(RaBtn1));
		printf("\nRadio button 2: %d ",NE_APIRadioButtonGetValue(RaBtn2));
		printf("\nRadio button 3: %d ",NE_APIRadioButtonGetValue(RaBtn3));
		
		printf("\n\nCheck box: %d ",NE_APICheckBoxGetValue(ChBx));
		
		printf("\n\nButton event: %d ",NE_APIObjectGetEvent(Button));
		
		NE_Process(Draw3DScene); //Draw things...
		//Update API, input and wait for vertical blank. You have to call scanKeys() each frame
		//for this to work.
		NE_WaitForVBL(NE_UPDATE_API); 
	}
	
	return 0;
}
