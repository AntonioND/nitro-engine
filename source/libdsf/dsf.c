// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2024 Antonio Niño Díaz

#include <stdint.h>
#include <stdio.h>

#include <nds.h>

#include <dsf.h>

// BMFont format structures
// ------------------------

typedef struct __attribute__((packed)) {
    uint8_t magic[3]; // "BMF"
    uint8_t version;  // 3
} bmf_header;

typedef struct __attribute__((packed)) {
    uint8_t type;     // 1, 2, 3, 4 or 5
    uint8_t size[4];  // Size of the block
    uint8_t data[];
} bmf_block_header;

typedef struct __attribute__((packed)) {
    uint16_t line_height; // Distance in pixels between each line of text.
    uint16_t base; // Number of pixels from the top of the line to the base of the characters.
    uint16_t scale_w;
    uint16_t scale_h;
    uint16_t pages;
    uint8_t  bit_field;
    uint8_t  alpha_channel;
    uint8_t  red_channel;
    uint8_t  green_channel;
    uint8_t  blue_channel;
} bmf_block_2_common;

// The number of characters in one file can be calculated by calculating the
// result of "size / sizeof(bmf_block_4_char)"
typedef struct __attribute__((packed)) {
    uint32_t id;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    int16_t  xoffset;
    int16_t  yoffset;
    int16_t  xadvance;
    uint8_t  page;
    uint8_t  channel;
} bmf_block_4_char;

typedef struct __attribute__((packed)) {
    uint32_t first;
    uint32_t second;
    uint16_t amount;
} bmf_block_5_kerning_pair;

// Internal data structures
// ------------------------

typedef bmf_block_4_char block_char;

typedef struct {
    uint32_t first;
    uint32_t second;
    int16_t  amount;
} kerning_pair;

typedef struct {
    uint16_t      line_height;
    uint16_t      base;

    size_t        num_chars;
    block_char   *chars;
    size_t        num_kernings;
    kerning_pair *kernings;

    // Variables used for the current printing context
    int16_t       pointer_x;
    int16_t       pointer_y;
    int16_t       box_left;
    int16_t       box_top;
    uint32_t      last_codepoint;

    // If the font includes a replacement character glyph, its index will be
    // saved here for ease of access.
    int32_t       replacement_character_id;
} dsf_font_internal_state;

// Functions
// ---------

static int DSF_CodepointFindGlyphIndex(dsf_handle handle, uint32_t codepoint)
{
    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    for (size_t i = 0; i < font->num_chars; i++)
    {
        block_char *ch = &(font->chars[i]);

        if (ch->id == codepoint)
            return i;
    }

    return font->replacement_character_id;
}

static dsf_error DSF_LoadFile(const char *path, void **data, size_t *_size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return DSF_FILE_OPEN_ERROR;

    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return DSF_FILE_SEEK_ERROR;
    }

    size_t size = ftell(f);
    if (size == 0)
    {
        fclose(f);
        return DSF_FILE_EMPTY;
    }

    rewind(f);

    char *buffer = malloc(size);
    if (buffer == NULL)
    {
        fclose(f);
        return DSF_NO_MEMORY;
    }

    if (fread(buffer, 1, size, f) != size)
    {
        fclose(f);
        return DSF_FILE_READ_ERROR;
    }

    fclose(f);

    *_size = size;
    *data = buffer;
    return DSF_NO_ERROR;
}

