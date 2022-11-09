#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_BASIC_H__
#define __NF_BASIC_H__





// NightFox LIB - Include de funciones basicas
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413



// Includes devKitPro
#include <nds.h>





// Define la variable global NF_ROOTFOLDER
extern char NF_ROOTFOLDER[32];



// Funcion NF_Error();
void NF_Error(u16 code, const char* text, u32 value);
// Errores para debug. Detiene el sistema e informa del error
// 101: Fichero no encontrado
// 102: Memoria insuficiente
// 103: No quedan Slots libres
// 104: Fondo no encontrado
// 105: Fondo no creado
// 106: Fuera de rango
// 107: Insuficientes bloques contiguos en VRAM (Tiles)
// 108: Insuficientes bloques contiguos en VRAM (Maps)
// 109: Id ocupada (ya esta en uso)
// 110: Id no cargada (en RAM)
// 111: Id no en VRAM
// 112: Sprite no creado
// 113:	Memoria VRAM insuficiente
// 114: La capa de Texto no existe
// 115:	Medidas del fondo no compatibles (no son multiplos de 256)
// 116:	Archivo demasiado grande
// 117: Medidas del fondo affine incorrectas
// 118: Capa de creacion del fondo affine incorrecta



// Funcion NF_SetRootFolder();
void NF_SetRootFolder(const char* folder);
// Define el nombre de la carpeta que se usara como "root" si se usa la FAT



// Funcion NF_DmaMemCopy();
void NF_DmaMemCopy(void* destination, const void* source, u32 size);
// Copia un bloque de memoria usando DMA (canal 3, halfwords) y vaciando previamente
// el cache. Con pruebas de bloques grandes (64kb o 128kb) he observado que memcpy(); 
// sigue siendo mas rapida.



// Funcion NF_GetLanguage();
extern u8 NF_GetLanguage(void);
// Devuelve el ID del idioma del usuario
// 0 : Japanese
// 1 : English
// 2 : French
// 3 : German
// 4 : Italian
// 5 : Spanish
// 6 : Chinese





#endif

#ifdef __cplusplus
}
#endif
