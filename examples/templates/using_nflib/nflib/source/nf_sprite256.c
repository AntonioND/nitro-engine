
// NightFox LIB - Funciones de Sprites a 256 colores
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
#include <stdarg.h>

// Includes propios
#include "nf_basic.h"
#include "nf_2d.h"
#include "nf_sprite256.h"



// Define los Buffers para almacenar los Sprites
char* NF_BUFFER_SPR256GFX[NF_SLOTS_SPR256GFX];
char* NF_BUFFER_SPR256PAL[NF_SLOTS_SPR256PAL];

// Define la estructura de datos de los Graficos de los Sprites
NF_TYPE_SPR256GFX_INFO NF_SPR256GFX[NF_SLOTS_SPR256GFX];
// Define la estructura de datos de las Paletas de los Sprites
NF_TYPE_SPR256PAL_INFO NF_SPR256PAL[NF_SLOTS_SPR256PAL];

// Define la estructura de Gfx en VRAM
NF_TYPE_SPR256VRAM_INFO NF_SPR256VRAM[2][128];
// Datos de paletas de Sprites en VRAM (en uso, slot en ram, etc)
NF_TYPE_SPRPALSLOT_INFO NF_SPRPALSLOT[2][16];

// Define la estructura de datos del OAM (Sprites)
NF_TYPE_SPRITEOAM_INFO NF_SPRITEOAM[2][128];		// 2 pantallas, 128 sprites

// Define la esturctura de control de la VRAM para Sprites
NF_TYPE_SPRVRAM_INFO NF_SPRVRAM[2];		// Informacion VRAM de Sprites en ambas pantallas





// Funcion NF_InitSpriteBuffers()
void NF_InitSpriteBuffers(void) {

	u16 n = 0;	// Variable comun

	// Inicializa Buffers de GFX
	for (n = 0; n < NF_SLOTS_SPR256GFX; n ++) {
		NF_BUFFER_SPR256GFX[n] = NULL;			// Inicializa puntero
		NF_SPR256GFX[n].size = 0;				// Tamaño (en bytes) del grafico (GFX)
		NF_SPR256GFX[n].width = 0;				// Ancho del Gfx
		NF_SPR256GFX[n].height = 0;				// Altura del Gfx
		NF_SPR256GFX[n].available = true;		// Disponibilidat del Slot
	}

	// Inicializa Buffers de PAL
	for (n = 0; n < NF_SLOTS_SPR256PAL; n ++) {
		NF_BUFFER_SPR256PAL[n] = NULL;		// Inicializa puntero
		NF_SPR256PAL[n].size = 0;			// Tamaño (en bytes) de la paleta (PAL)
		NF_SPR256PAL[n].available = true;	// Disponibilidat del Slot
	}

}



// Funcion NF_ResetSpriteBuffers()
void NF_ResetSpriteBuffers(void) {

	u16 n = 0;	// Variable comun

	// Borra los Buffers de GFX
	for (n = 0; n < NF_SLOTS_SPR256GFX; n ++) {
		free(NF_BUFFER_SPR256GFX[n]);
	}

	// Borra los Buffers de PAL
	for (n = 0; n < NF_SLOTS_SPR256PAL; n ++) {
		free(NF_BUFFER_SPR256PAL[n]);
	}

	// Reinicia el sistema de Sprites
	NF_InitSpriteBuffers();

}



