#ifndef LABEL_H
#define LABEL_H

#include "../utils/common.h"

typedef u64 LabelID;

STRUCT(Label, {
    union {
        u64 label_index;
        String label_name;
    };
    u64 ir_index;
    u32 correct_address;
    bool named;
});

bool Label_eq(const Label *a, const Label *b);

#endif // LABEL_H
