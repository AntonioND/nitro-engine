// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdio.h>
#include "palette.h"

int ConvertARGBintoRGB256(void * data, int size, char * texture_filename, char * palette_filename)
{
	printf("RGB256: -If image alpha == 0 -> Color index = 0.\n");
	printf("        -If image alpha != 0 -> Color index = actual color.\n");
	printf("         (Palette color 0 can be transparent.)\n");
	printf("        -If image has more than 256 colors the program will try\n");
	printf("         to optimize the palette (not recommended).\n\n");
	
	unsigned char * data_pointer = (unsigned char*) data;

	int a;
	//Check if transparent...
	bool transparent_image = false;
	a = 0;
	while(a < size)
		{
		if(data_pointer[a+3] == 0) 
			{
			transparent_image = true;
			break;
			}
		a+=4;
		}

	printf("Creating palette...\n\n");

	int colors_used;
	int optimization = 0;

	while(1) //Try to find a palette
		{
		Palette_New(256,optimization);

		if(transparent_image) Palette_NewColor(255,255,255); //Dummy color for transparence

		for(a = 0; a < size; a+=4)
			{
			if(data_pointer[a+3] > 0) //If alpha > 0
				{
				//Alpha not used in palettes
				Palette_NewColor((data_pointer[a]>>3) & 31,(data_pointer[a+1]>>3) & 31,(data_pointer[a+2]>>3) & 31);
				}
			}
	
		colors_used = Palette_GetColorsUsed();

		if(colors_used == 1)
			{
			printf("Image alpha channel is always 0!!\n\n");
			return -1;
			}

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

	int c;
	for(c = 0; c < size; c+=4)
		{
		if(data_pointer[c+3] > 0)
			index_ = Palette_GetIndex((data_pointer[c]>>3) & 31,(data_pointer[c+1]>>3) & 31,(data_pointer[c+2]>>3) & 31);
		else index_ = 0; // 0 = transparent

		if(fwrite(&index_,sizeof(index_),1,OUTPUT_FILE) != 1)
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