// Funcion NF_InitSpriteSys();
void NF_InitSpriteSys(int screen, ...) {

	// Analiza los parametros variables de la funcion
	va_list options;
	va_start(options, screen);
	u8 mode = va_arg(options, int);
	va_end(options);


	// Variables
	u8 n = 0;	// Uso comun

	// Inicializa la estructura de Gfx en VRAM
	// y la estructura de datos del OAM (Sprites)
	for (n = 0; n < 128; n ++) {	// 128 sprites
		// Gfx en la VRAM (128 Gfx x pantalla)
		NF_SPR256VRAM[screen][n].size = 0;				// Tamaño (en bytes) del Gfx
		NF_SPR256VRAM[screen][n].width = 0;				// Ancho del Gfx
		NF_SPR256VRAM[screen][n].height = 0;			// Altura del Gfx
		NF_SPR256VRAM[screen][n].address = 0;			// Posicion en la VRAM
		NF_SPR256VRAM[screen][n].ramid = 0;				// Numero de Slot en RAM del que provienes
		NF_SPR256VRAM[screen][n].framesize = 0;			// Tamaño del frame (en bytes)
		NF_SPR256VRAM[screen][n].lastframe = 0;			// Ultimo frame
		NF_SPR256VRAM[screen][n].keepframes = false;	// Si es un Sprite animado, debes de mantener los frames en RAM ?
		NF_SPR256VRAM[screen][n].inuse = false;			// Esta en uso ?
		// OAM (128 Sprites x pantalla)
		NF_SPRITEOAM[screen][n].index = n;			// Numero de Sprite (Index = N)
		NF_SPRITEOAM[screen][n].x = 0;				// Coordenada X del Sprite (0 por defecto)
		NF_SPRITEOAM[screen][n].y = 0;				// Coordenada Y del Sprite (0 por defecto)
		NF_SPRITEOAM[screen][n].layer = 0;			// Prioridad en las capas (0 por defecto)
		NF_SPRITEOAM[screen][n].pal = 0;			// Paleta que usaras (0 por defecto)
		NF_SPRITEOAM[screen][n].size = SpriteSize_8x8;					// Tamaño del Sprite (macro) (8x8 por defecto)
		NF_SPRITEOAM[screen][n].color = SpriteColorFormat_256Color;		// Modo de color (macro) (256 colores)
		NF_SPRITEOAM[screen][n].gfx = NULL;				// Puntero al grafico usado
		NF_SPRITEOAM[screen][n].rot = -1;				// Id de rotacion (-1 por defecto) (0 - 31 Id de rotacion)
		NF_SPRITEOAM[screen][n].doublesize = false;		// Usar el "double size" al rotar ? ("NO" por defecto)
		NF_SPRITEOAM[screen][n].hide = true;			// Ocultar el Sprite ("SI" por defecto)
		NF_SPRITEOAM[screen][n].hflip = false;			// Volteado Horizontal ("NO" por defecto)
		NF_SPRITEOAM[screen][n].vflip = false;			// Volteado Vertical ("NO" por defecto)
		NF_SPRITEOAM[screen][n].mosaic = false;			// Mosaico ("NO" por defecto)
		NF_SPRITEOAM[screen][n].gfxid = 0;				// Numero de Gfx usado
		NF_SPRITEOAM[screen][n].frame = 0;				// Frame actual
		NF_SPRITEOAM[screen][n].framesize = 0;			// Tamaño del frame (en bytes)
		NF_SPRITEOAM[screen][n].lastframe = 0;			// Ultimo frame
		NF_SPRITEOAM[screen][n].created = false;		// Esta creado este sprite ?
	}

	// Inicializa la estructura de datos de la VRAM de Sprites
	if (mode == 128) {
		NF_SPRVRAM[screen].max = 131072;
	} else {
		NF_SPRVRAM[screen].max = 65536;
	}
	NF_SPRVRAM[screen].free = NF_SPRVRAM[screen].max;		// Memoria VRAM libre (64kb/128kb)
	NF_SPRVRAM[screen].last = 0;							// Ultima posicion usada
	NF_SPRVRAM[screen].deleted = 0;							// Ningun Gfx borrado
	NF_SPRVRAM[screen].fragmented = 0;						// Memoria VRAM fragmentada
	NF_SPRVRAM[screen].inarow = NF_SPRVRAM[screen].max;		// Memoria VRAM contigua
	for (n = 0; n < 128; n ++) {
		NF_SPRVRAM[screen].pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		NF_SPRVRAM[screen].size[n] = 0;		// Tamaño del bloque libre para reusar
	}

	// Inicializa los datos de las paletas
	for (n = 0; n < 16; n ++) {
		NF_SPRPALSLOT[screen][n].inuse = false;
		NF_SPRPALSLOT[screen][n].ramslot = 0;
	}

	// Configura el Motor 2D y VRAM segun la pantalla de destino
	if (screen == 0) {

		// Configura la pantalla 0
		REG_DISPCNT |= (DISPLAY_SPR_ACTIVE);			// Activa los Sprites en la pantalla superior
		vramSetBankB(VRAM_B_MAIN_SPRITE_0x06400000);	// Banco B de la VRAM para Sprites (128kb)
		memset((void*)0x06400000, 0, 131072);			// Borra el contenido del banco B
		NF_SPRVRAM[screen].next = (0x06400000);			// Guarda la primera posicion de VRAM para Gfx
		vramSetBankF(VRAM_F_LCD);						// Banco F de la VRAM para paletas extendidas (Sprites) (8kb de 16kb)
		memset((void*)0x06890000, 0, 8192);				// Borra el contenido del banco F
		if (mode == 128) {
			oamInit(&oamMain, SpriteMapping_1D_128, true);	// Inicializa el OAM (Mapeado de 128 bytes, Paletas extendidas)
		} else {
			oamInit(&oamMain, SpriteMapping_1D_64, true);	// Inicializa el OAM (Mapeado de 64 bytes, Paletas extendidas)
		}

	} else {

		// Configura la pantalla 1
		REG_DISPCNT_SUB |= (DISPLAY_SPR_ACTIVE);		// Activa los Sprites en la pantalla inferior
		vramSetBankD(VRAM_D_SUB_SPRITE);				// Banco D de la VRAM para Sprites (128kb)
		memset((void*)0x06600000, 0, 131072);			// Borra el contenido del banco D
		NF_SPRVRAM[screen].next = (0x06600000);			// Guarda la primera posicion de VRAM para Gfx
		vramSetBankI(VRAM_I_LCD);						// Banco I de la VRAM para paletas extendidas (Sprites) (8kb de 16kb)
		memset((void*)0x068A0000, 0, 8192);				// Borra el contenido del banco I
		if (mode == 128) {
			oamInit(&oamSub, SpriteMapping_1D_128, true);	// Inicializa el OAM (Mapeado de 128 bytes, Paletas extendidas)
		} else {
			oamInit(&oamSub, SpriteMapping_1D_64, true);	// Inicializa el OAM (Mapeado de 64 bytes, Paletas extendidas)
		}

	}

}



// Funcion NF_LoadSpriteGfx();
void NF_LoadSpriteGfx(const char* file, u16 id,  u16 width, u16 height) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256GFX)) {
		NF_Error(106, "Sprite GFX", NF_SLOTS_SPR256GFX);
	}

	// Verifica si la Id esta libre
	if (!NF_SPR256GFX[id].available) {
		NF_Error(109, "Sprite GFX", id);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_SPR256GFX[id]);
	NF_BUFFER_SPR256GFX[id] = NULL;

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
		NF_SPR256GFX[id].size = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_BUFFER_SPR256GFX[id] = (char*) calloc (NF_SPR256GFX[id].size, sizeof(char));
		if (NF_BUFFER_SPR256GFX[id] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_SPR256GFX[id].size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_SPR256GFX[id], 1, NF_SPR256GFX[id].size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Guarda las medidas del grafico
	NF_SPR256GFX[id].width = width;		// Ancho del Gfx
	NF_SPR256GFX[id].height = height;	// Altura del Gfx

	// Y marca esta ID como usada
	NF_SPR256GFX[id].available = false;

}



