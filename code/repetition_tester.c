#include "repetition_tester.h"
#include "os_specifics.h"
#include "timing.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#undef bool


repetition_tester g_tester = {};

void table_printer__push_column(table_printer *table, char const *label, int width)
{
    table->labels[table->count] = label;
    table->widths[table->count] = width;
    table->count += 1;
}

void table_printer__start_row(table_printer *table)
{
    table->index = 0;
}

void table_printer__finish_row(table_printer *table)
{
    printf("|\n");
}

void table_printer__print_column(table_printer *table, char const *fmt, ...)
{
    int m = 0;
    m += printf("| ");
    va_list args;
    va_start(args, fmt);
    m += vprintf(fmt, args);
    va_end(args);
    printf("%.*s", max(table->widths[table->index] - m + 3, 0), spaces);
    table->index += 1;
}

void table_printer__print_separator(table_printer *table)
{
    if (!g_tester.print_results) return;

    // printf("+-|||-+")
    for (int i = 0; i < g_tester.printer.count; i++)
    {
        printf("+-");
        printf("%.*s", g_tester.printer.widths[i], dashes);
        printf("-");
    }
    printf("+\n");

    // printf("+-------------------+-----");
    // for (int i = 0; i < RepTestMeasurement_Count - 1; i++)
    // {
    //     printf("+-------------------");
    // }
    // printf("+\n");
}

void table_printer__print_header(table_printer *table)
{
    if (!g_tester.print_results) return;
    table_printer *printer = &g_tester.printer;

    table_printer__print_separator(printer);
    table_printer__start_row(printer);
    for (int i = 0; i < printer->count; i++)
    {
        table_printer__print_column(printer, "%s", g_tester.printer.labels[i]);
    }
    table_printer__finish_row(printer);
    table_printer__print_separator(printer);
}

void print_minimum_measurement()
{
    if (!g_tester.print_results) return;
    table_printer *printer = &g_tester.printer;

    printf("\r");

    uint64 time = g_tester.minimum.value[RepTestMeasurement_Time];
    uint64 bytes = g_tester.minimum.value[RepTestMeasurement_Bytes];
    uint64 page_faults = g_tester.minimum.value[RepTestMeasurement_PageFaults];
    float64 seconds = (float64) time / (float64) get_frequency();

    table_printer__start_row(printer);
    table_printer__print_column(printer, "%s", g_tester.label);
    table_printer__print_column(printer, "Min");
    table_printer__print_column(printer, "%.0lf us", 1e6 * seconds);
    table_printer__print_column(printer, "%.6lf gb/s", (float64) bytes / (GIGABYTES(1) * seconds));
    if (page_faults > 0) table_printer__print_column(printer, "%llu (%.4lf kB/f)", page_faults, 1e-3 * bytes / page_faults);
    else                 table_printer__print_column(printer, "");
}

void print_rest_of_measurements()
{
    if (!g_tester.print_results) return;
    table_printer *printer = &g_tester.printer;

    table_printer__finish_row(printer);

    {
        uint64 time = g_tester.maximum.value[RepTestMeasurement_Time];
        uint64 bytes = g_tester.maximum.value[RepTestMeasurement_Bytes];
        uint64 page_faults = g_tester.maximum.value[RepTestMeasurement_PageFaults];
        float64 seconds = (float64) time / (float64) get_frequency();

        table_printer__start_row(printer);
        table_printer__print_column(printer, "");
        table_printer__print_column(printer, "Max");
        table_printer__print_column(printer, "%.0lf us", 1e6 * seconds);
        table_printer__print_column(printer, "%.6lf gb/s", (float64) bytes / (GIGABYTES(1) * seconds));
        if (page_faults > 0) table_printer__print_column(printer, "%llu (%.4lf kB/f)", page_faults, (float64) 1e-3 * bytes / page_faults);
        else                 table_printer__print_column(printer, "");
        table_printer__finish_row(printer);
    }
    {
        table_printer__start_row(printer);
        uint64 total_time = g_tester.total.value[RepTestMeasurement_Time];
        float64 avg_time = (float64) total_time / (float64) g_tester.hit_count;
        float64 avg_seconds = avg_time / get_frequency();
        table_printer__print_column(printer, "");
        table_printer__print_column(printer, "Avg");
        table_printer__print_column(printer, "%.0lf us", 1e6 * avg_seconds);
        table_printer__print_column(printer, "");
        table_printer__print_column(printer, "");
        table_printer__finish_row(printer);
    }
}

