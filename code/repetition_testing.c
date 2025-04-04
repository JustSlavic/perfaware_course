#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "os_specifics.c"

#undef bool


#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


void reptest_fread()
{
    char const *filename = "data.json";

    uint64 filesize = get_filesize(filename);

    void *memory = malloc(filesize);
    g_tester.label = "fread";
    while (is_testing(5))
    {
        FILE *f = fopen(filename, "rb");

        reptest_begin_time();
        uint64 read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);

        fclose(f);
    }
    free(memory);
}

void reptest_mmap_fread()
{
    char const *filename = "data.json";

    uint64 filesize = get_filesize(filename);

    g_tester.label = "mmap + fread";
    while (is_testing(5))
    {
        void *memory = allocate_pages(filesize);
        FILE *f = fopen(filename, "rb");

        reptest_begin_time();
        uint64 read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);

        fclose(f);
        free_pages(memory, filesize);
    }
}

int main()
{
    g_tester.print_results = true;
    while (true)
    {
        reptest_fread();
        reptest_mmap_fread();
    }

    // get_pagefaults_count();

    // int page_size = 1 << 12;
    // int page_count = 100;
    // int size = page_count * page_size;
    // for (int touch_count = 0; touch_count < page_count; touch_count += 1)
    // {
    //     int size_to_touch = touch_count * page_size;
    //     uint8 *memory = (uint8 *) allocate_pages(size);

    //     uint64 pf0 = get_pagefaults_count();
    //     for (int i = 0; i < size_to_touch; i++)
    //     {
    //         memory[i] = 0xcd;
    //     }
    //     uint64 pf1 = get_pagefaults_count();
    //     free_pages(memory, size);

    //     printf("%d,%d,%llu,%d\n", page_count,touch_count,pf1 - pf0,touch_count - page_count);
    // }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