// Funcion NF_UnloadSpriteGfx();
void NF_UnloadSpriteGfx(u16 id) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256GFX)) {
		NF_Error(106, "Sprite GFX", NF_SLOTS_SPR256GFX);
	}

	// Verifica si la Id esta libre
	if (NF_SPR256GFX[id].available) {
		NF_Error(110, "Sprite GFX", id);
	}

	// Vacia el buffer
	free(NF_BUFFER_SPR256GFX[id]);

	// Y reinicia las variables
	NF_BUFFER_SPR256GFX[id] = NULL;			// Inicializa puntero
	NF_SPR256GFX[id].size = 0;				// Tamaño (en bytes) del grafico (GFX)
	NF_SPR256GFX[id].width = 0;				// Ancho del Gfx
	NF_SPR256GFX[id].height = 0;			// Altura del Gfx
	NF_SPR256GFX[id].available = true;		// Disponibilidat del Slot

}



// Funcion NF_LoadSpritePal();
void NF_LoadSpritePal(const char* file, u8 id) {

	// Variable temporal del tamaño de la paleta
	u32 pal_size = 0;

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256PAL)) {
		NF_Error(106, "Sprite PAL", NF_SLOTS_SPR256PAL);
	}

	// Verifica si la Id esta libre
	if (!NF_SPR256PAL[id].available) {
		NF_Error(109, "Sprite PAL", id);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_SPR256PAL[id]);
	NF_BUFFER_SPR256PAL[id] = NULL;

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
		NF_SPR256PAL[id].size = pal_size;
		rewind(file_id);
		// Si el tamaño es inferior a 512 bytes, ajustalo
		if (NF_SPR256PAL[id].size < 512) NF_SPR256PAL[id].size = 512;
		// Reserva el espacio en RAM
		NF_BUFFER_SPR256PAL[id] = (char*) calloc (NF_SPR256PAL[id].size, sizeof(char));
		if (NF_BUFFER_SPR256PAL[id] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_SPR256PAL[id].size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_SPR256PAL[id], 1, pal_size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Y marca esta ID como usada
	NF_SPR256PAL[id].available = false;

}



// Funcion NF_UnloadSpritePal();
void NF_UnloadSpritePal(u8 id) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256PAL)) {
		NF_Error(106, "Sprite PAL", NF_SLOTS_SPR256PAL);
	}

	// Verifica si la Id esta libre
	if (NF_SPR256PAL[id].available) {
		NF_Error(110, "Sprite PAL", id);
	}

	// Vacia el buffer
	free(NF_BUFFER_SPR256PAL[id]);

	// Y reinicia las variables
	NF_BUFFER_SPR256PAL[id] = NULL;		// Inicializa puntero
	NF_SPR256PAL[id].size = 0;			// Tamaño (en bytes) de la paleta (PAL)
	NF_SPR256PAL[id].available = true;	// Disponibilidat del Slot

}



