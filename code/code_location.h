#ifndef CODE_LOCATION_H
#define CODE_LOCATION_H


typedef struct
{
    char const *filename;
    char const *function;
    uint32 line;
} code_location;

#define CL_HERE ((code_location){ .filename = __FILE__, .function = __FUNCTION__, .line = __LINE__ })


#endif
