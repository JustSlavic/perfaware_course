#include <stdio.h>
#include <stdlib.h>

#if OS_LINUX
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <libproc.h>
uint64 get_filesize(char const *filename)
{
    struct stat filestat;
    stat(filename, &filestat);
    uint64 result = filestat.st_size;
    return result;
}
void *allocate_pages(uint64 size)
{
    void *result = mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return result;
}
void free_pages(void *memory, uint64 size)
{
    munmap(memory, filesize);
}
#endif

#include "base.h"

#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
uint64 get_filesize(char const *filename)
{
    HANDLE handle = CreateFileA(filename, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    LARGE_INTEGER result;
    BOOL success = GetFileSizeEx(handle, &result);
    CloseHandle(handle);
    return result.QuadPart;
}
void *allocate_pages(uint64 size)
{
    void *result = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return result;
}
void free_pages(void *memory, uint64 size)
{
    VirtualFree(memory, size, MEM_RELEASE);
}
#endif

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

uint64 get_pagefaults_count();

int main()
{
    // while (true)
    // {
    //     reptest_fread();
    //     reptest_mmap_fread();
    // }

    get_pagefaults_count();

    int page_size = 1 << 12;
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

        printf("%d,%d,%llu,%d\n", page_count,touch_count,pf1 - pf0,touch_count - page_count);
    }

    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
