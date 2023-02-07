#ifndef IR_H
#define IR_H

#include "../C/symbol_table.h"
#include "../utils/dyn_array.h"

typedef u64 TemporaryID;
typedef u64 LabelID;

TemporaryID temporary_index = 0;
LabelID     label_index     = 0;

typedef enum {
    // single character Operations will just be their ascii value
    OP_ERROR = 256,
    
    OP_JUMP     = 300,
    OP_IF_JUMP  = 301,
    OP_IFN_JUMP = 302,
    OP_CALL     = 303,
    OP_RETURN   = 304,
    OP_DEREF    = 305,
    OP_ADDRESS  = 306,
    OP_PLUS     = 307,
    OP_MINUS    = 308,
    OP_LABEL    = 309,
    
    OP_PHI   = 401,
    OP_LOAD  = 402,
    OP_STORE = 403,

    OP_NOT_EQ         = 806,
    OP_EQUALS         = 810,
    OP_LESS_EQ        = 811,
    OP_GREATER_EQ     = 812,
    OP_LOGICAL_OR     = 813,
    OP_LOGICAL_AND    = 814,
    OP_BITSHIFT_LEFT  = 819,
    OP_BITSHIFT_RIGHT = 820,
    
    OP_PRELUDE        = 900,
    OP_PUSH           = 901,
    OP_POP            = 902,
    OP_GET_RETURNED   = 903,
    OP_GET_ARG        = 904,
} Op;

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
    OT_DEREF_TEMPORARY = 10,
    OT_LABEL = 11,
    OT_VALUE = 12,
    
    OT_SIZE = 13,

    OT_PHI_VAR = 14,
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

struct BasicBlock;

