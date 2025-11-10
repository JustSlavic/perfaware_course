#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if OS_MAC
#include <arm_neon.h>
typedef float32x2_t float32x2;
typedef float32x4_t float32x4;
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif // OS_MAC

#if OS_WINDOWS
#include <immintrin.h>
#endif // OS_WINDOWS

/*
    Find method of computing the square root by using SIMD intrinsics.

    x86_64:
stdlib vs stubs:
    sqrt_see_64                   f(+0.000000) = 0.00000000000000000000
    sqrt_see_32                   f(+0.420000) = 0.00000003896462652531
    rrsqrt_sse                    f(+0.930000) = 0.00037581828679578155
*/

#include "base.h"
#include "math_reference_tables.c"

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    char const *name;
    math_function *reference_function;
    math_function *function;
    math_domain domain;
    float64 step;
} function_to_function_compare;

#if OS_WINDOWS
float64 sqrt_see_64(float64 x)
{
    __m128d xmm0 = _mm_set_sd(x);
    __m128d xmm1 = _mm_sqrt_sd(xmm0, xmm0);
    float64 result = _mm_cvtsd_f64(xmm1);
    return result;
}

float64 sqrt_see_32(float64 x)
{
    __m128 xmm0 = _mm_set_ss((float32) x);
    __m128 xmm1 = _mm_sqrt_ss(xmm0);
    float64 result = _mm_cvtss_f32(xmm1);
    return result;
}

float64 rrsqrt_sse(float64 x)
{
    __m128 xmm0 = _mm_set_ss((float32) x);
    __m128 xmm1 = _mm_rsqrt_ss(xmm0);
    __m128 xmm2 = _mm_rcp_ss(xmm1);
    float64 result = _mm_cvtss_f32(xmm2);
    return result;
}

function_to_function_compare function_compares[] =
{
    {
        .name = "sqrt_see_64",
        .reference_function = sqrt,
        .function = sqrt_see_64,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sqrt_see_32",
        .reference_function = sqrt,
        .function = sqrt_see_32,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "rrsqrt_sse",
        .reference_function = sqrt,
        .function = rrsqrt_sse,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
};
#endif // OS_WINDOWS

#if OS_MAC
double sqrt_neon_32x2(double x)
{
    float32x2 xmm0 = vdup_n_f32(x);
    float32x2 xmm1 = vsqrt_f32(xmm0);
    double result = vget_lane_f32(xmm1, 0);
    return result;
}

double sqrt_neon_32x4(double x)
{
    float32x4 xmm0 = vdupq_n_f32(x);
    float32x4 xmm1 = vsqrtq_f32(xmm0);
    double result = vgetq_lane_f32(xmm1, 0);
    return result;
}

float64 sqrt_neon_64x1(float64 x)
{
    float64x1 xmm0 = vdup_n_f64(x);
    float64x1 xmm1 = vsqrt_f64(xmm0);
    float64 result = vget_lane_f64(xmm1, 0);
    return result;
}

float64 sqrt_neon_64x2(float64 x)
{
    float64x2 xmm0 = vdupq_n_f64(x);
    float64x2 xmm1 = vsqrtq_f64(xmm0);
    float64 result = vgetq_lane_f64(xmm1, 0);
    return result;
}

double rrsqrt_neon_32bit(double x)
{
    float32x2 xmm0 = vdup_n_f32(x);
    float32x2 xmm1 = vrsqrte_f32(xmm0);
    float32x2 xmm2 = vrecpe_f32(xmm1);
    double result = vget_lane_f32(xmm2, 0);
    return result;
}

double rrsqrt_neon_64bit(double x)
{
    float64x1 xmm0 = vdup_n_f64(x);
    float64x1 xmm1 = vrsqrte_f64(xmm0);
    float64x1 xmm2 = vrecpe_f64(xmm1);
    double result = vget_lane_f64(xmm2, 0);
    return result;
}

function_to_function_compare function_compares[] =
{
    {
        .name = "sqrt_neon_32x2",
        .reference_function = sqrt,
        .function = sqrt_neon_32x2,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sqrt_neon_32x4",
        .reference_function = sqrt,
        .function = sqrt_neon_32x4,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sqrt_neon_64x1",
        .reference_function = sqrt,
        .function = sqrt_neon_64x1,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sqrt_neon_64x2",
        .reference_function = sqrt,
        .function = sqrt_neon_64x2,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "rrsqrt_neon_32bit",
        .reference_function = sqrt,
        .function = rrsqrt_neon_32bit,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "rrsqrt_neon_64bit",
        .reference_function = sqrt,
        .function = rrsqrt_neon_64bit,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
};
#endif // OS_MAC

typedef struct
{
    float64 x, y;
} compute_error_result;

compute_error_result compute_error_function(function_to_function_compare *comp, FILE *csv)
{
    compute_error_result max_error = {};

    if (comp->domain.max < comp->domain.min)
        return max_error;

    if (csv)
        fprintf(csv, "x,y,e\n");

    float64 x = comp->domain.min;
    while (x < comp->domain.max)
    {
        float64 reference_y = comp->reference_function(x);
        float64 y = comp->function(x);
        float64 e = fabs(reference_y - y);
        if (e > max_error.y)
        {
            max_error.x = x;
            max_error.y = e;
        }

        if (csv)
            fprintf(csv, "%lf,%lf,%lf\n", x, y, e);
        x += comp->step;
    }

    return max_error;
}


int main()
{
    printf("stdlib vs stubs:\n");
    for (int i = 0; i < ARRAY_COUNT(function_compares); i++)
    {
#if 0
        char error_filename[256] = {};
        sprintf(error_filename, "%s_error_graph.csv", comp->name);
        FILE *error_file = fopen(error_filename, "wb+");
        if (!error_file)
            return max_error;
#endif
        compute_error_result error = compute_error_function(function_compares + i, NULL);
        printf("    %s%.*sf(%+lf) = %.20lf\n", function_compares[i].name, (int)(30 - strlen(function_compares[i].name)), spaces, error.x, error.y);
#if 0
    fclose(error_file);
#endif
    }

    return 0;
};
