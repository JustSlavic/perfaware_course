#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test should show us the number of execution ports
    for loading and storing data on the CPU.

    The test contains loops with 1, 2, 3, and 4 load instructions.
    In the previous test we saw that loading 1 or 8 bytes takes
    exactly the same time, so we load 8 bytes on each load.

    We increase number of loads frontend will issue every cycle,
    and then see which of the load count will stall on the backend.

    Arm64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_1x8bytes     | Min | 336823 us         | 2.968918 gb/s     |                      |
|                   | Max | 362010 us         | 2.762355 gb/s     |                      |
|                   | Avg | 337719 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_2x8bytes     | Min | 168552 us         | 5.932887 gb/s     |                      |
|                   | Max | 172019 us         | 5.813311 gb/s     |                      |
|                   | Avg | 168877 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_3x8bytes     | Min | 112430 us         | 8.894423 gb/s     |                      |
|                   | Max | 117578 us         | 8.504992 gb/s     |                      |
|                   | Avg | 114299 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| load_4x8bytes     | Min | 112378 us         | 8.898539 gb/s     |                      |
|                   | Max | 132487 us         | 7.547910 gb/s     |                      |
|                   | Avg | 114280 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| store_1x8bytes    | Min | 336787 us         | 2.969236 gb/s     |                      |
|                   | Max | 375165 us         | 2.665494 gb/s     |                      |
|                   | Avg | 338076 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| store_2x8bytes    | Min | 168537 us         | 5.933415 gb/s     |                      |
|                   | Max | 171621 us         | 5.826793 gb/s     |                      |
|                   | Avg | 168819 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| store_3x8bytes    | Min | 168495 us         | 5.934894 gb/s     |                      |
|                   | Max | 175860 us         | 5.686341 gb/s     |                      |
|                   | Avg | 168971 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| store_4x8bytes    | Min | 168549 us         | 5.932993 gb/s     |                      |
|                   | Max | 172420 us         | 5.799791 gb/s     |                      |
|                   | Avg | 168991 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+

    x86_64
+----------------------+-----+-------------------+-------------------+----------------------+
| Label                |     | Time              | Bytes             | Page faults          |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_1x8bytes        | Min | 28415 us          | 35.192871 gb/s    |                      |
|                      | Max | 30735 us          | 32.536281 gb/s    |                      |
|                      | Avg | 29785 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_2x8bytes        | Min | 14327 us          | 69.798029 gb/s    |                      |
|                      | Max | 17474 us          | 57.226539 gb/s    |                      |
|                      | Avg | 15006 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_3x8bytes        | Min | 14395 us          | 69.470874 gb/s    |                      |
|                      | Max | 16867 us          | 59.288578 gb/s    |                      |
|                      | Avg | 15202 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_4x8bytes        | Min | 14111 us          | 70.866201 gb/s    |                      |
|                      | Max | 15725 us          | 63.592351 gb/s    |                      |
|                      | Avg | 14856 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| store_1x8bytes       | Min | 28595 us          | 34.971390 gb/s    |                      |
|                      | Max | 30903 us          | 32.358957 gb/s    |                      |
|                      | Avg | 30010 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| store_2x8bytes       | Min | 14133 us          | 70.754082 gb/s    |                      |
|                      | Max | 15788 us          | 63.339460 gb/s    |                      |
|                      | Avg | 15049 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| store_3x8bytes       | Min | 14126 us          | 70.793764 gb/s    |                      |
|                      | Max | 17021 us          | 58.751788 gb/s    |                      |
|                      | Avg | 14965 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| store_4x8bytes       | Min | 14329 us          | 69.787937 gb/s    |                      |
|                      | Max | 16967 us          | 58.938633 gb/s    |                      |
|                      | Avg | 14873 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+

    On M1 we see that the speed of loads flatten outs on 3 loads per cycle.
    But the number of stores flattens on 2 stores per cycle.

    On my x86_64 Tiger Lake the test suggests 2 loads per cycle and 2 stores per cycle.
*/

typedef ASM_CALL void (*callback_t)(uint64, void *);

ASM_CALL void load_1x8bytes(uint64 size, void *data);
ASM_CALL void load_2x8bytes(uint64 size, void *data);
ASM_CALL void load_3x8bytes(uint64 size, void *data);
ASM_CALL void load_4x8bytes(uint64 size, void *data);
ASM_CALL void store_1x8bytes(uint64 size, void *data);
ASM_CALL void store_2x8bytes(uint64 size, void *data);
ASM_CALL void store_3x8bytes(uint64 size, void *data);
ASM_CALL void store_4x8bytes(uint64 size, void *data);

char const *labels[] =
{
    "load_1x8bytes",
    "load_2x8bytes",
    "load_3x8bytes",
    "load_4x8bytes",
    "store_1x8bytes",
    "store_2x8bytes",
    "store_3x8bytes",
    "store_4x8bytes",
};

callback_t callbacks[] =
{
    load_1x8bytes,
    load_2x8bytes,
    load_3x8bytes,
    load_4x8bytes,
    store_1x8bytes,
    store_2x8bytes,
    store_3x8bytes,
    store_4x8bytes,
};

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    uint64 data[32]; // well more than necessary
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