dsf_error DSF_LoadFontMemory(dsf_handle *handle,
                             const void *data, int32_t data_size)
{
    dsf_error ret = DSF_NO_ERROR;
    const uint8_t *ptr = data;

    // Basic checks

    if ((handle == NULL) || (data == NULL) || (data_size <= 0))
        return DSF_INVALID_ARGUMENT;

    // Read header

    const bmf_header *header = (const bmf_header *)ptr;

    if (!((header->magic[0] == 'B') && (header->magic[1] == 'M') &&
          (header->magic[2] == 'F')))
        return DSF_BAD_MAGIC;

    if (header->version != 3)
        return DSF_BAD_VERSION;

    ptr += sizeof(bmf_header);
    data_size -= sizeof(bmf_header);

    // Allocate space for the handle

    dsf_font_internal_state *font = calloc(1, sizeof(dsf_font_internal_state));
    if (font == NULL)
        return DSF_NO_MEMORY;

    *handle = (dsf_handle)font; // Return handle to user

    // Read blocks

    while (1)
    {
        const bmf_block_header *block_header = (const bmf_block_header *)ptr;

        uint8_t type = block_header->type;
        uint32_t block_size = ((uint32_t)block_header->size[0]) |
                              ((uint32_t)block_header->size[1] << 8) |
                              ((uint32_t)block_header->size[2] << 16) |
                              ((uint32_t)block_header->size[3] << 24);

        const uint8_t *data = ptr + 1 + 4;

        if (type == 2)
        {
            // Ensure the size is correct
            if (block_size != sizeof(bmf_block_2_common))
            {
                ret = DSF_BAD_CHUNK_SIZE;
                goto error;
            }

            bmf_block_2_common block_common;
            memcpy(&block_common, data, block_size);

            font->line_height = block_common.line_height;
            font->base = block_common.base;
        }
        else if (type == 4)
        {
            // Ensure the total size is a multiple of the block size
            if (block_size % sizeof(bmf_block_4_char) != 0)
            {
                ret = DSF_BAD_CHUNK_SIZE;
                goto error;
            }

            font->num_chars = block_size / sizeof(bmf_block_4_char);
            font->chars = calloc(font->num_chars, sizeof(block_char));
            if (font->chars == NULL)
            {
                ret = DSF_NO_MEMORY;
                goto error;
            }

            memcpy(font->chars, data, block_size);
        }
        else if (type == 5)
        {
            // Ensure the total size is a multiple of the block size
            if (block_size % sizeof(bmf_block_5_kerning_pair) != 0)
            {
                ret = DSF_BAD_CHUNK_SIZE;
                goto error;
            }

            font->num_kernings = block_size / sizeof(bmf_block_5_kerning_pair);
            font->kernings = calloc(font->num_kernings, sizeof(kerning_pair));
            if (font->kernings == NULL)
            {
                ret = DSF_NO_MEMORY;
                goto error;
            }

            const uint8_t *src = data;
            uint8_t *dst = (uint8_t *)font->kernings;

            for (size_t i = 0; i < font->num_kernings; i++)
            {
                memcpy(dst, src, sizeof(bmf_block_5_kerning_pair));
                dst += sizeof(kerning_pair);
                src += sizeof(bmf_block_5_kerning_pair);
            }
        }

        ptr += block_size + 1 + 4;
        data_size -= block_size + 1 + 4;

        if (data_size == 0)
            break;

        if (data_size < 0)
        {
            ret = DSF_UNEXPECTED_END;
            goto error;
        }
    }

    if (font->num_chars == 0)
    {
        ret = DSF_NO_CHARACTERS;
        goto error;
    }

    // Look for a replacement character glyph in the font.

    // Initialize the value to -1 so that DSF_CodepointFindGlyphIndex() returns
    // -1 if no replacement character glyph is found.
    font->replacement_character_id = -1;
    font->replacement_character_id =
                    DSF_CodepointFindGlyphIndex(*handle, REPLACEMENT_CHARACTER);

    return DSF_NO_ERROR;

error:
    free(font->chars);
    free(font->kernings);
    free(font);
    return ret;
}

dsf_error DSF_FreeFont(dsf_handle *handle)
{
    if (handle == NULL)
        return DSF_INVALID_ARGUMENT;

    dsf_font_internal_state *font = (dsf_font_internal_state *)*handle;

    free(font->chars);
    free(font->kernings);
    free(font);

    *handle = 0;

    return DSF_NO_ERROR;
}

dsf_error DSF_LoadFontFilesystem(dsf_handle *handle, const char *path)
{
    if ((handle == NULL) || (path == NULL))
        return DSF_INVALID_ARGUMENT;

    size_t size;
    void *data;
    dsf_error error = DSF_LoadFile(path, &data, &size);
    if (error != DSF_NO_ERROR)
        return error;

    error = DSF_LoadFontMemory(handle, data, size);
    free(data);
    return error;
}

