#!/bin/bash

echo "==================================="
echo "               BUILD               "
echo "==================================="

SCRIPT_DIR="$( pwd )"

need_to_rebuild=0
c_files="$SCRIPT_DIR/main.c
         $SCRIPT_DIR/IR/*.c"
h_files="$SCRIPT_DIR/IR/*.h
         $SCRIPT_DIR/IR2AVR/*.h
         $SCRIPT_DIR/utils/*.h
         $SCRIPT_DIR/C/*.h
         $SCRIPT_DIR/AVR/*h"

# check if there have been any changes
if [ -f build/fcc ]; then
    last_modified_exe=`stat --format=%Y build/fcc`
    last_modified_src=`stat --format=%Y $c_files $h_files build.sh | sort -n | tail -1`
    if [ $last_modified_src -gt $last_modified_exe ]; then
        need_to_rebuild=1
    fi
else
    need_to_rebuild=1
fi

if [ $need_to_rebuild = "1" ]; then
    mkdir -p build
    pushd build > /dev/null

    time gcc -std=c17 -Wall -Wextra -Og -g -fdiagnostics-color=always $c_files -o fcc
    status=$?

    popd > /dev/null # build
    exit $status
else
    echo "Source hasn't been modified!"
fi


