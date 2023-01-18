#ifndef LABEL_H
#define LABEL_H

#include "../utils/common.h"

typedef struct {
    union {
        u64 label_index;
        String label_name;
    };
    u64 ir_index;
    u32 correct_address;
    bool named;
} Label;

#endif // LABEL_H
