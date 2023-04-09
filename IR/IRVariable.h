#ifndef IRVARIABLE_H
#define IRVARIABLE_H

#include "label.h"
#include "../utils/common.h"

typedef u64 TemporaryID;

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

STRUCT(IRVariable, {
    OperandType type;
    union {
        u64 integer_value;
        u64 pointer_size;
        double double_value;
        float float_value;
        struct {
            TemporaryID temporary_id; // TODO(mdizdar): find a way to know which variable coincides with which temporary
            union {
                LabelID label_index;
                String label_name;
            };
            bool named;
        };
    };
    uintptr_t entry; // why am I doing this instead of just including SymbolTableEntry?
                     // is the idea that STE is C specific and this shouldn't be?
});

const char *IRVariable_toStr(IRVariable * const var, char *s);

#endif // IRVARIABLE_H