// Funcion NF_VramSpriteGfx();
void NF_VramSpriteGfx(u8 screen, u16 ram, u16 vram, bool keepframes) {

	// Verifica el rango de Id's de RAM
	if ((ram < 0) || (ram >= NF_SLOTS_SPR256GFX)) {
		NF_Error(106, "Sprite GFX", (NF_SLOTS_SPR256GFX - 1));
	}

	// Verifica si slot de RAM esta vacio
	if (NF_SPR256GFX[ram].available) {
		NF_Error(110, "Sprite GFX", ram);
	}

	// Verifica el rango de Id's de VRAM
	if ((vram < 0) || (vram > 127)) {
		NF_Error(106, "VRAM GFX", 127);
	}

	// Verifica si el slot de VRAM esta libre
	if (NF_SPR256VRAM[screen][vram].inuse) {
		NF_Error(109, "VRAM", vram);
	}

	// Variables de uso general
	s16 n = 0;				// General
	s16 id = 255;			// Id del posible bloque libre
	s16 last_reuse = 0;		// Nº del ultimo bloque reusable
	u32 gfxsize = 0;		// Tamaño de los datos que se copiaran
	u32 size = 0;			// Diferencia de tamaños entre bloque libre y datos
	u8 width = 0;			// Calculo de las medidas
	u8 height = 0;
	bool organize = true;	// Se debe de reorganizar el array de bloques libres ?

	// Auto calcula el tamaño de 1 frame
	width = (NF_SPR256GFX[ram].width >> 3);		// (width / 8)
	height = (NF_SPR256GFX[ram].height >> 3);	// (height / 8)
	NF_SPR256VRAM[screen][vram].framesize = ((width * height) << 6);	// ((width * height) * 64)
	// Auto calcula el ultimo frame de la animacion
	NF_SPR256VRAM[screen][vram].lastframe = ((int)(NF_SPR256GFX[ram].size / NF_SPR256VRAM[screen][vram].framesize)) - 1;
	NF_SPR256VRAM[screen][vram].inuse = true;						// Slot ocupado

	// Calcula el tamaño del grafico a copiar segun si debes o no copiar todos los frames
	if (keepframes) {	// Si debes de mantener los frames en RAM, solo copia el primero
		gfxsize = NF_SPR256VRAM[screen][vram].framesize;
	} else {			// Si no, copialos todos
		gfxsize = NF_SPR256GFX[ram].size;
	}

	// Actualiza la VRAM disponible
	NF_SPRVRAM[screen].free -= gfxsize;

	// Si no hay suficiente VRAM, error
	if (NF_SPRVRAM[screen].free < 0) {
		NF_Error(113, "Sprites", gfxsize);
	}

	// Si hay que aprovechar algun bloque borrado... (tamaño identico, preferente)
	if (NF_SPRVRAM[screen].deleted > 0) {
		// Busca un bloque vacio del tamaño identico
		for (n = 0; n < NF_SPRVRAM[screen].deleted; n ++) {
			if (NF_SPRVRAM[screen].size[n] == gfxsize) {		// Si el bloque tiene el tamaño suficiente
				id = n;		// Guarda la Id
				n = NF_SPRVRAM[screen].deleted;	// y sal
			}
		}
		// Si no habia ningun bloque de tamaño identico, busca el mas parecido (produce fragmentacion)
		if (id == 255) {
			// Busca un bloque vacio del tamaño suficiente
			for (n = 0; n < NF_SPRVRAM[screen].deleted; n ++) {
				if (NF_SPRVRAM[screen].size[n] > gfxsize) {		// Si el bloque tiene el tamaño suficiente
					id = n;		// Guarda la Id
					n = NF_SPRVRAM[screen].deleted;	// y sal
				}
			}
		}
	}
	
	// Si hay algun bloque borrado libre del tamaño suficiente...
	if (id != 255) {

		// Transfiere el grafico a la VRAM
		NF_DmaMemCopy((void*)NF_SPRVRAM[screen].pos[id], NF_BUFFER_SPR256GFX[ram], gfxsize);
		// Guarda el puntero donde lo has almacenado
		NF_SPR256VRAM[screen][vram].address = NF_SPRVRAM[screen].pos[id];

		// Si no has usado todo el tamaño, deja constancia
		if (gfxsize < NF_SPRVRAM[screen].size[id]) {

			// Calcula el tamaño del nuevo bloque libre
			size = (NF_SPRVRAM[screen].size[id] - gfxsize);
			// Actualiza los datos
			NF_SPRVRAM[screen].pos[id] += gfxsize;			// Nueva direccion
			NF_SPRVRAM[screen].size[id] = size;				// Nuevo tamaño
			NF_SPRVRAM[screen].fragmented -= gfxsize;		// Actualiza el contador de VRAM fragmentada
			organize = false;								// No se debe de reorganizar el array de bloques

		} else {	// Si has usado todo el tamaño, deja constancia

			NF_SPRVRAM[screen].fragmented -= NF_SPRVRAM[screen].size[id];	// Actualiza el contador de VRAM fragmentada

		}

		// Se tiene que reorganizar el array de bloques libres ?
		if (organize) {
			last_reuse = (NF_SPRVRAM[screen].deleted - 1);
			if (
			(last_reuse > 0)	// Si hay mas de un bloque borrado
			&&
			(id != last_reuse)	// Y no es la ultima posicion
			) {
				// Coloca los valores de la ultima posicion en esta
				NF_SPRVRAM[screen].pos[id] = NF_SPRVRAM[screen].pos[last_reuse];		// Nueva direccion
				NF_SPRVRAM[screen].size[id] = NF_SPRVRAM[screen].size[last_reuse];		// Nuevo tamaño
			}
			NF_SPRVRAM[screen].deleted --;		// Actualiza el contador de bloques libres, borrando el ultimo registro
		}

	} else {	// Si no habia ningun bloque borrado o con el tamaño suficiente, colacalo al final de la VRAM ocupada

		// Actualiza la VRAM contigua disponible (mayor bloque libre al final)
		NF_SPRVRAM[screen].inarow -= gfxsize;

		// Si no hay suficiente VRAM (contigua), error
		if (NF_SPRVRAM[screen].inarow < 0) {
			NF_Error(113, "Sprites", gfxsize);
		}

		// Transfiere el grafico a la VRAM
		NF_DmaMemCopy((void*)NF_SPRVRAM[screen].next, NF_BUFFER_SPR256GFX[ram], gfxsize);
		// Guarda el puntero donde lo has almacenado
		NF_SPR256VRAM[screen][vram].address = NF_SPRVRAM[screen].next;
		// Guarda la direccion actual como la ultima usada
		NF_SPRVRAM[screen].last = NF_SPRVRAM[screen].next;
		// Calcula la siguiente posicion libre
		NF_SPRVRAM[screen].next += gfxsize;

	}

	// Guarda los datos del Gfx que se copiara a la VRAM.
	NF_SPR256VRAM[screen][vram].size = gfxsize;						// Tamaño en bytes de los datos copiados
	NF_SPR256VRAM[screen][vram].width = NF_SPR256GFX[ram].width;	// Alto (px)
	NF_SPR256VRAM[screen][vram].height = NF_SPR256GFX[ram].height;	// Ancho (px)
	NF_SPR256VRAM[screen][vram].ramid = ram;						// Slot RAM de origen
	NF_SPR256VRAM[screen][vram].keepframes = keepframes;			// Debes guardar los frames en RAM o copiarlos a la VRAM?

}



