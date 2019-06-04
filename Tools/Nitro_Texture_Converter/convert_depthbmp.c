// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdio.h>

int ConvertARGBintoDEPTHBMP(void *data, int size, char *texture_filename)
{
	printf("DEPTHBMP:\n");
	printf("- Depth is calculated like this:\n");
	printf("    if(R) -> 0x00000 (Hides 2D + 3D)\n");
	printf("    else 0x67FF + (0x1800 * (B) / 255) (Gradually hides 3D)\n");
	printf("  NOTE: It may change in the future...\n");
	printf("- If image alpha == 0 -> Fog = 1.\n");
	printf("- If image alpha != 0 -> Fog = 0.\n\n");

	FILE *OUTPUT_FILE = fopen(texture_filename, "wb+");

	if (!OUTPUT_FILE) {
		printf("Couldn't open %s in write mode!!\n\n",
		       texture_filename);
		return -1;
	}

	unsigned char *data_pointer = (unsigned char *)data;

	unsigned short pixel_data = 0;

	int a;
	for (a = 0; a < size; a += 4) {
		//int depth = ((data_pointer[a] + data_pointer[a+1] + data_pointer[a+2]) << 6 ) / 3;

		if (data_pointer[a])
			pixel_data = 0x00000;
		else
			pixel_data =
			    0x67FF + (0x1800 * data_pointer[a + 2] / 255);

		pixel_data &= 0x7FFF;
		if (data_pointer[a + 3] == 0)
			pixel_data |= 1 << 15;	// If alpha == 0 -> Fog enabled

		if (fwrite(&pixel_data, sizeof(pixel_data), 1, OUTPUT_FILE) !=
		    1) {
			fclose(OUTPUT_FILE);
			printf("Write error!!\n\n");
			return -1;
		}
	}

	fclose(OUTPUT_FILE);

	printf("Created file: %s\n\n", texture_filename);

	return 1;
}
