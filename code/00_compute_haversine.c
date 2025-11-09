#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "timing.h"
#include "json.h"
#include "profiler.h"

#include "00_reference_haversine.c"

#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


typedef struct
{
    float64 x0, y0;
    float64 x1, y1;
} haversine_pair;


int main(int32 argc, char **argv)
{
    PROFILE_BEGIN();

    char const *filename = "data.json";
    FILE *input_file = fopen(filename, "r");
    if (input_file)
    {
        fseek(input_file, 0, SEEK_END);
        long input_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        void *data = malloc(input_size + 1);
        PROFILE_BLOCK_BANDWIDTH(fread, input_size)
        {
            fread(data, input_size, 1, input_file);
        }
        *((char*)data + input_size) = 0;
        fclose(input_file);

        json *object = json__parse(data, input_size + 1);

        json *pairs_ = json__object_lookup(object, "pairs");
        int32 n = json__get_length(pairs_);

        haversine_pair *pairs = malloc(sizeof(haversine_pair) * n);

        for (int i = 0; i < n; i++)
        {
            json *coords = json__list_lookup(pairs_, i);
            json *x0_ = json__object_lookup(coords, "x0");
            json *y0_ = json__object_lookup(coords, "y0");
            json *x1_ = json__object_lookup(coords, "x1");
            json *y1_ = json__object_lookup(coords, "y1");

            float64 x0 = json__get_float(x0_);
            float64 y0 = json__get_float(y0_);
            float64 x1 = json__get_float(x1_);
            float64 y1 = json__get_float(y1_);

            pairs[i].x0 = x0;
            pairs[i].y0 = y0;
            pairs[i].x1 = x1;
            pairs[i].y1 = y1;
        }

        float64 one_over_n = 1.0 / n;
        float64 average = 0;
        PROFILE_BLOCK(haversine)
        {
            for (int i = 0; i < n; i++)
            {
                float64 d = reference_haversine(pairs[i].x0, pairs[i].y0, pairs[i].x1, pairs[i].y1, 6372.8);
                average += d * one_over_n;
            }
        }

        if (object)
        {
            json__free(object);
        }

        printf("Average distance = %lf\n", average);
    }
    else
    {
        printf("Error: Could not open file '%s' - make sure you run '00_generate_haversine_data' first.\n", filename);
        exit(1);
    }

    PROFILE_END();

    return 0;
}

#include "json.c"
#include "profiler.c"
#include "timing.c"
