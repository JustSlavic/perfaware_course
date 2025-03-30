#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    The point of this test is to determine, how we can put more work
    on the frontend of the CPU without doing any work (backend stalls)

    We start with the 1 3-byte nop, then we do 3 1-byte nops,
    and finally we do 9 1-byte nops.

    x86-64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| nop_3x1_asm       | Min | 604877 us         | 1.653229 gb/s     |                      |
|                   | Max | 737875 us         | 1.355242 gb/s     |                      |
|                   | Avg | 616647 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| nop_1x3_asm       | Min | 611761 us         | 1.634625 gb/s     |                      |
|                   | Max | 713409 us         | 1.401721 gb/s     |                      |
|                   | Avg | 619563 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| nop_1x9_asm       | Min | 1519604 us        | 0.658066 gb/s     |                      |
|                   | Max | 1830816 us        | 0.546205 gb/s     |                      |
|                   | Avg | 1541369 us        |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+

    We see that in x86-64 the replacement of 1 3-byte nop by 3 1-byte nops
    already has small performance impact.
    Doing 9 1-byte nops does stall CPU tremendously.
*/

void nop_3x1_asm(uint64 count);
void nop_1x3_asm(uint64 count);
void nop_1x9_asm(uint64 count);

typedef void (*callback_t)(uint64);

char const *labels[] =
{
    "nop_3x1_asm",
    "nop_1x3_asm",
    "nop_1x9_asm",
};

callback_t callbacks[] =
{
    nop_3x1_asm,
    nop_1x3_asm,
    nop_1x9_asm,
};

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    while (is_testing(60))
    {
        reptest_begin_time();
        callbacks[index](count);
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
