#!/bin/bash

C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"
DEFINES="-DDEBUG_PROFILE=1 -DOS_MAC"

INCLUDES="-I../code"

mkdir -p build

# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/generate_data ../code/generate_data.c
# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/compute ../code/compute.c
# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/rep_test ../code/repetition_testing.c

gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/01_test_fread code/01_test_fread.c
gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/02_test_pagefault_count code/02_test_pagefault_count.c

# ld abc.o -o abc -e _start -arch arm64  -l System -syslibroot `xcrun -sdk macosx --show-sdk-path`

as code/03_test_cpu_frontend.arm64 -o 03_asm.o
gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o build/03_test_cpu_frontend code/03_test_cpu_frontend.c build/03_asm.o

as code/04_test_decoding_bottleneck.arm64 -o build/04_asm.o
gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o build/04_test_decoding_bottleneck code/04_test_decoding_bottleneck.c build/04_asm.o

as code/05_test_branch_predictor.arm64 -o build/05_asm.o
gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o build/05_test_branch_predictor code/05_test_branch_predictor.c build/05_asm.o
