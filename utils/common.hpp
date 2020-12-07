#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include "printing.h"
#include "types.h"

struct String {
    char *str  = NULL;
    u64 length = 0;
};

#endif // COMMON_H