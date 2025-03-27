#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "os_specifics.c"

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

int mmx_available(void);
int sse_available(void);
int sse2_available(void);
int sse3_available(void);
int ssse3_available(void);
int sse4_1_available(void);
int sse4_2_available(void);

void test_load_2x32(uint64 count, void *data);
void test_load_2x64(uint64 count, void *data);
void test_load_2x128(uint64 count, void *data);
void test_load_2x256(uint64 count, void *data);
// void test_load_2x512(uint64 count, void *data);
#pragma comment(lib, "simdload")

typedef void (*test_function)(uint64, void *);

test_function f[] =
{
    test_load_2x32,
    test_load_2x64,
    test_load_2x128,
    test_load_2x256,
    // test_load_2x512,
};

char const *l[] =
{
    "test_load_2x32",
    "test_load_2x64",
    "test_load_2x128",
    "test_load_2x256",
    "test_load_2x512",
};


#pragma optimize("", off)

void reptest_do(int32 index, uint64 count)
{
    g_tester.label = l[index];
    uint64 data;
    while (is_testing(30))
    {
        reptest_begin_time();
        f[index](count, &data);
        reptest_end_time();
        reptest_count_bytes(count);
    }
}


int main()
{
    int mmx = mmx_available();
    int sse = sse_available();
    int sse2 = sse2_available();
    int sse3 = sse3_available();
    int ssse3 = ssse3_available();
    int sse41 = sse4_1_available();
    int sse42 = sse4_2_available();

    char const *HAS = "XXXXXXXXXXXXX";
    char const *DONT = "             ";

    printf("+-----+-----+------+------+-------+--------+--------+\n");
    printf("| MMX | SSE | SSE2 | SSE3 | SSSE3 | SSE4.1 | SSE4.2 |\n");
    printf("+-----+-----+------+------+-------+--------+--------+\n");
    printf("|%.*s|%.*s|%.*s|%.*s|%.*s|%.*s|%.*s|\n",
        5, mmx ? HAS : DONT,
        5, sse ? HAS : DONT,
        6, sse2 ? HAS : DONT,
        6, sse3 ? HAS : DONT,
        7, ssse3 ? HAS : DONT,
        8, sse41 ? HAS : DONT,
        8, sse42 ? HAS : DONT);
    printf("+-----+-----+------+------+-------+--------+--------+\n");

    while (true)
    {
        for (int i = 0; i < ARRAY_COUNT(f); i++)
        {
            reptest_do(i, 1000000);
        }
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
