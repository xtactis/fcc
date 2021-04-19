#ifndef IR_H
#define IR_H

#include "../C/symbol_table.h"

typedef enum {
    // single character Operations will just be their ascii value
    OP_ERROR = 256,
    
    OP_JUMP     = 300,
    OP_IF_JUMP = 301,
    OP_IF_ELSE = 302,
    OP_CALL    = 303,
    OP_RETURN  = 304,
    OP_DEREF   = 305,
    OP_ADDRESS = 306,
    OP_PLUS    = 307,
    OP_MINUS   = 308,
    OP_LABEL   = 309,
    
    OP_NOT_EQ         = 806,
    OP_EQUALS         = 810,
    OP_LESS_EQ        = 811,
    OP_GREATER_EQ     = 812,
    OP_LOGICAL_OR     = 813,
    OP_LOGICAL_AND    = 814,
    OP_BITSHIFT_LEFT  = 819,
    OP_BITSHIFT_RIGHT = 820,
} Op;

typedef enum {
    OT_NONE = 0,
    OT_VARIABLE = 1,
    OT_INTEGER = 2,
    OT_DOUBLE = 3,
    OT_FLOAT = 4,
    OT_TEMPORARY = 5,
    OT_LABEL = 6,
} OperandType;

typedef struct {
    union {
        struct variable {
            SymbolTableEntry *decl; // maybe?
            u64 id;
        };
        u64 temporary_id;
        u64 integer_value;
        double double_value;
        float float_value;
        struct {
            union {
                u64 label_index;
                String label_name;
            };
            bool named;
        };
    };
    OperandType type;
} IRVariable;

typedef struct {
    IRVariable result; // TODO(mdizdar): figure out how to store variables...
    IRVariable operands[2];
    
    Op instruction;
} IR;

const char *IRVariable_toStr(IRVariable * const var, char *s) {
    switch (var->type) {
        case OT_VARIABLE: {
            s = var->decl->name.data;
            break;
        }
        case OT_INTEGER: {
            sprintf(s, "%llu", var->integer_value);
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
            sprintf(s, "t%llu", var->temporary_id);
            break;
        }
        case OT_LABEL: {
            if (var->named) {
                s = var->label_name.data;
            } else {
                sprintf(s, "L%llu", var->label_index);
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

void IR_print(IR ** const ir, u64 size) {
#define TWO_OPERAND_OP(...) {                                         \
{                                                             \
char s[20];                                               \
printf("%s = ", IRVariable_toStr(&ir[i]->result, s));     \
}                                                             \
{                                                             \
char s[20];                                               \
printf("%s", IRVariable_toStr(&ir[i]->operands[0], s));   \
}                                                             \
printf(__VA_ARGS__);                                          \
{                                                             \
char s[20];                                               \
printf("%s\n", IRVariable_toStr(&ir[i]->operands[1], s)); \
}                                                             \
break;                                                        \
}
#define ONE_OPERAND_OP(...) {                                          \
{                                                              \
char s[20];                                                \
printf("%s = ", IRVariable_toStr(&ir[i]->result, s));      \
}                                                              \
printf(__VA_ARGS__);                                           \
{                                                              \
char s[20];                                                \
printf("%s\n", IRVariable_toStr(&ir[i]->operands[0], s));  \
}                                                              \
break;                                                         \
}
    for (u64 i = 0; i < size; ++i) {
        if (ir[i]->instruction == OP_LABEL) {
            char s[20];
            printf("%s:\n", IRVariable_toStr(&ir[i]->operands[0], s));
            continue;
        }
        printf("\t");
        switch ((int)ir[i]->instruction) {
            case OP_JUMP: {
                char s[20];
                printf("goto %s\n", IRVariable_toStr(&ir[i]->operands[0], s));
                break;
            }
            case OP_IF_JUMP: {
                {
                    char s[20];
                    printf("if %s ", IRVariable_toStr(&ir[i]->operands[0], s));
                }
                {
                    char s[20];
                    printf("goto %s\n", IRVariable_toStr(&ir[i]->operands[1], s));
                }
                break;
            }
            case OP_RETURN: {
                printf("return\n");
                break;
            }
            case OP_CALL: {
                char s[20];
                printf("call %s\n", IRVariable_toStr(&ir[i]->operands[0], s));
                break;
            }
            case '~': case '!': ONE_OPERAND_OP("%c", ir[i]->instruction);
            case '=':           ONE_OPERAND_OP("");
            case OP_PLUS:       ONE_OPERAND_OP("+");
            case OP_MINUS:      ONE_OPERAND_OP("-");
            case OP_DEREF:      ONE_OPERAND_OP("*");
            case OP_ADDRESS:    ONE_OPERAND_OP("&");
            case '*': case '/': case '%':
            case '|': case '&': case '^':
            case '+': case '-':     TWO_OPERAND_OP(" %c ", ir[i]->instruction);
            case OP_NOT_EQ:         TWO_OPERAND_OP(" != ");
            case OP_EQUALS:         TWO_OPERAND_OP(" == ");
            case OP_LESS_EQ:        TWO_OPERAND_OP(" <= ");
            case OP_GREATER_EQ:     TWO_OPERAND_OP(" >= ");
            case OP_LOGICAL_OR:     TWO_OPERAND_OP(" || ");
            case OP_LOGICAL_AND:    TWO_OPERAND_OP(" && ");
            case OP_BITSHIFT_LEFT:  TWO_OPERAND_OP(" << ");
            case OP_BITSHIFT_RIGHT: TWO_OPERAND_OP(" >> ");
            default: {
                printf("what in the god damn %d\n", ir[i]->instruction);
            }
        }
    }
#undef TWO_OPERAND_OP
#undef ONE_OPERAND_OP
}

#endif // IR_H