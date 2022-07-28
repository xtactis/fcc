#ifndef COMMON_H
#define COMMON_H

#include "printing.h"
#include "types.h"

#ifndef _MSC_VER
u64 max(u64 a, u64 b) {
    return a > b ? a : b;
}
#endif

typedef struct {
    char *data;
    u64 count;
} String;

#endif // COMMON_H