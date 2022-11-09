#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_TEXT_H__
#define __NF_TEXT_H__





// NightFox LIB - Include de Textos
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>



// Define el nº de caracteres que tiene la fuente
#define NF_TEXT_FONT_CHARS 127
#define NF_TEXT_FONT_LAST_VALID_CHAR 113


// Define la estructura de control de textos
typedef struct {
	u16 width;		// Ultimo tile de la fila (0 - x) 32 tiles serian 0 - 31
	u16 height;		// Ultimo tile de la columna (0 - y)
	u8 rotation;	// Rotacion del texto
	u8 slot;		// Slot donde esta cargado el tileset de esta capa de texto
	u8 pal;			// Paleta que usara el texto (0 por defecto)
	bool exist;		// Existe la capa de texto?
	bool update;	// Tienes que actualizar la capa?
} NF_TYPE_TEXT_INFO;
extern NF_TYPE_TEXT_INFO NF_TEXT[2][4];		// Datos de las capas de texto



// Funcion NF_InitTextSys();
void NF_InitTextSys(u8 screen);
// Inicializa el sistema de Texto para la pantalla dada



// Funcion NF_LoadTextFont();
void NF_LoadTextFont(const char* file, const char* name, u16 width, u16 height, u8 rotation);
// Carga una fuente para usar como texto
// La fuente se cargara en un slot libre de fondos tileados
// Debes especificar el archivos sin extension y un nombre para referenciarla
// En caso de que la fuente tenga los sets de rotacion a izquierda y derecha,
// especificar 1 o 2 en el parametro "rot". 0 carga la fuente sin rotacion



// Funcion NF_UnloadTextFont();
void NF_UnloadTextFont(const char* name);
// Borra un fuente cargada en RAM
// Esta funcion simplemente llama a NF_UnloadTiledBg(); para su borrado



// Funcion NF_CreateTextLayer();
void NF_CreateTextLayer(u8 screen, u8 layer, u8 rotation, const char* name);
// Crea un fondo tileado para usarlo con texto
// Esta funcion simplemente llama a NF_CreateTiledBg(); para su creacion



// Funcion NF_DeleteTextLayer();
void NF_DeleteTextLayer(u8 screen, u8 layer);
// Borra un fondo usado como capa de texto y sus buffers y variables asociadas



// Funcion NF_WriteText();
void NF_WriteText(u8 screen, u8 layer, u16 x, u16 y, const char* text);
// Escribe un texto en el buffer de texto de la pantalla y capa seleccionada



// Funcion NF_UpdateTextLayers();
void NF_UpdateTextLayers(void);
// Copia el buffer de texto a la VRAM en las capas que sea necesario
// realizar una actualizacion




// Funcion NF_ClearTextLayer();
void NF_ClearTextLayer(u8 screen, u8 layer);
// Borra el contanido de la capa de texto seleccionada





// Funcion NF_DefineTextColor();
void NF_DefineTextColor(u8 screen, u8 layer, u8 color, u8 r, u8 g, u8 b);
// Define uno de los 16 colores disponibles para texto, en formato RGB





// Function NF_SetTextColor();
void NF_SetTextColor(u8 screen, u8 layer, u8 color);
// Selecciona con que color definido se escribira el texto (no cambia el color del texto ya escrito)





#endif

#ifdef __cplusplus
}
#endif
