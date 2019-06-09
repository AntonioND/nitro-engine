// SPDX-License-Identifier: MIT
//
// Copyright (c) 2008-2011, 2019, Antonio Niño Díaz
//
// This file is part of Nitro Engine

#include <stdlib.h>

#include "NEMain.h"
#include "NEAlloc.h"

void NE_AllocInit(NEChunk **first_chunk, void *start, void *end)
{
	NE_AssertPointer(first_chunk, "NULL pointer");
	NE_Assert(end > start, "End must be after the start");

	*first_chunk = malloc(sizeof(NEChunk));
	NE_AssertPointer(*first_chunk, "Couldn't allocate chunk");

	(*first_chunk)->previous = NULL;
	(*first_chunk)->status = NE_STATE_FREE;
	(*first_chunk)->start = start;
	(*first_chunk)->end = end;
	(*first_chunk)->next = NULL;
}

void NE_AllocEnd(NEChunk *first_chunk)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search, *chunk_current = first_chunk;

	while (1) {
		chunk_search = chunk_current->next;
		free(chunk_current);
		if (chunk_search == NULL)
			break;
		chunk_current = chunk_search;
	}

	return;
}

void *NE_Alloc(NEChunk *first_chunk, size_t size, unsigned int align)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	// Minimum alignment...
	if (align < 4)
		align = 4;

	NEChunk *chunk_search = first_chunk;

	for ( ; chunk_search != NULL; chunk_search = chunk_search->next) {

		// Skip non-free chunks
		if (chunk_search->status != NE_STATE_FREE)
			continue;

		// Let's check if we can allocate here.
		NEChunk *free_chunk = chunk_search;

		uintptr_t free_chunk_start = (uintptr_t)free_chunk->start;
		uintptr_t free_chunk_end = (uintptr_t)free_chunk->end;

		size_t chunk_size = free_chunk_end - free_chunk_start;

		// If this chunk doesn't have enough space, simply skip it
		if (chunk_size < size)
			continue;

		// If we only have the space requested, it can be easy
		if (chunk_size == size) {
			// If it is already aligned, we're done
			if ((free_chunk_start & (align - 1)) == 0) {
				free_chunk->status = NE_STATE_USED;
				return free_chunk->start;
			}

			// If not, there isn't enough space in this chunk.
			// Continue with the next one.
			continue;
		}

		// If we have more space than requested

		// If it is aligned...
		if ((free_chunk_start & (align - 1)) == 0) {
			// Get next chunk and create a new one.
			NEChunk *next_chunk = free_chunk->next;
			NEChunk *new_chunk = malloc(sizeof(NEChunk));
			NE_AssertPointer(new_chunk,
					 "Couldn't allocate chunk. (1)");

			// Set as used
			free_chunk->status = NE_STATE_USED;

			// Set as free
			new_chunk->status = NE_STATE_FREE;

			// Now, free will point to new, and new will point to
			// next.

			//
			// |      FREE       | NEXT |
			// +-----------------+------+
			// |    NOT USED     | USED |
			//
			// | FREE |   NEW    | NEXT |
			// +------+----------+------+
			// | USED | NOT USED | USED |

			if (next_chunk != NULL) {
				// If this is not last chunk...
				new_chunk->next = next_chunk;
				next_chunk->previous = new_chunk;

				// Shouldn't be free
				NE_Assert(next_chunk->status != NE_STATE_FREE,
					  "Possible list corruption. (1)");
			} else {
				new_chunk->next = NULL;
			}

			new_chunk->previous = free_chunk;
			free_chunk->next = new_chunk;

			// Now, set pointers...
			new_chunk->end = free_chunk->end;

			free_chunk->end = (void *)(free_chunk_start + size);

			new_chunk->start = free_chunk->end;

			// Ready!!
			return free_chunk->start;
		} else {
			// If we need to align it, it is a bit more
			// complicated... Check if even with disalignment we can
			// create room for this
			uintptr_t end_ptr = (free_chunk_start & (~(align - 1)))
					  + align + size;

			if (end_ptr < free_chunk_end) {
				// Get chunks adjacent to this one, and create 2
				// new chunks
				NEChunk *next_chunk = free_chunk->next;
				NEChunk *new_chunk = malloc(sizeof(NEChunk));
				NEChunk *new2_chunk = malloc(sizeof(NEChunk));
				NE_AssertPointer(new_chunk,
						 "Couldn't allocate chunk. (2)");
				NE_AssertPointer(new2_chunk,
						 "Couldn't allocate chunk. (3)");

				// Set as used
				new_chunk->status = NE_STATE_USED;

				// Set as free
				new2_chunk->status = NE_STATE_FREE;

				// |         FREE         | NEXT |
				// +----------------------+------+
				// |      NOT USED        | USED |
				//
				// |  FREE  |NEW | NEW 2  | NEXT |
				// +~~~~~~~~+----+--------+------+
				// |NOT USED|USED|NOT USED| USED |

				free_chunk->next = new_chunk;
				new_chunk->previous = free_chunk;

				new_chunk->next = new2_chunk;
				new2_chunk->previous = new_chunk;

				if (next_chunk != NULL) {
					// If this is not last chunk...
					new2_chunk->next = next_chunk;
					next_chunk->previous = new2_chunk;

					// Shouldn't be free
					NE_Assert(next_chunk->status != NE_STATE_FREE,
						  "Possible list corruption. (2)");
				} else {
					new2_chunk->next = NULL;
				}

				// Now, set pointers acording to the size...
				new2_chunk->end = free_chunk->end;

				free_chunk->end = (void *)((free_chunk_start & ~(align - 1)) + align);
				new_chunk->start = free_chunk->end;
				new_chunk->end = (void *)((uintptr_t)new_chunk->start + size);
				new2_chunk->start = new_chunk->end;

				// Ready!!
				return new_chunk->start;
			} else if (end_ptr == free_chunk_end) {
				// Easy case

				// Get chunks next to this, and create 2 new ones.
				NEChunk *next_chunk = free_chunk->next;
				NEChunk *new_chunk = malloc(sizeof(NEChunk));
				NE_AssertPointer(new_chunk,
						 "NE_Alloc: Couldn't allocate chunk. (4)");

				// Set as used
				new_chunk->status = NE_STATE_USED;

				// |         FREE         | NEXT |
				// +----------------------+------+
				// |      NOT USED        | USED |
				//
				// |  FREE  |     NEW     | NEXT |
				// +~~~~~~~~+-------------+------+
				// |NOT USED|     USED    | USED |

				free_chunk->next = new_chunk;
				new_chunk->previous = free_chunk;

				if (next_chunk != NULL) {
					// If this is not last chunk...
					new_chunk->next = next_chunk;
					next_chunk->previous = new_chunk;

					// Shouldn't be free
					NE_Assert(next_chunk->status != NE_STATE_FREE,
						  "Possible list corruption. (3)");
				} else {
					new_chunk->next = NULL;
				}

				// Now, set pointers acording to the size...
				new_chunk->end = free_chunk->end;

				free_chunk->end = (void *)((free_chunk_start & ~(align - 1)) + align);
				new_chunk->start = free_chunk->end;

				// Ready!!
				return new_chunk->start;
			}
		}
	}

	// No more chunks... Not enough free space.
	return NULL;
}

