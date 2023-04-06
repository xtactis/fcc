#ifndef LABEL_H
#define LABEL_H

#include "../utils/common.h"

typedef u64 LabelID;

STRUCT_DECLARATION(Label);

bool Label_eq(const Label *a, const Label *b);

#endif // LABEL_H
