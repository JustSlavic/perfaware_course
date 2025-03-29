#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


int main()
{
    g_tester.print_results = true;

    get_pagefaults_count();

    printf("Pages, Touched, Page faults, Extra page faults\n");

#if OS_WINDOWS
    int page_size = 1 << 12;
#endif

#if OS_MAC
    int page_size = 1 << 14;
#endif

    int page_count = 100;
    int size = page_count * page_size;
    for (int touch_count = 0; touch_count < page_count; touch_count += 1)
    {
        int size_to_touch = touch_count * page_size;
        uint8 *memory = (uint8 *) allocate_pages(size);

        uint64 pf0 = get_pagefaults_count();
        for (int i = 0; i < size_to_touch; i++)
        {
            memory[i] = 0xcd;
        }
        uint64 pf1 = get_pagefaults_count();
        free_pages(memory, size);

        uint64 fault_count = pf1 - pf0;
        printf("%d,%d,%llu,%llu\n", page_count,touch_count,fault_count,fault_count - touch_count);
    }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
