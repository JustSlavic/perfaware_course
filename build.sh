#!/bin/bash

set -e # stops the script on the first error

os_name=$(uname -s)
C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"

INCLUDES="-I../code"

mkdir -p build

function compile_test_arm64_mac() {
    source_name="code/$1.c"
    executable_name="build/$1"
    clang $C_FLAGS -O2 $WARNINGS $DEFINES $INCLUDES -o $executable_name $source_name
    echo "Done [$executable_name]"
}

function compile_test_arm64_mac_asm() {
    assembly_source_name="code/asm_arm64/$1.S"
    assembly_object_name="build/$1_asm.o"
    source_name="code/$1.c"
    executable_name="build/$1"
    as $assembly_source_name -o $assembly_object_name
    echo "Done [$assembly_object_name]"

    clang $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o $executable_name $source_name $assembly_object_name
    echo "Done [$executable_name]"
}

function compile_test_x86_64_linux() {
    assembly_source_name="code/$1.x86_64.S"
    assembly_tmp_name="build/$1_tmp.S"
    assembly_object_name="build/$1_asm.o"
    source_name="code/$1.c"
    executable_name="build/$1"

    cp $assembly_source_name $assembly_tmp_name
    sed -i -e 's/global/\.global/g' -e 's/section \.text/\.text/g' -e 's/db/\.byte/g' -e 's/;/#/g' $assembly_tmp_name
    as -msyntax=intel -mmnemonic=intel -mnaked-reg -o $assembly_object_name $assembly_tmp_name
    rm $assembly_tmp_name
    echo "Done [$assembly_object_name]"

    gcc $C_FLAGS -O1 $WARNINGS $DEFINES $INCLUDES -o $executable_name $source_name $assembly_object_name
    echo "Done [$executable_name]"
}

if [[ $os_name == "Linux" ]]; then
    DEFINES="-DDEBUG_PROFILE=1 -DOS_LINUX"

    compile_test_x86_64_linux "03_test_cpu_frontend"
    compile_test_x86_64_linux "04_test_decoding_bottleneck"
    compile_test_x86_64_linux "05_test_branch_predictor"
    compile_test_x86_64_linux "06_test_rat"
    compile_test_x86_64_linux "07_test_one_byte_load"
    compile_test_x86_64_linux "08_test_execution_ports"
    compile_test_x86_64_linux "09_test_simd_load"
    compile_test_x86_64_linux "10_test_cache_size"
elif [[ $os_name == "Darwin" ]]; then
    DEFINES="-DDEBUG_PROFILE=1 -DOS_MAC"

    compile_test_arm64_mac "00_generate_haversine_data"
    compile_test_arm64_mac "00_compute_haversine"
    compile_test_arm64_mac "01_test_fread"
    compile_test_arm64_mac "02_test_pagefault_count"

    compile_test_arm64_mac_asm "03_test_cpu_frontend"
    compile_test_arm64_mac_asm "04_test_decoding_bottleneck"
    compile_test_arm64_mac_asm "05_test_branch_predictor"
    compile_test_arm64_mac_asm "06_test_rat"
    compile_test_arm64_mac_asm "07_test_one_byte_load"
    compile_test_arm64_mac_asm "08_test_execution_ports"
    compile_test_arm64_mac_asm "09_test_simd_load"
    compile_test_arm64_mac_asm "10_test_cache_size"
    compile_test_arm64_mac_asm "11_test_unaligned_load"

    compile_test_arm64_mac "12_test_math_functions_domain"
    compile_test_arm64_mac "13_test_math_functions_error"
    compile_test_arm64_mac "14_test_sqrt_simd"
    compile_test_arm64_mac "15_test_taylor_sine_approximations"
    compile_test_arm64_mac "16_test_taylor_horner"
    compile_test_arm64_mac "17_test_taylor_horner_fmadd"
    compile_test_arm64_mac "18_test_fma_table"
    compile_test_arm64_mac "19_test_arcsine_part_1"
    compile_test_arm64_mac "20_test_my_trig_functions"
    compile_test_arm64_mac "21_test_my_haversine"
    compile_test_arm64_mac "22_test_remove_waste"
    compile_test_arm64_mac "23_test_preventing_optimizations"
    clang -std=c11 -O3 -Wall -Werror -DDEBUG_PROFILE=1 -DOS_MAC -I../code -S code/23_test_preventing_optimizations.c -o build/23_test_preventing_optimizations.S
else
    echo "Could not recognize the system I'm on! (${os_name})"
    exit 1
fi
