#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#if OS_MAC
// Neon
#include <arm_neon.h>
typedef float32x2_t float32x2;
typedef float32x4_t float32x4;
typedef float64x1_t float64x1;
typedef float64x2_t float64x2;
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

double sqrt_0(double x) { return 0.0; }
double sin_0(double x) { return 0.0; }
double cos_0(double x) { return 0.0; }
double asin_0(double x) { return 0.0; }

function_to_function_compare function_compares[] =
{
    {
        .name = "sqrt_0",
        .reference_function = cos,
        .function = cos_0,
        .domain = {
            .min = -pi,
            .max =  pi,
        },
        .step = 0.01,
    },
    {
        .name = "sin_0",
        .reference_function = sin,
        .function = cos_0,
        .domain = {
            .min = -pi,
            .max =  pi,
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
