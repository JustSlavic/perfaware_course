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
    void *memory = allocate_pages(filesize);

    g_tester.label = "fread";
    while (is_testing(10))
    {
        FILE *f = fopen(filename, "rb");
        reptest_begin_time();
        uint64 read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);
        fclose(f);
    }

    free_pages(memory, filesize);
}

void reptest_mmap_fread()
{
    char const *filename = "data.json";
    uint64 filesize = get_filesize(filename);

    g_tester.label = "mmap + fread";
    while (is_testing(10))
    {
        FILE *f = fopen(filename, "rb");
        void *memory = allocate_pages(filesize);

        reptest_begin_time();
        uint64 read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);

        free_pages(memory, filesize);
        fclose(f);
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
    return 0;
}

#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
