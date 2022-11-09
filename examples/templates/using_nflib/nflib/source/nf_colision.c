
// NightFox LIB - Funciones de Colisiones
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
#include "nf_basic.h"
#include "nf_colision.h"




// Define los buffers y estructuras de control de los mapas de colision
NF_TYPE_CMAP_INFO NF_CMAP[NF_SLOTS_CMAP];



// Funcion NF_InitCmapBuffers();
void NF_InitCmapBuffers(void) {
	u8 n = 0;
	for (n = 0; n < NF_SLOTS_CMAP; n ++) {
		NF_CMAP[n].tiles = NULL;		// Inicializa los punteros de los buffers
		NF_CMAP[n].map = NULL;
		NF_CMAP[n].tiles_size = 0;		// Tamaño de archivo
		NF_CMAP[n].map_size = 0;
		NF_CMAP[n].width = 0;			// Ancho del mapa
		NF_CMAP[n].height = 0;			// Alto del mapa
		NF_CMAP[n].inuse = false;		// Esta en uso el slot?
	}
}



// Funcion NF_ResetCmapBuffers();
void NF_ResetCmapBuffers(void) {
	u8 n = 0;
	for (n = 0; n < NF_SLOTS_CMAP; n ++) {
		free(NF_CMAP[n].tiles);		// Vacia los buffers
		free(NF_CMAP[n].map);
	}
	NF_InitCmapBuffers();			// Y reinicia todas las variables
}



// Funcion NF_LoadColisionMap();
void NF_LoadColisionMap(const char* file, u8 id, u16 width, u16 height) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_CMAP)) {
		NF_Error(106, "Colision Map", NF_SLOTS_CMAP);
	}

	// Verifica si la Id esta libre
	if (NF_CMAP[id].inuse) {
		NF_Error(109, "Colision Map", id);
	}

	// Vacia los buffers que se usaran
	free(NF_CMAP[id].map);
	NF_CMAP[id].map = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];
	
	// Carga el archivo .CMP
	sprintf(filename, "%s/%s.cmp", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_CMAP[id].map_size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_CMAP[id].map = (char*) calloc (NF_CMAP[id].map_size, sizeof(char));
		if (NF_CMAP[id].map == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_CMAP[id].map_size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_CMAP[id].map, 1, NF_CMAP[id].map_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda las medidas
	NF_CMAP[id].width = width;
	NF_CMAP[id].height = height;

	// Y marca esta ID como usada
	NF_CMAP[id].inuse = true;

}



// Funcion NF_UnloadColisionMap();
void NF_UnloadColisionMap(u8 id) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_CMAP)) {
		NF_Error(106, "Colision Map", NF_SLOTS_CMAP);
	}

	// Verifica si la Id esta libre
	if (!NF_CMAP[id].inuse) {
		NF_Error(110, "Colision Map", id);
	}

	// Vacia los buffers que se usaran
	free(NF_CMAP[id].map);
	NF_CMAP[id].map = NULL;

	// Y marca esta ID como usada
	NF_CMAP[id].inuse = false;

}



// Funcion NF_GetTile();
u16 NF_GetTile(u8 slot, s32 x, s32 y) {

	// Si la coordenada esta fuera de rango, devuelve 0
	if (
		(x < 0)
		||
		(y < 0)
		||
		(x >= NF_CMAP[slot].width)
		||
		(y >= NF_CMAP[slot].height)
		) {
			// Devuelve 0
			return 0;

	} else {	// Si la coordenada esta dentro del rango...

		// Calcula el ancho en tiles del mapa
		u16 columns = (NF_CMAP[slot].width >> 3);		// (width / 8);

		// Calcula los tiles de posicion	(x / 8); (y / 8);
		u16 tile_x = (x >> 3);
		u16 tile_y = (y >> 3) + 1;		// +1, por que la primera fila se reserva para la referencia de tiles

		// Calcula el nº de tile
		u32 address = (((tile_y * columns) + tile_x) << 1);

		// Obten los bytes
		u8 lobyte = *(NF_CMAP[slot].map + address);
		u8 hibyte = *(NF_CMAP[slot].map + (address + 1));

		// Devuelve el valor del tile
		return ((hibyte << 8) | lobyte);

	}

}



