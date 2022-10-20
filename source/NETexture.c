// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"
#include "NEAlloc.h"

/// @file NETexture.c

typedef struct {
    u32 param;
    char *adress;
    NE_Palette *palette;
    int uses;
    int sizex, sizey;
} ne_textureinfo_t;

static ne_textureinfo_t *NE_Texture = NULL;
static NE_Material **NE_UserMaterials = NULL;

static NEChunk *NE_TexAllocList; // See NEAlloc.h

static bool ne_texture_system_inited = false;

static int NE_MAX_TEXTURES;

// Default material propierties
static u32 ne_defaultdiffuse, ne_defaultambient;
static u32 ne_defaultspecular, ne_defaultemission;
static bool ne_defaultvtxcolor, ne_defaultuseshininess;

static int __NE_GetValidSize(int size)
{
    for (int i = 0; i < 8; i++)
    {
        if (size <= (8 << i))
            return (8 << i);
    }
    return 0;
}

static int __NE_ConvertSizeRaw(int size)
{
    for (int i = 0; i < 8; i++)
    {
        if (size == 8)
            return i;
        size >>= 1;
    }
    return 0;
}

static inline void NE_MaterialTexParam(NE_Material *tex, int sizeX, int sizeY,
                                       uint32 *addr, GL_TEXTURE_TYPE_ENUM mode,
                                       u32 param)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No assigned texture");
    NE_Texture[tex->texindex].param = param
            | (__NE_ConvertSizeRaw(sizeX) << 20)
            | (__NE_ConvertSizeRaw(sizeY) << 23)
            | (((uint32) addr >> 3) & 0xFFFF) | (mode << 26);
}

//--------------------------------------------------------------------------

NE_Material *NE_MaterialCreate(void)
{
    if (!ne_texture_system_inited)
        return NULL;

    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_UserMaterials[i] != NULL)
            continue;

        NE_Material *mat = calloc(1, sizeof(NE_Material));
        NE_AssertPointer(mat, "Not enough memory");
        NE_UserMaterials[i] = mat;
        mat->texindex = NE_NO_TEXTURE;
        mat->color = NE_White;
        mat->diffuse = ne_defaultdiffuse;
        mat->ambient = ne_defaultambient;
        mat->specular = ne_defaultspecular;
        mat->emission = ne_defaultemission;
        mat->vtxcolor = ne_defaultvtxcolor;
        mat->useshininess = ne_defaultuseshininess;

        return mat;
    }

    NE_DebugPrint("No free slots");

    return NULL;
}

void NE_MaterialColorSet(NE_Material *tex, u32 color)
{
    NE_AssertPointer(tex, "NULL pointer");
    tex->color = color;
}

void NE_MaterialColorDelete(NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    tex->color = NE_White;
}

int NE_MaterialTexLoadFAT(NE_Material *tex, NE_TextureFormat fmt,
                          int sizeX, int sizeY, NE_TextureFlags flags,
                          char *path)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(sizeX > 0 && sizeY > 0, "Size must be positive");

    char *ptr = NE_FATLoadData(path);
    NE_AssertPointer(ptr, "Couldn't load file from FAT");

    int ret = NE_MaterialTexLoad(tex, fmt, sizeX, sizeY, flags, (u8 *)ptr);
    free(ptr);
    return ret;
}

