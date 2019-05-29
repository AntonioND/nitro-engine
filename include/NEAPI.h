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

#ifndef __NE_API_H__
#define __NE_API_H__

#include "NEMain.h"

/*! \file   NEAPI.h
 *  \brief  API functions. */

/*! @defgroup api_system API System.

	Functions to create buttons, check boxes, radio buttons and slidebars.
	@{
*/

#define NE_API_DEFAULT_OBJECTS      128 /*! \def   #define NE_API_DEFAULT_OBJECTS    128 */

#define NE_API_POLY_ID       62 /*! \def   #define NE_API_POLY_ID     62 */
#define NE_API_POLY_ID_ALT   61 /*! \def   #define NE_API_POLY_ID_ALT 61 */

/*! \def   #define NE_API_MIN_PRIORITY 100 */
#define NE_API_MIN_PRIORITY 100

/*! \enum  NE_APITypes
 *  \brief API object types. */
typedef enum {
	NE_Button = 1,  /*!< Simple button. */
	NE_CheckBox,    /*!< Check Box. */
	NE_RadioButton, /*!< Radio button. */
	NE_SlideBar     /*!< Slide bar. */
} NE_APITypes;

/*! \enum  NE_APIState
 *  \brief API states. */
typedef enum {
	NE_Error   = -1,    /*!< API hasn't been updated or input hasn't been updated. */
	NE_None    =  0,    /*!< Not pressed. */
	NE_Pressed =  1,    /*!< Just pressed. */
	NE_Held    =  1<<1, /*!< Held. */
	NE_Clicked =  1<<2  /*!< Just released. */
} NE_APIState;

/*! \struct NE_APIObj
 *  \brief  Holds information of an API object. */
typedef struct {
	NE_APITypes type;
	void * pointer;
} NE_APIObj;

//-----------------------------------------------------------------------

/*! \fn    void NE_APIUpdate(void);
 *  \brief Updates all API objects. NE_UpdateInput() must be called each frame for this to work. */
void NE_APIUpdate(void);

/*! \fn    void NE_APIDraw(void);
 *  \brief Do I really have to explain what does this function do?
 
You have to call NE_2DViewInit() before this. */
void NE_APIDraw(void);

//-----------------------------------------------------------------------

/*! \fn    NE_APIObj * NE_APIButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2);
 *  \brief Returns a pointer to a new button.
 *  \param x1 (x1,y1) Upper left corner. 
 *  \param y1 (x1,y1) Upper left corner. 
 *  \param x2 (x2,y2) Down right corner. 
 *  \param y2 (x2,y2) Down right corner. */
NE_APIObj * NE_APIButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2);

/*! \fn    NE_APIObj * NE_APICheckBoxCreate(s16 x1, s16 y1, s16 x2, s16 y2, bool initialvalue);
 *  \brief Returns a pointer to a new check button.
 *  \param x1 (x1,y1) Upper left corner. 
 *  \param y1 (x1,y1) Upper left corner. 
 *  \param x2 (x2,y2) Down right corner. 
 *  \param y2 (x2,y2) Down right corner. 
 *  \param initialvalue If true, checkbox is created in "checked" state. */
NE_APIObj * NE_APICheckBoxCreate(s16 x1, s16 y1, s16 x2, s16 y2, bool initialvalue);

/*! \fn    NE_APIObj * NE_APIRadioButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2, int group, bool initialvalue);
 *  \brief Returns a pointer to a new radio button.
 *  \param x1 (x1,y1) Upper left corner. 
 *  \param y1 (x1,y1) Upper left corner. 
 *  \param x2 (x2,y2) Down right corner. 
 *  \param y2 (x2,y2) Down right corner. 
 *  \param group Radio button group.
 *  \param initialvalue If true, button is created in "checked" state and resets other buttons in same group. */
NE_APIObj * NE_APIRadioButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2, int group, bool initialvalue);

/*! \fn    NE_APIObj * NE_APISlideBarCreate(s16 x1, s16 y1, s16 x2, s16 y2, int min, int max, int initialvalue);
 *  \brief Returns a pointer to a slide bar. It checks if it is vertical or horizontal.
 *  \param x1 (x1,y1) Upper left corner. 
 *  \param y1 (x1,y1) Upper left corner. 
 *  \param x2 (x2,y2) Down right corner. 
 *  \param y2 (x2,y2) Down right corner. 
 *  \param min Min. value (it can be negative).
 *  \param max Max. value (it can be negative).
 *  \param initialvalue Initial value. 

You shouldn't give too big numbers to min and max. Sliding button's size changes between 1 and 100 of difference.*/
NE_APIObj * NE_APISlideBarCreate(s16 x1, s16 y1, s16 x2, s16 y2, int min, int max, int initialvalue);

//-----------------------------------------------------------------------

/*! \fn    void NE_APIButtonConfig(NE_APIObj * btn, NE_Material * material, u32 color, u32 alpha, NE_Material * pressedmaterial, u32 pressedcolor, u32 pressedalpha);
 *  \brief Configures given button's material, color and alpha.
 *  \param btn Button to configure. 
 *  \param material Default (not-pressed) texture. 
 *  \param color Default (not-pressed) color. 
 *  \param alpha Default (not-pressed) alpha value... 
 *  \param pressedmaterial Pressed texture.
 *  \param pressedcolor Pressed color...
 *  \param pressedalpha Pressed alpha value... */
