#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"
#include "19_asine_coefficients.inl"

/*
    Compute the accuracy of taylor and minimax arcsine approximations.
    Find top accuracy solution.

Minimax FMA Table     of 20 coeff (39th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 21 coeff (41th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 19 coeff (37th power): e = +0.000000000000000111022302462516
Minimax FMA Table[19] of 19 coeff (37th power): e = +0.000000000000000111022302462516
Minimax FMA Table     of 18 coeff (35th power): e = +0.000000000000000222044604925031
Minimax FMA Table     of 17 coeff (33th power): e = +0.000000000000000555111512312578
    ...

    We will use the 19th power of minimax polynomial.
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

float64 arcsine_fma_table_minimax_19(float64 x, float64 *table, int32 n_coeff)
{
    double2 xx = v_splat(x * x);
    double2 a = v_splat( 0x1.dfc53682725cap-1);

    a = v_fma(v_splat(-0x1.bec6daf74ed61p1), a, xx);
    a = v_fma(v_splat(0x1.8bf4dadaf548cp2), a, xx);
    a = v_fma(v_splat(-0x1.b06f523e74f33p2), a, xx);
    a = v_fma(v_splat(0x1.4537ddde2d76dp2), a, xx);
    a = v_fma(v_splat(-0x1.6067d334b4792p1), a, xx);
    a = v_fma(v_splat(0x1.1fb54da575b22p0), a, xx);
    a = v_fma(v_splat(-0x1.57380bcd2890ep-2), a, xx);
    a = v_fma(v_splat(0x1.69b370aad086ep-4), a, xx);
    a = v_fma(v_splat(-0x1.21438ccc95d62p-8), a, xx);
    a = v_fma(v_splat(0x1.b8a33b8e380efp-7), a, xx);
    a = v_fma(v_splat(0x1.c37061f4e5f55p-7), a, xx);
    a = v_fma(v_splat(0x1.1c875d6c5323dp-6), a, xx);
    a = v_fma(v_splat(0x1.6e88ce94d1149p-6), a, xx);
    a = v_fma(v_splat(0x1.f1c73443a02f5p-6), a, xx);
    a = v_fma(v_splat(0x1.6db6db3184756p-5), a, xx);
    a = v_fma(v_splat(0x1.3333333380df2p-4), a, xx);
    a = v_fma(v_splat(0x1.555555555531ep-3), a, xx);
    a = v_fma(v_splat(0x1p0), a, xx);
    a = v_mul(a, v_splat(x));

    double result = v_get_x(a);
    return result;
}


compute_result compute_arcsine_error(
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
        float64 reference_y = asin(x);
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
    for (n_coeff = 1; n_coeff < 22; n_coeff += 1)
    {
        if (n_coeff < ARRAY_COUNT(asine_taylor_coeffs))
        {
            compute_result error = compute_arcsine_error(
                (math_domain){ .min = 0, .max = one_over_sqrt_2 },
                0.001,
                asine_taylor_coeffs,
                n_coeff,
                compute_odd_power_polynomial);
            error.name = "Taylor FMA Table";
            results[result_count++] = error;
        }
        if (n_coeff > 1 && n_coeff < ARRAY_COUNT(asine_minimax_coeffs))
        {
            compute_result error = compute_arcsine_error(
                (math_domain){ .min = 0, .max = one_over_sqrt_2 },
                0.001,
                asine_minimax_coeffs[n_coeff],
                n_coeff,
                compute_odd_power_polynomial);
            error.name = "Minimax FMA Table";
            results[result_count++] = error;
        }

        if (n_coeff == 19)
        {
            compute_result error = compute_arcsine_error(
                (math_domain){ .min = 0, .max = one_over_sqrt_2 },
                0.005*pi,
                NULL,
                n_coeff,
                arcsine_fma_table_minimax_19);
            error.name = "Minimax FMA Table[19]";
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
