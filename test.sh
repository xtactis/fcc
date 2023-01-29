#!/bin/bash

total_tests=0
passed_tests=0

fcc_silent='-s'
if [[ $1 = '-loud' ]]; then
    fcc_silent=''
fi

check() {
    # TODO(mdizdar): actually implement testing
    res="\e[32mOK"
    build/fcc $fcc_silent tests/$1.c -o tests/$1
    if [ $? != 0 ]; then
        res="\e[31mFAILED"
    else
        passed_tests=$((passed_tests + 1))
    fi
    total_tests=$((total_tests + 1))
    echo -e "test \`$1\` result: $res\e[0m"
}

./build.sh

if [ $? != 0 ]; then
    echo -e "\e[31mBuild failed!\e[0m"
    exit 1
fi

echo "==================================="
echo "               TESTS               "
echo "==================================="

check 'main'
check 'int'
check 'two_variables'
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

echo "Result: $passed_tests/$total_tests tests passed!"
