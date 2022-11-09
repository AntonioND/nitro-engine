
// NightFox LIB - Funciones de Fondos con tiles
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
#include "nf_2d.h"
#include "nf_tiledbg.h"



// Define los bancos de Mapas y Tiles
u8 NF_BANKS_TILES[2];	// (1 banks = 16kb)	Cada banco de tiles puede alvergar 8 bancos de Mapas
u8 NF_BANKS_MAPS[2];	// (1 bank = 2kb)	Usar multiplos de 8. Cada set de 8 bancos consume 1 banco de tiles

// Define los Buffers para almacenar los fondos
char* NF_BUFFER_BGTILES[NF_SLOTS_TBG];
char* NF_BUFFER_BGMAP[NF_SLOTS_TBG];
char* NF_BUFFER_BGPAL[NF_SLOTS_TBG];

// Define la estructura de propiedades de los fondos
NF_TYPE_TBG_INFO NF_TILEDBG[NF_SLOTS_TBG];			// Info de los fondos cargados en RAM
NF_TYPE_TBGLAYERS_INFO NF_TILEDBG_LAYERS[2][4];		// Info de los fondos en pantalla

// Define la estructura para las paletas extendidas
NF_TYPE_EXBGPAL_INFO NF_EXBGPAL[NF_SLOTS_EXBGPAL];	// Datos de las paletas extendidas

// Define el array de bloques libres
u8 NF_TILEBLOCKS[2][NF_MAX_BANKS_TILES];
u8 NF_MAPBLOCKS[2][NF_MAX_BANKS_MAPS];



// Funcion NF_InitTiledBgBuffers();
void NF_InitTiledBgBuffers(void) {
	u8 n = 0;
	// Buffers de fondos tileados
	for (n = 0; n < NF_SLOTS_TBG; n ++) {	// Inicializa todos los slots
		NF_BUFFER_BGTILES[n] = NULL;		// Buffer para los tiles
		NF_BUFFER_BGMAP[n] = NULL;			// Buffer para el map
		NF_BUFFER_BGPAL[n] = NULL;			// Buffer para la paleta
		sprintf(NF_TILEDBG[n].name, "xxxNONAMExxx");	// Nombre del Tileset
		NF_TILEDBG[n].tilesize = 0;			// Tamaño del Tileset
		NF_TILEDBG[n].mapsize = 0;			// Tamaño del Mapa
		NF_TILEDBG[n].palsize = 0;			// Tamaño de la Paleta
		NF_TILEDBG[n].width = 0;			// Ancho del Mapa
		NF_TILEDBG[n].height = 0;			// Alto del Mapa
		NF_TILEDBG[n].available = true;		// Disponibilidad
	}
	// Buffers de paletas extendidas
	for (n = 0; n < NF_SLOTS_EXBGPAL; n ++) {
		NF_EXBGPAL[n].buffer = NULL;
		NF_EXBGPAL[n].palsize = 0;
		NF_EXBGPAL[n].inuse = false;
	}

}



// Funcion NF_ResetTiledBgBuffers();
void NF_ResetTiledBgBuffers(void) {
	u8 n = 0;
	for (n = 0; n < NF_SLOTS_TBG; n ++) {	// Inicializa todos los slots
		free(NF_BUFFER_BGTILES[n]);			// Vacia el Buffer para los tiles
		free(NF_BUFFER_BGMAP[n]);			// Vacia Buffer para el map
		free(NF_BUFFER_BGPAL[n]);			// Vacia Buffer para la paleta			
	}
	for (n = 0; n < NF_SLOTS_EXBGPAL; n ++) {
		NF_EXBGPAL[n].buffer = NULL;
	}
	NF_InitTiledBgBuffers();				// Reinicia el resto de variables
}



