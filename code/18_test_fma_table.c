#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"
#include "18_sine_coefficients.inl"

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif

enum
{
    SineTaylor = 0,
    SineHorner = 1,
};

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    int n_coeff;
    double x, e;
    char const *name;
} compute_taylor_result;


float64 sine_taylor(float64 x, int32 n_coeff)
{
    int32 max_power = n_coeff * 2 - 1;
    double sum = 0;
    double term_sign = 1;
    double x_powered = x;
    uint64 factorial = 1;
    for (int32 power = 1; power <= max_power; power += 2)
    {
        sum += term_sign * x_powered / factorial;

        term_sign *= -1;
        x_powered *= x * x;
        factorial *= (power + 1) * (power + 2);
    }

    return sum;
}

float64 sine_fma_table_taylor(float64 x, int32 n_coeff)
{
    int32 max_power = n_coeff * 2 - 1;
#define C sine_taylor_coeffs
    float64x2 sum = vdupq_n_f64(0);
    float64x2 x2 = vdupq_n_f64(x * x);

    int32 index = max_power / 2;
    for (; index >= 0; index--)
    {
        float64x2 c = vdupq_n_f64(C[index]);
        sum = vfmaq_f64(c, sum, x2);
    }
    sum = vmulq_f64(sum, vdupq_n_f64(x));

    double result = vgetq_lane_f64(sum, 0);
    return result;
#undef C
}

float64 sine_fma_table_minimax(float64 x, int32 n_coeff)
{
#define C sine_minimax_coeffs
    float64x2 sum = vdupq_n_f64(0);
    float64x2 xx = vdupq_n_f64(x * x);

    int32 index = n_coeff - 1;
    for (; index >= 0; index--)
    {
        float64x2 c = vdupq_n_f64(C[n_coeff][index]);
        sum = vfmaq_f64(c, sum, xx);
    }
    sum = vmulq_f64(sum, vdupq_n_f64(x));

    double result = vgetq_lane_f64(sum, 0);
    return result;
#undef C
}

float64 sine_fma_table_minimax_9(float64 x, int32 unused)
{
    (void) unused;
    float64x2 xx = vdupq_n_f64(x * x);
    float64x2 a = vdupq_n_f64( 0x1.883c1c5deffbep-49);

    a = vfmaq_f64(vdupq_n_f64(-0x1.ae43dc9bf8ba7p-41), a, xx);
    a = vfmaq_f64(vdupq_n_f64( 0x1.6123ce513b09fp-33), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.ae6454d960ac4p-26), a, xx);
    a = vfmaq_f64(vdupq_n_f64( 0x1.71de3a52aab96p-19), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.a01a01a014eb6p-13), a, xx);
    a = vfmaq_f64(vdupq_n_f64( 0x1.11111111110c9p-7), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.5555555555555p-3), a, xx);
    a = vfmaq_f64(vdupq_n_f64( 0x1p0), a, xx);
    a = vmulq_f64(a, vdupq_n_f64(x));

    double result = vgetq_lane_f64(a, 0);
    return result;
}

uint64 compute_factorial(uint64 x)
{
    uint64 result = 1;
    for (uint64 i = 2; i <= x; i++)
        result *= i;
    return result;
}

int64 compute_taylor_term_denominator(uint64 power)
{
    if (power % 2 == 0) return -999;

    int sign = ((power/2) % 2) ? -1 : 1;
    return sign * compute_factorial(power);
}

float64 sine_taylor_horner(float64 x, int32 n_coeff)
{
    int32 max_power = n_coeff * 2 - 1;
    float64x2 x2 = vdupq_n_f64(x * x);
    float64x2 a = vdupq_n_f64(0);
    for (int power = max_power; power > 0; power -= 2)
    {
        float64x2 b = vdupq_n_f64(1.0 / compute_taylor_term_denominator(power));
        a = vfmaq_f64(b, a, x2);
    }
    float64x2 simd_result = vmulq_f64(a, vdupq_n_f64(x));
    double result = vgetq_lane_f64(simd_result, 0);
    return result;
}

compute_taylor_result compute_error_sine_taylor(
    math_domain domain,
    double step,
    int32 n_coeff,
    float64 (*f)(float64, int32))
{
    compute_taylor_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = sin(x);
        float64 y = f(x, n_coeff);
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
    compute_taylor_result results[128] = {};
    int32 result_count = 0;

    int32 n_coeff;
    for (n_coeff = 1; n_coeff < 14; n_coeff += 1)
    {
        {
            compute_taylor_result error = compute_error_sine_taylor(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                n_coeff,
                sine_taylor);
            error.name = "Taylor";
            results[result_count++] = error;
        }
        {
            compute_taylor_result error = compute_error_sine_taylor(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                n_coeff,
                sine_taylor_horner);
            error.name = "Horner FMA";
            results[result_count++] = error;
        }
        if (n_coeff < ARRAY_COUNT(sine_taylor_coeffs))
        {
            compute_taylor_result error = compute_error_sine_taylor(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                n_coeff,
                sine_fma_table_taylor);
            error.name = "Taylor FMA Table";
            results[result_count++] = error;
        }
        if (n_coeff > 1 && n_coeff < ARRAY_COUNT(sine_minimax_coeffs))
        {
            compute_taylor_result error = compute_error_sine_taylor(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
                n_coeff,
                sine_fma_table_minimax);
            error.name = "Minimax FMA Table";
            results[result_count++] = error;
        }

        if (n_coeff == 9)
        {
            compute_taylor_result error = compute_error_sine_taylor(
                (math_domain){ .min = 0, .max = half_pi },
                0.005*pi,
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
                compute_taylor_result tmp = results[j];
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

    sine_fma_table_minimax(0.3, 9);
    sine_fma_table_minimax_9(0.3, 9);

    return 0;
};
