#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_BITMAPBG_H__
#define __NF_BITMAPBG_H__





// NightFox LIB - Include de funciones de fondos en modo Bitmap
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413



// Includes devKitPro
#include <nds.h>




// Define los slots maximos para los fondos de 16 bits
#define NF_SLOTS_BG16B 16

// Define los slots maximos para los fondos de 8 bits
#define NF_SLOTS_BG8B 16

// Define los Buffers para almacenar datos de 16 bits
typedef struct {
	u16* buffer;	// Buffer de datos
	u32 size;		// Tamaño del buffer
	u16 width;		// Ancho de la imagen (256 max)
	u16 height;		// Altura de la imagen (256 max)
	bool inuse;		// Esta en uso el buffer?
} NF_TYPE_BG16B_INFO;
extern NF_TYPE_BG16B_INFO NF_BG16B[NF_SLOTS_BG16B];		// Fondos RAW de 16 bits

// BackBuffer de 16 bits de cada pantalla
extern u16* NF_16BITS_BACKBUFFER[2];

// Define los Buffers para almacenar datos de 8 bits
typedef struct {
	u8* data;			// Buffer de datos
	u32 data_size;		// Tamaño del buffer de datos
	u16* pal;			// Buffer para la paleta
	u32 pal_size;		// Tamaño de la paleta
	bool inuse;		// Esta en uso el buffer?
} NF_TYPE_BG8B_INFO;
extern NF_TYPE_BG8B_INFO NF_BG8B[NF_SLOTS_BG8B];	// Fondos indexados de 8 bits

// BackBuffer de 8 bits de cada pantalla
typedef struct {
	u8* data;
	u16* pal;
} NF_TYPE_BB8B_INFO;
extern NF_TYPE_BB8B_INFO NF_8BITS_BACKBUFFER[2];







// Funcion NF_Init16bitsBgBuffers();
void NF_Init16bitsBgBuffers(void);
// Inicia los buffers para almacenar fondos de BITMAP 16 bits
// Usalo UNA SOLA VEZ antes de usar los buffers



// Funcion NF_Reset16bitsBgBuffers();
void NF_Reset16bitsBgBuffers(void);
// Reinicia los buffers para almacenar fondos de BITMAP 16 bits,
// borrado su contenido



// Funcion NF_Init16bitsBackBuffer();
void NF_Init16bitsBackBuffer(u8 screen);
// Inicia el backbuffer de la pantalla dada.
// Usalo UNA SOLA VEZ antes de usar el backbuffer



// Funcion NF_Enable16bitsBackBuffer();
void NF_Enable16bitsBackBuffer(u8 screen);
// Habilita el backbuffer de la pantalla indicada.
// Si el buffer ya existe, la funcion borrara el contenido del mismo,
// poniendo a 0 todos los bytes del buffer.



// Funcion NF_Disble16bitsBackBuffer();
void NF_Disble16bitsBackBuffer(u8 screen);
// Deshabilita el backbuffer de la pantalla indicada, liberando de la
// RAM el espacio usado.



// Funcion NF_Flip16bitsBackBuffer();
void NF_Flip16bitsBackBuffer(u8 screen);
// Copia el contenido del backbuffer a la VRAM de la pantalla indicada.



// Funcion NF_InitBitmapBgSys();
void NF_InitBitmapBgSys(u8 screen, u8 mode);
// Inicia el modo BITMAP en la pantalla especificada, con la profundidad de color
// especificada. (0 -> 256 colores, 1 -> 16 bits)



// Funcion NF_Load16bitsBg();
void NF_Load16bitsBg(const char* file, u8 slot);
// Carga un Fondo BITMAP (en formato RAW) en el slot indicado,
// de un tamaño maximo de 256x256 pixeles.
// Debes especificar el nombre de archivo sin extension.
// El archivo debe ser un binario con extension .img



// Funcion NF_Load16bitsImage();
void NF_Load16bitsImage(const char* file, u8 slot, u16 size_x, u16 size_y);
// Carga una imagen BITMAP (en formato RAW) en el slot indicado,
// de un tamaño maximo de 256x256 pixeles.
// Debes especificar el nombre de archivo sin extension.
// El archivo debe ser un binario con extension .img
// Todos los pixeles Magenta de la imagen (0xFF00FF) seran transparentes.



// Funcion NF_Load16bImgData();
void NF_Load16bImgData(const char* file, u8 slot, u16 x, u16 y, u8 type);
// Funcion de uso interno, no documentar ni usar.
// Cargador generico de datos de imagen de 16 bits a la RAM.



// Funcion NF_Unload16bitsBg();
void NF_Unload16bitsBg(u8 slot);
// Borra de la RAM los datos del buffer especificado



// Funcion NF_Copy16bitsBuffer();
void NF_Copy16bitsBuffer(u8 screen, u8 destination, u8 slot);
// Copia los datos cargados en un Buffer de Bitmap a la VRAM o al BackBuffer
// destination: 0 -> VRAM, 1 -> BackBuffer



// Funcion NF_Draw16bitsImage();
void NF_Draw16bitsImage(u8 screen, u8 slot, s16 x, s16 y, bool alpha);
// Copia la imagen cargada en un Buffer de Bitmap al Backbuffer de la pantalla especificada



// Funcion NF_Init8bitsBgBuffers();
void NF_Init8bitsBgBuffers(void);
// Inicializa los buffers necesarios para la carda de fondos en modo BITMAP de 8 bits.
// Debes usar esta funcion antes de poder usar ninguno de los buffers.



// Funcion NF_Reset8bitsBgBuffers();
void NF_Reset8bitsBgBuffers(void);
// Reinicia los buffers de fondos de 8 bits, borrando todo el contenido de los buffers.



// Funcion NF_Load8bitsBg();
void NF_Load8bitsBg(const char* file, u8 slot);
// Carga los archivos necesarios de un fondo de 8 bits en el Slot indicado.
// Debes especificar el nombre de archivo sin extension.
// El archivo debe ser un binario con extension .img
// La paleta debe tener la extension .pal



// Funcion NF_Unload8bitsBg();
void NF_Unload8bitsBg(u8 slot);
// Borra de la RAM el fondo del slot indicado.



// Funcion NF_Copy8bitsBuffer();
void NF_Copy8bitsBuffer(u8 screen, u8 destination, u8 slot);
// Transfiere la imagen del buffer seleccionado a la pantalla y capas indicado o
// al backbuffer.



// Funcion NF_Init8bitsBackBuffer();
void NF_Init8bitsBackBuffer(u8 screen);
// Inicializa los buffers del backbuffer para la pantalla indicada.
// Debes usar esta funcion una vez antes de usar el backbuffer



// Funcion NF_Enable8bitsBackBuffer();
void NF_Enable8bitsBackBuffer(u8 screen);
// Habilita el backbuffer de la pantalla indicada.
// Si el buffer ya existe, la funcion borrara el contenido del mismo,
// poniendo a 0 todos los bytes del buffer.



// Funcion NF_Disble8bitsBackBuffer();
void NF_Disble8bitsBackBuffer(u8 screen);
// Deshabilita el backbuffer de la pantalla indicada, liberando de la
// RAM el espacio usado.



// Funcion NF_Flip8bitsBackBuffer();
void NF_Flip8bitsBackBuffer(u8 screen, u8 destination);
// Copia el contenido del backbuffer a la VRAM de la pantalla indicada.
// Debes especificar la capa de destino:
// 0 -> Capa 2
// 1 -> Capa 3





#endif

#ifdef __cplusplus
}
#endif
