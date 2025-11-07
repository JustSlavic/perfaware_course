#include <stdio.h>
#include <stdlib.h>
#include "timing.h"
#include "profiler.h"

#include "00_reference_haversine.c"


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

    FILE *file_json = fopen("data.json", "w+");
    if (file_json)
    {
        FILE *file_answers = fopen("answers.bin", "wb+");
        if (file_answers)
        {
            float64 one_over_n = 1.0 / n;
            float64 average = 0;

            fprintf(file_json, "{\n  \"pairs\": [\n");
            for (int i = 0; i < n; i++)
            {
                float64 x0 = uniform_float64(-180.0, 180.0);
                float64 y0 = uniform_float64(-90.0, 90.0);
                float64 x1 = uniform_float64(-180.0, 180.0);
                float64 y1 = uniform_float64(-90.0, 90.0);

                fprintf(file_json, "    { \"x0\": %21.16lf, \"y0\": %21.16lf, \"x1\": %21.16lf, \"y1\": %21.16lf },\n", x0, y0, x1, y1);

                float64 answer = reference_haversine(x0, y0, x1, y1, 6372.8);
                average += answer * one_over_n;
                fwrite(&answer, sizeof(answer), 1, file_answers);
            }
            fprintf(file_json, "  ]\n}\n");

            printf("Average distance = %lf\n", average);

            fclose(file_answers);
        }
        else
        {
            printf("Cannot create file 'answers.bin'\n");
        }

        fclose(file_json);
    }
    else
    {
        printf("Cannot create file 'data.json'\n");
    }

    return 0;
}


#include "profiler.c"
#include "timing.c"
