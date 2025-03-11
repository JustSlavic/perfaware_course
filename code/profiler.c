#include "profiler.h"


profiler g_profiler = {};
static uint32 g_profiler_parent_index = 0;


profile_point profile_begin(char const *label, code_location cl, uint32 index)
{
    profile_point result;
    result.cl = cl;
    result.label = label;
    result.index = index;
    result.parent_index = g_profiler_parent_index;

    profile_measurement *measurement = g_profiler.measurements + index;
    result.old_elapsed_inclusive = measurement->elapsed_inclusive;

    g_profiler_parent_index = index;
    result.t0 = get_time();
    return result;
}

profile_point profile_begin_bandwidth(char const *label, code_location cl, uint32 index, uint64 byte_count)
{
    profile_point result;
    result.cl = cl;
    result.label = label;
    result.index = index;
    result.parent_index = g_profiler_parent_index;

    profile_measurement *measurement = g_profiler.measurements + index;
    result.old_elapsed_inclusive = measurement->elapsed_inclusive;
    measurement->byte_count += byte_count;

    g_profiler_parent_index = index;
    result.t0 = get_time();
    return result;
}

void profile_end(profile_point p)
{
    uint64 elapsed = get_time() - p.t0;

    profile_measurement *measurement = g_profiler.measurements + p.index;
    measurement->elapsed_exclusive += elapsed;
    measurement->elapsed_inclusive = p.old_elapsed_inclusive + elapsed;
    measurement->hit_count += 1;
    measurement->label = p.label;
    measurement->cl = p.cl;

    profile_measurement *parent_measurement = g_profiler.measurements + p.parent_index;
    parent_measurement->elapsed_exclusive -= elapsed;

    g_profiler_parent_index = p.parent_index;
}

int clamp_left(int v, int min)
{
    if (v < min) return min;
    return v;
}

void print_profile_result()
{
    uint64 time_sum = g_profiler.t1 - g_profiler.t0;

    printf("+---------------------------------------------------------------------------------------------------------------+\n");
    int m = printf("| Time elapsed: %lldus", time_sum / 1000);
    printf("%.*s|\n", clamp_left(112 - m, 0), spaces);
    printf("+---------------------------------------------------------------------------------------------------------------+\n");
    printf("| Label               Hits   Microseconds   Persentage   %% w/ children   Bandwidth             File:Line        |\n");
    printf("+---------------------------------------------------------------------------------------------------------------+\n");
    for (int i__ = 1; i__ < ARRAY_COUNT(g_profiler.measurements); i__++) {
        profile_measurement measurement = g_profiler.measurements[i__];
        if (measurement.label != NULL) {
            int n = printf("| %s", measurement.label);
            n += printf("%.*s%d", clamp_left(22 - n, 0), spaces, measurement.hit_count);
            n += printf("%.*s%lldus", clamp_left(29 - n, 0), spaces, measurement.elapsed_exclusive / 1000);
            n += printf("%.*s%.4f%%", clamp_left(44 - n, 0), spaces, 100.f * measurement.elapsed_exclusive / time_sum);
            if (measurement.elapsed_exclusive != measurement.elapsed_inclusive)
                n += printf("%.*s%.4f%%", clamp_left(57 - n, 0), spaces, 100.f * measurement.elapsed_inclusive / time_sum);
            // n += printf("%.*s%s", clamp_left(57 - n, 0), spaces, measurement.cl.function);
            if (measurement.byte_count)
            {
                float64 seconds = (float64) measurement.elapsed_inclusive / 1000000000.0;
                float64 bytes_per_second = (float64) measurement.byte_count / seconds;
                float64 megabytes = (float64) measurement.byte_count / (float64) MEGABYTES(1);
                float64 gigabytes_per_second = bytes_per_second / (float64) GIGABYTES(1);

                n += printf("%.*s%.3lfmb at %.2lfgb/s", clamp_left(73 - n, 0), spaces, megabytes, gigabytes_per_second);
            }
            n += printf("%.*s%s:%d\n", clamp_left(95 - n, 0), spaces, measurement.cl.filename, measurement.cl.line);
        }
        else
        {
            break;
        }
    }
    printf("+---------------------------------------------------------------------------------------------------------------+\n");
}