static size_t DSF_UTF8_CodepointRead(const char *str, uint32_t *codepoint)
{
    // https://en.wikipedia.org/wiki/UTF-8#Encoding

    size_t size;
    uint32_t rune;

    uint32_t b1 = str[0];

    if ((b1 & 0x80) == 0)
    {
        size = 1;
        *codepoint = b1 & 0x7F;
        return 1;
    }
    else if ((b1 & 0xE0) == 0xC0)
    {
        size = 2;
        rune = b1 & 0x1F;
    }
    else if ((b1 & 0xF0) == 0xE0)
    {
        size = 3;
        rune = b1 & 0x0F;
    }
    else if ((b1 & 0xF8) == 0xF0)
    {
        size = 4;
        rune = b1 & 0x07;
    }
    else
    {
        goto error;
    }

    for (size_t i = 1; i < size; i++)
    {
        uint32_t b = str[i];
        if ((b & 0xC0) != 0x80)
            goto error;

        rune <<= 6;
        rune |= b & 0x3F;
    }

    *codepoint = rune;
    return size;

error:

    // Incorrect encoding. Advance characters until we find one character
    // that isn't a continuation character.
    size = 1;
    str++;

    while (1)
    {
        uint8_t c = *str++;
        size++;
        if ((c & 0xC0) != 0x80)
            break;
    }

    *codepoint = REPLACEMENT_CHARACTER;
    return size;
}

dsf_error DSF_CodepointRenderDryRun(dsf_handle handle, uint32_t codepoint)
{
    if ((handle == 0) || (codepoint == 0))
        return DSF_INVALID_ARGUMENT;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    if (codepoint == '\n')
    {
        font->pointer_x = font->box_left;
        font->pointer_y += font->line_height;
        return DSF_NO_ERROR;
    }

    int index = DSF_CodepointFindGlyphIndex(handle, codepoint);
    if (index < 0)
        return DSF_CODEPOINT_NOT_FOUND;

    block_char *ch = &(font->chars[index]);

    int x1 = font->pointer_x;
    int x2 = x1 + ch->width;
    int y1 = font->pointer_y;
    int y2 = y1 + ch->height;

    x1 += ch->xoffset;
    x2 += ch->xoffset;
    y1 += ch->yoffset;
    y2 += ch->yoffset;

    font->pointer_x += ch->xadvance;

    for (size_t i = 0; i < font->num_kernings; i++)
    {
        kerning_pair *ker = &(font->kernings[i]);

        if (ker->first != font->last_codepoint)
            continue;

        if (ker->second != codepoint)
            continue;

        x1 += ker->amount;
        x2 += ker->amount;
        font->pointer_x += ker->amount;
    }

    font->last_codepoint = codepoint;

    return DSF_NO_ERROR;
}

static dsf_error DSF_CodepointRender3D(dsf_handle handle, uint32_t codepoint,
                                       int16_t z)
{
    if ((handle == 0) || (codepoint == 0))
        return DSF_INVALID_ARGUMENT;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    if (codepoint == '\n')
    {
        font->pointer_x = font->box_left;
        font->pointer_y += font->line_height;
        return DSF_NO_ERROR;
    }

    int index = DSF_CodepointFindGlyphIndex(handle, codepoint);
    if (index < 0)
        return DSF_CODEPOINT_NOT_FOUND;

    block_char *ch = &(font->chars[index]);

    int tx1 = ch->x;
    int tx2 = tx1 + ch->width;
    int ty1 = ch->y;
    int ty2 = ty1 + ch->height;

    int x1 = font->pointer_x;
    int x2 = x1 + ch->width;
    int y1 = font->pointer_y;
    int y2 = y1 + ch->height;

    x1 += ch->xoffset;
    x2 += ch->xoffset;
    y1 += ch->yoffset;
    y2 += ch->yoffset;

    font->pointer_x += ch->xadvance;

    for (size_t i = 0; i < font->num_kernings; i++)
    {
        kerning_pair *ker = &(font->kernings[i]);

        if (ker->first != font->last_codepoint)
            continue;

        if (ker->second != codepoint)
            continue;

        x1 += ker->amount;
        x2 += ker->amount;
        font->pointer_x += ker->amount;
    }

    font->last_codepoint = codepoint;

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(tx1), inttot16(ty1));
    GFX_VERTEX16 = (y1 << 16) | (x1 & 0xFFFF); // Up-left
    GFX_VERTEX16 = z;

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(tx1), inttot16(ty2));
    GFX_VERTEX_XY = (y2 << 16) | (x1 & 0xFFFF); // Down-left

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(tx2), inttot16(ty2));
    GFX_VERTEX_XY = (y2 << 16) | (x2 & 0xFFFF); // Down-right

    GFX_TEX_COORD = TEXTURE_PACK(inttot16(tx2), inttot16(ty1));
    GFX_VERTEX_XY = (y1 << 16) | (x2 & 0xFFFF); // Up-right

    return DSF_NO_ERROR;
}

