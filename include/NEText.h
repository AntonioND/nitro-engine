// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_TEXT_H__
#define NE_TEXT_H__

/// @file  NEText.h
/// @brief Text system.

/// @defgroup text_system Text system
///
/// Monospaced text drawing functions using the 3D hardware. Use the rich text
/// module if you want a more useful text system.
///
/// The functions accept '\n', but not things like '%d', '%s', etc. You'll need
/// to use snprintf() or similar for that.
///
/// You need to call NE_2DViewInit() before using any of the text drawing
/// functions.
///
/// @{

#define NE_MAX_TEXT_FONTS 8 ///< Default max number of text fonts

/// Change the priority of text drawn after this function call.
///
/// @param priority New priority.
void NE_TextPrioritySet(int priority);

/// Set to 0 the priority of text drawn after this function call.
void NE_TextPriorityReset(void);

/// Assigns a material as a font in a font slot.
///
/// @param slot Text font slot.
/// @param mat Font texture.
/// @param sizex (sizex, sizey) Size of one character.
/// @param sizey (sizex, sizey) Size of one character.
void NE_TextInit(int slot, const NE_Material *mat, int sizex, int sizey);

/// Clears the given text font slot.
///
/// @param slot Text font slot.
void NE_TextEnd(int slot);

/// Clears all text font slots.
void NE_TextResetSystem(void);

/// Prints text.
///
/// '\n' is supported. '%d', '%s', etc aren't supported.
///
/// @param slot Text font slot to use.
/// @param x Row.
/// @param y Column.
/// @param color Text color.
/// @param text Text to print.
/// @return Returns the number of characters printed.
int NE_TextPrint(int slot, int x, int y, u32 color, const char *text);

/// Prints text within the limits of a rectangle.
///
/// '\n' is supported. '%d', '%s', etc aren't supported.
//
/// @param slot Text font slot to use.
/// @param x Start row.
/// @param y Start column.
/// @param endx End row.
/// @param endy End column.
/// @param color Text color.
/// @param charnum Number of characters to print. If -1 it prints everything.
/// @param text Text to print.
/// @return Returns the number of characters printed.
int NE_TextPrintBox(int slot, int x, int y, int endx, int endy, u32 color,
                    int charnum, const char *text);

/// Prints text which isn't restricted to any row or column.
///
/// '\n' does not work with this function.
///
/// @param slot Text font slot to use.
/// @param x (x, y) Start position in pixels.
/// @param y (x, y) Start position in pixels.
/// @param color Text color.
/// @param text Text to print.
/// @return Returns the number of characters printed.
int NE_TextPrintFree(int slot, int x, int y, u32 color, const char *text);

/// Prints text which isn't restricted to any row or column, within a rectangle.
///
/// @param slot Text font slot to use.
/// @param x (x, y) Start position in pixels.
/// @param y (x, y) Start position in pixels.
/// @param endx (endx, endy) End position in pixels.
/// @param endy (endx, endy) End position in pixels.
/// @param color Text color.
/// @param charnum Number of characters to print. If -1 it prints everything.
/// @param text Text to print.
/// @return Returns the number of characters printed.
int NE_TextPrintBoxFree(int slot, int x, int y, int endx, int endy, u32 color,
            int charnum, const char *text);

/// @}

#endif // NE_TEXT_H__
