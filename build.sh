#!/bin/bash

C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"
DEFINES="-DDEBUG_PROFILE=1 -DOS_MAC"

INCLUDES="-I../code"

gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o generate_data ../code/generate_data.c
gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o compute ../code/compute.c
gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o rep_test ../code/repetition_testing.c

gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o 01_test_fread ../code/01_test_fread.c
gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o 02_test_pagefault_count ../code/02_test_pagefault_count.c

# ld abc.o -o abc -e _start -arch arm64  -l System -syslibroot `xcrun -sdk macosx --show-sdk-path`

as ../code/03_test_cpu_frontend.arm64 -o 03_asm.o
gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o 03_test_cpu_frontend ../code/03_test_cpu_frontend.c 03_asm.o