// Funcion NF_InitTiledBgSys();
void NF_InitTiledBgSys(u8 screen) {

	// Variables
	u8 n = 0;

	// Define el numero de bancos de Mapas y Tiles
	NF_BANKS_TILES[screen] = 8;		// (1 banks = 16kb)	Cada banco de tiles puede alvergar 8 bancos de Mapas
	NF_BANKS_MAPS[screen] = 16;		// (1 bank = 2kb)	Usar multiplos de 8. Cada set de 8 bancos consume 1 banco de tiles
	// Por defecto Tiles = 8, Mapas = 16
	// Esto nos deja 6 bancos de 16kb para tiles
	// y 16 bancos de 2kb para mapas

	// Inicializa el array de bloques libres de Tiles
	for (n = 0; n < NF_BANKS_TILES[screen]; n ++) {
		NF_TILEBLOCKS[screen][n] = 0;
	}

	// Inicializa el array de bloques libres de Mapas
	for (n = 0; n < NF_BANKS_MAPS[screen]; n ++) {
		NF_MAPBLOCKS[screen][n] = 0;
	}

	// Inicializa el array de informacion de fondos en pantalla
	for (n = 0; n < 4; n ++) {
		NF_TILEDBG_LAYERS[screen][n].tilebase = 0;		// Base del Tileset
		NF_TILEDBG_LAYERS[screen][n].tileblocks = 0;	// Bloques usados por el Tileset
		NF_TILEDBG_LAYERS[screen][n].mapbase = 0;		// Base del Map
		NF_TILEDBG_LAYERS[screen][n].mapblocks = 0;		// Bloques usados por el Map
		NF_TILEDBG_LAYERS[screen][n].bgwidth = 0;		// Ancho del fondo
		NF_TILEDBG_LAYERS[screen][n].bgheight = 0;		// Altura del fondo
		NF_TILEDBG_LAYERS[screen][n].mapwidth = 0;		// Ancho del mapa
		NF_TILEDBG_LAYERS[screen][n].mapheight = 0;		// Altura del mapa
		NF_TILEDBG_LAYERS[screen][n].bgtype = 0;		// Tipo de mapa
		NF_TILEDBG_LAYERS[screen][n].bgslot = 0;		// Buffer de graficos usado
		NF_TILEDBG_LAYERS[screen][n].blockx = 0;		// Bloque de mapa actual (horizontal)
		NF_TILEDBG_LAYERS[screen][n].blocky = 0;		// Bloque de mapa actual (vertical)
		NF_TILEDBG_LAYERS[screen][n].created = false;	// Esta creado ?
	}

	// Ahora reserva los bancos necesarios de VRAM para mapas
	// Cada bloque de 16kb (1 banco de tiles) permite 8 bancos de mapas (de 2kb cada uno)
	u8 r_banks;
	r_banks = ((NF_BANKS_MAPS[screen] - 1) >> 3) + 1;		// Calcula los bancos de Tiles a reservar para Maps
	for (n = 0; n < r_banks; n ++) {
		NF_TILEBLOCKS[screen][n] = 128;				// Marca que bancos de VRAM son para MAPS
	}

	if (screen == 0) {
		// Si es la pantalla 0 (Superior, Main engine)
		REG_DISPCNT |= (DISPLAY_BG_EXT_PALETTE);	// Activa las paletas extendidas
		vramSetBankA(VRAM_A_MAIN_BG);				// Banco A de la VRAM para fondos (128kb)
		memset((void*)0x06000000, 0, 131072);		// Borra el contenido del banco A
		vramSetBankE(VRAM_E_LCD);					// Reserva el banco E de la VRAM para Paletas Extendidas (0-3) (32kb de 64kb)
		memset((void*)0x06880000, 0, 32768);		// Borra el contenido del banco E
		for (n = 0; n < 4; n ++) {					// Oculta todas las 4 capas
			NF_HideBg(0, n);
		}
	} else {
		// Si es la pantalla 1 (Inferior, Sub engine)
		REG_DISPCNT_SUB |= (DISPLAY_BG_EXT_PALETTE);	// Activa las paletas extendidas
		vramSetBankC(VRAM_C_SUB_BG);					// Banco C de la VRAM para fondos (128kb)
		memset((void*)0x06200000, 0, 131072);			// Borra el contenido del banco C
		vramSetBankH(VRAM_H_LCD);						// Reserva el banco H de la VRAM para Paletas Extendidas (0-3) (32kb)
		memset((void*)0x06898000, 0, 32768);			// Borra el contenido del banco H
		for (n = 0; n < 4; n ++) {						// Oculta todas las 4 capas
			NF_HideBg(1, n);
		}
	}

}



