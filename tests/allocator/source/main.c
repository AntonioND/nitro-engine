// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Antonio Niño Díaz, 2008-2022
//
// This file is part of Nitro Engine

// Nitro Engine comes with a general-purpose memory allocator. This file
// contains several tests for it.

#include <stdio.h>

#include <nds.h>

#include <NEAlloc.h>

#define POOL_START_ADDR     0x1000000
#define POOL_END_ADDR       0x2000000
#define POOL_SIZE           (POOL_END_ADDR - POOL_START_ADDR)

#define POOL_START          (void *)POOL_START_ADDR
#define POOL_END            (void *)POOL_END_ADDR

#define POOL_INITIALIZE()                           \
    NEChunk *alloc;                                 \
    NE_AllocInit(&alloc, POOL_START, POOL_END);

#define POOL_DEINITIALIZE()                         \
    NE_AllocEnd(&alloc);

#define ASSERT(cond)                                \
    if (!(cond)) {                                  \
        printf("Line %d\n", __LINE__);              \
        while (1);                                  \
    }

#define ASSERT_MSG(cond, msg)                       \
    if (!(cond)) {                                  \
        printf("Line %d: %s\n", __LINE__, msg);     \
        while (1);                                  \
    }

// Pointer to address
#define A(p) ((uintptr_t)p)
// Address to pointer
#define P(a) ((void *)a)

// Test that the pointer advances as expected when allocating memory, and that
// chunks that are too small are aligned to 16 bytes.
void test_alloc_align(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_START_ADDR;
    void *ptr;

    ptr = NE_Alloc(alloc, 64);
    ASSERT(A(ptr) == addr);
    addr += 64;

    ptr = NE_Alloc(alloc, 32);
    ASSERT(A(ptr) == addr);
    addr += 32;

    ptr = NE_Alloc(alloc, 16);
    ASSERT(A(ptr) == addr);
    addr += 16;

    ASSERT_MSG(16 == NE_ALLOC_MIN_SIZE, "Unexpected NE_ALLOC_MIN_SIZE");

    ptr = NE_Alloc(alloc, 8);
    ASSERT(A(ptr) == addr);
    addr += NE_ALLOC_MIN_SIZE;

    ptr = NE_Alloc(alloc, 4);
    ASSERT(A(ptr) == addr);
    addr += NE_ALLOC_MIN_SIZE;

    POOL_DEINITIALIZE();
}

// Test that the pointer advances as expected when allocating memory from the
// end of the pool, and that chunks that are too small are aligned to 16 bytes.
void test_alloc_from_end_align(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_END_ADDR;
    void *ptr;

    addr -= 64;
    ptr = NE_AllocFromEnd(alloc, 64);
    ASSERT(A(ptr) == addr);

    addr -= 32;
    ptr = NE_AllocFromEnd(alloc, 32);
    ASSERT(A(ptr) == addr);

    addr -= 16;
    ptr = NE_AllocFromEnd(alloc, 16);
    ASSERT(A(ptr) == addr);

    ASSERT_MSG(16 == NE_ALLOC_MIN_SIZE, "Unexpected NE_ALLOC_MIN_SIZE");

    addr -= NE_ALLOC_MIN_SIZE;
    ptr = NE_AllocFromEnd(alloc, 8);
    ASSERT(A(ptr) == addr);

    addr -= NE_ALLOC_MIN_SIZE;
    ptr = NE_AllocFromEnd(alloc, 4);
    ASSERT(A(ptr) == addr);

    POOL_DEINITIALIZE();
}

// Test that a freed chunk can be reused
void test_free(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_START_ADDR;

    void *ptr1 = NE_Alloc(alloc, 256);
    ASSERT(A(ptr1) == addr);
    addr += 256;

    void *ptr2 = NE_Alloc(alloc, 256);
    ASSERT(A(ptr2) == addr);

    // Free the first chunk
    int ret = NE_Free(alloc, ptr1);
    ASSERT(ret == 0);

    void *ptr3 = NE_Alloc(alloc, 256);
    ASSERT(A(ptr3) == A(ptr1));

    POOL_DEINITIALIZE();
}

