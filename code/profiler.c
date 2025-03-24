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

#ifndef max
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif

void print_profile_result()
{
    uint64 timer_freq = get_frequency();
    uint64 time_sum = g_profiler.t1 - g_profiler.t0;

    printf("+------------------------------------------------------------------------------------------------------------------+\n");
    int m = printf("| Time elapsed: %.2lfus", 1e6 * time_sum / timer_freq);
    printf("%.*s|\n", max(115 - m, 0), spaces);
    printf("+------------------------------------------------------------------------------------------------------------------+\n");
    printf("| Label               Hits      Microseconds   Persentage   %% w/ children   Bandwidth             File:Line        |\n");
    printf("+------------------------------------------------------------------------------------------------------------------+\n");
    for (int i__ = 1; i__ < ARRAY_COUNT(g_profiler.measurements); i__++) {
        profile_measurement measurement = g_profiler.measurements[i__];
        if (measurement.label != NULL) {
            int n = 0;
            int w = 0;

            print_column(21, "| %s", measurement.label);
            print_column(10, " %d", measurement.hit_count);
            print_column(15, " %.2lf us", 1e6 * measurement.elapsed_exclusive / timer_freq);
            print_column(13, " %.4f%%", 100.f * measurement.elapsed_exclusive / time_sum);
            print_column_optional(measurement.elapsed_exclusive != measurement.elapsed_inclusive,
                17, " %.4f%%", 100.f * measurement.elapsed_inclusive / time_sum);

            float64 seconds = (float64) measurement.elapsed_inclusive / timer_freq;
            float64 bytes_per_second = (float64) measurement.byte_count / seconds;
            float64 megabytes = (float64) measurement.byte_count / (float64) MEGABYTES(1);
            float64 gigabytes_per_second = bytes_per_second / (float64) GIGABYTES(1);

            print_column_optional(measurement.byte_count,
                22, "%.3lfmb at %.2lfgb/s", megabytes, gigabytes_per_second);
            print_column(30, "%s:%d\n", measurement.cl.filename, measurement.cl.line);
        }
        else
        {
            break;
        }
    }
    printf("+------------------------------------------------------------------------------------------------------------------+\n");
}
