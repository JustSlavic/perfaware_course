#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test tries to show levels of cache by loading several buffer of different size.

    M1 CPU
    gb/s
    280
    140  *--*--*--*---*---*---*----*
    120
    100
     80                                 *----*--*--*--*--*
     60                                                      *---*---*----*----*----*--*
     40
     20
         1k 2k 4k 8k 16k 32k 64k 128k 256k 512k 1M 2M 4M 8M 16M 32M 64M 128M 256M 512M 1Gb


    Tiger Lake CPU
    gb/s
    280  *--*--*--*--*---*
    140                      *----*----*----*---*
    120
    100
     80
     60                                            *--*--*---*
     40                                                          *
     20                                                              *----*----*----*--*
        1k 2k 4k 8k 16k 32k 64k 128k 256k 512k  1M 2M 4M 8M 16M 32M 64M 128M 256M 512M 1Gb


    By the look of the graph, the M1 chip has 128kB of L1 cache, and 8MB of L2 cache.
    There's no L3 cache on the M-series chips.

    The Tiger Lake has 32kB of L1 cache, 1MB of L2 cache, and 16-32MB L3 cache.

    Data from the Internet:

    L1 Instruction Cache: 8 x 32 KB
    L1 Data Cache: 8 x 48 KB
    L2 Cache: 8 x 1280 KB
    L3 Cache: 24 MB

    or from another site:

    Cache L1:   80 KB (per core)
    Cache L2:   1.25 MB (per core)
    Cache L3:   24 MB (shared)
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
