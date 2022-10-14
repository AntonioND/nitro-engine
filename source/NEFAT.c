// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "NEMain.h"

/// @file NEFAT.c

char *NE_FATLoadData(const char *filename)
{
    FILE *f = fopen(filename, "rb+");
    if (f == NULL)
    {
        NE_DebugPrint("%s could't be opened", filename);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    char *buffer = malloc(size);
    if (buffer == NULL)
    {
        NE_DebugPrint("Not enought memory to load %s", filename);
        return NULL;
    }

    fread(buffer, 1, size, f);
    fclose(f);
    return buffer;
}

size_t NE_FATFileSize(const char *filename)
{
    FILE *f = fopen(filename, "rb+");
    if (f == NULL)
    {
        NE_DebugPrint("%s could't be opened", filename);
        return -1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fclose(f);
    return size;
}

static void NE_write16(u16 *address, u16 value)
{
    u8 *first = (u8 *)address;
    u8 *second = first + 1;

    *first = value & 0xff;
    *second = value >> 8;
}

static void NE_write32(u32 *address, u32 value)
{
    u8 *first = (u8 *) address;
    u8 *second = first + 1;
    u8 *third = first + 2;
    u8 *fourth = first + 3;

    *first = value & 0xff;
    *second = (value >> 8) & 0xff;
    *third = (value >> 16) & 0xff;
    *fourth = (value >> 24) & 0xff;
}

extern bool NE_Dual;

int NE_ScreenshotBMP(const char *filename)
{
    FILE *f = fopen(filename, "wb");

    if (f == NULL)
    {
        NE_DebugPrint("%s could't be opened", filename);
        return 0;
    }

    NE_SpecialEffectPause(true);

    // In normal 3D mode we need to capture the composited (3D+2D output)
    // and save it to VRAM. In dual 3D mode it already is in VRAM.
    if (!NE_Dual)
    {
        vramSetBankD(VRAM_D_LCD);

        REG_DISPCAPCNT = DCAP_BANK(DCAP_BANK_VRAM_D)
                       | DCAP_SIZE(DCAP_SIZE_256x192)
                       | DCAP_MODE(DCAP_MODE_A)
                       | DCAP_SRC_A(DCAP_SRC_A_COMPOSITED)
                       | DCAP_ENABLE;

        while (REG_DISPCAPCNT & DCAP_ENABLE) ;
    }

    int ysize = 0;

    if (NE_Dual)
        ysize = 384;
    else
        ysize = 192;

    u8 *temp = (u8 *)malloc(256 * ysize * 3
                            + sizeof(NE_BMPInfoHeader)
                            + sizeof(NE_BMPHeader));

    NE_BMPHeader *header = (NE_BMPHeader *) temp;
    NE_BMPInfoHeader *infoheader =
        (NE_BMPInfoHeader *)(temp + sizeof(NE_BMPHeader));

    NE_write16(&header->type, 0x4D42);
    NE_write32(&header->size, 256 * ysize * 3 + sizeof(NE_BMPInfoHeader)
                              + sizeof(NE_BMPHeader));
    NE_write32(&header->offset,
           sizeof(NE_BMPInfoHeader) + sizeof(NE_BMPHeader));
    NE_write16(&header->reserved1, 0);
    NE_write16(&header->reserved2, 0);

    NE_write16(&infoheader->bits, 24);
    NE_write32(&infoheader->size, sizeof(NE_BMPInfoHeader));
    NE_write32(&infoheader->compression, 0);
    NE_write32(&infoheader->width, 256);
    NE_write32(&infoheader->height, ysize);
    NE_write16(&infoheader->planes, 1);
    NE_write32(&infoheader->imagesize, 256 * ysize * 3);
    NE_write32(&infoheader->xresolution, 0);
    NE_write32(&infoheader->yresolution, 0);
    NE_write32(&infoheader->importantcolors, 0);
    NE_write32(&infoheader->ncolors, 0);

    // Allow CPU to access VRAM
    uint32 vramTemp = 0;
    if (NE_Dual)
    {
        vramTemp = vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD,
                                       VRAM_C_LCD, VRAM_D_LCD);
    }

    for (int y = 0; y < ysize; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            u16 color = 0;

            if (NE_Dual)
            {
                if (y > 191)
                    color = VRAM_C[256 * 192 - (y - 192 + 1) * 256 + x];
                else
                    color = VRAM_D[256 * 192 - (y + 1) * 256 + x];
            }
            else
            {
                color = VRAM_D[256 * 192 - (y + 1) * 256 + x];
            }

            u8 b = (color & 31) << 3;
            u8 g = ((color >> 5) & 31) << 3;
            u8 r = ((color >> 10) & 31) << 3;

            int index = ((y * 256) + x) * 3
                      + sizeof(NE_BMPInfoHeader)
                      + sizeof(NE_BMPHeader);

            temp[index + 0] = r;
            temp[index + 1] = g;
            temp[index + 2] = b;
        }
    }

    if (NE_Dual)
        vramRestorePrimaryBanks(vramTemp);

    DC_FlushAll();
    fwrite(temp, 1, 256 * ysize * 3 + sizeof(NE_BMPInfoHeader)
                    + sizeof(NE_BMPHeader), f);
    fclose(f);
    free(temp);

    // TODO: Restore previous value, not just unpause
    NE_SpecialEffectPause(false);

    return 1;
}
