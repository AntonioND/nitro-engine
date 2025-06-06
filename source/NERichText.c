// SPDX-License-Identifier: MIT
//
// Copyright (c) 2024 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

#include "libdsf/dsf.h"

/// @file NERichText.c

typedef struct {
    // Fields used when the font texture is stored in VRAM
    NE_Material *material;
    NE_Palette *palette;

    // Fields used when the font texture is stored in RAM
    bool has_to_free_buffers;
    NE_TextureFormat fmt;
    void *texture_buffer;
    size_t texture_width;
    size_t texture_height;
    void *palette_buffer;
    size_t palette_size;

    // Other fields
    dsf_handle handle;
    bool active;
} ne_rich_textinfo_t;

static u32 NE_NumRichTextSlots = 0;

static ne_rich_textinfo_t *NE_RichTextInfo;

static int NE_RICH_TEXT_PRIORITY = 0;

void NE_RichTextPrioritySet(int priority)
{
    NE_RICH_TEXT_PRIORITY = priority;
}

void NE_RichTextPriorityReset(void)
{
    NE_RICH_TEXT_PRIORITY = 0;
}

void NE_RichTextInit(u32 slot)
{
    // Compatibility mode -- if someone tries to allocate a slot
    // without having called start system, we allocate the old maximum
    // number of slots for safety
    if (NE_NumRichTextSlots == 0)
        NE_RichTextStartSystem(NE_DEFAULT_RICH_TEXT_FONTS);

    if (slot >= NE_NumRichTextSlots)
    {
        NE_DebugPrint("Attempted to initialize a slot greater than the number of slots allocated; skipping");
        return;
    }

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (info->active)
        NE_RichTextEnd(slot);

    memset(info, 0, sizeof(ne_rich_textinfo_t));
    info->active = true;

    NE_RichTextPriorityReset();
}

int NE_RichTextEnd(u32 slot)
{
    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    if (info->material != NULL)
        NE_MaterialDelete(info->material);
    if (info->palette != NULL)
        NE_PaletteDelete(info->palette);

    if (info->has_to_free_buffers)
    {
        if (info->texture_buffer != NULL)
            free(info->texture_buffer);
        if (info->palette_buffer != NULL)
            free(info->palette_buffer);
    }

    int ret = 0;

    if (info->handle)
    {
        dsf_error err = DSF_FreeFont(&(info->handle));
        if (err != DSF_NO_ERROR)
            ret = err;
    }

    memset(info, 0, sizeof(ne_rich_textinfo_t));

    if (ret != 0)
        return 0;

    return 1;
}

int NE_RichTextStartSystem(u32 numSlots)
{
    NE_NumRichTextSlots = numSlots;
    NE_RichTextInfo = calloc(sizeof(ne_rich_textinfo_t), NE_NumRichTextSlots);
    if (NE_RichTextInfo == NULL)
    {
        NE_DebugPrint("Failed to allocate array for NE_RichTextInfo");
        return 0;
    }
    return 1;
}

void NE_RichTextResetSystem(void)
{
    for (int i = 0; i < NE_NumRichTextSlots; i++)
        NE_RichTextEnd(i);
    free(NE_RichTextInfo);
    NE_NumRichTextSlots = 0;
}

