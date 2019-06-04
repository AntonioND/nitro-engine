// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdio.h>
#include "palette.h"

int ConvertARGBintoA3RGB32(void *data, int size, char *texture_filename,
			   char *palette_filename)
{
	printf("A3RGB32:\n");
	printf("- If the image has more than 32 colors the program\n");
	printf("  will try to optimise the palette (discouraged).\n\n");

	unsigned char *data_pointer = (unsigned char *)data;

	int a;

	printf("Creating palette...\n\n");

	int colors_used;
	int optimization = 0;

	// Try to find a palette
	while (1) {
		Palette_New(32, optimization);

		for (a = 0; a < size; a += 4) {
			// Alpha not used in palettes
			Palette_NewColor((data_pointer[a] >> 3) & 31,
					 (data_pointer[a + 1] >> 3) & 31,
					 (data_pointer[a + 2] >> 3) & 31);
		}

		colors_used = Palette_GetColorsUsed();

		if (colors_used != -1)
			break;	// OK ;)

		optimization++;	// Let's try again...
	}

	if (optimization > 0)
		printf("Palette optimized after %d tries.\n\n", optimization);

	printf("The palette has got %d colors.\n\n", colors_used);

	// Now, save it to a file...
	FILE *OUTPUT_FILE = fopen(palette_filename, "wb+");

	if (!OUTPUT_FILE) {
		printf("Couldn't open %s in write mode!!\n\n",
		       palette_filename);
		return -1;
	}

	unsigned short color = 0;
	int red = 0, green = 0, blue = 0;

	int b;
	for (b = 0; b < colors_used; b++) {
		Palette_GetColor(b, &red, &green, &blue);

		color = red & 31;
		color |= (green & 31) << 5;
		color |= (blue & 31) << 10;
		// Alpha not used in palettes

		if (fwrite(&color, sizeof(color), 1, OUTPUT_FILE) != 1) {
			fclose(OUTPUT_FILE);
			printf("Write error!!\n\n");
			return -1;
		}
	}

	fclose(OUTPUT_FILE);

	printf("Created file: %s\n\n", palette_filename);

	// Palette created. Now, let's generate the texture
	printf("Creating texture...\n\n");

	OUTPUT_FILE = fopen(texture_filename, "wb+");

	if (!OUTPUT_FILE) {
		printf("Couldn't open %s in write mode!!\n\n",
		       texture_filename);
		return -1;
	}

	unsigned char index_ = 0;
	unsigned char alpha_ = 0;

	int c;
	for (c = 0; c < size; c += 4) {
		index_ = Palette_GetIndex((data_pointer[c] >> 3) & 31,
					  (data_pointer[c + 1] >> 3) & 31,
					  (data_pointer[c + 2] >> 3) & 31);

		alpha_ = (data_pointer[c + 3] >> 5) & 0x7;

		unsigned char save_to_file = (alpha_ << 5) | (index_ & 31);

		if (fwrite(&save_to_file, sizeof(save_to_file), 1, OUTPUT_FILE)
		    != 1) {
			fclose(OUTPUT_FILE);
			printf("Write error!!\n\n");
			return -1;
		}
	}

	fclose(OUTPUT_FILE);

	printf("Created file: %s\n\n", texture_filename);

	return 1;
}
