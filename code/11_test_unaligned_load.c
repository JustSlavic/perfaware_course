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

ASM_CALL uint64 load_8bytes_unaligned_asm(uint64 size, void *data, uint64 offset);

int main()
{
    uint64 iter_read_bytes = 128;
    uint64 size = GIGABYTES(1);
    uint64 max_alignment = 256;

    uint64 alignments[] = { 0, 1, 2, 3, 4, 15, 16, 17, 31, 32, 33, 63, 64, 65 };
    uint64 region_sizes[] = { 64*1024, 1024*1024, 32*1024*1024, size };

    uint8 *data = (uint8 *) allocate_pages(size + max_alignment);
    for (int i = 0; i < size; i++) { data[i] = (uint8) i; }

    g_tester.print_results = true;
    // while (true)
    {
        for (int region_index = 0; region_index < ARRAY_COUNT(region_sizes); region_index++)
        {
            for (int alignment_index = 0; alignment_index < ARRAY_COUNT(alignments); alignment_index++)
            {
                uint64 alignment = alignments[alignment_index];
                uint64 region_size = region_sizes[region_index];

                uint64 outer_loop_count = size / region_size;
                uint64 inner_loop_count = region_size / iter_read_bytes;
                uint64 bytes_read = (inner_loop_count * iter_read_bytes) * outer_loop_count;

                char label[64] = {};
                if (region_size > 1024*1024)
                    sprintf(label, "%.1lf Mb (%llu b)", (float64) region_size / MEGABYTES(1), alignment);
                else if (region_size > 1024)
                    sprintf(label, "%.1lf Kb (%llu b)", (float64) region_size / KILOBYTES(1), alignment);
                else
                    sprintf(label, "%llu b (%llu b)", region_size, alignment);

                g_tester.label = label;

                while (is_testing(5))
                {
                    reptest_begin_time();
                    load_8bytes_unaligned_asm(outer_loop_count, data + alignment, inner_loop_count);
                    reptest_end_time();
                    reptest_count_bytes(bytes_read);
                }
            }
        }
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
