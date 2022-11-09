
// NightFox LIB - Funciones de Textos
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
#include "nf_text.h"




// Define los buffers para almacenar las capas de texto
NF_TYPE_TEXT_INFO NF_TEXT[2][4];



// Funcion NF_InitTextSys();
void NF_InitTextSys(u8 screen) {

	u8 n = 0;

	// Reinicia las variables
	for (n = 0; n < 4; n ++) {
		NF_TEXT[screen][n].width = 0;		// Ancho del mapa de texto
		NF_TEXT[screen][n].height = 0;		// Alto del mapa de texto
		NF_TEXT[screen][n].rotation = 0;	// Rotacion a 0 (ninguna)
		NF_TEXT[screen][n].slot = 255;		// Slot donde esta el tileset
		NF_TEXT[screen][n].pal = 0;			// nº de paleta extendida (0 por defecto)
		NF_TEXT[screen][n].exist = false;	// Marcalo como no existente
		NF_TEXT[screen][n].update = false;	// No es necesario actualizarlo
	}
	
}



// Funcion NF_LoadTextFont();
void NF_LoadTextFont(const char* file, const char* name, u16 width, u16 height, u8 rotation) {

	// Busca un slot libre
	u16 n = 0;
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

	// Carga el archivo .FNT
	sprintf(filename, "%s/%s.fnt", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		NF_TILEDBG[slot].tilesize = (NF_TEXT_FONT_CHARS << 6);		// 100 caracteres x 64 bytes
		// Reserva el espacio en RAM
		NF_BUFFER_BGTILES[slot] = (char*) calloc (NF_TILEDBG[slot].tilesize, sizeof(char));
		if (NF_BUFFER_BGTILES[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].tilesize);
		}
		fread(NF_BUFFER_BGTILES[slot], 1, NF_TILEDBG[slot].tilesize, file_id);	// Lee el resto de caracteres de la fuente
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Rota los Gfx de los tiles si es necesario
	if (rotation > 0) {
		for (n = 0; n < NF_TEXT_FONT_CHARS; n ++) {
			NF_RotateTileGfx(slot, n, rotation);
		}
	}

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
		NF_TILEDBG[slot].palsize = ftell(file_id);
		rewind(file_id);
		// Reserva el espacio en RAM
		NF_BUFFER_BGPAL[slot] = (char*) calloc (NF_TILEDBG[slot].palsize, sizeof(char));
		if (NF_BUFFER_BGPAL[slot] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_TILEDBG[slot].palsize);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_BGPAL[slot], 1, NF_TILEDBG[slot].palsize, file_id);
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



// Funcion NF_UnloadTestFont();
void NF_UnloadTextFont(const char* name) {
	NF_UnloadTiledBg(name);
}



// Funcion NF_CreateTextLayer();
void NF_CreateTextLayer(u8 screen, u8 layer, u8 rotation, const char* name) {

	u8 n = 0;			// Bucle
	u8 slot = 255;		// Slot seleccionado
	char bg[32];		// Nombre

	// Crea un  fondo para usarlo como capa de texto
	NF_CreateTiledBg(screen, layer, name);

	// Busca el numero de slot donde esta cargada la fuente
	sprintf(bg, "%s", name);							// Obten el nombre del fondo a buscar
	for (n = 0; n < NF_SLOTS_TBG; n ++) {				// Busca en todos los slots
		if (strcmp(bg, NF_TILEDBG[n].name) == 0) {		// Si lo encuentras
			slot = n;									// Guarda el slot a usar
			n = NF_SLOTS_TBG;							// Deja de buscar
		}
	}

	// Guarda si el texto debe ser rotado
	NF_TEXT[screen][layer].rotation = rotation;

	// Guarda las medidas del fondo en tiles (ultimo numero de tile)
	NF_TEXT[screen][layer].width = ((NF_TILEDBG[slot].width >> 3) - 1);
	NF_TEXT[screen][layer].height = ((NF_TILEDBG[slot].height >> 3) - 1);

	// Almacena el slot donde esta cargada la fuente
	NF_TEXT[screen][layer].slot = slot;

	// Y marcalo como creado
	NF_TEXT[screen][layer].exist = true;

}



