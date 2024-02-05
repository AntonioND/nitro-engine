// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_POLYGON_H__
#define NE_POLYGON_H__

#include "NEMain.h"

/// @file   NEPolygon.h
/// @brief  Functions to draw polygons and more...

/// @defgroup other_functions Other functions
///
/// Some functions to set lights and its properties, to draw polygons, configure
/// the rear plane, etc...
///
/// @{

/// Predefined colors.
typedef enum {
    NE_Brown     = RGB15(10, 6, 1),     ///<  Brown
    NE_Red       = RGB15(31, 0, 0),     ///<  Red
    NE_Orange    = RGB15(31, 20, 0),    ///<  Orange
    NE_Yellow    = RGB15(31, 31, 0),    ///<  Yellow
    NE_LimeGreen = RGB15(15, 31, 0),    ///<  Lime green
    NE_Green     = RGB15(0, 31, 0),     ///<  Green
    NE_DarkGreen = RGB15(0, 15, 0),     ///<  Dark green
    NE_LightBlue = RGB15(7, 15, 31),    ///<  Light blue
    NE_Blue      = RGB15(0, 0, 31),     ///<  Blue
    NE_DarkBlue  = RGB15(0, 6, 15),     ///<  Dark blue
    NE_Violet    = RGB15(28, 8, 28),    ///<  Violet
    NE_Pink      = RGB15(31, 15, 22),   ///<  Pink
    NE_Purple    = RGB15(20, 4, 14),    ///<  Purple
    NE_Indigo    = RGB15(15, 15, 30),   ///<  Purple
    NE_Magenta   = RGB15(31, 0, 31),    ///<  Magenta
    NE_White     = RGB15(31, 31, 31),   ///<  White
    NE_Gray      = RGB15(20, 20, 20),   ///<  Gray
    NE_DarkGray  = RGB15(10, 10, 10),   ///<  Dark gray
    NE_Black     = RGB15(0, 0, 0)       ///<  Black
} NE_ColorEnum;

/// Supported texture formats
typedef enum {
    NE_A3PAL32    = 1, ///< 32 color palette, 3 bits of alpha
    NE_PAL4       = 2, ///< 4 color palette
    NE_PAL16      = 3, ///< 16 color palette
    NE_PAL256     = 4, ///< 256 color palette
    NE_COMPRESSED = 5, ///< @deprecated 4x4 compressed format (compatibilty name)
    NE_TEX4X4     = 5, ///< 4x4 compressed format
    NE_A5PAL8     = 6, ///< 8 color palette, 5 bits of alpha
    NE_A1RGB5     = 7, ///< Direct color (5 bits per channel), 1 bit of alpha
    NE_RGB5       = 8  ///< @deprecated Like NE_A1RGB5, but sets alpha to 1 when loading
} NE_TextureFormat;

/// Switch off a light.
///
/// @param index Index of the light to switch off (0 - 3).
void NE_LightOff(int index);

/// Switch on a light and define its color.
///
/// @param index Index of the light to switch on (0 - 3).
/// @param color Color of the light.
/// @param x (x, y, z) Vector of the light (v10).
/// @param y (x, y, z) Vector of the light (v10).
/// @param z (x, y, z) Vector of the light (v10).
void NE_LightSetI(int index, u32 color, int x, int y, int z);

/// Switch on a light and define its color.
///
/// @param i Index of the light to switch on (0 - 3).
/// @param c Color of the light.
/// @param x (x, y, z) Vector of the light (float).
/// @param y (x, y, z) Vector of the light (float).
/// @param z (x, y, z) Vector of the light (float).
#define NE_LightSet(i, c, x, y, z) \
    NE_LightSetI(i ,c, floattov10(x), floattov10(y), floattov10(z))

/// Sets the color of a light.
///
/// @param index Index of the light (0 - 3).
/// @param color Color.
void NE_LightSetColor(int index, u32 color);

