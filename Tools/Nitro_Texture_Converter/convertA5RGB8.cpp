// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdio.h>
#include "palette.h"

int ConvertARGBintoA5RGB8(void * data, int size, char * texture_filename, char * palette_filename)
{
	printf("A5RGB8: -If image has more than 8 colors the program will try\n");
	printf("         to optimize the palette (not recommended).\n\n");
	
	unsigned char * data_pointer = (unsigned char*) data;

	int a;
	
	printf("Creating palette...\n\n");

	int colors_used;
	int optimization = 0;

	while(1) //Try to find a palette
		{
		Palette_New(8,optimization);

		for(a = 0; a < size; a+=4)
			{
			//Alpha not used in palettes
			Palette_NewColor((data_pointer[a]>>3) & 31,(data_pointer[a+1]>>3) & 31,(data_pointer[a+2]>>3) & 31);
			}
	
		colors_used = Palette_GetColorsUsed();

		if(colors_used != -1) break; //OK ;)

		optimization ++; //Let's try again...
		}

	if(optimization > 0) printf("Palette optimized after %d tries.\n\n",optimization);

	printf("The palette has got %d colors.\n\n", colors_used);

	//Now, save it to a file...
	FILE * OUTPUT_FILE = fopen(palette_filename,"wb+");
	
	if(!OUTPUT_FILE)
		{
		printf("Couldn't open %s in write mode!!\n\n",palette_filename);
		return -1;
		}

	unsigned short color = 0;
	int red = 0 , green = 0 , blue = 0;

	int b;
	for(b = 0; b < colors_used; b++)
		{
		Palette_GetColor(b, &red, &green, &blue);

		color = red & 31; //Red
		color |= (green & 31) << 5; //Green
		color |= (blue & 31) << 10; //Blue
		//Alpha not used in palettes

		if(fwrite(&color,sizeof(color),1,OUTPUT_FILE) != 1)
			{
			fclose(OUTPUT_FILE);
			printf("Write error!!\n\n");
			return -1;
			}
		}

	fclose(OUTPUT_FILE);

	printf("Created file: %s\n\n", palette_filename);

	//Palette created, now, lets make the texture
	printf("Creating texture...\n\n");

	OUTPUT_FILE = fopen(texture_filename,"wb+");
	
	if(!OUTPUT_FILE)
		{
		printf("Couldn't open %s in write mode!!\n\n",texture_filename);
		return -1;
		}

	unsigned char index_ = 0;
	unsigned char alpha_ = 0;

	int c;
	for(c = 0; c < size; c+=4)
		{
		index_ = Palette_GetIndex((data_pointer[c]>>3) & 31,(data_pointer[c+1]>>3) & 31,(data_pointer[c+2]>>3) & 31);
		
		alpha_ = (data_pointer[c+3] >> 3) & 0x1F;
		
		unsigned char save_to_file = (alpha_ << 3) | (index_ & 0x7);

		if(fwrite(&save_to_file,sizeof(save_to_file),1,OUTPUT_FILE) != 1)
			{
			fclose(OUTPUT_FILE);
			printf("Write error!!\n\n");
			return -1;
			}
		}

	fclose(OUTPUT_FILE);

	printf("Created file: %s\n\n", texture_filename);

	return 1;
}


