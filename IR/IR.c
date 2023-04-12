#include "IR.h"

TemporaryID temporary_index = 0; // global
                                 //
STRUCT(IR);

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

IRArray IR_resolve_phi(IRArray *ir, LabelArray *labels) {
    IRArray new_ir;
    IRArray_construct(&new_ir);
    IRArray_copy(&new_ir, ir);
    for (ARRAY_EACH(IR, it, ir)) {
        if (it->instruction == OP_PHI) {
            Line left_index = -1;
            Line right_index = -1;
            for (ARRAY_EACH(Label, label, labels)) {
                if (label->named) continue;
                if (it->operands[0].label_index == label->label_index) {
                    left_index = label->ir_index;
                } else if (it->operands[1].label_index == label->label_index) {
                    right_index = label->ir_index;
                }
                if (left_index != (Line)-1 && right_index != (Line)-1) {
                    break;
                }
            }
            // NOTE (mdizdar): we want to be inserting BEFORE the jump at the end of the block
            if (IRArray_at(&new_ir, left_index-1)->instruction == OP_JUMP) {
                left_index -= 1;
            }
            if (IRArray_at(&new_ir, right_index-1)->instruction == OP_JUMP) {
                right_index -= 1;
            }
            IR left_mov = {
                .instruction = '=',
                .result = it->result,
                .operands[0] = {
                    .type = OT_TEMPORARY,
                    .entry = it->operands[0].entry,
                    .temporary_id = it->operands[0].temporary_id,
                },
            };
            IRArray_insert_ptr(&new_ir, &left_mov, left_index);
            if (right_index > left_index) ++right_index;
            IR right_mov = {
                .instruction = '=',
                .result = it->result,
                .operands[0] = {
                    .type = OT_TEMPORARY,
                    .entry = it->operands[1].entry,
                    .temporary_id = it->operands[1].temporary_id,
                },
            };
            IRArray_insert_ptr(&new_ir, &right_mov, right_index);
            for (ARRAY_EACH(Label, label, labels)) {
                if (label->ir_index > left_index) {
                    ++label->ir_index;
                }
                if (label->ir_index > right_index) {
                    ++label->ir_index;
                }
            }
        }
    }
    IR *result = IRArray_begin(&new_ir);
    u64 deleted = 0;
    for (ARRAY_EACH(IR, it, &new_ir)) {
        if (it->instruction != OP_PHI) {
            *result = *it;
            ++result;
        } else {
            ++deleted;
        }
    }
    new_ir.count -= deleted;
    IRArray_shrink_to_fit(&new_ir);

    LabelArray_destruct(labels);
    *labels = findLabels(&new_ir);

    IRArray_destruct(ir);
    return new_ir;
}

LabelArray findLabels(IRArray *ir) {
    LabelArray labels;
    LabelArray_construct(&labels);
    
    u64 i = 0;
    for (ARRAY_EACH(IR, it, ir)) {
        if (it->instruction == OP_LABEL) {
            Label newlabel;
            newlabel.named = it->operands[0].named;
            if (it->operands[0].named) {
                newlabel.label_name = it->operands[0].label_name;
            } else {
                newlabel.label_index = it->operands[0].label_index;
            }
            newlabel.ir_index = i;
            LabelArray_push_back(&labels, newlabel);
        }
        ++i;
    }
    return labels;
}