// Funcion NF_DeleteTextLayer();
void NF_DeleteTextLayer(u8 screen, u8 layer) {

	// Verifica si la capa de texto de destino existe
	if (!NF_TEXT[screen][layer].exist) {
		NF_Error(114, NULL, screen);
	}

	// Borra el fondo usado como capa de texto
	NF_DeleteTiledBg(screen, layer);

	// Guarda si el texto debe ser rotado
	NF_TEXT[screen][layer].rotation = 0;

	// Guarda las medidas del fondo en tiles
	NF_TEXT[screen][layer].width = 0;
	NF_TEXT[screen][layer].height = 0;

	// Y marcalo como creado
	NF_TEXT[screen][layer].exist = false;

}



// Funcion NF_WriteText();
void NF_WriteText(u8 screen, u8 layer, u16 x, u16 y, const char* text) {

	// Verifica si la capa de texto de destino existe
	if (!NF_TEXT[screen][layer].exist) {
		NF_Error(114, NULL, screen);
	}

	u16 n = 0;					// Variable de uso general

	s16 value = 0;				// Valor

	u16 tsize = 0;				// Almacena el numero de letras
	tsize = strlen(text);		// Calcula el numero de letras del texto

	u8* string;					// Buffer temporal
	string = NULL;
	string = (u8*) calloc (tsize, sizeof(u8));

	// Almacena en el buffer temporal el valor de los caracteres
	for (n = 0; n < tsize; n ++) {
		value = ((int)(text[n])) - 32;	// Resta 32 al valor entrado
		if (value < 0) value = 0;
		string[n] = value;
		// Si es un caracter especial...
		if (string[n] > 95) {
			// Resetea el caracter...
			string[n] = 0;
			// Verifica caracteres especiales
			switch (((int)(text[n]))) {
				// Salto de linea
				case 10:	// \n
					string[n] = 200;
					break;
				// Caracteres especiales
				case 199:	// Ç
					string[n] = 96;
					break;
				case 231:	// ç
					string[n] = 97;
					break;
				case 209:	// Ñ
					string[n] = 98;
					break;
				case 241:	// ñ
					string[n] = 99;
					break;
				// Acentos Mayusculas
				case 193:	// Á
					string[n] = 100;
					break;
				case 201:	// É
					string[n] = 101;
					break;
				case 205:	// Í
					string[n] = 102;
					break;
				case 211:	// Ó
					string[n] = 103;
					break;
				case 218:	// Ú
					string[n] = 104;
					break;
				// Acentos Minusculas
				case 225:	// á
					string[n] = 105;
					break;
				case 233:	// é
					string[n] = 106;
					break;
				case 237:	// í
					string[n] = 107;
					break;
				case 243:	// ó
					string[n] = 108;
					break;
				case 250:	// ú
					string[n] = 109;
					break;
				// Dieresis
				case 239:	// ï
					string[n] = 110;
					break;
				case 252:	// ü
					string[n] = 111;
					break;
				// Admiracion e interrogante (Español)
				case 161:	// ¡
					string[n] = 112;
					break;
				case 191:	// ¿
					string[n] = 113;
					break;
				// Caracter invalido
				default:	
					string[n] = 0;
					break;
			}
		}
	}

	// Variable para calcular la posicion del texto
	s16 tx = 0;		// X
	s16 ty = 0;		// Y

	// Escribe los datos en el buffer de texto, segun la rotacion
	switch (NF_TEXT[screen][layer].rotation) {

		case 0:		// Sin rotacion
			// Traspasa las coordenadas virtuales a las reales
			tx = x;
			ty = y;
			// Copia el texto al buffer letra a letra
			for (n = 0; n < tsize; n ++) {
				// Si es un caracter valido
				if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR) {
					// Escribe la letra correspondiente
					NF_SetTileOfMap(screen,layer, tx, ty, ((NF_TEXT[screen][layer].pal << 12) + string[n]));
					// Siguiente letra
					tx ++;
				}
				if ((tx > NF_TEXT[screen][layer].width) || (string[n] == 200)) {		// Si llegas al final de linea,
					tx = 0;			// salto de linea
					ty ++;
					if (ty > NF_TEXT[screen][layer].height) {	// Si estas en la ultima linea,
						ty = 0;		// vuelve a la primera
					}
				}
			}
			break;


		case 1:		// Rotacion 90º a la derecha
			// Traspasa las coordenadas virtuales a las reales
			tx = (NF_TEXT[screen][layer].width - y);
			ty = x;
			// Copia el texto al buffer letra a letra
			for (n = 0; n < tsize; n ++) {
				// Si es un caracter valido
				if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR) {
					// Escribe la letra correspondiente
					NF_SetTileOfMap(screen,layer, tx, ty, ((NF_TEXT[screen][layer].pal << 12) + string[n]));
					// Siguiente letra
					ty ++;
				}
				if ((ty > NF_TEXT[screen][layer].height) || (string[n] == 200)) {		// Si llegas al final de linea,
					ty = 0;			// salto de linea
					tx --;
					if (tx < 0) {	// Si estas en la ultima linea,
						tx = NF_TEXT[screen][layer].width;	// vuelve a la primera
					}
				}
			}
			break;


		case 2:		// Rotacion 90º a la izquierda
			// Traspasa las coordenadas virtuales a las reales
			tx = y;
			ty = (NF_TEXT[screen][layer].height - x);
			// Copia el texto al buffer letra a letra
			for (n = 0; n < tsize; n ++) {
				// Si es un caracter valido
				if (string[n] <= NF_TEXT_FONT_LAST_VALID_CHAR) {
					// Escribe la letra correspondiente
					NF_SetTileOfMap(screen,layer, tx, ty, ((NF_TEXT[screen][layer].pal << 12) + string[n]));
					// Siguiente letra
					ty --;
				}
				if ((ty < 0) || (string[n] == 200)) {		// Si llegas al final de linea,
					ty = NF_TEXT[screen][layer].height;		// Salto de linea
					tx ++;
					if (tx > NF_TEXT[screen][layer].width) {	// Si llegas a la ultima linea
						tx = 0;		// vuelve a la primera
					}
				}
			}
			break;

	}

	// Marca esta capa de texto para actualizar
	NF_TEXT[screen][layer].update = true;

	// Libera el buffer
	free(string);

}



