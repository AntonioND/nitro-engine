// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
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

typedef struct NEChunk {
    struct NEChunk *previous; // Pointer to previous chunk. NULL if this is the first one
    struct NEChunk *next;     // Pointer to next chunk. NULL if this is the last one
    ne_chunk_state state;     // Used, free or locked
    void *start, *end;        // Pointers to the start and end of this memory chunk
} NEChunk;

typedef struct {
    // Values in bytes. Total memory does not include locked memory
    size_t free, used, total, locked;
    unsigned int free_percent; // Locked memory doesn't count
} NEMemInfo;

#define NE_ALLOC_MIN_SIZE (16)

// They return 0 on success. On error, they return a negative number.
int NE_AllocInit(NEChunk **first_element, void *start, void *end);
int NE_AllocEnd(NEChunk **first_element);

// This function takes a memory range defined by ["start", "end"] and tries to
// look a chunk of free memory that is at least as big as "size". It doesn't
// allocate it, that needs to be done with NE_AllocAddress(). On error, this
// function returns NULL.
void *NE_AllocFindInRange(NEChunk *first_chunk, void *start, void *end, size_t size);

// It returns 0 on success. On error, it returns a negative number.
int NE_AllocAddress(NEChunk *first_chunk, void *address, size_t size);

// Allocates data at the first available space starting from the start of the
// memory pool. Returns NULL on error, or a valid pointer on success.
void *NE_Alloc(NEChunk *first_element, size_t size);

// Allocates data at the first available space starting from the end of the
// memory pool. Returns NULL on error, or a valid pointer on success.
void *NE_AllocFromEnd(NEChunk *first_element, size_t size);

// Returns 0 on success. On error, it returns a negative number.
int NE_Free(NEChunk *first_element, void *pointer);

// Only an allocated chunk of memory can be locked. After it is locked, it stops
// counting towards the total memory reported by NE_MemGetInformation(). When it
// is unlocked, it is considered to still be allocated, and it needs to be freed
// manually.
//
// They return 0 on success. On error, they returns a negative number.
int NE_Lock(NEChunk *first_element, void *pointer);
int NE_Unlock(NEChunk *first_element, void *pointer);

// Returns 0 on success. On error, it returns a negative number.
int NE_MemGetInformation(NEChunk *first_element, NEMemInfo *info);

#endif // NE_ALLOC_H__