// Funcion NF_LoadTiledBg();
void NF_LoadTiledBg(const char* file, const char* name, u16 width, u16 height) {

	// Variable temporal del tamaño de la paleta
	u32 pal_size = 0;

	// Busca un slot libre
	u8 n = 0;
	u8 slot = 255;
	for (n = 0; n < NF_SLOTS_TBG; n ++) {		// Busca en todos los slots
		if (NF_TILEDBG[n].available) {			// Si esta libre
			NF_TILEDBG[n].available = false;	// Marcalo como en uso
			slot = n;							// Guarda el slot a usar
			n = NF_SLOTS_TBG;					// Deja de buscar
		}
	}
	// Si no hay ningun slot libre, error
	if (slot == 255) {
		NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);
	}

	// Verifica que el fondo sea multiplo de 256px (32 tiles)
	if (((width % 256) != 0) || ((height % 256) != 0)) {
		NF_Error(115, file, 0);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_BGMAP[slot]);		// Buffer para los mapas
	NF_BUFFER_BGMAP[slot] = NULL;
	free(NF_BUFFER_BGTILES[slot]);		// Buffer para los tiles
	NF_BUFFER_BGTILES[slot] = NULL;
	free(NF_BUFFER_BGPAL[slot]);		// Buffer para los paletas
	NF_BUFFER_BGPAL[slot] = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .IMG
	sprintf(filename, "%s/%s.img", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_TILEDBG[slot].tilesize = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_BUFFER_BGTILES[slot] = (char*) calloc (NF_TILEDBG[slot].tilesize, sizeof(char));
		if (NF_BUFFER_BGTILES[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Carga el archivo .MAP
	sprintf(filename, "%s/%s.map", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_TILEDBG[slot].mapsize = ((((ftell(file_id) - 1) >> 11) + 1) << 11);	// Ajusta el tamaño a bloques de 2kb
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_BUFFER_BGMAP[slot] = (char*) calloc (NF_TILEDBG[slot].mapsize, sizeof(char));
		if (NF_BUFFER_BGMAP[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGMAP[slot], 1, NF_TILEDBG[slot].mapsize, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Carga el archivo .PAL
	sprintf(filename, "%s/%s.pal", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		pal_size = ftell(file_id);
		NF_TILEDBG[slot].palsize = pal_size;
		rewind(file_id);
		// Si el tamaño es inferior a 512 bytes, ajustalo
		if (NF_TILEDBG[slot].palsize < 512) NF_TILEDBG[slot].palsize = 512;
		// Reserva el espacio en RAM
		NF_BUFFER_BGPAL[slot] = (char*) calloc (NF_TILEDBG[slot].palsize, sizeof(char));
		if (NF_BUFFER_BGPAL[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].palsize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGPAL[slot], 1, pal_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda el nombre del Fondo
	sprintf(NF_TILEDBG[slot].name, "%s", name);

	// Y las medidas
	NF_TILEDBG[slot].width = width;
	NF_TILEDBG[slot].height = height;

}



// Funcion NF_LoadTilesForBg();
void NF_LoadTilesForBg(const char* file, const char* name, u16 width, u16 height, u16 tile_start, u16 tile_end) {

	// Variable temporal del tamaño de los datos
	u32 tile_size = 0;
	u32 pal_size = 0;

	// Busca un slot libre
	u8 n = 0;
	u8 slot = 255;
	for (n = 0; n < NF_SLOTS_TBG; n ++) {		// Busca en todos los slots
		if (NF_TILEDBG[n].available) {			// Si esta libre
			NF_TILEDBG[n].available = false;	// Marcalo como en uso
			slot = n;							// Guarda el slot a usar
			n = NF_SLOTS_TBG;					// Deja de buscar
		}
	}
	// Si no hay ningun slot libre, error
	if (slot == 255) {
		NF_Error(103, "Tiled Bg", NF_SLOTS_TBG);
	}

	// Verifica que el fondo sea multiplo de 256px (32 tiles)
	if (((width % 256) != 0) || ((height % 256) != 0)) {
		NF_Error(115, file, 0);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_BGMAP[slot]);		// Buffer para los mapas
	NF_BUFFER_BGMAP[slot] = NULL;
	free(NF_BUFFER_BGTILES[slot]);		// Buffer para los tiles
	NF_BUFFER_BGTILES[slot] = NULL;
	free(NF_BUFFER_BGPAL[slot]);		// Buffer para los paletas
	NF_BUFFER_BGPAL[slot] = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .IMG
	sprintf(filename, "%s/%s.img", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		tile_size = ftell(file_id);
		rewind(file_id);
		// Calcula el tamaño del tilesize a cargar
		NF_TILEDBG[slot].tilesize = (((tile_end - tile_start) + 1) << 6);	// nº de tiles x 64 bytes
		// Si el tamaño del Tilesed solicitado excede del tamaño de archivo, error
		if (((tile_end + 1) << 6) > tile_size) {
			NF_Error(106, "Tilenumber", (tile_size >> 6));
		}
		// Reserva el espacio en RAM
		NF_BUFFER_BGTILES[slot] = (char*) calloc (NF_TILEDBG[slot].tilesize, sizeof(char));
		if (NF_BUFFER_BGTILES[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);
		}
		// Posicionate en la posicion donde esta el primer tile a cargar
		fseek(file_id, (tile_start << 6), SEEK_SET);
		// ahora lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo


	// Crea un archivo .MAP vacio en RAM 
	// ((ancho / 8) * (alto / 8)) * 2
	NF_TILEDBG[slot].mapsize = (((width >> 3) * (height >> 3)) << 1);
	// Reserva el espacio en RAM
	NF_BUFFER_BGMAP[slot] = (char*) calloc (NF_TILEDBG[slot].mapsize, sizeof(char));
	if (NF_BUFFER_BGMAP[slot] == NULL) {		// Si no hay suficiente RAM libre
		NF_Error(102, NULL, NF_TILEDBG[slot].mapsize);
	}
	// Y ponlo a 0
	memset(NF_BUFFER_BGMAP[slot], 0, NF_TILEDBG[slot].mapsize);


	// Carga el archivo .PAL
	sprintf(filename, "%s/%s.pal", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		pal_size = ftell(file_id);
		NF_TILEDBG[slot].palsize = pal_size;
		rewind(file_id);
		// Si el tamaño es inferior a 512 bytes, ajustalo
		if (NF_TILEDBG[slot].palsize < 512) NF_TILEDBG[slot].palsize = 512;
		// Reserva el espacio en RAM
		NF_BUFFER_BGPAL[slot] = (char*) calloc (NF_TILEDBG[slot].palsize, sizeof(char));
		if (NF_BUFFER_BGPAL[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].palsize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGPAL[slot], 1, pal_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda el nombre del Fondo
	sprintf(NF_TILEDBG[slot].name, "%s", name);

	// Y las medidas
	NF_TILEDBG[slot].width = width;
	NF_TILEDBG[slot].height = height;

}



// Funcion NF_UnloadTiledBg();
void NF_UnloadTiledBg(const char* name) {

	// Busca el fondo solicitado
	u16 n = 0;			// Bucle
	u8 slot = 255;		// Slot seleccionado
	char bg[32];		// Nombre

	// Busca el fondo solicitado
	sprintf(bg, "%s", name);							// Obten el nombre del fondo a buscar
	for (n = 0; n < NF_SLOTS_TBG; n ++) {				// Busca en todos los slots
		if (strcmp(bg, NF_TILEDBG[n].name) == 0) {		// Si lo encuentras
			slot = n;									// Guarda el slot a usar
			n = NF_SLOTS_TBG;							// Deja de buscar
		}
	}
	// Si no se encuentra, error
	if (slot == 255) {
		NF_Error(104, name, 0);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_BGMAP[slot]);		// Buffer para los mapas
	NF_BUFFER_BGMAP[slot] = NULL;
	free(NF_BUFFER_BGTILES[slot]);		// Buffer para los tiles
	NF_BUFFER_BGTILES[slot] = NULL;
	free(NF_BUFFER_BGPAL[slot]);		// Buffer para los paletas
	NF_BUFFER_BGPAL[slot] = NULL;

	// Resetea las variables para ese fondo
	sprintf(NF_TILEDBG[slot].name, "xxxNONAMExxx");	// Nombre del Tileset
	NF_TILEDBG[slot].tilesize = 0;					// Tamaño del Tileset
	NF_TILEDBG[slot].mapsize = 0;					// Tamaño del Mapa
	NF_TILEDBG[slot].palsize = 0;					// Tamaño de la Paleta
	NF_TILEDBG[slot].width = 0;						// Ancho del Mapa
	NF_TILEDBG[slot].height = 0;					// Alto del Mapa
	NF_TILEDBG[slot].available = true;				// Disponibilidad

}



// Funcion NF_CreateTiledBg();
void NF_CreateTiledBg(u8 screen, u8 layer, const char* name) {

	// Variables
	u8 n = 0;			// Bucle
	u8 slot = 255;		// Slot seleccionado
	char bg[32];		// Nombre

	// Busca el fondo solicitado
	sprintf(bg, "%s", name);							// Obten el nombre del fondo a buscar
	for (n = 0; n < NF_SLOTS_TBG; n ++) {				// Busca en todos los slots
		if (strcmp(bg, NF_TILEDBG[n].name) == 0) {		// Si lo encuentras
			slot = n;									// Guarda el slot a usar
			n = NF_SLOTS_TBG;							// Deja de buscar
		}
	}
	// Si no se encuentra, error
	if (slot == 255) {
		NF_Error(104, name, 0);
	}

	// Si ya hay un fondo existente en esta pantalla y capa, borralo antes
	if (NF_TILEDBG_LAYERS[screen][layer].created) {
		NF_DeleteTiledBg(screen, layer);
	}

	// Variables de control de Tiles
	u8 counter = 0;
	u8 start = 255;
	u8 tilesblocks = 0;
	u8 basetiles = 0;

	// Transfiere el tamaño del fondo
	NF_TILEDBG_LAYERS[screen][layer].bgwidth = NF_TILEDBG[slot].width;		// Ancho del fondo
	NF_TILEDBG_LAYERS[screen][layer].bgheight = NF_TILEDBG[slot].height;	// Altura del fondo
	NF_TILEDBG_LAYERS[screen][layer].mapwidth = NF_TILEDBG[slot].width;		// Ancho del mapa
	NF_TILEDBG_LAYERS[screen][layer].mapheight = NF_TILEDBG[slot].height;	// Altura del mapa
	NF_TILEDBG_LAYERS[screen][layer].bgtype = 0;							// Tipo de fondo
	NF_TILEDBG_LAYERS[screen][layer].bgslot = slot;							// Buffer de graficos usado

	// Calcula el tipo y tamaño del mapa si es Infinito ( >512 )
	// ( >512 x n )
	if ((NF_TILEDBG[slot].width >= 512) && (NF_TILEDBG[slot].height <= 256)) {
		NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
		NF_TILEDBG_LAYERS[screen][layer].mapheight = NF_TILEDBG[slot].height;
		NF_TILEDBG_LAYERS[screen][layer].bgtype = 1;
	}
	// ( n x >512 )
	if ((NF_TILEDBG[slot].width <= 256) && (NF_TILEDBG[slot].height >= 512)) {
		NF_TILEDBG_LAYERS[screen][layer].mapwidth = NF_TILEDBG[slot].width;
		NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
		NF_TILEDBG_LAYERS[screen][layer].bgtype = 2;
	}
	// ( >512 x >512 )
	if ((NF_TILEDBG[slot].width >= 512) && (NF_TILEDBG[slot].height >= 512)) {
		NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
		NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
		NF_TILEDBG_LAYERS[screen][layer].bgtype = 3;
	}

	// Busca un los bloques libres para almacenar los Tiles en VRAM
	tilesblocks = ((NF_TILEDBG[slot].tilesize - 1) >> 14) + 1;	// Bloques necesarios para el Tileset

	for (n = 0; n < NF_BANKS_TILES[screen]; n ++) {
		if (NF_TILEBLOCKS[screen][n] == 0) {		// Si esta libre
			if (counter == 0) {						// Y el contador esta a 0
				start = n;							// Marca la posicion de inicio
			}
			counter ++;								
			if (counter == tilesblocks) {			// Si ya tienes suficientes bloques libres
				n = NF_BANKS_TILES[screen];			// Termina de buscar
			}
		} else {									// Si el bloque no esta libre
			start = 255;							// Borra el marcador
			counter = 0;							// Y resetea el contador
		}
	}

	// Si no se han encontrado bloques libres
	if ((start == 255) || (counter < tilesblocks)) {
		NF_Error(107, name, tilesblocks);
	} else {	
		basetiles = start;		// Guarda donde empiezan los bloques libres
	}

	// Marca los bancos de Tiles usados por este fondo
	for (n = basetiles; n < (basetiles + tilesblocks); n ++) {
		NF_TILEBLOCKS[screen][n] = 255;	// Marca los bloques usados por tiles
	}

	// Variables de control de Maps
	u8 mapblocks = 0;
	u8 basemap = 0;
	counter = 0;
	start = 255;
	u16 mapsize = 0;

	// Busca un los bloques libres para almacenar los Mapas en VRAM
	if (NF_TILEDBG_LAYERS[screen][layer].bgtype == 0) {
		// Si el mapa es normal =<512
		mapblocks = ((NF_TILEDBG[slot].mapsize - 1) >> 11) + 1;
	} else {
		// Si el mapa es infinito >512
		mapsize = (((NF_TILEDBG_LAYERS[screen][layer].mapwidth >> 3) * (NF_TILEDBG_LAYERS[screen][layer].mapheight >> 3)) << 1);
		mapblocks = ((mapsize - 1) >> 11) + 1;
	}

	for (n = 0; n < NF_BANKS_MAPS[screen]; n ++) {
		if (NF_MAPBLOCKS[screen][n] == 0) {			// Si esta libre
			if (counter == 0) {						// Y el contador esta a 0
				start = n;							// Marca la posicion de inicio
			}
			counter ++;								
			if (counter == mapblocks) {				// Si ya tienes suficientes bloques libres
				n = NF_BANKS_MAPS[screen];					// Termina de buscar
			}
		} else {									// Si el bloque no esta libre
			start = 255;							// Borra el marcador
			counter = 0;							// Y resetea el contador
		}
	}

	// Si no se han encontrado bloques libres
	if ((start == 255) || (counter < mapblocks)) {
		NF_Error(108, name, mapblocks);
	} else {	
		basemap = start;							// Guarda donde empiezan los bloques libres
	}

	// Marca los bancos de Mapa usados por este fondo
	for (n = basemap; n < (basemap + mapblocks); n ++) {
		NF_MAPBLOCKS[screen][n] = 255;	// Marca los bloques usados por mapas
	}


	// Obten el tamaño del fondo
	u32 bg_size = 0;
	// 256x256
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256)) {
		bg_size = BG_32x32;
	}
	// 512x256
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256)) {
		bg_size = BG_64x32;
	}
	// 256x512
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512)) {
		bg_size = BG_32x64;
	}
	// 512x512
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512)) {
		bg_size = BG_64x64;
	}


	// Crea el fondo segun la pantalla, capa y demas caracteristicas dadas
	// REG_BG0CNT	<- Carracteristicas del fondo
	// REG_BG0HOFS	<- Posicion X
	// REG_BG0VOFS	<- Posicion Y
	if (screen == 0) {
		switch (layer) {
			case 0:
				REG_BG0CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_0 | BG_PALETTE_SLOT0 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 1:
				REG_BG1CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_1 | BG_PALETTE_SLOT1 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 2:
				REG_BG2CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_2 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 3:
				REG_BG3CNT = BgType_Text8bpp | bg_size | BG_PRIORITY_3 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
		} 
	} else {
		switch (layer) {
			case 0:
				REG_BG0CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_0 | BG_PALETTE_SLOT0 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 1:
				REG_BG1CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_1 | BG_PALETTE_SLOT1 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 2:
				REG_BG2CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_2 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
			case 3:
				REG_BG3CNT_SUB = BgType_Text8bpp | bg_size | BG_PRIORITY_3 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap);
				break;
		} 
	}

	u32 address;		// Variable de direccion de VRAM;

	// Transfiere el Tileset a VRAM
	if (screen == 0) {	// (VRAM_A)
		address = (0x6000000) + (basetiles << 14);
	} else {			// (VRAM_C)
		address = (0x6200000) + (basetiles << 14);
	}
	NF_DmaMemCopy((void*)address, NF_BUFFER_BGTILES[slot], NF_TILEDBG[slot].tilesize);


	// Transfiere el Mapa a VRAM
	if (screen == 0) {	// (VRAM_A)
		address = (0x6000000) + (basemap << 11);
	} else {			// (VRAM_C)
		address = (0x6200000) + (basemap << 11);
	}

	if (NF_TILEDBG_LAYERS[screen][layer].bgtype == 0) {

		// Si el mapa es normal
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[slot], NF_TILEDBG[slot].mapsize);

	} else {

		// Segun el tipo de mapa infinito
		switch (NF_TILEDBG_LAYERS[screen][layer].bgtype) {

			case 1:	// >512x256
				// Bloque A y B (32x32) + (32x32) (2kb x 2 = 4kb)
				NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[slot], 4096);
				break;

			case 2:	// 256x>512
				// Bloque A (32x64) (2kb x 2 = 4kb)
				NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[slot], 4096);
				break;

			case 3: // >512x>512
				// Bloque A y B (32x32) + (32x32) (2kb x 2 = 4kb)
				NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[slot], 4096);
				// Bloque (+4096) C y D (32x32) + (32x32) (2kb x 2 = 4kb)
				u16 nextrow = 0;	// Desplazamiento para cargar la fila inferior
				nextrow = ((((NF_TILEDBG_LAYERS[screen][layer].bgwidth - 1) >> 8) + 1) << 11);
				NF_DmaMemCopy((void*)(address + 4096), (NF_BUFFER_BGMAP[slot] + nextrow), 4096);
				break;

		}

	}


	// Tranfiere la Paleta a VRAM
	if (screen == 0) {

		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	} else {	// Paletas de la pantalla 1 (VRAM_H)

		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

	}

	// Registra los datos del fondos en pantalla
	NF_TILEDBG_LAYERS[screen][layer].tilebase = basetiles;				// Base del Tileset
	NF_TILEDBG_LAYERS[screen][layer].tileblocks = tilesblocks;			// Bloques usados por el Tileset
	NF_TILEDBG_LAYERS[screen][layer].mapbase = basemap;					// Base del Map
	NF_TILEDBG_LAYERS[screen][layer].mapblocks = mapblocks;				// Bloques usados por el Map
	NF_TILEDBG_LAYERS[screen][layer].created = true;					// Esta creado ?

	// Posiciona el fondo en las coordenadas 0,0
	NF_ScrollBg(screen, layer, 0, 0);

	// Haz visible el fondo creado
	NF_ShowBg(screen, layer);

}