// Test that a freed chunk can be reused
void test_free_from_end(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_END_ADDR;

    addr -= 256;
    void *ptr1 = NE_AllocFromEnd(alloc, 256);
    ASSERT(A(ptr1) == addr);

    addr -= 256;
    void *ptr2 = NE_AllocFromEnd(alloc, 256);
    ASSERT(A(ptr2) == addr);

    // Free the first chunk
    int ret = NE_Free(alloc, ptr1);
    ASSERT(ret == 0);

    void *ptr3 = NE_AllocFromEnd(alloc, 256);
    ASSERT(A(ptr3) == A(ptr1));

    POOL_DEINITIALIZE();
}

// Several tests to lock and unlock chunks
void test_lock_unlock(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_START_ADDR;
    int ret;

    // Try locking a chunk that is used

    void *ptr = NE_Alloc(alloc, 256);
    ASSERT(A(ptr) == addr);

    ret = NE_Lock(alloc, ptr);
    ASSERT(ret == 0);

    // Try to free a locked chunk

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == -3);

    // Unlock a chunk that is locked

    ret = NE_Unlock(alloc, ptr);
    ASSERT(ret == 0);

    // Try unlocking a chunk that is used

    ret = NE_Unlock(alloc, ptr);
    ASSERT(ret == -2);

    // Try unlocking a chunk that has just been freed

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    ret = NE_Unlock(alloc, ptr);
    ASSERT(ret == -2);

    // Try locking a chunk that has just been freed

    ret = NE_Lock(alloc, ptr);
    ASSERT(ret == -2);

    // Now, allocate a new chunk that uses half of the memory and lock it. Then,
    // try to allocate more than half of memory and check it fails.

    ptr = NE_Alloc(alloc, POOL_SIZE / 2);
    ASSERT(A(ptr) == POOL_START_ADDR);

    ret = NE_Lock(alloc, ptr);
    ASSERT(ret == 0);

    void *fail = NE_Alloc(alloc, (POOL_SIZE / 2) + 1024);
    ASSERT(fail == NULL);

    ret = NE_Unlock(alloc, ptr);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    POOL_DEINITIALIZE();
}

// Tests to verify that allocation fails under some conditions
void test_alloc_fail(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_START_ADDR;
    int ret;
    void *ptr;

    // Try to allocate zero bytes

    ptr = NE_Alloc(alloc, 0);
    ASSERT(ptr == NULL);

    ptr = NE_AllocFromEnd(alloc, 0);
    ASSERT(ptr == NULL);

    // Try to allocate the maximum size

    ptr = NE_Alloc(alloc, POOL_SIZE);
    ASSERT(A(ptr) == addr);

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    ptr = NE_AllocFromEnd(alloc, POOL_SIZE);
    ASSERT(A(ptr) == addr);

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    // Try to allocate more than the limit

    void *fail = NE_Alloc(alloc, POOL_SIZE + 1);
    ASSERT(fail == NULL);

    fail = NE_AllocFromEnd(alloc, POOL_SIZE + 1);
    ASSERT(fail == NULL);

    // Fragment the memory pool and try to allocate the remaining space, which
    // should fail.

    ptr = NE_Alloc(alloc, POOL_SIZE / 4);
    ASSERT(A(ptr) == addr);

    void *ptr2 = NE_Alloc(alloc, POOL_SIZE / 2);
    ASSERT(A(ptr2) == (addr + (POOL_SIZE / 4)));

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    NEMemInfo info; // Get information about the remaining free memory
    ret = NE_MemGetInformation(alloc, &info);
    ASSERT(ret == 0);

    ASSERT(info.free == (POOL_SIZE / 2));

    fail = NE_Alloc(alloc, info.free);
    ASSERT(fail == NULL);

    fail = NE_AllocFromEnd(alloc, info.free);
    ASSERT(fail == NULL);

    POOL_DEINITIALIZE();
}

