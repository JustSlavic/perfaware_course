#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
#endif


float64 my_sqrt(float64 x)
{
    float64x1 xmm0 = vdup_n_f64(x);
    float64x1 xmm1 = vsqrt_f64(xmm0);
    float64 result = vget_lane_f64(xmm1, 0);
    return result;
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

float64 my_arcsine(float64 arg)
{
    int need_transform = arg > one_over_sqrt_2;
    float64 x = need_transform ? my_sqrt(1 - arg*arg) : arg;

    float64x2 xx = vdupq_n_f64(x * x);
    float64x2 a = vdupq_n_f64( 0x1.dfc53682725cap-1);
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
