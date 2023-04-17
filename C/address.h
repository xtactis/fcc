#ifndef ADDRESS_H
#define ADDRESS_H

#include "../utils/common.h"

STRUCT_HEADER(Address, {
    bool global;
    u64 offset;
});

#endif //ADDRESS_H
