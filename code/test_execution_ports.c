#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "os_specifics.c"

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

void test_load_ports_1(uint64 count, void *data);
void test_load_ports_2(uint64 count, void *data);
void test_load_ports_3(uint64 count, void *data);
void test_load_ports_4(uint64 count, void *data);
void test_store_ports_1(uint64 count, void *data);
void test_store_ports_2(uint64 count, void *data);
void test_store_ports_3(uint64 count, void *data);
void test_store_ports_4(uint64 count, void *data);
#pragma comment(lib, "exeports")

typedef void (*test_function)(uint64, void *);

test_function f[] =
{
    test_load_ports_1,
    test_load_ports_2,
    test_load_ports_3,
    test_load_ports_4,
    test_store_ports_1,
    test_store_ports_2,
    test_store_ports_3,
    test_store_ports_4,
};

char const *l[] =
{
    "load x1",
    "load x2",
    "load x3",
    "load x4",
    "store x1",
    "store x2",
    "store x3",
    "store x4",
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
