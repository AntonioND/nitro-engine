// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"

int ConvertARGBintoA1RGB5(void *data, int size, char *texture_filename)
{
	printf("A1RGB5:\n");
	printf("- If image alpha == 0 -> Result alpha = 0.\n");
	printf("- If image alpha != 0 -> Result alpha = 1.\n\n");

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
		pixel_data = (data_pointer[a] >> 3) & 31;		//Red
		pixel_data |= ((data_pointer[a + 1] >> 3) & 31) << 5;	//Green
		pixel_data |= ((data_pointer[a + 2] >> 3) & 31) << 10;	//Blue
		pixel_data |= (data_pointer[a + 3] > 0) << 15;		//Alpha

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
