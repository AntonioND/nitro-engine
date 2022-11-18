// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_TEXTURE_H__
#define NE_TEXTURE_H__

#include <nds.h>

#include "NEPalette.h"
#include "NEPolygon.h"

/// @file   NETexture.h
/// @brief  Texture and material functions.

/// @defgroup material_system Material system
///
/// Material manipulation system. A material is composed of a texture and,
/// optionally, a palette. It also has diffuse, ambient, specular and emission
/// properties.
///
/// @{

#define NE_DEFAULT_TEXTURES 128 ///< Default max number of materials

#define NE_NO_PALETTE       -1 ///< Value that represents not having a palette

/// Holds information of one material.
typedef struct {
    int texindex;           ///< Index to internal texture object
    NE_Palette *palette;    ///< Palette used by this material
    u32 color;              ///< Color of this material when lights aren't used
    u32 diffuse_ambient;    ///< Diffuse and ambient lighting material color
    u32 specular_emission;  ///< Specular and emission lighting material color
} NE_Material;

/// Supported texture options
typedef enum {
    NE_TEXTURE_WRAP_S = (1U << 16), ///< Wrap/repeat texture on S axis
    NE_TEXTURE_WRAP_T = (1U << 17), ///< Wrap/repeat texture on T axis
    NE_TEXTURE_FLIP_S = (1U << 18), ///< Flip texture on S axis when wrapping
    NE_TEXTURE_FLIP_T = (1U << 19), ///< Flip texture on T axis when wrapping
    NE_TEXTURE_COLOR0_TRANSPARENT = (1U << 29), ///< Make palette index 0 transparent
    NE_TEXGEN_OFF      = (0U << 30), ///< Don't modify texture coordinates
    NE_TEXGEN_TEXCOORD = (1U << 30), ///< Multiply coordinates by texture matrix
    NE_TEXGEN_NORMAL   = (2U << 30), ///< Texcoords = Normal * texture matrix (spherical reflection)
    NE_TEXGEN_POSITION = (3U << 30)  ///< Texcoords = Vertex * texture matrix
} NE_TextureFlags;

/// Creates a new material object.
///
/// @return Pointer to the newly created material.
NE_Material *NE_MaterialCreate(void);

/// Applies a color to a material.
///
/// Note that the color will only be displayed if no normal commands are used.
/// Any model with normals will ignore this color.
///
/// @param tex Material.
/// @param color Color.
void NE_MaterialColorSet(NE_Material *tex, u32 color);

/// Removes the color of a material (sets it back to white).
///
/// @param tex Material.
void NE_MaterialColorDelete(NE_Material *tex);

/// Loads a texture from the filesystem and assigns it to a material object.
///
/// The height doesn't need to be a power of two, but he width must be a power
/// of two.
///
/// Textures with width that isn't a power of two need to be resized manually,
/// which is very slow, and they don't save any VRAM when loaded compared to a
/// texture with the full width. The only advantage is that they need less
/// storage space, but you can achieve the same effect by compressing them.
///
/// Textures with a height that isn't a power of two don't need to be resized,
/// and they actually save VRAM space (you tell the GPU that the texture is
/// bigger, but then you ignore the additional space, as it will be used by
/// other textures).
///
/// @param tex Material.
/// @param fmt Texture format.
/// @param sizeX (sizeX, sizeY) Texture size.
/// @param sizeY (sizeX, sizeY) Texture size.
/// @param flags Parameters of the texture.
/// @param path Path of the texture file.
/// @return It returns 1 on success, 0 on error.
int NE_MaterialTexLoadFAT(NE_Material *tex, NE_TextureFormat fmt,
                          int sizeX, int sizeY, NE_TextureFlags flags,
                          char *path);

/// Loads a texture from RAM and assigns it to a material object.
///
/// Textures with width that isn't a power of two need to be resized manually,
/// which is very slow, and they don't save any VRAM when loaded compared to a
/// texture with the full width. The only advantage is that they need less
/// storage space, but you can achieve the same effect by compressing them.
///
/// Textures with a height that isn't a power of two don't need to be resized,
/// and they actually save VRAM space (you tell the GPU that the texture is
/// bigger, but then you ignore the additional space, as it will be used by
/// other textures).
///
/// @param tex Material.
/// @param fmt Texture format.
/// @param sizeX (sizeX, sizeY) Texture size.
/// @param sizeY (sizeX, sizeY) Texture size.
/// @param flags Parameters of the texture.
/// @param texture Pointer to the texture data.
/// @return It returns 1 on success, 0 on error.
int NE_MaterialTexLoad(NE_Material *tex, NE_TextureFormat fmt,
                       int sizeX, int sizeY, NE_TextureFlags flags,
                       void *texture);