dsf_error DSF_StringRenderDryRun(dsf_handle handle, const char *str,
                                 size_t *size_x, size_t *size_y)
{
    if ((handle == 0) || (str == NULL) || (size_x == NULL) || (size_y == NULL))
        return DSF_INVALID_ARGUMENT;

    if (strlen(str) == 0)
    {
        *size_x = 0;
        *size_y = 0;
        return DSF_NO_ERROR;
    }

    dsf_error ret = DSF_NO_ERROR;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    font->pointer_x = 0;
    font->pointer_y = 0;
    font->box_left = 0;
    font->box_top = 0;
    font->last_codepoint = 0;

    const char *readptr = str;

    size_t max_x = 0;
    size_t max_y = 0;

    while (*readptr != '\0')
    {
        uint32_t codepoint;
        size_t size = DSF_UTF8_CodepointRead(readptr, &codepoint);
        readptr += size;

        ret = DSF_CodepointRenderDryRun(handle, codepoint);
        if (ret != DSF_NO_ERROR)
            break;

        if (font->pointer_x > max_x)
            max_x = font->pointer_x;
        if (font->pointer_y > max_y)
            max_y = font->pointer_y;
    }

    *size_x = max_x;
    *size_y = max_y + font->line_height;

    return ret;
}

dsf_error DSF_StringRender3D(dsf_handle handle, const char *str,
                             int32_t x, int32_t y, int32_t z)
{
    if ((handle == 0) || (str == NULL))
        return DSF_INVALID_ARGUMENT;

    dsf_error ret = DSF_NO_ERROR;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    font->pointer_x = x;
    font->pointer_y = y;
    font->box_left = x;
    font->box_top = y;
    font->last_codepoint = 0;

    const char *readptr = str;

    glBegin(GL_QUADS);

    while (*readptr != '\0')
    {
        uint32_t codepoint;
        size_t size = DSF_UTF8_CodepointRead(readptr, &codepoint);
        readptr += size;

        ret = DSF_CodepointRender3D(handle, codepoint, z);
        if (ret != DSF_NO_ERROR)
            break;
    }

    glEnd();

    return ret;
}

dsf_error DSF_StringRender3DAlpha(dsf_handle handle, const char *str,
                                  int32_t x, int32_t y, int32_t z,
                                  uint32_t poly_fmt, int poly_id_base)
{
    if ((handle == 0) || (str == NULL))
        return DSF_INVALID_ARGUMENT;

    dsf_error ret = DSF_NO_ERROR;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    font->pointer_x = x;
    font->pointer_y = y;
    font->box_left = x;
    font->box_top = y;
    font->last_codepoint = 0;

    const char *readptr = str;

    int id_index = 0;

    while (*readptr != '\0')
    {
        uint32_t codepoint;
        size_t size = DSF_UTF8_CodepointRead(readptr, &codepoint);
        readptr += size;

        glPolyFmt(poly_fmt | POLY_ID(poly_id_base + id_index));
        id_index ^= 1;

        glBegin(GL_QUADS);
        ret = DSF_CodepointRender3D(handle, codepoint, z);
        glEnd();
        if (ret != DSF_NO_ERROR)
            break;
    }

    return ret;
}

