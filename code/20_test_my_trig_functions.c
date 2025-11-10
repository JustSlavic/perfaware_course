#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "20_my_trig_functions.c"

/*
    Test accuracy of all my new math functions

    x86_64
my_sqrt               e = +0.000000000000000000000000000000
my_sine               e = +0.000000000000000222044604925031
my_cosine             e = +0.000000000000000444089209850063
my_arcsine            e = +0.000000000000000444089209850063
*/

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    double x, e;
    char const *name;
} compute_result;


compute_result compute_error(
    math_domain domain,
    double step,
    float64 (*reference_function)(float64),
    float64 (*tested_function)(float64))
{
    compute_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = reference_function(x);
        float64 y = tested_function(x);
        float64 e = fabs(reference_y - y);
        if (e > max_error.e)
        {
            max_error.x = x;
            max_error.e = e;
        }
        x += step;
    }

    return max_error;
}

int main()
{
    compute_result results[128] = {};
    int32 result_count = 0;

    {
        compute_result error = compute_error(
            (math_domain){ .min = 0, .max = 1 },
            0.001,
            sqrt,
            my_sqrt);
        error.name = "my_sqrt";
        results[result_count++] = error;
    }
    {
        compute_result error = compute_error(
            (math_domain){ .min = -pi, .max = pi },
            0.001*pi,
            sin,
            my_sine);
        error.name = "my_sine";
        results[result_count++] = error;
    }
    {
        compute_result error = compute_error(
            (math_domain){ .min = -pi, .max = pi },
            0.001*pi,
            cos,
            my_cosine);
        error.name = "my_cosine";
        results[result_count++] = error;
    }
    {
        compute_result error = compute_error(
            (math_domain){ .min = 0, .max = 1 },
            0.001,
            asin,
            my_arcsine);
        error.name = "my_arcsine";
        results[result_count++] = error;
    }


    // Sort
    for (int i = 0; i < result_count; i++)
    {
        for (int j = i + 1; j < result_count; j++)
        {
            if (results[j].e < results[i].e)
            {
                compute_result tmp = results[j];
                results[j] = results[i];
                results[i] = tmp;
            }
        }
    }

    for (int i = 0; i < result_count; i++)
    {
        int n = printf("%s", results[i].name);
        printf("%.*se = %+.30lf\n",
            22 - n, spaces,
            results[i].e);
    }

    return 0;
};
