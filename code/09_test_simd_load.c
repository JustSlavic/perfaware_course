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

typedef ASM_CALL void (*callback_t)(uint64, void *);

#if OS_WINDOWS || OS_LINUX
ASM_CALL void load_2x4bytes(uint64 count, void *data);
ASM_CALL void load_2x8bytes(uint64 count, void *data);
ASM_CALL void load_2x16bytes(uint64 count, void *data);
ASM_CALL void load_2x32bytes(uint64 count, void *data);

char const *labels[] =
{
    "load_2x4bytes",
    "load_2x8bytes",
    "load_2x16bytes",
    "load_2x32bytes",
};

callback_t callbacks[] =
{
    load_2x4bytes,
    load_2x8bytes,
    load_2x16bytes,
    load_2x32bytes,
};
#elif OS_MAC
ASM_CALL void load_3x4bytes(uint64 count, void *data);
ASM_CALL void load_3x8bytes(uint64 count, void *data);
ASM_CALL void load_3x32bytes(uint64 count, void *data);

char const *labels[] =
{
    "load_3x4bytes",
    "load_3x8bytes",
    "load_3x32bytes",
};

callback_t callbacks[] =
{
    load_3x4bytes,
    load_3x8bytes,
    load_3x32bytes,
};
#endif

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    uint64 data[32]; // well more than necessary
    while (is_testing(30))
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
