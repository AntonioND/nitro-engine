
// NightFox LIB - Funciones 2D comunes
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413



// Includes devKitPro
#include <nds.h>

// Includes C
#include <stdio.h>
#include <string.h>

// Includes propios
#include "nf_basic.h"
#include "nf_3d.h"
#include "nf_sprite3d.h"
#include "nf_sprite256.h"





//////////////////////////////////
// Defines y variables globales //
//////////////////////////////////

// Estructura de control de los sprites 3d
NF_TYPE_3DSPRITE_INFO NF_3DSPRITE[NF_3DSPRITES];

// Estructura de control de las texturas en VRAM
NF_TYPE_TEX256VRAM_INFO NF_TEX256VRAM[NF_3DSPRITES];

// Estructura de control de las paletas en VRAM
NF_TYPE_3DSPRPALSLOT_INFO NF_TEXPALSLOT[32];

// Estructura de control de la VRAM de texturas
NF_TYPE_TEXVRAM_INFO NF_TEXVRAM;

// Define la estructura de control de los sprites 3d creados
NF_TYPE_CREATED_3DSPRITE_INFO NF_CREATED_3DSPRITE;





// Funcion NF_Init3dSpriteSys();
void NF_Init3dSpriteSys(void) {

	// Variables locales
	u16 n = 0;

	// Inicializaciones
	for (n = 0; n < NF_3DSPRITES; n ++) {

		// Inicializa las estructuras de control de los sprites
		NF_3DSPRITE[n].x = 0;				// Coordenada X
		NF_3DSPRITE[n].y = 0;				// Coordenada Y
		NF_3DSPRITE[n].z = 0;				// Coordenada Z
		NF_3DSPRITE[n].rx = 0;				// Rotacion Eje X
		NF_3DSPRITE[n].ry = 0;				// Rotacion Eje Y
		NF_3DSPRITE[n].rz = 0;				// Rotacion Eje Z
		NF_3DSPRITE[n].rot = false;			// Rotacion en uso
		NF_3DSPRITE[n].sx = (64 << 6);		// Escala X
		NF_3DSPRITE[n].sy = (64 << 6);		// Escala X
		NF_3DSPRITE[n].scale = false;		// Escalado en uso
		NF_3DSPRITE[n].width = 0;			// Ancho del sprite
		NF_3DSPRITE[n].height = 0;			// Altura del sprite
		NF_3DSPRITE[n].inuse = false;		// Esta este sprite en uso?
		NF_3DSPRITE[n].show = false;		// Debe mostrarse?
		NF_3DSPRITE[n].gfx_tex_format = 0;	// Guarda el formato de la textura
		NF_3DSPRITE[n].gfx = 0;				// Direccion en VRAM del GFX usado
		NF_3DSPRITE[n].gfxid = 0;			// Numero de Gfx usado
		NF_3DSPRITE[n].frame = 0;			// Frame actual
		NF_3DSPRITE[n].newframe = 0;		// Frame al que cambiar
		NF_3DSPRITE[n].framesize = 0;		// Tamaño del frame (en bytes)
		NF_3DSPRITE[n].lastframe = 0;		// Ultimo frame
		NF_3DSPRITE[n].gfx_pal_format = 0;	// Guarda el formato de la paleta
		NF_3DSPRITE[n].pal = 0;				// Direccion en VRAM de la paleta usada
		NF_3DSPRITE[n].palid = 0;			// Numero de paleta usada
		NF_3DSPRITE[n].prio = 0;			// Prioridad del Sprtie
		NF_3DSPRITE[n].poly_id = 0;			// Identificador unico para el Alpha (0 por defecto, 63 prohibido)
		NF_3DSPRITE[n].alpha = 31;			// Nivel de alpha (0 - 31) (31 por defecto)

		// Inicializa las estructuras de control de la VRAM de texturas
		NF_TEX256VRAM[n].size = 0;				// Tamaño (en bytes) del Gfx
		NF_TEX256VRAM[n].width = 0;				// Ancho del Gfx
		NF_TEX256VRAM[n].height = 0;			// Altura del Gfx
		NF_TEX256VRAM[n].address = 0;			// Posicion en la VRAM
		NF_TEX256VRAM[n].ramid = 0;				// Numero de Slot en RAM del que provienes
		NF_TEX256VRAM[n].framesize = 0;			// Tamaño del frame (en bytes)
		NF_TEX256VRAM[n].lastframe = 0;			// Ultimo frame
		NF_TEX256VRAM[n].keepframes = false;	// Si es un Sprite animado, debes de mantener los frames en RAM ?
		NF_TEX256VRAM[n].inuse = false;			// Esta en uso ?

		// Inicializa las esctructuras de control de los sprites creados
		NF_CREATED_3DSPRITE.id[n] = 0;
		NF_CREATED_3DSPRITE.bck[n] = 0;

	}

	// Inicializa la estructura de datos de la VRAM de Sprites
	NF_TEXVRAM.free = 131072;		// Memoria VRAM libre (128kb)
	NF_TEXVRAM.last = 0;			// Ultima posicion usada
	NF_TEXVRAM.deleted = 0;			// Ningun Gfx borrado
	NF_TEXVRAM.fragmented = 0;		// Memoria VRAM fragmentada
	NF_TEXVRAM.inarow = 131072;		// Memoria VRAM contigua
	for (n = 0; n < NF_3DSPRITES; n ++) {
		NF_TEXVRAM.pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		NF_TEXVRAM.size[n] = 0;		// Tamaño del bloque libre para reusar
	}

	// Inicializa los datos de las paletas
	for (n = 0; n < 32; n ++) {
		NF_TEXPALSLOT[n].inuse = false;
		NF_TEXPALSLOT[n].ramslot = 0;
	}

	// Inicializa el numero de sprites creados
	NF_CREATED_3DSPRITE.total = 0;

	// VRAM para TEXTURAS en el Banco B
	vramSetBankB(VRAM_B_LCD);					// Bloquea el banco para la escritura
	memset((void*)0x06820000, 0, 131072);		// Borra el contenido del banco B
	NF_TEXVRAM.next = (0x06820000);				// Guarda la primera posicion de VRAM para Gfx
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

	// VRAM para PALETAS
	vramSetBankF(VRAM_F_LCD);					// Bloquea el banco para la escritura
	memset((void*)0x06890000, 0, 16384);		// Borra el contenido del banco F
	vramSetBankF(VRAM_F_TEX_PALETTE);			// Banco F de la VRAM para paletas extendidas (Texturas) (16kb)

	// Inicializa el OpenGL
	NF_InitOpenGL();

}



