// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <stdlib.h>

#include "NEMain.h"
#include "NEAlloc.h"

int NE_AllocInit(NEChunk **first_chunk, void *start, void *end)
{
    if (first_chunk == NULL)
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    if (end <= start)
    {
        NE_DebugPrint("Invalid range");
        return -2;
    }

    *first_chunk = malloc(sizeof(NEChunk));
    if (*first_chunk == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return -3;
    }

    (*first_chunk)->previous = NULL;
    (*first_chunk)->state = NE_STATE_FREE;
    (*first_chunk)->start = start;
    (*first_chunk)->end = end;
    (*first_chunk)->next = NULL;

    return 0;
}

int NE_AllocEnd(NEChunk **first_chunk)
{
    if (first_chunk == NULL)
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    NEChunk *this = *first_chunk;

    while (this != NULL)
    {
        NEChunk *next = this->next;
        free(this);
        this = next;
    }

    *first_chunk = NULL;
    return 0;
}

// The function resizes this chunk to the provided size, and creates a new chunk
// with the remaining size.
//
// +-----------------+------+
// |      THIS       | NEXT |  Before
// +-----------------+------+
//
// +------+----------+------+
// | THIS |   NEW    | NEXT |  After
// +------+----------+------+
//
// It returns a pointer to the new chunk.
static NEChunk *ne_split_chunk(NEChunk *this, size_t this_size)
{
    NE_AssertPointer(this, "NULL pointer");

    // Get next chunk and create a new one.

    NEChunk *next = this->next;

    NEChunk *new = malloc(sizeof(NEChunk));
    if (new == NULL)
    {
        NE_DebugPrint("Not enough memory");
        return NULL;
    }

    // Update pointers in the linked list
    // ----------------------------------

    new->previous = this;
    this->next = new;

    if (next == NULL)
    {
        new->next = NULL;
    }
    else
    {
        new->next = next;
        next->previous = new;

        // It shouldn't be free because deallocating a chunk should merge it
        // with any free chunk next to it.
        NE_Assert(next->state != NE_STATE_FREE, "Possible list corruption");
    }

    // Update pointers to start and end of this chunk and the new chunk
    // ----------------------------------------------------------------

    new->end = this->end;
    this->end = (void *)((uintptr_t)this->start + this_size);
    new->start = this->end;

    return new;
}

// This returns a pointer to the chunk that contains the provided address.
// The start address of the chunk is considered to be part of that chunk, but
// the end address isn't considered part of that chunk.
static NEChunk *ne_search_address(NEChunk *first_chunk, void *address)
{
    NE_AssertPointer(first_chunk, "NULL pointer");

    NEChunk *this = first_chunk;

    uintptr_t addr = (uintptr_t)address;

    for ( ; this != NULL; this = this->next)
    {
        uintptr_t start = (uintptr_t)this->start;
        uintptr_t end = (uintptr_t)this->end;

        // We've gone too far. This pointer was likely before the allocated
        // memory pool.
        if (addr < start)
            return NULL;

        if (addr < end)
            return this;
    }

    return NULL;
}

void *NE_AllocFindInRange(NEChunk *first_chunk, void *start, void *end, size_t size)
{
    if ((first_chunk == NULL) || (start == NULL) || (end == NULL) || (size == 0))
    {
        NE_DebugPrint("Invalid arguments");
        return NULL;
    }

    // Get the chunk that contains the first address. If the start address of
    // the first chunk is after the provided start, get the first chunk.
    NEChunk *this;
    if (start < first_chunk->start)
        this = first_chunk;
    else
        this = ne_search_address(first_chunk, start);

    uintptr_t range_end = (uintptr_t)end;

    for ( ; this != NULL; this = this->next)
    {
        // Is this free?
        if (this->state != NE_STATE_FREE)
            continue;

        // "start" is inside "this", but "this->start" may be before "start". In
        // that case, we need to calculate the size actually inside the range
        // provided by the user.
        uintptr_t real_start;
        if (this->start < start)
            real_start = (uintptr_t)start;
        else
            real_start = (uintptr_t)this->start;

        uintptr_t this_end = (uintptr_t)this->end;
        size_t this_size = this_end - real_start;

        // Check if the requested chunk would fit here
        if (this_size < size)
            continue;

        // Check if the expected end of the allocated chunk is within the limits
        // provided by the user. If so, we've gone over the limit, there is no
        // need to keep searching.
        uintptr_t expected_end = real_start + size;
        if (expected_end > range_end)
            return NULL;

        return (void *)real_start;
    }

    return NULL;
}

// This function searches the list and returns a chunk that contains the
// specified range of memory (address, address + size) if it is free.
static NEChunk *ne_search_free_range_chunk(NEChunk *first_chunk,
                                           void *address, size_t size)
{
    NE_AssertPointer(first_chunk, "NULL pointer");

    // If that range of memory is free, it should be in one single chunk. Look
    // for the chunk that contains the base address, and check if the end
    // address is also part of that chunk.

    NEChunk *chunk = ne_search_address(first_chunk, address);
    if (chunk == NULL)
        return NULL;

    uintptr_t end_address = (uintptr_t)address + size;
    uintptr_t chunk_end = (uintptr_t)chunk->end;

    if (end_address > chunk_end)
        return NULL;

    if (chunk->state != NE_STATE_FREE)
        return NULL;

    return chunk;
}

