// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2009, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <png.h>

#define PNG_BYTES_TO_CHECK    4
#define MAX_WIDTH_HEIGHT   1024

#if !defined(PNG_SIMPLIFIED_READ_SUPPORTED)
# error "This code needs libpng 1.6"
#endif

void *LoadPNGtoARGB(char *filename, int *buffer_size)
{
    printf("Loading file %s...\n\n", filename);

    png_image image;

    // Only the image structure version number needs to be set
    memset(&image, 0, sizeof image);
    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_file(&image, filename))
    {
        printf("%s(): png_image_begin_read_from_file(): %s",
               __func__, image.message);
        return NULL;
    }

    image.format = PNG_FORMAT_RGBA;

    png_bytep buffer;
    buffer = malloc(PNG_IMAGE_SIZE(image));

    if (buffer == NULL)
    {
        png_image_free(&image);
        printf("%s(): Not enough memory", __func__);
        return NULL;
    }

    if (!png_image_finish_read(&image, NULL, buffer, 0, NULL))
    {
        printf("%s(): png_image_finish_read(): %s", __func__, image.message);
        free(buffer);
        return NULL;
    }

    int _width = image.width;
    int _height = image.height;

    printf("PNG information:\n");
    printf("   Width: %d\n", _width);
    printf("   Height: %d\n", _height);
    printf("\n");

    *buffer_size = _height * _width * 4;

    return buffer;
}
