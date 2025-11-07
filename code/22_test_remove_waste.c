#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"
#include "00_reference_haversine.c"


#define EARTH_RADIUS 6372.8

float64 my_sqrt(float64 x)
{
    float64x1 xmm0 = vdup_n_f64(x);
    float64x1 xmm1 = vsqrt_f64(xmm0);
    float64 result = vget_lane_f64(xmm1, 0);
    return result;
}

float64x1 v_sine_kernel(float64x1 x)
{
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

float64 my_sine(float64 arg)
{
    float64 abs_x = fabs(arg);
    float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;

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
    return (arg < 0) ? -result : result;
}

float64 my_cosine(float64 x)
{
    float64 result = my_sine(x + half_pi);
    return result;
}

float64x1 v_arcsine_kernel(float64x1 x)
{
    float64x1 xx = vmul_f64(x, x);
    float64x1 a = vdup_n_f64(0x1.dfc53682725cap-1);
    a = vfma_f64(vdup_n_f64(-0x1.bec6daf74ed61p1), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.8bf4dadaf548cp2), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.b06f523e74f33p2), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.4537ddde2d76dp2), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.6067d334b4792p1), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.1fb54da575b22p0), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.57380bcd2890ep-2), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.69b370aad086ep-4), a, xx);
    a = vfma_f64(vdup_n_f64(-0x1.21438ccc95d62p-8), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.b8a33b8e380efp-7), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.c37061f4e5f55p-7), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.1c875d6c5323dp-6), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.6e88ce94d1149p-6), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.f1c73443a02f5p-6), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.6db6db3184756p-5), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.3333333380df2p-4), a, xx);
    a = vfma_f64(vdup_n_f64(0x1.555555555531ep-3), a, xx);
    a = vfma_f64(vdup_n_f64(0x1p0), a, xx);
    a = vmul_f64(a, x);
    return a;
}

float64 my_arcsine(float64 arg)
{
    int need_transform = arg > one_over_sqrt_2;
    float64 x = need_transform ? my_sqrt(1 - arg*arg) : arg;

    float64x2 xx = vdupq_n_f64(x * x);
    float64x2 a = vdupq_n_f64(0x1.dfc53682725cap-1);
    a = vfmaq_f64(vdupq_n_f64(-0x1.bec6daf74ed61p1), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.8bf4dadaf548cp2), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.b06f523e74f33p2), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.4537ddde2d76dp2), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.6067d334b4792p1), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.1fb54da575b22p0), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.57380bcd2890ep-2), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.69b370aad086ep-4), a, xx);
    a = vfmaq_f64(vdupq_n_f64(-0x1.21438ccc95d62p-8), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.b8a33b8e380efp-7), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.c37061f4e5f55p-7), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.1c875d6c5323dp-6), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.6e88ce94d1149p-6), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.f1c73443a02f5p-6), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.6db6db3184756p-5), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.3333333380df2p-4), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1.555555555531ep-3), a, xx);
    a = vfmaq_f64(vdupq_n_f64(0x1p0), a, xx);
    a = vmulq_f64(a, vdupq_n_f64(x));

    double result = vgetq_lane_f64(a, 0);
    return need_transform ? half_pi - result : result;
}


float64 uniform_float64(float64 a, float64 b)
{
    float64 t = (float64) rand() / (float64) RAND_MAX;
    float64 result = (1.0 - t) * a + t * b;
    return result;
}

typedef struct
{
    float64 x0, y0, x1, y1;
    float64 answer;
} haversine_computation;

float64 compute_haversine_buffer(haversine_computation *hcomps, int32 n)
{
    float64 one_over_n = 1.0 / n;
    float64 average = 0;
    for (int i = 0; i < n; i++)
    {
        float64 x0 = uniform_float64(-180.0, 180.0);
        float64 y0 = uniform_float64(-90.0, 90.0);
        float64 x1 = uniform_float64(-180.0, 180.0);
        float64 y1 = uniform_float64(-90.0, 90.0);

        float64 answer = reference_haversine(x0, y0, x1, y1, EARTH_RADIUS);
        average += answer * one_over_n;

        hcomps[i].x0 = x0;
        hcomps[i].y0 = y0;
        hcomps[i].x1 = x1;
        hcomps[i].y1 = y1;
        hcomps[i].answer = answer;
    }

    return average;
}

float64 compute_haversine_buffer_average_A(haversine_computation *hcomps, int32 n)
{
    float64 one_over_n = 1.0 / n;
    float64 average = 0;
    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = deg_to_rad(x1 - x0);
        float64 dlat = deg_to_rad(y1 - y0);

        float64 lat0 = deg_to_rad(y0);
        float64 lat1 = deg_to_rad(y1);

        float64 a = square(my_sine(dlat/2.0)) + my_cosine(lat0)*my_cosine(lat1)*square(my_sine(dlon/2.0));
        float64 c = 2.0*my_arcsine(my_sqrt(a));

        float64 answer = EARTH_RADIUS * c;
        average += answer * one_over_n;
    }
    return average;
}

