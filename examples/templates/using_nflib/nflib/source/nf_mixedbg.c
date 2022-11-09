
// NightFox LIB - Include de Fondos mixtos (Tiled / Bitmap 8 bits)
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
#include "nf_bitmapbg.h"
#include "nf_mixedbg.h"





// Funcion NF_InitMixedBgSys();
void NF_InitMixedBgSys(u8 screen) {

	// Variables
	u8 n = 0;

	// Define el numero de bancos de Mapas y Tiles
	NF_BANKS_TILES[screen] = 4;		// (1 banks = 16kb)	Cada banco de tiles puede alvergar 8 bancos de Mapas
	NF_BANKS_MAPS[screen] = 8;		// (1 bank = 2kb)	Usar multiplos de 8. Cada set de 8 bancos consume 1 banco de tiles
	// Por defecto Tiles = 4, Mapas = 8
	// Esto nos deja 3 bancos de 16kb para tiles
	// y 8 bancos de 2kb para mapas

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

	// Inicializa la capa de dibujado para bitmaps (capa 3 unicamente)
	if (screen == 0) {
		// Modo 8 bits (Capas 3)
		REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
		// Resetea los registros de RotScale (Capa 3)
		REG_BG3PA = (1 << 8);
		REG_BG3PB = 0;
		REG_BG3PC = 0;
		REG_BG3PD = (1 << 8);
		NF_ScrollBg(0, 3, 0, 0);					// Posicionala en 0, 0
		NF_ShowBg(0, 3);							// Muestra la capa 3
	} else {
		// Modo 8 bits (Capas 2 y 3)
		REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
		// Resetea los registros de RotScale (Capa 3)
		REG_BG3PA_SUB = (1 << 8);
		REG_BG3PB_SUB = 0;
		REG_BG3PC_SUB = 0;
		REG_BG3PD_SUB = (1 << 8);
		NF_ScrollBg(1, 3, 0, 0);					// Posicionala en 0, 0
		NF_ShowBg(1, 3);							// Muestra la capa 3
	}

}