/// Copies the texture of a material into another material.
///
/// Unlike with models, you can delete the source and destination materials as
/// desired. Nitro Engine will keep track of how many materials use any specific
/// texture and palette and it will remove them when no more materials are using
/// them.
///
/// @param source Source.
/// @param dest Destination.
void NE_MaterialClone(NE_Material *source, NE_Material *dest);

/// Alias of NE_MaterialClone
///
/// @deprecated This definition is only present for backwards compatibility and
/// it will be removed.
#define NE_MaterialTexClone NE_MaterialClone

/// Assigns a palette to a material.
///
/// @param tex Material.
/// @param pal Palette.
void NE_MaterialSetPalette(NE_Material *tex, NE_Palette *pal);

/// Alias of NE_MaterialSetPalette().
///
/// @deprecated This definition is only present for backwards compatibility and
/// it will be removed.
#define NE_MaterialTexSetPal NE_MaterialSetPalette

/// Set active material to use when drawing polygons.
///
/// If the pointer passed is NULL the function will disable textures and new
/// polygons won't be affected by them until this function is called again with
/// a valid material.
///
/// @param tex Material to be used.
void NE_MaterialUse(const NE_Material *tex);

/// Flags to choose which VRAM banks Nitro Engine can use to allocate textures.
typedef enum {
    NE_VRAM_A = (1 << 0), ///< Bank A
    NE_VRAM_B = (1 << 1), ///< Bank B
    NE_VRAM_C = (1 << 2), ///< Bank C
    NE_VRAM_D = (1 << 3), ///< Bank D

    NE_VRAM_AB = NE_VRAM_A | NE_VRAM_B, ///< Banks A and B
    NE_VRAM_AC = NE_VRAM_A | NE_VRAM_C, ///< Banks A and C
    NE_VRAM_AD = NE_VRAM_A | NE_VRAM_D, ///< Banks A and D
    NE_VRAM_BC = NE_VRAM_B | NE_VRAM_C, ///< Banks B and C
    NE_VRAM_BD = NE_VRAM_B | NE_VRAM_D, ///< Banks B and D
    NE_VRAM_CD = NE_VRAM_C | NE_VRAM_D, ///< Banks C and D

    NE_VRAM_ABC = NE_VRAM_A | NE_VRAM_B | NE_VRAM_C, ///< Banks A, B and C
    NE_VRAM_ABD = NE_VRAM_A | NE_VRAM_B | NE_VRAM_D, ///< Banks A, B and D
    NE_VRAM_ACD = NE_VRAM_A | NE_VRAM_C | NE_VRAM_D, ///< Banks A, C and D
    NE_VRAM_BCD = NE_VRAM_B | NE_VRAM_C | NE_VRAM_D, ///< Banks B, C and D

    NE_VRAM_ABCD = NE_VRAM_A | NE_VRAM_B | NE_VRAM_C | NE_VRAM_D, ///< All main banks
} NE_VRAMBankFlags;

/// Resets the material system and sets the new max number of objects.
///
/// In Dual 3D mode, only VRAM A and B are available for textures.
///
/// If no VRAM banks are specified in this function, all VRAM banks A to D will
/// be used for textures (or just A and B in dual 3D mode).
///
/// @param max_textures Max number of textures. If lower than 1, it will
///                     create space for NE_DEFAULT_TEXTURES.
/// @param max_palettes Max number of palettes. If lower than 1, it will
///                     create space for NE_DEFAULT_PALETTES.
/// @param bank_flags VRAM banks where Nitro Engine can allocate textures.
void NE_TextureSystemReset(int max_textures, int max_palettes,
                           NE_VRAMBankFlags bank_flags);

/// Deletes a material object.
///
/// @param tex Pointer to the material object.
void NE_MaterialDelete(NE_Material *tex);