// Funcion NF_UpdateTextLayers();
void NF_UpdateTextLayers(void) {

	// Variables
	u8 screen = 0;
	u8 layer = 0;

	// Verifica si se tiene que actualizar la capa de texto
	for (screen = 0; screen < 2; screen ++) {		// Bucle de pantalla
		for (layer = 0; layer < 4; layer ++) {		// Bucle de capas
			if (NF_TEXT[screen][layer].update) {	// Si estas marcado para actualizar, hazlo
				NF_UpdateVramMap(screen, layer);
				// Y marcala como actualizada
				NF_TEXT[screen][layer].update = false;

			}
		}
	}
}



// Funcion NF_ClearTextLayer();
void NF_ClearTextLayer(u8 screen, u8 layer) {

	// Verifica si la capa de texto de destino existe
	if (!NF_TEXT[screen][layer].exist) {
		NF_Error(114, NULL, screen);
	}

	// Calcula el tamaño del buffer
	u32 size = (((NF_TEXT[screen][layer].width + 1) * (NF_TEXT[screen][layer].height + 1)) << 1);

	// Pon a 0 todos los bytes del mapa de la capa de texto
	memset(NF_BUFFER_BGMAP[NF_TEXT[screen][layer].slot], 0, size);

	// Marca esta capa de texto para actualizar
	NF_TEXT[screen][layer].update = true;

}



// Funcion NF_DefineTextColor();
void NF_DefineTextColor(u8 screen, u8 layer, u8 color, u8 r, u8 g, u8 b) {

	// Verifica si la capa de texto de destino existe
	if (!NF_TEXT[screen][layer].exist) {
		NF_Error(114, NULL, screen);
	}

	// Calcula el valor RGB
	u16 rgb = ((r)|((g) << 5)|((b) << 10));
	// Direccion en VRAM
	u32 address = 0;

	// Modifica la paleta
	if (screen == 0) {

		vramSetBankE(VRAM_E_LCD);
		address = (0x06880000) + (layer << 13) + (color << 9);		// Primer color de la paleta
		*((u16*)address) = (u16)0xFF00FF;
		address = (0x06880000) + (layer << 13) + (color << 9) + 2;	// Segundo color de la paleta
		*((u16*)address) = rgb;
		vramSetBankE(VRAM_E_BG_EXT_PALETTE);

	} else {	// Paletas de la pantalla 1 (VRAM_H)

		vramSetBankH(VRAM_H_LCD);
		address = (0x06898000) + (layer << 13)  + (color << 9);		// Primer color de la paleta
		*((u16*)address) = (u16)0xFF00FF;
		address = (0x06898000) + (layer << 13)  + (color << 9) + 2;	// Segundo color de la paleta
		*((u16*)address) = rgb;
		vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

	}

}



// Function NF_SetTextColor();
void NF_SetTextColor(u8 screen, u8 layer, u8 color) {

	NF_TEXT[screen][layer].pal = color;

}
