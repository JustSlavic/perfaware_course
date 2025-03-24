#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "os_specifics.c"

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

void test_branch_predictor(uint64 count, void *data);
#pragma comment(lib, "bpredict")

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
        test_branch_predictor(size, data);
        reptest_end_time();
        reptest_count_bytes(size);
    }
    free_pages(data, size);
}


int main()
{
    srand((uint32)time(0));
    uint64 size = MEGABYTES(500);
    while (true)
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
