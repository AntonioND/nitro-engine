// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_GUI_H__
#define NE_GUI_H__

#include "NEMain.h"

/*! \file   NEGUI.h
 *  \brief  GUI functions. */

/*! @defgroup gui_system GUI System.
 *
 * Functions to create buttons, check boxes, radio buttons and slidebars.
 *
 * @{
*/

/*! \def   #define NE_GUI_DEFAULT_OBJECTS    64 */
#define NE_GUI_DEFAULT_OBJECTS      64

#define NE_GUI_POLY_ID       62	/*! \def   #define NE_GUI_POLY_ID     62 */
#define NE_GUI_POLY_ID_ALT   61	/*! \def   #define NE_GUI_POLY_ID_ALT 61 */

/*! \def   #define NE_GUI_MIN_PRIORITY 100 */
#define NE_GUI_MIN_PRIORITY 100

/*! \enum  NE_GUITypes
 *  \brief GUI object types.
 */
typedef enum {
	NE_Button = 1,	/*!< Simple button. */
	NE_CheckBox,	/*!< Check Box. */
	NE_RadioButton,	/*!< Radio button. */
	NE_SlideBar	/*!< Slide bar. */
} NE_GUITypes;

/*! \enum  NE_GUIState
 *  \brief GUI states.
 */
typedef enum {
	NE_Error	= -1,		/*!< GUI hasn't been updated or input hasn't been updated. */
	NE_None		= 0,		/*!< Not pressed. */
	NE_Pressed	= 1,		/*!< Just pressed. */
	NE_Held		= 1 << 1,	/*!< Held. */
	NE_Clicked	= 1 << 2	/*!< Just released. */
} NE_GUIState;

/*! \struct NE_GUIObj
 *  \brief  Holds information of an GUI object.
 */
typedef struct {
	NE_GUITypes type;
	void *pointer;
} NE_GUIObj;

//------------------------------------------------------------------------------

/*! \fn    void NE_GUIUpdate(void);
 *  \brief Updates all GUI objects. NE_UpdateInput() must be called each frame
 *         for this to work.
 */
void NE_GUIUpdate(void);

/*! \fn    void NE_GUIDraw(void);
 *  \brief Draw all elements. You have to call NE_2DViewInit() before this.
 */
void NE_GUIDraw(void);

//------------------------------------------------------------------------------

/*! \fn    NE_GUIObj *NE_GUIButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2);
 *  \brief Returns a pointer to a new button.
 *  \param x1 (x1,y1) Upper left corner.
 *  \param y1 (x1,y1) Upper left corner.
 *  \param x2 (x2,y2) Down right corner.
 *  \param y2 (x2,y2) Down right corner.
 */
NE_GUIObj *NE_GUIButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2);

/*! \fn    NE_GUIObj *NE_GUICheckBoxCreate(s16 x1, s16 y1, s16 x2, s16 y2,
 *                                         bool initialvalue);
 *  \brief Returns a pointer to a new check button.
 *  \param x1 (x1,y1) Upper left corner.
 *  \param y1 (x1,y1) Upper left corner.
 *  \param x2 (x2,y2) Down right corner.
 *  \param y2 (x2,y2) Down right corner.
 *  \param initialvalue If true, checkbox is created in "checked" state.
 */
NE_GUIObj *NE_GUICheckBoxCreate(s16 x1, s16 y1, s16 x2, s16 y2,
				bool initialvalue);

/*! \fn    NE_GUIObj *NE_GUIRadioButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2,
 *                                            int group, bool initialvalue);
 *  \brief Returns a pointer to a new radio button.
 *  \param x1 (x1,y1) Upper left corner.
 *  \param y1 (x1,y1) Upper left corner.
 *  \param x2 (x2,y2) Down right corner.
 *  \param y2 (x2,y2) Down right corner.
 *  \param group Radio button group.
 *  \param initialvalue If true, button is created in "checked" state and resets
 *         other buttons in same group.
 */
NE_GUIObj *NE_GUIRadioButtonCreate(s16 x1, s16 y1, s16 x2, s16 y2, int group,
				   bool initialvalue);

/*! \fn    NE_GUIObj *NE_GUISlideBarCreate(s16 x1, s16 y1, s16 x2, s16 y2,
 *                                         int min, int max, int initialvalue);
 *  \brief Returns a pointer to a slide bar. It checks if it is vertical or
 *         horizontal.
 *  \param x1 (x1,y1) Upper left corner.
 *  \param y1 (x1,y1) Upper left corner.
 *  \param x2 (x2,y2) Down right corner.
 *  \param y2 (x2,y2) Down right corner.
 *  \param min Min. value (it can be negative).
 *  \param max Max. value (it can be negative).
 *  \param initialvalue Initial value. You shouldn't give too big numbers to
 *         min and max. Sliding button's size changes between 1 and 100 of
 *         difference.
 */
NE_GUIObj *NE_GUISlideBarCreate(s16 x1, s16 y1, s16 x2, s16 y2, int min,
				int max, int initialvalue);

//------------------------------------------------------------------------------

/*! \fn    void NE_GUIButtonConfig(NE_GUIObj *btn, NE_Material *material,
 *                                 u32 color, u32 alpha,
 *                                 NE_Material *pressedmaterial,
 *                                 u32 pressedcolor, u32 pressedalpha);
 *  \brief Configures given button's material, color and alpha.
 *  \param btn Button to configure.
 *  \param material Default (not-pressed) texture.
 *  \param color Default (not-pressed) color.
 *  \param alpha Default (not-pressed) alpha value...
 *  \param pressedmaterial Pressed texture.
 *  \param pressedcolor Pressed color...
 *  \param pressedalpha Pressed alpha value...
 */
