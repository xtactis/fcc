#!/bin/bash

echo "==================================="
echo "               BUILD               "
echo "==================================="

need_to_rebuild=0

# check if there have been any changes
if [ -f build/fcc ]; then
    last_modified_exe=`stat --format=%Y build/fcc`
    last_modified_src=`stat --format=%Y **/*.h main.c build.sh | sort -n | tail -1`
    if [ $last_modified_src -gt $last_modified_exe ]; then
        need_to_rebuild=1
    fi
else
    need_to_rebuild=1
fi

if [ $need_to_rebuild = "1" ]; then
    mkdir -p build
    pushd build > /dev/null

    time gcc -std=c17 -Wall -Wextra -Og -g ../main.c -o fcc
    status=$?

    popd > /dev/null # build
    exit $status
else
    echo "Source hasn't been modified!"
fi


