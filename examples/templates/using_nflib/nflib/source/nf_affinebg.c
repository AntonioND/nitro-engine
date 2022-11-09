
// NightFox LIB - Funciones de Fondos Affine
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
#include "nf_affinebg.h"





// Estructura para almacenar los parametros de los fondos Affine
NF_TYPE_AFFINE_BG NF_AFFINE_BG[2][4];





// Funcion NF_InitTiledBgSys();
void NF_InitAffineBgSys(u8 screen) {

	// Variables
	u8 n = 0;

	// Define los bancos de Mapas y Tiles
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
		vramSetBankA(VRAM_A_MAIN_BG);				// Banco A de la VRAM para fondos (128kb)
		memset((void*)0x06000000, 0, 131072);		// Borra el contenido del banco A
		for (n = 0; n < 4; n ++) {					// Oculta todas las 4 capas
			NF_HideBg(0, n);
		}
	} else {
		// Si es la pantalla 1 (Inferior, Sub engine)
		vramSetBankC(VRAM_C_SUB_BG);					// Banco C de la VRAM para fondos (128kb)
		memset((void*)0x06200000, 0, 131072);			// Borra el contenido del banco C
		for (n = 0; n < 4; n ++) {						// Oculta todas las 4 capas
			NF_HideBg(1, n);
		}
	}

}



