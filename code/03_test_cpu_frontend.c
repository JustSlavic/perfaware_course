#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

/*
    The point of this test is to determine, what CPU
    can do in parallel (instruction level parallelism).

    The assembly we have is something like that:

        mov [rdx + rax], al
        inc rax
        cmp rax, rcx
        jb .loop
        ret

    So, instruction stream will look something like that:

        mov [rdx + rax], al
        inc rax
        cmp rax, rcx
        jb .loop
        mov [rdx + rax], al
        inc rax
        cmp rax, rcx
        jb .loop
        mov [rdx + rax], al
        inc rax
        cmp rax, rcx
        jb .loop
        ...

    We have clear dependency on 'inc' instruction:

            inc
           / | \
        mov inc cmp(jb)
           / | \
        mov inc cmp(jb)

    We have clear dependency chain on 'inc'.

    What we get from this test:
    x86-64

    Arm64
+-------------------+-----+-------------------+-------------------+----------------------+
| Label             |     | Time              | Bytes             | Page faults          |
+-------------------+-----+-------------------+-------------------+----------------------+
| write_buffer_c    | Min | 336989 us         | 2.967456 gb/s     |                      |
|                   | Max | 442293 us         | 2.260945 gb/s     | 65536 (16.3840 kB/f) |
|                   | Avg | 338003 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| write_buffer_asm  | Min | 337249 us         | 2.965168 gb/s     |                      |
|                   | Max | 379286 us         | 2.636533 gb/s     |                      |
|                   | Avg | 338557 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| nop_buffer_asm    | Min | 337163 us         | 2.965924 gb/s     |                      |
|                   | Max | 353216 us         | 2.831129 gb/s     |                      |
|                   | Avg | 337676 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| cmp_buffer_asm    | Min | 336979 us         | 2.967544 gb/s     |                      |
|                   | Max | 340902 us         | 2.933394 gb/s     |                      |
|                   | Avg | 337496 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+
| dec_buffer_asm    | Min | 337213 us         | 2.965485 gb/s     |                      |
|                   | Max | 361329 us         | 2.767561 gb/s     |                      |
|                   | Avg | 338499 us         |                   |                      |
+-------------------+-----+-------------------+-------------------+----------------------+

    The results show that in x86-64 'mov' creates a bit of the slowdown,
    but the 4-byte nop also creates a little bit of slowdown, probably because
    of instruction decoding???

    This is not present on the M1 chip, probably because there's no bottleneck
    on the instruction decoding due to arm being fixed-sized encoding.
*/

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
    while (is_testing(60))
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
    // while (true)
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
