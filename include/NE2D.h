// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_2D_H__
#define NE_2D_H__

#include <nds.h>

#include "NETexture.h"

/*! \file   NE2D.h
 *  \brief  2D over 3D system.
 */

/*! @defgroup 2d_system 2D System.
 *
 * Functions to draw in 2D using the 3D hardware. If you want to draw quads
 * but you don't want to save their coordinates, transparency, etc... use
 * sprites, they save the information for you and you can draw them all using
 * just a function.
 *
 * If you want to draw polygons by yourself, use the X and Y coordinates only.
 * Z axis is distance from the camera.
 *
 * @{
 */

#define NE_DEFAULT_SPRITES 128	/*! \def #define NE_DEFAULT_SPRITES 128 */

/*! \struct NE_Sprite
 *  \brief  Holds information for a 2D sprite.
 */
typedef struct {
	s16 x, y;
	s16 sx, sy;
	int rot_angle;
	int scale;
	int priority;
	u32 color;
	NE_Material *mat;
	bool visible;
	u8 alpha, id;
} NE_Sprite;

/*! \fn    NE_Sprite *NE_SpriteCreate(void);
 *  \brief Returns a pointer to a NE_Sprite struct.
 */
NE_Sprite *NE_SpriteCreate(void);

/*! \fn    void NE_SpriteSetPos(NE_Sprite *sprite, int x, int y);
 *  \brief Set position of given sprite.
 *  \param sprite Sprite to be moved.
 *  \param x (x,y) New position.
 *  \param y (x,y) New position.
 */
void NE_SpriteSetPos(NE_Sprite *sprite, int x, int y);

/*! \fn    void NE_SpriteSetSize(NE_Sprite *sprite, int x, int y);
 *  \brief Set size of a sprite. If you want to flip the sprite, just use a
 *         negative value.
 *  \param sprite Sprite to be resized.
 *  \param x (x,y) New size (can be negative).
 *  \param y (x,y) New size (can be negative).
 */
void NE_SpriteSetSize(NE_Sprite *sprite, int x, int y);

/*! \fn    void NE_SpriteSetRot(NE_Sprite *sprite, int angle);
 *  \brief Rotates a sprite.
 *  \param sprite Sprite to be rotated.
 *  \param angle Angle to rotate the sprite.
 *
 * If 0, no operations will be done when drawing the sprite. If different, 2D
 * view will be rotated using as center: spritecoordinates + (spritesize / 2).
 */
void NE_SpriteSetRot(NE_Sprite *sprite, int angle);

/*! \fn    void NE_SpriteSetScaleI(NE_Sprite *sprite, int scale);
 *  \brief Scales a sprite.
 *  \param sprite Sprite to be scaled.
 *  \param scale Scale factor.
 *
 * 2D view is scaled using as center the center of the sprite.
 */
void NE_SpriteSetScaleI(NE_Sprite *sprite, int scale);

/*! \def   NE_SpriteSetScale(NE_Sprite *sprite, float scale);
 *  \brief Scales a sprite.
 *  \param sprite Sprite to be scaled.
 *  \param scale Scale factor.
 *
 * 2D view is scaled using as center the center of the sprite.
 */
#define NE_SpriteSetScale(sprite, scale) \
	NE_SpriteSetScaleI(sprite, floattof32(scale))

/*! \fn    void NE_SpriteSetMaterial(NE_Sprite *sprite, NE_Material *mat);
 *  \brief Set material to a sprite.
 *  \param sprite Sprite.
 *  \param mat Material.
 */
void NE_SpriteSetMaterial(NE_Sprite *sprite, NE_Material *mat);

/*! \fn    void NE_SpriteSetPriority(NE_Sprite *sprite, int priority);
 *  \brief Set priority of a sprite. The lower the vaue is, the higher the
 *         priority is.
 *  \param sprite Sprite.
 *  \param priority New priority.
 */
void NE_SpriteSetPriority(NE_Sprite *sprite, int priority);

/*! \fn    void NE_SpriteVisible(NE_Sprite *sprite, bool visible);
 *  \brief Makes a sprite visible or invisible.
 *  \param sprite Sprite.
 *  \param visible True = visible, false = invisible.
 */