// Funcion NF_Vram3dSpriteGfx();
void NF_Vram3dSpriteGfx(u16 ram, u16 vram, bool keepframes) {

	// Verifica el rango de Id's de RAM
	if ((ram < 0) || (ram >= NF_SLOTS_SPR256GFX)) {
		NF_Error(106, "Sprite GFX", (NF_SLOTS_SPR256GFX - 1));
	}

	// Verifica si slot de RAM esta vacio
	if (NF_SPR256GFX[ram].available) {
		NF_Error(110, "Sprite GFX", ram);
	}

	// Verifica el rango de Id's de VRAM
	if ((vram < 0) || (vram >= NF_3DSPRITES)) {
		NF_Error(106, "VRAM GFX", (NF_3DSPRITES - 1));
	}

	// Verifica si el slot de VRAM esta libre
	if (NF_TEX256VRAM[vram].inuse) {
		NF_Error(109, "VRAM", vram);
	}

	// Verifica que la textura tengo un tamaño valido
	if (
		(NF_GetTextureSize(NF_SPR256GFX[ram].width) == 255)
		||
		(NF_GetTextureSize(NF_SPR256GFX[ram].height) == 255)
		) {
			NF_Error(119, NULL, ram);
	}

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Variables de uso general
	s16 n = 0;				// General
	s16 id = 255;			// Id del posible bloque libre
	s16 last_reuse = 0;		// Nº del ultimo bloque reusable
	u32 gfxsize = 0;		// Tamaño de los datos que se copiaran
	u32 size = 0;			// Diferencia de tamaños entre bloque libre y datos
	u16 width = 0;			// Calculo de las medidas
	u16 height = 0;
	bool organize = true;	// Se debe de reorganizar el array de bloques libres ?

	// Auto calcula el tamaño de 1 frame
	width = (NF_SPR256GFX[ram].width >> 3);		// (width / 8)
	height = (NF_SPR256GFX[ram].height >> 3);	// (height / 8)
	NF_TEX256VRAM[vram].framesize = ((width * height) << 6);	// ((width * height) * 64)
	// Auto calcula el ultimo frame de la animacion
	NF_TEX256VRAM[vram].lastframe = ((int)(NF_SPR256GFX[ram].size / NF_TEX256VRAM[vram].framesize)) - 1;
	NF_TEX256VRAM[vram].inuse = true;						// Slot ocupado

	// Calcula el tamaño del grafico a copiar segun si debes o no copiar todos los frames
	if (keepframes) {	// Si debes de mantener los frames en RAM, solo copia el primero
		gfxsize = NF_TEX256VRAM[vram].framesize;
	} else {			// Si no, copialos todos
		gfxsize = NF_SPR256GFX[ram].size;
	}

	// Actualiza la VRAM disponible
	NF_TEXVRAM.free -= gfxsize;

	// Si no hay suficiente VRAM, error
	if (NF_TEXVRAM.free < 0) {
		NF_Error(113, "Sprites", gfxsize);
	}

	// Si hay que aprovechar algun bloque borrado... (tamaño identico, preferente)
	if (NF_TEXVRAM.deleted > 0) {
		// Busca un bloque vacio del tamaño identico
		for (n = 0; n < NF_TEXVRAM.deleted; n ++) {
			if (NF_TEXVRAM.size[n] == gfxsize) {		// Si el bloque tiene el tamaño suficiente
				id = n;		// Guarda la Id
				n = NF_TEXVRAM.deleted;	// y sal
			}
		}
		// Si no habia ningun bloque de tamaño identico, busca el mas parecido (produce fragmentacion)
		if (id != 255) {
			for (n = 0; n < NF_TEXVRAM.deleted; n ++) {
				if (NF_TEXVRAM.size[n] > gfxsize) {		// Si el bloque tiene el tamaño suficiente
					id = n;		// Guarda la Id
					n = NF_TEXVRAM.deleted;	// y sal
				}
			}
		}
	} 
	
	// Si hay algun bloque borrado libre del tamaño suficiente...
	if (id != 255) {

		// Transfiere el grafico a la VRAM
		NF_DmaMemCopy((void*)NF_TEXVRAM.pos[id], NF_BUFFER_SPR256GFX[ram], gfxsize);
		// Guarda el puntero donde lo has almacenado
		NF_TEX256VRAM[vram].address = NF_TEXVRAM.pos[id];

		// Si no has usado todo el tamaño, deja constancia
		if (gfxsize < NF_TEXVRAM.size[id]) {

			// Calcula el tamaño del nuevo bloque libre
			size = NF_TEXVRAM.size[id] - gfxsize;
			// Actualiza los datos
			NF_TEXVRAM.pos[id] += gfxsize;			// Nueva direccion
			NF_TEXVRAM.size[id] = size;				// Nuevo tamaño
			NF_TEXVRAM.fragmented -= gfxsize;		// Actualiza el contador de VRAM fragmentada
			organize = false;	// No se debe de reorganizar el array de bloques

		} else {	// Si has usado todo el tamaño, deja constancia

			NF_TEXVRAM.fragmented -= NF_TEXVRAM.size[id];	// Actualiza el contador de VRAM fragmentada

		}

		// Se tiene que reorganizar el array de bloques libres ?
		if (organize) {
			last_reuse = (NF_TEXVRAM.deleted - 1);
			if (
			(last_reuse > 0)	// Si hay mas de un bloque borrado
			&&
			(id != last_reuse)	// Y no es la ultima posicion
			) {
				// Coloca los valores de la ultima posicion en esta
				NF_TEXVRAM.pos[id] = NF_TEXVRAM.pos[last_reuse];		// Nueva direccion
				NF_TEXVRAM.size[id] = NF_TEXVRAM.size[last_reuse];		// Nuevo tamaño
			}
			NF_TEXVRAM.deleted --;		// Actualiza el contador de bloques libres, borrando el ultimo registro
		}

	} else {	// Si no habia ningun bloque borrado o con el tamaño suficiente, colacalo al final de la VRAM ocupada

		// Actualiza la VRAM contigua disponible (mayor bloque libre al final)
		NF_TEXVRAM.inarow -= gfxsize;

		// Si no hay suficiente VRAM (contigua), error
		if (NF_TEXVRAM.inarow < 0) {
			NF_Error(113, "Sprites", gfxsize);
		}

		// Transfiere el grafico a la VRAM
		NF_DmaMemCopy((void*)NF_TEXVRAM.next, NF_BUFFER_SPR256GFX[ram], gfxsize);
		// Guarda el puntero donde lo has almacenado
		NF_TEX256VRAM[vram].address = NF_TEXVRAM.next;
		// Guarda la direccion actual como la ultima usada
		NF_TEXVRAM.last = NF_TEXVRAM.next;
		// Calcula la siguiente posicion libre
		NF_TEXVRAM.next += gfxsize;

	}

	// Guarda los datos del Gfx que se copiara a la VRAM.
	NF_TEX256VRAM[vram].size = gfxsize;						// Tamaño en bytes de los datos copiados
	NF_TEX256VRAM[vram].width = NF_SPR256GFX[ram].width;	// Alto (px)
	NF_TEX256VRAM[vram].height = NF_SPR256GFX[ram].height;	// Ancho (px)
	NF_TEX256VRAM[vram].ramid = ram;						// Slot RAM de origen
	NF_TEX256VRAM[vram].keepframes = keepframes;			// Debes guardar los frames en RAM o copiarlos a la VRAM?

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion NF_Free3dSpriteGfx();
void NF_Free3dSpriteGfx(u16 id) {

	// Verifica si hay un grafico cargado en esa Id.
	if (!NF_TEX256VRAM[id].inuse) {
		NF_Error(110, "Sprite Gfx", id);
	}

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Borra el Gfx de la VRAM (pon a 0 todos los Bytes)
	memset((void*)NF_TEX256VRAM[id].address, 0, NF_TEX256VRAM[id].size);

	// Actualiza la cantidad de VRAM disponible
	NF_TEXVRAM.free += NF_TEX256VRAM[id].size;

	// Guarda la posicion y tamaño del bloque borrado para su reutilizacion
	NF_TEXVRAM.pos[NF_TEXVRAM.deleted] = NF_TEX256VRAM[id].address;
	NF_TEXVRAM.size[NF_TEXVRAM.deleted] = NF_TEX256VRAM[id].size;

	// Incrementa en contador de bloques borrados
	NF_TEXVRAM.deleted ++;

	// Incrementa el contador de memoria fragmentada
	NF_TEXVRAM.fragmented += NF_TEX256VRAM[id].size;

	// Reinicia los datos de esta Id. de gfx
	NF_TEX256VRAM[id].size = 0;			// Tamaño en bytes
	NF_TEX256VRAM[id].width = 0;		// Alto (px)
	NF_TEX256VRAM[id].height = 0;		// Ancho (px)
	NF_TEX256VRAM[id].address = 0;		// Puntero en VRAM
	NF_TEX256VRAM[id].framesize = 0;	// Tamaño del frame (en bytes)
	NF_TEX256VRAM[id].lastframe = 0;	// Ultimo frame
	NF_TEX256VRAM[id].inuse = false;

	// Debes desfragmentar la VRAM
	if (NF_TEXVRAM.fragmented >= (NF_TEXVRAM.inarow >> 1)) NF_Vram3dSpriteGfxDefrag();

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion NF_Vram3dSpriteGfxDefrag();
void NF_Vram3dSpriteGfxDefrag(void) {

	// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
	vramSetBankB(VRAM_B_LCD);

	// Calcula la VRAM en uso y crea un buffer para guardarla
	u32 used_vram = ((131072 - NF_TEXVRAM.free) + 1);
	char* buffer;
	buffer = (char*) calloc (used_vram, sizeof(char));
	if (buffer == NULL) {		// Si no hay suficiente RAM libre
		NF_Error(102, NULL, used_vram);
	}

	char* address[NF_3DSPRITES];	// Guarda la direccion en RAM
	u32 size[NF_3DSPRITES];			// Guarda el tamaño
	u32 ram = 0;					// Puntero inicial de RAM
	u16 n = 0;						// Variable General
	u16 x_size = 0;					// Formato de la textura
	u16 y_size = 0;
	u32 gfx_address = 0;

	// Copia los datos de la VRAM a la RAM
	for (n = 0; n < NF_3DSPRITES; n ++) {
		// Si esta en uso
		if (NF_TEX256VRAM[n].inuse) {
			// Copia el Gfx a la RAM
			address[n] = (buffer + ram);		// Calcula el puntero
			size[n] = NF_TEX256VRAM[n].size;		// Almacena el tamaño
			NF_DmaMemCopy(address[n], (void*)NF_TEX256VRAM[n].address, size[n]);	// Copialo a la VRAM
			ram += size[n];		// Siguiente posicion en RAM (relativa)
		}
	}

	// Inicializa la estructura de datos de la VRAM de Sprites
	NF_TEXVRAM.free = 131072;		// Memoria VRAM libre (128kb)
	NF_TEXVRAM.last = 0;			// Ultima posicion usada
	NF_TEXVRAM.deleted = 0;			// Ningun Gfx borrado
	NF_TEXVRAM.fragmented = 0;		// Memoria VRAM fragmentada
	NF_TEXVRAM.inarow = 131072;		// Memoria VRAM contigua
	for (n = 0; n < NF_3DSPRITES; n ++) {
		NF_TEXVRAM.pos[n] = 0;		// Posicion en VRAM para reusar despues de un borrado
		NF_TEXVRAM.size[n] = 0;		// Tamaño del bloque libre para reusar
	}
	// Aplica la direccion de inicio de la VRAM
	NF_TEXVRAM.next = (0x06820000);

	// Ahora, copia de nuevo los datos a la VRAM, pero alineados
	for (n = 0; n < NF_3DSPRITES; n ++) {
		// Si esta en uso
		if (NF_TEX256VRAM[n].inuse) {
			NF_DmaMemCopy((void*)NF_TEXVRAM.next, address[n], size[n]);		// Vuelve a colocar la el Gfx en VRAM
			NF_TEX256VRAM[n].address = NF_TEXVRAM.next;		// Guarda la nueva posicion en VRAM
			NF_TEXVRAM.free -= size[n];		// Ram libre
			NF_TEXVRAM.inarow -= size[n];	// Ram libre en bloque
			NF_TEXVRAM.last = NF_TEXVRAM.next;	// Guarda la posicion como ultima usada
			NF_TEXVRAM.next += size[n];		// Y calcula la siguiente posicion a escribir
		}
	}

	// Realinea los Sprites con sus graficos
	for (n = 0; n < NF_3DSPRITES; n ++) {
		if (NF_3DSPRITE[n].inuse) {
			// Asigna la nueva direccion de memoria
			NF_3DSPRITE[n].gfx = NF_TEX256VRAM[NF_3DSPRITE[n].gfxid].address;
			if (NF_TEX256VRAM[NF_3DSPRITE[n].gfxid].keepframes) {
				gfx_address = NF_3DSPRITE[n].gfx;
			} else {
				gfx_address = (NF_3DSPRITE[n].gfx + (NF_3DSPRITE[n].framesize * NF_3DSPRITE[n].frame));
			}
			// Recalcula el formato de la textura
			x_size = NF_GetTextureSize(NF_3DSPRITE[n].width);
			y_size = NF_GetTextureSize(NF_3DSPRITE[n].height);
			NF_3DSPRITE[n].gfx_tex_format = (((gfx_address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);
		}
	}

	// Vacia el buffer
	free(buffer);
	buffer = NULL;

	// Restaura el banco de VRAM en modo Textura
	vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

}



// Funcion NF_Vram3dSpritePal();
void NF_Vram3dSpritePal(u8 id, u8 slot) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_SPR256PAL)) {
		NF_Error(106, "Sprite PAL", NF_SLOTS_SPR256PAL);
	}

	// Verifica si la Id esta libre
	if (NF_SPR256PAL[id].available) {
		NF_Error(110, "Sprite PAL", id);
	}

	// Verifica si te has salido de rango (Paleta)
	if ((slot < 0) || (slot > 31)) {
		NF_Error(106, "Sprite Palette Slot", 31);
	}

	// Copia la paleta a la VRAM, segun el Slot
	u32 address = (0x06890000) + (slot << 9);			// Calcula donde guardaras la paleta
	vramSetBankF(VRAM_F_LCD);			// Bloquea el banco F para escribir las paletas
	NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[id], NF_SPR256PAL[id].size);	// Copia la paleta al banco F
	vramSetBankF(VRAM_F_TEX_PALETTE);	// Banco F de la VRAM para paletas extendidas (Texturas)

	// Guarda los parametros
	NF_TEXPALSLOT[slot].inuse = true;			// Marca el SLOT de paleta como en uso
	NF_TEXPALSLOT[slot].ramslot = id;			// Guarda el slot de RAM donde esta la paleta original

}