// Test statistics calculation
void test_statistics(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    uintptr_t addr = POOL_START_ADDR;
    int ret;

    const size_t size = POOL_SIZE / 8;

    // Allocate a few chunks

    void *ptr1 = NE_Alloc(alloc, size);
    ASSERT(A(ptr1) == addr);
    addr += size;

    void *ptr2 = NE_Alloc(alloc, size);
    ASSERT(A(ptr2) == addr);
    addr += size;

    void *ptr3 = NE_Alloc(alloc, size);
    ASSERT(A(ptr3) == addr);
    addr += size;

    void *ptr4 = NE_AllocFromEnd(alloc, size);
    ASSERT(A(ptr4) == POOL_END_ADDR - size);

    // Free one of them

    ret = NE_Free(alloc, ptr2);
    ASSERT(ret == 0);

    // Lock one of them

    ret = NE_Lock(alloc, ptr4);
    ASSERT(ret == 0);

    // Get statistics

    NEMemInfo info;
    ret = NE_MemGetInformation(alloc, &info);
    ASSERT(ret == 0);

    ASSERT(info.free == (5 * POOL_SIZE / 8));
    ASSERT(info.used == (2 * POOL_SIZE / 8));
    // Note: Locked memory isn't added to the total
    ASSERT(info.locked == (1 * POOL_SIZE / 8));
    ASSERT(info.total == (7 * POOL_SIZE / 8));

    ASSERT(info.free_percent == (100 * 5 / 7));

    POOL_DEINITIALIZE();
}

// Count the number of chunks present in the linked list.
int count_num_chunks(NEChunk *list)
{
    int count = 0;

    for ( ;list != NULL; list = list->next)
        count++;

    return count;
}

// Verify that the linked list of chunks has consistent start and end addresses.
int verify_consistency(NEChunk *list, void *start, void *end)
{
    if (list == NULL)
        return 0;

    // The first chunk should start at the start of the memory pool
    if (list->start != start)
        return -1;

    for ( ; list->next != NULL; list = list->next)
    {
        // The end of a chunk should be the start of the next one
        if (list->end != list->next->start)
            return -2;

        // Two free chunks should never be together
        if ((list->state == NE_STATE_FREE) && (list->next->state == NE_STATE_FREE))
            return -3;
    }

    // The last chunk should end at the end of the memory pool
    if (list->end != end)
        return -4;

    return 0;
}

void print_list(NEChunk *list)
{
    if (list == NULL)
    {
        printf("NULL\n");
        return;
    }

    for ( ; list != NULL; list = list->next)
    {
        printf("%p-%p (%zu)\n", list->start, list->end,
               (size_t)(A(list->end) - A(list->start)));
    }
}

// Test that the internal linked list is in the expected state
void test_internal_list_state(void)
{
    NEChunk *alloc;
    int ret, count;

    // Test with invalid linked list

    ret = NE_AllocInit(NULL, POOL_START, POOL_END);
    ASSERT(ret == -1);

    // Test with switched start and end

    ret = NE_AllocInit(&alloc, POOL_END, POOL_START);
    ASSERT(ret == -2);

    // Initialize a valid list

    ret = NE_AllocInit(&alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    // Allocate a few chunks

    void *ptr[10];
    for (int i = 0; i < 10; i++)
    {
        ptr[i] = NE_Alloc(alloc, 1024);
        ASSERT(A(ptr[i]) == POOL_START_ADDR + (1024 * i));
    }

    count = count_num_chunks(alloc);
    ASSERT(count == 11); // 10 chunks + chunk with all remaining memory

    // Free the first chunk and then the second one so that it is merged with
    // the first one.

    ret = NE_Free(alloc, ptr[0]);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 11);

    ret = NE_Free(alloc, ptr[1]);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 10);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Free the last chunk so that it is merged with the remaining free memory

    ret = NE_Free(alloc, ptr[9]);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 9);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, ptr[8]); // Do it again to test it
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 8);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Free two chunks with one chunk the middle. Then, free that chunk and see
    // if the three chunks are merged into one.

    ret = NE_Free(alloc, ptr[3]);
    ASSERT(ret == 0);
    ret = NE_Free(alloc, ptr[5]);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 8);

    ret = NE_Free(alloc, ptr[4]);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 6);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Deallocate invalid list

    ret = NE_AllocEnd(NULL);
    ASSERT(ret == -1);

    // Deallocate list correctly

    ret = NE_AllocEnd(&alloc);
    ASSERT(ret == 0);
}