void NE_SpriteVisible(NE_Sprite *sprite, bool visible);

/*! \fn    void NE_SpriteSetParams(NE_Sprite *sprite, u8 alpha, u8 id,
 *                                 u32 color);
 *  \brief Set some parameters a sprite.
 *  \param sprite Sprite.
 *  \param alpha Alpha value for that sprite (0 - 31).
 *  \param id Polygon ID of the sprite.
 *  \param color Color.
 *
 * If you want to make a sprite transparent over another one transparent you
 * have to make them have different ID.
 */
void NE_SpriteSetParams(NE_Sprite *sprite, u8 alpha, u8 id, u32 color);

/*! \fn    void NE_SpriteDelete(NE_Sprite *sprite);
 *  \brief Delete a NE_Sprite struct and free memory used by it.
 *  \param sprite Sprite to be deleted.
 */
void NE_SpriteDelete(NE_Sprite *sprite);

/*! \fn    void NE_SpriteDeleteAll(void);
 *  \brief Delete all sprites.
 */
void NE_SpriteDeleteAll(void);

/*! \fn    void NE_SpriteSystemReset(int number_of_sprites);
 *  \brief Resets the sprite system and sets the maximun number of sprites.
 *  \param number_of_sprites Number of sprites. If it is less than 1, it will
 *         create space for NE_DEFAULT_SPRITES.
 */
void NE_SpriteSystemReset(int number_of_sprites);

/*! \fn    void NE_SpriteSystemEnd(void);
 *  \brief Ends sprite system and all memory used by it.
 */
void NE_SpriteSystemEnd(void);

/*! \fn    void NE_SpriteDraw(NE_Sprite *sprite);
 *  \brief Draws selected sprite. You have to use NE_2DViewInit before drawing
 *         any sprite with this function.
 *  \param sprite Sprite to be drawn.
 */
void NE_SpriteDraw(NE_Sprite *sprite);

/*! \fn    void NE_SpriteDrawAll(void);
 *  \brief Draws all visible sprites.
 *
 * You have to call NE_2DViewInit() before this.
 */
void NE_SpriteDrawAll(void);

//------------------------------------------------------------------------------

/*! \fn    void NE_2DViewInit(void);
 *  \brief Sets current view to a 3D orthogonal view to draw 2D objects using 3D
 *         hardware.
 */
void NE_2DViewInit(void);

/*! \fn    void NE_2DViewRotateByPosition(int x, int y, int rotz);
 *  \brief Rotates current 2D view by given cordinates.
 *  \param x (x,y) Coordinates.
 *  \param y (x,y) Coordinates.
 *  \param rotz Angle (0-512) to rotate on the Z axis.
 *
 * Be careful if you print text after this. Text functions are prepared to avoid
 * useless work, for example, they don't draw text out of the screen. They don't
 * know if you have rotated the view or not. If you want to draw rotated text be
 * careful, print as less as you can.
 */
void NE_2DViewRotateByPosition(int x, int y, int rotz);

/*! \fn    void NE_2DViewScaleByPositionI(int x, int y, int scale);
 *  \brief Scales current 2D view by given cordinates.
 *  \param x (x,y) Coordinates.
 *  \param y (x,y) Coordinates.
 *  \param scale Scale factor.
 */
void NE_2DViewScaleByPositionI(int x, int y, int scale);

/*! \def   NE_2DViewScaleByPosition(int x, int y, float s);
 *  \brief Scales current 2D view by given cordinates.
 *  \param x (x,y) Coordinates.
 *  \param y (x,y) Coordinates.
 *  \param scale Scale factor.
 */
#define NE_2DViewScaleByPosition(x, y, s) \
	NE_2DViewScaleByPositionI(x, y, floattof32(s))

/*! \fn    void NE_2DViewRotateScaleByPositionI(int x, int y, int rotz,
 *                                              int scale);
 *  \brief Rotates and scales current 2D view by given cordinates.
 *  \param x (x,y) Coordinates.
 *  \param y (x,y) Coordinates.
 *  \param rotz Angle (0-512) to rotate on the Z axis.
 *  \param scale Scale factor.
 */
