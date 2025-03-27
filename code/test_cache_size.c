#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "os_specifics.c"

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


void test_cache_size(uint64 count, void *data, uint64 mask);
#pragma comment(lib, "cachesize")


uint64 sizes[] =
{
    1 << 6,
    1 << 7,
    1 << 8,
    1 << 9,
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
    "64 bytes",  // 1 << 6
    "128 bytes", // 1 << 7
    "256 bytes", // 1 << 8
    "512 bytes", // 1 << 9
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

float64 gbs[ARRAY_COUNT(sizes)];

#pragma optimize("", off)

void reptest_do(int32 index, uint64 count, void *data)
{
    g_tester.print_results = true;
    g_tester.label = labels[index];
    uint64 mask = sizes[index] - 1;
    while (is_testing(30))
    {
        reptest_begin_time();
        test_cache_size(count, data, mask);
        reptest_end_time();
        reptest_count_bytes(count);
    }

    // {
    //     uint64 time = g_tester.minimum.value[RepTestMeasurement_Time];
    //     uint64 bytes = g_tester.minimum.value[RepTestMeasurement_Bytes];
    //     uint64 page_faults = g_tester.minimum.value[RepTestMeasurement_PageFaults];
    //     printf("%llu,%lf\n", sizes[index], bytes / ((float64) GIGABYTES(1) * time * 1e-9));
    // }
}


int main()
{
    uint64 count = GIGABYTES(1);
    void *data = allocate_pages(count);
    // while (true)
    {
        for (int i = 0; i < ARRAY_COUNT(sizes); i++)
        {
            reptest_do(i, count, data);
        }
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
