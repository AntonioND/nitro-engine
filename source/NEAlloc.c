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
	NE_Assert(end > start, "NE_AllocInit: End must be after the start.");

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

void *NE_Alloc(NEChunk *first_chunk, int size, int align)
{
	NE_AssertPointer(first_chunk, "NULL pointer");
	NE_Assert(size > 0, "NE_Alloc: Size must be positive.");

	NEChunk *chunk_search = first_chunk;

	// Minimum alignment...
	if (align < 4)
		align = 4;

	while (1) {
		if (chunk_search->status == NE_STATE_FREE) {
			// Let's check if we can allocate here.
			NEChunk *free_chunk = chunk_search;

			int chunk_size = ((int)free_chunk->end - (int)free_chunk->start);

			// If we only have the space requested, it's easy.
			if (chunk_size == size) {
				// If it is aligned...
				if (((int)free_chunk->start & (align - 1)) == 0) {
					// Ready
					free_chunk->status = NE_STATE_USED;
					return free_chunk->start;
				}
				// If not, there isn't enough space. Continue...
			} else if (chunk_size > size) { //If we have more space...
				// If it is aligned...
				if (((int)free_chunk->start & (align - 1)) == 0) {
					// Get next chunk and create a new one.
					NEChunk *next_chunk = free_chunk->next;
					NEChunk *new_chunk = malloc(sizeof(NEChunk));
					NE_AssertPointer(new_chunk,
							 "Couldn't allocate chunk. (1)");

					// Set as used
					free_chunk->status = NE_STATE_USED;

					// Set as free
					new_chunk->status = NE_STATE_FREE;

					// Now, free will point to new, and new will point to next.
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
							  "NE_Alloc: Possible list corruption. (1)");
					} else {
						new_chunk->next = NULL;
					}

					new_chunk->previous = free_chunk;
					free_chunk->next = new_chunk;

					//Now, set pointers...
					new_chunk->end = free_chunk->end;

					free_chunk->end = (void *)((int)free_chunk->start + size);

					new_chunk->start = free_chunk->end;

					//Ready!!
					return free_chunk->start;
				} else {
					// If we need to align it, it is a bit
					// more complicated... Check if even
					// with disalignment we can create room
					// for this
					int end_ptr = (((int)free_chunk->start & (~(align - 1))) + align) + size;

					if (end_ptr < (int)free_chunk->end) { //OK
						// Get chunks next to this, and create 2 new ones.
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
							// If this is not last
							// chunk...
							new2_chunk->next = next_chunk;
							next_chunk->previous = new2_chunk;

							// Shouldn't be free
							NE_Assert(next_chunk->status != NE_STATE_FREE,
								  "NE_Alloc: Possible list corruption. (2)");
						} else {
							new2_chunk->next = NULL;
						}

						// Now, set pointers acording to the size...
						new2_chunk->end = free_chunk->end;

						free_chunk->end = (void *)(((int)free_chunk->start & ~(align - 1)) + align);
						new_chunk->start = free_chunk->end;
						new_chunk->end = (void *)((int)new_chunk->start + size);
						new2_chunk->start = new_chunk->end;

						// Ready!!
						return new_chunk->start;
					} else if (end_ptr == (int)free_chunk->end) { //OK. Even easier...
						// Get chunks next to this, and create 2 new ones.
						NEChunk *next_chunk = free_chunk->next;
						NEChunk *new_chunk = malloc(sizeof(NEChunk));
						NE_AssertPointer(new_chunk, "NE_Alloc: Couldn't allocate chunk. (4)");

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

						if (next_chunk != NULL) { //If this is not last chunk...
							new_chunk->next = next_chunk;
							next_chunk->previous = new_chunk;

							//Shouldn't be free
							NE_Assert(next_chunk->status != NE_STATE_FREE,
								  "NE_Alloc: Possible list corruption. (3)");
						} else {
							new_chunk->next = NULL;
						}

						// Now, set pointers acording to the size...
						new_chunk->end = free_chunk->end;

						free_chunk->end = (void *)(((int)free_chunk->start & ~(align - 1)) + align);
						new_chunk->start = free_chunk->end;

						// Ready!!
						return new_chunk->start;
					}
				}
			}
		}

		if (chunk_search->next == NULL) {
			// No more chunks... Not enough free space.
			return NULL;
		}

		chunk_search = chunk_search->next;
	}
}

void NE_Free(NEChunk *first_chunk, void *pointer)
{
	NE_AssertPointer(first_chunk, "NULL pointer");

	NEChunk *chunk_search = first_chunk;

	while (1) {
		if (chunk_search->start == pointer) {
			// If it isn't used, it is free or locked, so don't touch it.
			if (chunk_search->status != NE_STATE_USED)
				return;

			// Found!
			chunk_search->status = NE_STATE_FREE;

			// Check if we can put together this free chunk with the previous or the next one
			NEChunk *previous_chunk = chunk_search->previous;
			NEChunk *next_chunk = chunk_search->next;

			// Check the previous one
			if (previous_chunk) {
				if (previous_chunk->status == NE_STATE_FREE) {
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
					if (next_chunk) {
						if (next_chunk->status == NE_STATE_FREE) {
							// We can join them
							//
							// |       PREVIOUS      |   NEXT   | NEXT_NEXT |
							// +---------------------+----------+-----------+
							// |       NOT USED      | NOT USED |   USED    |
							//
							// |             PREVIOUS           | NEXT_NEXT |
							// +--------------------------------+-----------+
							// |             NOT USED           |   USED    |

							NEChunk *next_next_chunk = next_chunk->next;

							if (next_next_chunk) {
								// Next Next shouldn't be free. If not, something bad happens here.
								NE_Assert(next_next_chunk->status != NE_STATE_FREE,
									  "NE_Free: Possible list corruption. (1)");

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
					}

					// Done!
					return;
				}
			}

			// Check the next one
			if (next_chunk) {
				if (next_chunk->status == NE_STATE_FREE) {
					// We can join them
					//
					// |   FREE   |   NEXT   | NEXT NEXT |
					// +----------+----------+-----------+
					// | NOT USED | NOT USED |   USED    |
					//
					// |         FREE        | NEXT NEXT |
					// +---------------------+-----------+
					// |       NOT USED      |   USED    |

					NEChunk *next_next_chunk = next_chunk->next;

					if (next_next_chunk) {
						// Next Next should be used or locked. If not, something bad happens here.
						NE_Assert(next_next_chunk->status != NE_STATE_FREE,
							  "NE_Free: Possible list corruption. (2)");

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
			}

			return;
		}

		if (chunk_search->next == NULL)
			return;

		chunk_search = chunk_search->next;
	}
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
		int size = chunk_search->end - chunk_search->start;

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
			NE_DebugPrint("NE_MemGetInformation: Unknown chunk state.");
			break;
		}

		if (chunk_search->next == NULL)
			break;
		chunk_search = chunk_search->next;
	}

	info->FreePercent = (info->Free * 100) / info->Total;
}
