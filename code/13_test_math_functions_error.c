#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "base.h"

#include "reference_sqrt.h"
#include "reference_sin.h"
#include "reference_cos.h"
#include "reference_asin.h"

typedef struct
{
    char const *name;
    math_function *function;
    math_reference_table *table;
} function_to_table_compare;

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

float64 sqrt_0(float64 x)
{
    float64 result = x;
    return result;
}

float64 sin_0(float64 x)
{
    float64 result = x;
    return result;
}

float64 cos_0(float64 x)
{
    float64 result = x;
    return result;
}

float64 asin_0(float64 x)
{
    float64 result = x;
    return result;
}

function_to_function_compare function_compares[] =
{
    {
        .name = "sqrt",
        .reference_function = sqrt,
        .function = sqrt_0,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
    {
        .name = "sin",
        .reference_function = sin,
        .function = sin_0,
        .domain = {
            .min = -M_PI,
            .max =  M_PI,
        },
        .step = 0.01,
    },
    {
        .name = "cos",
        .reference_function = cos,
        .function = cos_0,
        .domain = {
            .min = -M_PI,
            .max =  M_PI,
        },
        .step = 0.01,
    },
    {
        .name = "asin",
        .reference_function = asin,
        .function = asin_0,
        .domain = {
            .min = 0.0,
            .max = 1.0,
        },
        .step = 0.01,
    },
};

float64 compute_error_function(function_to_function_compare *comp)
{
    float64 max_error = 0.f;

    if (comp->domain.max < comp->domain.min)
        return max_error;

    float64 x = comp->domain.min;
    while (x < comp->domain.max)
    {
        float64 reference_y = comp->reference_function(x);
        float64 y = comp->function(x);
        float64 e = fabs(reference_y - y);
        if (e > max_error) max_error = e;
        x += comp->step;
    }
    return max_error;
}


int main()
{
    printf("Max errors from table:\n");
    for (int i = 0; i < ARRAY_COUNT(table_compares); i++)
    {
        function_to_table_compare comp = table_compares[i];
        float64 error = compute_error_table(comp.table, comp.function);
        printf("    %s %lf\n", comp.name, error);
    }
    printf("Max errors from reference functions:\n");
    for (int i = 0; i < ARRAY_COUNT(function_compares); i++)
    {
        float64 error = compute_error_function(function_compares + i);
        printf("    %s %lf\n", function_compares[i].name, error);
    }

    return 0;
};
