// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, 2022 Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <stdlib.h>

#include "NEMain.h"
#include "NEAlloc.h"

int NE_AllocInit(NEChunk **first_chunk, void *start, void *end)
{
    if (first_chunk == NULL)
        return -1;

    if (end <= start)
        return -2;

    *first_chunk = malloc(sizeof(NEChunk));
    NE_AssertPointer(*first_chunk, "Couldn't allocate chunk");

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
        return -1;

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

void *NE_Alloc(NEChunk *first_chunk, size_t size)
{
    if ((first_chunk == NULL) || (size == 0))
        return NULL;

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

        uintptr_t this_start = (uintptr_t)this->start;
        uintptr_t this_end = (uintptr_t)this->end;

        size_t this_size = this_end - this_start;

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

        // Get next chunk and create a new one.

        NEChunk *next = this->next;

        NEChunk *new = malloc(sizeof(NEChunk));
        NE_AssertPointer(new, "Couldn't allocate chunk metadata.");

        // Flag the new chunk as free and this one as used

        new->state = NE_STATE_FREE;
        this->state = NE_STATE_USED;

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
            NE_Assert(next->state != NE_STATE_FREE,
                      "Possible list corruption");
        }

        // Update pointers to start and end of this chunk and the new chunk
        // ----------------------------------------------------------------

        new->end = this->end;
        this->end = (void *)(this_start + size);
        new->start = this->end;

        return this->start;
    }

    // No more chunks... Not enough free space.
    return NULL;
}

void *NE_AllocFromEnd(NEChunk *first_chunk, size_t size)
{
    if ((first_chunk == NULL) || (size == 0))
        return NULL;

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

        uintptr_t this_start = (uintptr_t)this->start;
        uintptr_t this_end = (uintptr_t)this->end;

        size_t this_size = this_end - this_start;

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

        // Get next chunk and create a new one.

        NEChunk *new = malloc(sizeof(NEChunk));
        NE_AssertPointer(new, "Couldn't allocate chunk metadata.");

        NEChunk *next = this->next;

        // Flag the new chunk as used

        new->state = NE_STATE_USED;

        // Update pointers in the linked list
        // ----------------------------------

        this->next = new;
        new->previous = this;

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
            NE_Assert(next->state != NE_STATE_FREE,
                      "Possible list corruption");
        }

        // Update pointers to start and end of this chunk and the new chunk
        // ----------------------------------------------------------------

        new->end = this->end;
        new->start = (void *)(this_end - size);
        this->end = new->start;

        return new->start;
    }

    // No more chunks... Not enough free space.
    return NULL;
}

int NE_Free(NEChunk *first_chunk, void *pointer)
{
    if (first_chunk == NULL)
        return -1;

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
        return -1;

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
        return -1;

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
        return -1;

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
