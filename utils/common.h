#ifndef COMMON_H
#define COMMON_H

#include "printing.h"
#include "types.h"
#include "dyn_array.h"
#include "hash_map.h"
#include "string.h"

#ifndef _MSC_VER
u64 max(u64 a, u64 b) {
    return a > b ? a : b;
}

u64 min(u64 a, u64 b) {
    return a < b ? a : b;
}
#endif

#endif // COMMON_H
