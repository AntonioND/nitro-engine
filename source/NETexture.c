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
    // For regular textures, this is the base address in VRAM of the texture.
    // For compressed textures, this is the address in slot 0 or 2. The address
    // of the data in slot 1 can be calculated from it.
    char *address;
    int uses; // Number of materials that use this texture
    int sizex, sizey;
} ne_textureinfo_t;

static ne_textureinfo_t *NE_Texture = NULL;
static NE_Material **NE_UserMaterials = NULL;

static NEChunk *NE_TexAllocList; // See NEAlloc.h

static bool ne_texture_system_inited = false;

static int NE_MAX_TEXTURES;

// Default material properties
static u32 ne_default_diffuse_ambient;
static u32 ne_default_specular_emission;

static int ne_is_valid_tex_size(int size)
{
    for (int i = 0; i < 8; i++)
    {
        if (size <= (8 << i))
            return (8 << i);
    }
    return 0;
}

static int ne_tex_raw_size(int size)
{
    for (int i = 0; i < 8; i++)
    {
        if (size == 8)
            return i;
        size >>= 1;
    }
    return 0;
}

// The provided address must be in VRAM_A
static inline void *slot0_to_slot1(void *ptr)
{
    uintptr_t offset0 = (uintptr_t)ptr - (uintptr_t)VRAM_A;
    return (void *)((uintptr_t)VRAM_B + (offset0 / 2));
}

// The provided address must be in VRAM_B
static inline void *slot1_to_slot0(void *ptr)
{
    uintptr_t offset1 = (uintptr_t)ptr - (uintptr_t)VRAM_B;
    return (void *)((uintptr_t)VRAM_A + (offset1 * 2));
}

// The provided address must be in VRAM_C
static inline void *slot2_to_slot1(void *ptr)
{
    uintptr_t offset2 = (uintptr_t)ptr - (uintptr_t)VRAM_C;
    return (void *)((uintptr_t)VRAM_B + (64 * 1024) + (offset2 / 2));
}

// The provided address must be in VRAM_B
static inline void *slot1_to_slot2(void *ptr)
{
    uintptr_t offset1 = (uintptr_t)ptr - (uintptr_t)VRAM_B - (64 * 1024);
    return (void *)((uintptr_t)VRAM_C + (offset1 * 2));
}

static inline void ne_set_material_tex_param(NE_Material *tex,
                            int sizeX, int sizeY, uint32_t *addr,
                            GL_TEXTURE_TYPE_ENUM mode, u32 param)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No assigned texture");
    NE_Texture[tex->texindex].param =
            (ne_tex_raw_size(sizeX) << 20) |
            (ne_tex_raw_size(sizeY) << 23) |
            (((uint32_t)addr >> 3) & 0xFFFF) |
            (mode << 26) | param;
}

static void ne_texture_delete(int texture_index)
{
    int slot = texture_index;

    // A texture may be used by several materials
    NE_Texture[slot].uses--;

    // If the number of users is zero, delete it.
    if (NE_Texture[slot].uses == 0)
    {
        uint32_t fmt = (NE_Texture[slot].param >> 26) & 7;

        if (fmt == NE_TEX4X4)
        {
            // Check if the texture is allocated in VRAM_A or VRAM_C, and
            // calculate the corresponding address in VRAM_B.
            void *slot02 = NE_Texture[slot].address;
            void *slot1 = (slot02 < (void *)VRAM_B) ?
                          slot0_to_slot1(slot02) : slot2_to_slot1(slot02);
            NE_Free(NE_TexAllocList, slot02);
            NE_Free(NE_TexAllocList, slot1);
        }
        else
        {
            NE_Free(NE_TexAllocList, NE_Texture[slot].address);
        }

        NE_Texture[slot].address = NULL;
        NE_Texture[slot].param = 0;
    }
}

//--------------------------------------------------------------------------

