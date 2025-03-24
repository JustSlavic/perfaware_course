#include "timing.h"

#if OS_WINDOWS

#include <intrin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

uint64 get_os_frequency()
{
    LARGE_INTEGER Freq;
    QueryPerformanceFrequency(&Freq);
    return Freq.QuadPart;
}

uint64 get_os_timer()
{
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return Value.QuadPart;
}

uint64 get_cpu_timer()
{
    return __rdtsc();
}

uint64 estimate_cpu_freq()
{
    uint64 milliseconds_to_wait_os = 100;
    uint64 os_frequency = get_os_frequency();

    uint64 os_wait_time = os_frequency * milliseconds_to_wait_os / 1000;
    uint64 os_elapsed = 0;

    uint64 os_t1 = 0;
    uint64 os_t0 = get_os_timer();
    uint64 cpu_t0 = get_cpu_timer();

    while (os_elapsed < os_wait_time)
    {
        os_t1 = get_os_timer();
        os_elapsed = os_t1 - os_t0;
    }

    uint64 cpu_t1 = get_cpu_timer();
    uint64 cpu_elapsed = cpu_t1 - cpu_t0;

    uint64 cpu_frequency = os_elapsed ? os_frequency * cpu_elapsed / os_elapsed : 0;
    return cpu_frequency;
}


uint64 get_time()
{
    return get_cpu_timer();
}

uint64 get_frequency()
{
    return estimate_cpu_freq();
}

#endif


#if OS_MAC
uint64 get_time()
{
    struct timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}
#endif