// Funcion NF_DeleteTiledBg();
void NF_DeleteTiledBg(u8 screen, u8 layer) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Esconde el fondo creado
	NF_HideBg(screen, layer);

	// Variables de uso general
	u32 address;				// Direccion de VRAM;
	u8 n;					// Uso general
	u16 basetiles = 0;		// Base del Tileset
	u16 basemap = 0;		// Base del Map
	u16 tilesize = 0;		// Tamaño del Tileset
	u16 mapsize = 0;		// Tamaño del Map

	// Borra el Tileset de la VRAM
	basetiles = NF_TILEDBG_LAYERS[screen][layer].tilebase;
	tilesize = (NF_TILEDBG_LAYERS[screen][layer].tileblocks << 14);
	if (screen == 0) {	// (VRAM_A)
		address = (0x6000000) + (basetiles << 14);
	} else {			// (VRAM_C)
		address = (0x6200000) + (basetiles << 14);
	}
	memset((void*)address, 0, tilesize);		// Pon a 0 todos los bytes de la area de VRAM

	// Borra el Mapa de la VRAM
	basemap = NF_TILEDBG_LAYERS[screen][layer].mapbase;
	mapsize = (NF_TILEDBG_LAYERS[screen][layer].mapblocks << 11);
	if (screen == 0) {	// (VRAM_A)
		address = (0x6000000) + (basemap << 11);
	} else {			// (VRAM_C)
		address = (0x6200000) + (basemap << 11);
	}
	memset((void*)address, 0, mapsize);		// Pon a 0 todos los bytes de la area de VRAM
	

	// Borra la Paleta a VRAM
	if (screen == 0) {

		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13);
		memset((void*)address, 0, 8192);
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	} else {	// Paletas de la pantalla 1 (VRAM_H)

		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13);
		memset((void*)address, 0, 8192);
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

	}


	// Marca como libres los bancos de Tiles usados por este fondo
	tilesize = (basetiles + NF_TILEDBG_LAYERS[screen][layer].tileblocks);
	for (n = basetiles; n < tilesize; n ++) {
		NF_TILEBLOCKS[screen][n] = 0;
	}

	// Marca como libres los bancos de Mapa usados por este fondo
	mapsize = (basemap + NF_TILEDBG_LAYERS[screen][layer].mapblocks);
	for (n = basemap; n < mapsize; n ++) {
		NF_MAPBLOCKS[screen][n] = 0;
	}

	// Borra los datos del fondos en pantalla
	NF_TILEDBG_LAYERS[screen][layer].tilebase = 0;		// Base del Tileset
	NF_TILEDBG_LAYERS[screen][layer].tileblocks = 0;	// Bloques usados por el Tileset
	NF_TILEDBG_LAYERS[screen][layer].mapbase = 0;		// Base del Map
	NF_TILEDBG_LAYERS[screen][layer].mapblocks = 0;		// Bloques usados por el Map
	NF_TILEDBG_LAYERS[screen][layer].bgwidth = 0;		// Ancho del fondo
	NF_TILEDBG_LAYERS[screen][layer].bgheight = 0;		// Altura del fondo
	NF_TILEDBG_LAYERS[screen][layer].mapwidth = 0;		// Ancho del mapa
	NF_TILEDBG_LAYERS[screen][layer].mapheight = 0;		// Altura del mapa
	NF_TILEDBG_LAYERS[screen][layer].bgtype = 0;		// Tipo de mapa
	NF_TILEDBG_LAYERS[screen][layer].bgslot = 0;		// Buffer de graficos usado
	NF_TILEDBG_LAYERS[screen][layer].blockx = 0;		// Bloque de mapa actual (horizontal)
	NF_TILEDBG_LAYERS[screen][layer].blocky = 0;		// Bloque de mapa actual (vertical)
	NF_TILEDBG_LAYERS[screen][layer].created = false;	// Esta creado ?

}