float64 compute_haversine_buffer_average_B(haversine_computation *hcomps, int32 n)
{
    // Move EARTH_RADIUS * one_over_n computation outside of the loop
    float64 one_over_n = 1.0 / n;
    float64 average = 0;

    float64 coeff = EARTH_RADIUS * one_over_n;

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = (x1 - x0) * 0.01745329251994329577;
        float64 dlat = (y1 - y0) * 0.01745329251994329577;

        float64 lat0 = (y0) * 0.01745329251994329577;
        float64 lat1 = (y1) * 0.01745329251994329577;

        float64 s1 = my_sine(dlat/2.0);
        float64 s2 = my_sine(dlon/2.0);
        float64 c1 = my_cosine(lat0);
        float64 c2 = my_cosine(lat1);

        float64 a = square(s1) + c1*c2*square(s2);
        float64 c = 2.0*my_arcsine(my_sqrt(a));

        average += coeff * c ;
    }
    return average;
}

float64 compute_haversine_buffer_average_C(haversine_computation *hcomps, int32 n)
{
    // Compute dlon and dlat by premultiplying by 0.5 constant
    float64 one_over_n = 1.0 / n;
    float64 average = 0;

    float64 coeff = EARTH_RADIUS * one_over_n;

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        float64 lat0 = (y0) * 0.01745329251994329577;
        float64 lat1 = (y1) * 0.01745329251994329577;

        float64 s1 = my_sine(dlat);
        float64 s2 = my_sine(dlon);
        float64 c1 = my_cosine(lat0);
        float64 c2 = my_cosine(lat1);

        float64 a = square(s1) + c1*c2*square(s2);
        float64 c = 2.0*my_arcsine(my_sqrt(a));

        average += coeff * c ;
    }
    return average;
}

float64 compute_haversine_buffer_average_D(haversine_computation *hcomps, int32 n)
{
    // Do fmadd at the end
    float64 one_over_n = 1.0 / n;
    float64x1 average = vdup_n_f64(0);
    float64x1 coeff = vdup_n_f64(EARTH_RADIUS * one_over_n);

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        float64 lat0 = (y0) * 0.01745329251994329577;
        float64 lat1 = (y1) * 0.01745329251994329577;

        float64 s1 = my_sine(dlat);
        float64 s2 = my_sine(dlon);
        float64 c1 = my_sine(lat0 + half_pi);
        float64 c2 = my_sine(lat1 + half_pi);

        float64 a = square(s1) + c1 * c2 * square(s2);
        float64 sq1 = my_sqrt(a);
        float64 c = 2.0*my_arcsine(sq1);

        average = vfma_f64(average, coeff, vdup_n_f64(c));
    }

    float64 result = vget_lane_f64(average, 0);
    return result;
}

float64 compute_haversine_buffer_average_E(haversine_computation *hcomps, int32 n)
{
    // Do fmadd for lat0 lat1
    float64 one_over_n = 1.0 / n;
    float64x1 average = vdup_n_f64(0);
    float64x1 coeff = vdup_n_f64(EARTH_RADIUS * one_over_n);
    float64x1 deg_to_rad_coeff = vdup_n_f64(0.01745329251994329577);
    float64x1 v_half_pi = vdup_n_f64(half_pi);

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        float64 lat0 = vfma_f64(v_half_pi, vdup_n_f64(y0), deg_to_rad_coeff);
        float64 lat1 = vfma_f64(v_half_pi, vdup_n_f64(y1), deg_to_rad_coeff);

        float64 s1 = my_sine(dlat);
        float64 s2 = my_sine(dlon);
        float64 c1 = my_sine(lat0);
        float64 c2 = my_sine(lat1);

        float64 a = square(s1) + c1 * c2 * square(s2);
        float64 sq1 = my_sqrt(a);
        float64 c = 2.0*my_arcsine(sq1);

        average = vfma_f64(average, coeff, vdup_n_f64(c));
    }

    float64 result = vget_lane_f64(average, 0);
    return result;
}