NE_Material *NE_MaterialCreate(void)
{
    if (!ne_texture_system_inited)
    {
        NE_DebugPrint("System not initialized");
        return NULL;
    }

    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_UserMaterials[i] != NULL)
            continue;

        NE_Material *mat = calloc(1, sizeof(NE_Material));
        if (mat == NULL)
        {
            NE_DebugPrint("Not enough memory");
            return NULL;
        }

        NE_UserMaterials[i] = mat;
        mat->texindex = NE_NO_TEXTURE;
        mat->palette = NULL;
        mat->color = NE_White;
        mat->diffuse_ambient = ne_default_diffuse_ambient;
        mat->specular_emission = ne_default_specular_emission;

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

int NE_MaterialTexLoadGRF(NE_Material *tex, NE_Palette *pal,
                          NE_TextureFlags flags, const char *path)
{
#ifndef NE_BLOCKSDS
    NE_DebugPrint("%s only supported in BlocksDS", __func__);
    return 0;
#else // NE_BLOCKSDS
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(path, "NULL path pointer");

    int ret = 0;

    void *gfxDst = NULL;
    void *palDst = NULL;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header, &gfxDst, NULL, NULL, NULL,
                               &palDst, NULL);
    if (err != GRF_NO_ERROR)
    {
        NE_DebugPrint("Couldn't load GRF file: %d", err);
        goto cleanup;
    }

    if (gfxDst == NULL)
    {
        NE_DebugPrint("No graphics found in GRF file");
        goto cleanup;
    }

    NE_TextureFormat fmt;
    switch (header.gfxAttr)
    {
        case GRF_TEXFMT_A5I3:
            fmt = NE_A5PAL8;
            break;
        case GRF_TEXFMT_A3I5:
            fmt = NE_A3PAL32;
            break;
        case GRF_TEXFMT_4x4:
            fmt = NE_TEX4X4;
            break;
        case 16:
            fmt = NE_A1RGB5;
            break;
        case 8:
            fmt = NE_PAL256;
            break;
        case 4:
            fmt = NE_PAL16;
            break;
        case 2:
            fmt = NE_PAL4;
            break;
        default:
            NE_DebugPrint("Invalid format in GRF file");
            goto cleanup;
    }

    if (NE_MaterialTexLoad(tex, fmt, header.gfxWidth, header.gfxHeight,
                           flags, gfxDst) == 0)
    {
        NE_DebugPrint("Failed to load GRF texture");
        goto cleanup;
    }

    // If there is no palette to be loaded there is nothing else to do
    if (palDst == NULL)
    {
        ret = 1; // Success
        goto cleanup;
    }

    // Okay, there is a palette to load. Make sure that the user has provided a
    // palette object.
    if (pal == NULL)
    {
        NE_DebugPrint("GRF with a palette, but no palette object provided");
        goto cleanup;
    }

    if (NE_PaletteLoadSize(pal, palDst, header.palAttr * 2, fmt) == 0)
    {
        NE_DebugPrint("Failed to load GRF palette");
        goto cleanup;
    }

    NE_MaterialSetPalette(tex, pal);

    ret = 1; // Success

cleanup:
    free(gfxDst);
    free(palDst);
    return ret;
#endif // NE_BLOCKSDS
}

int NE_MaterialTexLoadFAT(NE_Material *tex, NE_TextureFormat fmt,
                          int sizeX, int sizeY, NE_TextureFlags flags,
                          const char *path)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(path, "NULL path pointer");
    NE_Assert(sizeX > 0 && sizeY > 0, "Size must be positive");

    void *ptr = NE_FATLoadData(path);
    if (ptr == NULL)
    {
        NE_DebugPrint("Couldn't load file from FAT");
        return 0;
    }

    int ret = NE_MaterialTexLoad(tex, fmt, sizeX, sizeY, flags, ptr);
    free(ptr);

    return ret;
}

