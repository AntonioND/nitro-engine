// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_2D_H__
#define NE_2D_H__

#include <nds.h>

#include "NETexture.h"

/// @file   NE2D.h
/// @brief  2D over 3D system.

/// @defgroup 2d_sprites 2D Sprite System
///
/// Functions to draw sprites in 2D using the 3D hardware. Sprites are entities
/// that can have a rotation, material, color, alpha value, etc.
///
/// @{

#define NE_DEFAULT_SPRITES 128 ///< Default max number of sprites.

/// Holds information of a 2D sprite.
typedef struct {
    s16 x;            ///< X position in pixels
    s16 y;            ///< Y position in pixels
    s16 w;            ///< Width in pixels
    s16 h;            ///< Height in pixels
    int rot_angle;    ///< Rotation
    int xscale;       ///< Scale X (f32)
    int yscale;       ///< Scale Y (f32)
    int priority;     ///< Priority (Z coordinate)
    u32 color;        ///< Color
    NE_Material *mat; ///< Material
    s16 tl;           ///< Left coordinate of the texture canvas
    s16 tr;           ///< Right coordinate of the texture canvas
    s16 tt;           ///< Top coordinate of the texture canvas
    s16 tb;           ///< Bottom coordinate of the texture canvas
    bool visible;     ///< true if visible, false if not
    u8 alpha;         ///< Alpha value
    u8 id;            ///< Polygon ID
} NE_Sprite;

/// Creates a new sprite.
///
/// @return Pointer to the newly created sprite.
NE_Sprite *NE_SpriteCreate(void);

/// Set position of the provided sprite.
///
/// @param sprite Sprite to be moved.
/// @param x New position (x, y).
/// @param y New position (x, y).
void NE_SpriteSetPos(NE_Sprite *sprite, int x, int y);

/// Set size of a sprite.
///
/// If you want to flip the sprite, just use a negative value.
///
/// @param sprite Sprite to be resized.
/// @param w (w, h) New size (negative = flip).
/// @param h (w, h) New size (negative = flip).
void NE_SpriteSetSize(NE_Sprite *sprite, int w, int h);

/// Rotates a sprite.
///
/// If the angle is 0, no operations will be done when drawing the sprite. If
/// the angle isn't 0, the sprite will be rotated according to the specified
/// angle, using the center of the sprite as rotation center.
///
/// @param sprite Sprite to be rotated.
/// @param angle Angle to rotate the sprite.
void NE_SpriteSetRot(NE_Sprite *sprite, int angle);

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (f32).
void NE_SpriteSetScaleI(NE_Sprite *sprite, int scale);

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (float).
#define NE_SpriteSetScale(sprite, scale) \
    NE_SpriteSetScaleI(sprite, floattof32(scale))

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (f32).
void NE_SpriteSetXScaleI(NE_Sprite *sprite, int scale);

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (float).
#define NE_SpriteSetXScale(sprite, scale) \
    NE_SpriteSetXScaleI(sprite, floattof32(scale))

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (f32).
void NE_SpriteSetYScaleI(NE_Sprite *sprite, int scale);

/// Scales a sprite from its center.
///
/// @param sprite Sprite to be scaled.
/// @param scale Scale factor (float).
#define NE_SpriteSetYScale(sprite, scale) \
    NE_SpriteSetYScaleI(sprite, floattof32(scale))

/// Assign a material to a sprite.
///
/// This will also set the size of the sprite to the size of the texture of that
/// material.
///
/// @param sprite Sprite.
/// @param mat Material.
void NE_SpriteSetMaterial(NE_Sprite *sprite, NE_Material *mat);

/// Defines the part of the texture that is used when drawing the sprite.
///
/// This is useful if you have a texture with multiple images and you need to
/// define a specific part of the texture for a sprite, but other parts of the
/// texture for others.
///
/// @param sprite Sprite.
/// @param tl Texture left X coordinate.
/// @param tt Texture top Y coordinate.
/// @param tr Texture right X coordinate.
/// @param tb Texture bottom Y coordinate.
void NE_SpriteSetMaterialCanvas(NE_Sprite *sprite, int tl, int tt, int tr, int tb);

/// Set priority of a sprite.
///
/// The lower the vaue is, the higher the priority is. High priority sprites are
/// drawn over low priority sprites.
///
/// @param sprite Sprite.
/// @param priority New priority.
void NE_SpriteSetPriority(NE_Sprite *sprite, int priority);

/// Makes a sprite visible or invisible.
///
/// @param sprite Sprite.
/// @param visible true = visible, false = invisible.
void NE_SpriteVisible(NE_Sprite *sprite, bool visible);

/// Set some parameters of a sprite.
///
/// If you want to make a sprite transparent over another transparent sprite
/// they need to have different polygon IDs.
///
/// An alpha value of 0 makes the sprite wireframe, not invisible.
///
/// @param sprite Sprite.
/// @param alpha Alpha value for that sprite (0 - 31).
/// @param id Polygon ID of the sprite.
/// @param color Color.
void NE_SpriteSetParams(NE_Sprite *sprite, u8 alpha, u8 id, u32 color);

/// Delete a sprite struct and free memory used by it.
///
/// @param sprite Sprite to be deleted.
void NE_SpriteDelete(NE_Sprite *sprite);

/// Delete all sprites and free all memory used by them.
void NE_SpriteDeleteAll(void);

/// Resets the sprite system and sets the maximun number of sprites.
///
/// @param max_sprites Number of sprites. If it is lower than 1, it will create
///                    space for NE_DEFAULT_SPRITES.
/// @return Returns 0 on success.
int NE_SpriteSystemReset(int max_sprites);

