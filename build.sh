#!/bin/bash

set -e

C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"
DEFINES="-DDEBUG_PROFILE=1 -DOS_MAC"

INCLUDES="-I../code"

mkdir -p build

function compile_test_arm64_mac() {
    assembly_source_name="code/$1.arm64"
    assembly_object_name="build/$1_asm.o"
    source_name="code/$1.c"
    executable_name="build/$1"
    as $assembly_source_name -o $assembly_object_name
    echo "Done [$assembly_object_name]"
    gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o $executable_name $source_name $assembly_object_name
    echo "Done [$executable_name]"
}

# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/generate_data ../code/generate_data.c
# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/compute ../code/compute.c
# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/rep_test ../code/repetition_testing.c

# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/01_test_fread code/01_test_fread.c
# echo "Done [build/01_test_fread]"
# gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/02_test_pagefault_count code/02_test_pagefault_count.c
# echo "Done [build/02_test_pagefault_count]"

# ld abc.o -o abc -e _start -arch arm64  -l System -syslibroot `xcrun -sdk macosx --show-sdk-path`

compile_test_arm64_mac "03_test_cpu_frontend"
compile_test_arm64_mac "04_test_decoding_bottleneck"
compile_test_arm64_mac "05_test_branch_predictor"
compile_test_arm64_mac "06_test_rat"