// Funcion NF_GetTileMapAddress();
u32 NF_GetTileMapAddress(u8 screen, u8 layer, u16 tile_x, u16 tile_y) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Obtiene las medidas en tiles del mapa
	u16 size_x = (NF_TILEDBG_LAYERS[screen][layer].bgwidth >> 3);	// Tiles de ancho
	u16 size_y = (NF_TILEDBG_LAYERS[screen][layer].bgheight >> 3);	// Tiles de alto
	u16 block_x = (tile_x >> 5);			// nº de pantalla (X)
	u16 block_y = (tile_y >> 5);			// nº de pantalla (Y)
	u32 row_size = ((size_x >> 5) << 11);	// Tamaño en bytes de una fila de pantallas

	// Protegete de los fuera de rango
	if (tile_x > size_x) NF_Error(106, "Tile X", size_x);
	if (tile_y > size_y) NF_Error(106, "Tile Y", size_y);

	// Calcula la posicion de memoria que deberas leer
	// El mapa esta ordenado en bloques de 32x32 tiles, en filas.
	u32 scr_y = (block_y * row_size);	// Desplazamiento en memoria, bloques de pantallas (32x32) sobre Y
	u32 scr_x = (block_x << 11);		// Desplazamiento en memoria, bloques de pantallas (32x32) sobre X
	u32 tls_y = ((tile_y - (block_y << 5)) << 5);	// Desplazamiento en memoria, tiles sobre X
	u32 tls_x = (tile_x - (block_x << 5));			// Desplazamiento en memoria, tiles sobre Y
	u32 address =  scr_y + scr_x + ((tls_y + tls_x) << 1); 

	// Devuelve el la direccion en el buffer del Tile
	return address;

}



