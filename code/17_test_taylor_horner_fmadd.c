#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif

#if OS_WINDOWS
#include <immintrin.h>
#endif // OS_WINDOWS

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    int power;
    double x, e;
    char const *name;
} compute_taylor_result;


float64 sine_taylor(float64 x, int32 max_power)
{
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

#if OS_MAC
float64 sine_taylor_horner(float64 x, int32 max_power)
{
    float64x2 x2 = vdupq_n_f64(x * x);
    float64x2 a = vdupq_n_f64(0);
    for (int power = max_power; power > 0; power -= 2)
    {
        float64x2 b = vdupq_n_f64(1.0 / compute_taylor_term_denominator(power));
        a = vfmaq_f64(b, a, x2);
    }
    float64x2 simd_result = vmulq_f64(a, vdupq_n_f64(x));
    float64 result = vgetq_lane_f64(simd_result, 0);
    return result;
}
#endif // OS_MAC

#if OS_WINDOWS
float64 sine_taylor_horner(float64 x, int32 max_power)
{
    __m128d x2 = _mm_set_sd(x * x);
    __m128d a = _mm_set_sd(0);
    for (int power = max_power; power > 0; power -= 2)
    {
        __m128d b = _mm_set_sd(1.0 / compute_taylor_term_denominator(power));
        a = _mm_fmadd_sd(a, x2, b);
    }
    __m128d simd_result = _mm_mul_sd(a, _mm_set_sd(x));
    float64 result = _mm_cvtsd_f64(simd_result);
    return result;
}
#endif // OS_WINDOWS

compute_taylor_result compute_error_sine_taylor(
    math_domain domain,
    double step,
    int32 max_power,
    float64 (*f)(float64, int32))
{
    compute_taylor_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = sin(x);
        float64 y = f(x, max_power);
        float64 e = fabs(reference_y - y);
        if (e > max_error.e)
        {
            max_error.power = max_power;
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

    for (int power = 1; power < 32; power += 2)
    {
        compute_taylor_result error = compute_error_sine_taylor(
            (math_domain){ .min = 0, .max = half_pi },
            0.005*pi,
            power,
            sine_taylor);
        error.name = "Taylor";
        results[result_count++] = error;
    }
    for (int power = 1; power < 32; power += 2)
    {
        compute_taylor_result error = compute_error_sine_taylor(
            (math_domain){ .min = 0, .max = half_pi },
            0.005*pi,
            power,
            sine_taylor_horner);
        error.name = "Horner FMA";
        results[result_count++] = error;
    }

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
        if (results[i].power == 0) continue;
        int n = printf("%s", results[i].name);
        printf("%.*sof %dth power: e = %+.30lf\n",
            15-n, spaces,
            results[i].power, results[i].e);
    }

    return 0;
};
