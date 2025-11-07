#include <stdio.h>
#include <stdlib.h>
#include "timing.h"
#include "profiler.h"

#include "00_reference_haversine.c"
#include "21_my_haversine.c"


float64 uniform_float64(float64 a, float64 b)
{
    float64 t = (float64) rand() / (float64) RAND_MAX;
    float64 result = (1.0 - t) * a + t * b;
    return result;
}

int main(int32 argc, char **argv)
{
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

    {
        float64 one_over_n = 1.0 / n;
        float64 ref_average = 0;
        float64 my_average = 0;
        float64 max_error = 0;

        for (int i = 0; i < n; i++)
        {
            float64 x0 = uniform_float64(-180.0, 180.0);
            float64 y0 = uniform_float64(-90.0, 90.0);
            float64 x1 = uniform_float64(-180.0, 180.0);
            float64 y1 = uniform_float64(-90.0, 90.0);

            float64 reference = reference_haversine(x0, y0, x1, y1, 6372.8);
            float64 my_answer = my_haversine(x0, y0, x1, y1, 6372.8);
            float64 error = fabs(my_answer - reference);

            ref_average += reference * one_over_n;
            my_average += my_answer * one_over_n;
            if (error > max_error)
            {
                max_error = error;
            }
        }
        printf("Reference Average = %.20lf\n", ref_average);
        printf("My Average = %.20lf\n", my_average);
        printf("Max Error = %+.20lf\n", max_error);
        printf("Answer Error = %+.20lf\n", my_average - ref_average);
    }

    return 0;
}


#include "profiler.c"
#include "timing.c"
