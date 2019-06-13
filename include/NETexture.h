// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_TEXTURE_H__
#define NE_TEXTURE_H__

#include <nds.h>
#include "NEPalette.h"

/*! \file   NETexture.h
 *  \brief  Texture and material functions.
 */

/*! @defgroup texture_system Texture system.
 *
 * Texture manipulation functions. A material is just a texture with a color.
 *
 * @{
*/

/*! \def   #define NE_DEFAULT_TEXTURES 512 */
#define NE_DEFAULT_TEXTURES 512

/*! \def   #define NE_NO_PALETTE   -1 */
#define NE_NO_PALETTE   -1

/*! \struct NE_Material
 *  \brief  Information of one material.
 */
typedef struct {
	int texindex;
	u32 color;
	u32 diffuse, ambient, specular, emission;
	bool vtxcolor, useshininess;
} NE_Material;

/*! \fn    NE_Material *NE_MaterialCreate(void);
 *  \brief Returns a pointer to a new NE_Material struct.
 */
NE_Material *NE_MaterialCreate(void);

/*! \fn    void NE_MaterialColorSet(NE_Material *tex, u32 color);
 *  \brief Apply color to material.
 *  \param tex Material.
 *  \param color Color.
 */
void NE_MaterialColorSet(NE_Material *tex, u32 color);

/*! \fn    void NE_MaterialColorDelete(NE_Material *tex);
 *  \brief Remove color of a material.
 *  \param tex Material.
 */
void NE_MaterialColorDelete(NE_Material *tex);

/*! \fn    int NE_MaterialTexLoadFAT(NE_Material *tex,
 *                                   GL_TEXTURE_TYPE_ENUM type, int sizeX,
 *                                   int sizeY, int param, char * path);
 *  \brief Load a texture from FAT and set it to a NE_Material struct. Returns 1
 *         if OK, 0 if error.
 *  \param tex Material.
 *  \param type Texture type.
 *  \param sizeX (sizeX, sizeY) Texture size. It doesn't need to be a power of 2
 *  \param sizeY (sizeX, sizeY) Texture size. It doesn't need to be a power of 2
 *  \param param Parameters of the texture.
 *  \param path Path of the texture file.
 */
int NE_MaterialTexLoadFAT(NE_Material *tex, GL_TEXTURE_TYPE_ENUM type,
			  int sizeX, int sizeY, int param, char *path);

/*! \fn    int NE_MaterialTexLoad(NE_Material *tex, GL_TEXTURE_TYPE_ENUM type,
 *                                int sizeX, int sizeY, int param,
 *                                void *texture);
 *  \brief Load a texture from RAM and set it to a NE_Material struct. Returns 1
 *         if OK, 0 if error.
 *  \param tex Material.
 *  \param type Texture type.
 *  \param sizeX (sizeX, sizeY) Texture size. It doesn't need to be a power of 2
 *  \param sizeY (sizeX, sizeY) Texture size. It doesn't need to be a power of 2
 *  \param param Parameters of the texture.
 *  \param texture Pointer to the texture.
 */
int NE_MaterialTexLoad(NE_Material *tex, GL_TEXTURE_TYPE_ENUM type, int sizeX,
		       int sizeY, int param, void *texture);

/*! \fn    void NE_MaterialTexClone(NE_Material *source, NE_Material *dest);
 *  \brief Copies the texture of a material into another material. You can
 *         delete them as usual.
 *  \param source Source.
 *  \param dest Destination.
 */
void NE_MaterialTexClone(NE_Material *source, NE_Material *dest);

/*! \fn    void NE_MaterialTexSetPal(NE_Material *tex, NE_Palette *pal);
 *  \brief Set palete to a texture.
 *  \param tex Material with the texture.
 *  \param pal Palette. */
void NE_MaterialTexSetPal(NE_Material *tex, NE_Palette *pal);

/*! \fn    void NE_MaterialUse(NE_Material *tex);
 *  \brief Set next models to be drawn next to use the material.
 *  \param tex Material to be used.
 *
 * If NULL, it will disable textures and new polygons won't be affected by them
 * until it is called again.
 */
void NE_MaterialUse(NE_Material *tex);