// Funcion NF_Create3dSprite();
void NF_Create3dSprite(u16 id, u16 gfx, u16 pal, s16 x, s16 y) {

	// Verifica el rango de Id's de Sprites
	if ((id < 0) || (id > (NF_3DSPRITES - 1))) {
		NF_Error(106, "3D Sprite", (NF_3DSPRITES - 1));
	}

	// Verifica el rango de Id's de Gfx
	if ((gfx < 0) || (gfx > (NF_3DSPRITES - 1))) {
		NF_Error(106, "3D Sprite GFX", (NF_3DSPRITES - 1));
	}

	// Verifica si esta el Gfx en VRAM
	if (!NF_TEX256VRAM[gfx].inuse) {
		NF_Error(111, "3D Sprite GFX", gfx);
	}

	// Verifica el rango de slots de paletas
	if ((pal < 0) || (pal > 31)) {
		NF_Error(106, "3D Sprite Palette Slot", 31);
	}

	// Verifica si esta la paleta en VRAM
	if (!NF_TEXPALSLOT[pal].inuse) {
		NF_Error(111, "3D Sprite PAL", pal);
	}

	// Calcula la direccion de la paleta y almacenala en la estructura del sprite
	u32 pal_address = (pal << 9);
	NF_3DSPRITE[id].pal = pal_address;	// Direccion en VRAM de la paleta usada (relativa a VRAM_F)
	NF_3DSPRITE[id].palid = pal;		// Numero de paleta usada
	NF_3DSPRITE[id].gfx_pal_format = (pal_address >> 4) & 0x1FFF;	// Formato de la paleta

	// Calcula la direccion de la textura y almacenala en la estructura del sprite
	u32 gfx_address = NF_TEX256VRAM[gfx].address;
	NF_3DSPRITE[id].gfx = gfx_address;		// Direccion en VRAM del GFX usado
	NF_3DSPRITE[id].gfxid = gfx;			// Numero de Gfx usado
	u16 x_size = NF_GetTextureSize(NF_TEX256VRAM[gfx].width);
	u16 y_size = NF_GetTextureSize(NF_TEX256VRAM[gfx].height);
	NF_3DSPRITE[id].gfx_tex_format = (((gfx_address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);

	// Guarda los demas parametros del sprite
	NF_3DSPRITE[id].x = x;
	NF_3DSPRITE[id].y = y;
	NF_3DSPRITE[id].z = 0;
	NF_3DSPRITE[id].width = NF_TEX256VRAM[gfx].width;
	NF_3DSPRITE[id].height = NF_TEX256VRAM[gfx].height;
	NF_3DSPRITE[id].framesize = NF_TEX256VRAM[gfx].framesize;
	NF_3DSPRITE[id].lastframe = NF_TEX256VRAM[gfx].lastframe;
	NF_3DSPRITE[id].inuse = true;
	NF_3DSPRITE[id].show = true;
	NF_3DSPRITE[id].prio = NF_CREATED_3DSPRITE.total;
	NF_3DSPRITE[id].poly_id = 0;
	NF_3DSPRITE[id].alpha = 31;

	// Ahora registra su creacion
	NF_CREATED_3DSPRITE.id[NF_CREATED_3DSPRITE.total] = id;
	NF_CREATED_3DSPRITE.total ++;
	
}



// Funcion NF_Delete3dSprite();
void NF_Delete3dSprite(u16 id) {

	// Verifica el rango de Id's de Sprites
	if ((id < 0) || (id > (NF_3DSPRITES - 1))) {
		NF_Error(106, "3D Sprite", (NF_3DSPRITES - 1));
	}

	// Verifica si el Sprite esta creado
	if (!NF_3DSPRITE[id].inuse) {
		NF_Error(112, "3D", id);
	}

	// Resetea los parametros del Sprite dado
	NF_3DSPRITE[id].x = 0;				// Coordenada X
	NF_3DSPRITE[id].y = 0;				// Coordenada Y
	NF_3DSPRITE[id].z = 0;				// Coordenada Z
	NF_3DSPRITE[id].rx = 0;				// Rotacion Eje X
	NF_3DSPRITE[id].ry = 0;				// Rotacion Eje Y
	NF_3DSPRITE[id].rz = 0;				// Rotacion Eje Z
	NF_3DSPRITE[id].rot = false;		// Rotacion en uso
	NF_3DSPRITE[id].sx = (64 << 6);		// Escala X
	NF_3DSPRITE[id].sy = (64 << 6);		// Escala X
	NF_3DSPRITE[id].scale = false;		// Escalado en uso
	NF_3DSPRITE[id].width = 0;			// Ancho del sprite
	NF_3DSPRITE[id].height = 0;			// Altura del sprite
	NF_3DSPRITE[id].inuse = false;		// Esta este sprite en uso?
	NF_3DSPRITE[id].show = false;		// Debe mostrarse?
	NF_3DSPRITE[id].gfx_tex_format = 0;	// Guarda el formato de la textura
	NF_3DSPRITE[id].gfx = 0;			// Direccion en VRAM del GFX usado
	NF_3DSPRITE[id].gfxid = 0;			// Numero de Gfx usado
	NF_3DSPRITE[id].frame = 0;			// Frame actual
	NF_3DSPRITE[id].newframe = 0;		// Frame al que cambiar
	NF_3DSPRITE[id].framesize = 0;		// Tamaño del frame (en bytes)
	NF_3DSPRITE[id].lastframe = 0;		// Ultimo frame
	NF_3DSPRITE[id].gfx_pal_format = 0;	// Guarda el formato de la paleta
	NF_3DSPRITE[id].pal = 0;			// Direccion en VRAM de la paleta usada
	NF_3DSPRITE[id].palid = 0;			// Numero de paleta usada
	NF_3DSPRITE[id].prio = 0;			// Prioridad del Sprtie
	NF_3DSPRITE[id].poly_id = 0;		// Identificador unico para el Alpha (0 por defecto, 63 prohibido)
	NF_3DSPRITE[id].alpha = 31;			// Nivel de alpha (0 - 31) (31 por defecto)

	// Ahora vamos a eliminar de la cola es sprite seleccionado
	u16 n1 = 0;
	u16 n2 = 0;
	// Si hay mas de 1 Sprite creado...
	if (NF_CREATED_3DSPRITE.total > 1) {
		for (n1 = 0; n1 < NF_CREATED_3DSPRITE.total; n1 ++) {
			// Si el Sprite no es el seleccionado, copialo al array temporal
			if (NF_CREATED_3DSPRITE.id[n1] != id) {
				NF_CREATED_3DSPRITE.bck[n2] = NF_CREATED_3DSPRITE.id[n1];
				n2 ++;
			}
		}
		// Marca que hay un sprite menos
		NF_CREATED_3DSPRITE.total --;

		// Ahora, copia el array modificado al principal
		for (n1 = 0; n1 < NF_CREATED_3DSPRITE.total; n1 ++) {
			NF_CREATED_3DSPRITE.id[n1] = NF_CREATED_3DSPRITE.bck[n1];
		}
	} else {
		// Si era el ultimo Sprite de la cola...
		NF_CREATED_3DSPRITE.id[0] = 0;
		NF_CREATED_3DSPRITE.total = 0;
	}
	
}



// Funcion NF_Sort3dSprites();
void NF_Sort3dSprites(void) {

	// Variables
	u16 n1 = 0;
	u16 n2 = 0;
	u16 id1 = 0;
	u16 id2 = 0;

	// Si hay mas de 1 item...
	if (NF_CREATED_3DSPRITE.total > 1) {

		// Ordena la cola de sprites creados
		for (n1 = 0; n1 < (NF_CREATED_3DSPRITE.total - 1); n1 ++) {
			for (n2 = (n1 + 1); n2 < NF_CREATED_3DSPRITE.total; n2 ++) {
				// Si es menor, reordenalo
				if (NF_CREATED_3DSPRITE.id[n2] < NF_CREATED_3DSPRITE.id[n1]) {
					id2 = NF_CREATED_3DSPRITE.id[n2];
					id1 = NF_CREATED_3DSPRITE.id[n1];
					// Guarda las IDs de ordenadas en la cola
					NF_CREATED_3DSPRITE.id[n1] = id2;
					NF_CREATED_3DSPRITE.id[n2] = id1;
					// Actualiza la informacion de prioridades del sprite
					NF_3DSPRITE[id2].prio = n1;
					NF_3DSPRITE[id1].prio = n2;
				}
			}
		}

	}

}



// Funcion NF_Set3dSpritePriority();
void NF_Set3dSpritePriority(u16 id, u16 prio) {

	// Variables
	u16 n = 0;
	u16 a = 0;
	u16 b = 0;

	// Realiza una copia de seguridad de la cola
	for (n = 0; n < NF_CREATED_3DSPRITE.total; n ++) {
		NF_CREATED_3DSPRITE.bck[n] = NF_CREATED_3DSPRITE.id[n];
	}

	// Coloca el Sprite en la prioridad indicada
	NF_CREATED_3DSPRITE.id[prio] = id;
	NF_3DSPRITE[id].prio = prio;
	for (n = 0; n < NF_CREATED_3DSPRITE.total; n ++) {
		// Si no coinciden ni Id ni prioridad...
		if ((a != prio) && (NF_CREATED_3DSPRITE.bck[b] != id)) {
			NF_CREATED_3DSPRITE.id[a] = NF_CREATED_3DSPRITE.bck[b];
			NF_3DSPRITE[NF_CREATED_3DSPRITE.id[a]].prio = a;
			a ++;
			b ++;
		} else {
			// Si coincide la prioridad, saltatela
			if (a == prio) {
				a ++;
			}
			// Si coincide la ID, saltatela
			if (NF_CREATED_3DSPRITE.bck[b] == id) {
				b ++;
			}
		}
	}

}



// Funcion NF_Swap3dSpritePriority();
void NF_Swap3dSpritePriority(u16 id_a, u16 id_b) {

	// Variables, obten las prioridades a cambiar
	u16 prio_a = NF_3DSPRITE[id_b].prio;
	u16 prio_b = NF_3DSPRITE[id_a].prio;

	// Asignalas en su nuevo orden
	NF_3DSPRITE[id_a].prio = prio_a;
	NF_3DSPRITE[id_b].prio = prio_b;

	// Y por ultimo, modifica la cola de sprites
	NF_CREATED_3DSPRITE.id[prio_a] = id_a;
	NF_CREATED_3DSPRITE.id[prio_b] = id_b;

}



// Funcion NF_Move3dSprite();
void NF_Move3dSprite(u16 id, s16 x, s16 y) {

	// Actualiza las coordenadas del Sprite
	NF_3DSPRITE[id].x = x;
	NF_3DSPRITE[id].y = y;

}



// Funcion NF_Show3dSprite();
void NF_Show3dSprite(u16 id, bool show) {

	// Actualiza el flag de visibilidad
	NF_3DSPRITE[id].show = show;

}



// Funcion NF_Set3dSpriteFrame();
void NF_Set3dSpriteFrame(u16 id, u16 frame) {

	// Verifica el rango de Id's de Sprites
	if ((id < 0) || (id > (NF_3DSPRITES - 1))) {
		NF_Error(106, "3D Sprite", (NF_3DSPRITES - 1));
	}

	// Verifica si el Sprite esta creado
	if (!NF_3DSPRITE[id].inuse) {
		NF_Error(112, "3D", id);
	}

	// Verifica el rango de frames del Sprite
	if (frame > NF_3DSPRITE[id].lastframe) {
		NF_Error(106, "Sprite frame", NF_3DSPRITE[id].lastframe);
	}

	// Si debes de copiar el nuevo frame desde la RAM a la VRAM...
	if (NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].keepframes) {

		// Marca para que se copie la nueva textura a la VRAM durante la actualizacion de los Sprites 3D
		NF_3DSPRITE[id].newframe = frame;

	} else {	// Si todos los frames ya estan en VRAM...

		// Calcula la direccion del Gfx del frame
		u32 gfx_address = (NF_3DSPRITE[id].gfx + (NF_3DSPRITE[id].framesize * frame));
		u16 x_size = NF_GetTextureSize(NF_3DSPRITE[id].width);
		u16 y_size = NF_GetTextureSize(NF_3DSPRITE[id].height);
		NF_3DSPRITE[id].gfx_tex_format = (((gfx_address >> 3) & 0xFFFF) | (x_size << 20) | (y_size << 23) | (GL_RGB256 << 26) | GL_TEXTURE_COLOR0_TRANSPARENT | TEXGEN_OFF);
		// Guarda el numero de frame actual
		NF_3DSPRITE[id].frame = frame;
		NF_3DSPRITE[id].newframe = frame;

	}

}



// Funcion NF_Draw3dSprites();
void NF_Draw3dSprites(void) {

	// Variables
	u16 n = 0;		// Uso general
	s16 x1 = 0;		// Vertices
	s16 x2 = 0;
	s16 y1 = 0;
	s16 y2 = 0;
	s16 x = 0;
	s16 y = 0;
	s16 z = 0;
	u16 id = 0;

	// Si hay Sprites 3D que dibujar...
	if (NF_CREATED_3DSPRITE.total > 0) {
		// Dibuja todos los sprites creados
		for (n = 0; n < NF_CREATED_3DSPRITE.total; n ++) {
			// Obten la ID del sprite actual
			id = NF_CREATED_3DSPRITE.id[n];
			// Si el sprite es visible...
			if (NF_3DSPRITE[id].show) {
				// Calcula la Z actual
				z = (n + NF_3DSPRITE[id].z);
				// Aplicale el alpha indicado
				glPolyFmt(POLY_ALPHA(NF_3DSPRITE[id].alpha) | POLY_ID(NF_3DSPRITE[id].poly_id) | POLY_CULL_NONE);
				// Hay que aplicarle rotacion o escalado?
				if (NF_3DSPRITE[id].rot || NF_3DSPRITE[id].scale) {
					// Guarda la matriz
					glPushMatrix();
					// Trasladate al centro del Sprite
					x = (NF_3DSPRITE[id].x + (NF_3DSPRITE[id].width >> 1));
					y = (NF_3DSPRITE[id].y + (NF_3DSPRITE[id].height >> 1));
					glTranslatef32(x, y, z);
					// Aplica la rotacion
					if (NF_3DSPRITE[id].rot) {
						glRotateXi(NF_3DSPRITE[id].rx);
						glRotateYi(NF_3DSPRITE[id].ry);
						glRotateZi(NF_3DSPRITE[id].rz);
					}
					// Aplica el escalado
					if (NF_3DSPRITE[id].scale) {
						glScalef32(NF_3DSPRITE[id].sx, NF_3DSPRITE[id].sy, 0);
					}
					// Vuelve a la posicion original
					glTranslatef32(-x, -y, -z);
				}
				// Aplica la textura
				GFX_PAL_FORMAT = NF_3DSPRITE[id].gfx_pal_format;
				GFX_TEX_FORMAT = NF_3DSPRITE[id].gfx_tex_format;
				// Calcula los vertices
				x1 = NF_3DSPRITE[id].x;
				x2 = (NF_3DSPRITE[id].x + NF_3DSPRITE[id].width);
				y1 = NF_3DSPRITE[id].y;
				y2 = (NF_3DSPRITE[id].y + NF_3DSPRITE[id].height);
				// Dibuja el poligono en la pantalla
				glBegin(GL_QUAD);
					// Vertice 1 (arriba, izquierda)
					glTexCoord2t16(inttot16(0), inttot16(0));
					glVertex3v16(x1, y1, z);
					// Vertice 2 (abajo, izquierda)
					glTexCoord2t16(inttot16(0), inttot16(NF_3DSPRITE[id].height));
					glVertex3v16(x1, y2, z);
					// Vertice 3 (abajo, derecha)
					glTexCoord2t16(inttot16(NF_3DSPRITE[id].width), inttot16(NF_3DSPRITE[id].height));
					glVertex3v16(x2, y2, z);
					// Vertice 4 (arriba, derecha)
					glTexCoord2t16(inttot16(NF_3DSPRITE[id].width), inttot16(0));
					glVertex3v16(x2, y1, z);
				// Has aplicado rotacion o escalado?, restaura la matriz
				if (NF_3DSPRITE[id].rot || NF_3DSPRITE[id].scale) {
					// Restaura la matriz
					glPopMatrix(1);
				}
			}
		}

	}

}



// Funcion NF_Update3dSpritesGfx();
void NF_Update3dSpritesGfx(void) {

	// Variables
	u16 n = 0;				// Uso general
	u16 id = 0;
	char* source;			// Puntero de origen
	u32 destination = 0;	// Puntero de destino
	u16 ramid = 0;			// Slot de RAM donde se encuentra el Gfx

	// Si hay Sprites 3D que actualizar...
	if (NF_CREATED_3DSPRITE.total > 0) {

		// Si es necesario, actualiza las texturas de la RAM a la VRAM
		// Bloquea el banco de VRAM (modo LCD) para permitir la escritura
		vramSetBankB(VRAM_B_LCD);

		// Busca los frames a actualizar
		for (n = 0; n < NF_CREATED_3DSPRITE.total; n ++) {
			// Obten la ID del sprite actual
			id = NF_CREATED_3DSPRITE.id[n];
			// Si los frames adicionales estan en RAM y debe de cambiar de frame
			if (
				NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].keepframes
				&&
				(NF_3DSPRITE[id].frame != NF_3DSPRITE[id].newframe)
				) {
				// Calcula el origen y destino del nuevo frame a copiar
				ramid =  NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].ramid;
				source = NF_BUFFER_SPR256GFX[ramid] + (NF_3DSPRITE[id].framesize * NF_3DSPRITE[id].newframe);
				destination = NF_TEX256VRAM[NF_3DSPRITE[id].gfxid].address;
				// Copialo
				NF_DmaMemCopy((void*)destination, source, NF_3DSPRITE[id].framesize);
				// Y actualiza el frame actual
				NF_3DSPRITE[id].frame = NF_3DSPRITE[id].newframe;
			}
		}

		// Restaura el banco de VRAM en modo Textura
		vramSetBankB(VRAM_B_TEXTURE_SLOT1);			// Banco B de la VRAM para Texturas (128kb)

	}

}