int NE_AllocAddress(NEChunk *first_chunk, void *address, size_t size)
{
    if ((first_chunk == NULL) || (address == NULL) || (size == 0))
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    // Force sizes multiple of NE_ALLOC_MIN_SIZE
    const size_t mask = NE_ALLOC_MIN_SIZE - 1;
    if ((size & mask) != 0)
        size += NE_ALLOC_MIN_SIZE - (size & mask);

    // Get a free chunk that contains this range of memory. This function
    // returns NULL if there is no chunk that contains this range entirely.
    // It also returns NULL if it isn't free.
    NEChunk *this = ne_search_free_range_chunk(first_chunk, address, size);
    if (this == NULL)
        return -2;

    uintptr_t alloc_start = (uintptr_t)address;
    uintptr_t alloc_end = alloc_start + size;

    uintptr_t this_start = (uintptr_t)this->start;
    uintptr_t this_end = (uintptr_t)this->end;

    // If this point is reached we have a free chunk such that:
    //
    //     this_start <= alloc_start  &&  alloc_end <= this_end
    //
    // - If the start address isn't the same, we need to create a new chunk.
    //
    // - If the end address isn't the same, we need to create another chunk.
    //
    // Finally, return the chunk that stays in the middle.

    bool this_is_modified = false;

    if (this_start < alloc_start)
    {
        // Split this chunk into two, ignore the first one and get the second
        // one (which contains the start address)
        this = ne_split_chunk(this, alloc_start - this_start);
        if (this == NULL)
            return -3;

        this_is_modified = true;

        // Only the start has changed
        this_start = (uintptr_t)this->start;

        NE_Assert(this_end == (uintptr_t)this->end, "Unexpected error");
    }

    this->state = NE_STATE_USED;

    if (alloc_end < this_end)
    {
        // Split this chunk into two as well. The first one is the final desired
        // chunk, the second one is more free space.
        NEChunk *next = ne_split_chunk(this, size);
        if (next == NULL)
        {
            if (this_is_modified)
                NE_Free(first_chunk, this);

            return -4;
        }

        next->state = NE_STATE_FREE;

        // Only the end has changed
        this_end = (uintptr_t)this->end;

        NE_Assert(this_start == (uintptr_t)this->start, "Unexpected error");
    }

    NE_Assert(size == (this_end - this_start), "Unexpected error");
    NE_Assert(this->start == address, "Unexpected error");

    return 0;
}

void *NE_Alloc(NEChunk *first_chunk, size_t size)
{
    if ((first_chunk == NULL) || (size == 0))
    {
        NE_DebugPrint("Invalid arguments");
        return NULL;
    }

    // Force sizes multiple of NE_ALLOC_MIN_SIZE
    const size_t mask = NE_ALLOC_MIN_SIZE - 1;
    if ((size & mask) != 0)
        size += NE_ALLOC_MIN_SIZE - (size & mask);

    NEChunk *this = first_chunk;

    for ( ; this != NULL; this = this->next)
    {
        // Skip non-free chunks
        if (this->state != NE_STATE_FREE)
            continue;

        size_t this_size = (size_t)this->end - (size_t)this->start;

        // If this chunk doesn't have enough space, simply skip it.
        if (this_size < size)
            continue;

        // If we have exactly the space requested, we're done.
        if (this_size == size)
        {
            this->state = NE_STATE_USED;
            return this->start;
        }

        // If we have more space than requested, split this chunk:
        //
        // |      THIS       | NEXT |
        // +-----------------+------+  Before
        // |    NOT USED     | USED |
        //
        // | THIS |   NEW    | NEXT |
        // +------+----------+------+  After
        // | USED | NOT USED | USED |

        NEChunk *new = ne_split_chunk(this, size);
        if (new == NULL)
            return NULL;

        // Flag this chunk as used and the new one as free
        this->state = NE_STATE_USED;
        new->state = NE_STATE_FREE;

        return this->start;
    }

    // No more chunks... Not enough free space.
    return NULL;
}

void *NE_AllocFromEnd(NEChunk *first_chunk, size_t size)
{
    if ((first_chunk == NULL) || (size == 0))
    {
        NE_DebugPrint("Invalid arguments");
        return NULL;
    }

    // Force sizes multiple of NE_ALLOC_MIN_SIZE
    const size_t mask = NE_ALLOC_MIN_SIZE - 1;
    if ((size & mask) != 0)
        size += NE_ALLOC_MIN_SIZE - (size & mask);

    // Find last chunk
    NEChunk *this = first_chunk;
    while (this->next != NULL)
        this = this->next;

    // Traverse list from end to beginning
    for ( ; this != NULL; this = this->previous)
    {
        // Skip non-free chunks
        if (this->state != NE_STATE_FREE)
            continue;

        size_t this_size = (size_t)this->end - (size_t)this->start;

        // If this chunk doesn't have enough space, simply skip it.
        if (this_size < size)
            continue;

        // If we have exactly the space requested, we're done.
        if (this_size == size)
        {
            this->state = NE_STATE_USED;
            return this->start;
        }

        // If we have more space than requested, split this chunk:
        //
        // |        THIS      | NEXT |
        // +------------------+------+  Before
        // |      NOT USED    | USED |
        //
        // |    THIS   | NEW  | NEXT |
        // +-----------+------+------+  After
        // |  NOT USED | USED | USED |

        // The size of this chunk has to be the current one minus the requested
        // size for the new chunk.
        NEChunk *new = ne_split_chunk(this, this_size - size);
        if (new == NULL)
            return NULL;

        // Flag the new chunk as used
        new->state = NE_STATE_USED;

        return new->start;
    }

    // No more chunks... Not enough free space.
    return NULL;
}

