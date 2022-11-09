
// NightFox LIB - Funciones basicas y de Debug
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 



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
#include "nf_defines.h"



// Define la variable global NF_ROOTFOLDER
char NF_ROOTFOLDER[32];



// Funcion NF_Error();
void NF_Error(u16 code, const char* text, u32 value) {

	consoleDemoInit();		// Inicializa la consola de texto
	consoleClear();			// Borra la pantalla
	setBrightness(3, 0);	// Restaura el brillo

	u32 n = 0;	// Variables de uso general

	// Captura el codigo de error
	switch (code) {

		case 101:	// Fichero no encontrado
			iprintf("File %s not found.\n", text);
			break;

		case 102:	// Memoria insuficiente
			iprintf("Out of memory.\n");
			iprintf("%d bytes\n", (int)value);
			iprintf("can't be allocated.\n");
			break;

		case 103:	// No quedan Slots libres
			iprintf("Out of %s slots.\n", text);
			iprintf("All %d slots are in use.\n", (int)value);
			break;

		case 104:	// Fondo no encontrado
			iprintf("Tiled Bg %s\n", text);
			iprintf("not found.\n");
			break;

		case 105:	// Fondo no creado
			iprintf("Background number %d\n", (int)value);
			iprintf("on screen %s is\n", text);
			iprintf("not created.\n");
			break;

		case 106:	// Fuera de rango
			iprintf("%s Id out\n", text);
			iprintf("of range (%d max).\n", (int)value);
			break;

		case 107:	// Insuficientes bloques contiguos en VRAM (Tiles)
			n = (int)((value * 16384) / 1024);
			iprintf("Can't allocate %d\n", (int)value);
			iprintf("blocks of tiles for\n");
			iprintf("%s background\n", text);
			iprintf("Free %dkb of VRAM or try to\n", (int)n);
			iprintf("reload all Bg's again\n");
			break;

		case 108:	// Insuficientes bloques contiguos en VRAM (Maps)
			n = (int)((value * 2048) / 1024);
			iprintf("Can't allocate %d\n", (int)value);
			iprintf("blocks of maps for\n");
			iprintf("%s background\n", text);
			iprintf("Free %dkb of VRAM or try to\n", (int)n);
			iprintf("reload all Bg's again\n");
			break;

		case 109:	// Id ocupada
			iprintf("%s Id.%d\n", text, (int)value);
			iprintf("is already in use.\n");
			break;

		case 110:	// Id no cargada
			iprintf("%s\n", text);
			iprintf("%d not loaded.\n", (int)value);
			break;

		case 111:	// Id no en VRAM
			iprintf("%s\n", text);
			iprintf("%d not in VRAM.\n", (int)value);
			break;

		case 112:	// Sprite no creado
			iprintf("Sprite number %d\n", (int)value);
			iprintf("on screen %s is\n", text);
			iprintf("not created.\n");
			break;

		case 113:	// Memoria VRAM insuficiente
			iprintf("Out of VRAM.\n");
			iprintf("%d bytes for %s\n", (int)value, text);
			iprintf("can't be allocated.\n");
			break;

		case 114:	// La capa de Texto no existe
			iprintf("Text layer on screen\n");
			iprintf("nº %d don't exist.\n", (int)value);
			break;

		case 115:	// Medidas del fondo no compatibles (no son multiplos de 256)
			iprintf("Tiled Bg %s\n", text);
			iprintf("has wrong size.\n");
			iprintf("Your bg sizes must be\n");
			iprintf("dividable by 256 pixels.\n");
			break;

		case 116:	// Archivo demasiado grande
			iprintf("File %s\n", text);
			iprintf("is too big.\n");
			iprintf("Max size for\n");
			iprintf("file is %dkb.\n", (int)(value >> 10));
			break;

		case 117:	// Medidas del fondo affine no compatibles (Solo se admiten 256x256 y 512x512)
			iprintf("Affine Bg %s\n", text);
			iprintf("has wrong size.\n");
			iprintf("Your bg sizes must be\n");
			iprintf("256x256 or 512x512 and\n");
			iprintf("with 256 tiles or less.\n");
			break;

		case 118:	// Medidas del fondo affine no compatibles (Solo se admiten 256x256 y 512x512)
			iprintf("Affine Bg %s\n", text);
			iprintf("only can be created\n");
			iprintf("on layers 2 or 3.\n");
			break;

		case 119:	// Tamaño de la textura ilegal.
			iprintf("Texture id.%d illegal size.\n", (int)value);
			iprintf("Only power of 2 sizes can\n");
			iprintf("be used (8 to 1024).\n");
			break;

		case 120:	// Tamaño de la Sprite ilegal.
			iprintf("Sprite id.%d illegal size.\n", (int)value);
			iprintf("8x8 Sprites can't be used\n");
			iprintf("in 1D_128 mode.\n");
			break;

	}

	iprintf("Error code %d.\n", (int)code);		// Imprime el codigo de error

	// Deten la ejecucion del programa
	while (1) {
		swiWaitForVBlank();
	}

}