// Funcion NF_Rotate3dSprite();
void NF_Rotate3dSprite(u16 id, s16 x, s16 y, s16 z) {

	// Variables locales
	s16 temp = 0;
	s16 rx = 0;
	s16 ry = 0;
	s16 rz = 0;

	// Calcula la rotacion eje X
	if (x < 0) {
		temp = -x;
		rx = (temp << 6);
		rx = -rx;
	} else {
		rx = (x << 6);
	}
	NF_3DSPRITE[id].rx = rx;

	// Calcula la rotacion eje Y
	if (y < 0) {
		temp = -y;
		ry = (temp << 6);
		ry = -ry;
	} else {
		ry = (y << 6);
	}
	NF_3DSPRITE[id].ry = ry;

	// Calcula la rotacion eje Z
	if (z < 0) {
		temp = -z;
		rz = (temp << 6);
		rz = -rz;
	} else {
		rz = (z << 6);
	}
	NF_3DSPRITE[id].rz = rz;

	// Cambia el estado del flag de rotacion
	if ((NF_3DSPRITE[id].rx != 0) || (NF_3DSPRITE[id].ry != 0) || (NF_3DSPRITE[id].rz != 0)) {
		NF_3DSPRITE[id].rot = true;
	} else {
		NF_3DSPRITE[id].rot = false;
	}

}