// Tests to verify that a list can be filled with blocks
void test_alloc_fill(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    int ret;
    int count;
    size_t size = POOL_SIZE / 32;
    NEMemInfo info;

    // Allocate forwards

    for (int i = 0; i < 32; i++)
    {
        uintptr_t addr = POOL_START_ADDR + size * i;
        void *ptr = NE_Alloc(alloc, size);
        ASSERT(ptr == (void *)addr);
    }

    ret = NE_MemGetInformation(alloc, &info);
    ASSERT(ret == 0);
    ASSERT(info.free == 0);
    ASSERT(info.used == POOL_SIZE);

    count = count_num_chunks(alloc);
    ASSERT(count == 32);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    for (int i = 0; i < 32; i++)
    {
        uintptr_t addr = POOL_START_ADDR + size * i;
        ret = NE_Free(alloc, (void *)addr);
        ASSERT(ret == 0);
    }

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Allocate backwards

    for (int i = 0; i < 32; i++)
    {
        uintptr_t addr = POOL_START_ADDR + size * (31 - i);
        void *ptr = NE_AllocFromEnd(alloc, size);
        ASSERT(ptr == (void *)addr);
    }

    ret = NE_MemGetInformation(alloc, &info);
    ASSERT(ret == 0);
    ASSERT(info.free == 0);
    ASSERT(info.used == POOL_SIZE);

    count = count_num_chunks(alloc);
    ASSERT(count == 32);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    for (int i = 0; i < 32; i++)
    {
        uintptr_t addr = POOL_START_ADDR + size * i;
        ret = NE_Free(alloc, (void *)addr);
        ASSERT(ret == 0);
    }

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    POOL_DEINITIALIZE();
}

// Try to allocate chunks at specified addresses
void test_alloc_range(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    int ret;
    int count;

    // Try to allocate with invalid list

    ret = NE_AllocAddress(NULL, POOL_START, 1024);
    ASSERT(ret == -1);

    // Try to allocate invalid addresses

    ret = NE_AllocAddress(alloc, NULL, 1024);
    ASSERT(ret == -1);

    ret = NE_AllocAddress(alloc, (void *)(POOL_START_ADDR - 1), 1024);
    ASSERT(ret == -2);

    ret = NE_AllocAddress(alloc, POOL_END, 1024);
    ASSERT(ret == -2);

    // Try to allocate zero bytes

    ret = NE_AllocAddress(alloc, POOL_START, 0);
    ASSERT(ret == -1);

    // Try to allocate too many bytes

    ret = NE_AllocAddress(alloc, POOL_START, POOL_SIZE + 1);
    ASSERT(ret == -2);

    // Try to allocate the maximum size, ensure that the list is one chunk long
    // (it isn't needed to split it).

    ret = NE_AllocAddress(alloc, POOL_START, POOL_SIZE);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, POOL_START);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Try to allocate a chunk that leaves free space after the chunk

    ret = NE_AllocAddress(alloc, POOL_START, POOL_SIZE / 2);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 2);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, POOL_START);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Try to allocate a chunk that leaves free space before the chunk

    void *half = (void *)(POOL_START_ADDR + (POOL_SIZE / 2));
    ret = NE_AllocAddress(alloc, half, POOL_SIZE / 2);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 2);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, half);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Try to allocate a chunk that leaves free space before and after the chunk

    void *quarter = (void *)(POOL_START_ADDR + (POOL_SIZE / 4));

    ret = NE_AllocAddress(alloc, quarter, POOL_SIZE / 2);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 3);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, quarter);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    // Try to allocate in a chunk that is used

    void *ptr = NE_Alloc(alloc, POOL_SIZE / 2);
    ASSERT(ptr == POOL_START);

    ret = NE_AllocAddress(alloc, POOL_START, POOL_SIZE / 4);
    ASSERT(ret == -2);

    ret = NE_Free(alloc, ptr);
    ASSERT(ret == 0);

    // Try to allocate in a chunk that is free, but small

    ptr = NE_AllocFromEnd(alloc, POOL_SIZE / 2);
    ASSERT(ptr == half);

    ret = NE_AllocAddress(alloc, POOL_START, 3 * POOL_SIZE / 4);
    ASSERT(ret == -2);

    ret = NE_Free(alloc, half);
    ASSERT(ret == 0);

    // Try to allocate in a free chunk surrounded by used chunks, to verify that
    // all links between chunks are correct.

    void *three_quarters = (void *)(POOL_START_ADDR + (3 * POOL_SIZE / 4));

    ptr = NE_Alloc(alloc, POOL_SIZE / 4);
    ASSERT(ptr == POOL_START);

    ptr = NE_AllocFromEnd(alloc, POOL_SIZE / 4);
    ASSERT(ptr == three_quarters);

    ret = NE_AllocAddress(alloc, half, POOL_SIZE / 8);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 5);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, half);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, POOL_START);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, three_quarters);
    ASSERT(ret == 0);

    count = count_num_chunks(alloc);
    ASSERT(count == 1);

    ret = verify_consistency(alloc, POOL_START, POOL_END);
    ASSERT(ret == 0);

    POOL_DEINITIALIZE();
}

