#ifndef BASE_H
#define BASE_H

typedef int bool;
#define true 1
#define false 0

char const *spaces = "                                                                                                                                                                                                        ";
char const *dashes = "--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------";

typedef   signed char        int8;
typedef unsigned char       uint8;
typedef          short       int16;
typedef unsigned short      uint16;
typedef   signed int         int32;
typedef unsigned int        uint32;
typedef   signed long long   int64;
typedef unsigned long long  uint64;
typedef          float     float32;
typedef          double    float64;

#if OS_WINDOWS || OS_LINUX
#define UINT64_MAX 0xffffffffffffffff
#endif

#if OS_LINUX
#if defined(__GNUC__) && !defined(__clang__)
#define ASM_CALL __attribute__((ms_abi))
#endif // GNU COMPILER
#else
#define ASM_CALL
#endif // OS_LINUX

#define STRUCT(NAME) typedef struct NAME NAME; struct NAME
#define ENUM(NAME) typedef enum NAME NAME; enum NAME
#define ARRAY_COUNT(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
#define MACRO_EXPAND(X) X
#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)
#define KILOBYTES(N) (1024ull * (N))
#define MEGABYTES(N) (1024ull * KILOBYTES(N))
#define GIGABYTES(N) (1024ull * MEGABYTES(N))
#define TERABYTES(N) (1024ull * GIGABYTES(N))
#define square(x) ((x)*(x))
#define deg_to_rad(deg) (0.01745329251994329577*(deg))

#define pi 3.141592653589793238462643383279502884197169399375105820974944592307816406286
#define half_pi 1.5707963267948966192313216916397514420985846996875529104874722961539082031
#define two_pi 6.28318530717958647692528676655900576839433879875021164194988918461563281257
#define sqrt_2 1.41421356237309504880168872420969807856967187537694807317667973799073247846
#define one_over_sqrt_2 0.707106781186547524400844362104849039284835937688474036588339868995366239231053519425193767163820786367506923115456148512462418027925368606322061

typedef struct
{
    char const *filename;
    char const *function;
    uint32 line;
} code_location;

#define CL_HERE ((code_location){ .filename = __FILE__, .function = __FUNCTION__, .line = __LINE__ })


#define print_column(width, fmt, ...) \
    n += printf("%.*s", max(w - n, 0), spaces); \
    n += printf(fmt, __VA_ARGS__); \
    w += width
#define print_column_optional(cond, width, fmt, ...) \
    if (cond) { \
        n += printf("%.*s", max(w - n, 0), spaces); \
        n += printf(fmt, __VA_ARGS__); \
    } \
    w += width

#endif