// Funcion NF_Scale3dSprite();
void NF_Scale3dSprite(u16 id, u16 x, u16 y) {

	// Calcula la rotacion eje X
	NF_3DSPRITE[id].sx = (x << 6);

	// Calcula la rotacion eje Y
	NF_3DSPRITE[id].sy = (y << 6);

	// Cambia el estado del flag de rotacion
	if ((NF_3DSPRITE[id].sx != (64 << 6)) || (NF_3DSPRITE[id].sy != (64 << 6))) {
		NF_3DSPRITE[id].scale = true;
	} else {
		NF_3DSPRITE[id].scale = false;
	}

}



// Funcion NF_Blend3dSprite();
void NF_Blend3dSprite(u8 sprite, u8 poly_id, u8 alpha) {
	// Si el nivel de alpha es opaco o el ID es 0...
	if ((poly_id == 0) || (alpha == 31)) {
		NF_3DSPRITE[sprite].poly_id = 0;
		NF_3DSPRITE[sprite].alpha = 31;
	} else {
		// De lo contrario, asignale el ID y alpha indicados
		NF_3DSPRITE[sprite].poly_id = poly_id;
		NF_3DSPRITE[sprite].alpha = alpha;
	}
}



// Funcion NF_3dSpritesLayer();
void NF_3dSpritesLayer(u8 layer) {

	// Resetea los BITS de control de prioridad en todos los fondos
	REG_BG0CNT &= 0xfffc;		// Pon a 0 los bits 0 y 1 del registro
	REG_BG1CNT &= 0xfffc;
	REG_BG2CNT &= 0xfffc;
	REG_BG3CNT &= 0xfffc;

	// Reordena todas las capas segun la solicitud
	switch (layer) {
		case 0:		// 3D Sprites en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 1:		// 3D Sprites en la capa 1
			REG_BG0CNT += BG_PRIORITY_1;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 2:		// 3D Sprites en la capa 2
			REG_BG0CNT += BG_PRIORITY_2;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
		case 3:		// 3D Sprites en la capa 3
			REG_BG0CNT += BG_PRIORITY_3;
			REG_BG1CNT += BG_PRIORITY_0;
			REG_BG2CNT += BG_PRIORITY_1;
			REG_BG3CNT += BG_PRIORITY_2;
			break;
		default:	// 3D Sprites en la capa 0
			REG_BG0CNT += BG_PRIORITY_0;
			REG_BG1CNT += BG_PRIORITY_1;
			REG_BG2CNT += BG_PRIORITY_2;
			REG_BG3CNT += BG_PRIORITY_3;
			break;
	}

}