// Tests for NE_AllocFindInRange()
void test_find_range(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

    int ret;
    void *found;

    void *half = (void *)(POOL_START_ADDR + POOL_SIZE / 2);

    // Invalid arguments

    found = NE_AllocFindInRange(NULL, POOL_START, POOL_END, 1024);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, NULL, POOL_END, 1024);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, POOL_START, NULL, 1024);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, 0);
    ASSERT(found == NULL);

    // A few tests with a completely empty pool
    // ----------------------------------------

    // Get memory from the start of a chunk

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, 1024);
    ASSERT(found == POOL_START);

    // Get memory from the middle of a chunk

    found = NE_AllocFindInRange(alloc, half, POOL_END, 1024);
    ASSERT(found == half);

    // Ask for too much memory with an empty pool

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, POOL_SIZE + 1);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, half, POOL_END, POOL_SIZE);
    ASSERT(found == NULL);

    // The user requests to search in a range that starts before the memory pool

    void *before_start = (void *)(POOL_START_ADDR - 1024);

    found = NE_AllocFindInRange(alloc, before_start, POOL_END, POOL_SIZE);
    ASSERT(found == POOL_START);

    // The range requested by the user ends before the memory pool

    found = NE_AllocFindInRange(alloc, before_start, POOL_START, 64);
    ASSERT(found == NULL);

    // The range requested by the user starts after the memory pool

    void *after_end = (void *)(POOL_END_ADDR + 1024);

    found = NE_AllocFindInRange(alloc, POOL_END, after_end, 64);
    ASSERT(found == NULL);

    // Now, fill the memory pool with a few chunks of data and run more tests
    // ----------------------------------------------------------------------

    void *one_eight = (void *)(POOL_START_ADDR + (POOL_SIZE / 8));
    void *one_quarter = (void *)(POOL_START_ADDR + (POOL_SIZE / 4));
    void *three_eights = (void *)(POOL_START_ADDR + (3 * POOL_SIZE / 8));
    void *three_quarters = (void *)(POOL_START_ADDR + (3 * POOL_SIZE / 4));
    void *five_eights = (void *)(POOL_START_ADDR + (5 * POOL_SIZE / 8));

    size_t quarter = POOL_SIZE / 4;

    // +-----------------+-----------------+-----------------+-----------------+
    // |                 |      USED       |                 |      USED       |
    // +-----------------+-----------------+-----------------+-----------------+

    ret = NE_AllocAddress(alloc, one_quarter, quarter);
    ASSERT(ret == 0);

    ret = NE_AllocAddress(alloc, three_quarters, quarter);
    ASSERT(ret == 0);

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, quarter);
    ASSERT(found == POOL_START);

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, POOL_SIZE / 2);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, one_eight, POOL_END, quarter);
    ASSERT(found == half);

    found = NE_AllocFindInRange(alloc, three_eights, POOL_END, quarter);
    ASSERT(found == half);

    found = NE_AllocFindInRange(alloc, three_eights, POOL_END, POOL_SIZE / 2);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, five_eights, POOL_END, quarter);
    ASSERT(found == NULL);

    ret = NE_Free(alloc, one_quarter);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, three_quarters);
    ASSERT(ret == 0);

    // +-----------------+-----------------+-----------------+-----------------+
    // |      USED       |                 |      USED       |                 |
    // +-----------------+-----------------+-----------------+-----------------+

    ret = NE_AllocAddress(alloc, POOL_START, quarter);
    ASSERT(ret == 0);

    ret = NE_AllocAddress(alloc, half, quarter);
    ASSERT(ret == 0);

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, quarter);
    ASSERT(found == one_quarter);

    found = NE_AllocFindInRange(alloc, POOL_START, POOL_END, POOL_SIZE / 2);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, one_eight, POOL_END, quarter);
    ASSERT(found == one_quarter);

    found = NE_AllocFindInRange(alloc, three_eights, POOL_END, quarter);
    ASSERT(found == three_quarters);

    found = NE_AllocFindInRange(alloc, three_eights, POOL_END, POOL_SIZE / 2);
    ASSERT(found == NULL);

    found = NE_AllocFindInRange(alloc, five_eights, POOL_END, quarter);
    ASSERT(found == three_quarters);

    ret = NE_Free(alloc, POOL_START);
    ASSERT(ret == 0);

    ret = NE_Free(alloc, half);
    ASSERT(ret == 0);

    POOL_DEINITIALIZE();
}

