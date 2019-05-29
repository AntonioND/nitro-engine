/****************************************************************************
*****************************************************************************
****       _   _ _ _               ______             _                  ****
****      | \ | (_) |             |  ____|           (_)                 ****
****      |  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___       ****
****      | . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \      ****
****      | |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/      ****
****      |_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___|      ****
****                                             __/ |                   ****
****                                            |___/      V 0.6.1       ****
****                                                                     ****
****                     Copyright (C) 2008 - 2011 Antonio Niño Díaz     ****
****                                   All rights reserved.              ****
****                                                                     ****
*****************************************************************************
****************************************************************************/

/****************************************************************************
*                                                                           *
* Nitro Engine V 0.6.1 is licensed under the terms of <readme_license.txt>. *
* If you have any question, email me at <antonio_nd@hotmail.com>.           *
*                                                                           *
****************************************************************************/

#include "NEMain.h"

/*! \file   NEAPI.c */

extern _NE_input_ NE_Input;

static NE_APIObj ** NE_apipointers;
static int NE_API_OBJECTS;
static bool ne_api_system_inited = false;

typedef struct {
	int x1, y1, x2, y2;
	int event; // 0 = nothing, 1 = just pressed, 2 = held, 3 = just released
	NE_Material * tex_1, * tex_2;
	u32 color1, color2; //button not pressed/pressed -- default = white
	u32 alpha1, alpha2;
} _NE_Button_;
	
typedef struct {
	int x1, y1, x2, y2;
	int event;
	bool checked;
	NE_Material * tex_1, * tex_2;
	u32 color1, color2;
	u32 alpha1, alpha2;
} _NE_CheckBox_;
	
typedef struct {
	int x1, y1, x2, y2;
	int event;
	bool checked;
	int group;
	NE_Material * tex_1, * tex_2;
	u32 color1, color2;
	u32 alpha1, alpha2;
} _NE_RadioButton_;
	
typedef struct {
	int x1, y1, x2, y2;
	int event_minus, event_plus, event_bar;
	int value;
	int range, desp; //range = max - min; value + desp = realvalue
	bool isvertical;
	NE_Material * texbtn, * texbar, * texlong;
	int totalsize, barsize;
	int coord; //to avoid some operations ^^
	u32 color1, color2, barcolor; //1,2 used for buttons, barcolor used for bg of slidebar
	u32 alpha1, alpha2, baralpha;
} _NE_SlideBar_;


static void NE_ResetRadioButtonGroup(int group) //For internal use
{
	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) if(NE_apipointers[a] != NULL)
	{
		if(NE_apipointers[a]->type == NE_RadioButton)
		{
			if(  ((_NE_RadioButton_*)(NE_apipointers[a]->pointer))->group == group  )
			{
				((_NE_RadioButton_*)(NE_apipointers[a]->pointer))->checked = false;
			}
		}
	}
}

