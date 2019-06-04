// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define NITRO_TEXTURE_CONVERTER_VERSION "1.0.0"

#define FORMAT_TYPES   7

const char * FORMAT_STRINGS[FORMAT_TYPES] = {
	"A1RGB5", "RGB256", "RGB16", "RGB4", "A3RGB32", "A5RGB8", "DEPTHBMP" };

void PrintUsage(void)
{
	printf("Usage:\n");
	printf("   Nitro_Texture_Converter [input].png [format]\n\n");

	printf("Output files:\n");
	printf("   [input]_tex.bin - Texture\n");
	printf("   [input]_pal.bin - Palette (if any)\n\n");

	printf("Format list: (Palete format is always RGB5)\n");
	printf("   A1RGB5   - 1 Alpha, 5 Red Green Blue\n");
	printf("   RGB256   - 8 Palette (256 colors)\n");
	printf("   RGB16    - 4 Palette (16 colors)\n");
	printf("   RGB4     - 2 Palette (4 colors)\n");
	printf("   A3RGB32  - 3 Alpha, 5 Palette (32 colors\n");
	printf("   A5RGB8   - 5 Alpha, 3 Palette (8 colors\n");
	printf("   DEPTHBMP - 1 Fog enable, 15 Depth (For clear BMP)\n\n\n");
}

int GetFormat(char * string)
{
	int a;
	for(a = 0; a < FORMAT_TYPES; a++)
		{
		if(strcmp(string, FORMAT_STRINGS[a]) == 0)
			return a;
		}

	printf("Format %s unsupported!\n\n");
	return -1;
}



void * LoadPNGtoARGB(char * filename, int * buffer_size);

int ConvertARGBintoA1RGB5(void * data, int size, char * texture_filename);
int ConvertARGBintoRGB256(void * data, int size, char * texture_filename, char * palette_filename);
int ConvertARGBintoRGB16(void * data, int size, char * texture_filename, char * palette_filename);
int ConvertARGBintoRGB4(void * data, int size, char * texture_filename, char * palette_filename);
int ConvertARGBintoA3RGB32(void * data, int size, char * texture_filename, char * palette_filename);
int ConvertARGBintoA5RGB8(void * data, int size, char * texture_filename, char * palette_filename);
int ConvertARGBintoDEPTHBMP(void * data, int size, char * texture_filename);


int main(int argc, char* argv[])
{
	printf("\n\n");
	printf("   +---------------------------------------+\n");
	printf("   |                                       |\n");
	printf("   |   Nitro Texture Converter - V " NITRO_TEXTURE_CONVERTER_VERSION "   |\n");
	printf("   |                                       |\n");
	printf("   |     (C) 2008-2009 Antonio Ni%co D%caz   |\n",164, 161);
	printf("   |                                       |\n");
	printf("   +---------------------------------------+\n\n");

	printf("Visit http://antoniond.drunkencoders.com to get the latest version.\n\n");
	
	if(argc != 3) 
		{
		PrintUsage();
		return 1;
		}
	
	int OUTPUT_FORMAT = GetFormat(argv[2]);
	if(OUTPUT_FORMAT == -1)
		{
		PrintUsage();
		return 1;
		}

	char * test_extension_png = argv[1] + strlen(argv[1]) - strlen(".png");
	if(strcmp(test_extension_png,".png")!= 0)
		{
		printf("Input file must have the name like this: [input].png\n\n");
		PrintUsage();
		return -1;
		}

	printf("Converting %s into format %s...\n\n",argv[1],argv[2]);

	int ARGB_BUFFER_SIZE = 0; //Convert it into ARGB raw data
	void * ARGB_BUFFER = LoadPNGtoARGB(argv[1],&ARGB_BUFFER_SIZE);
	if(ARGB_BUFFER == NULL) return -1;
    
	//Get output filenames
	int __strlen = strlen(argv[1])-strlen(".png");
	char OUTPUT_BASE_FILENAME[FILENAME_MAX]; 
	strncpy(OUTPUT_BASE_FILENAME,argv[1],__strlen);
	OUTPUT_BASE_FILENAME[__strlen] = '\0';

	char OUTPUT_TEXTURE_FILENAME[FILENAME_MAX];
	sprintf(OUTPUT_TEXTURE_FILENAME,"%s_tex.bin",OUTPUT_BASE_FILENAME);

	char OUTPUT_PALETTE_FILENAME[FILENAME_MAX];
	sprintf(OUTPUT_PALETTE_FILENAME,"%s_pal.bin",OUTPUT_BASE_FILENAME);

	//Convert raw data into the format requested

	int returned_value = 0;

	switch(OUTPUT_FORMAT)
	{
		case 0: // A1RGB5
			returned_value = ConvertARGBintoA1RGB5(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME);
			break;
		case 1: // RGB256
			returned_value = ConvertARGBintoRGB256(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME, 
					OUTPUT_PALETTE_FILENAME);
			break;
		case 2: // RGB16
			returned_value = ConvertARGBintoRGB16(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME, 
					OUTPUT_PALETTE_FILENAME);
			break;
		case 3: // RGB4
			returned_value = ConvertARGBintoRGB4(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME, 
					OUTPUT_PALETTE_FILENAME);
			break;
		case 4: // A3RGB32
			returned_value = ConvertARGBintoA3RGB32(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME, 
					OUTPUT_PALETTE_FILENAME);
			break;
		case 5: // A5RGB8
			returned_value = ConvertARGBintoA5RGB8(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME, 
					OUTPUT_PALETTE_FILENAME);
			break;
		case 6: // DEPTHBMP
			returned_value = ConvertARGBintoDEPTHBMP(ARGB_BUFFER,ARGB_BUFFER_SIZE, OUTPUT_TEXTURE_FILENAME);
			break;
		default:
			//Program should never get here!
			printf("Please, select a valid format!\n\n");
			break;
	}
	
	free(ARGB_BUFFER);
	
	if(returned_value < 1) return -2;

	printf("Done!!\n\n");

	return 0;
}

