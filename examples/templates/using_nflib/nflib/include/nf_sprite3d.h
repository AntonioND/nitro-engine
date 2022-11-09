#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_SPRITE3D_H__
#define __NF_SPRITE3D_H__





// NightFox LIB - Include de funciones 3D
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413



// Includes devKitPro
#include <nds.h>



//////////////////////////////////
// Defines y variables globales //
//////////////////////////////////

// Numero maximo de sprites en pantalla
#define NF_3DSPRITES 256

// Estructura de control de los sprites 3d
typedef struct {
	s16 x;					// Coordenada X
	s16 y;					// Coordenada Y
	s16 z;					// Coordenada Z
	s16 rx;					// Rotacion Eje X (-512/0/512) << 6
	s16 ry;					// Rotacion Eje Y (-512/0/512) << 6
	s16 rz;					// Rotacion Eje Z (-512/0/512) << 6
	bool rot;				// Rotacion en uso
	u16 sx;					// Escala X (0/64/512) << 6
	u16 sy;					// Escala Y (0/64/512) << 6
	bool scale;				// Escalado en uso
	s16 width;				// Ancho del sprite
	s16 height;				// Alto del sprite
	bool inuse;				// Esta en uso?
	bool show;				// Debe mostrarse el sprite?
	u32 gfx_tex_format;		// Guarda el formato de la textura
	u32 gfx;				// Direccion donde esta almacenado el grafico en VRAM
	u16 gfxid;				// Id de Gfx usado
	u16 frame;				// Frame actual
	u16 newframe;			// Frame al que cambiar
	u16 framesize;			// Tamaño del frame (en bytes)
	u16 lastframe;			// Ultimo frame
	u32 gfx_pal_format;		// Guarda el formato de la paleta
	u32 pal;				// Direccion donde esta almacenada la paleta en VRAM
	u16 palid;				// Id de la paleta usada
	u16 prio;				// Prioridad de dibujado del sprite
	u8 poly_id;				// Identificador unico para el Alpha (0 por defecto, 63 prohibido)
	u8 alpha;				// Nivel de alpha (0 - 31) (31 por defecto)
} NF_TYPE_3DSPRITE_INFO;
extern NF_TYPE_3DSPRITE_INFO NF_3DSPRITE[NF_3DSPRITES];

// Estructura de control Texturas en VRAM
typedef struct {
	u32 size;			// Tamaño (en bytes) del Gfx
	u16 width;			// Ancho del Gfx
	u16 height;			// Altura del Gfx
	u32 address;		// Posicion en la VRAM
	u16 ramid;			// Numero de Slot en RAM del que provienes
	u16 framesize;		// Tamaño del frame (en bytes)
	u16 lastframe;		// Ultimo frame
	bool keepframes;	// Si es un Sprite animado, debes de mantener los frames en RAM ?
	bool inuse;			// Disponibilidat del Slot
} NF_TYPE_TEX256VRAM_INFO;
extern NF_TYPE_TEX256VRAM_INFO NF_TEX256VRAM[NF_3DSPRITES];

// Estructura de control de las paletas en VRAM
typedef struct {
	bool inuse;		// Slot en uso
	u8 ramslot;		// Paleta original en RAM
} NF_TYPE_3DSPRPALSLOT_INFO;
extern NF_TYPE_3DSPRPALSLOT_INFO NF_TEXPALSLOT[32];

// Define la esturctura de control de la VRAM para Sprites 3d
typedef struct {
	s32 free;					// Memoria VRAM libre
	u32 next;					// Siguiente posicion libre
	u32 last;					// Ultima posicion usada
	u32 pos[NF_3DSPRITES];		// Posicion en VRAM para reusar despues de un borrado
	u32 size[NF_3DSPRITES];		// Tamaño del bloque libre para reusar
	u16 deleted;				// Numero de bloques borrados
	s32 fragmented;				// Memoria VRAM fragmentada
	s32 inarow;					// Memoria VRAM contigua
} NF_TYPE_TEXVRAM_INFO;
extern NF_TYPE_TEXVRAM_INFO NF_TEXVRAM;		// Informacion VRAM de texturas


// Define la estructura de control de los sprites 3d creados
typedef struct {
	s16 total;					// Numero de sprites creados
	u16 id[NF_3DSPRITES];		// ID del Sprite
	u16 bck[NF_3DSPRITES];		// Backup del ID
} NF_TYPE_CREATED_3DSPRITE_INFO;
extern NF_TYPE_CREATED_3DSPRITE_INFO NF_CREATED_3DSPRITE;





// Funcion NF_Init3dSpriteSys();
void NF_Init3dSpriteSys(void);
// Inicializa el sistema de Sprites en 3D


