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

bool Label_eq(const Label *a, const Label *b) {
    if (a->named != b->named) return false;
    if (a->named) {
        return String_eq(&a->label_name, &b->label_name);
    }
    return a->label_index == b->label_index;
}

#endif // LABEL_H