// Known random number generator to always generate the same sequence of numbers
// and make this test reproducible.
int my_rand(void)
{
    static unsigned long int next = 1;
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

// Stress test
void test_stress(void)
{
    printf("%s\n", __func__);

    POOL_INITIALIZE();

#define NUM_PTRS 32

    int ret;
    void *ptr[NUM_PTRS];

    for (int i = 0; i < NUM_PTRS; i++)
        ptr[i] = NULL;

    for (int i = 0; i < 500000; i++)
    {
        unsigned int selected = my_rand() % NUM_PTRS;

        if (ptr[selected] == NULL)
        {
            // Unallocated pointer. Allocate it. The number of chunks is too
            // small to ever fill the memory with this chunk size, so it should
            // always be allocated.

            size_t size = (my_rand() & 0x3FFF) + 1;

            void *p;

            if (size & 1)
                p = NE_Alloc(alloc, size);
            else
                p = NE_AllocFromEnd(alloc, size);

            ASSERT(p != NULL);

            ptr[selected] = p;
        }
        else
        {
            // Allocated pointer. Deallocate it.
            ret = NE_Free(alloc, ptr[selected]);
            ASSERT(ret == 0);
            ptr[selected] = NULL;
        }

        // Verify consistency of the list
        ret = verify_consistency(alloc, POOL_START, POOL_END);
        if (ret != 0)
        {
            printf("ret = %d", ret);
            print_list(alloc);
            while (1);
        }
    }

    POOL_DEINITIALIZE();
}

int main(int argc, char *argv[])
{
    // This test doesn't use Nitro Engine at all. Initialize the default console
    // of libnds to print the results of the tests.
    consoleDemoInit();

    test_alloc_align();
    test_alloc_from_end_align();
    test_free();
    test_free_from_end();
    test_lock_unlock();
    test_alloc_fail();
    test_statistics();
    test_internal_list_state();
    test_alloc_fill();
    test_alloc_range();
    test_find_range();
    test_stress();

    printf("Done!");

    while (1)
        swiWaitForVBlank();

    return 0;
}
