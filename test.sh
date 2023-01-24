#!/bin/bash

check() {
    # TODO(mdizdar): actually implement testing
    res="OK"
    build/fcc -s $1.c -o $1
    if [ $? != 0 ]; then
        res="FAILED"
    fi
    echo "$1 ..... $res"
}

if ! [ -f build/fcc ]; then
    ./build.sh
fi

echo "==================================="
echo "               TESTS               "
echo "==================================="

check 'tests/scope'
check 'tests/ternary'
check 'tests/if'
check 'tests/ifelse'
check 'tests/ifelseif'
check 'tests/while'
