#ifndef COMMON_H
#define COMMON_H

#include "printing.h"
#include "types.h"
#include "dyn_array.h"

#ifndef _MSC_VER
u64 max(u64 a, u64 b) {
    return a > b ? a : b;
}

u64 min(u64 a, u64 b) {
    return a < b ? a : b;
}
#endif

STRUCT(String, {
    char *data;
    u64 count;
});

#endif // COMMON_H
