#include "repetition_tester.h"

#include <string.h>
#if OS_LINUX
#include <libproc.h>
#endif
#undef bool


repetition_tester g_tester = {};

char const *value_to_cstring(int32 value)
{
    switch (value)
    {
    case RepTestMeasurement_Time: return "Time";
    case RepTestMeasurement_Bytes: return "Bytes";
    case RepTestMeasurement_PageFaults: return "Page Faults";

    case RepTestMeasurement_None:
    default:
        return "<error>";
    }
    return "<error>";
}

void print_separator()
{
    printf("+-------------------+-----");
    for (int i = 0; i < RepTestMeasurement_Count - 1; i++)
    {
        printf("+-------------------");
    }
    printf("+\n");
}

void print_inter_separator()
{
    printf("+                   +-----");
    for (int i = 0; i < RepTestMeasurement_Count - 1; i++)
    {
        printf("+-------------------");
    }
    printf("+\n");
}

void print_header()
{
    int n = 0;
    int w = 0;

    print_separator();
    print_column(20, "| %s", "Label");
    print_column(6, "| %s", "");

    for (int value = RepTestMeasurement_None + 1; value < RepTestMeasurement_Count; value++)
    {
        print_column(20, "| %s", value_to_cstring(value));
    }
    print_column(1, "%s\n", "|");
    print_separator();
}

void print_minimum_measurement()
{
    printf("\r");

    int n = 0;
    int w = 0;

    uint64 time = g_tester.minimum.value[RepTestMeasurement_Time];
    uint64 bytes = g_tester.minimum.value[RepTestMeasurement_Bytes];
    uint64 page_faults = g_tester.minimum.value[RepTestMeasurement_PageFaults];

    print_column(20, "| %s", g_tester.label);
    print_column(6, "| %s", "Min");
    print_column(20, "| %.0lf us", time / 1000.0);
    print_column(20, "| %.6lf gb/s", bytes / ((float64) GIGABYTES(1) * time * 1e-9));
    if (page_faults > 0)
        print_column(20, "| %llu (%.2lf kB/f)", page_faults, (float64) 1e-3 * bytes / page_faults);
    else
        print_column(20, "%s", "|");
    print_column(1, "%s", "|");
}

void print_rest_of_measurements()
{
    printf("\n");

    int n = 0;
    int w = 0;

    {
        print_column(20, "%s", "|");
        print_column(6, "| %s", "Max");

        uint64 time = g_tester.maximum.value[RepTestMeasurement_Time];
        uint64 bytes = g_tester.maximum.value[RepTestMeasurement_Bytes];
        uint64 page_faults = g_tester.maximum.value[RepTestMeasurement_PageFaults];
        print_column(20, "| %.0lf us", time / 1000.0);
        print_column(20, "| %.2lf gb/s", bytes / ((float64) GIGABYTES(1) * time * 1e-9));
        if (page_faults > 0)
            print_column(20, "| %llu (%.2lf kB/f)", page_faults, (float64) 1e-3 * bytes / page_faults);
        else
            print_column(20, "%s", "|");
        print_column(1, "%s", "|");
    }
    printf("\n");
    // {
    //     print_column(20, "%s", "|");
    //     print_column(6, "| %s", "Avg");
    //     for (int value = RepTestMeasurement_None + 1; value < RepTestMeasurement_Count; value++)
    //     {
    //         print_column(20, "| %.1lf", (float64) g_tester.total.value[value] / (float64) g_tester.hit_count);
    //     }
    //     print_column(1, "%s", "|");
    // }
    // printf("\n");
}

#if OS_LINUX
uint64 get_pagefaults_count()
{
    struct proc_taskinfo info;
    int pid = getpid();

    if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &info, sizeof(info)) > 0) {
        // printf("Resident Memory: %llu KB\n", info.pti_resident_size / 1024);
        // printf("Virtual Memory: %llu KB\n", info.pti_virtual_size / 1024);
        // printf("Page Faults: %d\n", info.pti_faults);
        return info.pti_faults;
    } else {
        printf("Failed to get proc info\n");
    }
    return 0;
}
#endif

#if OS_WINDOWS
#include <psapi.h>
uint64 get_pagefaults_count()
{
    static HANDLE ProcessHandle;
    if(!ProcessHandle)
    {
        ProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    }

    uint64 Result = 0;
    if (ProcessHandle)
    {
        PROCESS_MEMORY_COUNTERS_EX MemoryCounters = {};
        MemoryCounters.cb = sizeof(MemoryCounters);
        GetProcessMemoryInfo(ProcessHandle, (PROCESS_MEMORY_COUNTERS *)&MemoryCounters, sizeof(MemoryCounters));

        Result = MemoryCounters.PageFaultCount;
    }
    return Result;
}
#endif

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
        print_header();

        g_tester.state = RepTestState_Testing;
        g_tester.start_time = get_time();
        g_tester.hit_count = 0;

        memset(&g_tester.current, 0, sizeof(g_tester.current));
        memset(&g_tester.minimum, 0, sizeof(g_tester.total));
        memset(&g_tester.maximum, 0, sizeof(g_tester.total));
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
        float64 seconds_since_start = (float64) (get_time() - g_tester.start_time) / 1000000000.0;

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
            print_separator();
        }

        memset(&g_tester.current, 0, sizeof(g_tester.current));
    }
    if (g_tester.state == RepTestState_Testing) g_tester.hit_count += 1;
    return (g_tester.state == RepTestState_Testing);
}