// Funcion NF_GetTileOfMap();
u16 NF_GetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Obten los bytes
	u8 lobyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + address);
	u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

	// Devuelve el valor del tile
	return ((hibyte << 8) | lobyte);

}



// Funcion NF_SetTileOfMap();
void NF_SetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u16 tile) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Calcula los valores para el HI-Byte y el LO-Byte
	u8 hibyte = ((tile >> 8) & 0xff);
	u8 lobyte = (tile & 0xff);

	// Graba los bytes
	*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + address) = lobyte;
	*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;

}



// Funcion NF_UpdateVramMap();
void NF_UpdateVramMap(u8 screen, u8 layer) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Variables
	u32 address = 0;		// Direccion de destino
	u32 mov_x = 0;		// Desplazamiento X
	u32 mov_y = 0;		// Desplazamiento Y
	u32 rowsize = 0;	// Tamaño de la fila de pantallas

	// Calcula la direccion base del mapa
	if (screen == 0) {	// (VRAM_A)
		address = (0x6000000) + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);
	} else {			// (VRAM_C)
		address = (0x6200000) + (NF_TILEDBG_LAYERS[screen][layer].mapbase << 11);
	}

	// Segun el tipo de mapa
	switch (NF_TILEDBG_LAYERS[screen][layer].bgtype) {

		case 0:	// 256x256  -  Bloque A (32x32) (2kb)
			// Copias el Bloque A (32x32) (2kb)
			NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot], 2048);
			break;

		case 1:	// 512x256  -  Bloque A y B (32x32) + (32x32) (2kb x 2 = 4kb)
			// Calcula el desplazamiento de datos
			mov_x = (NF_TILEDBG_LAYERS[screen][layer].blockx << 11);
			// Copias los Bloques A y B (32x32) + (32x32) (2kb x 2 = 4kb)
			NF_DmaMemCopy((void*)address, (NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mov_x), 4096);
			break;


		case 2:	// 256x512  -  Bloque A (32x64) (2kb x 2 = 4kb)
			// Calcula el desplazamiento de datos
			mov_y = (NF_TILEDBG_LAYERS[screen][layer].blocky << 11);
			// Copias los Bloques A y B (32x32) + (32x32) (2kb x 2 = 4kb)
			NF_DmaMemCopy((void*)address, (NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mov_y), 4096);
			break;

		case 3: // >512x>512
			// Calcula el Rowsize
			rowsize = (((((NF_TILEDBG_LAYERS[screen][layer].bgwidth - 1) >> 8)) + 1) << 11);
			// Y el desplazamiento de datos
			mov_x = (NF_TILEDBG_LAYERS[screen][layer].blocky * rowsize) + (NF_TILEDBG_LAYERS[screen][layer].blockx << 11);
			mov_y = mov_x + rowsize;
			// Bloque A y B (32x32) + (32x32) (2kb x 2 = 4kb)
			NF_DmaMemCopy((void*)address, (NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mov_x), 4096);
			// Bloque (+4096) C y D (32x32) + (32x32) (2kb x 2 = 4kb)
			NF_DmaMemCopy((void*)(address + 4096), (NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + mov_y), 4096);
			break;

	}

}




