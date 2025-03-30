#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*

*/

typedef void (*callback_t)(uint64, void *);

void load_1x4bytes(uint64 size, void *data);
void load_2x4bytes(uint64 size, void *data);
void load_3x4bytes(uint64 size, void *data);
void load_4x4bytes(uint64 size, void *data);
void store_1x4bytes(uint64 size, void *data);
void store_2x4bytes(uint64 size, void *data);
void store_3x4bytes(uint64 size, void *data);
void store_4x4bytes(uint64 size, void *data);

char const *labels[] =
{
    "load_1x4bytes",
    "load_2x4bytes",
    "load_3x4bytes",
    "load_4x4bytes",
    "store_1x4bytes",
    "store_2x4bytes",
    "store_3x4bytes",
    "store_4x4bytes",
};

callback_t callbacks[] =
{
    load_1x4bytes,
    load_2x4bytes,
    load_3x4bytes,
    load_4x4bytes,
    store_1x4bytes,
    store_2x4bytes,
    store_3x4bytes,
    store_4x4bytes,
};

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    uint64 data;
    while (is_testing(60))
    {
        reptest_begin_time();
        callbacks[index](count, &data);
        reptest_end_time();
        reptest_count_bytes(count);
    }
}

int main()
{
    uint64 count = GIGABYTES(1);
    g_tester.print_results = true;
    // while (true)
    {
        for (int i = 0; i < ARRAY_COUNT(callbacks); i++)
        {
            reptest_do(i, count);
        }
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