void NE_Free(NEChunk *first_chunk, void *pointer)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search = first_chunk;

	// Look for the chunk that corresponds to the given pointer
	while (1) {
		// Check if we have reached the end of the list
		if (chunk_search == NULL) {
			NE_DebugPrint("Chunk not found");
			return;
		}

		// If this is the chunk we're looking for, exit loop
		if (chunk_search->start == pointer)
			break;

		chunk_search = chunk_search->next;
	}

	// If the specified chunk is free or locked, it can't be freed.
	if (chunk_search->status != NE_STATE_USED)
		return;

	// Chunk found. Free it.
	chunk_search->status = NE_STATE_FREE;

	// Now, check if we can join this free chunk with the previous or the
	// next one
	NEChunk *previous_chunk = chunk_search->previous;
	NEChunk *next_chunk = chunk_search->next;

	// Check the previous one
	if (previous_chunk && previous_chunk->status == NE_STATE_FREE) {
		// We can join them
		//
		// | PREVIOUS | FREEING  | NEXT |
		// +----------+----------+------+
		// | NOT USED | NOT USED | ???? |
		//
		// |       PREVIOUS      | NEXT |
		// +---------------------+------+
		// |       NOT USED      | ???? |

		if (next_chunk) {
			// First, join the previous and the next
			next_chunk->previous = previous_chunk;
			previous_chunk->next = next_chunk;
		} else {
			previous_chunk->next = NULL;
		}

		// Expand the previous one
		previous_chunk->end = chunk_search->end;

		// Delete current chunk
		free(chunk_search);

		// Check the next one
		if (next_chunk && next_chunk->status == NE_STATE_FREE) {
			// We can join them

			// |      PREVIOUS      |   NEXT   | NEXT_NEXT |
			// +--------------------+----------+-----------+
			// |      NOT USED      | NOT USED |   USED    |
			//
			// |            PREVIOUS           | NEXT_NEXT |
			// +-------------------------------+-----------+
			// |            NOT USED           |   USED    |

			NEChunk *next_next_chunk = next_chunk->next;

			if (next_next_chunk) {
				// Next Next shouldn't be free. If not,
				// something bad is happening here.
				NE_Assert(next_next_chunk->status != NE_STATE_FREE,
					  "Possible list corruption. (1)");

				// First, join the previous and the next next
				next_next_chunk->previous = previous_chunk;
				previous_chunk->next = next_next_chunk;
			} else {
				previous_chunk->next = NULL;
			}

			// Expand the previous one
			previous_chunk->end = next_chunk->end;

			// Delete next chunk
			free(next_chunk);
		}

		// Done!
		return;
	}

	// Check the next one
	if (next_chunk && next_chunk->status == NE_STATE_FREE) {
		// We can join them

		// |   FREE   |   NEXT   | NEXT NEXT |
		// +----------+----------+-----------+
		// | NOT USED | NOT USED |   USED    |
		//
		// |         FREE        | NEXT NEXT |
		// +---------------------+-----------+
		// |       NOT USED      |   USED    |

		NEChunk *next_next_chunk = next_chunk->next;

		if (next_next_chunk) {
			// Next Next should be used or locked. If not,
			// something bad is happening here.
			NE_Assert(next_next_chunk->status != NE_STATE_FREE,
				  "Possible list corruption. (2)");

			// First, join the free and the next next
			next_next_chunk->previous = chunk_search;
			chunk_search->next = next_next_chunk;
		} else {
			chunk_search->next = NULL;
		}

		// Expand the free one
		chunk_search->end = next_chunk->end;

		// Delete next chunk
		free(next_chunk);

		// Done!
		return;
	}

	// We've done everything we can after freeing the chunk, exit
}