/// Returns the available free memory for textures.
///
/// Note that, even if it is all available, it may not be contiguous, so you may
/// not be able to load a texture because there isn't enough space in any free
/// gap.
///
/// @return Returns the available memory in bytes.
int NE_TextureFreeMem(void);

/// Returns the percentage of available free memory for textures.
///
/// @return Returns the percentage of available memory (0-100).
int NE_TextureFreeMemPercent(void);

/// Defragment memory used for textures.
///
/// WARNING: This function is currently not working.
void NE_TextureDefragMem(void);

/// End texture system and free all memory used by it.
void NE_TextureSystemEnd(void);

/// Returns the width of a texture.
///
/// This is the size given when the texture was loaded.
///
/// @param tex Material.
/// @return Returns the size in pixels.
int NE_TextureGetSizeX(const NE_Material *tex);

/// Returns the height of a texture.
///
/// This is the size given when the texture was loaded.
///
/// @param tex Material.
/// @return Returns the size in pixels.
int NE_TextureGetSizeY(const NE_Material *tex);

/// Returns the real width of a texture.
///
/// This is the internal size given to the GPU when the texture is used, not the
/// size used to load the texture, which may have been smaller.
///
/// @param tex Material.
/// @return Returns the size in pixels.
int NE_TextureGetRealSizeX(const NE_Material *tex);

/// Returns the real height size of a texture.
///
/// This is the internal size given to the GPU when the texture is used, not the
/// size used to load the texture, which may have been smaller.
///
/// @param tex Material.
/// @return Returns the size in pixels.
int NE_TextureGetRealSizeY(const NE_Material *tex);

/// Sets lighting propierties of this material.
///
/// @param tex Material to modify.
/// @param diffuse Set diffuse color: lights that directly hits the polygon.
/// @param ambient Set ambient color: lights that indirectly hit the polygon
///                (reflections from the walls, etc).
/// @param specular Set specular color: lights reflected towards the camera,
///                 like a mirror.
/// @param emission Set emission color: light emitted by the polygon.
/// @param vtxcolor If true, diffuse reflection will work as a color command.
/// @param useshininess If true, specular reflection will use the shininess
///                     table.
void NE_MaterialSetPropierties(NE_Material *tex, u32 diffuse, u32 ambient,
                               u32 specular, u32 emission, bool vtxcolor,
                               bool useshininess);

/// Sets default lighting propierties of materials when they are created.
///
/// @param diffuse Set diffuse color: lights that directly hits the polygon.
/// @param ambient Set ambient color: lights that indirectly hit the polygon
///                (reflections from the walls, etc).
/// @param specular Set specular color: lights reflected towards the camera,
///                 like a mirror.
/// @param emission Set emission color: light emitted by the polygon.
/// @param vtxcolor If true, diffuse reflection will work as a color command.
/// @param useshininess If true, specular reflection will use the shininess
///                     table.
void NE_MaterialSetDefaultPropierties(u32 diffuse, u32 ambient, u32 specular,
                                      u32 emission, bool vtxcolor,
                                      bool useshininess);

/// Enables modification of the specified texture.
///
/// Use this during VBL. Remember to use NE_TextureDrawingEnd() when you finish.
/// If you don't, the GPU won't be able to render textures to the screen.
///
/// @param tex Texture to modify.
/// @return Returns a pointer to the base address of the texture in VRAM.
void *NE_TextureDrawingStart(const NE_Material *tex);

/// Sets the specified pixel to the specified color.
///
/// This only works for textures in RGBA/RGB format.
///
/// Use this during VBL.
///
/// @param x (x, y) Pixel coordinates.
/// @param y (x, y) Pixel coordinates.
/// @param color Color in RGB15. Bit 15 must be set to make the pixel visible.
void NE_TexturePutPixelRGBA(u32 x, u32 y, u16 color);

/// Sets the specified pixel to the specified palette color index.
///
/// This only works for textures in RGB256 format.
///
/// Use this during VBL.
///
/// @param x (x,y) Pixel coordinates.
/// @param y (x,y) Pixel coordinates.
/// @param palettecolor New palette color index.
void NE_TexturePutPixelRGB256(u32 x, u32 y, u8 palettecolor);

/// Disables modification of textures.
///
/// Use this during VBL.
void NE_TextureDrawingEnd(void);

/// @}

#endif // NE_TEXTURE_H__