void NE_GUIButtonConfig(NE_GUIObj *btn, NE_Material *material, u32 color,
			u32 alpha, NE_Material *pressedmaterial,
			u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_GUICheckBoxConfig(NE_GUIObj *chbx, NE_Material *materialtrue,
 *                                   NE_Material *materialfalse, u32 color,
 *                                   u32 alpha, u32 pressedcolor,
 *                                   u32 pressedalpha);
 *  \brief Configures given check box's material, color and alpha.
 *  \param chbx Check box to configure.
 *  \param materialtrue Texture used when is checked.
 *  \param materialfalse Texture used when is NOT checked.
 *  \param color Color when not pressed.
 *  \param alpha Alpha when not pressed.
 *  \param pressedcolor Color when pressed.
 *  \param pressedalpha Alpha when pressed.
 */
void NE_GUICheckBoxConfig(NE_GUIObj *chbx, NE_Material *materialtrue,
			  NE_Material *materialfalse, u32 color, u32 alpha,
			  u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_GUIRadioButtonConfig(NE_GUIObj *rdbtn,
 *                                      NE_Material *materialtrue,
 *                                      NE_Material *materialfalse, u32 color,
 *                                      u32 alpha, u32 pressedcolor,
 *                                      u32 pressedalpha);
 *  \brief Configures given radio button's material, color and alpha.
 *  \param rdbtn Radio button to configure.
 *  \param materialtrue Texture used when is checked.
 *  \param materialfalse Texture used when is NOT checked.
 *  \param color Color when not pressed.
 *  \param alpha Alpha when not pressed.
 *  \param pressedcolor Color when pressed.
 *  \param pressedalpha Alpha when pressed.
 */
void NE_GUIRadioButtonConfig(NE_GUIObj *rdbtn, NE_Material *materialtrue,
			     NE_Material *materialfalse, u32 color, u32 alpha,
			     u32 pressedcolor, u32 pressedalpha);

/*! \fn    void NE_GUISlideBarConfig(NE_GUIObj *sldbar, NE_Material *matbtn,
 *                                   NE_Material *matbarbtn,
 *                                   NE_Material *matbar, u32 normalcolor,
 *                                   u32 pressedcolor, u32 barcolor, u32 alpha,
 *                                   u32 pressedalpha, u32 baralpha);
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
 *  \param baralpha Bar alpha.
 */
void NE_GUISlideBarConfig(NE_GUIObj *sldbar, NE_Material *matbtn,
			  NE_Material *matbarbtn, NE_Material *matbar,
			  u32 normalcolor, u32 pressedcolor, u32 barcolor,
			  u32 alpha, u32 pressedalpha, u32 baralpha);

/*! \fn    void NE_GUISlideBarSetMinMax(NE_GUIObj *sldbr, int min, int max);
 *  \brief Configures given slide bar's min. and max. values.
 *  \param sldbr Slide bar to configure.
 *  \param min Min (it can be negative).
 *  \param max Max (it can be negative).
 *
 * You shouldn't give too big numbers. Sliding button's size changes between 1
 * and 100 of difference.
 */
void NE_GUISlideBarSetMinMax(NE_GUIObj *sldbr, int min, int max);

//------------------------------------------------------------------------------

/*! \fn    NE_GUIState NE_GUIObjectGetEvent(NE_GUIObj *obj);
 *  \brief Returns current event of any object.
 *  \param obj Pointer to the object.
 *
 * It returns a NE_GUIState element. If obj is a slide bar, returns
 * (event_button_a | event_button_b | event_button_bar).
 */
NE_GUIState NE_GUIObjectGetEvent(NE_GUIObj *obj);

/*! \fn    bool NE_GUICheckBoxGetValue(NE_GUIObj *chbx);
 *  \brief Returns true if given check box is checked.
 *  \param chbx Check box.
 */
bool NE_GUICheckBoxGetValue(NE_GUIObj *chbx);

/*! \fn    bool NE_GUIRadioButtonGetValue(NE_GUIObj *rdbtn);
 *  \brief Returns true if given radio button is checked.
 *  \param rdbtn Radio button.
 */
bool NE_GUIRadioButtonGetValue(NE_GUIObj *rdbtn);

/*! \fn    int NE_GUISlideBarGetValue(NE_GUIObj *sldbr);
 *  \brief Returns given slide bar's value.
 *  \param sldbr Slide bar.
 */
int NE_GUISlideBarGetValue(NE_GUIObj *sldbr);

//------------------------------------------------------------------------------

/*! \fn    void NE_GUIDeleteObject(NE_GUIObj *obj);
 *  \brief Deletes given object.
 *  \param obj Object.
 */
void NE_GUIDeleteObject(NE_GUIObj *obj);

/*! \fn    void NE_GUIDeleteAll(void);
 *  \brief Deletes all objects.
 */
void NE_GUIDeleteAll(void);

/*! \fn    void NE_GUISystemReset(int number_of_objects);
 *  \brief Resets the GUI system and sets the maximun number of objects.
 *  \param number_of_objects Number of objects. If it is less than 1, it will
 *         create space for NE_GUI_DEFAULT_OBJECTS.
 */
void NE_GUISystemReset(int number_of_objects);

/*! \fn    void NE_GUISystemEnd(void);
 *  \brief Ends GUI system and all memory used by it.
 */
void NE_GUISystemEnd(void);

/*! @} */

#endif // NE_GUI_H__