// Funcion NF_BgSetPalColor();
void NF_BgSetPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));
	// Direccion en VRAM
	u32 address = 0;

	// Modifica la paleta
	if (screen == 0) {

		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13) + (number << 1);
		*((u16*)address) = rgb;
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	} else {	// Paletas de la pantalla 1 (VRAM_H)

		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13)  + (number << 1);
		*((u16*)address) = rgb;
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

	}

}



// Funcion NF_BgEditPalColor();
void NF_BgEditPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));

	// Calcula los valores para el HI-Byte y el LO-Byte
	u8 hibyte = ((rgb >> 8) & 0xff);
	u8 lobyte = (rgb & 0xff);

	// Graba los bytes
	*(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (number << 1)) = lobyte;
	*(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + ((number << 1) + 1)) = hibyte;

}



// Funcion 	NF_BgUpdatePalette();
void NF_BgUpdatePalette(u8 screen, u8 layer) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Direccion en VRAM
	u32 address = 0;

	// Obten el slot donde esta la paleta en RAM
	u8 slot = NF_TILEDBG_LAYERS[screen][layer].bgslot;

	// Tranfiere la Paleta a VRAM
	if (screen == 0) {

		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	} else {	// Paletas de la pantalla 1 (VRAM_H)

		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

	}

}



// Funcion NF_BgGetPalColor();
void NF_BgGetPalColor(u8 screen, u8 layer, u8 number, u8* r, u8* g, u8* b) {

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Obten los bytes
	u8 lobyte = *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (number << 1));
	u8 hibyte = *(NF_BUFFER_BGPAL[NF_TILEDBG_LAYERS[screen][layer].bgslot] + ((number << 1) + 1));

	// Calcula el RGB (compuesto)
	u16 rgb = ((hibyte << 8) | lobyte);

	// Calcula los RGB
	*r = (rgb & 0x1F);
	*g = ((rgb >> 5) & 0x1F);
	*b = ((rgb >> 10) & 0x1F);

}



// Funcion NF_GetTilePal();
u8 NF_GetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Obten los bytes
	u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

	// Devuelve el nº de la paleta (4 ultimos bits del HI-Byte)
	return ((hibyte >> 4) & 0x0F);

}



// Funcion NF_SetTilePal();
void NF_SetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u8 pal) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Obten el valor actual del HI-Byte
	u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

	// Y determina el valor del resto de datos del byte, excluyendo la paleta
	u8 data = (hibyte & 0x0F);

	// Graba los bytes
	*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = ((pal << 4) | data);

}



// Funcion NF_LoadExBgPal();
void NF_LoadExBgPal(const char* file, u8 slot) {

	// Verifica el rango de Id's de Gfx
	if (slot > (NF_SLOTS_EXBGPAL - 1)) {
		NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));
	}

	// Variable temporal del tamaño de la paleta
	u32 pal_size = 0;

	// Vacia el buffer
	free(NF_EXBGPAL[slot].buffer);
	NF_EXBGPAL[slot].buffer = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .PAL
	sprintf(filename, "%s/%s.pal", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		pal_size = ftell(file_id);
		NF_EXBGPAL[slot].palsize = pal_size;
		rewind(file_id);
		// Si el tamaño es inferior a 512 bytes, ajustalo
		if (NF_EXBGPAL[slot].palsize < 512) NF_EXBGPAL[slot].palsize = 512;
		// Reserva el espacio en RAM
		NF_EXBGPAL[slot].buffer = (char*) calloc (NF_EXBGPAL[slot].palsize, sizeof(char));
		if (NF_EXBGPAL[slot].buffer == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_EXBGPAL[slot].palsize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_EXBGPAL[slot].buffer, 1, pal_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda el estado del slot
	NF_EXBGPAL[slot].inuse = true;

}



// Funcion NF_UnloadExBgPal();
void NF_UnloadExBgPal(u8 slot) {

	// Verifica el rango de Id's de Gfx
	if (slot > (NF_SLOTS_EXBGPAL - 1)) {
		NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));
	}

	// Verifica si la Id esta cargada
	if (!NF_EXBGPAL[slot].inuse) {
		NF_Error(110, "ExBgPal", slot);
	}

	// Vacia el buffer
	free(NF_EXBGPAL[slot].buffer);
	NF_EXBGPAL[slot].buffer = NULL;

	// Tamaño de la paleta a 0
	NF_EXBGPAL[slot].palsize = 0;

	// Guarda el estado del slot
	NF_EXBGPAL[slot].inuse = false;

}



