#include "label.h"

LabelID label_index = 0; // global

bool Label_eq(const Label *a, const Label *b) {
    if (a->named != b->named) return false;
    if (a->named) {
        return String_eq(&a->label_name, &b->label_name);
    }
    return a->label_index == b->label_index;
}
