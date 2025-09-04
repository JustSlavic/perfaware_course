#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

function_to_function_compare function_compares[] =
{
    {
        .name = "sine_quadratic_approximation",
        .reference_function = sin,
        .function = sine_quadratic_approximation,
        .domain = {
            .min =   0,
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
    sprintf(error_filename, "%s_error_graph.csv", comp->name);
    FILE *error_file = fopen(error_filename, "wb+");
    if (!error_file)
        return max_error;

    fprintf(error_file, "x,y,e\n");

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

        fprintf(error_file, "%lf,%lf,%lf\n", x, y, e);
        x += comp->step;
    }

    fclose(error_file);
    return max_error;
}


int main()
{
    printf("stdlib vs stubs:\n");
    for (int i = 0; i < ARRAY_COUNT(function_compares); i++)
    {
        compute_error_result error = compute_error_function(function_compares + i);
        printf("    %s%.*sf(%+lf) = %.20lf\n", function_compares[i].name, (int)(30 - strlen(function_compares[i].name)), spaces, error.x, error.y);
    }

    return 0;
};
