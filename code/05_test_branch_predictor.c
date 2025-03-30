#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test creates buffers with various contents, on which jumps will be performed.
    Such load will strain branch predictor and test its capabilities.

    I created 5 predictable buffers: full of ones, full of zeroes, with ones
    in every second place, in every third place, and in every forth place.

    Also send 2 buffers with random data, one from CRT's random number generator,
    and the second from OS'es random number generator.

    x86-64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpAlways        | Min | 923249 us         | 1.083132 gb/s     |                      |
|                   | Max | 1056770 us        | 0.946279 gb/s     |                      |
|                   | Avg | 942854 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpNever         | Min | 948210 us         | 1.054619 gb/s     |                      |
|                   | Max | 1124645 us        | 0.889170 gb/s     |                      |
|                   | Avg | 977888 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpEverySecond   | Min | 945744 us         | 1.057368 gb/s     |                      |
|                   | Max | 1158909 us        | 0.862881 gb/s     |                      |
|                   | Avg | 980807 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpEveryThird    | Min | 906157 us         | 1.103561 gb/s     |                      |
|                   | Max | 1077308 us        | 0.928240 gb/s     |                      |
|                   | Avg | 923270 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpEveryForth    | Min | 895473 us         | 1.116728 gb/s     |                      |
|                   | Max | 1076219 us        | 0.929179 gb/s     |                      |
|                   | Avg | 915088 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpRandomCRT     | Min | 7873115 us        | 0.127015 gb/s     |                      |
|                   | Max | 8648661 us        | 0.115625 gb/s     |                      |
|                   | Avg | 7980616 us        |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| JumpRandomOS      | Min | 8033269 us        | 0.124482 gb/s     |                      |
|                   | Max | 8193014 us        | 0.122055 gb/s     |                      |
|                   | Avg | 8056673 us        |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+

    Looks like Intel's branch predictor performs well on every predictable load, but
    performs equally bad on any random contents (which is expected);

*/


void test_branch_predictor_asm(uint64 count, void *data);

enum
{
    JumpAlways,
    JumpNever,
    JumpEverySecond,
    JumpEveryThird,
    JumpEveryForth,
    JumpRandomCRT,
    JumpRandomOS,
};

void fill_bytes(int32 jump_strategy, uint64 size, uint8 *data)
{
    switch (jump_strategy)
    {
        case JumpAlways:
            for (uint64 index = 0; index < size; index++)
                data[index] = 1;
            break;
        case JumpNever:
            for (uint64 index = 0; index < size; index++)
                data[index] = 0;
            break;
        case JumpEverySecond:
            for (uint64 index = 0; index < size; index++)
                data[index] = (index % 2) == 0;
            break;
        case JumpEveryThird:
            for (uint64 index = 0; index < size; index++)
                data[index] = (index % 3) == 0;
            break;
        case JumpEveryForth:
            for (uint64 index = 0; index < size; index++)
                data[index] = (index % 4) == 0;
            break;
        case JumpRandomCRT:
            for (uint64 index = 0; index < size; index++)
                data[index] = (uint8) rand();
            break;
        case JumpRandomOS:
            {
                int n = 0;
                uint32 chunk_size = 0xffffffff;
                while (size >= chunk_size)
                {
                    get_os_random_buffer(chunk_size, data + n*chunk_size);
                    size -= chunk_size;
                }
                if (size > 0)
                    get_os_random_buffer((uint32) size, data);
            }

            break;
    }
}

char const *labels[] =
{
    "JumpAlways",
    "JumpNever",
    "JumpEverySecond",
    "JumpEveryThird",
    "JumpEveryForth",
    "JumpRandomCRT",
    "JumpRandomOS",
};

#pragma optimize("", off)
void reptest_branch_predictor(uint64 size, int jump_strategy)
{
    uint8 *data = (uint8 *) allocate_pages(size);
    fill_bytes(jump_strategy, size, data);
    g_tester.label = labels[jump_strategy];
    while (is_testing(60))
    {
        reptest_begin_time();
        test_branch_predictor_asm(size, data);
        reptest_end_time();
        reptest_count_bytes(size);
    }
    free_pages(data, size);
}


int main()
{
    srand((uint32)time(0));
    uint64 size = GIGABYTES(1);
    g_tester.print_results = true;
    // while (true)
    {
        reptest_branch_predictor(size, JumpAlways);
        reptest_branch_predictor(size, JumpNever);
        reptest_branch_predictor(size, JumpEverySecond);
        reptest_branch_predictor(size, JumpEveryThird);
        reptest_branch_predictor(size, JumpEveryForth);
        reptest_branch_predictor(size, JumpRandomCRT);
        reptest_branch_predictor(size, JumpRandomOS);
    }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