int NE_Free(NEChunk *first_chunk, void *pointer)
{
    if (first_chunk == NULL)
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    NEChunk *this = first_chunk;

    // Look for the chunk that corresponds to the given pointer
    while (1)
    {
        // Check if we have reached the end without finding the chunk
        if (this == NULL)
            return -2;

        // If this is the chunk we're looking for, exit loop
        if (this->start == pointer)
            break;

        this = this->next;
    }

    // If the specified chunk is free or locked, it can't be freed.
    if (this->state != NE_STATE_USED)
        return -3;

    // Chunk found. Free it.
    this->state = NE_STATE_FREE;

    // Now, check if we can join this free chunk with the previous or the
    // next one
    NEChunk *previous = this->previous;
    NEChunk *next = this->next;

    // Check the previous one
    if (previous && previous->state == NE_STATE_FREE)
    {
        // We can join them
        //
        // | PREVIOUS |   THIS   | NEXT |
        // +----------+----------+------+
        // | NOT USED | NOT USED | ???? |
        //
        // |       PREVIOUS      | NEXT |
        // +---------------------+------+
        // |       NOT USED      | ???? |

        if (next)
        {
            // First, join the previous and the next
            next->previous = previous;
            previous->next = next;
        }
        else
        {
            previous->next = NULL;
        }

        // Expand the previous one
        previous->end = this->end;

        // Delete current chunk
        free(this);

        // Change the active chunk to try to join it with the next one.
        this = previous;
    }

    // Check the next one
    if (next && next->state == NE_STATE_FREE)
    {
        // We can join them
        //
        // |   THIS   |   NEXT   | NEXT NEXT |
        // +----------+----------+-----------+  Before
        // | NOT USED | NOT USED |   USED    |
        //
        // |         THIS        | NEXT NEXT |
        // +---------------------+-----------+  After
        // |       NOT USED      |   USED    |

        NEChunk *next_next = next->next;

        if (next_next)
        {
            // Next Next should be used or locked. If not,
            // something bad is happening here.
            NE_Assert(next_next->state != NE_STATE_FREE,
                      "Possible list corruption. (2)");

            // First, join this chunk and the next next chunk
            next_next->previous = this;
            this->next = next_next;
        }
        else
        {
            this->next = NULL;
        }

        // Expand this node one
        this->end = next->end;

        // Delete next chunk
        free(next);
    }

    return 0;
}

int NE_Lock(NEChunk *first_chunk, void *pointer)
{
    if (first_chunk == NULL)
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    NEChunk *this = first_chunk;

    while (1)
    {
        if (this->start == pointer)
        {
            // Check if we are trying to lock a chunk that isn't in use
            if (this->state != NE_STATE_USED)
                return -2;

            this->state = NE_STATE_LOCKED;
            return 0;
        }

        // Couldn't find a chunk at the specified address
        if (this->next == NULL)
            return -3;

        this = this->next;
    }
}

int NE_Unlock(NEChunk *first_chunk, void *pointer)
{
    if (first_chunk == NULL)
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    NEChunk *this = first_chunk;

    while (1)
    {
        if (this->start == pointer)
        {
            // Check if we are trying to unlock a chunk that isn't locked
            if (this->state != NE_STATE_LOCKED)
                return -2;

            this->state = NE_STATE_USED;
            return 0;
        }

        // Couldn't find a chunk at the specified address
        if (this->next == NULL)
            return -3;

        this = this->next;
    }
}

int NE_MemGetInformation(NEChunk *first_chunk, NEMemInfo *info)
{
    if ((first_chunk == NULL) || (info == NULL))
    {
        NE_DebugPrint("Invalid arguments");
        return -1;
    }

    info->free = 0;
    info->used = 0;
    info->total = 0;
    info->locked = 0;

    NEChunk *this = first_chunk;

    for ( ; this != NULL; this = this->next)
    {
        size_t size = (uintptr_t)this->end - (uintptr_t)this->start;

        switch (this->state)
        {
            case NE_STATE_FREE:
                info->free += size;
                info->total += size;
                break;
            case NE_STATE_USED:
                info->used += size;
                info->total += size;
                break;
            case NE_STATE_LOCKED:
                // Locked memory isn't added to the total
                info->locked += size;
                break;
            default:
                return -2;
                break;
        }
    }

    info->free_percent = (info->free * 100) / info->total;
    return 0;
}
