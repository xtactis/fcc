#ifndef IR_H
#define IR_H

#include "../C/symbol_table.h"
#include "label.h"
#include "../utils/dyn_array.h"

typedef u64 TemporaryID;

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

STRUCT(IRPointer, {
    IRVariable *reference;
    u64 offset;
});

STRUCT_DECLARATION(IR);

void IR_saveOne(IR *ir, FILE *fp, char *newline);
void IR_save(const IRArray *generated_IR, char *outfile);
void IR_print(IR * const ir, u64 size);
// Returns new IRArray with no Phi functions, destructs the original IRArray
IRArray IR_resolve_phi(IRArray *ir, LabelArray *labels);

// TODO(mdizdar): I can't keep shuffling functions like this
// the solution is using header and source files
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


#endif // IR_H
