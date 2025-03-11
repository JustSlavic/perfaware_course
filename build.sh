#!/bin/bash

C_FLAGS="-std=c11 -g"
WARNINGS="-Wall -Werror"
DEFINES="-DDEBUG_PROFILE=1"

INCLUDES="-I../code"

gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o generate_data ../code/generate_data.c
gcc $C_FLAGS $WARNINGS $DEFINES $INCLUDES -o compute ../code/compute.c
