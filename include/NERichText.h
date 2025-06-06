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

#define NE_DEFAULT_RICH_TEXT_FONTS 8 ///< Default max number of rich text fonts
#define NE_MAX_RICH_TEXT_FONTS NE_DEFAULT_RICH_TEXT_FONTS ///< Deprecated and unused, left for compatibility

/// Change the priority of rich text drawn after this function call.
///
/// @param priority New priority.
void NE_RichTextPrioritySet(int priority);

/// Set to 0 the priority of rich text drawn after this function call.
void NE_RichTextPriorityReset(void);

/// Initializes the rich text system with the specified number of slots.
///
/// This must be called before initializing any rich text slots.
///
/// @param numSlots The number of rich text slots to allocate.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextStartSystem(u32 numSlots);

/// Clears all rich text font slots.
void NE_RichTextResetSystem(void);

/// Initialize a rich text slot.
///
/// @param slot The slot to initialize (from 0 to the number of slots specified in NE_RichTextSystemStart).
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
/// Also, note that this texture doesn't need to have a size that is a power of
/// two. However, consider that the size of a row should at least be a multiple
/// of a full byte (for example, for a 16 color texture, don't use a texture
/// with width of 143 because the last byte won't be full). To be sure that you
/// never find any issue, ensure that your textures have a width multiple of 4
/// pixels, that will work with all texture formats.
///
/// This is required for NE_RichTextRenderMaterial().
///
/// This isn't required for NE_RichTextRender3D() or NE_RichTextRender3DAlpha().
///
/// @param slot The slot to use.
/// @param path Path to the GRF file.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextBitmapLoadGRF(u32 slot, const char *path);

/// Assign a texture (and palette) to be used to render text to textures.
///
/// This doesn't load the texture to VRAM, it keeps it in RAM. The texture will
/// be used whenever the user uses NE_RichTextRenderMaterial() to render text to
/// a new material.
///
/// The buffers provided won't be freed when the text font is cleared with
/// NE_RichTextEnd(), they have to be freed manually.
///
/// Also, note that this texture doesn't need to have a size that is a power of
/// two. However, consider that the size of a row should at least be a multiple
/// of a full byte (for example, for a 16 color texture, don't use a texture
/// with width of 143 because the last byte won't be full). To be sure that you
/// never find any issue, ensure that your textures have a width multiple of 4
/// pixels, that will work with all texture formats.
///
/// This is required for NE_RichTextRenderMaterial().
///
/// This isn't required for NE_RichTextRender3D() or NE_RichTextRender3DAlpha().
///
/// @param slot The slot to use.
/// @param texture_buffer Pointer to the texture.
/// @param texture_width Width of the texture.h
/// @param texture_height Height of the texture.
/// @param texture_fmt Format of the texture.
/// @param palette_buffer Pointer to the palette.
/// @param palette_size Size of the palette in bytes.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextBitmapSet(u32 slot, const void *texture_buffer,
                         size_t texture_width, size_t texture_height,
                         NE_TextureFormat texture_fmt,
                         const void *palette_buffer, size_t palette_size);

/// Calculates the final size and cursor position of rendering the provided string.
///More actions
/// @param slot The slot to use.
/// @param str The string to render.
/// @param size_x Pointer to store the width of the resulting image.
/// @param size_y Pointer to store the height of the resulting image.
/// @param final_x Pointer to store the final X position of the cursor.
/// @param final_y Pointer to store the final Y position of the cursor.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRenderDryRunWithPos(u32 slot, const char *str,
                            size_t *size_x, size_t *size_y,
                            size_t *final_x, size_t *final_y);

/// Calculates the final size of rendering the provided string.
///
/// @param slot The slot to use.
/// @param str The string to render.
/// @param size_x Pointer to store the width of the resulting image.
/// @param size_y Pointer to store the height of the resulting image.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRenderDryRun(u32 slot, const char *str,
                            size_t *size_x, size_t *size_y);

/// Render a string by rendering one 3D quad per codepoint.
///
/// This preserves the polygon format that is currently active.
///
/// @param slot The slot to use.
/// @param str The string to render.
/// @param x The left coordinate of the text.
/// @param y The top coordinate of the text.
/// @param xIndent The horizontal indentation to apply to the first line of text.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRender3DWithIndent(u32 slot, const char *str, s32 x, s32 y,
                                  s32 xIndent);

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

/// Render a string by rendering one 3D quad per codepoint.More actions
///
/// This preserves the polygon format that is currently active.
///
/// @param slot The slot to use.
/// @param str The string to render.
/// @param x The left coordinate of the text.
/// @param y The top coordinate of the text.
/// @param xIndent The horizontal indentation to apply to the first line of text.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRender3DWithIndent(u32 slot, const char *str, s32 x, s32 y,
                                  s32 xIndent);

/// Render a string by rendering one 3D quad per codepoint.
///
/// This preserves the polygon format that is currently active.
@@ -178,6 +204,29 @@
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
/// @param xIndent The horizontal indentation to apply to the first line of text.
/// @return Returns 1 on success, 0 on failure.
int NE_RichTextRender3DAlphaWithIndent(u32 slot, const char *str, s32 x, s32 y,
                                       uint32_t poly_fmt, int poly_id_base,
                                       s32 xIndent);

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
