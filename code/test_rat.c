#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "base.h"
#include "os_specifics.c"

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"

void rat_add(void);
void rat_mov_add(void);
#pragma comment(lib, "rat")

typedef void (*test_function)(void);

test_function f[] =
{
    rat_add,
    rat_mov_add,
};

char const *labels[] =
{
    "rat_add",
    "rat_mov_add",
};

#pragma optimize("", off)

void reptest_rat(int32 index)
{
    g_tester.label = labels[index];
    while (is_testing(30))
    {
        reptest_begin_time();
        f[index]();
        reptest_end_time();
        reptest_count_bytes(1000000000);
    }
}


int main()
{
    while (true)
    {
        reptest_rat(0);
        reptest_rat(1);
    }
    return 0;
}


#include "timing.c"
#include "profiler.c"
#include "repetition_tester.c"
