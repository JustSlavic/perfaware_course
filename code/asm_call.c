#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


void write_buffer_asm(uint64 count, void *data);
void noop_buffer_asm(uint64 count);
void cmp_buffer_asm(uint64 count);
void dec_buffer_asm(uint64 count);
#pragma comment(lib, "nop_loop")


void reptest_write_buffer(uint64 size)
{
    uint8 *data = (uint8 *) allocate_pages(size);
    while (is_testing(60))
    {
        g_tester.label = "write_buffer";
        reptest_begin_time();
        for (uint64 index = 0; index < size; index++)
        {
            data[index] = (uint8) index;
        }
        reptest_end_time();
        reptest_count_bytes(size);
    }
    free_pages(data, size);
}

void reptest_write_buffer_asm(uint64 size)
{
    uint8 *data = (uint8 *) allocate_pages(size);
    while (is_testing(60))
    {
        g_tester.label = "write_buffer_asm";
        reptest_begin_time();
        write_buffer_asm(size, data);
        reptest_end_time();
        reptest_count_bytes(size);
    }
    free_pages(data, size);
}

void reptest_noop_buffer_asm(uint64 size)
{
    while (is_testing(60))
    {
        g_tester.label = "noop_buffer_asm";
        reptest_begin_time();
        noop_buffer_asm(size);
        reptest_end_time();
        reptest_count_bytes(size);
    }
}

void reptest_cmp_buffer_asm(uint64 size)
{
    while (is_testing(60))
    {
        g_tester.label = "cmp_buffer_asm";
        reptest_begin_time();
        cmp_buffer_asm(size);
        reptest_end_time();
        reptest_count_bytes(size);
    }
}

void reptest_dec_buffer_asm(uint64 size)
{
    while (is_testing(60))
    {
        g_tester.label = "dec_buffer_asm";
        reptest_begin_time();
        dec_buffer_asm(size);
        reptest_end_time();
        reptest_count_bytes(size);
    }
}

int main()
{
    g_tester.print_results = true;
    uint64 size = GIGABYTES(1);
    // while (true)
    {
        reptest_write_buffer(size);
        reptest_write_buffer_asm(size);
        reptest_noop_buffer_asm(size);
        reptest_cmp_buffer_asm(size);
        reptest_dec_buffer_asm(size);
    }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
