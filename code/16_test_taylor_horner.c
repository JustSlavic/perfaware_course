#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "base.h"
#include "math_reference_tables.c"

/*
    Compare accuracy of Taylor series computed naively and by Horner's

    x86_64:
Horner of 19th power: e = +0.000000000000000333066907387547
Taylor of 19th power: e = +0.000000000000000333066907387547
Taylor of 21th power: e = +0.000000000000000888178419700125
Taylor of 23th power: e = +0.000000000000003108624468950438
Horner of 21th power: e = +0.000000000000003330669073875470
Horner of 25th power: e = +0.000000000000003996802888650564
Taylor of 27th power: e = +0.000000000000006994405055138486
Horner of 23th power: e = +0.000000000000007327471962526033
Taylor of 25th power: e = +0.000000000000008215650382226158
Horner of 29th power: e = +0.000000000000028754776337791554
Taylor of 29th power: e = +0.000000000000035749181392930041
Horner of 27th power: e = +0.000000000000039968028886505635
Horner of 17th power: e = +0.000000000000043520742565306136
Taylor of 17th power: e = +0.000000000000043742787170231168
Taylor of 31th power: e = +0.000000000000204614103438416350
Horner of 31th power: e = +0.000000000000269229083471600461
Taylor of 15th power: e = +0.000000000006023181953196399263
Horner of 15th power: e = +0.000000000006023403997801324294
Horner of 13th power: e = +0.000000000662780053062306251377
Taylor of 13th power: e = +0.000000000662780275106911176408
Horner of 11th power: e = +0.000000056258948943899156347470
Taylor of 11th power: e = +0.000000056258949054921458809986
Horner of 9th power: e = +0.000003542584286142513860795589
Taylor of 9th power: e = +0.000003542584286142513860795589
Horner of 7th power: e = +0.000156898600501276241914183629
Taylor of 7th power: e = +0.000156898600501276241914183629
Horner of 5th power: e = +0.004524855534817406876868517429
Taylor of 5th power: e = +0.004524855534817406876868517429
Horner of 3th power: e = +0.075167770711348946832686124253
Taylor of 3th power: e = +0.075167770711349057854988586769
Horner of 1th power: e = +0.570796326794894337552932483959
Taylor of 1th power: e = +0.570796326794894337552932483959
*/

enum
{
    SineTaylor = 0,
    SineHorner = 1,
};

typedef struct
{
    float64 min, max;
} math_domain;

typedef struct
{
    int is_horner;
    int power;
    double x, e;
} compute_taylor_result;


float64 sine_taylor(float64 x, int32 max_power)
{
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

    return sum;
}

uint64 compute_factorial(uint64 x)
{
    uint64 result = 1;
    for (uint64 i = 2; i <= x; i++)
        result *= i;
    return result;
}

int64 compute_taylor_term_denominator(uint64 power)
{
    if (power % 2 == 0) return -999;

    int sign = ((power/2) % 2) ? -1 : 1;
    return sign * compute_factorial(power);
}

float64 sine_taylor_horner(float64 x, int32 max_power)
{
    double x2 = x * x;
    double a = 0;
    for (int power = max_power; power > 0; power -= 2)
    {
        double b = 1.0 / compute_taylor_term_denominator(power);
        a = a*x2 + b;
    }
    double result = a * x;
    return result;
}

compute_taylor_result compute_error_sine_taylor(
    math_domain domain,
    double step,
    int32 max_power,
    float64 (*f)(float64, int32))
{
    compute_taylor_result max_error = {};

    if (domain.max < domain.min)
        return max_error;

    float64 x = domain.min;
    while (x < domain.max)
    {
        float64 reference_y = sin(x);
        float64 y = f(x, max_power);
        float64 e = fabs(reference_y - y);
        if (e > max_error.e)
        {
            max_error.power = max_power;
            max_error.x = x;
            max_error.e = e;
        }
        x += step;
    }

    return max_error;
}

int main()
{
    compute_taylor_result results[128] = {};
    for (int power = 1; power < 32; power += 2)
    {
        char buffer[64] = {};
        sprintf(buffer, "sine taylor %dth power", power);
        compute_taylor_result error = compute_error_sine_taylor(
            (math_domain){ .min = 0, .max = half_pi },
            0.005*pi,
            power,
            sine_taylor);
        error.is_horner = SineTaylor;
        results[power] = error;
    }
    for (int power = 1; power < 32; power += 2)
    {
        char buffer[64] = {};
        sprintf(buffer, "sine horner %dth power", power);
        compute_taylor_result error = compute_error_sine_taylor(
            (math_domain){ .min = 0, .max = half_pi },
            0.005*pi,
            power,
            sine_taylor_horner);
        error.is_horner = SineHorner;
        results[power + 1] = error;
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
        printf("%s of %dth power: e = %+.30lf\n",
            results[i].is_horner ? "Horner" : "Taylor",
            results[i].power, results[i].e);
    }

    return 0;
};
