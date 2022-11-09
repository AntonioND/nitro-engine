#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_AFFINEBG_H__
#define __NF_AFFINEBG_H__





// NightFox LIB - Includes de Fondos Affine
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>





// Estructura para almacenar los parametros de los fondos Affine
typedef struct {
	s32 x;			// Posicion X
	s32 y;			// Posicion Y
	s32 x_center;	// Centro X
	s32 y_center;	// Centro Y
	s32 x_scale;	// Valor Zoom X (PA)
	s32 y_scale;	// Valor Zoom Y (PD)
	s32 x_tilt;		// Valor Inclinacion X (PB)
	s32 y_tilt;		// Valor Inclinacion Y (PC)
	s32 angle;		// Valor de la rotacion
} NF_TYPE_AFFINE_BG;
extern NF_TYPE_AFFINE_BG NF_AFFINE_BG[2][4];





// Funcion NF_InitTiledBgSys();
void NF_InitAffineBgSys(u8 screen);
// Inicializa el sistema de fondos Affine. Solo puede usarse en las capas 2 y 3, los fondos en
// la misma pantalla deben de compartir la paleta y no pueden tener mas de 256 tiles.



// Funcion NF_LoadAffineBg();
void NF_LoadAffineBg(const char* file, const char* name, u16 width, u16 height);
// Carga un fondo tileado desde FAT
// Debes de especificar el archivo que se cargara (sin extension) y el nombre
// que le quieres dar y las medidas en pixeles
// Los buffers para fondos tileados deben estar inicializados antes de usar esta funcion



// Funcion NF_UnloadAffineBg();
void NF_UnloadAffineBg(const char* name);
// Borra de la RAM el fondo affine especificado. Es una simple llamada a la funcion NF_UnloadTiledBg();



// Funcion NF_CreateAffineBg();
void NF_CreateAffineBg(u8 screen, u8 layer, const char* name, u8 wrap);
// Crea un fondo con los parametros dados, indicale la pantalla, capa, nombre y si se activa la opcion
// de WRAP arround (0 desactivado, 1 activado).



// Funcion NF_DeleteAffineBg();
void NF_DeleteAffineBg(u8 screen, u8 layer);
// Borra el fondo Affine especificado



// Funcion NF_AffineBgTransform();
void NF_AffineBgTransform(u8 screen, u8 layer, s32 x_scale, s32 y_scale, s32 x_tilt, s32 y_tilt);
// Modifica los parametros de la matriz de transformacion del fondo affine



// Funcion NF_AffineBgMove();
void NF_AffineBgMove(u8 screen, u8 layer, s32 x, s32 y, s32 angle);
// Desplaza el fondo affine y rotalo (-2048 a 2048)



// Funcion NF_AffineBgCenter();
void NF_AffineBgCenter(u8 screen, u8 layer, s32 x, s32 y);
// Define el centro de rotacion de un fondo affine





#endif

#ifdef __cplusplus
}
#endif