static dsf_error DSF_CodepointRenderBuffer(dsf_handle handle,
                    uint32_t codepoint, unsigned int texture_fmt,
                    const void *font_texture, size_t font_width, size_t font_height,
                    void *out_texture, size_t out_width, size_t out_height)
{
    // Don't check if the arguments are valid, this is an internal function

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    if (codepoint == '\n')
    {
        font->pointer_x = font->box_left;
        font->pointer_y += font->line_height;
        return DSF_NO_ERROR;
    }

    int index = DSF_CodepointFindGlyphIndex(handle, codepoint);
    if (index < 0)
        return DSF_CODEPOINT_NOT_FOUND;

    block_char *ch = &(font->chars[index]);

    int tx1 = ch->x;
    int ty1 = ch->y;

    int x1 = font->pointer_x;
    int x2 = x1 + ch->width;
    int y1 = font->pointer_y;
    int y2 = y1 + ch->height;

    x1 += ch->xoffset;
    x2 += ch->xoffset;
    y1 += ch->yoffset;
    y2 += ch->yoffset;

    font->pointer_x += ch->xadvance;

    for (size_t i = 0; i < font->num_kernings; i++)
    {
        kerning_pair *ker = &(font->kernings[i]);

        if (ker->first != font->last_codepoint)
            continue;

        if (ker->second != codepoint)
            continue;

        x1 += ker->amount;
        x2 += ker->amount;
        font->pointer_x += ker->amount;
    }

    font->last_codepoint = codepoint;

    if (texture_fmt == GL_RGB256)
    {
        const uint8_t *src = font_texture;
        uint8_t *dst = out_texture;

        src += tx1 + ty1 * font_width;
        dst += x1 + y1 * out_width;

        for (int y = 0; y <= y2 - y1; y++)
        {
            const uint8_t *src_row = src;
            uint8_t *dst_row = dst;

            for (int x = 0; x <= x2 - x1; x++)
            {
                uint8_t color = *src_row++;
                if (color != 0)
                    *dst_row = color;
                dst_row++;
            }

            src += font_width;
            dst += out_width;
        }
    }
    else if (texture_fmt == GL_RGBA)
    {
        const uint16_t *src = font_texture;
        uint16_t *dst = out_texture;

        src += tx1 + ty1 * font_width;
        dst += x1 + y1 * out_width;

        for (int y = 0; y <= y2 - y1; y++)
        {
            const uint16_t *src_row = src;
            uint16_t *dst_row = dst;

            for (int x = 0; x <= x2 - x1; x++)
            {
                uint16_t color = *src_row++;
                if (color & BIT(15))
                    *dst_row = color;
                dst_row++;
            }

            src += font_width;
            dst += out_width;
        }
    }
    else if ((texture_fmt == GL_RGB32_A3) || (texture_fmt == GL_RGB8_A5))
    {
        const uint8_t *src = font_texture;
        uint8_t *dst = out_texture;

        src += tx1 + ty1 * font_width;
        dst += x1 + y1 * out_width;

        int alpha_shift;

        if (texture_fmt == GL_RGB32_A3)
            alpha_shift = 5;
        else // if (texture_fmt == GL_RGB8_A5)
            alpha_shift = 3;

        for (int y = 0; y <= y2 - y1; y++)
        {
            const uint8_t *src_row = src;
            uint8_t *dst_row = dst;

            for (int x = 0; x <= x2 - x1; x++)
            {
                uint8_t color = *src_row++;
                // We can't really blend two different colors because we're
                // limited by the palette. For that reason, if the new alpha is
                // 0 we leave the old entry. If the new alpha is anything other
                // than 0, we replace the old entry by the new entry, even if
                // the result is incorrect.
                if ((color >> alpha_shift) != 0)
                    *dst_row = color;
                dst_row++;
            }

            src += font_width;
            dst += out_width;
        }
    }
    else if (texture_fmt == GL_RGB16)
    {
        const uint8_t *src = font_texture;
        uint8_t *dst = out_texture;

        for (int y = 0; y <= y2 - y1; y++)
        {
            const uint8_t *src_row = src + (((ty1 + y) * font_width) >> 1);
            uint8_t *dst_row = dst + (((y1 + y) * out_width) >> 1);

            for (int x = 0; x <= x2 - x1; x++)
            {
                const uint8_t *src_px = src_row + ((tx1 + x) >> 1);
                uint8_t *dst_px = dst_row + ((x1 + x) >> 1);

                int shift = ((tx1 + x) & 1) * 4;
                uint8_t src_color = (*src_px >> shift) & 0xF;

                if (src_color == 0)
                    continue;

                shift = ((x1 + x) & 1) * 4;
                uint8_t mask = ~(0xF << shift);
                *dst_px = (*dst_px & mask) | (src_color << shift);
            }
        }
    }
    else if (texture_fmt == GL_RGB4)
    {
        const uint8_t *src = font_texture;
        uint8_t *dst = out_texture;

        for (int y = 0; y <= y2 - y1; y++)
        {
            const uint8_t *src_row = src + (((ty1 + y) * font_width) >> 2);
            uint8_t *dst_row = dst + (((y1 + y) * out_width) >> 2);

            for (int x = 0; x <= x2 - x1; x++)
            {
                const uint8_t *src_px = src_row + ((tx1 + x) >> 2);
                uint8_t *dst_px = dst_row + ((x1 + x) >> 2);

                int shift = ((tx1 + x) & 3) * 2;
                uint8_t src_color = (*src_px >> shift) & 0x3;

                if (src_color == 0)
                    continue;

                shift = ((x1 + x) & 3) * 2;
                uint8_t mask = ~(0x3 << shift);
                *dst_px = (*dst_px & mask) | (src_color << shift);
            }
        }
    }

    return DSF_NO_ERROR;
}

