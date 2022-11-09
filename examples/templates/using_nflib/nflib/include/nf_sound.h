#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_SOUND_H__
#define __NF_SOUND_H__





// NightFox LIB - Include de funciones de sonido
// Requiere DevkitARM
// Codigo por Cesar Rincon "NightFox"
// http://www.nightfoxandco.com/
// Version 20140413





// Includes devKitPro
#include <nds.h>





// Define los Slots para archivos de audio
#define NF_SLOTS_RAWSOUND 32

// Define los Buffers para almacenar los archivos de audio
extern char* NF_BUFFER_RAWSOUND[NF_SLOTS_RAWSOUND];

// Define la estructura de datos de los buffers (Audio)
typedef struct {
	bool available;		// Disponibilidat del Slot
	u32 size;			// Tamaño (en bytes) del sonido
	u16 freq;			// Frecuencia del sample
	u8 format;			// Formato del sample
} NF_TYPE_RAWSOUND_INFO;
extern NF_TYPE_RAWSOUND_INFO NF_RAWSOUND[NF_SLOTS_RAWSOUND];	// Datos de los sonidos cargado





// Funcion NF_InitRawSoundBuffers();
void NF_InitRawSoundBuffers(void);
// Iniciliaza los buffers y estructuras de datos para cargar sonidos en 
// formato RAW. Debes de ejecutar esta funcion una vez antes de cargar
// ningun sonido en este formato



// Funcion NF_ResetRawSoundBuffers();
void NF_ResetRawSoundBuffers(void);
// Reinicia todos los buffers de sonido. Esta funcion es util para vaciar todos los datos
// en un cambio de pantalla, etc.



// Funcion NF_LoadRawSound();
void NF_LoadRawSound(const char* file, u16 id,  u16 freq, u8 format);
// Carga un archivo RAW a la RAM desde la FAT o EFS
// Debes especificar el nombre del archivo sin extension, el slot
// donde lo guardaras (0 - 31), la frecuencia del sample (en Hz, 
// por ejemplo 11050) y el formato de datos
// (0 - > 8 bits, 1 - > 16 bits, 2 -> ADPCM)



// Funcion UnloadRawSound();
void NF_UnloadRawSound(u8 id);
// Borra de la RAM el archivo cargado en el slot indicado.
// Debes especificar el slot a borrar (0 - 31)



// Funcion NF_PlayRawSound();
extern u8 NF_PlayRawSound(u8 id, u8 volume, u8 pan, bool loop, u16 loopfrom);
// Reproduce un archivo de sonido cargado en RAM.
// Debes especificar el slot donde se ha cargado el sonido, el volumen (0 - 127), 
// el balance (0 Izquierda, 64 Centro, 127 Derecha), si se reproducira en bucle y
// de ser asi, a partir de que sample se producira este.
// Esta funcion devuelve el canal asignado a esta reproduccion





#endif

#ifdef __cplusplus
}
#endif