/// Types of functions used to generate a shininess table.
typedef enum {
    NE_SHININESS_NONE,      ///< Fill table with zeroes
    NE_SHININESS_LINEAR,    ///< Increase values linearly
    NE_SHININESS_QUADRATIC, ///< Increase values proportionaly to x^2
    NE_SHININESS_CUBIC,     ///< Increase values proportionaly to x^3
    NE_SHININESS_QUARTIC    ///< Increase values proportionaly to x^4
} NE_ShininessFunction;

/// Generate and load a shininess table used for specular lighting.
///
/// @param function The name of the function used to generate the table.
void NE_ShininessTableGenerate(NE_ShininessFunction function);

/// Begins a polygon.
///
/// @param mode Type of polygon to draw (GL_TRIANGLE, GL_QUAD...).
static inline void NE_PolyBegin(int mode)
{
    GFX_BEGIN = mode;
}

/// Stops drawing polygons.
static inline void NE_PolyEnd(void)
{
    GFX_END = 0;
}

/// Sets the color for the following vertices.
///
/// @param color Color.
static inline void NE_PolyColor(u32 color)
{
    GFX_COLOR = color;
}

/// Set the normal vector for next group of vertices.
///
/// @param x (x, y, z) Unit vector (v10).
/// @param y (x, y, z) Unit vector (v10).
/// @param z (x, y, z) Unit vector (v10).
static inline void NE_PolyNormalI(int x, int y, int z)
{
    GFX_NORMAL = NORMAL_PACK(x, y, z);
}

/// Set the normal vector for next group of vertices.
///
/// @param x (x, y, z) Unit vector (float).
/// @param y (x, y, z) Unit vector (float).
/// @param z (x, y, z) Unit vector (float).
#define NE_PolyNormal(x, y, z) \
    NE_PolyNormalI(floattov10(x), floattov10(y), floattov10(z))

/// Send vertex to the GPU.
///
/// @param x (x, y, z) Vertex coordinates (v16).
/// @param y (x, y, z) Vertex coordinates (v16).
/// @param z (x, y, z) Vertex coordinates (v16).
static inline void NE_PolyVertexI(int x, int y, int z)
{
    GFX_VERTEX16 = (y << 16) | (x & 0xFFFF);
    GFX_VERTEX16 = (uint32_t)(uint16_t)z;
}

/// Send vertex to the GPU.
///
/// @param x (x, y, z) Vertex coordinates (float).
/// @param y (x, y, z) Vertex coordinates (float).
/// @param z (x, y, z) Vertex coordinates (float).
#define NE_PolyVertex(x, y, z) \
    NE_PolyVertexI(floattov16(x), floattov16(y), floattov16(z))

/// Set texture coordinates.
///
/// "When texture mapping, the Geometry Engine works faster if you issue commands
/// in the order TexCoord -> Normal -> Vertex."
///
/// https://problemkaputt.de/gbatek.htm#ds3dtextureattributes
///
/// @param u (u, v) Texture coordinates (0 - texturesize).
/// @param v (u, v) Texture coordinates (0 - texturesize).
static inline void NE_PolyTexCoord(int u, int v)
{
    GFX_TEX_COORD = TEXTURE_PACK(inttot16(u), inttot16(v));
}

