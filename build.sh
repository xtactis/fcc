#!/bin/bash
mkdir -p build
pushd build
time gcc -std=c17 -Wall -Wextra -O2 ../main.c -o fcc
popd # build