int NE_MaterialTex4x4LoadFAT(NE_Material *tex, int sizeX, int sizeY,
                             NE_TextureFlags flags, const char *path02,
                             const char *path1)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(path02, "NULL path02 pointer");
    NE_AssertPointer(path1, "NULL path1 pointer");
    NE_Assert(sizeX > 0 && sizeY > 0, "Size must be positive");

    void *texture02 = NE_FATLoadData(path02);
    if (texture02 == NULL)
    {
        NE_DebugPrint("Couldn't load file from FAT");
        return 0;
    }

    void *texture1 = NE_FATLoadData(path1);
    if (texture1 == NULL)
    {
        NE_DebugPrint("Couldn't load file from FAT");
        free(texture02);
        return 0;
    }

    int ret = NE_MaterialTex4x4Load(tex, sizeX, sizeY, flags, texture02,
                                    texture1);

    free(texture02);
    free(texture1);

    return ret;
}

// This function takes as argument the size of the chunk of the compressed
// texture chunk that goes into slots 0 or 2. The size that goes into slot 1 is
// always half of this size, so it isn't needed to provide it.
//
// It returns 0 on success, as well as pointers to the address where both chunks
// need to be copied.
static int ne_alloc_compressed_tex(size_t size, void **slot02, void **slot1)
{
    size_t size02 = size;
    size_t size1 = size / 2;

    // First, try with slot 0 + slot 1
    // -------------------------------

    // Get the first valid range in slot 0
    void *addr0 = NE_AllocFindInRange(NE_TexAllocList, VRAM_A, VRAM_B, size02);
    if (addr0 != NULL)
    {
        // Only use the first half of slot 1 for slot 0
        void *addr1;
        void *addr1_end = (void *)((uintptr_t)VRAM_B + (64 * 1024));

        while (1)
        {
            // Get the address in bank 1 assigned to the current bank 0 address
            addr1 = slot0_to_slot1(addr0);

            // Check if this address is free and has enough space
            addr1 = NE_AllocFindInRange(NE_TexAllocList, addr1, addr1_end, size1);
            if (addr1 == NULL)
                break;

            // If both addresses match, both of them are free
            if (addr1 == slot0_to_slot1(addr0))
            {
                *slot02 = addr0;
                *slot1 = addr1;
                return 0;
            }

            // Get the address in bank 0 assigned to the current bank 1 address
            addr0 = slot1_to_slot0(addr1);

            // Check if this address is free and has enough space
            addr0 = NE_AllocFindInRange(NE_TexAllocList, addr0, VRAM_B, size02);
            if (addr0 == NULL)
                break;

            // If both addresses match, both of them are free
            if (addr1 == slot0_to_slot1(addr0))
            {
                *slot02 = addr0;
                *slot1 = addr1;
                return 0;
            }
        }
    }

    // Then, try with slot 2 + slot 1
    // ------------------------------

    // Get the first valid range in slot 2
    void *addr2 = NE_AllocFindInRange(NE_TexAllocList, VRAM_C, VRAM_D, size02);
    if (addr2 == NULL)
        return -1;

    // Only use the second half of slot 1 for slot 2
    void *addr1;
    void *addr1_end = VRAM_C;

    while (1)
    {
        // Get the address in bank 1 assigned to the current bank 2 address
        addr1 = slot2_to_slot1(addr2);

        // Check if this address is free and has enough space
        addr1 = NE_AllocFindInRange(NE_TexAllocList, addr1, addr1_end, size1);
        if (addr1 == NULL)
            break;

        // If both addresses match, both of them are free
        if (addr1 == slot2_to_slot1(addr2))
        {
            *slot02 = addr2;
            *slot1 = addr1;
            return 0;
        }

        // Get the address in bank 2 assigned to the current bank 1 address
        addr2 = slot1_to_slot2(addr1);

        // Check if this address is free and has enough space
        addr2 = NE_AllocFindInRange(NE_TexAllocList, addr2, VRAM_B, size02);
        if (addr2 == NULL)
            break;

        // If both addresses match, both of them are free
        if (addr1 == slot2_to_slot1(addr2))
        {
            *slot02 = addr2;
            *slot1 = addr1;
            return 0;
        }
    }

    return -1;
}

