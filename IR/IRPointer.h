#ifndef IRPOINTER_H
#define IRPOINTER_H

#include "../utils/common.h"

struct IRVariable;
typedef struct IRVariable IRVariable;

STRUCT_HEADER(IRPointer, {
    IRVariable *reference_var;
    u64 offset;
});

#endif //IRPOINTER_H
