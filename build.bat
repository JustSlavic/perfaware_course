@echo off

DEL *.obj 2>NUL
DEL *.pdb 2>NUL
DEL *.ilk 2>NUL

SET MSVC_FLAGS=/std:c++17 /MTd /nologo /GR- /O2 /Zi /EHa-
SET WARNINGS=/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4702 /D_CRT_SECURE_NO_WARNINGS
SET DEFINES=/DOS_WINDOWS=1 /DDEBUG_PROFILE=1 /DARCH_64BIT=1 /DBYTE_ORDER=1234

SET INCLUDES=/I../code

rem 00 generate and compute haversine data naively
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fegendata ../code/00_generate_haversine_data.c
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fecompute ../code/00_compute_haversine.c

rem 01 test the speed of fread with and without allocating memory pages
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe01_test_fread ../code/01_test_fread.c

rem 02 test pagefault count
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe02_test_pagefault_count ../code/02_test_pagefault_count.c

rem 03 test instruction level parallelism
nasm -f win64 -o 03_asm.obj ../code/03_test_cpu_frontend.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe03_test_cpu_frontend ../code/03_test_cpu_frontend.c 03_asm.obj

rem 04 find bottleneck on decoding
nasm -f win64 -o 04_asm.obj ../code/04_test_decoding_bottleneck.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe04_test_decoding_bottleneck ../code/04_test_decoding_bottleneck.c 04_asm.obj

rem 05 test branch prediction capabilities
nasm -f win64 -o 05_asm.obj ../code/05_test_branch_predictor.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe05_test_branch_predictor ../code/05_test_branch_predictor.c 05_asm.obj

rem 06 test the work of RAT
nasm -f win64 -o 06_asm.obj ../code/06_test_rat.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe06_test_rat ../code/06_test_rat.c 06_asm.obj

rem 07 load 1 byte vs load 8 bytes
nasm -f win64 -o 07_asm.obj ../code/07_test_one_byte_load.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe07_test_one_byte_load ../code/07_test_one_byte_load.c 07_asm.obj

rem 08 test number of execution ports by watching speedup on compute
nasm -f win64 -o 08_asm.obj ../code/08_test_execution_ports.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe08_test_execution_ports ../code/08_test_execution_ports.c 08_asm.obj

rem 09 test the maximum throughput we could get
nasm -f win64 -o 09_asm.obj ../code/09_test_simd_load.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe09_test_simd_load ../code/09_test_simd_load.c 09_asm.obj

rem 10 test cache sizes
nasm -f win64 -o 10_asm.obj ../code/10_test_cache_size.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe10_test_cache_size ../code/10_test_cache_size.c 10_asm.obj

rem 11 test unaligned loads
nasm -f win64 -o 11_asm.obj ../code/11_test_unaligned_load.x86_64.S
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe11_test_unaligned_load ../code/11_test_unaligned_load.c 11_asm.obj

rem 12 test math function domains
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe12_test_math_functions_domain ../code/12_test_math_functions_domain.c

rem 13 test math function error table vs stdlib
cl /nologo /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fe13_test_math_functions_error ../code/13_test_math_functions_error.c


rem nasm -f win64 -o bpredict.obj ../code/test_branch_predictor.asm
rem lib bpredict.obj
rem cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Febpredict ../code/test_branch_predictor.c

rem nasm -f win64 -o rat.obj ../code/rat.asm
rem lib rat.obj
rem cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Ferat ../code/test_rat.c

rem nasm -f win64 -o exeports.obj ../code/test_execution_ports.asm
rem lib exeports.obj
rem cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Feeports ../code/test_execution_ports.c

rem nasm -f win64 -o simdload.obj ../code/simd_load.asm
rem lib simdload.obj
rem cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fesimd_load ../code/test_simd_load.c

rem nasm -f win64 -o cachesize.obj ../code/cache_size.asm
rem lib cachesize.obj
rem cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fecache_size ../code/test_cache_size.c
