#include "timing.h"


uint64 get_time()
{
    struct timespec t = {};
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000000000 + t.tv_nsec;
}