int NE_RichTextMetadataLoadFAT(u32 slot, const char *path)
{
    NE_AssertPointer(path, "NULL path pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    dsf_handle handle;
    dsf_error ret = DSF_LoadFontFilesystem(&handle, path);
    if (ret != DSF_NO_ERROR)
    {
        NE_DebugPrint("DSF_LoadFontFilesystem(): %d\n", ret);
        return 0;
    }

    info->handle = handle;

    return 1;
}

int NE_RichTextMetadataLoadMemory(u32 slot, const void *data, size_t data_size)
{
    NE_AssertPointer(data, "NULL data pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    dsf_handle handle;
    dsf_error ret = DSF_LoadFontMemory(&handle, data, data_size);
    if (ret != DSF_NO_ERROR)
    {
        NE_DebugPrint("DSF_LoadFontMemory(): %d\n", ret);
        return 0;
    }

    info->handle = handle;

    return 1;
}

int NE_RichTextMaterialLoadGRF(u32 slot, const char *path)
{
    NE_AssertPointer(path, "NULL path pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    info->material = NE_MaterialCreate();
    info->palette = NE_PaletteCreate();

    int ret = NE_MaterialTexLoadGRF(info->material, info->palette,
                    NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT, path);
    if (ret == 0)
    {
        NE_MaterialDelete(info->material);
        NE_PaletteDelete(info->palette);
        return 0;
    }

    return 1;
}

int NE_RichTextMaterialSet(u32 slot, NE_Material *mat, NE_Palette *pal)
{
    NE_AssertPointer(mat, "NULL material pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    info->material = mat;
    info->palette = pal;

    return 1;
}

int NE_RichTextBitmapLoadGRF(u32 slot, const char *path)
{
#ifndef NE_BLOCKSDS
    (void)slot;
    (void)path;
    NE_DebugPrint("%s only supported in BlocksDS", __func__);
    return 0;
#else // NE_BLOCKSDS
    NE_AssertPointer(path, "NULL path pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    if (info->has_to_free_buffers)
    {
        if (info->texture_buffer != NULL)
            free(info->texture_buffer);
        if (info->palette_buffer != NULL)
            free(info->palette_buffer);
    }

    void *gfxDst = NULL;
    void *palDst = NULL;
    size_t palSize;
    GRFHeader header = { 0 };
    GRFError err = grfLoadPath(path, &header, &gfxDst, NULL, NULL, NULL,
                               &palDst, &palSize);
    if (err != GRF_NO_ERROR)
    {
        NE_DebugPrint("Couldn't load GRF file: %d", err);
        goto error;
    }

    if (gfxDst == NULL)
    {
        NE_DebugPrint("No graphics found in GRF file");
        goto error;
    }

    bool palette_required = true;
    switch (header.gfxAttr)
    {
        case GRF_TEXFMT_A5I3:
            info->fmt = NE_A5PAL8;
            break;
        case GRF_TEXFMT_A3I5:
            info->fmt = NE_A3PAL32;
            break;
        case GRF_TEXFMT_4x4:
            info->fmt = NE_TEX4X4;
            break;
        case 16:
            info->fmt = NE_A1RGB5;
            palette_required = false;
            break;
        case 8:
            info->fmt = NE_PAL256;
            break;
        case 4:
            info->fmt = NE_PAL16;
            break;
        case 2:
            info->fmt = NE_PAL4;
            break;
        default:
            NE_DebugPrint("Invalid format in GRF file");
            goto error;
    }

    info->texture_buffer = gfxDst;
    info->texture_width = header.gfxWidth;
    info->texture_height = header.gfxHeight;

    if (palDst != NULL)
    {
        info->palette_buffer = palDst;
        info->palette_size = palSize;
    }
    else
    {
        if (palette_required)
        {
            NE_DebugPrint("No palette found in GRF, but format requires it");
            goto error;
        }

        info->palette_buffer = NULL;
        info->palette_size = 0;
    }

    info->has_to_free_buffers = true;

    return 1; // Success

error:
    free(gfxDst);
    free(palDst);
    return 0;
#endif // NE_BLOCKSDS
}

int NE_RichTextBitmapSet(u32 slot, const void *texture_buffer,
                         size_t texture_width, size_t texture_height,
                         NE_TextureFormat texture_fmt,
                         const void *palette_buffer, size_t palette_size)
{
    NE_AssertPointer(texture_buffer, "NULL texture pointer");
    NE_AssertPointer(palette_buffer, "NULL palette pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    if (info->has_to_free_buffers)
    {
        if (info->texture_buffer != NULL)
            free(info->texture_buffer);
        if (info->palette_buffer != NULL)
            free(info->palette_buffer);
    }

    info->has_to_free_buffers = false;

    info->texture_buffer = (void *)texture_buffer;
    info->texture_width = texture_width;
    info->texture_height = texture_height;
    info->fmt = texture_fmt;
    info->palette_buffer = (void *)palette_buffer;
    info->palette_size = palette_size;

    return 1;
}

int NE_RichTextRenderDryRunWithPos(u32 slot, const char *str,
                            size_t *size_x, size_t *size_y,
                            size_t *final_x, size_t *final_y)
{
    NE_AssertPointer(str, "NULL str pointer");
    NE_AssertPointer(size_x, "NULL size X pointer");
    NE_AssertPointer(size_y, "NULL size Y pointer");
    NE_AssertPointer(final_x, "NULL final X pointer");
    NE_AssertPointer(final_y, "NULL final Y pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    dsf_error err = DSF_StringRenderDryRunWithCursor(info->handle, str,
                                                     size_x, size_y,
                                                     final_x, final_y);
    if (err != DSF_NO_ERROR)
        return 0;

    return 1;
}

int NE_RichTextRenderDryRun(u32 slot, const char *str,
                            size_t *size_x, size_t *size_y)
{
    size_t final_x, final_y;
    return NE_RichTextRenderDryRunWithPos(slot, str, size_x, size_y, &final_x, &final_y);
}

int NE_RichTextRender3DWithIndent(u32 slot, const char *str, s32 x, s32 y,
                                  s32 xIndent)
{
    NE_AssertPointer(str, "NULL str pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    NE_MaterialUse(info->material);

    dsf_error err = DSF_StringRender3DWithIndent(info->handle, str, x, y,
                                               NE_RICH_TEXT_PRIORITY, xIndent);
    if (err != DSF_NO_ERROR)
        return 0;

    return 1;
}

int NE_RichTextRender3D(u32 slot, const char *str, s32 x, s32 y)
{
    return NE_RichTextRender3DWithIndent(slot, str, x, y, 0);
}

int NE_RichTextRender3DAlphaWithIndent(u32 slot, const char *str, s32 x, s32 y,
                                       uint32_t poly_fmt, int poly_id_base,
                                       s32 xIndent)
{
    NE_AssertPointer(str, "NULL str pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    NE_MaterialUse(info->material);

    dsf_error err = DSF_StringRender3DAlphaWithIndent(info->handle, str, x, y,
                                                      NE_RICH_TEXT_PRIORITY,
                                                      poly_fmt, poly_id_base, xIndent);
    if (err != DSF_NO_ERROR)
        return 0;

    return 1;
}

int NE_RichTextRender3DAlpha(u32 slot, const char *str, s32 x, s32 y,
                             uint32_t poly_fmt, int poly_id_base)
{
    return NE_RichTextRender3DAlphaWithIndent(slot, str, x, y, poly_fmt, poly_id_base, 0);
}

int NE_RichTextRenderMaterial(u32 slot, const char *str, NE_Material **mat,
                              NE_Palette **pal)
{
    NE_AssertPointer(str, "NULL str pointer");
    NE_AssertPointer(mat, "NULL mat pointer");
    NE_AssertPointer(pal, "NULL pal pointer");

    if (slot >= NE_NumRichTextSlots)
        return 0;

    ne_rich_textinfo_t *info = &NE_RichTextInfo[slot];
    if (!info->active)
        return 0;

    void *out_texture = NULL;
    size_t out_width, out_height;
    dsf_error err = DSF_StringRenderToTexture(info->handle,
                            str, info->fmt, info->texture_buffer,
                            info->texture_width, info->texture_height,
                            &out_texture, &out_width, &out_height);
    if (err != DSF_NO_ERROR)
    {
        free(out_texture);
        return 0;
    }

    *mat = NE_MaterialCreate();
    if (NE_MaterialTexLoad(*mat, info->fmt, out_width, out_height,
                           NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT,
                           out_texture) == 0)
    {
        free(out_texture);
        return 0;
    }

    if (info->palette_buffer != NULL)
    {
        NE_Palette *palette = NE_PaletteCreate();
        if (NE_PaletteLoad(palette, info->palette_buffer,
                           info->palette_size / 2, info->fmt) == 0)
        {
            NE_MaterialDelete(*mat);
            free(out_texture);
            return 0;
        }

        NE_MaterialSetPalette(*mat, palette);

        // If the caller has requested the pointer to the palette, return the
        // value to the user. If not, mark it to be autodeleted.
        if (pal)
            *pal = palette;
        else
            NE_MaterialAutodeletePalette(*mat);
    }

    // This isn't needed after it has been loaded to VRAM
    free(out_texture);

    return 1;
}
