#ifndef IRPOINTER_H
#define IRPOINTER_H

#include "../utils/common.h"
#include "IRVariable.h"

STRUCT_HEADER(IRPointer, {
    IRVariable *reference;
    u64 offset;
});

#endif //IRPOINTER_H