static int __NE_TextureResizeWidth(void *source, void *dest,
                                   NE_TextureFormat fmt,
                                   int height, int width, int newwidth)
{
    NE_AssertPointer(source, "NULL source pointer");
    NE_AssertPointer(dest, "NULL dest pointer");

    static const int __NE_TextureDepth[] = {
        0,  // Nothing
        8,  // NE_A3PAL32
        2,  // NE_PAL4
        4,  // NE_PAL16
        8,  // NE_PAL256
        0,  // NE_COMPRESSED
        8,  // NE_A5PAL8
        16, // NE_A1RGB5
        16  // NE_RGB5
    };

    int bits = __NE_TextureDepth[fmt];

    if (bits == 16)
    {
        // NE_A1RGB5 or NE_RGB5
        // --------------------

        // Cast to correct width
        u16 *d = dest;
        u16 *s = source;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < newwidth; x++)
            {
                if (x < width)
                    *d = *s++;
                d++;
            }
        }

        return 1;

    }
    else if (bits == 8)
    {
        // NE_PAL256, NE_A3PAL32 or NE_A5PAL8
        // ----------------------------------

        // Cast to correct width
        u8 *d = dest;
        u8 *s = source;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < newwidth; x++)
            {
                if (x < width)
                    *d = *s++;
                d++;
            }
        }

        return 1;
    }
    else if (bits == 4)
    {
        // NE_PAL16
        // --------

        // Cast to correct width
        u8 *d = dest;
        u8 *s = source;
        int src_idx = 0;
        int dst_idx = 0;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < newwidth; x++)
            {
                if (x < width)
                {
                    if (dst_idx == 0)
                        *d = 0;

                    *d |= ((*s >> (src_idx << 2)) & 0xF)
                        << (dst_idx << 2);

                    if (src_idx == 1)
                        s++;
                    src_idx ^= 1;
                }

                if (dst_idx == 1)
                    d++;

                dst_idx ^= 1;
            }
        }

        return 1;
    }
    else if (bits == 2)
    {
        // NE_PAL4
        // -------

        // Cast to correct width
        u8 *d = dest;
        u8 *s = source;
        int src_idx = 0;
        int dst_idx = 0;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < newwidth; x++)
            {
                if (x < width)
                {
                    if (dst_idx == 0)
                        *d = 0;

                    *d |= ((*s >> (src_idx << 1)) & 0x3)
                        << (dst_idx << 1);

                    if (src_idx == 3)
                        s++;
                    src_idx = (src_idx + 1) & 3;
                }

                if (dst_idx == 3)
                    d++;
                dst_idx = (dst_idx + 1) & 3;
            }
        }

        return 1;
    }

    return 0;
}

static const int __NE_TextureSizeShift[] = {
    0, // Nothing
    1, // NE_A3PAL32
    3, // NE_PAL4
    2, // NE_PAL16
    1, // NE_PAL256
    0, // NE_COMPRESSED
    1, // NE_A5PAL8
    0, // NE_A1RGB5
    0, // NE_RGB5
};

int NE_MaterialTexLoad(NE_Material *tex, NE_TextureFormat fmt,
                       int sizeX, int sizeY, NE_TextureFlags flags,
                       void *texture)
{
    NE_AssertPointer(tex, "NULL material pointer");

    // Check if texture exists
    if (tex->texindex != NE_NO_TEXTURE)
    {
        NE_MaterialDelete(tex);
        tex = NE_MaterialCreate();
    }

    // Get free slot
    tex->texindex = NE_NO_TEXTURE;
    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_Texture[i].adress == NULL)
        {
            tex->texindex = i;
            break;
        }
    }

    if (tex->texindex == NE_NO_TEXTURE)
    {
        NE_DebugPrint("No free slots");
        tex->texindex = 0;
        return 0;
    }

    int slot = tex->texindex;

    // Save real size
    NE_Texture[slot].sizex = sizeX;
    NE_Texture[slot].sizey = sizeY;

    u32 size = 0;

    // If width is not a power of 2
    bool invalidwidth = false;
    if (__NE_GetValidSize(sizeX) != sizeX)
    {
        invalidwidth = true;
        // The width must be a power of 2, but this texture has an invalid size.
        // Let's expand the texture so that it is valid, load it to VRAM, and
        // delete the temporary buffer used to expand it.

        size = (__NE_GetValidSize(sizeX) * sizeY << 1) >>
                __NE_TextureSizeShift[fmt];

        void *newbuffer = malloc(size);
        NE_AssertPointer(newbuffer, "Not enough memory for temporary buffer");

        if (__NE_TextureResizeWidth(texture, newbuffer, fmt, sizeY, sizeX,
                                    __NE_GetValidSize(sizeX)) == 0)
        {
            free(newbuffer);
            return 0;
        }
        // New width
        sizeX = __NE_GetValidSize(sizeX);

        // Use new data
        texture = newbuffer;
    }

    // The height doesn't need to be power of 2, but we will have to cheat later
    // and make the DS believe it is a power of 2.
    if (!invalidwidth)
        size = (sizeX * sizeY << 1) >> __NE_TextureSizeShift[fmt];

    u32 *addr = (u32 *) NE_Alloc(NE_TexAllocList, size, 8);

    if (!addr)
    {
        NE_DebugPrint("Not enough memory");
        if (invalidwidth)
            free(texture); // Free temp data
        return 0;
    }

    NE_Texture[slot].adress = (void *)addr;
    // Initially only this material is using this texture
    NE_Texture[slot].uses = 1;

    // unlock texture memory
    u32 vramTemp = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                       VRAM_D_LCD);

    // We do NE_RGB5 as NE_A1RGB5, but we set each alpha bit to 1 during the
    // copy to VRAM.
    if (fmt == NE_RGB5)
    {
        u16 *src = (u16 *)texture;
        u16 *dest = (u16 *)addr;
        NE_MaterialTexParam(tex, sizeX, __NE_GetValidSize(sizeY), addr,
                            NE_A1RGB5, flags);

        while (size--)
        {
            *dest++ = *src | (1 << 15);
            src++;
        }
    }
    else
    {
        // For everything else, we do a straight copy
        NE_MaterialTexParam(tex, sizeX, __NE_GetValidSize(sizeY), addr,
                            fmt, flags);

        swiCopy((u32 *)texture, addr, (size >> 2) | COPY_MODE_WORD);
    }

    vramRestorePrimaryBanks(vramTemp);
    if (invalidwidth)
        free(texture); // Free temp data

    return 1;
}

