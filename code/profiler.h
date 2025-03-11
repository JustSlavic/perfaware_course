#ifndef PROFILER_H
#define PROFILER_H

#include "base.h"
#include "code_location.h"


typedef struct
{
    uint64 t0;
    uint64 old_elapsed_inclusive;

    uint32 index;
    uint32 parent_index;

    char const *label;
    code_location cl;
} profile_point;

typedef struct
{
    uint64 elapsed_exclusive;
    uint64 elapsed_inclusive;
    uint32 hit_count;
    uint64 byte_count;

    char const *label;
    code_location cl;
} profile_measurement;

typedef struct
{
    uint64 t0, t1;
    profile_measurement measurements[256];
} profiler;


extern profiler g_profiler;

profile_point profile_begin(char const *label, code_location cl, uint32 index);
profile_point profile_begin_bandwidth(char const *label, code_location cl, uint32 index, uint64 byte_count);
void profile_end(profile_point pp);
void print_profile_result();

#define PROFILE_BEGIN() do { g_profiler.t0 = get_time(); } while(0)
#define PROFILE_END() do { g_profiler.t1 = get_time(); print_profile_result(); } while(0)
#if DEBUG_PROFILE
#define PROFILE_HASH hash_djb2
#define PROFILE_BLOCK_BEGIN(NAME) profile_point NAME##__ = profile_begin(#NAME, CL_HERE, __COUNTER__ + 1)
#define PROFILE_BLOCK_BANDWIDTH_BEGIN(NAME, BYTECOUNT) profile_point NAME##__ = profile_begin_bandwidth(#NAME, CL_HERE, __COUNTER__ + 1, BYTECOUNT)
#define PROFILE_BLOCK_END(PROFILE_POINT) profile_end(PROFILE_POINT##__)
#define PROFILE_FUNCTION_BEGIN() profile_point pfp_##__FUNCTION__##__ = profile_begin(__FUNCTION__, CL_HERE, __COUNTER__ + 1)
#define PROFILE_FUNCTION_BANDWIDTH_BEGIN(BYTECOUNT) profile_point pfp_##__FUNCTION__##__ = profile_begin_bandwidth(__FUNCTION__, CL_HERE, __COUNTER__ + 1, BYTECOUNT)
#define PROFILE_FUNCTION_END() profile_end(pfp_##__FUNCTION__##__)
#else
#define PROFILE_HASH
#define PROFILE_BLOCK_BEGIN(...)
#define PROFILE_BLOCK_BANDWIDTH_BEGIN(...)
#define PROFILE_BLOCK_END(...)
#define PROFILE_FUNCTION_BEGIN(...)
#define PROFILE_FUNCTION_BANDWIDTH_BEGIN(...)
#define PROFILE_FUNCTION_END(...)
#endif



#endif // PROFILER_H
