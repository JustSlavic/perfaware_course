#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"
#include "18_sine_coefficients.inl"

/*
    Compute accuracy of the sine taylor series, and polynomial we got from mathematician.

    x86_64
Taylor FMA Table      of 13 coeff (25th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 9 coeff (17th power): e = +0.000000000000000111022302462516
Minimax FMA Table[9]  of 9 coeff (17th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 10 coeff (19th power): e = +0.000000000000000111022302462516
Taylor FMA Table      of 11 coeff (21th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 11 coeff (21th power): e = +0.000000000000000111022302462516
Taylor FMA Table      of 12 coeff (23th power): e = +0.000000000000000111022302462516
    ...
*/

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x2_t double2;
double2 v_splat(double x) { return vdupq_n_f64(x); }
double2 v_fma(double2 a, double2 b, double2 c) { return vfmaq_f64(a, b, c); }
double2 v_mul(double2 a, double2 b) { return vmulq_f64(a, b); }
double  v_get_x(double2 a) { return vgetq_lane_f64(a, 0) }
#endif

#if OS_WINDOWS
#include <immintrin.h>
typedef __m128d double2;
double2 v_splat(double x) { return _mm_set1_pd(x); }
double2 v_fma(double2 a, double2 b, double2 c) { return _mm_fmadd_pd(b, c, a); }
double2 v_mul(double2 a, double2 b) { return _mm_mul_pd(a, b); }
double  v_get_x(double2 a) { return _mm_cvtsd_f64(a); }
#endif // OS_WINDOWS

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    int n_coeff;
    double x, e;
    char const *name;
} compute_result;


float64 compute_odd_power_polynomial(float64 x, float64 *table, int32 n_coeff)
{
    double2 sum = v_splat(0);
    double2 xx = v_splat(x * x);

    int32 index = n_coeff - 1;
    for (; index >= 0; index--)
    {
        double2 c = v_splat(table[index]);
        sum = v_fma(c, sum, xx);
    }
    sum = v_mul(sum, v_splat(x));

    double result = v_get_x(sum);
    return result;
}

float64 sine_fma_table_minimax_9(float64 x, float64 *table, int32 n_coeff)
{
    double2 xx = v_splat(x * x);
    double2 a = v_splat( 0x1.883c1c5deffbep-49);

    a = v_fma(v_splat(-0x1.ae43dc9bf8ba7p-41), a, xx);
    a = v_fma(v_splat( 0x1.6123ce513b09fp-33), a, xx);
    a = v_fma(v_splat(-0x1.ae6454d960ac4p-26), a, xx);
    a = v_fma(v_splat( 0x1.71de3a52aab96p-19), a, xx);
    a = v_fma(v_splat(-0x1.a01a01a014eb6p-13), a, xx);
    a = v_fma(v_splat( 0x1.11111111110c9p-7), a, xx);
    a = v_fma(v_splat(-0x1.5555555555555p-3), a, xx);
    a = v_fma(v_splat( 0x1p0), a, xx);
    a = v_mul(a, v_splat(x));

    double result = v_get_x(a);
    return result;
}


compute_result compute_sine_error(
    math_domain domain,
    double step,
    float64 *table,
    int32 n_coeff,
    float64 (*f)(float64, float64 *, int32))
{
    compute_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = sin(x);
        float64 y = f(x, table, n_coeff);
        float64 e = fabs(reference_y - y);
        if (e > max_error.e)
        {
            max_error.n_coeff = n_coeff;
            max_error.x = x;
            max_error.e = e;
        }
        x += step;
    }

    return max_error;
}

int main()
{
    compute_result results[128] = {};
    int32 result_count = 0;

    int32 n_coeff;
    for (n_coeff = 1; n_coeff < 14; n_coeff += 1)
    {
        if (n_coeff < ARRAY_COUNT(sine_taylor_coeffs))
        {
            compute_result error = compute_sine_error(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                sine_taylor_coeffs,
                n_coeff,
                compute_odd_power_polynomial);
            error.name = "Taylor FMA Table";
            results[result_count++] = error;
        }
        if (n_coeff > 1 && n_coeff < ARRAY_COUNT(sine_minimax_coeffs))
        {
            compute_result error = compute_sine_error(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                sine_minimax_coeffs[n_coeff],
                n_coeff,
                compute_odd_power_polynomial);
            error.name = "Minimax FMA Table";
            results[result_count++] = error;
        }

        if (n_coeff == 9)
        {
            compute_result error = compute_sine_error(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                NULL,
                n_coeff,
                sine_fma_table_minimax_9);
            error.name = "Minimax FMA Table[9]";
            results[result_count++] = error;
        }
    }

    // Sort
    for (int i = 0; i < ARRAY_COUNT(results); i++)
    {
        for (int j = i + 1; j < ARRAY_COUNT(results); j++)
        {
            if (results[j].e < results[i].e)
            {
                compute_result tmp = results[j];
                results[j] = results[i];
                results[i] = tmp;
            }
        }
    }

    for (int i = 0; i < ARRAY_COUNT(results); i++)
    {
        if (results[i].n_coeff == 0) continue;
        int n = printf("%s", results[i].name);
        printf("%.*sof %d coeff (%dth power): e = %+.30lf\n",
            22 - n, spaces,
            results[i].n_coeff, results[i].n_coeff * 2 - 1, results[i].e);
    }

    return 0;
};