void NE_MaterialTexClone(NE_Material *source, NE_Material *dest)
{
    NE_AssertPointer(source, "NULL source pointer");
    NE_AssertPointer(dest, "NULL dest pointer");
    NE_Assert(source->texindex != NE_NO_TEXTURE,
              "No texture asigned to source material");
    // Increase count of materials using this texture
    NE_Texture[source->texindex].uses++;
    dest->texindex = source->texindex;
}

void NE_MaterialTexSetPal(NE_Material *tex, NE_Palette *pal)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(pal, "NULL palette pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    NE_Texture[tex->texindex].palette = pal;
}

void NE_MaterialUse(const NE_Material *tex)
{
    if (tex == NULL)
    {
        GFX_TEX_FORMAT = 0;
        GFX_COLOR = NE_White;
        GFX_DIFFUSE_AMBIENT = ne_defaultdiffuse
                            | (ne_defaultambient << 16)
                            | (ne_defaultvtxcolor << 15);
        GFX_SPECULAR_EMISSION = ne_defaultspecular
                              | (ne_defaultemission << 16)
                              | (ne_defaultuseshininess << 15);
        return;
    }

    GFX_DIFFUSE_AMBIENT = tex->diffuse | (tex->ambient << 16)
                        | (tex->vtxcolor << 15);
    GFX_SPECULAR_EMISSION = tex->specular | (tex->emission << 16)
                          | (tex->useshininess << 15);

    NE_Assert(tex->texindex != NE_NO_TEXTURE,
          "No texture asigned to material");

    if (NE_Texture[tex->texindex].palette)
        NE_PaletteUse(NE_Texture[tex->texindex].palette);

    GFX_COLOR = (u32) tex->color;
    GFX_TEX_FORMAT = NE_Texture[tex->texindex].param;
}

extern bool NE_Dual;