void NE_APIButtonConfig(NE_APIObj * btn, NE_Material * material, u32 color, u32 alpha, 
						NE_Material * pressedmaterial, u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_APICheckBoxConfig(NE_APIObj * chbx, NE_Material * materialtrue, NE_Material * materialfalse, u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha);
 *  \brief Configures given check box's material, color and alpha.
 *  \param chbx Check box to configure. 
 *  \param materialtrue Texture used when is checked. 
 *  \param materialfalse Texture used when is NOT checked. 
 *  \param color Color when not pressed. 
 *  \param alpha Alpha when not pressed.
 *  \param pressedcolor Color when pressed. 
 *  \param pressedalpha Alpha when pressed. */
void NE_APICheckBoxConfig(NE_APIObj * chbx, NE_Material * materialtrue, NE_Material * materialfalse,
						u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_APIRadioButtonConfig(NE_APIObj * rdbtn, NE_Material * materialtrue, NE_Material * materialfalse, u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha);
 *  \brief Configures given radio button's material, color and alpha.
 *  \param rdbtn Radio button to configure. 
 *  \param materialtrue Texture used when is checked. 
 *  \param materialfalse Texture used when is NOT checked. 
 *  \param color Color when not pressed. 
 *  \param alpha Alpha when not pressed.
 *  \param pressedcolor Color when pressed. 
 *  \param pressedalpha Alpha when pressed. */						
void NE_APIRadioButtonConfig(NE_APIObj * rdbtn, NE_Material * materialtrue, NE_Material * materialfalse,
						u32 color, u32 alpha, u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_APISlideBarConfig(NE_APIObj * sldbar, NE_Material * matbtn, NE_Material * matbarbtn, NE_Material * matbar, u32 normalcolor, u32 pressedcolor, u32 barcolor, u32 alpha, u32 pressedalpha, u32 baralpha);
 *  \brief Configures given slide bar's material, color and alpha.
 *  \param sldbar Slide bar to configure. 
 *  \param matbtn Button's texture.
 *  \param matbarbtn Sliding button's texture.
 *  \param matbar Bar texture.
 *  \param normalcolor Normal button's color.
 *  \param pressedcolor Pressed button's color.
 *  \param barcolor Bar color.
 *  \param alpha Normal button's alpha.
 *  \param pressedalpha Pressed button's alpha.
 *  \param baralpha Bar alpha. */	
void NE_APISlideBarConfig(NE_APIObj * sldbar, NE_Material * matbtn, NE_Material * matbarbtn,
		NE_Material * matbar, u32 normalcolor, u32 pressedcolor, u32 barcolor, u32 alpha, 
		u32 pressedalpha, u32 baralpha);

/*! \fn    void NE_APISlideBarSetMinMax(NE_APIObj * sldbr, int min, int max);
 *  \brief Configures given slide bar's min. and max. values.
 *  \param sldbr Slide bar to configure. 
 *  \param min Min (it can be negative).
 *  \param max Max (it can be negative). 

You shouldn't give too big numbers. Sliding button's size changes between 1 and 100 of difference. */	
void NE_APISlideBarSetMinMax(NE_APIObj * sldbr, int min, int max);
//-----------------------------------------------------------------------

/*! \fn    NE_APIState NE_APIObjectGetEvent(NE_APIObj * obj);
 *  \brief Returns current event of any object.
 *  \param obj Pointer to the object. 

It returns a NE_APIState element. 
If obj is a slide bar, returns (event_button_a | event_button_b | event_button_bar). */
NE_APIState NE_APIObjectGetEvent(NE_APIObj * obj);

/*! \fn    bool NE_APICheckBoxGetValue(NE_APIObj * chbx);
 *  \brief Returns true if given check box is checked.
 *  \param chbx Check box. */
bool NE_APICheckBoxGetValue(NE_APIObj * chbx);

/*! \fn    bool NE_APIRadioButtonGetValue(NE_APIObj * rdbtn);
 *  \brief Returns true if given radio button is checked.
 *  \param rdbtn Radio button. */
bool NE_APIRadioButtonGetValue(NE_APIObj * rdbtn);

/*! \fn    int NE_APISlideBarGetValue(NE_APIObj * sldbr);
 *  \brief Returns given slide bar's value.
 *  \param sldbr Slide bar. */
int NE_APISlideBarGetValue(NE_APIObj * sldbr);

//-----------------------------------------------------------------------

/*! \fn    void NE_APIDeleteObject(NE_APIObj * obj);
 *  \brief Deletes given api object.
 *  \param obj Object. */
void NE_APIDeleteObject(NE_APIObj * obj);

/*! \fn    void NE_APIDeleteAll(void);
 *  \brief Deletes all api objects. */
void NE_APIDeleteAll(void);

/*! \fn    void NE_APISystemReset(int number_of_objects);
 *  \brief Resets the API system and sets the maximun number of objects.
 *  \param number_of_objects Number of objects. If it is less than 1, it will 
           create space for NE_API_DEFAULT_OBJECTS.  */
void NE_APISystemReset(int number_of_objects);

/*! \fn    void NE_APISystemEnd(void);
 *  \brief Ends API system and all memory used by it. */
void NE_APISystemEnd(void);


/*! @} */ //api_system

#endif //__NE_API_H__
