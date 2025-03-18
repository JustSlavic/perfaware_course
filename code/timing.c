#include "timing.h"


uint64 get_time()
{
    struct timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1e9 + t.tv_nsec;
}