/*! \enum  NE_VRAMBankFlags
 *  \brief Enum to choose what banks can use Nitro Engine to allocate textures.
 */
typedef enum {
	NE_VRAM_A = (1 << 0),	/*!< Use bank A. */
	NE_VRAM_B = (1 << 1),	/*!< Use bank B. */
	NE_VRAM_C = (1 << 2),	/*!< Use bank C. */
	NE_VRAM_D = (1 << 3),	/*!< Use bank D. */

	NE_VRAM_AB = NE_VRAM_A | NE_VRAM_B,	/*!< Use banks A and B. */
	NE_VRAM_AC = NE_VRAM_A | NE_VRAM_C,	/*!< Use banks A and C. */
	NE_VRAM_AD = NE_VRAM_A | NE_VRAM_D,	/*!< Use banks A and D. */
	NE_VRAM_BC = NE_VRAM_B | NE_VRAM_C,	/*!< Use banks B and C. */
	NE_VRAM_BD = NE_VRAM_B | NE_VRAM_D,	/*!< Use banks B and D. */
	NE_VRAM_CD = NE_VRAM_C | NE_VRAM_D,	/*!< Use banks C and D. */

	NE_VRAM_ABC = NE_VRAM_A | NE_VRAM_B | NE_VRAM_C,	/*!< Use banks A, B and C. */
	NE_VRAM_ABD = NE_VRAM_A | NE_VRAM_B | NE_VRAM_D,	/*!< Use banks A, B and D. */
	NE_VRAM_ACD = NE_VRAM_A | NE_VRAM_C | NE_VRAM_D,	/*!< Use banks A, C and D. */
	NE_VRAM_BCD = NE_VRAM_B | NE_VRAM_C | NE_VRAM_D,	/*!< Use banks B, C and D. */

	NE_VRAM_ABCD = NE_VRAM_A | NE_VRAM_B | NE_VRAM_C | NE_VRAM_D,	/*!< Use the 4 main banks. */
} NE_VRAMBankFlags;

/*! \fn    void NE_TextureSystemReset(int texture_number, int palette_number,
 *                                    NE_VRAMBankFlags bank_flags);
 *  \brief Resets texture and palette system.
 *  \param texture_number Max number of textures. If lower than 1, it will
 *         create space for NE_DEFAULT_TEXTURES.
 *  \param palette_number Max number of palettes. If lower than 1, it will
 *         create space for NE_DEFAULT_PALETTES.
 * \param bank_flags VRAM banks where Nitro Engine can allocate textures.
 *        In Dual 3D mode, only A and B are available. If you are planning to
 *        use a depth BMP, you can only use banks A and B. If you don't tell
 *        Nitro Engine to use at least a valid bank, it will use the default
 *        ones.
 */
void NE_TextureSystemReset(int texture_number, int palette_number,
			   NE_VRAMBankFlags bank_flags);

/*! \fn    void NE_MaterialUse(NE_Material *tex);
 *  \brief Delete material.
 *  \param tex Material.
 */
void NE_MaterialDelete(NE_Material *tex);

/*! \fn    int NE_TextureFreeMem(void);
 *  \brief Returns total free space for textures.
 */
int NE_TextureFreeMem(void);

/*! \fn    int NE_TextureFreeMemPercent(void);
 *  \brief Returns percent of free space for textures.
 */
int NE_TextureFreeMemPercent(void);

/*! \fn    void NE_TextureDefragMem(void);
 *  \brief Defragment VRAM used by textures. It may take some time to finish.
 *
 * WARNING: This function doesn't currently work.
 */
void NE_TextureDefragMem(void);

/*! \fn    void NE_TextureSystemEnd(void);
 *  \brief Terminates texture and palette system and frees memory used by them.
 */
void NE_TextureSystemEnd(void);

/*! \fn    int NE_TextureGetSizeX(NE_Material *tex);
 *  \brief Returns used X size of texture.
 *  \param tex Material.
 */
int NE_TextureGetSizeX(NE_Material *tex);

/*! \fn    int NE_TextureGetSizeY(NE_Material *tex);
 *  \brief Returns used Y size of texture.
 *  \param tex Material.
 */
