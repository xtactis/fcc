#include "IRVariable.h"

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

const char *IRVariable_toStr(IRVariable * const var, char *s) {
    switch (var->type) {
        case OT_VARIABLE: {
            sprintf(s, "%s", ((SymbolTableEntry *)var->entry)->name.data);
            break;
        }
        case OT_INT8: {
            sprintf(s, "%u", (u8)var->integer_value);
            break;
        }
        case OT_INT16: {
            sprintf(s, "%u", (u16)var->integer_value);
            break;
        }
        case OT_INT32: {
            sprintf(s, "%u", (u32)var->integer_value);
            break;
        }
        case OT_INT64: {
            sprintf(s, "%lu", var->integer_value);
            break;
        }
        case OT_DOUBLE: {
            sprintf(s, "%.7lf", var->double_value);
            break;
        }
        case OT_FLOAT: {
            sprintf(s, "%.7ff", var->float_value);
            break;
        }
        case OT_TEMPORARY: {
            if (var->entry != 0) {
                sprintf(s, "%s_%lu", ((SymbolTableEntry *)var->entry)->name.data, var->temporary_id);
            } else {
                sprintf(s, "t%lu", var->temporary_id);
            }
            break;
        }
        case OT_LABEL: {
            if (var->named) {
                s = var->label_name.data;
            } else {
                sprintf(s, "L%lu", var->label_index);
            }
            break;
        }
        case OT_SIZE: {
            sprintf(s, "(%lu)", var->integer_value);
            break;
        }
        case OT_PHI_VAR: {
            if (var->entry != 0) {
                sprintf(s, "[%s_%lu from before L%lu]", ((SymbolTableEntry *)var->entry)->name.data, var->temporary_id, var->label_index);
            } else {
                sprintf(s, "[t%lu from before L%lu]", var->temporary_id, var->label_index);
            }
            break;
        }
        default: {
            //error(0, "OT_NONE isn't printable my guy");
            sprintf(s, "wtf %d", var->type);
        }
    }
    return s;
}
