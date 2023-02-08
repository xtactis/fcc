#!/bin/bash

all_tests=( 'main' 'int' 'two_variables' 'int_assign' 'int_assign_exp' 'return_exp' 'return_var' 'scope' 'ternary' 'if' 'ifelse' 'ifelseif' 'ifsabound' 'while' 'for' 'undeclared_variable' )
declare -A negative_tests=(['undeclared_variable']=1)

usage() {
    echo "Usage: test [ -l | --loud] 
            [ -o | --only test1[,test2[,...]]]"
    exit 2
}

fcc_loud=0
run_only=${all_tests[@]}

parsed_arguments=$(getopt -a -n test -o lo: --long loud,only: -- "$@")
valid_arguments=$?
if [ "$valid_arguments" != "0" ]; then
    usage
fi

eval set -- "$parsed_arguments"
while :
do
    case "$1" in
        -l | --loud) fcc_loud=1 ; shift ;;
        -o | --only) IFS=',' read -ra run_only <<< "$2" ; shift 2 ;;
        --) shift; break ;;
        *) echo "Unrecognized option $1"
           usage ;;
    esac
done

total_tests=0
passed_tests=0

check() {
    # TODO(mdizdar): actually implement testing
    res="\e[32m[       OK ]"
    echo -e "\e[32m[ RUN      ]\e[0m $1"
    build/fcc $([ $fcc_loud == 0 ] && echo "-s" ]) tests/$1.c -o tests/$1
    result=$?
    if [ $result != 0 ] && [ -z "${negative_tests[$1]}" ]; then 
        res="\e[31m[  FAILED  ]"
    else
        passed_tests=$((passed_tests + 1))
    fi
    total_tests=$((total_tests + 1))
    echo -e "$res\e[0m $1"
}

./build.sh

if [ $? != 0 ]; then
    echo -e "\e[31mBuild failed!\e[0m"
    exit 1
fi

echo "==================================="
echo "               TESTS               "
echo "==================================="

for t in ${run_only[@]}; do
    check $t
done

echo "Result: $passed_tests/$total_tests tests passed!"

if [ $passed_tests != $total_tests ]; then
    exit 1
fi
