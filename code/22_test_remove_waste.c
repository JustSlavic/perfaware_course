#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
    x86_64
+----------------------+-----+-------------------+-------------------+----------------------+
| Label                |     | Time              | Bytes             | Page faults          |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine A          | Min | 0 us              | 0.987084 gb/s     |                      |
|                      | Max | 18 us             | 0.020234 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine B          | Min | 0 us              | 1.318913 gb/s     |                      |
|                      | Max | 11 us             | 0.033881 gb/s     | 1 (0.4000 kB/f)      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine C          | Min | 0 us              | 1.035449 gb/s     |                      |
|                      | Max | 20 us             | 0.018426 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine D          | Min | 0 us              | 1.135327 gb/s     |                      |
|                      | Max | 1 us              | 0.250022 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine E          | Min | 0 us              | 1.233200 gb/s     |                      |
|                      | Max | 2 us              | 0.159327 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine F          | Min | 0 us              | 1.379575 gb/s     |                      |
|                      | Max | 3 us              | 0.124978 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
+----------------------+-----+-------------------+-------------------+----------------------+
| haversine G          | Min | 0 us              | 1.774493 gb/s     |                      |
|                      | Max | 2 us              | 0.161514 gb/s     |                      |
|                      | Avg | 1 us              |                   |                      |
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

#include "timing.h"
#include "profiler.h"
#include "repetition_tester.h"
#include "00_reference_haversine.c"


#define EARTH_RADIUS 6372.8

float64 my_sqrt(float64 x)
{
    double2 xmm0 = v_splat(x);
    double2 xmm1 = v_sqrt(xmm0);
    float64 result = v_get_x(xmm1);
    return result;
}

double2 v_sine_kernel(double2 x)
{
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
    return a;
}

float64 my_sine(float64 arg)
{
    float64 abs_x = fabs(arg);
    float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;

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
    return (arg < 0) ? -result : result;
}

float64 my_cosine(float64 x)
{
    float64 result = my_sine(x + half_pi);
    return result;
}

double2 v_arcsine_kernel(double2 x)
{
    double2 xx = v_mul(x, x);
    double2 a = v_splat(0x1.dfc53682725cap-1);
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
    a = v_mul(a, x);
    return a;
}

float64 my_arcsine(float64 arg)
{
    int need_transform = arg > one_over_sqrt_2;
    float64 x = need_transform ? my_sqrt(1 - arg*arg) : arg;

    double2 xx = v_splat(x * x);
    double2 a = v_splat(0x1.dfc53682725cap-1);
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
    double2 average = v_splat(0);
    double2 coeff = v_splat(EARTH_RADIUS * one_over_n);

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

        average = v_fma(average, coeff, v_splat(c));
    }

    float64 result = v_get_x(average);
    return result;
}

float64 compute_haversine_buffer_average_E(haversine_computation *hcomps, int32 n)
{
    // Do fmadd for lat0 lat1
    float64 one_over_n = 1.0 / n;
    double2 average = v_splat(0);
    double2 coeff = v_splat(EARTH_RADIUS * one_over_n);
    double2 deg_to_rad_coeff = v_splat(0.01745329251994329577);
    double2 v_half_pi = v_splat(half_pi);

    for (int i = 0; i < n; i++)
    {
        float64 x0 = hcomps[i].x0;
        float64 y0 = hcomps[i].y0;
        float64 x1 = hcomps[i].x1;
        float64 y1 = hcomps[i].y1;

        float64 dlon = (x1 - x0) * 0.008726646259971647884618453842443;
        float64 dlat = (y1 - y0) * 0.008726646259971647884618453842443;

        double2 lat0 = v_fma(v_half_pi, v_splat(y0), deg_to_rad_coeff);
        double2 lat1 = v_fma(v_half_pi, v_splat(y1), deg_to_rad_coeff);

        float64 s1 = my_sine(dlat);
        float64 s2 = my_sine(dlon);
        float64 c1 = my_sine(v_get_x(lat0));
        float64 c2 = my_sine(v_get_x(lat1));

        float64 a = square(s1) + c1 * c2 * square(s2);
        float64 sq1 = my_sqrt(a);
        float64 c = 2.0*my_arcsine(sq1);

        average = v_fma(average, coeff, v_splat(c));
    }

    float64 result = v_get_x(average);
    return result;
}

float64 compute_haversine_buffer_average_F(haversine_computation *hcomps, int32 n)
{
    // Unwrap sine
    double2 average = v_splat(0);
    double2 coeff = v_splat(2.0 * EARTH_RADIUS / n);
    double2 deg_to_rad_coeff = v_splat(0.01745329251994329577);
    double2 v_half_pi = v_splat(half_pi);

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
        double2 lat0 = v_fma(v_half_pi, v_splat(y0), deg_to_rad_coeff);
        double2 lat1 = v_fma(v_half_pi, v_splat(y1), deg_to_rad_coeff);

        // Unwrap sine to remove branch at the end, because we square result anyway
        double2 s1;
        {
            float64 abs_x = fabs(dlat);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s1 = v_sine_kernel(v_splat(x));
        }

        double2 s2;
        {
            float64 abs_x = fabs(dlon);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s2 = v_sine_kernel(v_splat(x));
        }

        float64 c1 = my_sine(v_get_x(lat0));
        float64 c2 = my_sine(v_get_x(lat1));

        double2 a = v_fma(v_mul(s1, s1), v_splat(c1 * c2), v_mul(s2, s2));
        float64 sq1 = my_sqrt(v_get_x(a));
        float64 c = my_arcsine(sq1);

        average = v_fma(average, coeff, v_splat(c));
    }

    float64 result = v_get_x(average);
    return result;
}

float64 compute_haversine_buffer_average_G(haversine_computation *hcomps, int32 n)
{
    // Unwrap sine
    double2 average = v_splat(0);
    double2 coeff = v_splat(2.0 * EARTH_RADIUS / n);
    double2 deg_to_rad_coeff = v_splat(0.01745329251994329577);
    double2 v_half_pi = v_splat(half_pi);

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
        float64 lat0 = v_get_x(v_fma(v_half_pi, v_splat(y0), deg_to_rad_coeff));
        float64 lat1 = v_get_x(v_fma(v_half_pi, v_splat(y1), deg_to_rad_coeff));

        // Unwrap sine to remove branch at the end, because we square result anyway
        double2 s1;
        {
            float64 abs_x = fabs(dlat);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s1 = v_sine_kernel(v_splat(x));
        }

        double2 s2;
        {
            float64 abs_x = fabs(dlon);
            float64 x = (abs_x > half_pi) ? (pi - abs_x) : abs_x;
            s2 = v_sine_kernel(v_splat(x));
        }

        float64 c1 = my_sine(lat0);
        float64 c2 = my_sine(lat1);

        double2 a = v_fma(v_mul(s1, s1), v_splat(c1 * c2), v_mul(s2, s2));
        double2 asin_arg = v_sqrt(a);
        float64 c; // = my_arcsine(asin_arg);
        {
            int need_transform = v_get_x(asin_arg) > one_over_sqrt_2;
            double2 X = need_transform ? v_sqrt(v_sub(v_splat(1), a)) : asin_arg;

            float64 Y = v_get_x(v_arcsine_kernel(X));

            c = need_transform ? half_pi - Y : Y;
        }

        average = v_fma(average, coeff, v_splat(c));
    }

    float64 result = v_get_x(average);
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