STRUCT(IR, {
    struct BasicBlock *block;
    IRVariableArray liveVars;
    
    IRVariable result;
    IRVariable operands[2];

    Op instruction;
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
        case OT_DEREF_TEMPORARY: {
            sprintf(s, "*t%lu", var->temporary_id);
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

void IR_saveOne(IR *ir, FILE *fp, char *newline) {
#define TWO_OPERAND_OP(...) {                                         \
fprintf(fp, "%s = ", IRVariable_toStr(&ir->result, s));     \
fprintf(fp, "%s", IRVariable_toStr(&ir->operands[0], s));   \
fprintf(fp, __VA_ARGS__);                                          \
fprintf(fp, "%s%s", IRVariable_toStr(&ir->operands[1], s), newline); \
break;                                                        \
}
#define ONE_OPERAND_OP(...) {                                          \
fprintf(fp, "%s = ", IRVariable_toStr(&ir->result, s));      \
fprintf(fp, __VA_ARGS__);                                           \
fprintf(fp, "%s%s", IRVariable_toStr(&ir->operands[0], s), newline);  \
break;                                                         \
}
    char s[40], q[40];
    if (ir->instruction == OP_LABEL) {
        fprintf(fp, "%s:%s", IRVariable_toStr(&ir->operands[0], s), newline);
        return;
    }
    fprintf(fp, "\t");
    switch ((int)ir->instruction) {
        case OP_JUMP: {
            fprintf(fp, "goto %s%s", IRVariable_toStr(&ir->operands[0], s), newline);
            break;
        }
        case OP_PUSH: {
            if (ir->operands[0].type != OT_NONE) {
                fprintf(fp, "push %s (%lu)%s", IRVariable_toStr(&ir->operands[0], s), ir->operands[1].integer_value, newline);
            } else {
                fprintf(fp, "push (%lu)%s", ir->operands[1].integer_value, newline);
            }
            break;
        }
        case OP_POP: {
            if (ir->operands[0].type != OT_NONE) {
                fprintf(fp, "pop %s (%lu)%s", IRVariable_toStr(&ir->operands[0], s), ir->operands[1].integer_value, newline);
            } else {
                fprintf(fp, "pop (%lu)%s", ir->operands[1].integer_value, newline);
            }
            break;
        }
        case OP_GET_ARG: {
            fprintf(fp, "%s = arg #%s%s", IRVariable_toStr(&ir->result, s), IRVariable_toStr(&ir->operands[0], q), newline);
            break;
        }
        case OP_GET_RETURNED: {
            fprintf(fp, "%s = returned%s", IRVariable_toStr(&ir->result, s), newline);
            break;
        }
        case OP_PRELUDE: {
            fprintf(fp, "prelude %s%s", IRVariable_toStr(&ir->operands[0], s), newline);
            break;
        }
        case OP_IF_JUMP: {
            fprintf(fp, "if %s ", IRVariable_toStr(&ir->operands[0], s));
            fprintf(fp, "goto %s%s", IRVariable_toStr(&ir->operands[1], s), newline);
            break;
        }
        case OP_IFN_JUMP: {
            fprintf(fp, "if not %s ", IRVariable_toStr(&ir->operands[0], s));
            fprintf(fp, "goto %s%s", IRVariable_toStr(&ir->operands[1], s), newline);
            break;
        }
        case OP_RETURN: {
            if (ir->operands[0].type == OT_NONE) {
                fprintf(fp, "return%s", newline);
            } else {
                fprintf(fp, "return %s%s", IRVariable_toStr(&ir->operands[0], s), newline);
            }
            break;
        }
        case OP_CALL: {
            fprintf(fp, "call %s%s", IRVariable_toStr(&ir->operands[0], s), newline);
            break;
        }
        case OP_PHI: {
            fprintf(fp, "%s = phi ", IRVariable_toStr(&ir->result, s));
            fprintf(fp, "%s ", IRVariable_toStr(&ir->operands[0], s));
            fprintf(fp, "%s%s", IRVariable_toStr(&ir->operands[1], s), newline);
            break;
        }
        case OP_STORE: {
            fprintf(fp, "store %s -> ", IRVariable_toStr(&ir->operands[0], s));
            fprintf(fp, "%s%s", IRVariable_toStr(&ir->operands[1], s), newline);
            break;
        }
        case OP_LOAD: ONE_OPERAND_OP("load ");
        case '~': case '!': ONE_OPERAND_OP("%c", ir->instruction);
        case '=':           ONE_OPERAND_OP("%s", ""); // This is a stupid way of getting rid of a warning, I just want an empty string since there is no prefix operator
        case OP_PLUS:       ONE_OPERAND_OP("+");
        case OP_MINUS:      ONE_OPERAND_OP("-");
        case OP_DEREF:      ONE_OPERAND_OP("*");
        case OP_ADDRESS:    ONE_OPERAND_OP("&");
        case '*': case '/': case '%':
        case '|': case '&': case '^':
        case '+': case '-':
        case '<': case '>':     TWO_OPERAND_OP(" %c ", ir->instruction);
        case OP_NOT_EQ:         TWO_OPERAND_OP(" != ");
        case OP_EQUALS:         TWO_OPERAND_OP(" == ");
        case OP_LESS_EQ:        TWO_OPERAND_OP(" <= ");
        case OP_GREATER_EQ:     TWO_OPERAND_OP(" >= ");
        case OP_LOGICAL_OR:     TWO_OPERAND_OP(" || ");
        case OP_LOGICAL_AND:    TWO_OPERAND_OP(" && ");
        case OP_BITSHIFT_LEFT:  TWO_OPERAND_OP(" << ");
        case OP_BITSHIFT_RIGHT: TWO_OPERAND_OP(" >> ");
        default: {
            fprintf(fp, "what in the god damn %d%s", ir->instruction, newline);
        }
    }
#undef TWO_OPERAND_OP
#undef ONE_OPERAND_OP
}

void IR_save(const IRArray *generated_IR, char *outfile) {
    u64 len = strlen(outfile);
    char *of = malloc(sizeof(char) * len+5);
    strcpy(of, outfile);
    strcat(of, ".ir");
    FILE *fp = fopen(of, "w");
    
    for (ARRAY_EACH(IR, it, generated_IR)) {
        IR_saveOne(it, fp, "\n");
    }
    fclose(fp);
}

void IR_print(IR * const ir, u64 size) {
    for (u64 i = 0; i < size; ++i) {
        printf("%3lu: ", i);
        IR_saveOne(&ir[i], stdout, "\n");
    }
}

#endif // IR_H
