
// NightFox LIB - Include de funciones de carga de archivos multimedia
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413



// Includes devKitPro
#include <nds.h>
#include <filesystem.h>
#include <fat.h>

// Includes C
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Includes propios
#include "nf_media.h"
#include "nf_bitmapbg.h"
#include "nf_basic.h"





// Funcion NF_LoadBMP();
void NF_LoadBMP(const char* file, u8 slot) {

	// Buffers locales
	char* buffer;		// Buffer temporal
	buffer = NULL;
	char* palette;		// Paleta (requerida por algun modo)
	palette = NULL;

	// Magic ID
	char magic_id[4];
	memset(magic_id, 0, 4);

	// Define la estructura para almacenar la cabecera del BMP
	typedef struct {
		u32 bmp_size;		// Tamaño en bytes del BMP
		u16 res_a;			// Reservado
		u16 res_b;			// Reservado
		u32 offset;			// Offset donde empiezan los datos
		u32 header_size;	// Tamaño de la cabecera (40 bytes)
		u32 bmp_width;		// Ancho de la imagen en pixeles
		u32 bmp_height;		// Altura de la imagen en pixeles
		u16 color_planes;	// Numero de planos de color
		u16 bpp;			// Numero de bits por pixel
		u32 compression;	// Compresion usada
		u32 raw_size;		// Tamaño de los datos en RAW despues de la cabecera
		u32 dpi_hor;		// Puntos por pulgada (horizontal)
		u32 dpi_ver;		// Puntos por pulgada (vertical)
		u32 pal_colors;		// Numero de colores en la paleta
		u32 imp_colors;		// Colores importantes
	} bmp_header_info;
	bmp_header_info bmp_header;

	// Pon todos los bytes de la estructura a 0
	memset(&bmp_header, 0, sizeof(bmp_header));

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .BMP
	sprintf(filename, "%s/%s.bmp", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");

	if (file_id) {	// Si el archivo existe...
		// Posicionate en el byte 0
		fseek(file_id, 0, SEEK_SET);
		// Lee el Magic String del archivo BMP (2 primeros Bytes, "BM") / (0x00 - 0x01)
		fread(magic_id, 1, 2, file_id);
		// Si es un archivo BMP... (Magic string == "BM")
		if (strcmp(magic_id, "BM") == 0) {
			// Posicionate en el byte 2
			fseek(file_id, 2, SEEK_SET);
			// Lee la cabecera del archivo BMP (0x02 - 0x36)
			fread((void*)&bmp_header, 1, sizeof(bmp_header), file_id);
			/////////////////////////////////////////////////////////////
			// Es un archivo BMP valido, cargalo en un buffer temporal //
			/////////////////////////////////////////////////////////////
			// Crea un buffer temporal
			buffer = (char*) calloc (bmp_header.raw_size, sizeof(char));
			if (buffer == NULL) NF_Error(102, NULL, bmp_header.raw_size);
			// Si se ha creado con exito, carga el archivo al buffer
			fseek(file_id, bmp_header.offset, SEEK_SET);
			fread(buffer, 1, bmp_header.raw_size, file_id);
		} else {
			// No es un archivo BMP valido
			NF_Error(101, "BMP", 0);
		}
	} else {
		// El archivo no existe
		NF_Error(101, filename, 0);
	}
	// Cierra el archivo
	fclose(file_id);

	// Variables que se usaran a partir de aqui
	u16 pixel = 0;		// Color del pixel
	u16 x = 0;			// Coordemada X
	u16 y = 0;			// Coordenada Y
	u32 idx = 0;		// Indice en el buffer temporal
	u32 offset = 0;		// Indice en el buffer de destino
	u8 r = 0;			// Valores RGB
	u8 g = 0;
	u8 b = 0;
	u16 colors = 0;		// En 8 bits, numero de colores
	u32 size = 0;		// Tamaño del buffer de 16 bits (en bytes)

	// Habilita el buffer de destino (u16 de alto x ancho del tamaño de imagen)
	size = ((bmp_header.bmp_width * bmp_header.bmp_height) << 1);
	free(NF_BG16B[slot].buffer);
	NF_BG16B[slot].buffer = NULL;
	NF_BG16B[slot].buffer = (u16*) calloc ((size >> 1), sizeof(u16));
	if (NF_BG16B[slot].buffer == NULL) NF_Error(102, NULL, size);

	// Segun los BITS por Pixel (8, 16, 24)
	switch (bmp_header.bpp) {

		case 8:		// 8 bits por pixel
			// Calcula el tamaño de la paleta
			colors = ((bmp_header.offset - 0x36) >> 2);
			palette = (char*) calloc ((colors << 2), sizeof(char));
			if (palette == NULL) NF_Error(102, NULL, (colors << 2));
			// Abre de nuevo el archivo y carga la paleta
			file_id = fopen(filename, "rb");
			if (!file_id) NF_Error(101, filename, 0);
			fseek(file_id, 0x36, SEEK_SET);
			fread(palette, 1, (colors << 2), file_id);
			fclose(file_id);
			// Convierte el archivo a 16 bits
			for (y = 0; y < bmp_header.bmp_height; y ++) {
				for (x = 0; x < bmp_header.bmp_width; x ++) {
					// Calcula los offsets
					offset = ((((bmp_header.bmp_height - 1) - y) * bmp_header.bmp_width) + x);
					// Obten el pixel
					pixel = (buffer[idx] << 2);
					// Desglosa el RGB para rotar los colores BGR -> RGB
					b = (palette[pixel] >> 3);
					g = (palette[(pixel + 1)] >> 3);
					r = (palette[(pixel + 2)] >> 3);
					// Recombina el RGB a 16 bits
					pixel = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
					NF_BG16B[slot].buffer[offset] = pixel;
					// Siguiente pixel
					idx ++;
				}
				// Ajusta la alineacion a 4 bytes al final de linea
				while ((idx % 4) != 0) idx ++;
			}
			// Elimina la paleta de la RAM
			free(palette);
			palette = NULL;
			break;

		case 16:	// 16 bits por pixel
			for (y = 0; y < bmp_header.bmp_height; y ++) {
				for (x = 0; x < bmp_header.bmp_width; x ++) {
					// Calcula los offsets
					offset = ((((bmp_header.bmp_height - 1) - y) * bmp_header.bmp_width) + x);
					// Obten el pixel
					pixel = buffer[idx] + (buffer[(idx + 1)] << 8);
					// Desglosa el RGB para rotar los colores BGR -> RGB
					b = (pixel & 0x1F);
					g = ((pixel >> 5) & 0x1F);
					r = ((pixel >> 10) & 0x1F);
					// Recombina el RGB a 16 bits
					pixel = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
					NF_BG16B[slot].buffer[offset] = pixel;
					// Siguiente pixel
					idx += 2;
				}
				// Ajusta la alineacion a 4 bytes al final de linea
				while ((idx % 4) != 0) idx ++;
			}
			break;

		case 24:	// 24 bits por pixel
			for (y = 0; y < bmp_header.bmp_height; y ++) {
				for (x = 0; x < bmp_header.bmp_width; x ++) {
					// Calcula los offsets
					offset = ((((bmp_header.bmp_height - 1) - y) * bmp_header.bmp_width) + x);
					// Obten el pixel
					b = ((buffer[idx]) >> 3);
					g = ((buffer[idx + 1]) >> 3);
					r = ((buffer[idx + 2]) >> 3);
					// Recombina el RGB a 16 bits
					pixel = ((r)|((g) << 5)|((b) << 10)|(BIT(15)));
					NF_BG16B[slot].buffer[offset] = pixel;
					// Siguiente pixel
					idx += 3;
				}
				// Ajusta la alineacion a 4 bytes al final de linea
				while ((idx % 4) != 0) idx ++;
			}
			break;

	}

	// Guarda los parametros del fondo
	NF_BG16B[slot].size = size;			// Guarda el tamaño
	NF_BG16B[slot].width = bmp_header.bmp_width;		// Ancho del fondo
	NF_BG16B[slot].height = bmp_header.bmp_height;		// Altura del fondo
	NF_BG16B[slot].inuse = true;						// Marca que esta en uso

	// Libera el buffer temporal
	free(buffer);
	buffer = NULL;

}
