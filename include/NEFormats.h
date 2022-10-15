// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_FORMATS_H__
#define NE_FORMATS_H__

#include "NETexture.h"

/// @file   NEFormats.h
/// @brief  Used to convert common formats into DS formats at runtime.

/// @defgroup formats Format coversion functions
///
/// Functions to convert BMP files into DS textures. They support BMP of 4, 8,
/// 16 (X1RGB5) and 24 bits per pixel.
///
/// @{

/// @struct NE_BMPHeader
/// Header struct for a BMP file.
typedef struct PACKED {
    u16 type;       /// Magic identifier
    u32 size;       /// File size in bytes
    u16 reserved1;  /// Reserved
    u16 reserved2;  /// Reserved
    u32 offset;     /// Offset to image data, bytes
} NE_BMPHeader;

/// @struct NE_BMPInfoHeader
/// Information struct of a BMP file.
typedef struct PACKED {
    u32 size;               /// Header size in bytes
    u32 width;              /// Width of the image
    u32 height;             /// Height of the image
    u16 planes;             /// Number of color planes
    u16 bits;               /// Bits per pixel
    u32 compression;        /// Compression type
    u32 imagesize;          /// Image size in bytes
    u32 xresolution;        /// Horizontal resolution in pixels per meter
    u32 yresolution;        /// Vertical resolution in pixels per meter
    u32 ncolors;            /// Number of colors
    u32 importantcolors;    /// Important colors
} NE_BMPInfoHeader;

/// Converts a BMP file from a filesystem to an RGBA texture and loads it to
/// a material object.
///
/// @param tex Material to hold the texture.
/// @param filename Path to a BMP file in the filesystem. Its format must be 24
///                 bpp or 16 bpp (X1RGB5).
/// @param transpcolor If true, pixel 0 color will be transparent.
/// @return It returns 1 on success.
int NE_FATMaterialTexLoadBMPtoRGBA(NE_Material *tex, char *filename,
                                   bool transpcolor);

/// Converts a BMP file from a filesystem to an RGB256 texture and loads it to
/// texture and palette objects.
///
/// @param tex Material to hold the data.
/// @param pal Palette to hold the data.
/// @param filename Path to the BMP file. Bits per pixel must be 8 or 4.
/// @param transpcolor If true, palette color 0 will be transparent.
/// @return It returns 1 on success.
int NE_FATMaterialTexLoadBMPtoRGB256(NE_Material *tex, NE_Palette *pal,
                                     char *filename, bool transpcolor);

/// Converts a BMP file from RAM to an RGBA texture and loads it to a material
/// object.
///
/// @param tex Material to hold the texture.
/// @param pointer Pointer to a BMP file in RAM. Its format must be 24 bpp or
///                16 bpp (X1RGB5).
/// @param transpcolor If true, pixel 0 color will be transparent.
/// @return It returns 1 on success.
int NE_MaterialTexLoadBMPtoRGBA(NE_Material *tex, void *pointer,
                                bool transpcolor);

/// Converts a BMP file from RAM to an RGB256 texture and loads it to texture
/// and palette objects.
///
/// @param tex Material to hold the data.
/// @param pal Palette to hold the data.
/// @param pointer Pointer to the BMP file. Bits per pixel must be 8 or 4.
/// @param transpcolor If true, palette color 0 will be transparent.
/// @return It returns 1 on success.
int NE_MaterialTexLoadBMPtoRGB256(NE_Material *tex, NE_Palette *pal,
                                  void *pointer, bool transpcolor);

/// @}

#endif // NE_FORMATS_H__
