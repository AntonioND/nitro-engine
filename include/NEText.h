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

#ifndef __NE_TEXT_H__
#define __NE_TEXT_H__

/*! \file   NEText.h
 *  \brief  Text system. */

/*! @defgroup text_system Text system.

	Text drawing functions using the 3D hardware. Functions accept '\n', but don't '%d', '%s'...
	You have to call NE_2DViewInit() before using any of them.
	@{
*/

#define NE_MAX_TEXT_FONTS 8 /*! \def   #define NE_MAX_TEXT_FONTS 8 */

/*! \fn    void NE_TextPrioritySet(int priority);
 *  \brief Change priority of text drawn after this. 
 *  \param priority New priority. */
void NE_TextPrioritySet(int priority);

/*! \fn    void NE_TextPriorityReset(void);
 *  \brief Set priority of text drawn after this to 0. */
void NE_TextPriorityReset(void);

/*! \fn    void NE_TextInit(int slot, NE_Material * mat, int sizex, int sizey);
 *  \brief Configures given texture and dimensions as a text font. 
 *  \param slot Text slot.
 *  \param mat Font texture.
 *  \param sizex (sizex,sizey) Size of one character.
 *  \param sizey (sizex,sizey) Size of one character. */
void NE_TextInit(int slot, NE_Material * mat, int sizex, int sizey);

/*! \fn    void NE_TextEnd(int slot);
 *  \brief Resets given text slot. 
 *  \param slot Text slot. */
void NE_TextEnd(int slot);

/*! \fn    void NE_TextResetSystem(void);
 *  \brief Resets all text slots. */
void NE_TextResetSystem(void);

/*! \fn    int NE_TextPrint(int slot, int x, int y, u32 color, const char * text);
 *  \brief Prints text.
 *  \param slot Text slot to use. 
 *  \param x Row.
 *  \param y Column.
 *  \param color Text color.
 *  \param text Text to print. */
int NE_TextPrint(int slot, int x, int y, u32 color, const char * text);

/*! \fn    int NE_TextPrintBox(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text);
 *  \brief Prints text in a rectangle. 
 *  \param slot Text slot to use. 
 *  \param x Start row.
 *  \param y Start column.
 *  \param endx End row.
 *  \param endy End column.
 *  \param color Text color.
 *  \param charnum Number of characters to print. If -1 prints everything.
 *  \param text Text to print. */
int NE_TextPrintBox(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text);

/*! \fn    int NE_TextPrintFree(int slot, int x, int y, u32 color, const char * text);
 *  \brief Prints text in any coordinates. 
 *  \param slot Text slot to use. 
 *  \param x (x,y) Start position.
 *  \param y (x,y) Start position.
 *  \param color Text color.
 *  \param text Text to print. 
 
NOTE: '\n' does NOT work in this function. */
int NE_TextPrintFree(int slot, int x, int y, u32 color, const char * text);

/*! \fn    int NE_TextPrintBoxFree(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text);
 *  \brief Prints text in a box of given coordinates. 
 *  \param slot Text slot to use. 
 *  \param x (x,y) Start position.
 *  \param y (x,y) Start position.
 *  \param endx (endx,endy) End position.
 *  \param endy (endx,endy) End position.
 *  \param color Text color.
 *  \param charnum Number of characters to print. If -1 prints everything.
 *  \param text Text to print. */
int NE_TextPrintBoxFree(int slot, int x, int y, int endx, int endy, u32 color, int charnum, const char * text);

/*! @} */ //text_system

#endif //__NE_TEXT_H__
