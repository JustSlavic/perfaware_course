#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float64x1_t double1;
typedef float64x2_t double2;
#endif

#include "base.h"

#include "math_reference_tables.c"

typedef struct
{
    char const *name;
    math_function *function;
    math_reference_table *table;
} function_to_table_compare;

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

typedef struct
{
    char const *name;
    math_function *reference_function;
    math_function *function;
    math_domain domain;
    float64 step;
} taylor_to_function_compare;


function_to_table_compare table_compares[] =
{
    {
        .name = "sqrt",
        .function = sqrt,
        .table = &math_reference_sqrt,
    },
    {
        .name = "sin",
        .function = sin,
        .table = &math_reference_sin,
    },
    {
        .name = "cos",
        .function = cos,
        .table = &math_reference_cos,
    },
    {
        .name = "asin",
        .function = asin,
        .table = &math_reference_asin,
    },
};

float64 compute_error_table(math_reference_table *table, math_function *f)
{
    float64 max_error = 0.f;
    for (int i = 0; i < table->count; i++)
    {
        math_reference_pair p = table->table[i];

        float64 y = f(p.x);
        float64 e = fabs(p.y - y);
        if (e > max_error) max_error = e;
    }
    return max_error;
}

#if OS_MAC
float64 sqrt_neon_x1(float64 x)
{
    double1 xmm0 = vdup_n_f64(x);
    double1 xmm1 = vsqrt_f64(xmm0);
    float64 result = vget_lane_f64(xmm1, 0);
    return result;
}

float64 sqrt_neon_x2(float64 x)
{
    double2 xmm0 = vdupq_n_f64(x);
    double2 xmm1 = vsqrtq_f64(xmm0);
    float64 result = vgetq_lane_f64(xmm1, 0);
    return result;
}
#endif // OS_MAC

float64 sine_quadratic_approximation(float64 x)
{
    float64 result = (4.0 / pi) * x - (4.0 / (pi*pi)) * x * x;
    return result;
}

float64 sine_taylor_2_terms(float64 x)
{
    float64 result = x - x*x*x/6.0;
    return result;
}

float64 sine_taylor_3_terms(float64 x)
{
    float64 result = x - x*x*x/6.0 + x*x*x*x*x/120.0;
    return result;
}

float64 sine_taylor_4_terms(float64 x)
{
    float64 result = x - x*x*x/6.0 + x*x*x*x*x/120.0 - x*x*x*x*x*x*x/5040.0;
    return result;
}

float64 cos_0(float64 x)
{
    return 0.0;
}

float64 asin_0(float64 x)
{
    return 0.0;
}


function_to_function_compare function_compares[] =
{
#if OS_MAC
    {
        .name = "sqrt_neon_x1",
        .reference_function = sqrt,
        .function = sqrt_neon_x1,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sqrt_neon_x2",
        .reference_function = sqrt,
        .function = sqrt_neon_x2,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
#endif // OS_MAC
    {
        .name = "sine_quadratic_approximation",
        .reference_function = sin,
        .function = sine_quadratic_approximation,
        .domain = {
            .min = -pi,
            .max =  pi,
        },
        .step = 0.01,
    },
    {
        .name = "sine_taylor_2_terms",
        .reference_function = sin,
        .function = sine_taylor_2_terms,
        .domain = {
            .min = -pi,
            .max = pi,
        },
        .step = 0.01,
    },
    {
        .name = "sine_taylor_3_terms",
        .reference_function = sin,
        .function = sine_taylor_3_terms,
        .domain = {
            .min = -pi,
            .max = pi,
        },
        .step = 0.01,
    },
    {
        .name = "sine_taylor_4_terms",
        .reference_function = sin,
        .function = sine_taylor_4_terms,
        .domain = {
            .min = -pi,
            .max = pi,
        },
        .step = 0.01,
    },
    {
        .name = "cos_0",
        .reference_function = cos,
        .function = cos_0,
        .domain = {
            .min = -pi,
            .max =  pi,
        },
        .step = 0.01,
    },
    {
        .name = "asin_0",
        .reference_function = asin,
        .function = asin_0,
        .domain = {
            .min = 0.0,
            .max = pi,
        },
        .step = 0.01,
    },
};

typedef struct
{
    float64 x, y;
} compute_error_result;

compute_error_result compute_error_function(function_to_function_compare *comp)
{
    compute_error_result max_error = {};

    if (comp->domain.max < comp->domain.min)
        return max_error;

    char error_filename[256] = {};
    sprintf(error_filename, "%s_error_graph.txt", comp->name);
    FILE *error_file = fopen(error_filename, "wb+");
    if (!error_file)
        return max_error;

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

        fprintf(error_file, "%10lf %10lf %10lf\n", x, y, e);
        x += comp->step;
    }

    fclose(error_file);
    return max_error;
}


int main()
{
    printf("stdlib vs table:\n");
    for (int i = 0; i < ARRAY_COUNT(table_compares); i++)
    {
        function_to_table_compare comp = table_compares[i];
        float64 error = compute_error_table(comp.table, comp.function);
        printf("    %10s %.20lf\n", comp.name, error);
    }
    printf("stdlib vs stubs:\n");
    for (int i = 0; i < ARRAY_COUNT(function_compares); i++)
    {
        compute_error_result error = compute_error_function(function_compares + i);
        printf("    %s%.*sf(%+lf) = %.20lf\n", function_compares[i].name, (int)(30 - strlen(function_compares[i].name)), spaces, error.x, error.y);
    }

    return 0;
};
