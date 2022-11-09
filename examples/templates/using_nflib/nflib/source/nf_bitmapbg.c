
// NightFox LIB - Include de funciones de fondos en modo Bitmap
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
#include "nf_bitmapbg.h"
#include "nf_basic.h"
#include "nf_2d.h"



// Define los Buffers para almacenar datos de 16 bits
NF_TYPE_BG16B_INFO NF_BG16B[NF_SLOTS_BG16B];		// Fondos RAW de 16 bits

// Backbuffer de 16 bits de cada pantalla
u16* NF_16BITS_BACKBUFFER[2];

// Define los Buffers para almacenar datos de 8 bits
NF_TYPE_BG8B_INFO NF_BG8B[NF_SLOTS_BG8B];	// Fondos indexados de 8 bits

// Backbuffer de 8 bits de cada pantalla
NF_TYPE_BB8B_INFO NF_8BITS_BACKBUFFER[2];





// Funcion NF_Init16bitsBgBuffers();
void NF_Init16bitsBgBuffers(void) {
	// Variables locales
	u8 n = 0;
	for (n = 0; n < NF_SLOTS_BG16B; n ++) {
		NF_BG16B[n].buffer = NULL;
		NF_BG16B[n].size = 0;
		NF_BG16B[n].inuse = false;
		NF_BG16B[n].width = 0;
		NF_BG16B[n].height = 0;
	}
}



// Funcion NF_Reset16bitsBgBuffers();
void NF_Reset16bitsBgBuffers(void) {
	// Variables locales
	u8 n = 0;
	// Libera la RAM
	for (n = 0; n < NF_SLOTS_BG16B; n ++) {
		free(NF_BG16B[n].buffer);
	}
	// Reinicia los datos
	NF_Init16bitsBgBuffers();
}



// Funcion NF_Init16bitsBackBuffer();
void NF_Init16bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	NF_16BITS_BACKBUFFER[scr] = NULL;
}


// Funcion NF_Enable16bitsBackBuffer();
void NF_Enable16bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	// Resetea el buffer
	free(NF_16BITS_BACKBUFFER[scr]);
	NF_16BITS_BACKBUFFER[scr] = NULL;
	// Asignale 128kb de memoria
	NF_16BITS_BACKBUFFER[scr] = (u16*) calloc(65536, sizeof(u16));
	// Devuelve error si no hay suficiente memoria
	if (NF_16BITS_BACKBUFFER[scr] == NULL) NF_Error(102, NULL, 131072);
}



// Funcion NF_Disble16bitsBackBuffer();
void NF_Disble16bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	// Resetea el buffer
	free(NF_16BITS_BACKBUFFER[scr]);
	NF_16BITS_BACKBUFFER[scr] = NULL;
}



// Funcion NF_Flip16bitsBackBuffer();
void NF_Flip16bitsBackBuffer(u8 screen) {
	// Copia el contenido del Backbuffer a la VRAM
	// de la pantalla solicitada
	if (screen == 0) {
		NF_DmaMemCopy((void*)0x06000000, NF_16BITS_BACKBUFFER[0], 131072);
	} else {
		NF_DmaMemCopy((void*)0x06200000, NF_16BITS_BACKBUFFER[1], 131072);
	}
}



