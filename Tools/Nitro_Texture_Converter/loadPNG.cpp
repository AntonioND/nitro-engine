
#include "stdafx.h"
#include <png.h>
#include <stdio.h>
#include <malloc.h>

#define PNG_BYTES_TO_CHECK    4
#define MAX_WIDTH_HEIGHT   1024

void * LoadPNGtoARGB(char * filename, int * buffer_size)
{
	printf("Loading file %s...\n\n",filename);

	FILE * PNG_file = fopen(filename,"rb");
	if(PNG_file == NULL)
		{
		printf("File %s could't be opened!\n\n",filename);
		return NULL;
		}
	
	fseek(PNG_file,0,SEEK_END);
	printf("File size is %d bytes.\n\n",ftell(PNG_file));
	rewind(PNG_file);
	
	int buffer[PNG_BYTES_TO_CHECK];
	
	fread (buffer,1,PNG_BYTES_TO_CHECK,PNG_file);
	
	if(png_sig_cmp((png_bytep)buffer,0,PNG_BYTES_TO_CHECK))
		{
		fclose(PNG_file);
		printf("File not valid - not a PNG!\n\n");
		return NULL;
		}

	printf("File succesfully loaded!\n\n");

	printf("Reading file...\n\n");

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
		{
		fclose(PNG_file);
		printf("Couldn't create png structs...\n\n");
		return NULL;
		}

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
		{
		fclose(PNG_file);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        printf("Couldn't create png structs...\n\n");
		return NULL;
		}

	if (setjmp(png_jmpbuf(png_ptr)))
		{
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(PNG_file);
		return NULL;
		}

	png_init_io(png_ptr, PNG_file);
	png_set_sig_bytes(png_ptr, PNG_BYTES_TO_CHECK);
	
	printf("Loading image data...\n\n");

	png_read_info(png_ptr, info_ptr);

	png_uint_32 PNG_width = png_get_image_width(png_ptr, info_ptr);
	png_uint_32 PNG_height = png_get_image_height(png_ptr, info_ptr);
	png_uint_32 PNG_depth = png_get_bit_depth(png_ptr, info_ptr);
	int PNG_color_type = png_get_color_type(png_ptr, info_ptr);

	printf("PNG information:\n");
	printf("   Width: %d\n",PNG_width);
	printf("   Height: %d\n",PNG_height);
	printf("   Bit depth: %d\n",PNG_depth);

	char * color_type_string = NULL;

	if(PNG_color_type == PNG_COLOR_TYPE_GRAY)
		color_type_string = "PNG_COLOR_TYPE_GRAY";
	else if(PNG_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		color_type_string = "PNG_COLOR_TYPE_GRAY_ALPHA";
	else if(PNG_color_type == PNG_COLOR_TYPE_PALETTE)             
       	color_type_string = "PNG_COLOR_TYPE_PALETTE";
	else if(PNG_color_type == PNG_COLOR_TYPE_RGB)            
		color_type_string = "PNG_COLOR_TYPE_RGB";
	else if(PNG_color_type == PNG_COLOR_TYPE_RGB_ALPHA)   
		color_type_string = "PNG_COLOR_TYPE_RGB_ALPHA";
	else color_type_string = " - ";

	printf("   Color type: %s\n\n",color_type_string);
	
	if( (PNG_width != 8 && PNG_width != 16 && PNG_width != 32 && PNG_width != 64 && PNG_width != 128
		 && PNG_width != 256 && PNG_width != 512 && PNG_width != 1024) &&
		  (PNG_height != 8 && PNG_height != 16 && PNG_height != 32 && PNG_height != 64 && PNG_height != 128
		 && PNG_height != 256 && PNG_height != 512 && PNG_height != 1024)  )
		{
		printf("Width or height is not a power of 2!!\n\n");
		}

	printf("Converting PNG into RGBA 8 bits...\n\n");

	if(PNG_color_type == PNG_COLOR_TYPE_PALETTE)
		{
        png_set_palette_to_rgb(png_ptr); //Convert to RGB
		}
	else if (PNG_color_type == PNG_COLOR_TYPE_GRAY)
		{
		if( PNG_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr); //Expand to 8 bits

		png_set_gray_to_rgb(png_ptr);  //Convert to RGB
		}
	else if (PNG_color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		{
		if( PNG_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
		png_set_gray_to_rgb(png_ptr); //Convert to RGB
		}
	else if(PNG_color_type == PNG_COLOR_TYPE_RGB)            
		{
		if (PNG_depth < 8) png_set_packing(png_ptr);//Is this possible?
		}
	else if(PNG_color_type == PNG_COLOR_TYPE_RGB_ALPHA)   
		{
		if (PNG_depth < 8) png_set_packing(png_ptr);//Is this possible?
		}
	else
		{
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(PNG_file);
		printf("Not valid color type!!");
		return NULL;
		}

	if (PNG_depth == 16)
        png_set_strip_16(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	PNG_depth = png_get_bit_depth(png_ptr, info_ptr);
	PNG_color_type = png_get_color_type(png_ptr, info_ptr);

	if(PNG_color_type==PNG_COLOR_TYPE_RGB)
		{
		if (png_get_valid(png_ptr, info_ptr,PNG_INFO_tRNS))  //Add alpha
			png_set_tRNS_to_alpha(png_ptr);
		else png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

		png_read_update_info(png_ptr, info_ptr);

		PNG_depth = png_get_bit_depth(png_ptr, info_ptr);
		PNG_color_type = png_get_color_type(png_ptr, info_ptr);
		printf("Created an alpha channel.\n\n");
		}

		//Uhmmmm... Done! Let's check...

	if(!(PNG_depth == 8 && PNG_color_type==PNG_COLOR_TYPE_RGB_ALPHA))
		{
		printf("PNG was not succesfully converted!!\n\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(PNG_file);
		return NULL;
		}

	printf("Done! Reading image...\n\n");
	
	png_bytep row_pointers[MAX_WIDTH_HEIGHT];

	unsigned int a;
    for (a = 0; a < PNG_height; a++)
		{
		row_pointers[a] = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));
		}

	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, NULL);

	printf("Image succesfully read!\n\n");
	
	int * IMAGE_BUFFER = (int*)malloc(PNG_height * PNG_width * 4); //R + G + B + A
	*buffer_size = PNG_height * PNG_width * 4;

	if(!IMAGE_BUFFER)
		{
		printf("Couldn't allocate image data for convertion!!\n\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
		fclose(PNG_file);
		return NULL;
		}
	
	unsigned int x, y;
	for(y = 0; y < PNG_height; y++) for(x = 0; x < PNG_width; x++)
		{
		unsigned char blue = row_pointers[y][(x*4)+0];
		unsigned char green = row_pointers[y][(x*4)+1];
		unsigned char red = row_pointers[y][(x*4)+2];
		unsigned char alpha = row_pointers[y][(x*4)+3];

		IMAGE_BUFFER[(y*PNG_width)+x] = (((unsigned int)alpha) << 24) |
			(((unsigned int)red) << 16)| (((unsigned int)green) << 8) |
			((unsigned int)blue);
		}

	//For some strange reason, if I uncomment this the program crashes...
	//This means that some memory isn't deallocated...
	png_destroy_read_struct(/*&png_ptr*/ NULL, &info_ptr, png_infopp_NULL);

    fclose(PNG_file);

	printf("PNG freed. Image allocated in buffer. Let's convert it!\n\n");

	return IMAGE_BUFFER; //Everything goes right...
}