int NE_TextureGetSizeY(NE_Material *tex);

/*! \fn    int NE_TextureGetRealSizeX(NE_Material *tex);
 *  \brief Returns real X size of a texture.
 *  \param tex Material.
 */
int NE_TextureGetRealSizeX(NE_Material *tex);

/*! \fn    int NE_TextureGetRealSizeY(NE_Material *tex);
 *  \brief Returns real Y size of a texture.
 *  \param tex Material.
 */
int NE_TextureGetRealSizeY(NE_Material *tex);

/*! \fn    void NE_MaterialSetPropierties(NE_Material *tex, u32 diffuse,
 *                                        u32 ambient, u32 specular,
 *                                        u32 emission, bool vtxcolor,
 *                                        bool useshininess);
 *  \brief Sets propierties of this material.
 *  \param tex Material.
 *  \param diffuse Set diffuse color, lights that directly hits the polygon.
 *  \param ambient Set ambient color, lights that indirectly hits the polygon
 *         (walls reflections...).
 *  \param specular Set specular color, lights reflected towards the camera,
 *         like a mirror.
 *  \param emission Set emission color, light emitted by the polygon.
 *  \param vtxcolor If true, diffuse reflection will work as color command.
 *  \param useshininess If true, specular reflection will use the shininess
 *         table.
 */
void NE_MaterialSetPropierties(NE_Material *tex, u32 diffuse, u32 ambient,
			       u32 specular, u32 emission, bool vtxcolor,
			       bool useshininess);

/*! \fn    void NE_MaterialSetDefaultPropierties(u32 diffuse, u32 ambient,
 *                                               u32 specular, u32 emission,
 *                                               bool vtxcolor,
 *                                               bool useshininess);
 *  \brief Sets default propierties of materials when they are loaded.
 *  \param diffuse Set diffuse color, lights that directly hits the polygon.
 *  \param ambient Set ambient color, lights that indirectly hits the polygon
 *         (walls reflections...).
 *  \param specular Set specular color, lights reflected towards the camera,
 *         like a mirror.
 *  \param emission Set emission color, light emitted by the polygon.
 *  \param vtxcolor If true, diffuse reflection will work as color command.
 *  \param useshininess If true, specular reflection will use the shininess
 *         table.
 */
void NE_MaterialSetDefaultPropierties(u32 diffuse, u32 ambient, u32 specular,
				      u32 emission, bool vtxcolor,
				      bool useshininess);

/*! \fn    void *NE_TextureDrawingStart(NE_Material *tex);
 *  \brief Enables drawing for given texture. Returns a pointer to the data.
 *  \param tex Texture to draw.
 *
 * Use this DURING VBL. YOU MUST USE NE_TextureDrawingEnd() WHEN YOU FINISH
 * DRAWING. IF YOU DON'T, GPU WON'T BE ABLE TO RENDER ANY TEXTURE TO SCREEN.
 */
void *NE_TextureDrawingStart(NE_Material *tex);

/*! \fn    void NE_TexturePutPixelRGBA(u32 x, u32 y, u16 color);
 *  \brief Puts a pixel to given RGBA texture.
 *  \param x (x, y) Coordinates.
 *  \param y (x, y) Coordinates.
 *  \param color Color in RGB15. Bit 15 must be set to make the pixel visible.
 *
 * Use this DURING VBL.
 */
void NE_TexturePutPixelRGBA(u32 x, u32 y, u16 color);

/*! \fn    void NE_TexturePutPixelRGB256(u32 x, u32 y, u8 palettecolor);
 *  \brief Sets given pixel to "palettecolor".
 *  \param x (x,y) Pixel coordinates.
 *  \param y (x,y) Pixel coordinates.
 *  \param palettecolor New palette color index.
 *
 * Use this DURING VBL.
 */
void NE_TexturePutPixelRGB256(u32 x, u32 y, u8 palettecolor);

/*! \fn    void NE_TextureDrawingEnd(void);
 *  \brief Ends texture drawing.
 *
 * Use this DURING VBL.
 */
void NE_TextureDrawingEnd(void);

/*! @} */

#endif // NE_TEXTURE_H__