// Funcion NF_FreeSpriteGfx();
void NF_FreeSpriteGfx(u8 screen, u16 id) {

	// Verifica si hay un grafico cargado en esa Id.
	if (!NF_SPR256VRAM[screen][id].inuse) {
		NF_Error(110, "Sprite Gfx", id);
	}

	// Borra el Gfx de la VRAM (pon a 0 todos los Bytes)
	memset((void*)NF_SPR256VRAM[screen][id].address, 0, NF_SPR256VRAM[screen][id].size);

	// Actualiza la cantidad de VRAM disponible
	NF_SPRVRAM[screen].free += NF_SPR256VRAM[screen][id].size;

	// Guarda la posicion y tamaño del bloque borrado para su reutilizacion
	NF_SPRVRAM[screen].pos[NF_SPRVRAM[screen].deleted] = NF_SPR256VRAM[screen][id].address;
	NF_SPRVRAM[screen].size[NF_SPRVRAM[screen].deleted] = NF_SPR256VRAM[screen][id].size;

	// Incrementa en contador de bloques borrados
	NF_SPRVRAM[screen].deleted ++;

	// Incrementa el contador de memoria fragmentada
	NF_SPRVRAM[screen].fragmented += NF_SPR256VRAM[screen][id].size;

	// Reinicia los datos de esta Id. de gfx
	NF_SPR256VRAM[screen][id].size = 0;			// Tamaño en bytes
	NF_SPR256VRAM[screen][id].width = 0;		// Alto (px)
	NF_SPR256VRAM[screen][id].height = 0;		// Ancho (px)
	NF_SPR256VRAM[screen][id].address = 0;		// Puntero en VRAM
	NF_SPR256VRAM[screen][id].framesize = 0;	// Tamaño del frame (en bytes)
	NF_SPR256VRAM[screen][id].lastframe = 0;	// Ultimo frame
	NF_SPR256VRAM[screen][id].inuse = false;

	// Debes desfragmentar la VRAM
	if (NF_SPRVRAM[screen].fragmented >= (NF_SPRVRAM[screen].inarow >> 1)) {
		NF_VramSpriteGfxDefrag(screen);
	}

}



// Funcion NF_VramSpriteGfxDefrag();
void NF_VramSpriteGfxDefrag(u8 screen) {

	// Calcula la VRAM en uso y crea un buffer para guardarla
	u32 used_vram = ((NF_SPRVRAM[screen].max - NF_SPRVRAM[screen].free) + 1);
	char* buffer;
	buffer = (char*) calloc (used_vram, sizeof(char));
	if (buffer == NULL) {		// Si no hay suficiente RAM libre
		NF_Error(102, NULL, used_vram);
	}

	char* address[128];		// Guarda la direccion en RAM
	u32 size[128];			// Guarda el tamaño
	u32 ram = 0;			// Puntero inicial de RAM
	u8 n = 0;				// Variable General
	u32 frame_address = 0;	// Guarda la direccion de VRAM del frame


	// Copia los datos de la VRAM a la RAM
	for (n = 0; n < 128; n ++) {
		// Si esta en uso
		if (NF_SPR256VRAM[screen][n].inuse) {
			// Copia el Gfx a la RAM
			address[n] = (buffer + ram);		// Calcula el puntero
			size[n] = NF_SPR256VRAM[screen][n].size;		// Almacena el tamaño
			NF_DmaMemCopy(address[n], (void*)NF_SPR256VRAM[screen][n].address, size[n]);	// Copialo a la VRAM
			ram += size[n];		// Siguiente posicion en RAM (relativa)
		}
	}

	// Inicializa la estructura de datos de la VRAM de Sprites
	NF_SPRVRAM[screen].free = NF_SPRVRAM[screen].max;		// Memoria VRAM libre (128kb)
	NF_SPRVRAM[screen].last = 0;							// Ultima posicion usada
	NF_SPRVRAM[screen].deleted = 0;							// Ningun Gfx borrado
	NF_SPRVRAM[screen].fragmented = 0;						// Memoria VRAM fragmentada
	NF_SPRVRAM[screen].inarow = NF_SPRVRAM[screen].max;		// Memoria VRAM contigua
	for (n = 0; n < 128; n ++) {
		NF_SPRVRAM[screen].pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		NF_SPRVRAM[screen].size[n] = 0;		// Tamaño del bloque libre para reusar
	}
	// Aplica la direccion de inicio de la VRAM
	if (screen == 0) {
		NF_SPRVRAM[screen].next = (0x06400000);
	} else {
		NF_SPRVRAM[screen].next = (0x06600000);
	}

	// Ahora, copia de nuevo los datos a la VRAM, pero alineados
	for (n = 0; n < 128; n ++) {
		// Si esta en uso
		if (NF_SPR256VRAM[screen][n].inuse) {
			NF_DmaMemCopy((void*)NF_SPRVRAM[screen].next, address[n], size[n]);		// Vuelve a colocar la el Gfx en VRAM
			NF_SPR256VRAM[screen][n].address = NF_SPRVRAM[screen].next;				// Guarda la nueva posicion en VRAM
			NF_SPRVRAM[screen].free -= size[n];		// Ram libre
			NF_SPRVRAM[screen].inarow -= size[n];	// Ram libre en bloque
			NF_SPRVRAM[screen].last = NF_SPRVRAM[screen].next;	// Guarda la posicion como ultima usada
			NF_SPRVRAM[screen].next += size[n];		// Y calcula la siguiente posicion a escribir
		}
	}

	// Reasigna a los sprites las nuevas posiciones de los graficos que usan
	for (n = 0; n < 128; n ++) {
		if (NF_SPRITEOAM[screen][n].created) {
			if (NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].keepframes) {
				// Si la opcion de animacion KEEP FRAMES esta activada,
				// simplemente asigna la nueva direccion en VRAM del grafico.
				NF_SPRITEOAM[screen][n].gfx = (u32*)NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].address;
			} else {
				// Si la opcion KEEP FRAMES esta desactivada,
				// calcula el desplazamiento dentro de la nueva direccion asignada.
				frame_address = (NF_SPR256VRAM[screen][NF_SPRITEOAM[screen][n].gfxid].address + (NF_SPRITEOAM[screen][n].framesize * NF_SPRITEOAM[screen][n].frame));
				NF_SPRITEOAM[screen][n].gfx = (u32*)frame_address;
			}
		}
	}

	// Vacia el buffer
	free(buffer);
	buffer = NULL;

}