// Funcion NF_SetTile();
void NF_SetTile(u8 slot, s32 x, s32 y, u16 value) {

	// Si la coordenada esta dentro del rango...
	if (
		(x >= 0)
		&&
		(y >= 0)
		&&
		(x < NF_CMAP[slot].width)
		&&
		(y < NF_CMAP[slot].height)
		) {

		// Calcula el ancho en tiles del mapa
		u16 columns = (NF_CMAP[slot].width >> 3);		// (width / 8);

		// Calcula los tiles de posicion	(x / 8); (y / 8);
		u16 tile_x = (x >> 3);
		u16 tile_y = (y >> 3) + 1;		// +1, por que la primera fila se reserva para la referencia de tiles

		// Calcula el nº de tile
		u32 address = (((tile_y * columns) + tile_x) << 1);
		// nº de tile x2, dado que el mapa es de 16 bits (2 bytes por dato) y el buffer
		// es de 8 bits, se lee el 2do byte, por eso se multiplica por 2.

		// Calcula los valores de los bytes
		u8 hibyte = ((value >> 8) & 0xff);		// HI Byte
		u8 lobyte = (value & 0xff);				// LO Byte

		// Escribe el nuevo valor en el mapa de colisiones
		*(NF_CMAP[slot].map + address) = lobyte;
		*(NF_CMAP[slot].map + (address + 1)) = hibyte;

	}

}



// Funcion NF_LoadColisionBg();
void NF_LoadColisionBg(const char* file, u8 id, u16 width, u16 height) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_CMAP)) {
		NF_Error(106, "Colision Map", NF_SLOTS_CMAP);
	}

	// Verifica si la Id esta libre
	if (NF_CMAP[id].inuse) {
		NF_Error(109, "Colision Map", id);
	}

	// Vacia los buffers que se usaran
	free(NF_CMAP[id].tiles);
	NF_CMAP[id].tiles = NULL;
	free(NF_CMAP[id].map);
	NF_CMAP[id].map = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];
	
	// Carga el archivo .DAT (TILES)
	sprintf(filename, "%s/%s.dat", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_CMAP[id].tiles_size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_CMAP[id].tiles = (char*) calloc (NF_CMAP[id].tiles_size, sizeof(char));
		if (NF_CMAP[id].tiles == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_CMAP[id].tiles_size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_CMAP[id].tiles, 1, NF_CMAP[id].tiles_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Carga el archivo .CMP
	sprintf(filename, "%s/%s.cmp", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_CMAP[id].map_size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_CMAP[id].map = (char*) calloc (NF_CMAP[id].map_size, sizeof(char));
		if (NF_CMAP[id].map == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_CMAP[id].map_size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_CMAP[id].map, 1, NF_CMAP[id].map_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda las medidas
	NF_CMAP[id].width = width;
	NF_CMAP[id].height = height;

	// Y marca esta ID como usada
	NF_CMAP[id].inuse = true;

}



// Funcion NF_UnloadColisionBg();
void NF_UnloadColisionBg(u8 id) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_CMAP)) {
		NF_Error(106, "Colision Map", NF_SLOTS_CMAP);
	}

	// Verifica si la Id esta libre
	if (!NF_CMAP[id].inuse) {
		NF_Error(110, "Colision Map", id);
	}

	// Vacia los buffers que se usaran
	free(NF_CMAP[id].tiles);
	NF_CMAP[id].tiles = NULL;
	free(NF_CMAP[id].map);
	NF_CMAP[id].map = NULL;

	// Y marca esta ID como usada
	NF_CMAP[id].inuse = false;

}



// Funcion NF_GetPoint();
u8 NF_GetPoint(u8 slot, s32 x, s32 y) {

	// Si la coordenada esta fuera de rango, devuelve 0
	if (
		(x < 0)
		||
		(y < 0)
		||
		(x >= NF_CMAP[slot].width)
		||
		(y >= NF_CMAP[slot].height)
		) {
			// Devuelve 0
			return 0;

	} else {	// Si la coordenada esta dentro del rango...

		// Calcula el ancho en tiles del mapa
		u16 columns = (NF_CMAP[slot].width >> 3);		// (width / 8);

		// Calcula los tiles de posicion	(x / 8); (y / 8);
		u16 tile_x = (x >> 3);
		u16 tile_y = (y >> 3) + 1;			// +1, por que la primera fila se reserva para la referencia de tiles

		// Calcula los pixeles relativos
		u16 pixel_x = x - (tile_x << 3);
		u16 pixel_y = (y + 8) - (tile_y << 3); 

		// Calcula la posicion de tile dentro del archivo de mapa
		s32 address = (((tile_y * columns) + tile_x) << 1);
		u8 lobyte = *(NF_CMAP[slot].map + address);
		u8 hibyte = *(NF_CMAP[slot].map + (address + 1));
		u16 tile = ((hibyte << 8) | lobyte);

		// Obten el valor del pixel leyendola del archivo de tiles
		address = ((tile << 6) + (pixel_y << 3) + pixel_x);	// (tile * 64) + (y * 8) + x
		lobyte = *(NF_CMAP[slot].tiles + address);

		// Devuelve el valor del pixel
		return lobyte;

	}

}