// Funcion NF_SetRootFolder();
void NF_SetRootFolder(const char* folder) {

	if (strcmp(folder, "NITROFS") == 0) {	// Si se debe iniciar el modo NitroFS y FAT

		// Define NitroFS como la carpeta inicial
		sprintf(NF_ROOTFOLDER, "%s", "");
		// Intenta inicializar NitroFS
		if(nitroFSInit(NULL)) {
			// NitroFS ok
			// Si es correcto, cambia al ROOT del NitroFS
			chdir("nitro:/");
		} else {
			// Fallo. Deten el programa
			consoleDemoInit();	// Inicializa la consola de texto
			if (NF_GetLanguage() == 5) {
				iprintf("Error iniciando NitroFS.\n");
				iprintf("Programa detenido.\n\n");
				iprintf("Verifica que tu flashcard\n");
				iprintf("es compatible con Argv.\n");
				iprintf("Si no lo es, intenta usar el\n");
				iprintf("Homebrew Menu para ejecutarla.\n\n");
			} else {
				iprintf("NitroFS Init Error.\n");
				iprintf("Abnormal termination.\n\n");
				iprintf("Check if your flashcard is\n");
				iprintf("Argv compatible.\n");
				iprintf("If not, try to launch the ROM\n");
				iprintf("using the Homebrew Menu.\n\n");
			}
			iprintf("http://sourceforge.net/projects/devkitpro/files/hbmenu/");
			// Bucle infinito. Fin del programa
			while(1) {
				swiWaitForVBlank();
			}
		}

	} else {	// Si se debe iniciar solo la FAT

		// Define la carpeta inicial de la FAT
		sprintf(NF_ROOTFOLDER, "%s", folder);
		// Intenta inicializar la FAT
		if (fatInitDefault()) {
			// Si es correcto, cambia al ROOT del FAT
			chdir("fat:/");
		} else {
			// Fallo. Deten el programa
			consoleDemoInit();	// Inicializa la consola de texto
			if (NF_GetLanguage() == 5) {
				iprintf("Error iniciando FAT.\n");
				iprintf("Programa detenido.\n\n");
				iprintf("Verifica que tu flashcard es\n");
				iprintf("compatible con DLDI y la ROM\n");
				iprintf("este parcheada correctamente.\n");
			} else {
				iprintf("FAT Init Error.\n");
				iprintf("Abnormal termination.\n\n");
				iprintf("Check if your flashcard is\n");
				iprintf("DLDI compatible and the ROM\n");
				iprintf("is correctly patched.\n");
			}
			// Bucle infinito. Fin del programa
			while(1) {
				swiWaitForVBlank();
			}
		}

	}

}




// Funcion NF_DmaMemCopy();
void NF_DmaMemCopy(void* destination, const void* source, u32 size) {

	// Funcion basada en la documentacion de Coranac
	// http://www.coranac.com/2009/05/dma-vs-arm9-fight/

	// Datos de origen y destino
	u32 src = (u32)source;
	u32 dst = (u32)destination;

	// Verifica si los datos estan correctamente alineados
	if ((src | dst) & 1) {

		// No estan alineados para un DMA copy
		// Se realiza un copia con el memcpy();
		memcpy(destination, source, size);

	} else {

		// Estan alineados correctamente

		// Espera a que el canal 3 de DMA este libre
		while (dmaBusy(3));

		// Manda el cache a la memoria
		DC_FlushRange(source, size);

		// Dependiendo de la alineacion de datos, selecciona el metodo de copia
		if ((src | dst | size) & 3) {
			// Copia de 16 bits
			 dmaCopyHalfWords(3, source, destination, size);
		} else {
			// Copia de 32 bits
			dmaCopyWords(3, source, destination, size);
		}

		// Evita que el destino sea almacenado en cache
		DC_InvalidateRange(destination, size);

	}

}



// Funcion NF_GetLanguage();
u8 NF_GetLanguage(void) {

	// Asegurate que el valor devuelto corresponde a los
	// contenidos en los BITS 0, 1 y 2 de la direccion de memoria
	return (NF_UDATA_LANG & 0x07);

}
