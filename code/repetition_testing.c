#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <libproc.h>
#undef bool

#include "base.h"
#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"


void reptest_fread()
{
    char const *filename = "data.json";

    struct stat filestat;
    stat(filename, &filestat);

    uint64 filesize = filestat.st_size;

    void *memory = malloc(filesize);
    g_tester.label = "fread";
    while (is_testing(5))
    {
        FILE *f = fopen(filename, "rb");

        reptest_begin_time();
        int read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);

        fclose(f);
    }
    free(memory);
}

void reptest_mmap_fread()
{
    char const *filename = "data.json";

    struct stat filestat;
    stat(filename, &filestat);

    uint64 filesize = filestat.st_size;

    g_tester.label = "mmap + fread";
    while (is_testing(5))
    {
        void *memory = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        FILE *f = fopen(filename, "rb");

        reptest_begin_time();
        int read = fread(memory, filesize, 1, f);
        reptest_end_time();
        if (read) reptest_count_bytes(filesize);

        fclose(f);
        munmap(memory, filesize);
    }
}

uint32 get_pagefaults_count();

int main()
{
    // while (true)
    // {
    //     reptest_fread();
    //     reptest_mmap_fread();
    // }

    for (int size = 1 << 14; size < MEGABYTES(50); size += (1 * (1 << 14)))
    {
        void *memory = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        uint32 pf0 = get_pagefaults_count();

        for (int i = size - 1; i >= 0; i--)
        {
            *((char *) memory + i) = 0xcd;
        }

        uint32 pf1 = get_pagefaults_count();
        printf("%lf,%u\n", size * 1e-6, pf1 - pf0);

        munmap(memory, size);
    }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