void NE_TextureSystemReset(int max_textures, int max_palettes,
                           NE_VRAMBankFlags bank_flags)
{
    if (ne_texture_system_inited)
        NE_TextureSystemEnd();

    if (max_textures < 1)
        NE_MAX_TEXTURES = NE_DEFAULT_TEXTURES;
    else
        NE_MAX_TEXTURES = max_textures;

    NE_AllocInit(&NE_TexAllocList, (void *)VRAM_A, (void *)VRAM_E);

    NE_Assert((bank_flags & 0xF) != 0, "No VRAM banks selected");

    // Prevent user from not selecting any bank
    if ((bank_flags & 0xF) == 0)
        bank_flags = NE_VRAM_ABCD;

    // VRAM_C and VRAM_D can't be used in dual 3D mode
    if (NE_Dual)
        bank_flags &= ~NE_VRAM_CD;

    // Now, configure allocation system. The buffer size always sees the
    // four banks of VRAM. It is needed to allocate one chunk for each and
    // lock the ones that aren't allowed to be used by Nitro Engine.

    NE_Alloc(NE_TexAllocList, 128 * 1024, 0); // VRAM_A
    NE_Alloc(NE_TexAllocList, 128 * 1024, 0); // VRAM_B
    NE_Alloc(NE_TexAllocList, 128 * 1024, 0); // VRAM_C
    NE_Alloc(NE_TexAllocList, 128 * 1024, 0); // VRAM_D

    if (bank_flags & NE_VRAM_A)
    {
        vramSetBankA(VRAM_A_TEXTURE_SLOT0);
        NE_Free(NE_TexAllocList, VRAM_A);
    }
    else
    {
        NE_Lock(NE_TexAllocList, VRAM_A);
    }

    if (bank_flags & NE_VRAM_B)
    {
        vramSetBankB(VRAM_B_TEXTURE_SLOT1);
        NE_Free(NE_TexAllocList, VRAM_B);
    }
    else
    {
        NE_Lock(NE_TexAllocList, VRAM_B);
    }

    if (bank_flags & NE_VRAM_C)
    {
        vramSetBankC(VRAM_C_TEXTURE_SLOT2);
        NE_Free(NE_TexAllocList, VRAM_C);
    }
    else
    {
        NE_Lock(NE_TexAllocList, VRAM_C);
    }

    if (bank_flags & NE_VRAM_D)
    {
        vramSetBankD(VRAM_D_TEXTURE_SLOT3);
        NE_Free(NE_TexAllocList, VRAM_D);
    }
    else
    {
        NE_Lock(NE_TexAllocList, VRAM_D);
    }

    NE_Texture = calloc(NE_MAX_TEXTURES, sizeof(ne_textureinfo_t));
    NE_AssertPointer(NE_Texture, "Not enough memory");
    NE_UserMaterials = calloc(NE_MAX_TEXTURES, sizeof(NE_UserMaterials));
    NE_AssertPointer(NE_UserMaterials, "Not enough memory");

    NE_PaletteSystemReset(max_palettes);

    GFX_TEX_FORMAT = 0;

    ne_texture_system_inited = true;
}

void NE_MaterialDelete(NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");

    // If there is an asigned texture
    if (tex->texindex != NE_NO_TEXTURE)
    {
        int slot = tex->texindex;
        // A texture may be used by several materials
        NE_Texture[slot].uses--;
        // If this is the only material to use it, delete it
        if (NE_Texture[slot].uses == 0)
        {
            NE_Free(NE_TexAllocList, NE_Texture[slot].adress);
            NE_Texture[slot].adress = NULL;
            NE_Texture[slot].param = 0;
            NE_Texture[slot].palette = 0;
        }
    }

    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_UserMaterials[i] == tex)
        {
            NE_UserMaterials[i] = NULL;
            free(tex);
            return;
        }
    }

    NE_DebugPrint("Object not found");
}

int NE_TextureFreeMem(void)
{
    if (!ne_texture_system_inited)
        return 0;

    NEMemInfo Info;
    NE_MemGetInformation(NE_TexAllocList, &Info);

    return Info.Free;
}

int NE_TextureFreeMemPercent(void)
{
    if (!ne_texture_system_inited)
        return 0;

    NEMemInfo Info;
    NE_MemGetInformation(NE_TexAllocList, &Info);

    return Info.FreePercent;
}

void NE_TextureDefragMem(void)
{
    NE_Assert(0, "This function doesn't work");
    return;
    /*
    // REALLY OLD CODE -- DOESN'T WORK

    if (!ne_texture_system_inited)
        return;

    uint32 vramTemp = vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                       VRAM_D_LCD);

    bool ok = false;
    while (!ok)
    {
        ok = true;
        int i;
        for (i = 0; i < NE_MAX_TEXTURES; i++)
        {
            int size = NE_GetSize(NE_TexAllocList, (void*)NE_Texture[i].adress);
            NE_Free(NE_TexAllocList,(void*)NE_Texture[i].adress);
            void *pointer = NE_Alloc(NE_TexAllocList, size, 8);

            NE_AssertPointer(pointer, "Couldn't reallocate texture");

            if (pointer != NE_Texture[i].adress)
            {
                dmaCopy((void*) NE_Texture[i].adress, pointer, size);
                NE_Texture[i].adress = pointer;
                NE_Texture[i].param &= 0xFFFF0000;
                NE_Texture[i].param |= ((uint32)pointer >> 3) & 0xFFFF;
                ok = false;
            }
        }
    }
    vramRestoreMainBanks(vramTemp);
    */
}

void NE_TextureSystemEnd(void)
{
    if (!ne_texture_system_inited)
        return;

    NE_AllocEnd(NE_TexAllocList);

    free(NE_Texture);

    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_UserMaterials[i])
            free(NE_UserMaterials[i]);
    }

    free(NE_UserMaterials);

    NE_Texture = NULL;

    NE_PaletteSystemEnd();

    ne_texture_system_inited = false;
}

