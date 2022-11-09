#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_TILEDBG_H__
#define __NF_TILEDBG_H__





// NightFox LIB - Include de Fondos con tiles
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>





// Define los slots maximos para los fondos
#define NF_SLOTS_TBG 64			// Datos de los fondos
#define NF_SLOTS_EXBGPAL 128	// Paletas extendidas (maximo 16 paletas por fondo)

// Define el numero maximo de bancos para tiles y mapas
#define NF_MAX_BANKS_TILES 8
#define NF_MAX_BANKS_MAPS 16

// Define el numero de bancos de Mapas y Tiles
extern u8 NF_BANKS_TILES[2];	// (1 banks = 16kb)	Cada banco de tiles puede alvergar 8 bancos de Mapas
extern u8 NF_BANKS_MAPS[2];		// (1 bank = 2kb)	Usar multiplos de 8. Cada set de 8 bancos consume 1 banco de tiles

// Define los Buffers para almacenar los fondos
extern char* NF_BUFFER_BGTILES[NF_SLOTS_TBG];
extern char* NF_BUFFER_BGMAP[NF_SLOTS_TBG];
extern char* NF_BUFFER_BGPAL[NF_SLOTS_TBG];


// Define estructura para almacenar la info de los fondos
typedef struct {
	char name[32];		// Nombre del fondo
	u32 tilesize;		// Tamaño del Tileset
	u32 mapsize;		// Tamaño del Map
	u32 palsize;		// Tamaño de la Paleta
	u16 width;			// Ancho del fondo
	u16 height;			// Altura del fondo
	bool available;		// Disponibilidat del Slot
} NF_TYPE_TBG_INFO;
extern NF_TYPE_TBG_INFO NF_TILEDBG[NF_SLOTS_TBG];	// Datos de los fondos

// Define la estructura para almacenar la info y datos de las paletas extendidas
typedef struct {
	char* buffer;	// Buffer para almacenar la paleta
	u32 palsize;	// Tamaño de la paleta
	bool inuse;		// Slot libre o en uso
} NF_TYPE_EXBGPAL_INFO;
extern NF_TYPE_EXBGPAL_INFO NF_EXBGPAL[NF_SLOTS_EXBGPAL];	// Datos de las paletas extendidas

// Define estructura para almacenar la info de los fondos en pantalla
typedef struct {
	u8 tilebase;		// Bloque de inicio en VRAM del Tileset
	u8 tileblocks;		// Bloques usados por el Tileset
	u8 mapbase;			// Bloque de inicio en VRAM del Map
	u8 mapblocks;		// Bloques usados por el Map
	u16 bgwidth;		// Ancho del fondo
	u16 bgheight;		// Altura del fondo
	u16 mapwidth;		// Ancho del mapa
	u16 mapheight;		// Altura del mapa
	u8 bgtype;			// Tipo de mapa
	u8 bgslot;			// Buffer de graficos usado (NF_BUFFER_BGMAP)
	u8 blockx;			// Bloque de mapa (horizontal)
	u8 blocky;			// bloque de mapa (vertical)
	bool created;		// Flag de si esta creado
} NF_TYPE_TBGLAYERS_INFO;
// El hardware de la DS no permite mapas mayores de 512x512
// Asi que informaremos si nuestor mapa lo gestionara el hardware si es menor o 
// igual a 512x512, o usaremos nuestro motor de Tile Swaping
// bgtype 0: Normal (maximo 512 x 512)
// bgtype 1: >512 x 256
// bgtype 2: 256 x >512
// bgtype 3: >512 x >512
extern NF_TYPE_TBGLAYERS_INFO NF_TILEDBG_LAYERS[2][4];	//[screen][layer]



// Define el array de bloques libres
extern u8 NF_TILEBLOCKS[2][NF_MAX_BANKS_TILES];
extern u8 NF_MAPBLOCKS[2][NF_MAX_BANKS_MAPS];



// Funcion NF_InitTiledBgBuffers();
void NF_InitTiledBgBuffers(void);
// Inicializa los buffers y estructuras de control para usar los fondos "tileados"
// Se debe usar antes de cargar o usar cualquier fondo
// No uses esta funcion mas de una vez en tu codigo



// Funcion NF_ResetTiledBgBuffers();
void NF_ResetTiledBgBuffers(void);
// Borra todos los buffers y reinicia las estructuras de fondos "tileados"
// Usala para los cambios de nivel y similares



// Funcion NF_InitTiledBgSys();
void NF_InitTiledBgSys(u8 screen);
// Inicializa las variables de control de tiles, mapas y paletas
// Asigna 128kb de RAM para fondos tileados
// Se debe especificar la pantalla (0 o 1)



// Funcion NF_LoadTiledBg();
void NF_LoadTiledBg(const char* file, const char* name, u16 width, u16 height);
// Carga un fondo tileado desde FAT
// Debes de especificar el archivo que se cargara (sin extension) y el nombre
// que le quieres dar y las medidas en pixeles



