#include <stdio.h>

#include "base.h"
#include "repetition_tester.h"

/*
    Selectively prevent optimizations

    x86_64
+----------------------+-----+-------------------+-------------------+----------------------+
| Label                |     | Time              | Bytes             | Page faults          |
+----------------------+-----+-------------------+-------------------+----------------------+
| reptest_escape_A     | Min | 112 us            | 0.829432 gb/s     |                      |
|                      | Max | 216 us            | 0.430521 gb/s     |                      |
|                      | Avg | 121 us            |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| reptest_escape_B     | Min | 835 us            | 0.111488 gb/s     |                      |
|                      | Max | 1311 us           | 0.071053 gb/s     |                      |
|                      | Avg | 870 us            |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| reptest_escape_C     | Min | 106 us            | 0.878653 gb/s     |                      |
|                      | Max | 149 us            | 0.626043 gb/s     |                      |
|                      | Avg | 112 us            |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
*/

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x2_t double2;
double2 v_splat(double x) { return vdupq_n_f64(x); }
double2 v_fma(double2 a, double2 b, double2 c) { return vfmaq_f64(a, b, c); }
double2 v_mul(double2 a, double2 b) { return vmulq_f64(a, b); }
double  v_get_x(double2 a) { return vgetq_lane_f64(a, 0) }
double2 v_sub(double2 a, double2 b) { return vsub_f64(a, b); }
double2 v_sqrt(double2 a) { return vsqrt_f64(a); }
#endif

#if OS_WINDOWS
#include <immintrin.h>
typedef __m128d double2;
double2 v_splat(double x) { return _mm_set1_pd(x); }
double2 v_fma(double2 a, double2 b, double2 c) { return _mm_fmadd_pd(b, c, a); }
double2 v_mul(double2 a, double2 b) { return _mm_mul_pd(a, b); }
double  v_get_x(double2 a) { return _mm_cvtsd_f64(a); }
double2 v_sub(double2 a, double2 b) { return _mm_sub_pd(a, b); }
double2 v_sqrt(double2 a) { return _mm_sqrt_pd(a); }
#endif // OS_WINDOWS


static float64 v_sine_kernel(float64 arg)
{
    double2 x = v_splat(arg);
    double2 xx = v_mul(x, x);
    double2 a = v_splat( 0x1.883c1c5deffbep-49);
    a = v_fma(v_splat(-0x1.ae43dc9bf8ba7p-41), a, xx);
    a = v_fma(v_splat( 0x1.6123ce513b09fp-33), a, xx);
    a = v_fma(v_splat(-0x1.ae6454d960ac4p-26), a, xx);
    a = v_fma(v_splat( 0x1.71de3a52aab96p-19), a, xx);
    a = v_fma(v_splat(-0x1.a01a01a014eb6p-13), a, xx);
    a = v_fma(v_splat( 0x1.11111111110c9p-7), a, xx);
    a = v_fma(v_splat(-0x1.5555555555555p-3), a, xx);
    a = v_fma(v_splat( 0x1p0), a, xx);
    a = v_mul(a, x);
    return v_get_x(a);
}

static void escape(void *p)
{
#if COMPILER_CLANG
    __asm volatile (
        ""
        : /* outputs */
        : /* inputs */ "g"(p)
        : /* clobbers */ "memory"
    );
#endif
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
#if COMPILER_GCC || COMPILER_CLANG
        __asm volatile ( "" : "=X"(x) );
#endif
        float64 y = v_sine_kernel(x);
#if COMPILER_GCC || COMPILER_CLANG
        __asm volatile ( "" : : "X"(y) );
#endif
        (void) y;
    }
}

void reptest_escape_C(int rep_count)
{
    for (int i = 0; i < rep_count; i++)
    {
        float64 x = 0.7;
#if COMPILER_GCC || COMPILER_CLANG
        __asm volatile ( "" : "+X"(x) );
#endif
        float64 y = v_sine_kernel(x);
#if COMPILER_GCC || COMPILER_CLANG
        __asm volatile ( "" : : "X"(y) );
#endif
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