void NE_2DViewRotateScaleByPositionI(int x, int y, int rotz, int scale);

/*! \fn    NE_2DViewRotateScaleByPosition(int x, int y, int r, float s);
 *  \brief Rotates and scales current 2D view by given cordinates.
 *  \param x (x,y) Coordinates.
 *  \param y (x,y) Coordinates.
 *  \param r Angle (0-512) to rotate on the Z axis.
 *  \param s Scale factor.
 */
#define NE_2DViewRotateScaleByPosition(x, y, r, s) \
	NE_2DViewRotateScaleByPositionI(x, y, r, floattof32(s))

/*! \fn    void NE_2DDrawQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color);
 *  \brief Draws a quad of given coordinates and color.
 *  \param x1 (x1, y1) Upper - left vertex.
 *  \param y1 (x1, y1) Upper - left vertex.
 *  \param x2 (x2, y2) Lower - right vertex.
 *  \param y2 (x2, y2) Lower - right vertex.
 *  \param z Priority.
 *  \param color Quad color.
 */
void NE_2DDrawQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color);

/*! \fn    void NE_2DDrawQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
 *                                    u32 color1, u32 color2, u32 color3,
 *                                    u32 color4);
 *  \brief Draws a quad of given coordinates with color gradient.
 *  \param x1 (x1, y1) Upper - left vertex.
 *  \param y1 (x1, y1) Upper - left vertex.
 *  \param x2 (x2, y2) Lower - right vertex.
 *  \param y2 (x2, y2) Lower - right vertex.
 *  \param z Priority.
 *  \param color1 Upper left color.
 *  \param color2 Upper right color.
 *  \param color3 Lower right color.
 *  \param color4 Lower left color.
 */
void NE_2DDrawQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color1,
			   u32 color2, u32 color3, u32 color4);

/*! \fn    void NE_2DDrawTexturedQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
 *                                    NE_Material *mat);
 *  \brief Draws a quad of given coordinates and material.
 *  \param x1 (x1, y1) Upper - left vertex.
 *  \param y1 (x1, y1) Upper - left vertex.
 *  \param x2 (x2, y2) Lower - right vertex.
 *  \param y2 (x2, y2) Lower - right vertex.
 *  \param z Priority.
 *  \param mat Material to use.
 */
void NE_2DDrawTexturedQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
			   NE_Material *mat);

/*! \fn    void NE_2DDrawTexturedQuadColor(s16 x1, s16 y1, s16 x2, s16 y2,
 *                                         s16 z, NE_Material *mat, u32 color);
 *  \brief Draws a quad of given coordinates and material with given color.
 *  \param x1 (x1, y1) Upper - left vertex.
 *  \param y1 (x1, y1) Upper - left vertex.
 *  \param x2 (x2, y2) Lower - right vertex.
 *  \param y2 (x2, y2) Lower - right vertex.
 *  \param z Priority.
 *  \param mat Material to use.
 *  \param color Color
 */
void NE_2DDrawTexturedQuadColor(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
				NE_Material *mat, u32 color);

/*! \fn    void NE_2DDrawTexturedQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2,
 *                                            s16 z, NE_Material *mat,
 *                                            u32 color1, u32 color2,
 *                                            u32 color3, u32 color4);
 *  \brief Draws a quad of given coordinates and material with color gradient.
 *  \param x1 (x1, y1) Upper - left vertex.
 *  \param y1 (x1, y1) Upper - left vertex.
 *  \param x2 (x2, y2) Lower - right vertex.
 *  \param y2 (x2, y2) Lower - right vertex.
 *  \param z Priority.
 *  \param mat Material to use.
 *  \param color1 Upper left color.
 *  \param color2 Upper right color.
 *  \param color3 Lower right color.
 *  \param color4 Lower left color.
 */
void NE_2DDrawTexturedQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
				   NE_Material *mat, u32 color1, u32 color2,
				   u32 color3, u32 color4);

/*! @} */

#endif // NE_2D_H__
