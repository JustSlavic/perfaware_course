#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test is designed to find the maximum available load throughput.
    We know that on my Tiger Lake processor there are only two load execution ports,
    so we fix this parameter, and start varying the throughput of one instruction.

    x86_64
+----------------------+-----+-------------------+-------------------+----------------------+
| Label                |     | Time              | Bytes             | Page faults          |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_2x4bytes        | Min | 28244 us          | 35.406295 gb/s    |                      |
|                      | Max | 33363 us          | 29.973601 gb/s    |                      |
|                      | Avg | 29440 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_2x8bytes        | Min | 14158 us          | 70.629948 gb/s    |                      |
|                      | Max | 18071 us          | 55.337543 gb/s    |                      |
|                      | Avg | 14884 us          |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_2x16bytes       | Min | 7031 us           | 142.235944 gb/s   |                      |
|                      | Max | 10332 us          | 96.783894 gb/s    |                      |
|                      | Avg | 7712 us           |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| load_2x32bytes       | Min | 7053 us           | 141.779894 gb/s   |                      |
|                      | Max | 8184 us           | 122.187239 gb/s   |                      |
|                      | Avg | 7559 us           |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+

    The result shows that loading into xmm register renders the same speed as loading the ymm register,
    which is unexpected for me. Probably there's a some other limit.
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
