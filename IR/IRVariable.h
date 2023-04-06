#ifndef IRVARIABLE_H
#define IRVARIABLE_H

#include "../utils/common.h"

typedef enum {
    OT_NONE = 0,
    OT_VARIABLE = 1,
    
    OT_INT8  = 2,
    OT_INT16 = 3,
    OT_INT32 = 4,
    OT_INT64 = 5,
    
    OT_POINTER = 6,
    
    OT_DOUBLE = 7,
    OT_FLOAT  = 8,
    
    OT_TEMPORARY = 9,
    OT_LABEL = 10,
    OT_VALUE = 11,
    
    OT_SIZE = 12,

    OT_PHI_VAR = 13,
} OperandType;

STRUCT_DECLARATION(IRVariable);

const char *IRVariable_toStr(IRVariable * const var, char *s);

#endif // IRVARIABLE_H