int NE_MaterialTex4x4Load(NE_Material *tex, int sizeX, int sizeY,
                          NE_TextureFlags flags, const void *texture02,
                          const void *texture1)
{
    NE_AssertPointer(tex, "NULL material pointer");

    // For tex4x4 textures, both width and height must be valid
    if ((ne_is_valid_tex_size(sizeX) != sizeX)
        || (ne_is_valid_tex_size(sizeY) != sizeY))
    {
        NE_DebugPrint("Width and height of tex4x4 textures must be a power of 2");
        return 0;
    }

    // Check if a texture exists
    if (tex->texindex != NE_NO_TEXTURE)
        ne_texture_delete(tex->texindex);

    // Get free slot
    tex->texindex = NE_NO_TEXTURE;
    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_Texture[i].address == NULL)
        {
            tex->texindex = i;
            break;
        }
    }

    if (tex->texindex == NE_NO_TEXTURE)
    {
        NE_DebugPrint("No free slots");
        return 0;
    }

    size_t size02 = (sizeX * sizeY) >> 2;
    size_t size1 = size02 >> 1;

    void *slot02, *slot1;
    int ret = ne_alloc_compressed_tex(size02, &slot02, &slot1);
    if (ret != 0)
    {
        NE_DebugPrint("Can't find space for compressed texture");
        return 0;
    }

    ret = NE_AllocAddress(NE_TexAllocList, slot02, size02);
    if (ret != 0)
    {
        NE_DebugPrint("Can't allocate slot 0/2");
        return 0;
    }

    ret = NE_AllocAddress(NE_TexAllocList, slot1, size1);
    if (ret != 0)
    {
        NE_Free(NE_TexAllocList, slot02);
        NE_DebugPrint("Can't allocate slot 1");
        return 0;
    }

    // Save information
    int slot = tex->texindex;
    NE_Texture[slot].sizex = sizeX;
    NE_Texture[slot].sizey = sizeY;
    NE_Texture[slot].address = slot02;
    NE_Texture[slot].uses = 1; // Initially only this material uses the texture

    // Unlock texture memory for writing
    // TODO: Only unlock the banks that Nitro Engine uses.
    u32 vramTemp = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                        VRAM_D_LCD);

    swiCopy(texture02, slot02, (size02 >> 2) | COPY_MODE_WORD);
    swiCopy(texture1, slot1, (size1 >> 2) | COPY_MODE_WORD);

    int hardware_size_y = ne_is_valid_tex_size(sizeY);
    ne_set_material_tex_param(tex, sizeX, hardware_size_y, slot02,
                              NE_TEX4X4, flags);

    vramRestorePrimaryBanks(vramTemp);

    return 1;
}

