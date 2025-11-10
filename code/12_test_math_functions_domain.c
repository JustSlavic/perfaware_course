#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "timing.h"
#include "json.h"
#include "profiler.h"

#include "00_reference_haversine.c"


#if OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

/*
    Replace math functions with functions that count their domain.

Average distance = 10011.641728
Domains:
         sqrt(x) x \in [0.000000, 1.000000]
         sin(x)  x \in [-3.137182, 3.136128]
         cos(x)  x \in [-1.570796, 1.570796]
         asin(x) x \in [0.000114, 1.000000]
*/


typedef struct
{
    float64 x0, y0;
    float64 x1, y1;
} haversine_pair;

typedef struct
{
    float64 min, max;
} math_domain;

void init_domain(math_domain *domain)
{
    domain->min = HUGE_VAL;
    domain->max = -HUGE_VAL;
}

void expand_domain(math_domain *domain, float64 x)
{
    if (x < domain->min) domain->min = x;
    if (x > domain->max) domain->max = x;
}

static math_domain math__sqrt_domain;
static math_domain math__sin_domain;
static math_domain math__cos_domain;
static math_domain math__asin_domain;

float64 math__sqrt(float64 x)
{
    expand_domain(&math__sqrt_domain, x);
    return sqrt(x);
}

float64 math__sin(float64 x)
{
    expand_domain(&math__sin_domain, x);
    return sin(x);
}

float64 math__cos(float64 x)
{
    expand_domain(&math__cos_domain, x);
    return cos(x);
}

float64 math__asin(float64 x)
{
    expand_domain(&math__asin_domain, x);
    return asin(x);
}


float64 domain_haversine(float64 x0, float64 y0, float64 x1, float64 y1, float64 radius)
{
    PROFILE_FUNCTION_BEGIN();
    float64 dlon = deg_to_rad(x1 - x0);
    float64 dlat = deg_to_rad(y1 - y0);

    float64 lat0 = deg_to_rad(y0);
    float64 lat1 = deg_to_rad(y1);

    float64 a = square(math__sin(dlat/2.0)) + math__cos(lat0)*math__cos(lat1)*square(math__sin(dlon/2.0));
    float64 c = 2.0*math__asin(math__sqrt(a));

    float64 result = radius * c;
    PROFILE_FUNCTION_END();
    return result;
}


int main(int32 argc, char **argv)
{
    FILE *input_file = fopen("data.json", "r");
    if (!input_file)
    {
        printf("Could not open the file \"%s\"\n", "data.json");
    }
    else
    {
        init_domain(&math__sqrt_domain);
        init_domain(&math__sin_domain);
        init_domain(&math__cos_domain);
        init_domain(&math__asin_domain);

        fseek(input_file, 0, SEEK_END);
        long input_size = ftell(input_file);
        fseek(input_file, 0, SEEK_SET);

        void *data = malloc(input_size + 1);
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
        {
            for (int i = 0; i < n; i++)
            {
                float64 d = domain_haversine(pairs[i].x0, pairs[i].y0, pairs[i].x1, pairs[i].y1, 6372.8);
                float64 r = reference_haversine(pairs[i].x0, pairs[i].y0, pairs[i].x1, pairs[i].y1, 6372.8);
                if (d != r)
                {
                    printf("Error! Domain interfered with the haversine computation!\n");
                    exit(1);
                }
                average += d * one_over_n;
            }
        }

        if (object)
        {
            json__free(object);
        }

        printf("Average distance = %lf\n", average);
        printf("Domains: \n");
        printf("         sqrt(x) x \\in [%lf, %lf]\n", math__sqrt_domain.min, math__sqrt_domain.max);
        printf("         sin(x)  x \\in [%lf, %lf]\n", math__sin_domain.min, math__sin_domain.max);
        printf("         cos(x)  x \\in [%lf, %lf]\n", math__cos_domain.min, math__cos_domain.max);
        printf("         asin(x) x \\in [%lf, %lf]\n", math__asin_domain.min, math__asin_domain.max);
    }

    return 0;
}

#include "json.c"
#include "profiler.c"
#include "timing.c"
