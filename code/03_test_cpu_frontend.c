#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


void write_buffer_c(uint64 count, void *data)
{
    for (uint64 index = 0; index < count; index++)
    {
        ((uint8 *) data)[index] = (uint8) index;
    }
}
void write_buffer_asm(uint64 count, void *data);
void nop_buffer_asm(uint64 count, void *data);
void cmp_buffer_asm(uint64 count, void *data);
void dec_buffer_asm(uint64 count, void *data);

typedef void (*callback_t)(uint64, void *);

char const *labels[] =
{
    "write_buffer_c",
    "write_buffer_asm",
    "nop_buffer_asm",
    "cmp_buffer_asm",
    "dec_buffer_asm",
};

callback_t callbacks[] =
{
    write_buffer_c,
    write_buffer_asm,
    nop_buffer_asm,
    cmp_buffer_asm,
    dec_buffer_asm,
};

void reptest_do(int index, uint64 count, void *data)
{
    g_tester.label = labels[index];
    while (is_testing(10))
    {
        reptest_begin_time();
        callbacks[index](count, data);
        reptest_end_time();
        reptest_count_bytes(count);
    }
}

int main()
{
    uint64 count = GIGABYTES(1);
    uint8 *data = (uint8 *) allocate_pages(count);
    g_tester.print_results = true;
    while (true)
    {
        for (int i = 0; i < ARRAY_COUNT(callbacks); i++)
        {
            reptest_do(i, count, data);
        }
    }
    free_pages(data, count);
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
