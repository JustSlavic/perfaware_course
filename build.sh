#!/bin/bash

set -e # stops the script on the first error

os_name=%(uname -s)
C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"

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

function compile_test_x86_64_linux() {
    # hint: 0. make base.S file to handle preprocessor directives
    #                    NASM <-> GNU
    #                  global <-> .global
    #           section .text <-> .text
    #                      db <-> .byte
    #       1. use `gcc` to preprocessor assembly files
    #       2. don't forget about `-x assembly-with-cpp` otherwise
    #          `gcc` will say you're a faggot, and stop working
    #       3. save the preprocessored file in the temporary file
    #       4. use `as` to assemble the object file
    #       5. don't forget that default syntax is AT&T
    #          (use -msyntax=intel)
    #       6. don't forget to use intel's instruction mnemonics
    #          (use -mmnemonic=intel)
    #       7. don't forget to turn off % signs before register names
    #          (use -mnaked-reg)
    #       9. don't forget that Linux'es ABI is different from Microsoft's,
    #          (use __attribute__((ms-abi)) for all asm functions)
    #       8. use `gcc` to compile c file and link it with object
    #          file as usual
}

if [[ $os_name == "Linux" ]]; then
    DEFINES="-DDEBUG_PROFILE=1 -DOS_LINUX"

    compile_test_x86_64_linux "03_test_cpu_frontend"
    compile_test_x86_64_linux "04_test_decoding_bottleneck"
    compile_test_x86_64_linux "05_test_branch_predictor"
    compile_test_x86_64_linux "06_test_rat"
    compile_test_x86_64_linux "07_test_one_byte_load"
    compile_test_x86_64_linux "08_test_execution_ports"
elif [[ $os_name == "Darwin" ]]; then
    DEFINES="-DDEBUG_PROFILE=1 -DOS_MAC"

    # gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/generate_data ../code/generate_data.c
    # gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/compute ../code/compute.c
    # gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/rep_test ../code/repetition_testing.c

    # gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/01_test_fread code/01_test_fread.c
    # gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o build/02_test_pagefault_count code/02_test_pagefault_count.c

    # ld abc.o -o abc -e _start -arch arm64  -l System -syslibroot `xcrun -sdk macosx --show-sdk-path`

    compile_test_arm64_mac "03_test_cpu_frontend"
    compile_test_arm64_mac "04_test_decoding_bottleneck"
    compile_test_arm64_mac "05_test_branch_predictor"
    compile_test_arm64_mac "06_test_rat"
    compile_test_arm64_mac "07_test_one_byte_load"
    compile_test_arm64_mac "08_test_execution_ports"
else
    echo "Could not recognize the system I'm on! (${os_name})"
    exit 1
fi
