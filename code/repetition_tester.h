#ifndef REPETITION_TESTER_H
#define REPETITION_TESTER_H

#include "base.h"
#undef max
#define max(a, b) ((a) < (b) ? (b) : (a))

enum
{
    RepTestMeasurement_None,
    RepTestMeasurement_Time,
    RepTestMeasurement_Bytes,
    RepTestMeasurement_PageFaults,

    RepTestMeasurement_Count,
};

STRUCT(reptest_measurement)
{
    uint64 value[RepTestMeasurement_Count];
};

ENUM(repetition_tester_state)
{
    RepTestState_Uninitialized,
    RepTestState_Testing,
    RepTestState_Completed,
    RepTestState_Error,
};

STRUCT(repetition_tester)
{
    repetition_tester_state state;
    char const *label;
    bool print_results;

    uint64 start_time;
    uint32 hit_count;

    reptest_measurement current;
    reptest_measurement minimum;
    reptest_measurement maximum;
    reptest_measurement total;
};

extern repetition_tester g_tester;

bool is_testing(float64 seconds);
void reptest_count_bytes(uint64 bytes);
void reptest_begin_time();
void reptest_end_time();

void print_separator();
void print_header();
void print_min_max();

#endif
