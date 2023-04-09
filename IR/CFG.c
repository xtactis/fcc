#include "CFG.h"

u64 basic_block_index = 0;

BasicBlock *makeBasicBlock(IRArray *ir, u64 index, LabelArray *labels) {
    BasicBlock *bb = malloc(sizeof(BasicBlock));
    bb->begin = index;
    bb->jump = NULL;
    bb->next = NULL;
    bb->id = basic_block_index++;
    bb->in_blocks = malloc(sizeof(BasicBlockPtrArray));
    BasicBlockPtrArray_construct(bb->in_blocks);
    bb->livenessDone = false;
    
    IR *irs = ir->data;
    irs[index].block = bb;
    for (u64 i = index; i < ir->count; ++i) {
        if (irs[i].instruction != OP_LABEL) {
            irs[i].block = bb;
        }
        if (irs[i].instruction == OP_JUMP) {
            bb->end = i;
            bb->jump = findBasicBlock(ir, i+1, labels, &irs[i].operands[0]);
            BasicBlockPtrArray_push_back(bb->jump->in_blocks, bb);
            return bb;
        } else if (irs[i].instruction == OP_IF_JUMP ||
                   irs[i].instruction == OP_IFN_JUMP) {
            bb->end = i;
            bb->next = findBasicBlock(ir, i+1, labels, NULL);
            bb->jump = findBasicBlock(ir, i+1, labels, &irs[i].operands[1]);
            if (bb->next->begin == bb->jump->begin) {
                free(bb->next);
                bb->next = NULL;
            } else {
                BasicBlockPtrArray_push_back(bb->next->in_blocks, bb);
            }
            BasicBlockPtrArray_push_back(bb->jump->in_blocks, bb);
            return bb;
        } else if (i != index && irs[i].instruction == OP_LABEL) {
            bb->next = findBasicBlock(ir, i, labels, NULL);
            BasicBlockPtrArray_push_back(bb->next->in_blocks, bb);
            bb->end = i-1;
            return bb;
        } else if (irs[i].instruction == OP_RETURN) {
            bb->end = i;
            return bb;
        }
    }
    bb->end = ir->count-1;
    return bb;
}

void makeBasicBlocks(IRArray *ir, LabelArray *labels) {
    int i = -1;
    for (ARRAY_EACH(IR, it, ir)) {
        ++i;
        IRVariableArray_construct(&it->liveVars);
        if (it->block) continue;
        makeBasicBlock(ir, i, labels);
    }
}

BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label) {
    if (label) {
        // looking for a label, not an index
        for (ARRAY_EACH(Label, it, labels)) {
            if (label->named != it->named) continue;
            if (it->named) {
                if (strcmp(label->label_name.data, it->label_name.data)) continue;
            } else {
                if (label->label_index != it->label_index) continue;
            }
            // found it
            if (IRArray_at(ir, it->ir_index)->block) {
                return IRArray_at(ir, it->ir_index)->block;
            } else {
                return makeBasicBlock(ir, it->ir_index, labels);
            }
        }
        // oops, didn't find it
        error(0, "nice label bro | %d %d | %lu", label->label_index, label->named, labels->count);
    } else {
        // looking for an index
        if (IRArray_at(ir, index)->block) {
            return IRArray_at(ir, index)->block;
        } else {
            return makeBasicBlock(ir, index, labels);
        }
    }
}
