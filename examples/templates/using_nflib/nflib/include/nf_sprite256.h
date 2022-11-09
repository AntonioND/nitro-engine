#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_SPRITE256_H__
#define __NF_SPRITE256_H__





// NightFox LIB - Include de Sprites a 256 colores
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>





// Define los Slots para sprites
#define NF_SLOTS_SPR256GFX 256		// Almacena los graficos
#define NF_SLOTS_SPR256PAL 64		// Almacena las paletas

// Define los Buffers para almacenar los Sprites
extern char* NF_BUFFER_SPR256GFX[NF_SLOTS_SPR256GFX];
extern char* NF_BUFFER_SPR256PAL[NF_SLOTS_SPR256PAL];

// Define la estructura de datos de los buffers (GFX)
typedef struct {
	u32 size;			// Tamaño (en bytes) del grafico (GFX)
	u16 width;			// Ancho del Gfx
	u16 height;			// Altura del Gfx
	bool available;		// Disponibilidat del Slot
} NF_TYPE_SPR256GFX_INFO;
extern NF_TYPE_SPR256GFX_INFO NF_SPR256GFX[NF_SLOTS_SPR256GFX];		// Buffers de Graficos

// Define la estructura de datos de los buffers (PAL)
typedef struct {
	u32 size;			// Tamaño (en bytes) de la paleta (PAL)
	bool available;		// Disponibilidat del Slot
} NF_TYPE_SPR256PAL_INFO;
extern NF_TYPE_SPR256PAL_INFO NF_SPR256PAL[NF_SLOTS_SPR256PAL];		// Buffers de Paletas

// Define la estructura de Gfx en VRAM
typedef struct {
	u32 size;			// Tamaño (en bytes) del Gfx
	u16 width;			// Ancho del Gfx
	u16 height;			// Altura del Gfx
	u32 address;			// Posicion en la VRAM
	u16 ramid;			// Numero de Slot en RAM del que provienes
	u16 framesize;		// Tamaño del frame (en bytes)
	u16 lastframe;		// Ultimo frame
	bool keepframes;	// Si es un Sprite animado, debes de mantener los frames en RAM ?
	bool inuse;			// Disponibilidat del Slot
} NF_TYPE_SPR256VRAM_INFO;
extern NF_TYPE_SPR256VRAM_INFO NF_SPR256VRAM[2][128];

// Datos de paletas de Sprites en VRAM (en uso, slot en ram, etc)
typedef struct {
	bool inuse;		// Slot en uso
	u8 ramslot;		// Paleta original en RAM
} NF_TYPE_SPRPALSLOT_INFO;
extern NF_TYPE_SPRPALSLOT_INFO NF_SPRPALSLOT[2][16];

// Define la estructura de datos del OAM (Sprites)
typedef struct {
	u8 index;			// Numero de Sprite
	s16 x;				// Coordenada X del Sprite
	s16 y;				// Coordenada Y del Sprite
	u8 layer;			// Prioridad en las capas
	u8 pal;				// Paleta que usaras
	u32 size;			// Tamaño del Sprite (macro)
	u32 color;			// Modo de color (macro)
	u32* gfx;			// Puntero al grafico usado
	s8 rot;				// Id de rotacion (-1 ninguno) (0 - 31 Id de rotacion)
	bool doublesize;	// Usar el "double size" al rotar ?
	bool hide;			// Ocultar el Sprite
	bool vflip;			// Volteado Vertical
	bool hflip;			// Volteado Horizontal
	bool mosaic;		// Mosaico
	u16 gfxid;			// Id de Gfx usado
	u16 frame;			// Frame actual
	u16 framesize;		// Tamaño del frame (en bytes)
	u16 lastframe;		// Ultimo frame
	bool created;		// Esta creado este sprite ?
} NF_TYPE_SPRITEOAM_INFO;
extern NF_TYPE_SPRITEOAM_INFO NF_SPRITEOAM[2][128];		// 2 pantallas, 128 sprites

// Define la esturctura de control de la VRAM para Sprites
typedef struct {
	s32 free;			// Memoria VRAM libre
	u32 next;			// Siguiente posicion libre
	u32 last;			// Ultima posicion usada
	u32 pos[128];		// Posicion en VRAM para reusar despues de un borrado
	u32 size[128];		// Tamaño del bloque libre para reusar
	u16 deleted;		// Numero de bloques borrados
	s32 fragmented;		// Memoria VRAM fragmentada
	s32 inarow;			// Memoria VRAM contigua
	s32 max;			// Maxima memoria VRAM direccionable
} NF_TYPE_SPRVRAM_INFO;
extern NF_TYPE_SPRVRAM_INFO NF_SPRVRAM[2];		// Informacion VRAM de Sprites en ambas pantallas






// Funcion NF_InitSpriteBuffers()
void NF_InitSpriteBuffers(void);
// Inicializa los buffers y estructuras de datos de los Buffers para almacenar Sprites
// Usala antes de cargar cualquier Sprites
// No uses esta funcion mas de una vez en tu codigo



// Funcion NF_ResetSpriteBuffers()
void NF_ResetSpriteBuffers(void);
// Reinicia los buffers y datos para almacenar Sprites



