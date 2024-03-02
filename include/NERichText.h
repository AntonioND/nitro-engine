// SPDX-License-Identifier: MIT
//
// Copyright (c) 2024 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_RICHTEXT_H__
#define NE_RICHTEXT_H__

/// @file  NERichText.h
/// @brief Rich text system.

/// @defgroup text_system Text system
///
/// Rich text drawing functions. It is possible to draw text as a series of
/// quad, or to render text to a material (which needs to be drawn by the
/// user as a single quad).
///
/// The functions accept '\n', but not things like '%d', '%s', etc. You'll need
/// to use snprintf() or similar for that.
///
/// You need to call NE_2DViewInit() before using any of the quad drawing
/// functions.
///
/// In order to generate fonts, please check the website of BMFont:
///
/// https://www.angelcode.com/products/bmfont/
///
/// You need to export fonts in binary format, and the images in PNG format.
///
/// Note: If the replacement character � (`U+FFFD`) codepoint is included in the
/// font, it will be used to print missing characters in the font, or when the
/// UTF-8 string has encoding errors.
///
/// @{

#define NE_MAX_RICH_TEXT_FONTS 8 ///< Default max number of rich text fonts

/// Change the priority of rich text drawn after this function call.
///
/// @param priority New priority.
void NE_RichTextPrioritySet(int priority);

/// Set to 0 the priority of rich text drawn after this function call.
void NE_RichTextPriorityReset(void);

/// Initialize a rich text slot.
///
/// @param slot The slot to initialize (from 0 to NE_MAX_RICH_TEXT_FONTS).
void NE_RichTextInit(u32 slot);

/// End a rich text slot and free all the resources used by it.
///
/// @param slot The slot to end.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextEnd(u32 slot);

/// Load font metadata from the specified file in the filesystem.
///
/// @param slot The slot to use.
/// @param path Path to a '.fnt' binary file.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextMetadataLoadFAT(u32 slot, const char *path);

/// Load font metadata from the buffer in RAM.
///
/// @param slot The slot to use.
/// @param data Pointer to a '.fnt' binary file in RAM
/// @param data_size Size of the file.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextMetadataLoadMemory(u32 slot, const void *data, size_t data_size);

/// Load a GRF file to be used as material when drawing text as 3D quads.
///
/// This loads a GRF file with a texture and loads it to VRAM as a material.
///
/// This is required for NE_RichTextRender3D() and NE_RichTextRender3DAlpha().
///
/// This isn't required for NE_RichTextRenderMaterial().
///
/// @param slot The slot to use.
/// @param path Path to the GRF file.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextMaterialLoadGRF(u32 slot, const char *path);

/// Assign a material to the slot, which will be used to draw text as 3D quads.
///
/// After this call, the font will own the material and texture, and it will
/// delete them when the font is deleted. The caller of this function must not
/// try to delete the material or palette manually after calling this function.
///
/// This is required for NE_RichTextRender3D() and NE_RichTextRender3DAlpha().
///
/// This isn't required for NE_RichTextRenderMaterial().
///
/// @param slot The slot to use.
/// @param mat The material to assign.
/// @param pal The palette to assign.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextMaterialSet(u32 slot, NE_Material *mat, NE_Palette *pal);

/// Load a GRF file to RAM to be used to render text to textures.
///
/// This doesn't load the texture to VRAM, it keeps it in RAM. The texture will
/// be used whenever the user uses NE_RichTextRenderMaterial() to render text to
/// a new material.
///
/// This is required for NE_RichTextRenderMaterial().
///
/// This isn't required for NE_RichTextRender3D() or NE_RichTextRender3DAlpha().
///
/// @param slot The slot to use.
/// @param path Path to the GRF file.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextBitmapLoadGRF(u32 slot, const char *path);

/// Render a string by rendering one 3D quad per codepoint.
///
/// This preserves the polygon format that is currently active.
///
/// @param slot The slot to use.
/// @param str The string to render.
/// @param x The left coordinate of the text.
/// @param y The top coordinate of the text.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRender3D(u32 slot, const char *str, s32 x, s32 y);

/// Render a string by rendering one 3D quad per codepoint with alternating
/// polygon IDs.
///
/// This function will alternate between polygon IDs so that alpha blending
/// works between multiple polygons when they overlap. This is a requirement of
/// the NDS 3D hardware.
///
/// It is required to pass the base polygon format as a parameter because the
/// polygon format data is write-only. Whenever the polygon ID needs to be
/// changed, the rest of the polygon format flags need to be set as well.
///
/// @param slot The slot to use.
/// @param str The string to render.
/// @param x The left coordinate of the text.
/// @param y The top coordinate of the text.
/// @param poly_fmt The polygon format values to be used for the quads.
/// @param poly_id_base The base polygon ID to use for the quads.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRender3DAlpha(u32 slot, const char *str, s32 x, s32 y,
                             uint32_t poly_fmt, int poly_id_base);

/// Render a string with the specified font and create a material from it.
///
/// This function renders the string to a buffer and creates a material that
/// uses that buffer as texture. This is quite CPU-intensive, but it makes sense
/// to do it if the text is going to be displayed unchanged for a long time.
///
/// If the font uses an image that doesn't use palette, it will return a
/// material and it won't try to return a palette. If the font uses an image
/// with a palette, there are two options. If you want the pointer to the
/// palette, the function will give it to you. If you don't want the pointer,
/// you can pass NULL to that argument, and the palette will be autodeleted when
/// the material is deleted.
///
/// @param slot The font slot to use.
/// @param str The string to print.
/// @param mat A pointer to a NE_Material to store the new material.
/// @param pal A pointer to a NE_Palette to store the new palette, or NULL.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRenderMaterial(u32 slot, const char *str, NE_Material **mat,
                              NE_Palette **pal);

/// @}

#endif // NE_RICHTEXT_H__
