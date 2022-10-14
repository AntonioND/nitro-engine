// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#ifndef NE_ALLOC_H__
#define NE_ALLOC_H__

/// @file   NEAlloc.h
/// @brief  VRAM memory allocator
///
/// Internal use, used to handle VRAM when other function requests memory.

typedef enum {
    NE_STATE_FREE,
    NE_STATE_USED,
    NE_STATE_LOCKED
} ne_chunk_state;

typedef struct {
    void *previous, *next; // Pointers to other chunks, NULL if start or end

    ne_chunk_state status; // Used, free or locked
    void *start, *end;     // Pointers to the start and end of the pool
} NEChunk;

typedef struct {
    // Values in bytes. Total memory does not include locked memory
    size_t Free, Used, Total, Locked;
    unsigned int FreePercent; // Locked memory doesn't count
} NEMemInfo;

void NE_AllocInit(NEChunk **first_element, void *start, void *end);
void NE_AllocEnd(NEChunk *first_element);

void *NE_Alloc(NEChunk *first_element, size_t size, unsigned int align);
void NE_Free(NEChunk *first_element, void *pointer);

void NE_Lock(NEChunk *first_element, void *pointer);
void NE_Unlock(NEChunk *first_element, void *pointer);

//int NE_GetSize(NEChunk *first_chunk, void *pointer);
void NE_MemGetInformation(NEChunk *first_element, NEMemInfo *info);

#endif // NE_ALLOC_H__