// Funcion NF_3dSpriteEditPalColor();
void NF_3dSpriteEditPalColor(u8 pal, u8 number, u8 r, u8 g, u8 b) {

	// Verifica si esta la paleta en VRAM
	if (!NF_TEXPALSLOT[pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));

	// Calcula los valores para el HI-Byte y el LO-Byte
	u16 hibyte = (rgb >> 8);
	u16 lobyte = rgb - (hibyte << 8);

	// Graba los bytes
	*(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + (number << 1)) = lobyte;
	*(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + ((number << 1) + 1)) = hibyte;

}



// Funcion 	NF_3dSpriteUpdatePalette();
void NF_3dSpriteUpdatePalette(u8 pal) {

	// Verifica si esta la paleta en VRAM
	if (!NF_TEXPALSLOT[pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Obten el slot donde esta la paleta en RAM
	u8 slot = NF_TEXPALSLOT[pal].ramslot;

	// Actualiza la paleta en VRAM
	u32 address = (0x06890000) + (pal << 9);			// Calcula donde guardaras la paleta
	vramSetBankF(VRAM_F_LCD);			// Bloquea el banco F para escribir las paletas
	NF_DmaMemCopy((void*)address, NF_BUFFER_SPR256PAL[slot], NF_SPR256PAL[slot].size);	// Copia la paleta al banco F
	vramSetBankF(VRAM_F_TEX_PALETTE);	// Banco F de la VRAM para paletas extendidas (Texturas)

}



// Funcion NF_3dSpriteGetPalColor();
void NF_3dSpriteGetPalColor(u8 pal, u8 number, u8* r, u8* g, u8* b) {

	// Verifica si esta la paleta en VRAM
	if (!NF_TEXPALSLOT[pal].inuse) {
		NF_Error(111, "Sprite PAL", pal);
	}

	// Obten los bytes
	u16 lobyte = *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + (number << 1));
	u16 hibyte = *(NF_BUFFER_SPR256PAL[NF_TEXPALSLOT[pal].ramslot] + ((number << 1) + 1));

	// Calcula el RGB (compuesto)
	u16 rgb = (hibyte << 8) + lobyte;

	// Calcula los RGB
	*r = (rgb & 0x1F);
	*g = ((rgb >> 5) & 0x1F);
	*b = ((rgb >> 10) & 0x1F);

}


// Funcion NF_3dSpriteSetDeep();
void NF_3dSpriteSetDeep(u8 id, s16 z) {

	// Asigna la profundidad
	NF_3DSPRITE[id].z = z;

	// Verifica los limites
	if (NF_3DSPRITE[id].z < -512) NF_3DSPRITE[id].z = -512;
	if (NF_3DSPRITE[id].z > 512) NF_3DSPRITE[id].z = 512;

}