// Internal use
int __NE_TextureGetRawX(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE,
          "No texture asigned to material");
    return (NE_Texture[tex->texindex].param & (0x7 << 20)) >> 20;
}

// Internal use
int __NE_TextureGetRawY(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    return (NE_Texture[tex->texindex].param & (0x7 << 23)) >> 23;
}

int NE_TextureGetRealSizeX(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    return 8 << __NE_TextureGetRawX(tex);
}

int NE_TextureGetRealSizeY(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    return 8 << __NE_TextureGetRawY(tex);
}

int NE_TextureGetSizeX(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    return NE_Texture[tex->texindex].sizex;
}

int NE_TextureGetSizeY(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    return NE_Texture[tex->texindex].sizey;
}

void NE_MaterialSetPropierties(NE_Material *tex, u32 diffuse,
                               u32 ambient, u32 specular, u32 emission,
                               bool vtxcolor, bool useshininess)
{
    NE_AssertPointer(tex, "NULL pointer");
    tex->diffuse = diffuse;
    tex->ambient = ambient;
    tex->specular = specular;
    tex->emission = emission;
    tex->vtxcolor = vtxcolor;
    tex->useshininess = useshininess;
}

void NE_MaterialSetDefaultPropierties(u32 diffuse, u32 ambient,
                                      u32 specular, u32 emission,
                                      bool vtxcolor, bool useshininess)
{
    ne_defaultdiffuse = diffuse;
    ne_defaultambient = ambient;
    ne_defaultspecular = specular;
    ne_defaultemission = emission;
    ne_defaultvtxcolor = vtxcolor;
    ne_defaultuseshininess = useshininess;
    GFX_DIFFUSE_AMBIENT = ne_defaultdiffuse | (ne_defaultambient << 16)
                        | (ne_defaultvtxcolor << 15);
    GFX_SPECULAR_EMISSION = ne_defaultspecular | (ne_defaultemission << 16)
                          | (ne_defaultuseshininess << 15);
}

static u16 *drawingtexture_adress = NULL;
static int drawingtexture_x, drawingtexture_y;
static int drawingtexture_type;
static int drawingtexture_realx;
static u32 ne_vram_saved;

void *NE_TextureDrawingStart(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");

    NE_Assert(drawingtexture_adress == NULL,
              "Another texture is already active");

    drawingtexture_x = NE_TextureGetSizeX(tex);
    drawingtexture_realx = NE_TextureGetRealSizeX(tex);
    drawingtexture_y = NE_TextureGetSizeY(tex);
    drawingtexture_adress = (u16 *) ((uintptr_t)VRAM_A
                          + ((NE_Texture[tex->texindex].param & 0xFFFF) << 3));
    drawingtexture_type = ((NE_Texture[tex->texindex].param >> 26) & 0x7);

    ne_vram_saved = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                        VRAM_D_LCD);

    return drawingtexture_adress;
}

void NE_TexturePutPixelRGBA(u32 x, u32 y, u16 color)
{
    NE_AssertPointer(drawingtexture_adress,
                     "No texture active for drawing");
    NE_Assert(drawingtexture_type == NE_A1RGB5,
              "Ative texture isn't NE_A1RGB5");

    if (x >= drawingtexture_x || y >= drawingtexture_y)
        return;

    drawingtexture_adress[x + (y * drawingtexture_realx)] = color;
}

void NE_TexturePutPixelRGB256(u32 x, u32 y, u8 palettecolor)
{
    NE_AssertPointer(drawingtexture_adress,
                     "No texture active for drawing.");
    NE_Assert(drawingtexture_type == NE_PAL256,
              "Active texture isn't NE_PAL256");

    if (x >= drawingtexture_x || y >= drawingtexture_y)
        return;

    int position = (x + (y * drawingtexture_realx)) >> 1;
    int desp = (x & 1) << 3;

    drawingtexture_adress[position] &= 0xFF00 >> desp;
    drawingtexture_adress[position] |= ((u16) palettecolor) << desp;
}

void NE_TextureDrawingEnd(void)
{
    NE_Assert(drawingtexture_adress != NULL, "No active texture");

    vramRestorePrimaryBanks(ne_vram_saved);

    drawingtexture_adress = NULL;
}
