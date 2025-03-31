#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test is designed to show that loading one byte
    takes exactly the same time as loading the 4 or 8 bytes.

    Arm64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_1byte        | Min | 337398 us         | 2.963859 gb/s     |                      |
|                   | Max | 363984 us         | 2.747374 gb/s     |                      |
|                   | Avg | 340694 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_4bytes       | Min | 337300 us         | 2.964720 gb/s     |                      |
|                   | Max | 356167 us         | 2.807672 gb/s     |                      |
|                   | Avg | 344607 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_8bytes       | Min | 338228 us         | 2.956585 gb/s     |                      |
|                   | Max | 464335 us         | 2.153618 gb/s     |                      |
|                   | Avg | 346496 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
*/

typedef ASM_CALL void (*callback_t)(uint64, void *);

ASM_CALL void load_1byte(uint64 size, void *data);
ASM_CALL void load_4bytes(uint64 size, void *data);
ASM_CALL void load_8bytes(uint64 size, void *data);

char const *labels[] =
{
    "load_1byte",
    "load_4bytes",
    "load_8bytes",
};

callback_t callbacks[] =
{
    load_1byte,
    load_4bytes,
    load_8bytes,
};

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    uint64 data;
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