int NE_MaterialTexLoad(NE_Material *tex, NE_TextureFormat fmt,
                       int sizeX, int sizeY, NE_TextureFlags flags,
                       const void *texture)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_Assert(fmt != 0, "No texture format provided");

    if (fmt == NE_TEX4X4)
    {
        // Split tex4x4 texture into its two parts, that have been concatenated

        size_t size02 = (sizeX * sizeY) >> 2;

        const void *texture02 = texture;
        const void *texture1 = (const void *)((uintptr_t)texture + size02);

        return NE_MaterialTex4x4Load(tex, sizeX, sizeY, flags,
                                     texture02, texture1);
    }

    // The width of a texture must be a power of 2. The height doesn't need to
    // be a power of 2, but we will have to cheat later and make the DS believe
    // it is a power of 2.
    if (ne_is_valid_tex_size(sizeX) != sizeX)
    {
        NE_DebugPrint("Width of textures must be a power of 2");
        return 0;
    }

    // Check if a texture exists
    if (tex->texindex != NE_NO_TEXTURE)
        ne_texture_delete(tex->texindex);

    // Get free slot
    tex->texindex = NE_NO_TEXTURE;
    for (int i = 0; i < NE_MAX_TEXTURES; i++)
    {
        if (NE_Texture[i].address == NULL)
        {
            tex->texindex = i;
            break;
        }
    }

    if (tex->texindex == NE_NO_TEXTURE)
    {
        NE_DebugPrint("No free slots");
        return 0;
    }

    // All non-compressed texture types are handled here

    const int size_shift[] = {
        0, // Nothing
        1, // NE_A3PAL32
        3, // NE_PAL4
        2, // NE_PAL16
        1, // NE_PAL256
        0, // NE_TEX4X4 (This value isn't used)
        1, // NE_A5PAL8
        0, // NE_A1RGB5
        0, // NE_RGB5
    };

    uint32_t size = (sizeX * sizeY << 1) >> size_shift[fmt];

    // This pointer must be aligned to 8 bytes at least
    void *addr = NE_AllocFromEnd(NE_TexAllocList, size);
    if (!addr)
    {
        tex->texindex = NE_NO_TEXTURE;
        NE_DebugPrint("Not enough memory");
        return 0;
    }

    // Save information
    int slot = tex->texindex;
    NE_Texture[slot].sizex = sizeX;
    NE_Texture[slot].sizey = sizeY;
    NE_Texture[slot].address = addr;
    NE_Texture[slot].uses = 1; // Initially only this material uses the texture

    // Unlock texture memory for writing
    // TODO: Only unlock the banks that Nitro Engine uses.
    u32 vramTemp = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                       VRAM_D_LCD);

    if (fmt == NE_RGB5)
    {
        // Treat NE_RGB5 as NE_A1RGB5, but set each alpha bit to 1 during the
        // copy to VRAM.
        u16 *src = (u16 *)texture;
        u16 *dest = addr;
        size >>= 1; // We are going to process two bytes each iteration
        while (size--)
            *dest++ = *src++ | (1 << 15);

        fmt = NE_A1RGB5;
    }
    else
    {
        swiCopy((u32 *)texture, addr, (size >> 2) | COPY_MODE_WORD);
    }

    int hardware_size_y = ne_is_valid_tex_size(sizeY);
    ne_set_material_tex_param(tex, sizeX, hardware_size_y, addr, fmt, flags);

    vramRestorePrimaryBanks(vramTemp);

    return 1;
}

void NE_MaterialClone(NE_Material *source, NE_Material *dest)
{
    NE_AssertPointer(source, "NULL source pointer");
    NE_AssertPointer(dest, "NULL dest pointer");
    NE_Assert(source->texindex != NE_NO_TEXTURE,
              "No texture asigned to source material");
    // Increase count of materials using this texture
    NE_Texture[source->texindex].uses++;
    memcpy(dest, source, sizeof(NE_Material));
}

void NE_MaterialSetPalette(NE_Material *tex, NE_Palette *pal)
{
    NE_AssertPointer(tex, "NULL material pointer");
    NE_AssertPointer(pal, "NULL palette pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");
    tex->palette = pal;
}

void NE_MaterialUse(const NE_Material *tex)
{
    if (tex == NULL)
    {
        GFX_TEX_FORMAT = 0;
        GFX_COLOR = NE_White;
        GFX_DIFFUSE_AMBIENT = ne_default_diffuse_ambient;
        GFX_SPECULAR_EMISSION = ne_default_specular_emission;
        return;
    }

    GFX_DIFFUSE_AMBIENT = tex->diffuse_ambient;
    GFX_SPECULAR_EMISSION = tex->specular_emission;

    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");

    if (tex->palette)
        NE_PaletteUse(tex->palette);

    GFX_COLOR = tex->color;
    GFX_TEX_FORMAT = NE_Texture[tex->texindex].param;
}