void NE_Lock(NEChunk *first_chunk, void *pointer)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search = first_chunk;

	while (1) {
		if (chunk_search->start == pointer) {
			// Found!
			chunk_search->status = NE_STATE_LOCKED;

			return;
		}

		if (chunk_search->next == NULL)
			return;

		chunk_search = chunk_search->next;
	}
}

void NE_Unlock(NEChunk *first_chunk, void *pointer)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search = first_chunk;

	while (1) {
		if (chunk_search->start == pointer) {
			// Found!
			if (chunk_search->status == NE_STATE_LOCKED) {
				chunk_search->status = NE_STATE_USED;
			}

			return;
		}

		if (chunk_search->next == NULL)
			return;

		chunk_search = chunk_search->next;
	}
}

/*
int NE_GetSize(NEChunk *first_chunk, void *pointer)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search = first_chunk;

	while (1) {
		if(chunk_search->start == pointer) {
			// Found!
			return (int)(chunk_search->end - chunk_search->start);
		}

		if (chunk_search->next == NULL)
			return 0;

		chunk_search = chunk_search->next;
	}
}
*/

void NE_MemGetInformation(NEChunk *first_chunk, NEMemInfo *info)
{
	NE_AssertPointer(first_chunk, "NULL list pointer");
	NE_AssertPointer(info, "NULL info pointer");

	info->Free = info->Used = info->Total = info->Locked = 0;

	NEChunk *chunk_search = first_chunk;

	while (1) {
		size_t size = (uintptr_t)chunk_search->end
			    - (uintptr_t)chunk_search->start;

		switch (chunk_search->status) {
		case NE_STATE_FREE:
			info->Free += size;
			info->Total += size;
			break;
		case NE_STATE_USED:
			info->Used += size;
			info->Total += size;
			break;
		case NE_STATE_LOCKED:
			info->Locked += size;
			break;
		default:
			NE_DebugPrint("Unknown chunk state");
			break;
		}

		if (chunk_search->next == NULL)
			break;

		chunk_search = chunk_search->next;
	}

	info->FreePercent = (info->Free * 100) / info->Total;
}
