// SPDX-License-Identifier: Zlib OR MIT
//
// Copyright (c) 2024 Antonio Niño Díaz

#ifndef DSF_H__
#define DSF_H__

/// @file dsf.h
///
/// @brief Global include of LibDSF.

#include <stddef.h>
#include <stdint.h>

/// @defgroup libdsf_types LibDSF definitions and types.
/// @{

/// Major version of LibDSF (semver)
#define LIBDSF_VERSION_MAJOR 0
/// Minor version of LibDSF (semver)
#define LIBDSF_VERSION_MINOR 1
/// Patch version of LibDSF (semver)
#define LIBDSF_VERSION_PATCH 1

/// Value that combines all version numbers, useful for version checks.
#define LIBDSF_VERSION \
                ((LIBDSF_VERSION_MAJOR << 16) | \
                 (LIBDSF_VERSION_MINOR << 8) | \
                 (LIBDSF_VERSION_PATCH << 1))

/// Version string
#define LIBDSF_VERSION_STRING "0.1.1"

/// Character that is used when there are UTF-8 decoding errors.
///
/// This is returned when a character with an invalid encoding is found.
#define REPLACEMENT_CHARACTER 0xFFFD

/// List of possible errors of the library.
typedef enum {
    DSF_NO_ERROR              =  0,  ///< No error
    DSF_INVALID_ARGUMENT      = -1,  ///< Invalid argument (e.g. NULL pointer)
    DSF_BAD_MAGIC             = -2,  ///< Bad BMFont magic number
    DSF_BAD_VERSION           = -3,  ///< Bad BMFont version
    DSF_BAD_CHUNK_SIZE        = -4,  ///< The size of a BMFont chunk is wrong
    DSF_NO_MEMORY             = -5,  ///< Not enough RAM to allocate font data
    DSF_UNEXPECTED_END        = -6,  ///< File ends unexpectedly
    DSF_NO_CHARACTERS         = -7,  ///< No characters present in font
    DSF_FILE_OPEN_ERROR       = -8,  ///< Can't open the provided file
    DSF_FILE_EMPTY            = -9,  ///< The provided file is empty
    DSF_FILE_SEEK_ERROR       = -10, ///< Can't fseek() file
    DSF_FILE_READ_ERROR       = -11, ///< Can't read file
    DSF_CODEPOINT_NOT_FOUND   = -12, ///< Codepoint not present in font
    DSF_TEXTURE_TOO_BIG       = -12, ///< The texture size required is too big for the NDS
    DSF_TEXTURE_BAD_FORMAT    = -13, ///< Unsupported NDS texture format
} dsf_error;

/// Type that represents a DSF font internal state.
typedef uintptr_t dsf_handle;

/// @}
/// @defgroup libdsf_load_unload Font loading and unloading functions.
/// @{

/// This function loads a BMFont in binary format from RAM.
///
/// @param handle    Pointer to a handle that will be configured for this font.
/// @param data      Pointer to the font data (".fnt" file).
/// @param data_size Size of the font data.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_LoadFontMemory(dsf_handle *handle,
                             const void *data, int32_t data_size);

/// This function loads a BMFont in binary format from the filesystem.
///
/// @param handle Pointer to a handle that will be configured for this font.
/// @param path   Path to the ".fnt" file in the filesystem.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_LoadFontFilesystem(dsf_handle *handle, const char *path);

/// Free all memory used by this font.
///
/// It also invalidates the provided handler.
///
/// @param handle Pointer to the font handler.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_FreeFont(dsf_handle *handle);

/// @}
/// @defgroup libdsf_render Functions to draw text strings.
/// @{

/// Pretend to render a string to calculate its final size once rendered.
///
/// @param handle Handler of the font to use.
/// @param str    String to print.
/// @param size_x Pointer to a variable to store the size.
/// @param size_y Pointer to a variable to store the size.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_StringRenderDryRun(dsf_handle handle, const char *str,
                                 size_t *size_x, size_t *size_y);

/// Render a string by rendering one 3D quad per codepoint.
///
/// @param handle Handler of the font to use.
/// @param str    String to print.
/// @param x      Top x coordinate (0 to 255, but you can go outside of that).
/// @param y      Left y coordinate (0 to 191, but you can go outside of that).
/// @param z      Z coordinate (depth).
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_StringRender3D(dsf_handle handle, const char *str,
                             int32_t x, int32_t y, int32_t z);

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
/// @param handle       Handler of the font to use.
/// @param str          String to print.
/// @param x            Top x coordinate (0 to 255, but you can go outside of that).
/// @param y            Left y coordinate (0 to 191, but you can go outside of that).
/// @param z            Z coordinate (depth).
/// @param poly_fmt     Polygon formats to apply to the characters.
/// @param poly_id_base poly_id_base and poly_id_base + 1 will be used.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_StringRender3DAlpha(dsf_handle handle, const char *str,
                                  int32_t x, int32_t y, int32_t z,
                                  uint32_t poly_fmt, int poly_id_base);

/// Allocates a buffer and renders the provided string to that buffer.
///
/// This function takes a texture stored in RAM with the desired font. It will
/// calculate the final size of the text it has to print, and it will allocate
/// enough space for it. Note that NDS texture sizes must be powers of two, so
/// the end result may be bigger than the actual text. After allocating the
/// buffer, it will render the string to that buffer.
///
/// This function won't load the texture to VRAM. The returned buffer needs to
/// be loaded to texture VRAM to be used (with functions like glTexImage2D() or
/// NE_MaterialTexLoad()).
///
/// Also, note that this texture doesn't need to have a size that is a power of
/// two. However, consider that the size of a row should at least be a multiple
/// of a full byte (for example, for a 16 color texture, don't use a texture
/// with width of 143 because the last byte won't be full). To be sure that you
/// never find any issue, ensure that your textures have a width multiple of 4
/// pixels, that will work with all texture formats.
///
/// @param handle       Handler of the font to use.
/// @param str          String to print.
/// @param texture_fmt  Texture format (GL_TEXTURE_TYPE_ENUM, NE_TextureFormat).
/// @param font_texture Pointer to the font texture data that contains the.
/// @param font_width   Width of the font texture.
/// @param font_height  Height of the font texture.
/// @param out_texture  The pointer to the new buffer is returned here.
/// @param out_width    The width to the new buffer is returned here.
/// @param out_height   The height to the new buffer is returned here.
///
/// @return An error code or DSF_NO_ERROR on success.
dsf_error DSF_StringRenderToTexture(dsf_handle handle,
                    const char *str, unsigned int texture_fmt,
                    const void *font_texture, size_t font_width, size_t font_height,
                    void **out_texture, size_t *out_width, size_t *out_height);

/// @}

#endif // DSF_H__
