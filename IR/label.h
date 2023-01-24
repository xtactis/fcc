#ifndef LABEL_H
#define LABEL_H

#include "../utils/common.h"

STRUCT(Label, {
    union {
        u64 label_index;
        String label_name;
    };
    u64 ir_index;
    u32 correct_address;
    bool named;
});

#endif // LABEL_H
