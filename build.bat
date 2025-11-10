@echo off

DEL *.obj 2>NUL
DEL *.pdb 2>NUL
DEL *.ilk 2>NUL

SET MSVC_FLAGS=/std:c++17 /MTd /nologo /GR- /O2 /Zi /EHa-
SET WARNINGS=/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4702 /D_CRT_SECURE_NO_WARNINGS
SET DEFINES=/DOS_WINDOWS=1 /DDEBUG_PROFILE=1 /DARCH_64BIT=1 /DBYTE_ORDER=1234

SET INCLUDES=/I../code

rem 00 generate and compute haversine data naively
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fe00_generate_haversine_data ../code/00_generate_haversine_data.c
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fe00_compute_haversine ../code/00_compute_haversine.c

rem 01 test the speed of fread with and without allocating memory pages
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe01_test_fread ../code/01_test_fread.c

rem 02 test pagefault count
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe02_test_pagefault_count ../code/02_test_pagefault_count.c

rem 03 test instruction level parallelism
nasm -f win64 -o 03_asm.obj ../code/asm_x86_64/03_test_cpu_frontend.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe03_test_cpu_frontend ../code/03_test_cpu_frontend.c 03_asm.obj

rem 04 find bottleneck on decoding
nasm -f win64 -o 04_asm.obj ../code/asm_x86_64/04_test_decoding_bottleneck.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe04_test_decoding_bottleneck ../code/04_test_decoding_bottleneck.c 04_asm.obj

rem 05 test branch prediction capabilities
nasm -f win64 -o 05_asm.obj ../code/asm_x86_64/05_test_branch_predictor.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe05_test_branch_predictor ../code/05_test_branch_predictor.c 05_asm.obj

rem 06 test the work of RAT
nasm -f win64 -o 06_asm.obj ../code/asm_x86_64/06_test_rat.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe06_test_rat ../code/06_test_rat.c 06_asm.obj

rem 07 load 1 byte vs load 8 bytes
nasm -f win64 -o 07_asm.obj ../code/asm_x86_64/07_test_one_byte_load.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe07_test_one_byte_load ../code/07_test_one_byte_load.c 07_asm.obj

rem 08 test number of execution ports by watching speedup on compute
nasm -f win64 -o 08_asm.obj ../code/asm_x86_64/08_test_execution_ports.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe08_test_execution_ports ../code/08_test_execution_ports.c 08_asm.obj

rem 09 test the maximum throughput we could get
nasm -f win64 -o 09_asm.obj ../code/asm_x86_64/09_test_simd_load.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe09_test_simd_load ../code/09_test_simd_load.c 09_asm.obj

rem 10 test cache sizes
nasm -f win64 -o 10_asm.obj ../code/asm_x86_64/10_test_cache_size.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe10_test_cache_size ../code/10_test_cache_size.c 10_asm.obj

rem 11 test unaligned loads
nasm -f win64 -o 11_asm.obj ../code/asm_x86_64/11_test_unaligned_load.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe11_test_unaligned_load ../code/11_test_unaligned_load.c 11_asm.obj

rem 12 test math function domains
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe12_test_math_functions_domain ../code/12_test_math_functions_domain.c

rem 13 test math function error table vs stdlib
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe13_test_math_functions_error ../code/13_test_math_functions_error.c

rem 14 test variants of how to compute sqrt from simd instructions
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe14_test_sqrt_simd ../code/14_test_sqrt_simd.c

rem 15 test accuracy of sine taylor series
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe15_test_taylor_sine_approximations ../code/15_test_taylor_sine_approximations.c

rem 16 test accuracy of horner method of computing the polynomial
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe16_test_taylor_horner ../code/16_test_taylor_horner.c

rem 17 test accuracy of horner methods using fmadds
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe17_test_taylor_horner_fmadd ../code/17_test_taylor_horner_fmadd.c

rem 18 test accuracy of sine approximation compare taylor and minimax accuracy
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe18_test_fma_table ../code/18_test_fma_table.c

rem 19 arcsine approximation
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe19_test_arcsine_part_1 ../code/19_test_arcsine_part_1.c

rem 20 test all my new math functions accuracy
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe20_test_my_trig_functions ../code/20_test_my_trig_functions.c

rem 21 test error between my haversine with my math functions and CRT haversine
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe21_test_my_haversine ../code/21_test_my_haversine.c

rem 22 remove waste
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe22_test_remove_waste ../code/22_test_remove_waste.c

rem 23 prevent optimizations
clang -ggdb -O3 -Wall -Werror -Wno-microsoft-enum-forward-reference -DOS_WINDOWS=1 -DCOMPILER_CLANG=1 -mavx2 -mfma -I../code -o 23_test_preventing_optimizations.exe ../code/23_test_preventing_optimizations.c
echo 23_test_preventing_optimizations.c
rem cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe23_test_preventing_optimizations ../code/23_test_preventing_optimizations.c
