#ifndef IR_H
#define IR_H

#include "../utils/common.h"
#include "../C/symbol_table.h"
#include "IRVariable.h"
#include "label.h"
#include "basic_block.h"

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

STRUCT_HEADER(IR, {
    struct BasicBlock *block;
    IRVariableArray liveVars;
    
    IRVariable result;
    IRVariable operands[2];

    Op instruction;
});


void IR_saveOne(IR *ir, FILE *fp, char *newline);
void IR_save(const IRArray *generated_IR, char *outfile);
void IR_print(IR * const ir, u64 size);
// Returns new IRArray with no Phi functions, destructs the original IRArray
IRArray IR_resolve_phi(IRArray *ir, LabelArray *labels);
LabelArray findLabels(IRArray *ir);

#endif // IR_H