// Funcion NF_VramSpritePal();
void NF_VramSpritePal(u8 screen, u8 id, u8 slot) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256PAL)) {
		NF_Error(106, "Sprite PAL", NF_SLOTS_SPR256PAL);
	}

	// Verifica si la Id esta libre
	if (NF_SPR256PAL[id].available) {
		NF_Error(110, "Sprite PAL", id);
	}

	// Verifica si te has salido de rango (Paleta)
	if ((slot < 0) || (slot > 15)) {
		NF_Error(106, "Sprite Palette Slot", 15);
	}

	// Copia la paleta a la VRAM, segun la pantalla y el Slot
	u32 address = 0;
	if (screen == 0) {
		address = (0x06890000) + (slot << 9);			// Calcula donde guardaras la paleta
		vramSetBankF(VRAM_F_LCD);						// Bloquea el banco F para escribir las paletas
		NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);	// Copia la paleta al banco F
		vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);		// Pon el banco F en modo paleta extendida
	} else {
		address = (0x068A0000) + (slot << 9);			// Calcula donde guardaras la paleta
		vramSetBankI(VRAM_I_LCD);						// Bloquea el banco I para escribir las paletas
		NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);	// Copia la paleta al banco I
		vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);	// Pon el banco I en modo paleta extendida
	}

	NF_SPRPALSLOT[screen][slot].inuse = true;			// Marca el SLOT de paleta como en uso
	NF_SPRPALSLOT[screen][slot].ramslot = id;			// Guarda el slot de RAM donde esta la paleta original

}



// Funcion NF_CreateSprite();
void NF_CreateSprite(u8 screen, u8 id, u16 gfx, u8 pal, s16 x, s16 y) {

	// Verifica el rango de Id's de Sprites
	if ((id < 0) || (id > 127)) {
		NF_Error(106, "Sprite", 127);
	}

	// Verifica el rango de Id's de Gfx
	if ((gfx < 0) || (gfx > 127)) {
		NF_Error(106, "Sprite GFX", 127);
	}

	// Verifica si esta el Gfx en VRAM
	if (!NF_SPR256VRAM[screen][gfx].inuse) {
		NF_Error(111, "Sprite GFX", gfx);
	}

	// Verifica el rango de slots de paletas
	if ((pal < 0) || (pal > 15)) {
		NF_Error(106, "Sprite Palette Slot", 15);
	}

	// Verifica si esta la paleta en VRAM
	if (!NF_SPRPALSLOT[screen][pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// // Informa al array de OAM del Id
	NF_SPRITEOAM[screen][id].index = id;

	// Informa al array de OAM del Gfx a usar
	NF_SPRITEOAM[screen][id].gfx = (u32*)NF_SPR256VRAM[screen][gfx].address;

	// Informa al array de OAM de la Paleta a usar
	NF_SPRITEOAM[screen][id].pal = pal;

	// Informa al array de OAM de la coordenada X
	NF_SPRITEOAM[screen][id].x = x;

	// Informa al array de OAM de la coordenada X
	NF_SPRITEOAM[screen][id].y = y;

	// Informa al array de OAM del numero de colores
	NF_SPRITEOAM[screen][id].color = SpriteColorFormat_256Color;

	// Informa al array de OAM de que debe mostrar el sprite
	NF_SPRITEOAM[screen][id].hide = false;

	// Informa al array de OAM del Id de Gfx usado
	NF_SPRITEOAM[screen][id].gfxid = gfx;

	// Informa al array de OAM de que el sprite se ha creado
	NF_SPRITEOAM[screen][id].created = true;

	// Informa al array de OAM del tamaño
	if ((NF_SPR256VRAM[screen][gfx].width == 8) && (NF_SPR256VRAM[screen][gfx].height == 8)) {	// 8x8
		if (NF_SPRVRAM[screen].max != 131072) {		// En modo 1D_128, este tamaño es ilegal
			NF_SPRITEOAM[screen][id].size = SpriteSize_8x8;
		} else {
			NF_Error(120, NULL, id);
		}
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 16) && (NF_SPR256VRAM[screen][gfx].height == 16)) {	// 16x16
		NF_SPRITEOAM[screen][id].size = SpriteSize_16x16;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 32) && (NF_SPR256VRAM[screen][gfx].height == 32)) {	// 32x32
		NF_SPRITEOAM[screen][id].size = SpriteSize_32x32;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 64) && (NF_SPR256VRAM[screen][gfx].height == 64)) {	// 64x64
		NF_SPRITEOAM[screen][id].size = SpriteSize_64x64;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 16) && (NF_SPR256VRAM[screen][gfx].height == 8)) {	// 16x8
		NF_SPRITEOAM[screen][id].size = SpriteSize_16x8;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 32) && (NF_SPR256VRAM[screen][gfx].height == 8)) {	// 32x8
		NF_SPRITEOAM[screen][id].size = SpriteSize_32x8;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 32) && (NF_SPR256VRAM[screen][gfx].height == 16)) {	// 32x16
		NF_SPRITEOAM[screen][id].size = SpriteSize_32x16;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 64) && (NF_SPR256VRAM[screen][gfx].height == 32)) {	// 64x32
		NF_SPRITEOAM[screen][id].size = SpriteSize_64x32;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 8) && (NF_SPR256VRAM[screen][gfx].height == 16)) {	// 8x16
		NF_SPRITEOAM[screen][id].size = SpriteSize_8x16;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 8) && (NF_SPR256VRAM[screen][gfx].height == 32)) {	// 8x32
		NF_SPRITEOAM[screen][id].size = SpriteSize_8x32;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 16) && (NF_SPR256VRAM[screen][gfx].height == 32)) {	// 16x32
		NF_SPRITEOAM[screen][id].size = SpriteSize_16x32;
	}
	if ((NF_SPR256VRAM[screen][gfx].width == 32) && (NF_SPR256VRAM[screen][gfx].height == 64)) {	// 32x64
		NF_SPRITEOAM[screen][id].size = SpriteSize_32x64;
	}

	// Informa al array de OAM del ultimo frame del Sprite
	NF_SPRITEOAM[screen][id].lastframe = NF_SPR256VRAM[screen][gfx].lastframe;

	// Informa al array de OAM del tamaño del frame del Sprite (en bytes)
	NF_SPRITEOAM[screen][id].framesize = NF_SPR256VRAM[screen][gfx].framesize;

	// Por defecto, el primer frame (0)
	NF_SPRITEOAM[screen][id].frame = 0;

}