int NE_TextureSystemReset(int max_textures, int max_palettes,
                          NE_VRAMBankFlags bank_flags)
{
    if (ne_texture_system_inited)
        NE_TextureSystemEnd();

    NE_Assert((bank_flags & 0xF) != 0, "No VRAM banks selected");

    if (max_textures < 1)
        NE_MAX_TEXTURES = NE_DEFAULT_TEXTURES;
    else
        NE_MAX_TEXTURES = max_textures;

    if (NE_PaletteSystemReset(max_palettes) != 0)
        return -1;

    NE_Texture = calloc(NE_MAX_TEXTURES, sizeof(ne_textureinfo_t));
    NE_UserMaterials = calloc(NE_MAX_TEXTURES, sizeof(NE_UserMaterials));
    if ((NE_Texture == NULL) || (NE_UserMaterials == NULL))
        goto cleanup;

    if (NE_AllocInit(&NE_TexAllocList, VRAM_A, VRAM_E) != 0)
        goto cleanup;

    // Prevent user from not selecting any bank
    if ((bank_flags & 0xF) == 0)
        bank_flags = NE_VRAM_ABCD;

    // VRAM_C and VRAM_D can't be used in dual 3D mode
    if (NE_CurrentExecutionMode() != NE_ModeSingle3D)
        bank_flags &= ~NE_VRAM_CD;

    // Now, configure allocation system. The buffer size always sees the
    // four banks of VRAM. It is needed to allocate and lock one chunk per bank
    // that isn't allocated to Nitro Engine.

    if (bank_flags & NE_VRAM_A)
    {
        vramSetBankA(VRAM_A_TEXTURE_SLOT0);
    }
    else
    {
        NE_AllocAddress(NE_TexAllocList, VRAM_A, 128 * 1024);
        NE_Lock(NE_TexAllocList, VRAM_A);
    }

    if (bank_flags & NE_VRAM_B)
    {
        vramSetBankB(VRAM_B_TEXTURE_SLOT1);
    }
    else
    {
        NE_AllocAddress(NE_TexAllocList, VRAM_B, 128 * 1024);
        NE_Lock(NE_TexAllocList, VRAM_B);
    }

    if (bank_flags & NE_VRAM_C)
    {
        vramSetBankC(VRAM_C_TEXTURE_SLOT2);
    }
    else
    {
        NE_AllocAddress(NE_TexAllocList, VRAM_C, 128 * 1024);
        NE_Lock(NE_TexAllocList, VRAM_C);
    }

    if (bank_flags & NE_VRAM_D)
    {
        vramSetBankD(VRAM_D_TEXTURE_SLOT3);
    }
    else
    {
        NE_AllocAddress(NE_TexAllocList, VRAM_D, 128 * 1024);
        NE_Lock(NE_TexAllocList, VRAM_D);
    }

    GFX_TEX_FORMAT = 0;

    ne_texture_system_inited = true;
    return 0;

cleanup:
    NE_DebugPrint("Not enough memory");
    NE_PaletteSystemEnd();
    free(NE_Texture);
    free(NE_UserMaterials);
    return -1;
}

void NE_MaterialDelete(NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");

    // If there is an asigned texture
    if (tex->texindex != NE_NO_TEXTURE)
        ne_texture_delete(tex->texindex);

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

    NEMemInfo info;
    NE_MemGetInformation(NE_TexAllocList, &info);

    return info.free;
}

int NE_TextureFreeMemPercent(void)
{
    if (!ne_texture_system_inited)
        return 0;

    NEMemInfo info;
    NE_MemGetInformation(NE_TexAllocList, &info);

    return info.free_percent;
}

