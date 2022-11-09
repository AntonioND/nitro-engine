
// NightFox LIB - Funciones de de funciones de sonido
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
#include "nf_basic.h"
#include "nf_sound.h"





// Define los Buffers para almacenar los archivos de audio
char* NF_BUFFER_RAWSOUND[NF_SLOTS_RAWSOUND];

// Datos de los sonidos cargado
NF_TYPE_RAWSOUND_INFO NF_RAWSOUND[NF_SLOTS_RAWSOUND];





// Funcion NF_InitRawSoundBuffers();
void NF_InitRawSoundBuffers(void) {

	u8 n = 0;	// Variable comun

	// Inicializa Buffers de sonido en RAW
	for (n = 0; n < NF_SLOTS_RAWSOUND; n ++) {
		NF_BUFFER_RAWSOUND[n] = NULL;			// Inicializa puntero
		NF_RAWSOUND[n].available = true;		// Disponibilidad del slot
		NF_RAWSOUND[n].size = 0;				// Tamaño del archivo
		NF_RAWSOUND[n].freq = 0;				// Frecuencia del sample
		NF_RAWSOUND[n].format = 0;				// Formato del sample
	}

}





// Funcion NF_ResetRawSoundBuffers();
void NF_ResetRawSoundBuffers(void) {

	u8 n = 0;	// Variable comun

	// Borra los datos de los buffers de sonido en RAW
	for (n = 0; n < NF_SLOTS_RAWSOUND; n ++) {
		free(NF_BUFFER_RAWSOUND[n]);			// Borra el contenido puntero
	}

	// Reinicia las estructuras de datos
	NF_InitRawSoundBuffers();

}





// Funcion NF_LoadRawSound();
void NF_LoadRawSound(const char* file, u16 id,  u16 freq, u8 format) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_RAWSOUND)) {
		NF_Error(106, "Raw Sound", NF_SLOTS_RAWSOUND);
	}

	// Verifica si la Id esta libre
	if (!NF_RAWSOUND[id].available) {
		NF_Error(109, "Raw Sound", id);
	}

	// Vacia los buffers que se usaran
	free(NF_BUFFER_RAWSOUND[id]);
	NF_BUFFER_RAWSOUND[id] = NULL;

	// Declara los punteros a los ficheros
	FILE* file_id;

	// Variable para almacenar el path al archivo
	char filename[256];

	// Carga el archivo .RAW
	sprintf(filename, "%s/%s.raw", NF_ROOTFOLDER, file);
	file_id = fopen(filename, "rb");
	if (file_id) {	// Si el archivo existe...
		// Obten el tamaño del archivo
		fseek(file_id, 0, SEEK_END);
		NF_RAWSOUND[id].size = ftell(file_id);
		rewind(file_id);
		// Si excede del tamaño maximo, error
		if (NF_RAWSOUND[id].size > (1 << 18)) NF_Error(116, filename, (1 << 18));
		// Reserva el espacio en RAM
		NF_BUFFER_RAWSOUND[id] = (char*) calloc (NF_RAWSOUND[id].size, sizeof(char));
		if (NF_BUFFER_RAWSOUND[id] == NULL) {		// Si no hay suficiente RAM libre
			NF_Error(102, NULL, NF_RAWSOUND[id].size);
		}
		// Lee el archivo y ponlo en la RAM
		fread(NF_BUFFER_RAWSOUND[id], 1, NF_RAWSOUND[id].size, file_id);
	} else {	// Si el archivo no existe...
		NF_Error(101, filename, 0);
	}
	fclose(file_id);		// Cierra el archivo
	// swiWaitForVBlank();		// Espera al cierre del archivo (Usar en caso de corrupcion de datos)

	// Guarda los parametros del archivo de sonido
	NF_RAWSOUND[id].freq = freq;		// Frequencia del sample (en Hz)
	NF_RAWSOUND[id].format = format;	// Formato del sample (0 - > 8 bits, 1 - > 16 bits, 2 -> ADPCM)

	// Y marca esta ID como usada
	NF_RAWSOUND[id].available = false;

}





// Funcion UnloadRawSound();
void NF_UnloadRawSound(u8 id) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_RAWSOUND)) NF_Error(106, "RAW Sound", NF_SLOTS_RAWSOUND); 

	// Verifica si el sonido existe
	if (NF_RAWSOUND[id].available) NF_Error(110, "RAW Sound", id);

	// Vacia los buffers de la Id. seleccionada
	free(NF_BUFFER_RAWSOUND[id]);
	NF_BUFFER_RAWSOUND[id] = NULL;

	// Resetea las variables
	NF_RAWSOUND[id].freq = 0;		// Frequencia del sample (en Hz)
	NF_RAWSOUND[id].format = 0;	// Formato del sample (0 - > 8 bits, 1 - > 16 bits, 2 -> ADPCM)

	// Y marca esta ID como libre
	NF_RAWSOUND[id].available = true;

}





// Funcion NF_PlayRawSound();
u8 NF_PlayRawSound(u8 id, u8 volume, u8 pan, bool loop, u16 loopfrom) {

	// Verifica el rango de Id's
	if ((id < 0) || (id >= NF_SLOTS_RAWSOUND)) NF_Error(106, "RAW Sound", NF_SLOTS_RAWSOUND); 

	// Verifica si el sonido existe
	if (NF_RAWSOUND[id].available) NF_Error(110, "RAW Sound", id);

	return soundPlaySample(NF_BUFFER_RAWSOUND[id], NF_RAWSOUND[id].format, NF_RAWSOUND[id].size, NF_RAWSOUND[id].freq, volume, pan, loop, loopfrom);

}
