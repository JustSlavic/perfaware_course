#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"

/*
    Compute sine through quadratic approximation on the domain [0, pi]:
                y = (4.0 / pi) * x - (4.0 / (pi*pi)) * x * x
    compare its accuracy on the domain [-pi, pi],
    compare its accuracy on the domain [0, pi],
    do domain reduction, compare resulting accuracy on the domain [-pi, pi]
    compute sine through quadratic approximation on the domain [0, pi/2]:
                y = (8(1 - sqrt(2))/pi^2) * x * x + ((2 - 4(1 - sqrt(2)))/pi) * x
    and compare its accuracy with domain reduction


    x86_64:

    stdlib vs stubs:
    sine_quadratic_approximation
        domain: [-3.141593, 3.141593]
        e(-3.14159265358979311600) = 8.00000000000000000000

    sine_quadratic_approximation
        domain: [0.000000, 3.141593]
        e(+2.67035375555131571090) = 0.05600950026045337093

    sine_reduced_half
        domain: [-3.141593, 3.141593]
        e(-0.47123889803848029167) = 0.05600950026045325991

    sine_reduced_quarter
        domain: [-3.141593, 3.141593]
        e(+0.31415926535896826488) = 0.02353134558444303082

Taylor of 19th power: e = +0.00000000000000033307
Taylor of 21th power: e = +0.00000000000000088818
Taylor of 23th power: e = +0.00000000000000310862
Taylor of 27th power: e = +0.00000000000000699441
Taylor of 25th power: e = +0.00000000000000821565
Taylor of 29th power: e = +0.00000000000003574918
Taylor of 17th power: e = +0.00000000000004374279
Taylor of 31th power: e = +0.00000000000020461410
Taylor of 15th power: e = +0.00000000000602318195
Taylor of 13th power: e = +0.00000000066278027511
Taylor of 11th power: e = +0.00000005625894905492
Taylor of 9th power: e = +0.00000354258428614251
Taylor of 7th power: e = +0.00015689860050127624
Taylor of 5th power: e = +0.00452485553481740688
Taylor of 3th power: e = +0.07516777071134905785
Taylor of 1th power: e = +0.57079632679489433755
*/

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

double sine_reduced_half(double x)
{
    double sign = (x < 0) ? -1.0 : 1.0;
    double result = sign * sine_quadratic_approximation(sign * x);
    return result;
}

double sine_reduced_quarter(double x)
{
    int pi_count = (int)(x / pi) - (x < 0 ? 1 : 0);
    int sign = pi_count % 2 ? -1 : 1;
    double arg = x - pi_count * pi;
    arg = (arg > half_pi) ? pi - arg : arg;

    /*
        sin(0) = 0
        sin(pi/4) = sqrt(2)/2
        sin(pi/2) = 1

        axx + bx + c = 0
        c = 0
        2 a pi^2 + 4 b pi = 8
          a pi^2 + 4 b pi = 8 sqrt(2)

        a = 8(1 - sqrt(2))/pi^2
        b = (2 - 4(1 - sqrt(2)))/pi
    */

    double a = -0.3357488673628103541807525733876701910953780492546723687387637750157263772;
    double b = 1.16401285994663079603486332852342371719130971694861545615220556622733027090;

    double result = sign * (a * arg * arg + b * arg);
    return result;
}

float64 sine_taylor(float64 x, int32 max_power)
{
    int pi_count = (int)(x / pi) - (x < 0 ? 1 : 0);
    int sign = pi_count % 2 ? -1 : 1;
    double arg = x - pi_count * pi;
    x = (arg > half_pi) ? pi - arg : arg;

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

    return sign * sum;
}


function_to_function_compare function_compares[] =
{
    {
        .name = "sine_quadratic_approximation",
        .reference_function = sin,
        .function = sine_quadratic_approximation,
        .domain = {
            .min = -pi,
            .max =  pi,
        },
        .step = 0.005*pi,
    },
    {
        .name = "sine_quadratic_approximation",
        .reference_function = sin,
        .function = sine_quadratic_approximation,
        .domain = {
            .min =   0,
            .max =  pi,
        },
        .step = 0.005*pi,
    },
    {
        .name = "sine_reduced_half",
        .reference_function = sin,
        .function = sine_reduced_half,
        .domain = {
            .min = -pi,
            .max = pi,
        },
        .step = 0.005*pi,
    },
    {
        .name = "sine_reduced_quarter",
        .reference_function = sin,
        .function = sine_reduced_quarter,
        .domain = {
            .min = -pi,
            .max = pi,
        },
        .step = 0.005*pi,
    },
};

typedef struct
{
    float64 x, e;
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
        if (e > max_error.e)
        {
            max_error.x = x;
            max_error.e = e;
        }

        if (csv)
            fprintf(csv, "%lf,%lf,%lf\n", x, y, e);
        x += comp->step;
    }

    return max_error;
}

compute_error_result compute_error_sine_taylor(math_domain domain, double step, int32 max_power)
{
    compute_error_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = sin(x);
        float64 y = sine_taylor(x, max_power);
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

typedef struct
{
    int power;
    double x, e;
} compute_taylor_result;

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
        printf("    %s\n"
               "        domain: [%lf, %lf]\n"
               "        e(%+.20lf) = %.20lf\n\n",
            function_compares[i].name,
            function_compares[i].domain.min, function_compares[i].domain.max,
            error.x, error.e);
#if 0
        fclose(error_file);
#endif
    }

    compute_taylor_result results[32] = {};
    for (int power = 1; power < ARRAY_COUNT(results); power += 2)
    {
        char buffer[64] = {};
        sprintf(buffer, "sine taylor %dth power", power);
        compute_error_result error = compute_error_sine_taylor(
            (math_domain){ .min = 0, .max = half_pi },
            0.005*pi,
            power);
        results[power] = (compute_taylor_result)
        {
            .power = power,
            .x = error.x,
            .e = error.e,
        };
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
        printf("Taylor of %dth power: e = %+.20lf\n", results[i].power, results[i].e);
    }

    return 0;
};