dsf_error DSF_StringRenderToTexture(dsf_handle handle,
                    const char *str, unsigned int texture_fmt,
                    const void *font_texture, size_t font_width, size_t font_height,
                    void **out_texture, size_t *out_width, size_t *out_height)
{
    if ((handle == 0) || (str == NULL) || (font_texture == NULL) ||
        (out_texture == NULL) || (out_width == NULL) || (out_height == NULL))
        return DSF_INVALID_ARGUMENT;

    if ((texture_fmt < 1) || (texture_fmt > 7) || (texture_fmt == GL_COMPRESSED))
        return DSF_TEXTURE_BAD_FORMAT;

    if ((font_width == 0) || (font_height == 0))
        return DSF_INVALID_ARGUMENT;

    if (strlen(str) == 0)
        return DSF_INVALID_ARGUMENT;

    // Start process

    dsf_error ret = DSF_NO_ERROR;

    dsf_font_internal_state *font = (dsf_font_internal_state *)handle;

    // Get size

    size_t tex_width, tex_height;

    ret = DSF_StringRenderDryRun(handle, str, &tex_width, &tex_height);
    if (ret != DSF_NO_ERROR)
        return ret;

    if ((tex_width > 1024) || (tex_height > 1024))
        return DSF_TEXTURE_TOO_BIG;

    // Expand to a valid texture size

    for (size_t i = 8; i <= 1024; i <<= 1)
    {
        if (tex_width <= i)
        {
            tex_width = i;
            break;
        }
    }

    for (size_t i = 8; i <= 1024; i <<= 1)
    {
        if (tex_height <= i)
        {
            tex_height = i;
            break;
        }
    }

    const int size_shift[] = {
        0, // Nothing
        1, // A3PAL32
        3, // PAL4
        2, // PAL16
        1, // PAL256
        0, // TEX4X4 (This value isn't used)
        1, // A5PAL8
        0, // A1RGB5
        0, // RGB5
    };

    size_t tex_size = (2 * tex_width * tex_height) >> size_shift[texture_fmt];

    // Allocate buffer

    void *tex_buffer = calloc(1, tex_size);
    memset(tex_buffer, 0, tex_size);
    if (tex_buffer == NULL)
        return DSF_NO_MEMORY;

    // Render string

    font->pointer_x = 0;
    font->pointer_y = 0;
    font->box_left = 0;
    font->box_top = 0;
    font->last_codepoint = 0;

    const char *readptr = str;

    while (*readptr != '\0')
    {
        uint32_t codepoint;
        size_t size = DSF_UTF8_CodepointRead(readptr, &codepoint);
        readptr += size;

        ret = DSF_CodepointRenderBuffer(handle, codepoint, texture_fmt,
                                        font_texture, font_width, font_height,
                                        tex_buffer, tex_width, tex_height);
        if (ret != DSF_NO_ERROR)
            break;
    }

    // Return texture information

    *out_texture = tex_buffer;
    *out_width = tex_width;
    *out_height = tex_height;

    return ret;
}