// Funcion NF_InitSpriteSys();
void NF_InitSpriteSys(int screen, ...);
// Inicializa las variables de control de Sprites y paletas
// Asigna 128kb de RAM para Sprites
// Activa el Soporte para Sprites
// Se debe especificar la pantalla (0 o 1)



// Funcion NF_LoadSpriteGfx();
void NF_LoadSpriteGfx(const char* file, u16 id, u16 width, u16 height);
// Carga un grafico para usarlo despues en la creacion de Sprites
// Especifica el archivo, ID exclusivo (0 - 255) y medidas del grafico



// Funcion NF_UnloadSpriteGfx();
void NF_UnloadSpriteGfx(u16 id);
// Borra de la RAM un Gfx cargado previamente con NF_LoadSpriteGfx();
// Especifica la Id exclusiva (0 - 255) de grafico a borrar.



// Funcion NF_LoadSpritePal();
void NF_LoadSpritePal(const char* file, u8 id);
// Carga una paleta para usarla despues en la creacion de Sprites
// Especifica el archivo y ID exclusivo (0 - 63)



// Funcion NF_UnloadSpritePal();
void NF_UnloadSpritePal(u8 id);
// Borra de la RAM una Paleta cargada previamente con NF_LoadSpritePal();
// Especifica la Id exclusiva (0 - 63) de la paleta a borrar.



// Funcion NF_VramSpriteGfx();
void NF_VramSpriteGfx(u8 screen, u16 ram, u16 vram, bool keepframes);
// Transfiere un Grafico cargado para Sprites a la VRAM
// Debes especificar la pantalla, el Id del Grafico en RAM (origen),
// la Id del grafico en VRAM (destino) y si, en caso de estar animado,
// debe de copiar a la VRAM todos los frames o solo el que este en uso



// Funcion NF_FreeSpriteGfx();
void NF_FreeSpriteGfx(u8 screen, u16 id);
// Borra de la VRAM el Grafico con la Id. especificada.
// Cualquier Sprite que este usando ese Gfx, quedara corrupto.



// Funcion NF_VramSpriteGfxDefrag();
void NF_VramSpriteGfxDefrag(u8 screen);
// Desfragmenta la VRAM de Sprites (Gfx) de la pantalla dada
// Esta se realiza automaticamente al borrar un Gfx de la VRAM
// si la memoria fragmentada es superior a la contigua libre.
// Puedes consultar estos valores con las siguientes variables
// NF_SPRVRAM[u8 screen].free			<- Memoria VRAM de Sprites libre
// NF_SPRVRAM[u8 screen].fragmented		<- Memoria VRAM de sprites libre fragmentada
// NF_SPRVRAM[u8 screen].inarow			<- Memoria VRAM de sprites libre, ultimo bloque mas grande
// NF_SPRVRAM[u8 screen].lost			<- Memoria VRAM libre no usable por fragmentacion



// Funcion NF_VramSpritePal();
void NF_VramSpritePal(u8 screen, u8 id, u8 slot);
// Transfiere una Paleta cargada para Sprites a la VRAM
// Debes especificar la pantalla, la Id de la Paleta a trasnferir
// y el slot de destino (0 - 15)



// Funcion NF_CreateSprite();
void NF_CreateSprite(u8 screen, u8 id, u16 gfx, u8 pal, s16 x, s16 y);
// Crea un sprite en la pantalla.
// Debes especificar la pantalla, la ID del Sprite (0 - 127),
// la ID del Grafico que usaras (0 - 255), el slot de paleta (0 - 15),
// y las coordenadas donde lo crearas.



// Funcion NF_DeleteSprite();
void NF_DeleteSprite(u8 screen, u8 id);
// Borra un sprite de la pantalla
// Debes especificar la pantalla y la Id del Sprite a borrar



// Funcion NF_SpriteOamSet();
void NF_SpriteOamSet(u8 screen);
// Copia los datos del array propio de OAM al OAM real
// Debes especificar la pantalla





// Funcion NF_SpriteSetPalColor();
void NF_SpriteSetPalColor(u8 screen, u8 pal, u8 number, u8 r, u8 g, u8 b);
// Cambia el valor de un color de la paleta de sprites especificada.
// Debes especificar la pantalla, slot de la paleta en VRAM, numero de color
// a cambiar dentro de la paleta y el nuevo valor a darle en formato RGB



// Funcion NF_SpriteEditPalColor();
void NF_SpriteEditPalColor(u8 screen, u8 pal, u8 number, u8 r, u8 g, u8 b);
// Modifica los valores de la paleta seleccionada. Las modificaciones se efectuan
// sobre la copia en RAM, por lo que los cambios no seran visibles hasta que se
// transfiera la paleta a la VRAM



// Funcion 	NF_SpriteUpdatePalette();
void NF_SpriteUpdatePalette(u8 screen, u8 pal);
// Actualiza la paleta en VRAM con la copia que se encuentra en la RAM




// Funcion NF_SpriteGetPalColor();
void NF_SpriteGetPalColor(u8 screen, u8 pal, u8 number, u8* r, u8* g, u8* b);
// Obtiene el valor de un color de la paleta que se encuentra en la RAM





#endif

#ifdef __cplusplus
}
#endif
