#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "label.h"
#include "IR.h"
#include "IRVariable.h"
#include "../utils/common.h"

STRUCT_HEADER(BasicBlock, {
    bool livenessDone;
    
    u64 id;
    u64 begin;
    u64 end;
    struct BasicBlock *jump;
    struct BasicBlock *next;
    
    struct BasicBlockPtrArray *in_blocks;
});


#endif // BASIC_BLOCK_H