/// Flags for NE_PolyFormat() to enable lights.
typedef enum {
    NE_LIGHT_0 = (1 << 0), ///< Light 0
    NE_LIGHT_1 = (1 << 1), ///< Light 1
    NE_LIGHT_2 = (1 << 2), ///< Light 2
    NE_LIGHT_3 = (1 << 3), ///< Light 3

    NE_LIGHT_01 = NE_LIGHT_0 | NE_LIGHT_1, ///< Lights 0 and 1
    NE_LIGHT_02 = NE_LIGHT_0 | NE_LIGHT_2, ///< Lights 0 and 2
    NE_LIGHT_03 = NE_LIGHT_0 | NE_LIGHT_3, ///< Lights 0 and 3
    NE_LIGHT_12 = NE_LIGHT_1 | NE_LIGHT_2, ///< Lights 1 and 2
    NE_LIGHT_13 = NE_LIGHT_1 | NE_LIGHT_3, ///< Lights 1 and 3
    NE_LIGHT_23 = NE_LIGHT_2 | NE_LIGHT_3, ///< Lights 2 and 3

    NE_LIGHT_012 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2, ///< Lights 0, 1 and 2
    NE_LIGHT_013 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2, ///< Lights 0, 1 and 3
    NE_LIGHT_023 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2, ///< Lights 0, 2 and 3
    NE_LIGHT_123 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2, ///< Lights 1, 2 and 3

    NE_LIGHT_0123 = NE_LIGHT_0 | NE_LIGHT_1 | NE_LIGHT_2 | NE_LIGHT_3, ///< All lights

    NE_LIGHT_ALL = NE_LIGHT_0123 ///< All lights
} NE_LightEnum;

/// Flags for NE_PolyFormat() to specify the type of culling.
typedef enum {
    NE_CULL_FRONT = (1 << 6), ///< Don't draw polygons looking at the camera
    NE_CULL_BACK  = (2 << 6), ///< Don't draw polygons not looking at the camera
    NE_CULL_NONE  = (3 << 6)  ///< Draw all polygons
} NE_CullingEnum;

/// Miscellaneous flags used in NE_PolyFormat().
typedef enum {
    NE_MODULATION             = (0 << 4), ///< Modulation (normal) shading
    NE_DECAL                  = (1 << 4), ///< Decal
    NE_TOON_HIGHLIGHT_SHADING = (2 << 4), ///< Toon or highlight shading
    NE_SHADOW_POLYGONS        = (3 << 4), ///< Shadow polygons

    NE_TRANS_DEPTH_KEEP   = (0 << 11), ///< Keep old depth for translucent pixels
    NE_TRANS_DEPTH_UPDATE = (1 << 11), ///< Set new depth for translucent pixels

    NE_HIDE_FAR_CLIPPED   = (0 << 12), ///< Hide far-plane intersecting polys
    NE_RENDER_FAR_CLIPPED = (1 << 12), ///< Draw far-plane intersecting polys

    NE_HIDE_ONE_DOT_POLYS   = (0 << 13), ///< Hide 1-dot polygons behind DISP_1DOT_DEPTH
    NE_RENDER_ONE_DOT_POLYS = (0 << 13), ///< Draw 1-dot polygons behind DISP_1DOT_DEPTH

    NE_DEPTH_TEST_LESS  = (0 << 14), ///< Depth Test: draw pixels with less depth
    NE_DEPTH_TEST_EQUAL = (0 << 14), ///< Depth Test: draw pixels with equal depth

    NE_FOG_DISABLE = (0 << 15), ///< Enable fog
    NE_FOG_ENABLE  = (1 << 15), ///< Enable fog
} NE_OtherFormatEnum;

/// Enable or disable multiple polygon-related options.
///
/// Remember that translucent polygons can only be blended on top of other
/// translucent polygons if they have different polygon IDs.
///
/// @param alpha Alpha value (0 = wireframe, 31 = opaque, 1-30 translucent).
/// @param id Polygon ID used for antialias, blending and outlining (0 - 63).
/// @param lights Lights enabled. Use the enum NE_LightEnum for this.
/// @param culling Which polygons must be drawn. Use the enum  NE_CullingEnum.
/// @param other Other parameters. All possible flags are in NE_OtherFormatEnum.
void NE_PolyFormat(u32 alpha, u32 id, NE_LightEnum lights,
                   NE_CullingEnum culling, NE_OtherFormatEnum other);

/// Enable or disable polygon outline.
///
/// For outlining to work, set up the colors with NE_OutliningSetColor().
///
/// Color 0 works with polygon IDs 0 to 7, color 1 works with IDs 8 to 15, up to
/// color 7.
///
/// It only works with opaque or wireframe polygons.
///
/// @param value True enables it, false disables it.
static inline void NE_OutliningEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_OUTLINE;
    else
        GFX_CONTROL &= ~GL_OUTLINE;
}

