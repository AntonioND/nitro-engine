// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_POLYGON_H__
#define NE_POLYGON_H__

#include "NEMain.h"

/*! \file   NEPolygon.h
 *  \brief  Functions to draw polygons and more...
 */

/*! @defgroup other_ Other functions.
 *
 * Some functions to set lights and its propierties, to draw polygons, configure
 * the rear plane, etc...
 *
 * @{
*/

/*! \enum  NE_ColorEnum
 *  \brief Predefined colors.
 */
typedef enum {
	NE_Brown	= RGB15(10, 6, 1),	/*!< Brown. */
	NE_Red		= RGB15(31, 0, 0),	/*!< Red. */
	NE_Orange	= RGB15(31, 20, 0),	/*!< Orange. */
	NE_Yellow	= RGB15(31, 31, 0),	/*!< Yellow. */
	NE_LimeGreen	= RGB15(15, 31, 0),	/*!< Lime green. */
	NE_Green	= RGB15(0, 31, 0),	/*!< Green. */
	NE_DarkGreen	= RGB15(0, 15, 0),	/*!< Dark green. */
	NE_LightBlue	= RGB15(7, 15, 31),	/*!< Light blue. */
	NE_Blue		= RGB15(0, 0, 31),	/*!< Blue. */
	NE_DarkBlue	= RGB15(0, 6, 15),	/*!< Dark blue. */
	NE_Violet	= RGB15(28, 8, 28),	/*!< Violet. */
	NE_Pink		= RGB15(31, 15, 22),	/*!< Pink. */
	NE_Purple	= RGB15(20, 4, 14),	/*!< Purple. */
	NE_Indigo	= RGB15(15, 15, 30),	/*!< Purple. */
	NE_Magenta	= RGB15(31, 0, 31),	/*!< Magenta. */
	NE_White	= RGB15(31, 31, 31),	/*!< White. */
	NE_Gray		= RGB15(20, 20, 20),	/*!< Gray. */
	NE_DarkGray	= RGB15(10, 10, 10),	/*!< Dark gray. */
	NE_Black	= RGB15(0, 0, 0)	/*!< Black. */
} NE_ColorEnum;

/*! \fn    void NE_LightOff(int num);
 *  \brief Switch off a light.
 *  \param num Number of light to switch off (0 - 3).
 */
void NE_LightOff(int num);

/*! \fn    void NE_LightSetI(int num, u32 color, int x, int y, int z);
 *  \brief Switch on a light and set a color for it.
 *  \param num Number of light to switch on (0 - 3).
 *  \param color Color of the light.
 *  \param x (x, y, z) Vector of the light.
 *  \param y (x, y, z) Vector of the light.
 *  \param z (x, y, z) Vector of the light.
 */
void NE_LightSetI(int num, u32 color, int x, int y, int z);

/*! \fn    void NE_LightSetColor(int num, u32 color);
 *  \brief Sets the color of a light.
 *  \param num Number of light (0 - 3).
 *  \param color Color.
 */
void NE_LightSetColor(int num, u32 color);

/*! \def   NE_LightSet(int num, u32 color, float x, float y, float z);
 *  \brief Switch on a light and set a color for it.
 *  \param n Number of light to switch on (0 - 3).
 *  \param c color of the light.
 *  \param x (x, y, z) Vector of the light.
 *  \param y (x, y, z) Vector of the light.
 *  \param z (x, y, z) Vector of the light.
 */
#define NE_LightSet(n, c, x, y, z) \
	NE_LightSetI(n ,c, floattov10(x), floattov10(y), floattov10(z))

/*! \fn    void NE_PolyBegin(int mode);
 *  \brief Begin a polygon.
 *  \param mode Type of polygon to draw (GL_TRIANGLE, GL_QUAD...).
 */
void NE_PolyBegin(int mode);

/*! \fn    void NE_PolyEnd(void);
 *  \brief Just a dummy function. Feel free to never use it.
 */
void NE_PolyEnd(void);

/*! \fn    void NE_PolyColor(u32 color);
 *  \brief Set color for next group of vertex.
 *  \param color Color.
 */
void NE_PolyColor(u32 color);

/*! \fn    void NE_PolyNormalI(int x, int y, int z);
 *  \brief Set normal for next group of vertex.
 *  \param x (x, y, z) Unit vector.
 *  \param y (x, y, z) Unit vector.
 *  \param z (x, y, z) Unit vector.
 */
void NE_PolyNormalI(int x, int y, int z);

/*! \def   NE_PolyNormal(float x, float y, float z);
 *  \brief Set normal for next group of vertex.
 *  \param x (x, y, z) Unit vector.
 *  \param y (x, y, z) Unit vector.
 *  \param z (x, y, z) Unit vector.
 */