float64 compute_haversine_buffer_average_F(haversine_computation *hcomps, int32 n)
{
    // Unwrap sine
    float64x1 average = vdup_n_f64(0);
    float64x1 coeff = vdup_n_f64(2.0 * EARTH_RADIUS / n);
    float64x1 deg_to_rad_coeff = vdup_n_f64(0.01745329251994329577);
    float64x1 v_half_pi = vdup_n_f64(half_pi);

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        // Combined convert to radians and divide by 2
        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        // Convert to radians + add half_pi for cos compute
        float64 lat0 = vfma_f64(v_half_pi, vdup_n_f64(y0), deg_to_rad_coeff);
        float64 lat1 = vfma_f64(v_half_pi, vdup_n_f64(y1), deg_to_rad_coeff);

        // Unwrap sine to remove branch at the end, because we square result anyway
        float64x1 s1;
        {
            float64 abs_x = fabs(dlat);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s1 = v_sine_kernel(vdup_n_f64(x));
        }

        float64x1 s2;
        {
            float64 abs_x = fabs(dlon);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s2 = v_sine_kernel(vdup_n_f64(x));
        }

        float64 c1 = my_sine(lat0);
        float64 c2 = my_sine(lat1);

        float64 a = vfma_f64(vmul_f64(s1, s1), vdup_n_f64(c1 * c2), vmul_f64(s2, s2));
        float64 sq1 = my_sqrt(a);
        float64 c = my_arcsine(sq1);

        average = vfma_f64(average, coeff, vdup_n_f64(c));
    }

    float64 result = vget_lane_f64(average, 0);
    return result;
}

float64 compute_haversine_buffer_average_G(haversine_computation *hcomps, int32 n)
{
    // Unwrap sine
    float64x1 average = vdup_n_f64(0);
    float64x1 coeff = vdup_n_f64(2.0 * EARTH_RADIUS / n);
    float64x1 deg_to_rad_coeff = vdup_n_f64(0.01745329251994329577);
    float64x1 v_half_pi = vdup_n_f64(half_pi);

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        // Combined convert to radians and divide by 2
        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        // Convert to radians + add half_pi for cos compute
        float64 lat0 = vfma_f64(v_half_pi, vdup_n_f64(y0), deg_to_rad_coeff);
        float64 lat1 = vfma_f64(v_half_pi, vdup_n_f64(y1), deg_to_rad_coeff);

        // Unwrap sine to remove branch at the end, because we square result anyway
        float64x1 s1;
        {
            float64 abs_x = fabs(dlat);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s1 = v_sine_kernel(vdup_n_f64(x));
        }

        float64x1 s2;
        {
            float64 abs_x = fabs(dlon);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s2 = v_sine_kernel(vdup_n_f64(x));
        }

        float64 c1 = my_sine(lat0);
        float64 c2 = my_sine(lat1);

        float64x1 a = vfma_f64(vmul_f64(s1, s1), vdup_n_f64(c1 * c2), vmul_f64(s2, s2));
        float64x1 asin_arg = vsqrt_f64(a);
        float64 c; // = my_arcsine(asin_arg);
        {
            int need_transform = vget_lane_f64(asin_arg, 0) > one_over_sqrt_2;
            float64x1 X = need_transform ? vsqrt_f64(vsub_f64(vdup_n_f64(1), a)) : asin_arg;

            float64 Y = v_arcsine_kernel(X);

            c = need_transform ? half_pi - Y : Y;
        }

        average = vfma_f64(average, coeff, vdup_n_f64(c));
    }

    float64 result = vget_lane_f64(average, 0);
    return result;
}

float64 reptest_haversine(haversine_computation *hcomps, int32 n, float64 ref_average,
    float64 (*f)(haversine_computation *, int32),
    char const *name)
{
    float64 max_error = 0;

    g_tester.label = name;
    while (is_testing(30))
    {
        reptest_begin_time();
        float64 average = f(hcomps, n);
        reptest_end_time();
        reptest_count_bytes(sizeof(*hcomps) * n);

        float64 error = fabs(average - ref_average);
        if (max_error < error)
            max_error = error;
    }

    return max_error;
}

int main(int32 argc, char **argv)
{
    float64 max_error;
    int32 seed = 12345;
    if (argc > 1)
    {
        seed = atoi(argv[1]);
    }
    srand(seed);

    int32 n = 10;
    if (argc > 2)
    {
        n = atoi(argv[2]);
    }

    haversine_computation *comps = malloc(n * sizeof(*comps));
    float64 average = compute_haversine_buffer(comps, n);
    printf("average = %.20lf\n", average);

    g_tester.print_results = true;

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_A, "haversine A");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_B, "haversine B");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_C, "haversine C");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_D, "haversine D");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_E, "haversine E");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_F, "haversine F");
    printf("max_error = %+.20lf\n", max_error);

    max_error = reptest_haversine(comps, n, average, compute_haversine_buffer_average_G, "haversine G");
    printf("max_error = %+.20lf\n", max_error);

    return 0;
}


#include "profiler.c"
#include "repetition_tester.c"
#include "timing.c"
#include "os_specifics.c"
