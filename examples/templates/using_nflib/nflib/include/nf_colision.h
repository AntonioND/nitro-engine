#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_COLISION_H__
#define __NF_COLISION_H__





// NightFox LIB - Include de Colisiones
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>





// Define los slots maximos para los mapas de colisiones
#define NF_SLOTS_CMAP 32

// Define la estructura de control de mapas de colisiones
typedef struct {
	char* tiles;		// Buffer para almacenar los tiles
	char* map;			// Buffer para almacenar el mapa
	char* pal;			// Buffer para almacenar la paleta
	u32 tiles_size;		// Tamaño de los archivos
	u32 map_size;
	u32 pal_size;
	u16 width;			// Ancho del mapa (en pixeles)
	u16 height;			// Alto del mapa (en pixeles)
	bool inuse;			// Esta en uso el slot?
} NF_TYPE_CMAP_INFO;
extern NF_TYPE_CMAP_INFO NF_CMAP[NF_SLOTS_CMAP];		// Datos de los mapas de colision




// Funcion NF_InitCmapBuffers();
void NF_InitCmapBuffers(void);
// Inicializa los buffers que almacenaran los mapas de colision
// Debes usar esta funcion una unica vez antes de cargar ningun mapa de colision



// Funcion NF_ResetCmapBuffers();
void NF_ResetCmapBuffers(void);
// Reinicia los buffers y variables de los mapas de colisiones.



// Funcion NF_LoadColisionMap();
void NF_LoadColisionMap(const char* file, u8 id, u16 width, u16 height);
// Carga un mapa de colisiones en el slot indicado.



// Funcion NF_UnloadColisionMap();
void NF_UnloadColisionMap(u8 id);
// Borra de la RAM el mapa de colisiones con el nº de slot indicado.



// Funcion NF_GetTile();
extern u16 NF_GetTile(u8 slot, s32 x, s32 y);
// Devuelve el numero de tile de la posicion especificada.



// Funcion NF_SetTile();
void NF_SetTile(u8 slot, s32 x, s32 y, u16 value);
// Cambia el valor del tile en la posicion especificada.



// Funcion NF_LoadColisionBg();
void NF_LoadColisionBg(const char* file, u8 id, u16 width, u16 height);
// Carga un fondo de colisiones para pixel perfect



// Funcion NF_UnloadColisionBg();
void NF_UnloadColisionBg(u8 id);
// Descarga un fondo de colisiones para pixel perfect



// Funcion NF_GetPoint();
extern u8 NF_GetPoint(u8 slot, s32 x, s32 y);
// Obten el color del pixel de las coordenadas dadas





#endif

#ifdef __cplusplus
}
#endif