// Funcion NF_VramExBgPal();
void NF_VramExBgPal(u8 screen, u8 layer, u8 id, u8 slot) {

	// Verifica el rango de Id's de Gfx
	if (slot > (NF_SLOTS_EXBGPAL - 1)) {
		NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));
	}

	// Verifica si la Id esta cargada
	if (!NF_EXBGPAL[slot].inuse) {
		NF_Error(110, "ExBgPal", slot);
	}

	// Tranfiere la Paleta a VRAM
	u32 address = 0;
	if (screen == 0) {
		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13) + (slot << 9);
		NF_DmaMemCopy((void*)address, NF_EXBGPAL[id].buffer, NF_EXBGPAL[id].palsize);
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);
	} else {	// Paletas de la pantalla 1 (VRAM_H)
		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13) + (slot << 9);
		NF_DmaMemCopy((void*)address, NF_EXBGPAL[id].buffer, NF_EXBGPAL[id].palsize);
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
	}

}



// Funcion NF_SetExBgPal();
void NF_SetExBgPal(u8 screen, u8 layer, u8 pal) {

	// Verifica el rango de Id's de Gfx
	if (pal > 15) {
		NF_Error(106, "ExBgPal", (NF_SLOTS_EXBGPAL - 1));
	}

	// Verifica que el fondo esta creado
	if (!NF_TILEDBG_LAYERS[screen][layer].created) {
		char text[32];
		sprintf(text, "%d", screen);
		NF_Error(105, text, layer);		// Si no existe, error
	}

	// Tamaño del buffer
	u32 mapsize = NF_TILEDBG[NF_TILEDBG_LAYERS[screen][layer].bgslot].mapsize;
	// Variables comunes
	u32 pos = 0;
	u16 hibyte = 0;
	u8 data = 0;

	for (pos = 0; pos < mapsize; pos += 2) {
		// Obten el valor actual del HI-Byte
		hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (pos + 1));
		// Y determina el valor del resto de datos del byte, excluyendo la paleta
		data = (hibyte & 0x0F);
		// Graba los bytes
		*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (pos + 1)) = ((pal << 4) | data);
	}

	// Actualiza el mapa en la VRAM
	NF_UpdateVramMap(screen, layer);

}



// Funcion NF_SetTileHflip();
void NF_SetTileHflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Obten el valor actual del HI-Byte
	u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

	// Invierte el BIT correspondiente a FLIP horizontal del tile (BIT 3)
	hibyte ^= 0x04;

	// Graba el valor actualizado
	*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;

}



// Funcion NF_SetTileVflip();
void NF_SetTileVflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y) {

	// Obten la direccion en el buffer del Tile
	u32 address = NF_GetTileMapAddress(screen, layer, tile_x, tile_y);

	// Obten el valor actual del HI-Byte
	u8 hibyte = *(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1));

	// Invierte el BIT correspondiente a FLIP vertical del tile (BIT 4)
	hibyte ^= 0x08;

	// Graba el valor actualizado
	*(NF_BUFFER_BGMAP[NF_TILEDBG_LAYERS[screen][layer].bgslot] + (address + 1)) = hibyte;

}





// Funcion NF_RotateTileGfx();
void NF_RotateTileGfx(u8 slot, u16 tile, u8 rotation) {

	// Buffers temporales
	char* character_a;
	character_a = NULL;
	character_a = (char*) calloc (64, sizeof(char));
	if (character_a == NULL) {		// Si no hay suficiente RAM libre
		NF_Error(102, NULL, 64);
	}
	char* character_b;
	character_b = NULL;
	character_b = (char*) calloc (64, sizeof(char));
	if (character_b == NULL) {		// Si no hay suficiente RAM libre
		NF_Error(102, NULL, 64);
	}

	// Variables locales
	s16 xa = 0;
	s16 xb = 0;
	s16 pos_a = 0;
	s16 ya = 0;
	s16 yb = 0;
	s16 pos_b = 0;
	
	// Copia el tile al buffer temporal A
	memcpy(character_a, (NF_BUFFER_BGTILES[slot] + (tile << 6)), 64);

	switch (rotation) {

		case 1:		// 90º a la derecha
			xb = 7;
			yb = 0;
			for (ya = 0; ya < 8; ya ++) {
				for (xa = 0; xa < 8; xa ++) {
					pos_a = ((ya << 3) + xa);
					pos_b = ((yb << 3) + xb);
					character_b[pos_b] = character_a[pos_a];
					yb ++;
					if (yb > 7) {
						yb = 0;
						xb --;
					}
				}
			}
			break;

		case 2:		// 90º a la izquierda
			xb = 0;
			yb = 7;
			for (ya = 0; ya < 8; ya ++) {
				for (xa = 0; xa < 8; xa ++) {
					pos_a = ((ya << 3) + xa);
					pos_b = ((yb << 3) + xb);
					character_b[pos_b] = character_a[pos_a];
					yb --;
					if (yb < 0) {
						yb = 7;
						xb ++;
					}
				}
			}
			break;

		case 3:		// 180 º
			xb = 7;
			yb = 7;
			for (ya = 0; ya < 8; ya ++) {
				for (xa = 0; xa < 8; xa ++) {
					pos_a = ((ya << 3) + xa);
					pos_b = ((yb << 3) + xb);
					character_b[pos_b] = character_a[pos_a];
					xb --;
					if (xb < 0) {
						xb = 0;
						yb --;
					}
				}
			}
			break;

		default:
			break;

	}
	
	// Copia el tile desde buffer temporal B
	memcpy((NF_BUFFER_BGTILES[slot] + (tile << 6)), character_b, 64);

	// Libera los buffers temporales
	free (character_a);
	free (character_b);

}