/// Set outlining color for the specified index.
///
/// @param index Color index.
/// @param color Color.
void NE_OutliningSetColor(u32 index, u32 color);

/// Setup shading tables for toon shading.
///
/// For the shading to look nice, change the properties of materials affecte
/// by this to, for example:
///
/// - AMBIENT = RGB15(8, 8, 8)
/// - DIFFUSE = RGB15(24, 24, 24)
/// - SPECULAR = RGB15(0, 0, 0)
/// - EMISSION = RGB15(0, 0, 0)
///
/// @param value True sets up tables for toon shading, false clears them.
void NE_ShadingEnable(bool value);

/// Set highlight shading or toon shading modes.
///
/// By default, toon shading is selected.
///
/// @param value True enables highlight shading, false enables toon shading.
static inline void NE_ToonHighlightEnable(bool value)
{
    if (value)
        GFX_CONTROL |= GL_TOON_HIGHLIGHT;
    else
        GFX_CONTROL &= ~GL_TOON_HIGHLIGHT;
}

/// Set color and related values of the rear plane.
///
/// @param color Color.
/// @param alpha Alpha value.
/// @param id Rear plane polygon ID.
void NE_ClearColorSet(u32 color, u32 alpha, u32 id);

/// Clear BMP scroll register.
#ifndef REG_CLRIMAGE_OFFSET
#define REG_CLRIMAGE_OFFSET (*(vu16*)0x4000356)
#endif

/// Enable or disable the clear bitmap.
///
/// The clear bitmap uses VRAM_C as color bitmap and VRAM_D as depth bitmap. You
/// have to copy data there and then use this function to enable it. Those 2
/// VRAM banks can't be used as texture banks with clear bitmap enabled, so you
/// have to call NE_TextureSystemReset(0, 0, USE_VRAM_AB) before enabling it.
///
/// The dual 3D mode needs those two banks for the display capture, so you can't
/// use a clear BMP (even if you could, you would have no space for textures).
///
/// VRAM_C: ABBBBBGGGGGRRRRR (Alpha, Blue, Green, Red)
///
/// VRAM_D: FDDDDDDDDDDDDDDD (Fog enable, Depth) [0 = near, 0x7FFF = far]
///
/// The only real use for this seems to be having a background image with
/// different depths per pixel. If you just want to display a background image
/// it's better to use a textured polygon (or the 2D hardware).
///
/// @param value True to enable it, false to disable it.
void NE_ClearBMPEnable(bool value);

/// Sets scroll of the clear BMP.
///
/// @param x Scroll on the X axis (0 - 255).
/// @param y Scroll on the Y axis (0 - 255).
static inline void NE_ClearBMPScroll(u32 x, u32 y)
{
    REG_CLRIMAGE_OFFSET = (x & 0xFF) | ((y & 0xFF) << 8);
}

/// Enables fog and sets its parameters.
///
/// The values must be determined by trial and error.
///
/// The depth is the distance to the start of the fog from the camera. Use the
/// helpers float_to_12d3() or int_to_12d3().
///
/// @param shift Distance between fog bands (1 - 15).
/// @param color Fog color.
/// @param alpha Alpha value.
/// @param mass Mass of fog.
/// @param depth Start point of fog (0 - 7FFFh)
void NE_FogEnable(u32 shift, u32 color, u32 alpha, int mass, int depth);

/// Enable or disable the background fog.
///
/// This only affects the clear plane, not polygons.
///
/// @param value True enables it, false disables it.
void NE_FogEnableBackground(bool value);

/// Disable fog.
static inline void NE_FogDisable(void)
{
    GFX_CONTROL &= ~(GL_FOG | (15 << 8));
}

/// TODO: Function to set value of DISP_1DOT_DEPTH

/// @}

#endif // NE_POLYGON_H__