// Funcion NF_InitBitmapBgSys();
void NF_InitBitmapBgSys(u8 screen, u8 mode) {

	// Habilita la capa 3 de la pantalla indicada en modo BITMAP

	// Variables locales
	u8 n = 0;

	// Inicializa la VRAM
	if (screen == 0) {
		vramSetBankA(VRAM_A_MAIN_BG);				// Banco A de la VRAM para fondos (128kb)
		memset((void*)0x06000000, 0, 131072);		// Borra el contenido del banco A
		// Oculta todas las capas
		for (n = 0; n < 4; n ++) {					// Oculta todas las 4 capas
			NF_HideBg(0, n);
		}
	} else {
		vramSetBankC(VRAM_C_SUB_BG);					// Banco C de la VRAM para fondos (128kb)
		memset((void*)0x06200000, 0, 131072);			// Borra el contenido del banco C
		// Oculta todas las capas
		for (n = 0; n < 4; n ++) {					// Oculta todas las 4 capas
			NF_HideBg(1, n);
		}
	}

	// Inicializa la capa de dibujado
	if (screen == 0) {
		if (mode == 0) {	// Modo 8 bits (Capas 1 y 3)
			REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
			REG_BG2CNT = BG_PRIORITY_2 | BG_BMP_BASE(0) | BG_BMP8_256x256;
		} else {			// Modo 16 bits
			REG_BG3CNT = BG_PRIORITY_3 | BG_BMP_BASE(0) | BG_BMP16_256x256;
		}
		// Resetea los registros de RotScale (Capa 3)
		REG_BG3PA = (1 << 8);
		REG_BG3PB = 0;
		REG_BG3PC = 0;
		REG_BG3PD = (1 << 8);
		NF_ScrollBg(0, 3, 0, 0);					// Posicionala en 0, 0
		NF_ShowBg(0, 3);							// Muestra la capa 3
		// Resetea los registros de RotScale (Capa 2)
		if (mode == 0) {
			REG_BG2PA = (1 << 8);
			REG_BG2PB = 0;
			REG_BG2PC = 0;
			REG_BG2PD = (1 << 8);
			NF_ScrollBg(0, 2, 0, 0);				// Posicionala en 0, 0
			NF_ShowBg(0, 2);						// Muestra la capa 2
		}
	} else {
		if (mode == 0) {	// Modo 8 bits (Capas 2 y 3)
			REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(4) | BG_BMP8_256x256;
			REG_BG2CNT_SUB = BG_PRIORITY_2 | BG_BMP_BASE(0) | BG_BMP8_256x256;
		} else {			// Modo 16 bits
			REG_BG3CNT_SUB = BG_PRIORITY_3 | BG_BMP_BASE(0) | BG_BMP16_256x256;
		}
		// Resetea los registros de RotScale (Capa 3)
		REG_BG3PA_SUB = (1 << 8);
		REG_BG3PB_SUB = 0;
		REG_BG3PC_SUB = 0;
		REG_BG3PD_SUB = (1 << 8);
		NF_ScrollBg(1, 3, 0, 0);					// Posicionala en 0, 0
		NF_ShowBg(1, 3);							// Muestra la capa 3
		// Resetea los registros de RotScale (Capa 2)
		if (mode == 0) {
			REG_BG2PA_SUB = (1 << 8);
			REG_BG2PB_SUB = 0;
			REG_BG2PC_SUB = 0;
			REG_BG2PD_SUB = (1 << 8);
			NF_ScrollBg(1, 2, 0, 0);				// Posicionala en 0, 0
			NF_ShowBg(1, 2);						// Muestra la capa 2
		}
	}

}



// Funcion NF_Load16bitsBg();
void NF_Load16bitsBg(const char* file, u8 slot) {
	// Llama a la funcion de carga de datos de imagen de 16bits
	NF_Load16bImgData(file, slot, 256, 256, 0);
}



// Funcion NF_Load16bitsImage();
void NF_Load16bitsImage(const char* file, u8 slot, u16 size_x, u16 size_y) {
	// Llama a la funcion de carga de datos de imagen de 16bits
	NF_Load16bImgData(file, slot, size_x, size_y, 1);
}