// Funcion NF_DeleteSprite();
void NF_DeleteSprite(u8 screen, u8 id) {

	// Verifica el rango de Id's de Sprites
	if ((id < 0) || (id > 127)) {
		NF_Error(106, "Sprite", 127);
	}

	// Verifica si el Sprite esta creado
	if (!NF_SPRITEOAM[screen][id].created) {
		char text[4];
		sprintf(text, "%d", screen);
		NF_Error(112, text, id);
	}

	// Reinicia todas las variables de ese Sprite
	NF_SPRITEOAM[screen][id].index = id;	// Numero de Sprite
	NF_SPRITEOAM[screen][id].x = 0;			// Coordenada X del Sprite (0 por defecto)
	NF_SPRITEOAM[screen][id].y = 0;			// Coordenada Y del Sprite (0 por defecto)
	NF_SPRITEOAM[screen][id].layer = 0;		// Prioridad en las capas (0 por defecto)
	NF_SPRITEOAM[screen][id].pal = 0;		// Paleta que usaras (0 por defecto)
	NF_SPRITEOAM[screen][id].size = SpriteSize_8x8;					// Tamaño del Sprite (macro) (8x8 por defecto)
	NF_SPRITEOAM[screen][id].color = SpriteColorFormat_256Color;	// Modo de color (macro) (256 colores)
	NF_SPRITEOAM[screen][id].gfx = NULL;			// Puntero al grafico usado
	NF_SPRITEOAM[screen][id].rot = -1;				// Id de rotacion (-1 ninguno) (0 - 31 Id de rotacion)
	NF_SPRITEOAM[screen][id].doublesize = false;	// Usar el "double size" al rotar ? ("NO" por defecto)
	NF_SPRITEOAM[screen][id].hide = true;			// Ocultar el Sprite ("SI" por defecto)
	NF_SPRITEOAM[screen][id].hflip = false;			// Volteado Horizontal ("NO" por defecto)
	NF_SPRITEOAM[screen][id].vflip = false;			// Volteado Vertical ("NO" por defecto)
	NF_SPRITEOAM[screen][id].mosaic = false;		// Mosaico ("NO" por defecto)
	NF_SPRITEOAM[screen][id].gfxid = 0;				// Numero de Gfx usado
	NF_SPRITEOAM[screen][id].frame = 0;				// Frame actual
	NF_SPRITEOAM[screen][id].framesize = 0;			// Tamaño del frame (en bytes)
	NF_SPRITEOAM[screen][id].lastframe = 0;			// Ultimo frame
	NF_SPRITEOAM[screen][id].created = false;		// Esta creado este sprite ?

}