#define NE_PolyNormal(x, y, z) \
	NE_PolyNormalI(floattov10(x), floattov10(y), floattov10(z))

/*! \fn    void NE_PolyVertexI(int x, int y, int z);
 *  \brief Put new vertex...
 *  \param x (x, y, z) Vertex coordinates.
 *  \param y (x, y, z) Vertex coordinates.
 *  \param z (x, y, z) Vertex coordinates.
 */
void NE_PolyVertexI(int x, int y, int z);

/*! \def   NE_PolyVertex(float x, float y, float z);
 *  \brief Put new vertex...
 *  \param x (x, y, z) Vertex coordinates.
 *  \param y (x, y, z) Vertex coordinates.
 *  \param z (x, y, z) Vertex coordinates.
 */
#define NE_PolyVertex(x, y, z) \
	NE_PolyVertexI(floattov16(x), floattov16(y), floattov16(z))

/*! \fn    void NE_PolyTexCoord(int u, int v);
 *  \brief Set texture coordinates.
 *  \param u (u, v) Texture coordinates (0 - texturesize).
 *  \param v (u, v) Texture coordinates (0 - texturesize).
 *
 * "When texture mapping, the Geometry Engine works faster if you issue commands
 * in the order TexCoord -> Normal -> Vertex."
 *
 * https://problemkaputt.de/gbatek.htm#ds3dtextureattributes
 */
void NE_PolyTexCoord(int u, int v);

/*! \enum  NE_LightEnum
 *  \brief Used in NE_PolyFormat(). Tells what lights to use.
 */
typedef enum {
	NE_LIGHT_0 = (1 << 0),	/*!< Use light 0. */
	NE_LIGHT_1 = (1 << 1),	/*!< Use light 1. */
	NE_LIGHT_2 = (1 << 2),	/*!< Use light 2. */
	NE_LIGHT_3 = (1 << 3),	/*!< Use light 3. */

	NE_LIGHT_01 = NE_LIGHT_0 | NE_LIGHT_1,	/*!< Use lights 0 and 1. */
	NE_LIGHT_02 = NE_LIGHT_0 | NE_LIGHT_2,	/*!< Use lights 0 and 2. */
	NE_LIGHT_03 = NE_LIGHT_0 | NE_LIGHT_3,	/*!< Use lights 0 and 3. */
	NE_LIGHT_12 = NE_LIGHT_1 | NE_LIGHT_2,	/*!< Use lights 1 and 2. */
	NE_LIGHT_13 = NE_LIGHT_1 | NE_LIGHT_3,	/*!< Use lights 1 and 3. */
	NE_LIGHT_23 = NE_LIGHT_2 | NE_LIGHT_3,	/*!< Use lights 2 and 3. */

	NE_LIGHT_012 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2,	/*!< Use lights 0, 1 and 2. */
	NE_LIGHT_013 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2,	/*!< Use lights 0, 1 and 3. */
	NE_LIGHT_023 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2,	/*!< Use lights 0, 2 and 3. */
	NE_LIGHT_123 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2,	/*!< Use lights 1, 2 and 3. */

	NE_LIGHT_0123 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2 | NE_LIGHT_3,	/*!< Use all lights. */

	NE_LIGHT_ALL = NE_LIGHT_0123	/*!< Use all lights. */
} NE_LightEnum;

/*! \enum  NE_CullingEnum
 *  \brief Used in NE_PolyFormat(). Tells what polygons to draw.
 */
typedef enum {
	NE_CULL_FRONT = (1 << 6),	/*!< Don't draw polygons looking at the camera. */
	NE_CULL_BACK = (2 << 6),	/*!< Don't draw polygons not looking at the camera. */
	NE_CULL_NONE = (3 << 6)	/*!< Draw all polygons polygon. */
} NE_CullingEnum;

/*! \enum  NE_OtherFormatEnum
 *  \brief Used in NE_PolyFormat().
 */
typedef enum {
	NE_MODULATION = (0 << 4),	/*!< Normal shading. */
	NE_TOON_SHADING = (2 << 4),	/*!< Toon shading. */
	NE_USE_FOG = (1 << 15),	/*!< Use fog. */
} NE_OtherFormatEnum;

/*! \fn    void NE_PolyFormat(u32 alpha, u32 id, NE_LightEnum lights,
 *                            NE_CullingEnum culling,
 *                            NE_OtherFormatEnum other);
 *  \brief Enable/disable multiple options.
 *  \param alpha Alpha value. 0 = Wireframe, 31 = Opaque, 1-30 Transparent. You
 *         can only blend one polygon over another if they have different ID.
 *  \param id Polygon ID used for antialias, blending and outlining. (0 - 63)
 *  \param lights Lights used... Use NE_LightEnum for this.
 *  \param culling What polygons must not be drawn. Possible options in
 *         NE_CullingEnum.
 *  \param other Other parameters. Possible flags in NE_OtherFormatEnum.
 */
