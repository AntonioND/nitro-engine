/****************************************************************************
*****************************************************************************
****       _   _ _ _               ______             _                  ****
****      | \ | (_) |             |  ____|           (_)                 ****
****      |  \| |_| |_ _ __ ___   | |__   _ __   __ _ _ _ __   ___       ****
****      | . ` | | __| '__/ _ \  |  __| | '_ \ / _` | | '_ \ / _ \      ****
****      | |\  | | |_| | | (_) | | |____| | | | (_| | | | | |  __/      ****
****      |_| \_|_|\__|_|  \___/  |______|_| |_|\__, |_|_| |_|\___|      ****
****                                             __/ |                   ****
****                                            |___/      V 0.6.1       ****
****                                                                     ****
****                     Copyright (C) 2008 - 2011 Antonio Niño Díaz     ****
****                                   All rights reserved.              ****
****                                                                     ****
*****************************************************************************
****************************************************************************/

/****************************************************************************
*                                                                           *
* Nitro Engine V 0.6.1 is licensed under the terms of <readme_license.txt>. *
* If you have any question, email me at <antonio_nd@hotmail.com>.           *
*                                                                           *
****************************************************************************/

#ifndef _NE_ALLOC_
#define _NE_ALLOC_

/*! \file   NEAlloc.h
 *  \brief  VRAM handling. 
 
Internal use, used to handle VRAM when other function requests memory. */

//----------------------------------------------------------------------------

typedef enum {
	NE_STATE_FREE,
	NE_STATE_USED,
	NE_STATE_LOCKED
} ne_chunk_state;

typedef struct {
	void * previous, * next; //pointers to other chunks, NULL if start or end
	
	ne_chunk_state status; //used, free or locked
	void * start, * end; //pointers to the start and end of the memory range
} NEChunk;

typedef struct {
	int Free, Used, Total, Locked; //in bytes. total does not count locked memory
	int FreePercent; //locked memory doesn't count
} NEMemInfo;

//----------------------------------------------------------------------------

void NE_AllocInit(NEChunk ** first_element, void * start, void * end);
void NE_AllocEnd(NEChunk * first_element);

void * NE_Alloc(NEChunk * first_element, int size, int align);
void NE_Free(NEChunk * first_element, void * pointer);

void NE_Lock(NEChunk * first_element, void * pointer);
void NE_Unlock(NEChunk * first_element, void * pointer);

//int NE_GetSize(NEChunk * first_chunk, void * pointer);
void NE_MemGetInformation(NEChunk * first_element, NEMemInfo * info);

//----------------------------------------------------------------------------

#endif //_NE_ALLOC_