// Funcion NF_Load16bImgData();
void NF_Load16bImgData(const char* file, u8 slot, u16 x, u16 y, u8 type) {
	
	// Verifica el rango de Id's
	if ((slot < 0) || (slot >= NF_SLOTS_BG16B)) {
		if (type == 0) {
			NF_Error(106, "16 Bits Bg's", NF_SLOTS_BG16B);
		} else {
			NF_Error(106, "16 Bits Image", NF_SLOTS_BG16B);
		}
	}

	// Vacia los buffers que se usaran
	free(NF_BG16B[slot].buffer);
	NF_BG16B[slot].buffer = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Variable para el tamaño de archivo
	u32 size = 0;

	// Carga el archivo .IMG
	sprintf(filename, "%s/%s.img", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		size = ftell(file_id);
		rewind(file_id);
		// Si excede del tamaño maximo (128kb), error
		if (size > 131072) NF_Error(116, filename, 131072);
		// Reserva el espacio en RAM
		NF_BG16B[slot].buffer = (u16*) calloc ((size >> 1), sizeof(u16));
		if (NF_BG16B[slot].buffer == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BG16B[slot].buffer, 1, size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo

	// Asegurate que el alpha bit (BIT 15) esta marcado
	u32 n = 0;
	for (n = 0; n < (size >> 1); n ++) {
		NF_BG16B[slot].buffer[n] |= BIT(15);
	}

	// Guarda los parametros del fondo
	NF_BG16B[slot].size = size;		// Guarda el tamaño
	NF_BG16B[slot].width = x;		// Ancho del fondo
	NF_BG16B[slot].height = y;		// Altura del fondo
	NF_BG16B[slot].inuse = true;	// Marca que esta en uso

}



// Funcion NF_Unload16bitsBg();
void NF_Unload16bitsBg(u8 slot) {
	
	// Verifica si el buffer contiene datos
	if (!NF_BG16B[slot].inuse) NF_Error(110, "16 Bits Bg", slot);

	// Vacia los buffers que se usaran
	free(NF_BG16B[slot].buffer);
	NF_BG16B[slot].buffer = NULL;

	NF_BG16B[slot].size = 0;		// Tamaño a 0
	NF_BG16B[slot].inuse = false;	// Marca que esta libre

}



// Funcion NF_Copy16bitsBuffer();
void NF_Copy16bitsBuffer(u8 screen, u8 destination, u8 slot) {

	// Verifica si el buffer contiene datos
	if (!NF_BG16B[slot].inuse) NF_Error(110, "16 Bits Bg", slot);

	if (destination == 0) {		// Si el destino es la VRAM
		// Dependiendo de la pantalla
		if (screen == 0) {
			NF_DmaMemCopy((void*)0x06000000, NF_BG16B[slot].buffer, NF_BG16B[slot].size);
		} else {
			NF_DmaMemCopy((void*)0x06200000, NF_BG16B[slot].buffer, NF_BG16B[slot].size);
		}
	} else {					// Si el destino es el BackBuffer
		// Dependiendo de la pantalla
		if (screen == 0) {
			memcpy(NF_16BITS_BACKBUFFER[0], NF_BG16B[slot].buffer, NF_BG16B[slot].size);
		} else {
			memcpy(NF_16BITS_BACKBUFFER[1], NF_BG16B[slot].buffer, NF_BG16B[slot].size);
		}
	}

}



// Funcion NF_Draw16bitsImage();
void NF_Draw16bitsImage(u8 screen, u8 slot, s16 x, s16 y, bool alpha) {

	// Verifica si el buffer contiene datos
	if (!NF_BG16B[slot].inuse) NF_Error(110, "16 Bits Image", slot);

	// Variables locales
	u16 img_x = 0;
	u16 img_y = 0;
	s16 buff_x = 0;
	s16 buff_y = 0;
	u32 buff_idx = 0;
	u16 data = 0;

	// Filtro de pantalla
	u8 scr = screen;
	if (scr > 1) scr = 1;

	// Si el destino es el BackBuffer
	for (img_y = 0; img_y < NF_BG16B[slot].height; img_y ++) {
		for (img_x = 0; img_x < NF_BG16B[slot].width; img_x ++ ) {
			// Calcula donde se escribira el pixel
			buff_x = (img_x + x);
			buff_y = (img_y + y);
			// Si esta dentro de la pantalla, dibujalo
			if (
				(buff_x >= 0)
				&&
				(buff_x <= 255)
				&&
				(buff_y >= 0)
				&&
				(buff_y <= 255)
				) {
				// Calcula el offset dentro del buffer
				buff_idx = ((buff_y << 8) + buff_x);
				// Valor del Pixel
				data = NF_BG16B[slot].buffer[((img_y * NF_BG16B[slot].width) + img_x)]; 
				// Si el pixel NO es magenta !(RGB15(31, 0, 31) | BIT(15))
				if ((data != 0xFC1F) || (!alpha)) {
					// Escribe el pixel en el BackBuffer
					*(NF_16BITS_BACKBUFFER[scr] + buff_idx) = data;
				}
			}
		}
	}

}



// Funcion NF_Init8bitsBgBuffers();
void NF_Init8bitsBgBuffers(void) {
	// Variables locales
	u8 n = 0;
	for (n = 0; n < NF_SLOTS_BG8B; n ++) {
		NF_BG8B[n].data = NULL;
		NF_BG8B[n].pal = NULL;
		NF_BG8B[n].data_size = 0;
		NF_BG8B[n].pal_size = 0;
		NF_BG8B[n].inuse = false;
	}
}



// Funcion NF_Reset8bitsBgBuffers();
void NF_Reset8bitsBgBuffers(void) {
	// Variables locales
	u8 n = 0;
	// Libera la RAM usada
	for (n = 0; n < NF_SLOTS_BG8B; n ++) {
		free(NF_BG8B[n].data);
		free(NF_BG8B[n].pal);
	}
	// Reinicia los datos
	NF_Init8bitsBgBuffers();
}


// Funcion NF_Load8bitsBg();
void NF_Load8bitsBg(const char* file, u8 slot) {
	
	// Verifica el rango de Id's
	if ((slot < 0) || (slot >= NF_SLOTS_BG8B)) {
		NF_Error(106, "8 Bits Bg's", NF_SLOTS_BG8B);
	}

	// Vacia los buffers que se usaran
	free(NF_BG8B[slot].data);
	NF_BG8B[slot].data = NULL;
	free(NF_BG8B[slot].pal);
	NF_BG8B[slot].pal = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Variable para el tamaño de archivo
	u32 size = 0;

	// Carga el archivo .IMG
	sprintf(filename, "%s/%s.img", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		size = ftell(file_id);
		rewind(file_id);
		// Si excede del tamaño maximo (64kb), error
		if (size > 65536) NF_Error(116, filename, 65536);
		// Reserva el espacio en RAM
		NF_BG8B[slot].data = (u8*) calloc (size, sizeof(u8));
		if (NF_BG8B[slot].data == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BG8B[slot].data, 1, size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo
	NF_BG8B[slot].data_size = size;		// Guarda el tamaño del buffer

	// Carga el archivo .PAL
	sprintf(filename, "%s/%s.pal", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		size = ftell(file_id);
		rewind(file_id);
		// Si la paleta tiene un tamaño inferior a 512, ajusta el tamaño
		if (size < 512) size = 512;
		// Reserva el espacio en RAM
		NF_BG8B[slot].pal = (u16*) calloc ((size >> 1), sizeof(u16));
		if (NF_BG8B[slot].pal == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BG8B[slot].pal, 1, size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo
	NF_BG8B[slot].pal_size = size;		// Guarda el tamaño del buffer

	// Marca el slot como que esta en uso
	NF_BG8B[slot].inuse = true;

}



// Funcion NF_Unload8bitsBg();
void NF_Unload8bitsBg(u8 slot) {
	
	// Verifica si el buffer contiene datos
	if (!NF_BG8B[slot].inuse) NF_Error(110, "8 Bits Bg", slot);

	// Vacia los buffers que se usaran
	free(NF_BG8B[slot].data);
	NF_BG8B[slot].data = NULL;
	NF_BG8B[slot].data_size = 0;
	free(NF_BG8B[slot].pal);
	NF_BG8B[slot].pal = NULL;
	NF_BG8B[slot].pal_size = 0;

	NF_BG8B[slot].inuse = false;	// Marca que esta libre

}



// Funcion NF_Copy8bitsBuffer();
void NF_Copy8bitsBuffer(u8 screen, u8 destination, u8 slot) {

	// Verifica si el buffer contiene datos
	if (!NF_BG8B[slot].inuse) NF_Error(110, "8 Bits Bg", slot);

	u8 scr = screen;
	if (scr > 1) scr = 1;

	// Si el destino es la VRAM
	if (destination < 2) {

		// Segun la pantalla...
		u32 data = 0;
		u32 pal = 0;
		if (screen == 0) {
			data = (0x06000000);	// Direccion en VRAM para los datos
			pal = (0x05000000);		// Direccion en VRAM para la paleta
		} else {
			data = (0x06200000);	// Direccion en VRAM para los datos
			pal = (0x05000400);		// Direccion en VRAM para la paleta
		}
		// Segun la capa
		if (destination == 1) data += 65536;
		// Copia los datos a la VRAM
		NF_DmaMemCopy((void*)data, NF_BG8B[slot].data, NF_BG8B[slot].data_size);
		NF_DmaMemCopy((void*)pal, NF_BG8B[slot].pal, NF_BG8B[slot].pal_size);

	} else {

		// Copia los datos al BackBuffer
		memcpy(NF_8BITS_BACKBUFFER[scr].data, NF_BG8B[slot].data, NF_BG8B[slot].data_size);
		memcpy(NF_8BITS_BACKBUFFER[scr].pal, NF_BG8B[slot].pal, NF_BG8B[slot].pal_size);

	}

}



// Funcion NF_Init8bitsBackBuffer();
void NF_Init8bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	NF_8BITS_BACKBUFFER[scr].data = NULL;
	NF_8BITS_BACKBUFFER[scr].pal = NULL;
}



// Funcion NF_Enable8bitsBackBuffer();
void NF_Enable8bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	// Resetea el buffer
	free(NF_8BITS_BACKBUFFER[scr].data);
	free(NF_8BITS_BACKBUFFER[scr].pal);
	NF_8BITS_BACKBUFFER[scr].data = NULL;
	NF_8BITS_BACKBUFFER[scr].pal = NULL;
	// Asignale 64kb de memoria para datos
	NF_8BITS_BACKBUFFER[scr].data = (u8*) calloc(65536, sizeof(u8));
	if (NF_8BITS_BACKBUFFER[scr].data == NULL) NF_Error(102, NULL, 65536);
	// Asignale 512 bytes de memoria para paletas
	NF_8BITS_BACKBUFFER[scr].pal = (u16*) calloc(256, sizeof(u16));
	if (NF_8BITS_BACKBUFFER[scr].pal == NULL) NF_Error(102, NULL, 512);
}



// Funcion NF_Disble8bitsBackBuffer();
void NF_Disble8bitsBackBuffer(u8 screen) {
	u8 scr = screen;
	if (scr > 1) scr = 1; 
	// Resetea el buffer
	free(NF_8BITS_BACKBUFFER[scr].data);
	free(NF_8BITS_BACKBUFFER[scr].pal);
	NF_8BITS_BACKBUFFER[scr].data = NULL;
	NF_8BITS_BACKBUFFER[scr].pal = NULL;
}



// Funcion NF_Flip8bitsBackBuffer();
void NF_Flip8bitsBackBuffer(u8 screen, u8 destination) {

	// Copia el contenido del Backbuffer a la VRAM
	// de la pantalla solicitada
	u8 scr = screen;
	if (scr > 1) scr = 1; 

	// Segun la pantalla...
	u32 data = 0;
	u32 pal = 0;
	if (scr == 0) {
		data = (0x06000000);			// Direccion en VRAM para los datos
		pal = (0x05000000);				// Direccion en VRAM para la paleta
	} else {
		data = (0x06200000);			// Direccion en VRAM para los datos
		pal = (0x05000400);				// Direccion en VRAM para la paleta
	}

	// Segun la capa
	if (destination == 1) data += 65536;

	// Copia los datos a la VRAM
	NF_DmaMemCopy((void*)data, NF_8BITS_BACKBUFFER[scr].data, 65536);
	NF_DmaMemCopy((void*)pal, NF_8BITS_BACKBUFFER[scr].pal, 512);

}