void NE_APIUpdate(void)
{
	if(!ne_api_system_inited) return;

	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) if(NE_apipointers[a] != NULL)
	{
	  
		//-------------------    BUTTON    -------------------
	  
		if(NE_apipointers[a]->type == NE_Button)
		{
			_NE_Button_ * button = NE_apipointers[a]->pointer;
			
			if(button->x1 < NE_Input.touch.px && button->x2 > NE_Input.touch.px &&
				button->y1 < NE_Input.touch.py && button->y2 > NE_Input.touch.py)
			{
				if(NE_Input.kdown & KEY_TOUCH) button->event = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (button->event == 1 || button->event == 2)) 
					button->event = 2;
				else if(NE_Input.kup & KEY_TOUCH && button->event == 2) button->event = 4;
				else button->event = 0;
			}
			else button->event = 0;
		}

		//-------------------    CHECK BOX    -------------------
		
		else if(NE_apipointers[a]->type == NE_CheckBox)
		{
			_NE_CheckBox_ * chbox = NE_apipointers[a]->pointer;
			
			if(chbox->x1 < NE_Input.touch.px && chbox->x2 > NE_Input.touch.px &&
				chbox->y1 < NE_Input.touch.py && chbox->y2 > NE_Input.touch.py)
			{
				if(NE_Input.kdown & KEY_TOUCH) chbox->event = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (chbox->event == 1 || chbox->event == 2)) 
					chbox->event = 2;
				else if(NE_Input.kup & KEY_TOUCH && chbox->event == 2) 
				{
					chbox->event = 4;
					chbox->checked = !chbox->checked;
				}
				else chbox->event = 0;
			}
			else chbox->event = 0;
		}

		//-------------------    RADIO BUTTON    -------------------
	  
		else if(NE_apipointers[a]->type == NE_RadioButton)
		{
			_NE_RadioButton_ * rabutton = NE_apipointers[a]->pointer;
			
			if(rabutton->x1 < NE_Input.touch.px && rabutton->x2 > NE_Input.touch.px &&
				rabutton->y1 < NE_Input.touch.py && rabutton->y2 > NE_Input.touch.py)
			{
				if(NE_Input.kdown & KEY_TOUCH) rabutton->event = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (rabutton->event == 1 || rabutton->event == 2)) 
					rabutton->event = 2;
				else if(NE_Input.kup & KEY_TOUCH && rabutton->event == 2) 
				{
					rabutton->event = 4;
					NE_ResetRadioButtonGroup(rabutton->group);
					rabutton->checked = true;
				}
				else rabutton->event = 0;
			}
			else rabutton->event = 0;
		}
	  
		//-------------------    SLIDE BAR    -------------------
	  
		else if(NE_apipointers[a]->type == NE_SlideBar) 
		{
			_NE_SlideBar_ * sldbar = NE_apipointers[a]->pointer;
			
			//Simplify code...
			int x1 = sldbar->x1, x2 = sldbar->x2, y1 = sldbar->y1, y2 = sldbar->y2;
			int px = NE_Input.touch.px, py = NE_Input.touch.py;
			bool vertical = sldbar->isvertical;
			int coord = sldbar->coord, barsize = sldbar->barsize;
			int tmp1, tmp2; //auxiliar coordinates
			if(sldbar->isvertical) { tmp1 = y1 + (x2 - x1); tmp2 = y2 - (x2 - x1); }
			else { tmp1 = x1 + (y2- y1); tmp2 = x2 - (y2- y1); }
			
			//------------  PLUS BUTTON  ------------
			if(   ((x1 < px && x2 > px && tmp2 < py && y2 > py) && vertical)   ||
			  ((tmp2 < px && x2 > px && y1 < py && y2 > py) && !vertical)  )
			{
				if(NE_Input.kdown & KEY_TOUCH) sldbar->event_plus = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (sldbar->event_plus == 1 || sldbar->event_plus == 2)) 
					{ sldbar->event_plus = 2;
					sldbar->value++; }
				else if(NE_Input.kup & KEY_TOUCH && sldbar->event_plus == 2) sldbar->event_plus = 4;
				else sldbar->event_plus = 0;
			}
			else sldbar->event_plus = 0;
			
			//------------  MINUS BUTTON  ------------
			if(   (vertical && (x1 < px && x2 > px && y1 < py && tmp1 > py))   ||
				(!vertical && (x1 < px && tmp1 > px && y1 < py && y2 > py))  )	
			{
				if(NE_Input.kdown & KEY_TOUCH) sldbar->event_minus = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (sldbar->event_minus == 1 || sldbar->event_minus == 2)) 
					{ sldbar->event_minus = 2;
					sldbar->value--; }
				else if(NE_Input.kup & KEY_TOUCH && sldbar->event_minus == 2) sldbar->event_minus = 4;
				else sldbar->event_minus = 0;
			}
			else sldbar->event_minus = 0;
			
			//------------  BAR BUTTON  ------------
			if(sldbar->event_bar == 2)
			{
				int tmp = ((vertical) ? py : px) - tmp1 - (barsize >> 1);
				tmp *= sldbar->range;
				tmp = divf32(tmp << 12, (tmp2 - tmp1 - barsize) << 12) >> 12;
				sldbar->value = tmp;
			}
			sldbar->value = (sldbar->value > sldbar->range) ? sldbar->range : sldbar->value;
			sldbar->value = (sldbar->value < 0) ? 0 : sldbar->value;	
			
			sldbar->coord = (sldbar->totalsize - barsize) * sldbar->value;
			sldbar->coord = divf32(sldbar->coord << 12, sldbar->range << 12) >> 12;
			sldbar->coord += (vertical) ?     y1 + (x2 - x1)      :    x1 + (y2 - y1);	
			coord = sldbar->coord;
			
			if(   ((x1 < px && x2 > px && coord < py && (coord + barsize) > py) && vertical)   ||
			  ((coord < px && (coord + barsize) > px && y1 < py && y2 > py) && !vertical)  )
			{
				if(NE_Input.kdown & KEY_TOUCH) sldbar->event_bar = 1;
				else if(NE_Input.kheld & KEY_TOUCH && (sldbar->event_bar == 1 || sldbar->event_bar == 2)) 
					sldbar->event_bar = 2;
				else if(NE_Input.kup & KEY_TOUCH && sldbar->event_bar == 2) sldbar->event_bar = 4;
				else sldbar->event_bar = 0;
			}
			else sldbar->event_bar = 0;
		}
	}
}


