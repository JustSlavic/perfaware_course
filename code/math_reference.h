#ifndef MATH_REFERENCE_H
#define MATH_REFERENCE_H

#include "base.h"


typedef float64 math_function(float64);

typedef struct
{
    float64 x, y;
} math_reference_pair;

typedef struct
{
    char const *function_name;
    math_reference_pair *table;
    uint64 count;
} math_reference_table;



#endif // MATH_REFERENCE_H