void reptest_count_bytes(uint64 bytes)
{
    g_tester.current.value[RepTestMeasurement_Bytes] = bytes;
}

void reptest_begin_time()
{
    g_tester.current.value[RepTestMeasurement_PageFaults] -= get_pagefaults_count();
    g_tester.current.value[RepTestMeasurement_Time] -= get_time();
}

void reptest_end_time()
{
    g_tester.current.value[RepTestMeasurement_Time] += get_time();
    g_tester.current.value[RepTestMeasurement_PageFaults] += get_pagefaults_count();
}

bool is_testing(float64 seconds)
{
    if (g_tester.state == RepTestState_Uninitialized)
    {
        table_printer__push_column(&g_tester.printer, "Label", 20);
        table_printer__push_column(&g_tester.printer, "", 3);
        table_printer__push_column(&g_tester.printer, "Time", 17);
        table_printer__push_column(&g_tester.printer, "Bytes", 17);
        table_printer__push_column(&g_tester.printer, "Page faults", 20);

        table_printer__print_header(&g_tester.printer);

        g_tester.state = RepTestState_Testing;
        g_tester.start_time = get_time();
        g_tester.hit_count = 0;

        memset(&g_tester.current, 0, sizeof(g_tester.current));
        memset(&g_tester.minimum, 0, sizeof(g_tester.minimum));
        memset(&g_tester.maximum, 0, sizeof(g_tester.maximum));
        memset(&g_tester.total, 0, sizeof(g_tester.total));

        g_tester.minimum.value[RepTestMeasurement_Time] = UINT64_MAX;
    }
    else if (g_tester.state == RepTestState_Completed)
    {
        g_tester.state = RepTestState_Testing;
        g_tester.start_time = get_time();
        g_tester.hit_count = 0;

        memset(&g_tester.current, 0, sizeof(g_tester.current));
        memset(&g_tester.minimum, 0, sizeof(g_tester.total));
        memset(&g_tester.maximum, 0, sizeof(g_tester.total));
        memset(&g_tester.total, 0, sizeof(g_tester.total));

        g_tester.minimum.value[RepTestMeasurement_Time] = UINT64_MAX;
    }
    else if (g_tester.state == RepTestState_Testing)
    {
        float64 seconds_since_start = (float64) (get_time() - g_tester.start_time) / get_frequency();

        for (int value = RepTestMeasurement_None; value < RepTestMeasurement_Count; value++)
        {
            g_tester.total.value[value] += g_tester.current.value[value];
        }

        uint64 elapsed = g_tester.current.value[RepTestMeasurement_Time];
        if (elapsed > g_tester.maximum.value[RepTestMeasurement_Time])
        {
            g_tester.maximum = g_tester.current;
        }
        if (elapsed < g_tester.minimum.value[RepTestMeasurement_Time])
        {
            g_tester.minimum = g_tester.current;
            g_tester.start_time = get_time();
            print_minimum_measurement();
            fflush(stdout);
        }

        if (seconds_since_start >= seconds)
        {
            g_tester.state = RepTestState_Completed;
            print_rest_of_measurements();
            table_printer__print_separator(&g_tester.printer);
        }

        memset(&g_tester.current, 0, sizeof(g_tester.current));
    }
    if (g_tester.state == RepTestState_Testing)
        g_tester.hit_count += 1;
    return (g_tester.state == RepTestState_Testing);
}
