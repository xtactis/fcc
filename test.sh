#!/bin/bash

check() {
    # TODO(mdizdar): actually implement testing
    res="OK"
    build/fcc -s tests/$1.c -o tests/$1
    if [ $? != 0 ]; then
        res="FAILED"
    fi
    echo "$1 ..... $res"
}

./build.sh

echo "==================================="
echo "               TESTS               "
echo "==================================="

check 'main'
check 'int'
check 'int_assign'
check 'int_assign_exp'
check 'return_exp'
check 'return_var'
check 'scope'
check 'ternary'
check 'if'
check 'ifelse'
check 'ifelseif'
check 'while'