// Funcion NF_Vram3dSpriteGfx();
void NF_Vram3dSpriteGfx(u16 ram, u16 vram, bool keepframes);
// Transfiere un grafico de la RAM a la VRAM


// Funcion NF_Free3dSpriteGfx();
void NF_Free3dSpriteGfx(u16 id);
// Elimina de la VRAM un grafico de texturas y desfragmenta la VRAM si es necesario


// Funcion NF_Vram3dSpriteGfxDefrag();
void NF_Vram3dSpriteGfxDefrag(void);
// Desfragmenta la VRAM usada para texturas


// Funcion NF_Vram3dSpritePal();
void NF_Vram3dSpritePal(u8 id, u8 slot);
// Copia una paleta a la VRAM


// Funcion NF_Create3dSprite();
void NF_Create3dSprite(u16 id, u16 gfx, u16 pal, s16 x, s16 y);
// Crea un Sprite 3D en las coordenadas indicadas


// Funcion NF_Delete3dSprite();
void NF_Delete3dSprite(u16 id);
// Borra el Sprite con la ID indicada


// Funcion NF_Sort3dSprites();
void NF_Sort3dSprites(void);
// Reordena la cola de Sprites 3D creados de menor a mayor segun su ID
// Los Sprites con numeros mas bajos tienen prioridad.


// Funcion NF_Set3dSpritePriority();
void NF_Set3dSpritePriority(u16 id, u16 prio);
// Cambia la prioridad del Sprite


// Funcion NF_Swap3dSpritePriority();
void NF_Swap3dSpritePriority(u16 id_a, u16 id_b);
// Intercambia la prioridad de dos Sprites


// Funcion NF_Move3dSprite();
void NF_Move3dSprite(u16 id, s16 x, s16 y);
// Mueve el Sprite seleccionado a las coordenadas dadas


// Funcion NF_Show3dSprite();
void NF_Show3dSprite(u16 id, bool show);
// Muestra u oculta el sprite con la id indicada


// Funcion NF_Set3dSpriteFrame();
void NF_Set3dSpriteFrame(u16 id, u16 frame);
// Cambia el frame visible del sprite indicado


// Funcion NF_Draw3dSprites();
void NF_Draw3dSprites(void);
// Dibuja en pantalla todos los sprites creados


// Funcion NF_Update3dSpritesGfx();
void NF_Update3dSpritesGfx(void);
// Actualiza si es necesario las texturas de los sprites animados


// Funcion NF_Rotate3dSprite();
void NF_Rotate3dSprite(u16 id, s16 x, s16 y, s16 z);
// Rota el Sprite sobre los ejes indicados (-512/0/512)


// Funcion NF_Scale3dSprite();
void NF_Scale3dSprite(u16 id, u16 x, u16 y);
// Escala el sprite al tamaño indicado (0/64/512)


// Funcion NF_Blend3dSprite();
void NF_Blend3dSprite(u8 sprite, u8 poly_id, u8 alpha);
// Habilita y cambia el nivel de alpha de el sprite 3d indicado. Para que la transparencia
// sea efectiva entre Sprites, debes especificar un poly_id diferente para cada sprite
// (entre 1 y 62). El rango de alpha es de 0 a 31, siendo 31 opaco. Para eliminar la
// transparencia, selecciona un valor para alpha de 31 o especifica como poly_id el nº 0.


// Funcion NF_3dSpritesLayer();
void NF_3dSpritesLayer(u8 layer);
// Selecciona la capa en la que se dibujaran los Sprites 3D. (0 - 3)
// En realidad los Sprites 3D siempre se dibujan sobre la CAPA 0, esta funcion solo cambia
// la prioridad de esta capa sobre las demas.


// Funcion NF_3dSpriteEditPalColor();
void NF_3dSpriteEditPalColor(u8 pal, u8 number, u8 r, u8 g, u8 b);
// Modifica los valores de la paleta seleccionada. Las modificaciones se efectuan
// sobre la copia en RAM, por lo que los cambios no seran visibles hasta que se
// transfiera la paleta a la VRAM


// Funcion 	NF_3dSpriteUpdatePalette();
void NF_3dSpriteUpdatePalette(u8 pal);
// Actualiza la paleta en VRAM con la copia que se encuentra en la RAM


// Funcion NF_3dSpriteGetPalColor();
void NF_3dSpriteGetPalColor(u8 pal, u8 number, u8* r, u8* g, u8* b);
// Obtiene el valor de un color de la paleta que se encuentra en la RAM


// Funcion NF_3dSpriteSetDeep();
void NF_3dSpriteSetDeep(u8 id, s16 z);
// Cambia la profuncidad de dibujado (z) del sprite (-512/0/512),
// siendo -512 el punto mas cercano, 0 el centro por defecto
// y 512 el punto mas lejano.





#endif

#ifdef __cplusplus
}
#endif
