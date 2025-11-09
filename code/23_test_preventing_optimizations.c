#include <stdio.h>

#include "base.h"
#include "repetition_tester.h"

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif


static float64 v_sine_kernel(float64 arg)
{
    float64x1 x = vdup_n_f64(arg);
    float64x1 xx = vmul_f64(x, x);
    float64x1 a = vdup_n_f64( 0x1.883c1c5deffbep-49);
    a = vfma_f64(vdup_n_f64(-0x1.ae43dc9bf8ba7p-41), a, xx);
    a = vfma_f64(vdup_n_f64( 0x1.6123ce513b09fp-33), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.ae6454d960ac4p-26), a, xx);
    a = vfma_f64(vdup_n_f64( 0x1.71de3a52aab96p-19), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.a01a01a014eb6p-13), a, xx);
    a = vfma_f64(vdup_n_f64( 0x1.11111111110c9p-7), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.5555555555555p-3), a, xx);
    a = vfma_f64(vdup_n_f64( 0x1p0), a, xx);
    a = vmul_f64(a, x);
    return a;
}

static void escape(void *p)
{
    __asm volatile (
        ""
        : /* outputs */
        : /* inputs */ "g"(p)
        : /* clobbers */ "memory"
    );
}

void reptest_escape_A(int rep_count)
{
    for (int i = 0; i < rep_count; i++)
    {
        float64 x = 0.7;
        escape(&x);
        float64 y = v_sine_kernel(x);
        escape(&y);
        (void) y;
    }
}

void reptest_escape_B(int rep_count)
{
    for (int i = 0; i < rep_count; i++)
    {
        float64 x = 0.7;
        __asm volatile ( "" : "=X"(x) );
        float64 y = v_sine_kernel(x);
        __asm volatile ( "" : : "X"(y) );
        (void) y;
    }
}

void reptest_escape_C(int rep_count)
{
    for (int i = 0; i < rep_count; i++)
    {
        float64 x = 0.7;
        __asm volatile ( "" : "+X"(x) );
        float64 y = v_sine_kernel(x);
        __asm volatile ( "" : : "X"(y) );
        (void) y;
    }
}

void reptest_do(int rep_count, void (*f)(int), char const *name)
{
    g_tester.label = name;
    while (is_testing(10))
    {
        reptest_begin_time();
        f(rep_count);
        reptest_end_time();
        reptest_count_bytes(rep_count);
    }
}

int main()
{
    int rep_count = 100000;
    g_tester.print_results = true;
    reptest_do(rep_count, reptest_escape_A, "reptest_escape_A");
    reptest_do(rep_count, reptest_escape_B, "reptest_escape_B");
    reptest_do(rep_count, reptest_escape_C, "reptest_escape_C");
    return 0;
}

#include "repetition_tester.c"
#include "timing.c"
#include "os_specifics.c"
