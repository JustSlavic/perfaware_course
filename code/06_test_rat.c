#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    This test designed to show the work of the RAT (Register Allocation Table (?))

    The first test does two adds in a row, which make the first dependency chain.
    The second dependency chain is on 'dec'.

        add 0  dec
          |     |
        add 1   |
          |     |
        add 0  dec
          |     |
        ...  ...

    But the second example does mov before add, so although the
    name of the register is the same, the RAT makes sure that
    computation happens in parallel:

        mov 0  mov 1  dec
          |      |     |
        add 0  add 1  dec
          |      |     |
         ...   ...    ...

    Arm64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| rat_add_asm       | Min | 674476 us         | 1.482632 gb/s     |                      |
|                   | Max | 702283 us         | 1.423927 gb/s     |                      |
|                   | Avg | 675849 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| rat_mov_add_asm   | Min | 337379 us         | 2.964026 gb/s     |                      |
|                   | Max | 342224 us         | 2.922063 gb/s     |                      |
|                   | Avg | 338327 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+

    x86_64
+----------------------+-----+-------------------+-------------------+----------------------+
| Label                |     | Time              | Bytes             | Page faults          |
+----------------------+-----+-------------------+-------------------+----------------------+
| rat_add_asm          | Min | 453907 us         | 2.203093 gb/s     |                      |
|                      | Max | 468365 us         | 2.135086 gb/s     |                      |
|                      | Avg | 457509 us         |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| rat_mov_add_asm      | Min | 302796 us         | 3.302551 gb/s     |                      |
|                      | Max | 322440 us         | 3.101350 gb/s     |                      |
|                      | Avg | 307431 us         |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+

    The result on M1 CPU clearly shows that although using moves
    is seemingly more work, the dependency becomes "fake" and now
    computation happens two times faster, which aligns with our
    mental model of the CPU.

    For the x86_64 the result is similar, except the speedup is 50%, not 100%.
*/

typedef ASM_CALL void (*callback_t)(uint64);

ASM_CALL void rat_add_asm(uint64 count);
ASM_CALL void rat_mov_add_asm(uint64 count);

char const *labels[] =
{
    "rat_add_asm",
    "rat_mov_add_asm",
};

callback_t callbacks[] =
{
    rat_add_asm,
    rat_mov_add_asm,
};

void reptest_do(int index, uint64 count)
{
    g_tester.label = labels[index];
    while (is_testing(30))
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