// Funcion NF_LoadAffineBg();
void NF_LoadAffineBg(const char* file, const char* name, u16 width, u16 height) {

	// Verifica si el fondo cumple las medidas correctas
	if (((width == 256) && (height == 256)) || ((width == 512) && (height == 512))) {
		// Medida Ok
	} else {
		// Error de tamaño
		NF_Error(117, name, 0);
	}
	
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
	// swiWaitForVBlank();		// Espera al cierre del archivo (Usar en caso de corrupcion de datos)

	// Verifica el tamaño del tileset (Menos de 256 tiles)
	if (NF_TILEDBG[slot].tilesize > 16384) NF_Error(117, name, 0);


	// Carga el archivo .MAP
	sprintf(filename, "%s/%s.map", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_TILEDBG[slot].mapsize = ((((ftell(file_id) - 1) >> 10) + 1) << 10);	// Ajusta el tamaño a bloques de 1kb
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
	// swiWaitForVBlank();		// Espera al cierre del archivo (Usar en caso de corrupcion de datos)

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



// Funcion NF_UnloadAffineBg();
void NF_UnloadAffineBg(const char* name) {
	NF_UnloadTiledBg(name);
}





// Funcion NF_CreateAffineBg();
void NF_CreateAffineBg(u8 screen, u8 layer, const char* name, u8 wrap) {

	// Variables
	u8 n = 0;			// Bucle
	u8 slot = 255;		// Slot seleccionado
	char bg[32];		// Nombre

	// Verifica la capa de destino
	if ((layer != 2) && (layer != 3)) NF_Error(118, name, 0);

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

	// Calcula el tipo y tamaño del mapa
	n = 0;
	// ( 256 x 256 )
	if ((NF_TILEDBG[slot].width == 256) && (NF_TILEDBG[slot].height == 256)) {
		NF_TILEDBG_LAYERS[screen][layer].mapwidth = 256;
		NF_TILEDBG_LAYERS[screen][layer].mapheight = 256;
		NF_TILEDBG_LAYERS[screen][layer].bgtype = 11;
		n = 1;
	}
	// ( 512 x 512 )
	if ((NF_TILEDBG[slot].width == 512) && (NF_TILEDBG[slot].height == 512)) {
		NF_TILEDBG_LAYERS[screen][layer].mapwidth = 512;
		NF_TILEDBG_LAYERS[screen][layer].mapheight = 512;
		NF_TILEDBG_LAYERS[screen][layer].bgtype = 12;
		n = 1;
	}

	// Verifica el tamaño del tileset (Menos de 256 tiles)
	if (NF_TILEDBG[slot].tilesize > 16384) n = 0;

	// Si el fondo es de una medida incorrecta...
	if (n == 0) NF_Error(117, name, 0);

	// Busca un los bloques libres para almacenar los Tiles en VRAM
	tilesblocks = ((NF_TILEDBG[slot].tilesize - 1) >> 14) + 1;	// Bloques necesarios para el Tileset

	for (n = 0; n < NF_BANKS_TILES[screen]; n ++) {
		if (NF_TILEBLOCKS[screen][n] == 0) {		// Si esta libre
			if (counter == 0) {						// Y el contador esta a 0
				start = n;							// Marca la posicion de inicio
			}
			counter ++;								
			if (counter == tilesblocks) {			// Si ya tienes suficientes bloques libres
				n = NF_BANKS_TILES[screen];					// Termina de buscar
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

	// Calcula los bloques para mapas necesarios
	mapblocks = ((NF_TILEDBG[slot].mapsize - 1) >> 11) + 1;

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
	s32 bg_size = 0;
	// 256x256
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 256) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 256)) {
		bg_size = BG_RS_32x32;
	}
	// 512x512
	if ((NF_TILEDBG_LAYERS[screen][layer].mapwidth == 512) && (NF_TILEDBG_LAYERS[screen][layer].mapheight == 512)) {
		bg_size = BG_RS_64x64;
	}

	// Decide si se activa o no el WRAP
	u32 wrap_mode = 0;
	if (wrap == 0) {
		wrap_mode = BG_WRAP_OFF;
	} else {
		wrap_mode = BG_WRAP_ON;
	}


	// Crea el fondo segun la pantalla, capa y demas caracteristicas dadas
	// REG_BG0CNT	<- Carracteristicas del fondo
	if (screen == 0) {
		switch (layer) {
			case 2:
				REG_BG2CNT = BgType_Rotation | bg_size | BG_PRIORITY_2 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
				break;
			case 3:
				REG_BG3CNT = BgType_Rotation | bg_size | BG_PRIORITY_3 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
				break;
		} 
	} else {
		switch (layer) {
			case 2:
				REG_BG2CNT_SUB = BgType_Rotation | bg_size | BG_PRIORITY_2 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
				break;
			case 3:
				REG_BG3CNT_SUB = BgType_Rotation | bg_size | BG_PRIORITY_3 | BG_COLOR_256 | BG_TILE_BASE(basetiles) | BG_MAP_BASE(basemap) | wrap_mode;
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
	NF_DmaMemCopy((void*)address, NF_BUFFER_BGMAP[slot], NF_TILEDBG[slot].mapsize);


	// Tranfiere la Paleta a VRAM
	if (screen == 0) {
		address = (0x05000000);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
	} else {	// Paletas de la pantalla 1 (VRAM_H)
		address = (0x05000400);
		NF_DmaMemCopy((void*)address, NF_BUFFER_BGPAL[slot], NF_TILEDBG[slot].palsize);
	}

	// Registra los datos del fondos en pantalla
	NF_TILEDBG_LAYERS[screen][layer].tilebase = basetiles;				// Base del Tileset
	NF_TILEDBG_LAYERS[screen][layer].tileblocks = tilesblocks;			// Bloques usados por el Tileset
	NF_TILEDBG_LAYERS[screen][layer].mapbase = basemap;					// Base del Map
	NF_TILEDBG_LAYERS[screen][layer].mapblocks = mapblocks;				// Bloques usados por el Map
	NF_TILEDBG_LAYERS[screen][layer].created = true;					// Esta creado ?

	// Resetea los parametros del affine
	NF_AffineBgTransform(screen, layer, 256, 256, 0, 0);
	NF_AffineBgMove(screen, layer, 0, 0, 0);

	// Haz visible el fondo creado
	NF_ShowBg(screen, layer);

}



// Funcion NF_DeleteAffineBg();
void NF_DeleteAffineBg(u8 screen, u8 layer) {

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



// Funcion NF_AffineBgTransform();
void NF_AffineBgTransform(u8 screen, u8 layer, s32 x_scale, s32 y_scale, s32 x_tilt, s32 y_tilt) {

	if (screen == 0) {
		switch (layer) {
			case 2:
				REG_BG2PA = x_scale;
				REG_BG2PB = x_tilt;
				REG_BG2PC = y_tilt;
				REG_BG2PD = y_scale;
				break;
			case 3:
				REG_BG3PA = x_scale;
				REG_BG3PB = x_tilt;
				REG_BG3PC = y_tilt;
				REG_BG3PD = y_scale;
				break;
		}
	} else {
		switch (layer) {
			case 2:
				REG_BG2PA_SUB = x_scale;
				REG_BG2PB_SUB = x_tilt;
				REG_BG2PC_SUB = y_tilt;
				REG_BG2PD_SUB = y_scale;
				break;
			case 3:
				REG_BG3PA_SUB = x_scale;
				REG_BG3PB_SUB = x_tilt;
				REG_BG3PC_SUB = y_tilt;
				REG_BG3PD_SUB = y_scale;
				break;
		}
	}

	// Registra los valores asignados
	NF_AFFINE_BG[screen][layer].x_scale = x_scale;
	NF_AFFINE_BG[screen][layer].x_tilt = x_tilt;
	NF_AFFINE_BG[screen][layer].y_tilt = y_tilt;
	NF_AFFINE_BG[screen][layer].y_scale = y_scale;

}





// Funcion NF_AffineBgMove();
void NF_AffineBgMove(u8 screen, u8 layer, s32 x, s32 y, s32 angle) {

	// Funcion de rotacion basada en la original de Libnds
	// creada por Dovoto y Wintermute.

	// Variables
	s32 pa = 0;				// x_scale
	s32 pb = 0;				// x_tilt
	s32 pc = 0;				// y_tilt;
	s32 pd = 0;				// y_scale;
	s16 angle_sin = 0;		// Seno
	s16 angle_cos = 0;		// Coseno
	s16 in = 0;				// Angulo dado
	s16 out = 0;			// Angulo convertido
	s32 pos_x = 0;			// Posicion X del fondo
	s32 pos_y = 0;			// Posicion Y del fondo

	in = angle;

	// Limites del angulo
	if (in < -2048) {
		in += 2048;
	}
	if (in > 2048) {
		in -= 2048;
	}
	
	// Si es un numero negativo...
	if (in < 0) {
		in = -in;			// Pasa a positivo (para poder hacer el bitshift)
		out = (in << 4);	// (in * 16); Pasa de base 2048 a base 32768
		// Dejalo en positivo para que <0 gire a la izquierda
	} else {
		out = (in << 4);
		out = -out;			// Pasalo a negativo para que >0 gire a la derecha
	}

	// Calcula los senos y cosenos
	angle_sin = sinLerp(out);
	angle_cos = cosLerp(out);

	// Calcula la matriz de transformacion
	pa = ( angle_cos * NF_AFFINE_BG[screen][layer].x_scale ) >> 12;
	pb = (-angle_sin * NF_AFFINE_BG[screen][layer].x_scale ) >> 12;
	pc = ( angle_sin * NF_AFFINE_BG[screen][layer].y_scale ) >> 12;
	pd = ( angle_cos * NF_AFFINE_BG[screen][layer].y_scale ) >> 12;

	// Aplica los parametros de tranformacion
	NF_AffineBgTransform(screen, layer, pa, pd, pb, pc);

	// Ahora calcula la posicion del fondo
    pos_x = ((x << 8) - (((pa * (NF_AFFINE_BG[screen][layer].x_center << 8)) + (pb * (NF_AFFINE_BG[screen][layer].y_center << 8))) >> 8));
	pos_y = ((y << 8) - (((pc * (NF_AFFINE_BG[screen][layer].x_center << 8)) + (pd * (NF_AFFINE_BG[screen][layer].y_center << 8))) >> 8));

	// Aplica la posicion del centro
	if (screen == 0) {
		switch (layer) {
			case 2:
				REG_BG2X = pos_x;
				REG_BG2Y = pos_y;
				break;
			case 3:
				REG_BG3X = pos_x;
				REG_BG3Y = pos_y;
				break;
		}
	} else {
		switch (layer) {
			case 2:
				REG_BG2X_SUB = pos_x;
				REG_BG2Y_SUB = pos_y;
				break;
			case 3:
				REG_BG3X_SUB = pos_x;
				REG_BG3Y_SUB = pos_y;
				break;
		}
	}

	// Guarda los parametros
	NF_AFFINE_BG[screen][layer].angle = out;
	NF_AFFINE_BG[screen][layer].x = x;
	NF_AFFINE_BG[screen][layer].y = y;

}



// Funcion NF_AffineBgCenter();
void NF_AffineBgCenter(u8 screen, u8 layer, s32 x, s32 y) {

	NF_AFFINE_BG[screen][layer].x_center = x;
	NF_AFFINE_BG[screen][layer].y_center = y;

}

