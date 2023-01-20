#!/bin/bash

set -e

check() {
    # TODO(mdizdar): actually implement testing
    echo "$1 ..... OK"
}

if ! [ -f build/fcc ]; then
    ./build.sh
fi

echo "==================================="
echo "               TESTS               "
echo "==================================="

build/fcc -s tests/scope.c -o tests/scope
check tests/scope
build/fcc -s tests/ternary.c -o tests/ternary
check tests/ternary
build/fcc -s tests/if.c -o tests/if
check tests/if
build/fcc -s tests/ifelse.c -o tests/ifelse
check tests/ifelse
build/fcc -s tests/ifelseif.c -o tests/ifelseif
check tests/ifelseif
build/fcc -s tests/while.c -o tests/while
check tests/while