// Funcion NF_LoadTilesForBg();
void NF_LoadTilesForBg(const char* file, const char* name, u16 width, u16 height, u16 tile_start, u16 tile_end);
// Carga desde la FAT los tiles especificados y su paleta
// Ademas, crea un mapa vacio de la medida especificada
// Esta funcion es util para cargar varios tiles y despues generar fondos
// modificando el MAP desde cogido (Generador de escenarios, animaciones, etc)



// Funcion NF_UnloadTiledBg();
void NF_UnloadTiledBg(const char* name);
// Borra de la RAM un fondo cargado con NF_LoadTiledBg();
// Debes especificar el nombre que le diste al fondo



// Funcion NF_CreateTiledBg();
void NF_CreateTiledBg(u8 screen, u8 layer, const char* name);
// Crea un fondo con los parametros dados, indicale la pantalla, capa y nombre



// Funcion NF_DeleteTiledBg();
void NF_DeleteTiledBg(u8 screen, u8 layer);
// Borra un fondo de la memoria VRAM
// Debes especificar la pantalla y numero de capa





// Funcion NF_GetTileMapAddress();
extern u32 NF_GetTileMapAddress(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Funcion de uso interno de la libreria
// Devuelve la direccion en el buffer de un tile concreto





// Funcion NF_GetTileOfMap();
extern u16 NF_GetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Obten el valor del tile del mapa indicado en las coordenadas (en tiles) indicadas.




// Funcion NF_SetTileOfMap();
void NF_SetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u16 tile);
// Cambia el valor del tile del mapa indicado en las coordenadas (en tiles) indicadas.




// Funcion NF_UpdateVramMap();
void NF_UpdateVramMap(u8 screen, u8 layer);
// Actualiza en VRAM el contenido del mapa seleccionado.




// Funcion NF_BgSetPalColor();
void NF_BgSetPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b);
// Cambia al momento el valor de un color de la paleta
// Cuidado! Funcion Muy lenta, usar solo para 2 o 3 colores por ciclo
// Cambia el color directamente en la VRAM





// Funcion NF_BgEditPalColor();
void NF_BgEditPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b);
// Edita un color de la paleta seleccionada.
// El color se edita en el buffer de RAM, para que sea efectivo,
// mandala a la VRAM con NF_UpdatePalette();





// Funcion 	NF_BgUpdatePalette();
void NF_BgUpdatePalette(u8 screen, u8 layer);
// Actualiza la paleta en VRAM con la que se encuentra en el buffer de RAM





// Funcion NF_BgGetPalColor();
void NF_BgGetPalColor(u8 screen, u8 layer, u8 number, u8* r, u8* g, u8* b);
// Obtiene el valor de un color de la paleta que se encuentra en la RAM





// Funcion NF_GetTilePal();
extern u8 NF_GetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Devuelve que numero de paleta (0 - 15) esta usando el tile del fondo especificado.
// Por defecto, todos los tiles usan la paleta del Slot nº0
// Los datos se obtienen de la compia en RAM del mapa del fondo.





// Funcion NF_SetTilePal();
void NF_SetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u8 pal);
// Cambia el numero de paleta (0 - 15) que usara el tile del fondo especificado.
// Por defecto, todos los tiles usan la paleta del Slot nº0
// Los datos se escriben de la compia en RAM del mapa del fondo, por lo que no seran
// visibles hasta que ejecutes la funcion NF_UpdateVramMap();





// Funcion NF_LoadExBgPal();
void NF_LoadExBgPal(const char* file, u8 slot);
// Carga en el buffer de RAM correspondiente una paleta de fondos, para poderla usar
// mas tarde como paleta extendida.





// Funcion NF_UnloadExBgPal();
void NF_UnloadExBgPal(u8 slot);
// Borra de la RAM la paleta del slot especificado.





// Funcion NF_VramExBgPal();
void NF_VramExBgPal(u8 screen, u8 layer, u8 id, u8 slot);
// Transfiere a la VRAM una paleta extendida en el slot de la pantalla y
// fondo especificados.





// Funcion NF_SetExBgPal();
void NF_SetExBgPal(u8 screen, u8 layer, u8 pal);
// Cambia la paleta extendida que usara un fondo.
// La paleta debe de estar transferida en la VRAM previamente





// Funcion NF_SetTileHflip();
void NF_SetTileHflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Invierte horizontalmente el estado actual del tile seleccionado
// Los cambios no seran visibles hasta que actualices el mapa
// con la funcion NF_UpdateVramMap();





// Funcion NF_SetTileVflip();
void NF_SetTileVflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Invierte verticalmente el estado actual del tile seleccionado
// Los cambios no seran visibles hasta que actualices el mapa
// con la funcion NF_UpdateVramMap();




// Funcion NF_RotateTileGfx();
void NF_RotateTileGfx(u8 slot, u16 tile, u8 rotation);
// Rota el grafico de un tile especificado. Rota el tile almacenado en un buffer de fondos
// que se encuentra en RAM. Debes especificar el SLOT del buffer, numero de tile en el buffer
// y el angulo de la rotacion.
// 1 - 90º a la derecha
// 2 - 90º a la izquierda
// 3 - 180º





#endif

#ifdef __cplusplus
}
#endif