void NE_PolyFormat(u32 alpha, u32 id, NE_LightEnum lights,
		   NE_CullingEnum culling, NE_OtherFormatEnum other);

/*! \fn    void NE_OutliningEnable(bool value);
 *  \brief Enable/disable outlining.
 *  \param value True/False for enabling/disabling.
 *
 * You have to set some colors for this with NE_OutliningSetColor(). Color 0
 * works with polygon IDs 0 - 7, color 1 with IDs 8 - 15... 8 colors available.
 * Only works with opaque or wireframe polygons.
 */
void NE_OutliningEnable(bool value);

/*! \fn    void NE_OutliningSetColor(u32 num, u32 color);
 *  \brief Set outlining color.
 *  \param num Color number.
 *  \param color Color.
 */
void NE_OutliningSetColor(u32 num, u32 color);

/*! \fn    void NE_ShadingEnable(bool value);
 *  \brief Set shading on/off.
 *  \param value Truefalse for on/off.
 *
 * Change propierties of materials affected by this to, for example:
 * AMBIENT = RGB15(8,8,8), DIFFUSE = RGB15(24,24,24), SPECULAR = 0, EMISSION = 0
 */
void NE_ShadingEnable(bool value);

/*! \fn    void NE_ToonHighlightEnable(bool value);
 *  \brief Set toon highlight on/off.
 *  \param value True/false for on/off.
 *
 * What does this actually do? I can only notice that if it is enabled colors
 * are darker and some polygons are not iuminated...
 */
void NE_ToonHighlightEnable(bool value);

/*! \fn    void NE_ClearColorSet(u32 color, u32 alpha, u32 id);
 *  \brief Set some data to rear plane.
 *  \param color Color.
 *  \param alpha Alpha value.
 *  \param id Rear plane polygon ID.
 */
void NE_ClearColorSet(u32 color, u32 alpha, u32 id);

/*! \def   #define REG_CLRIMAGE_OFFSET (*(vu16*)0x4000356)
 *  \brief Clear BMP scroll register. Taken from gbatek.
 */
#ifndef REG_CLRIMAGE_OFFSET
#define REG_CLRIMAGE_OFFSET (*(vu16*)0x4000356)
#endif

/*! \fn    void NE_ClearBMPEnable(bool value);
 *  \brief Set clear bitmap on/off.
 *  \param value True/false for on/off.
 *
 * It uses VRAM_C as color bitmap and VRAM_D as depth bitmap. You have to copy
 * data there and then use this function. Those 2 VRAM banks can't be used as
 * texture banks with this enabled, so you have to call
 * NE_TextureSystemReset(0, 0, USE_VRAM_AB) before using this.
 *
 * Dual 3D mode needs those two banks for the display capture, so you can't use
 * a clear BMP.
 *
 * VRAM_C: ABBBBBGGGGGRRRRR -- Alpha - Blue - Green - Red
 *
 * VRAM_D: FDDDDDDDDDDDDDDD -- Fog - Depth (0 = near, 0x7FFF = far)
 *
 * Use this only if you really need it. If you want to display just an image,
 * set the rear plane alpha to 0 with NE_ClearColorSet() and use a simple
 * background. This is meant to be used only if you need to set different depths
 * to every pixel or enable the fog for just some pixels.
 */
void NE_ClearBMPEnable(bool value);

/*! \fn    void NE_ClearBMPScroll(u32 x, u32 y);
 *  \brief Sets scroll of clear BMP. (0 - 255)
 *  \param x Scroll on the X axis.
 *  \param y Scroll on the Y axis.
 */
void NE_ClearBMPScroll(u32 x, u32 y);

/*! \fn    void NE_FogEnable(u32 shift, u32 color, u32 alpha, int mass,
 *                           int depth);
 *  \brief Enables fog and sets its parameters.
 *  \param shift Distance between fog bands. (1 - 15)
 *  \param color Fog color.
 *  \param alpha Alpha value.
 *  \param mass Quantity of fog.
 *  \param depth Start point of fog. (0 - 7FFFh) [Use float_to_12d3(n)
 *         int_to_12d3(n) ?]
 *
 * You should try different values until you find the configuration you like.
 */
void NE_FogEnable(u32 shift, u32 color, u32 alpha, int mass, int depth);

/*! \fn    void NE_FogEnableBackground(bool value);
 *  \brief Enable/disable background fog (doesn't affect polygons).
 *  \param value True/false for on/off.
 */
void NE_FogEnableBackground(bool value);

/*! \fn    void NE_FogDisable(void);
 *  \brief Disables fog.
 */
void NE_FogDisable(void);

/*! @} */

#endif // NE_POLYGON_H__