void NE_TextureDefragMem(void)
{
    NE_Assert(0, "This function doesn't work");
    return;
    /*
    // REALLY OLD CODE -- DOESN'T WORK

    if (!ne_texture_system_inited)
        return;

    uint32_t vramTemp = vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                       VRAM_D_LCD);

    bool ok = false;
    while (!ok)
    {
        ok = true;
        int i;
        for (i = 0; i < NE_MAX_TEXTURES; i++)
        {
            int size = NE_GetSize(NE_TexAllocList, (void*)NE_Texture[i].address);
            NE_Free(NE_TexAllocList,(void*)NE_Texture[i].address);
            void *pointer = NE_Alloc(NE_TexAllocList, size);
            // Aligned to 8 bytes

            NE_AssertPointer(pointer, "Couldn't reallocate texture");

            if (pointer != NE_Texture[i].address)
            {
                dmaCopy((void *)NE_Texture[i].address, pointer, size);
                NE_Texture[i].address = pointer;
                NE_Texture[i].param &= 0xFFFF0000;
                NE_Texture[i].param |= ((uint32_t)pointer >> 3) & 0xFFFF;
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

    NE_AllocEnd(&NE_TexAllocList);

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

void NE_MaterialSetProperties(NE_Material *tex, u32 diffuse,
                              u32 ambient, u32 specular, u32 emission,
                              bool vtxcolor, bool useshininess)
{
    NE_AssertPointer(tex, "NULL pointer");
    tex->diffuse_ambient = diffuse | (ambient << 16) | (vtxcolor << 15);
    tex->specular_emission = specular | (emission << 16) | (useshininess << 15);
}

void NE_MaterialSetDefaultProperties(u32 diffuse, u32 ambient,
                                     u32 specular, u32 emission,
                                     bool vtxcolor, bool useshininess)
{
    ne_default_diffuse_ambient = diffuse | (ambient << 16) | (vtxcolor << 15);
    ne_default_specular_emission = specular | (emission << 16)
                                 | (useshininess << 15);

    GFX_DIFFUSE_AMBIENT = ne_default_diffuse_ambient;
    GFX_SPECULAR_EMISSION = ne_default_specular_emission;
}

static u16 *drawingtexture_address = NULL;
static int drawingtexture_x, drawingtexture_y;
static int drawingtexture_type;
static int drawingtexture_realx;
static u32 ne_vram_saved;

void *NE_TextureDrawingStart(const NE_Material *tex)
{
    NE_AssertPointer(tex, "NULL pointer");
    NE_Assert(tex->texindex != NE_NO_TEXTURE, "No texture asigned to material");

    NE_Assert(drawingtexture_address == NULL,
              "Another texture is already active");

    drawingtexture_x = NE_TextureGetSizeX(tex);
    drawingtexture_realx = NE_TextureGetRealSizeX(tex);
    drawingtexture_y = NE_TextureGetSizeY(tex);
    drawingtexture_address = (u16 *) ((uintptr_t)VRAM_A
                          + ((NE_Texture[tex->texindex].param & 0xFFFF) << 3));
    drawingtexture_type = ((NE_Texture[tex->texindex].param >> 26) & 0x7);

    ne_vram_saved = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD,
                                        VRAM_D_LCD);

    return drawingtexture_address;
}

void NE_TexturePutPixelRGBA(u32 x, u32 y, u16 color)
{
    NE_AssertPointer(drawingtexture_address,
                     "No texture active for drawing");
    NE_Assert(drawingtexture_type == NE_A1RGB5,
              "Ative texture isn't NE_A1RGB5");

    if (x >= drawingtexture_x || y >= drawingtexture_y)
        return;

    drawingtexture_address[x + (y * drawingtexture_realx)] = color;
}

void NE_TexturePutPixelRGB256(u32 x, u32 y, u8 palettecolor)
{
    NE_AssertPointer(drawingtexture_address,
                     "No texture active for drawing.");
    NE_Assert(drawingtexture_type == NE_PAL256,
              "Active texture isn't NE_PAL256");

    if (x >= drawingtexture_x || y >= drawingtexture_y)
        return;

    int position = (x + (y * drawingtexture_realx)) >> 1;
    int desp = (x & 1) << 3;

    drawingtexture_address[position] &= 0xFF00 >> desp;
    drawingtexture_address[position] |= ((u16) palettecolor) << desp;
}

void NE_TextureDrawingEnd(void)
{
    NE_Assert(drawingtexture_address != NULL, "No active texture");

    vramRestorePrimaryBanks(ne_vram_saved);

    drawingtexture_address = NULL;
}