// Funcion NF_SpriteOamSet();
void NF_SpriteOamSet(u8 screen) {

	u8 n  = 0;	// Variable de uso general

	if (screen == 0) {

		for (n = 0; n < 128; n ++) {
			oamSet(&oamMain,							// OAM pantalla superior (Main, 0)
				NF_SPRITEOAM[screen][n].index,			// Numero de Sprite
				NF_SPRITEOAM[screen][n].x,				// Coordenada X del Sprite
				NF_SPRITEOAM[screen][n].y,				// Coordenada Y del Sprite
				NF_SPRITEOAM[screen][n].layer,			// Prioridad en las capas
				NF_SPRITEOAM[screen][n].pal,			// Paleta que usaras
				NF_SPRITEOAM[screen][n].size,			// Tamaño del Sprite (macro)
				NF_SPRITEOAM[screen][n].color,			// Modo de color (macro)
				NF_SPRITEOAM[screen][n].gfx,			// Puntero al grafico usado
				NF_SPRITEOAM[screen][n].rot,			// Valor de la rotacion
				NF_SPRITEOAM[screen][n].doublesize,		// Usar el "double size" al rotar ?
				NF_SPRITEOAM[screen][n].hide,			// Ocultar el Sprite
				NF_SPRITEOAM[screen][n].hflip,			// Volteado Horizontal
				NF_SPRITEOAM[screen][n].vflip,			// Volteado Vertical
				NF_SPRITEOAM[screen][n].mosaic);		// Mosaico
		}

	} else {

		for (n = 0; n < 128; n ++) {
			oamSet(&oamSub,								// OAM pantalla superior (Main, 0)
				NF_SPRITEOAM[screen][n].index,			// Numero de Sprite
				NF_SPRITEOAM[screen][n].x,				// Coordenada X del Sprite
				NF_SPRITEOAM[screen][n].y,				// Coordenada Y del Sprite
				NF_SPRITEOAM[screen][n].layer,			// Prioridad en las capas
				NF_SPRITEOAM[screen][n].pal,			// Paleta que usaras
				NF_SPRITEOAM[screen][n].size,			// Tamaño del Sprite (macro)
				NF_SPRITEOAM[screen][n].color,			// Modo de color (macro)
				NF_SPRITEOAM[screen][n].gfx,			// Puntero al grafico usado
				NF_SPRITEOAM[screen][n].rot,			// Valor de la rotacion
				NF_SPRITEOAM[screen][n].doublesize,		// Usar el "double size" al rotar ?
				NF_SPRITEOAM[screen][n].hide,			// Ocultar el Sprite
				NF_SPRITEOAM[screen][n].hflip,			// Volteado Horizontal
				NF_SPRITEOAM[screen][n].vflip,			// Volteado Vertical
				NF_SPRITEOAM[screen][n].mosaic);		// Mosaico
		}

	}

}



// Funcion NF_SpriteSetPalColor();
void NF_SpriteSetPalColor(u8 screen, u8 pal, u8 number, u8 r, u8 g, u8 b) {

	// Verifica si esta la paleta en VRAM
	if (!NF_SPRPALSLOT[screen][pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));
	// Direccion en VRAM
	u32 address = 0;

	// Modifica la paleta
	if (screen == 0) {
		address = (0x06890000) + (pal << 9) + (number << 1);	// Calcula donde guardaras el color de la paleta
		vramSetBankF(VRAM_F_LCD);								// Bloquea el banco F para escribir las paletas
		*((u16*)address) = rgb;									// Cambia el color
		vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);				// Pon el banco F en modo paleta extendida
	} else {
		address = (0x068A0000) + (pal << 9) + (number << 1);	// Calcula donde guardaras el color de la paleta
		vramSetBankI(VRAM_I_LCD);								// Bloquea el banco I para escribir las paletas
		*((u16*)address) = rgb;									// Cambia el color
		vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);			// Pon el banco I en modo paleta extendida
	}

}



// Funcion NF_SpriteEditPalColor();
void NF_SpriteEditPalColor(u8 screen, u8 pal, u8 number, u8 r, u8 g, u8 b) {

	// Verifica si esta la paleta en VRAM
	if (!NF_SPRPALSLOT[screen][pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));

	// Calcula los valores para el HI-Byte y el LO-Byte
	u8 hibyte = ((rgb >> 8) & 0xff);
	u8 lobyte = (rgb & 0xff);

	// Graba los bytes
	*(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + (number << 1)) = lobyte;
	*(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + ((number << 1) + 1)) = hibyte;

}



// Funcion 	NF_SpriteUpdatePalette();
void NF_SpriteUpdatePalette(u8 screen, u8 pal) {

	// Verifica si esta la paleta en VRAM
	if (!NF_SPRPALSLOT[screen][pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Direccion en VRAM
	u32 address = 0;

	// Obten el slot donde esta la paleta en RAM
	u8 slot = NF_SPRPALSLOT[screen][pal].ramslot;

	// Actualiza la paleta en VRAM
	if (screen == 0) {
		address = (0x06890000) + (pal << 9);			// Calcula donde guardaras la paleta
		vramSetBankF(VRAM_F_LCD);					// Bloquea el banco F para escribir las paletas
		NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);	// Copia la paleta al banco F
		vramSetBankF(VRAM_F_SPRITE_EXT_PALETTE);	// Pon el banco F en modo paleta extendida
	} else {
		address = (0x068A0000) + (pal << 9);			// Calcula donde guardaras la paleta
		vramSetBankI(VRAM_I_LCD);					// Bloquea el banco I para escribir las paletas
		NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);	// Copia la paleta al banco I
		vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);	// Pon el banco I en modo paleta extendida
	}

}



// Funcion NF_SpriteGetPalColor();
void NF_SpriteGetPalColor(u8 screen, u8 pal, u8 number, u8* r, u8* g, u8* b) {

	// Verifica si esta la paleta en VRAM
	if (!NF_SPRPALSLOT[screen][pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Obten los bytes
	u8 lobyte = *(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + (number << 1));
	u8 hibyte = *(NF_BUFFER_SPR256PAL[NF_SPRPALSLOT[screen][pal].ramslot] + ((number << 1) + 1));

	// Calcula el RGB (compuesto)
	u16 rgb = ((hibyte << 8) | lobyte);

	// Calcula los RGB
	*r = (rgb & 0x1F);
	*g = ((rgb >> 5) & 0x1F);
	*b = ((rgb >> 10) & 0x1F);

}