void NE_APIDraw(void)
{
	if(!ne_api_system_inited) return;

	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) if(NE_apipointers[a] != NULL)
	{
		int b = a + NE_API_MIN_PRIORITY;
		if(NE_apipointers[a]->type == NE_Button) //BUTTON
		{
			_NE_Button_ * button = NE_apipointers[a]->pointer;
			NE_Material * tex = NULL;
			u32 color = 0;
			
			if(button->event > 0) //pressed
			{
				GFX_POLY_FORMAT = POLY_ALPHA(button->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				tex = button->tex_2;
				color = button->color2;
			}
			else
			{
				GFX_POLY_FORMAT = POLY_ALPHA(button->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				tex = button->tex_1;
				color = button->color1;
			}
				
			if(tex == NULL)
				NE_2DDrawQuad(button->x1,button->y1,button->x2,button->y2,b, color);
			else
				NE_2DDrawTexturedQuadColor(button->x1,button->y1,button->x2,button->y2,b, tex, color);
		}
		else if(NE_apipointers[a]->type == NE_CheckBox) //CHECK BOX
		{
			_NE_CheckBox_ * chbox = NE_apipointers[a]->pointer;
			u32 color = 0;
			
			if(chbox->event > 0)
			{
				GFX_POLY_FORMAT = POLY_ALPHA(chbox->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = chbox->color2;
			}
			else
			{
				GFX_POLY_FORMAT = POLY_ALPHA(chbox->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = chbox->color1;
			}
			
			NE_Material * tex = (chbox->checked) ? chbox->tex_2 : chbox->tex_1;
			
			if(tex == NULL)
				NE_2DDrawQuad(chbox->x1,chbox->y1,chbox->x2,chbox->y2,b, color);
			else
				NE_2DDrawTexturedQuadColor(chbox->x1,chbox->y1,chbox->x2,chbox->y2,b, tex, color);
		}
		else if(NE_apipointers[a]->type == NE_RadioButton) //RADIO BUTTON
		{
			_NE_RadioButton_ * rabutton = NE_apipointers[a]->pointer;
			u32 color = 0;
			
			if(rabutton->event > 0)
			{
				GFX_POLY_FORMAT = POLY_ALPHA(rabutton->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = rabutton->color2;
			}
			else
			{
				GFX_POLY_FORMAT = POLY_ALPHA(rabutton->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = rabutton->color1;
			}
			
			NE_Material * tex = (rabutton->checked) ? rabutton->tex_2 : rabutton->tex_1;
			
			if(tex == NULL)
				NE_2DDrawQuad(rabutton->x1,rabutton->y1,rabutton->x2,rabutton->y2,b, color);
			else
				NE_2DDrawTexturedQuadColor(rabutton->x1,rabutton->y1,rabutton->x2,rabutton->y2,b, tex, color);
		}
		else if(NE_apipointers[a]->type == NE_SlideBar) //SLIDE BAR
		{
			_NE_SlideBar_ * sldbar = NE_apipointers[a]->pointer;
			
			//-------   used to simplify code
			int x1 = sldbar->x1, x2 = sldbar->x2, y1 = sldbar->y1, y2 = sldbar->y2; //coordinates
			int tmp1, tmp2; //auxiliar coordinates
			if(sldbar->isvertical) { tmp1 = y1 + (x2 - x1); tmp2 = y2 - (x2 - x1); }
			else { tmp1 = x1 + (y2- y1); tmp2 = x2 - (y2- y1); }
			//----------
			
			u32 color = 0;
			NE_Material * tex = sldbar->texbtn;
			//----------------      PLUS BUTTON  ---------------
			if(sldbar->event_plus > 0)
				{ GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color2; }
			else { GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color1; }	
			
			if(sldbar->isvertical)
				{ if(tex == NULL) NE_2DDrawQuad(x1,tmp2,x2,y2,b, color);
				else NE_2DDrawTexturedQuadColor(x1,tmp2,x2,y2,b, tex, color); }
			else { if(tex == NULL) NE_2DDrawQuad(tmp2,y1,x2,y2,b, color);
				else NE_2DDrawTexturedQuadColor(tmp2,y1,x2,y2,b, tex, color); }
			
			//----------------      MINUS BUTTON  ---------------
			if(sldbar->event_minus > 0)
				{ GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color2; }
			else { GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color1; }	
			
			if(sldbar->isvertical)
				{ if(tex == NULL) NE_2DDrawQuad(x1,y1,x2,tmp1,b, color);
				else NE_2DDrawTexturedQuadColor(x1,y1,x2,tmp1,b, tex, color); }
			else { if(tex == NULL) NE_2DDrawQuad(x1,y1,tmp1,y2,b, color);
				else NE_2DDrawTexturedQuadColor(x1,y1,tmp1,y2,b, tex, color); }
			
			tex = sldbar->texbar;
			//----------------      BAR BUTTON  ---------------
			if(sldbar->event_bar > 0)
				{ GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha2) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color2; }
			else { GFX_POLY_FORMAT = POLY_ALPHA(sldbar->alpha1) | POLY_ID(NE_API_POLY_ID) | NE_CULL_NONE;
				color = sldbar->color1; }	
			
			if(sldbar->isvertical)
				{ if(tex == NULL) NE_2DDrawQuad(x1,sldbar->coord,x2,sldbar->coord + sldbar->barsize,b, color);
				else NE_2DDrawTexturedQuadColor(x1,sldbar->coord,x2,sldbar->coord + sldbar->barsize,b, tex, color); }
			else { if(tex == NULL) NE_2DDrawQuad(sldbar->coord,y1,sldbar->coord + sldbar->barsize,y2,b, color);
				else NE_2DDrawTexturedQuadColor(sldbar->coord,y1,sldbar->coord + sldbar->barsize,y2,b, tex, color); }
				
			
			tex = sldbar->texlong;
			color = sldbar->barcolor;
			//----------------      SLIDE BAR     ---------------
			GFX_POLY_FORMAT = POLY_ALPHA(sldbar->baralpha) | POLY_ID(NE_API_POLY_ID_ALT) | NE_CULL_NONE;
			
			//NOTE: b+1 -> Bar button must be in front of bar (b+1 is less priority than b).
			if(sldbar->isvertical) 
				{ if(tex == NULL) NE_2DDrawQuad(x1,tmp1,x2,tmp2,b+1, color);
				else NE_2DDrawTexturedQuadColor(x1,tmp1,x2,tmp2,b+1, tex, color); }
			else { if(tex == NULL) NE_2DDrawQuad(tmp1,y1,tmp2,y2,b+1, color);
				else NE_2DDrawTexturedQuadColor(tmp1,y1,tmp2,y2,b+1, tex, color); }
		}
	}
}



NE_APIObj * NE_APIButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2)
{
	if(!ne_api_system_inited) return NULL;

	int a = 0;
	while(a < NE_API_OBJECTS)
	{
		if(NE_apipointers[a] == NULL)
		{
			_NE_Button_ * ptr = (_NE_Button_*)malloc(sizeof(_NE_Button_));
			NE_AssertPointer(ptr,"NE_APIButtonCreate: Couldn't allocate object.");
			
			NE_apipointers[a] = (void*)malloc(sizeof(NE_APIObj));
			NE_AssertPointer(NE_apipointers[a],"NE_APIButtonCreate: Couldn't allocate object.");
			
			NE_apipointers[a]->pointer = (void*)ptr;
			NE_apipointers[a]->type = NE_Button;
			
			ptr->x1 = x1;  ptr->y1 = y1;  ptr->x2 = x2;  ptr->y2 = y2;
			ptr->event = -1;
			ptr->tex_1 = ptr->tex_2 = NULL;
			ptr->color1 = ptr->color2 = NE_White;
			ptr->alpha1 = ptr->alpha2 = 31;
			return NE_apipointers[a];
		}
		a++;
	}

	NE_DebugPrint("NE_APIButtonCreate: No free slots...");

	return NULL;
}


NE_APIObj * NE_APICheckBoxCreate(s16 x1, s16 y1, s16 x2, s16 y2, bool initialvalue)
{
	if(!ne_api_system_inited) return NULL;

	int a = 0;
	while(a < NE_API_OBJECTS)
	{
		if(NE_apipointers[a] == NULL)
		{
			_NE_CheckBox_ * ptr = (_NE_CheckBox_*)malloc(sizeof(_NE_CheckBox_));
			NE_AssertPointer(ptr,"NE_APICheckBoxCreate: Couldn't allocate object.");
			
			NE_apipointers[a] = (void*)malloc(sizeof(NE_APIObj));
			NE_AssertPointer(NE_apipointers[a],"NE_APICheckBoxCreate: Couldn't allocate object.");
			
			NE_apipointers[a]->pointer = (void*)ptr;
			NE_apipointers[a]->type = NE_CheckBox;
			
			ptr->x1 = x1;  ptr->y1 = y1;  ptr->x2 = x2;  ptr->y2 = y2;
			ptr->event = -1;
			ptr->tex_1 = ptr->tex_2 = NULL;
			ptr->color1 = ptr->color2 = NE_White;
			ptr->alpha1 = ptr->alpha2 = 31;
			ptr->checked = initialvalue;
			return NE_apipointers[a];
		}
		a++;
	}

	NE_DebugPrint("NE_APICheckBoxCreate: No free slots...");

	return NULL;
}


NE_APIObj * NE_APIRadioButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2, int group, bool initialvalue)
{
	if(!ne_api_system_inited) return NULL;

	int a = 0;
	while(a < NE_API_OBJECTS)
	{
		if(NE_apipointers[a] == NULL)
		{
			_NE_RadioButton_ * ptr = (_NE_RadioButton_*)malloc(sizeof(_NE_RadioButton_));
			NE_AssertPointer(ptr,"NE_APIRadioButtonCreate: Couldn't allocate object.");
			
			NE_apipointers[a] = (void*)malloc(sizeof(NE_APIObj));
			NE_AssertPointer(NE_apipointers[a],"NE_APIRadioButtonCreate: Couldn't allocate object.");
			
			NE_apipointers[a]->pointer = (void*)ptr;
			NE_apipointers[a]->type = NE_RadioButton;
			
			ptr->x1 = x1;  ptr->y1 = y1;  ptr->x2 = x2;  ptr->y2 = y2;
			ptr->event = -1;
			ptr->tex_1 = ptr->tex_2 = NULL;
			ptr->color1 = ptr->color2 = NE_White;
			ptr->alpha1 = ptr->alpha2 = 31;
			ptr->group = group;
			
			if(initialvalue) NE_ResetRadioButtonGroup(group);
			
			ptr->checked = initialvalue;
			return NE_apipointers[a];
		}
		a++;
	}

	NE_DebugPrint("NE_APIRadioButtonCreate: No free slots...");

	return NULL;
}


NE_APIObj * NE_APISlideBarCreate(s16 x1, s16 y1, s16 x2, s16 y2, int min, int max, int initialvalue)
{
	if(!ne_api_system_inited) return NULL;

	int a = 0;
	while(a < NE_API_OBJECTS)
	{
		if(NE_apipointers[a] == NULL)
		{
			_NE_SlideBar_ * ptr = (_NE_SlideBar_*)malloc(sizeof(_NE_SlideBar_));
			NE_AssertPointer(ptr,"NE_APISlideBarCreate: Couldn't allocate object.");
			
			NE_apipointers[a] = (void*)malloc(sizeof(NE_APIObj));
			NE_AssertPointer(NE_apipointers[a],"NE_APISlideBarCreate: Couldn't allocate object.");
			
			NE_apipointers[a]->pointer = (void*)ptr;
			NE_apipointers[a]->type = NE_SlideBar;
			
			ptr->x1 = x1;  ptr->y1 = y1;  ptr->x2 = x2;  ptr->y2 = y2;
			ptr->event_minus = ptr->event_plus = ptr->event_bar = -1;
			ptr->texbtn = ptr->texbar = ptr->texlong = NULL;
			ptr->color1 = ptr->color2 = ptr->barcolor = NE_White;
			ptr->alpha1 = ptr->alpha2 = ptr->baralpha = 31;
			ptr->value = initialvalue - min;
			ptr->desp = min;
			ptr->range = max - min;
			
			ptr->isvertical = ( x2-x1 > y2-y1 ) ? false : true;
			
			if(ptr->isvertical) ptr->totalsize = y2-y1 - ( (x2-x1) << 1);
			else ptr->totalsize = x2-x1 - ( (y2-y1) << 1);
			
			ptr->barsize = 100 - ptr->range;
			ptr->barsize = (ptr->barsize < 20) ? (20 << 12) : (ptr->barsize << 12);
			ptr->barsize = (divf32(ptr->barsize,100 << 12) * ptr->totalsize) >> 12;
			
			ptr->coord = (ptr->totalsize - ptr->barsize) * ptr->value;
			ptr->coord = divf32(ptr->coord << 12, ptr->range << 12) >> 12;
			ptr->coord += (ptr->isvertical) ? ptr->y1 + (ptr->x2 - ptr->x1) : ptr->x1 + (ptr->y2- ptr->y1); 
			
			return NE_apipointers[a];
		}
		a++;
	}

	NE_DebugPrint("NE_APISlideBarCreate: No free slots...");

	return NULL;
}


void NE_APIButtonConfig(NE_APIObj * btn, NE_Material * material, u32 color, u32 alpha, 
						NE_Material * pressedmaterial, u32 pressedcolor, u32 pressedalpha)
{
	NE_AssertPointer(btn,"NE_APIButtonConfig: NULL pointer.");
	NE_Assert(btn->type == NE_Button,"NE_APIButtonConfig: Not a button.");

	_NE_Button_ * button = btn->pointer;

	button->tex_1 = material;
	button->tex_2 = pressedmaterial;
	button->color1 = color;
	button->color2 = pressedcolor;
	button->alpha1 = alpha;
	button->alpha2 = pressedalpha;
}

void NE_APICheckBoxConfig(NE_APIObj * chbx, NE_Material * materialtrue, NE_Material * materialfalse,
						u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha)
{
	NE_AssertPointer(chbx,"NE_APICheckBoxConfig: NULL pointer.");
	NE_Assert(chbx->type == NE_CheckBox,"NE_APICheckBoxConfig: Not a check box.");

	_NE_CheckBox_ * checkbox = chbx->pointer;

	checkbox->tex_1 = materialfalse;
	checkbox->tex_2 = materialtrue;
	checkbox->color1 = color;
	checkbox->color2 = pressedcolor;
	checkbox->alpha1 = alpha;
	checkbox->alpha2 = pressedalpha;
}


void NE_APIRadioButtonConfig(NE_APIObj * rdbtn, NE_Material * materialtrue, NE_Material * materialfalse,
						u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha)
{
	NE_AssertPointer(rdbtn,"NE_APIRadioButtonConfig: NULL pointer.");
	NE_Assert(rdbtn->type == NE_RadioButton,"NE_APIRadioButtonConfig: Not a radio button.");

	_NE_RadioButton_ * radiobutton = rdbtn->pointer;

	radiobutton->tex_1 = materialfalse;
	radiobutton->tex_2 = materialtrue;
	radiobutton->color1 = color;
	radiobutton->color2 = pressedcolor;
	radiobutton->alpha1 = alpha;
	radiobutton->alpha2 = pressedalpha;
}


void NE_APISlideBarConfig(NE_APIObj * sldbar, NE_Material * matbtn, NE_Material * matbarbtn,
		NE_Material * matbar, u32 normalcolor, u32 pressedcolor, u32 barcolor, u32 alpha, 
		u32 pressedalpha, u32 baralpha)
{
	NE_AssertPointer(sldbar,"NE_APISlideBarConfig: NULL pointer.");
	NE_Assert(sldbar->type == NE_SlideBar,"NE_APISlideBarConfig: Not a slide bar.");

	_NE_SlideBar_ * slidebar = sldbar->pointer;

	slidebar->texbtn = matbtn;
	slidebar->texbar = matbarbtn;
	slidebar->texlong = matbar;
	slidebar->color1 = normalcolor;
	slidebar->color2 = pressedcolor;
	slidebar->barcolor = barcolor;
	slidebar->alpha1 = alpha;
	slidebar->alpha2 = pressedalpha;
	slidebar->baralpha = baralpha;
}



void NE_APISlideBarSetMinMax(NE_APIObj * sldbr, int min, int max)
{
	NE_AssertPointer(sldbr,"NE_APISlideBarSetMinMax: NULL pointer.");
	NE_Assert(sldbr->type == NE_SlideBar,"NE_APISlideBarSetMinMax: Not a slide bar.");

	_NE_SlideBar_ * slidebar = sldbr->pointer;

	slidebar->desp = min; 
	slidebar->range = max - min;

	slidebar->barsize = 100 - slidebar->range;
	slidebar->barsize = (slidebar->barsize < 20) ? (20 << 12) : (slidebar->barsize << 12);
	slidebar->barsize = (divf32(slidebar->barsize,100 << 12) * slidebar->totalsize) >> 12;

	slidebar->coord = (slidebar->totalsize - slidebar->barsize) * slidebar->value;
	slidebar->coord = divf32(slidebar->coord << 12, slidebar->range << 12) >> 12;
	slidebar->coord += (slidebar->isvertical) ? 
					slidebar->y1 + (slidebar->x2 - slidebar->x1) : 
					slidebar->x1 + (slidebar->y2- slidebar->y1); 
}

NE_APIState NE_APIObjectGetEvent(NE_APIObj * obj)
{
	NE_AssertPointer(obj,"NE_APIObjectGetEvent: NULL pointer.");

	switch(obj->type)
	{
		case NE_Button:      return ((_NE_Button_*)(obj->pointer))->event;      break;
		case NE_CheckBox:    return ((_NE_CheckBox_*)(obj->pointer))->event;    break;
		case NE_RadioButton: return ((_NE_RadioButton_*)(obj->pointer))->event; break;
		case NE_SlideBar:    break; //Calculated after switch
		default: 
			NE_DebugPrint("NE_APIObjectGetEvent: Unknown object type.");
			return -1;
	}

	// if NE_SlideBar
	_NE_SlideBar_ * ptr = obj->pointer;
	return ptr->event_plus | ptr->event_minus | ptr->event_bar;
}


bool NE_APICheckBoxGetValue(NE_APIObj * chbx)
{
	NE_AssertPointer(chbx,"NE_APICheckBoxGetValue: NULL pointer.");
	NE_Assert(chbx->type == NE_CheckBox,"NE_APICheckBoxGetValue: Not a check box.");
	return ((_NE_CheckBox_*)(chbx->pointer))->checked;
}

bool NE_APIRadioButtonGetValue(NE_APIObj * rdbtn)
{
	NE_AssertPointer(rdbtn,"NE_APIRadioButtonGetValue: NULL pointer.");
	NE_Assert(rdbtn->type == NE_RadioButton,"NE_APIRadioButtonGetValue: Not a radio button.");
	return ((_NE_RadioButton_*)(rdbtn->pointer))->checked;
}


int NE_APISlideBarGetValue(NE_APIObj * sldbr)
{
	NE_AssertPointer(sldbr,"NE_APISlideBarGetValue: NULL pointer.");
	NE_Assert(sldbr->type == NE_SlideBar,"NE_APISlideBarGetValue: Not a slide bar.");
	_NE_SlideBar_ * slidebar = sldbr->pointer;
	return slidebar->value + slidebar->desp;
}


void NE_APIDeleteObject(NE_APIObj * obj)
{
	NE_AssertPointer(obj,"NE_APIDeleteObject: NULL pointer.");

	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) if(NE_apipointers[a] == obj)
	{
		free(obj->pointer);
		free(obj);
		NE_apipointers[a] = NULL;
		return;
	}

	NE_DebugPrint("NE_APIDeleteObject: Pointer not found in array.");
}


void NE_APIDeleteAll(void)
{
	if(!ne_api_system_inited) return;

	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) if(NE_apipointers[a])
	{
		free(NE_apipointers[a]->pointer);
		free(NE_apipointers[a]);
		NE_apipointers[a] = NULL;
	}
}

void NE_APISystemReset(int number_of_objects)
{
	if(ne_api_system_inited) NE_APISystemEnd();

	if(number_of_objects < 1) NE_API_OBJECTS = NE_API_DEFAULT_OBJECTS;
	else NE_API_OBJECTS = number_of_objects;

	NE_apipointers = malloc(NE_API_OBJECTS * sizeof(NE_apipointers));
	NE_AssertPointer(NE_apipointers,"NE_APISystemReset: Not enough memory to allocate array.");

	int a;
	for(a = 0; a < NE_API_OBJECTS; a++) NE_apipointers[a] = NULL;

	ne_api_system_inited = true;
}

void NE_APISystemEnd(void)
{
	if(!ne_api_system_inited) return;

	NE_APIDeleteAll();

	free(NE_apipointers);

	ne_api_system_inited = false;
}




