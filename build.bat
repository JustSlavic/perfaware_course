@echo off

DEL *.obj 2>NUL
DEL *.pdb 2>NUL
DEL *.ilk 2>NUL

SET MSVC_FLAGS=/std:c++17 /MTd /nologo /GR- /O2 /Zi /EHa-
SET WARNINGS=/W4 /WX /wd4201 /wd4100 /wd4189 /wd4505 /wd4702 /D_CRT_SECURE_NO_WARNINGS
SET DEFINES=/DOS_WINDOWS=1 /DDEBUG_PROFILE=1 /DARCH_64BIT=1 /DBYTE_ORDER=1234

SET INCLUDES=/I../code

cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fegendata ../code/generate_data.c
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fecompute ../code/compute.c
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fereptest ../code/repetition_testing.c
cl %MSVC_FLAGS% %WARNINGS% %DEFINES% %INCLUDES% /Fepfcount ../code/page_fault_counter.c

nasm -f win64 -o nop_loop.obj ../code/nop_loop.asm
lib nop_loop.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Feasmcall ../code/asm_call.c

nasm -f win64 -o bpredict.obj ../code/test_branch_predictor.asm
lib bpredict.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Febpredict ../code/test_branch_predictor.c

nasm -f win64 -o rat.obj ../code/rat.asm
lib rat.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Ferat ../code/test_rat.c

nasm -f win64 -o exeports.obj ../code/test_execution_ports.asm
lib exeports.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Feeports ../code/test_execution_ports.c

nasm -f win64 -o simdload.obj ../code/simd_load.asm
lib simdload.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fesimd_load ../code/test_simd_load.c

nasm -f win64 -o cachesize.obj ../code/cache_size.asm
lib cachesize.obj
cl /Zi /O2 %WARNINGS% %DEFINES% %INCLUDES% /Fecache_size ../code/test_cache_size.c