/// Ends sprite system and all memory used by it.
void NE_SpriteSystemEnd(void);

/// Draws the selected sprite.
///
/// You have to call NE_2DViewInit() before drawing any sprite with this
/// function.
///
/// @param sprite Sprite to be drawn.
void NE_SpriteDraw(const NE_Sprite *sprite);

/// Draws all visible sprites.
///
/// You have to call NE_2DViewInit() before drawing any sprite with this
/// function.
void NE_SpriteDrawAll(void);

/// @}

/// @defgroup 2d_view 2D View System
///
/// Functions to manipulate the 2D view in 3D mode and to draw polygons by hand.
///
/// @{

/// Setup a 2D view in 3D mode.
///
/// Loads an orthogonal matrix to the DS geometry engine so that it is possible
/// to draw 2D elements using 3D hardware easily. The matrix is formed so that
/// the X and Y coordinates of a polygon correspond to the X and Y coordinates
/// of the screen.
void NE_2DViewInit(void);

/// Rotates the current 2D view from the specified point.
///
/// Be careful if you print text after this. Text functions are prepared to avoid
/// unnecessary work. For example, they don't draw text out of the screen. And
/// they don't know if you have rotated the view or not. If you want to draw
/// rotated text, be careful.
///
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param rotz Angle (0-512) to rotate on the Z axis.
void NE_2DViewRotateByPosition(int x, int y, int rotz);

/// Scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param scale Scale factor (f32).
void NE_2DViewScaleByPositionI(int x, int y, int scale);

/// Scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param s Scale factor (float).
#define NE_2DViewScaleByPosition(x, y, s) \
    NE_2DViewScaleByPositionI(x, y, floattof32(s))

/// Scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param xscale X axis scale factor (f32).
/// @param yscale Y axis scale factor (f32).
void NE_2DViewScaleByPositionXYI(int x, int y, int xscale, int yscale);

/// Scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param xs x axis scale factor (float).
/// @param ys y axis scale factor (float).
#define NE_2DViewScaleByPositionXY(x, y, xs, xy) \
    NE_2DViewScaleByPositionXYI(x, y, floattof32(xs), floattof32(xy))

/// Rotates and scales the current 2D view from the specified point.
///
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param rotz Angle (0-512) to rotate on the Z axis.
/// @param scale Scale factor (f32).
void NE_2DViewRotateScaleByPositionI(int x, int y, int rotz, int scale);

/// Rotates and scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param r Angle (0-512) to rotate on the Z axis.
/// @param s Scale factor (float).
#define NE_2DViewRotateScaleByPosition(x, y, r, s) \
    NE_2DViewRotateScaleByPositionI(x, y, r, floattof32(s))

/// Rotates and scales the current 2D view from the specified point.
///
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param rotz Angle (0-512) to rotate on the Z axis.
/// @param xscale X axis scale factor (f32).
/// @param yscale Y axis scale factor (f32).
void NE_2DViewRotateScaleByPositionXYI(int x, int y, int rotz, int xscale, int yscale);

/// Rotates and scales the current 2D view from the specified point.
//
/// @param x (x, y) Coordinates.
/// @param y (x, y) Coordinates.
/// @param r Angle (0-512) to rotate on the Z axis.
/// @param xs x axis scale factor (float).
/// @param ys y axis scale factor (float).
#define NE_2DViewRotateScaleByPositionXY(x, y, r, xs, ys) \
    NE_2DViewRotateScaleByPositionXYI(x, y, r, floattof32(xs), floattof32(ys))

/// Draws a quad at the given coordinates with a flat color.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param color Quad color.
void NE_2DDrawQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color);

/// Draws a quad at the given coordinates with a color gradient.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param color1 Upper left color.
/// @param color2 Upper right color.
/// @param color3 Lower right color.
/// @param color4 Lower left color.
void NE_2DDrawQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z, u32 color1,
                           u32 color2, u32 color3, u32 color4);

/// Draws a quad with a material at the given coordinates.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param mat Material to use.
void NE_2DDrawTexturedQuad(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
                           const NE_Material *mat);

/// Draws a quad with a material at the given coordinates with a flat color.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param mat Material to use.
/// @param color Color
void NE_2DDrawTexturedQuadColor(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
                                const NE_Material *mat, u32 color);

/// Draws a quad with a material at the given coordinates with a color gradient.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param mat Material to use.
/// @param color1 Upper left color.
/// @param color2 Upper right color.
/// @param color3 Lower right color.
/// @param color4 Lower left color.
void NE_2DDrawTexturedQuadGradient(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
                                   const NE_Material *mat, u32 color1,
                                   u32 color2, u32 color3, u32 color4);

/// Draws a quad with a material at the given coordinates with a flat color and
/// with the specified texture coordinates.
///
/// @param x1 (x1, y1) Upper - left vertex.
/// @param y1 (x1, y1) Upper - left vertex.
/// @param x2 (x2, y2) Lower - right vertex.
/// @param y2 (x2, y2) Lower - right vertex.
/// @param z Priority.
/// @param tl Texture left X coordinate.
/// @param tt Texture top Y coordinate.
/// @param tr Texture right X coordinate.
/// @param tb Texture bottom Y coordinate.
/// @param mat Material to use.
/// @param color Color
void NE_2DDrawTexturedQuadColorCanvas(s16 x1, s16 y1, s16 x2, s16 y2, s16 z,
                                      int tl, int tt, int tr, int tb,
                                      const NE_Material *mat, u32 color);

/// @}

#endif // NE_2D_H__
