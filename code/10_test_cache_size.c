#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    M1 CPU
    gb/s
    140  *--*--*--*--*--*--*--*--*\
    120                            \
    100                             \
     80                              \*----*---*--*--*--*\
     60                                                   \*---*---*----*----*----*---*
     40
     20
        1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M 32M 64M 128M 256M 512M 1Gb
*/

ASM_CALL void test_cache_size_asm(uint64 count, void *data, uint64 mask);

uint64 sizes[] =
{
    1 << 10,
    1 << 11,
    1 << 12,
    1 << 13,
    1 << 14,
    1 << 15,
    1 << 16,
    1 << 17,
    1 << 18,
    1 << 19,
    1 << 20,
    1 << 21,
    1 << 22,
    1 << 23,
    1 << 24,
    1 << 25,
    1 << 26,
    1 << 27,
    1 << 28,
    1 << 29,
    1 << 30,
};

char const *labels[] =
{
    "1 Kb",      // 1 << 10
    "2 Kb",      // 1 << 11
    "4 Kb",      // 1 << 12
    "8 Kb",      // 1 << 13
    "16 Kb",     // 1 << 14
    "32 Kb",     // 1 << 15
    "64 Kb",     // 1 << 16
    "128 Kb",    // 1 << 17
    "256 Kb",    // 1 << 18
    "512 Kb",    // 1 << 19
    "1 Mb",      // 1 << 20
    "2 Mb",      // 1 << 21
    "4 Mb",      // 1 << 22
    "8 Mb",      // 1 << 23
    "16 Mb",     // 1 << 24
    "32 Mb",     // 1 << 25
    "64 Mb",     // 1 << 26
    "128 Mb",    // 1 << 27
    "256 Mb",    // 1 << 28
    "512 Mb",    // 1 << 29
    "1 Gb",      // 1 << 30
};

void reptest_do(int index, uint64 size, void *data)
{
    g_tester.label = labels[index];
    uint64 mask = sizes[index] - 1;
    while (is_testing(30))
    {
        reptest_begin_time();
        test_cache_size_asm(size, data, mask);
        reptest_end_time();
        reptest_count_bytes(size);
    }
}

int main()
{
    uint64 size = GIGABYTES(1);
    uint8 *data = (uint8 *) allocate_pages(size);
    // Write to all bytes, so Linux will really allocate
    // those pages, instead of pointing all of them to one
    // fake ass page.
    for (uint64 index = 0; index < size; index++) data[index] = (uint8) index;

    g_tester.print_results = true;
    // while (true)
    {
        for (int i = 0; i < ARRAY_COUNT(sizes); i++)
        {
            reptest_do(i, size, data);
        }
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
