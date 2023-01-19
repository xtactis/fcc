#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "../IR/label.h"
#include "../IR/IR.h"
#include "../utils/common.h"

static u64 basic_block_index = 0;

typedef struct BasicBlock {
    bool livenessDone;
    
    u64 id;
    u64 begin;
    u64 end;
    struct BasicBlock *jump;
    struct BasicBlock *next;
    
    DynArray in_blocks;
} BasicBlock;

BasicBlock *findBasicBlock(DynArray *ir, u64 index, DynArray *labels, IRVariable *label);

BasicBlock *makeBasicBlock(DynArray *ir, u64 index, DynArray *labels) {
    BasicBlock *bb = malloc(sizeof(BasicBlock));
    bb->begin = index;
    bb->jump = NULL;
    bb->next = NULL;
    bb->id = basic_block_index++;
    bb->in_blocks.element_size = sizeof(BasicBlock *);
    bb->in_blocks.capacity = 0;
    bb->in_blocks.count = 0;
    bb->livenessDone = false;
    
    IR *irs = (IR *)(ir->data);
    irs[index].block = bb;
    for (u64 i = index; i < ir->count; ++i) {
        if (irs[i].instruction != OP_LABEL) {
            irs[i].block = bb;
        }
        if (irs[i].instruction == OP_JUMP) {
            bb->end = i;
            bb->jump = findBasicBlock(ir, i+1, labels, &irs[i].operands[0]);
            DynArray_add(&bb->jump->in_blocks, &bb);
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
                DynArray_add(&bb->next->in_blocks, &bb);
            }
            DynArray_add(&bb->jump->in_blocks, &bb);
            return bb;
        } else if (i != index && irs[i].instruction == OP_LABEL) {
            bb->next = findBasicBlock(ir, i, labels, NULL);
            DynArray_add(&bb->next->in_blocks, &bb);
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

void makeBasicBlocks(DynArray *ir, DynArray *labels) {
    IR *irs = (IR *)(ir->data);
    for (u64 i = 0; i < ir->count; ++i) {
        irs[i].liveVars.element_size = sizeof(IRVariable);
        irs[i].liveVars.count = 0;
        irs[i].liveVars.capacity = 0;
        if (irs[i].block) continue;
        makeBasicBlock(ir, i, labels);
    }
}

BasicBlock *findBasicBlock(DynArray *ir, u64 index, DynArray *labels, IRVariable *label) {
    IR *irs = (IR *)(ir->data);
    if (label) {
        // looking for a label, not an index
        Label *ls = (Label *)labels->data;
        for (u64 i = 0; i < labels->count; ++i) {
            if (label->named != ls[i].named) continue;
            if (ls[i].named) {
                if (strcmp(label->label_name.data, ls[i].label_name.data)) continue;
            } else {
                if (label->label_index != ls[i].label_index) continue;
            }
            // found it
            if (irs[ls[i].ir_index].block) {
                return irs[ls[i].ir_index].block;
            } else {
                return makeBasicBlock(ir, ls[i].ir_index, labels);
            }
        }
        // oops, didn't find it
        error(0, "nice label bro | %d %d | %lu", label->label_index, label->named, labels->count);
    } else {
        // looking for an index
        if (irs[index].block) {
            return irs[index].block;
        } else {
            return makeBasicBlock(ir, index, labels);
        }
    }
}

void addHelper(DynArray *vars, IRVariable *var, bool *changed) {
    IRVariable* vs = vars->data;
    for (u64 i = 0; i < vars->count; ++i) {
        if (vs[i].type == var->type && vs[i].temporary_id == var->temporary_id) return;
    }
    *changed = true;
    DynArray_add(vars, var);
}

void addVariable(DynArray *vars, IRVariable *var, bool *changed) {
    if (var->type == OT_TEMPORARY) {
        addHelper(vars, var, changed);
    } else if (var->type == OT_DEREF_TEMPORARY) {
        IRVariable *derefd = malloc(sizeof(IRVariable));
        derefd->type = OT_TEMPORARY;
        derefd->entry = 0;
        derefd->temporary_id = var->temporary_id;
        addHelper(vars, derefd, changed);
    }
}

void removeVariable(DynArray *vars, IRVariable *var, bool *changed) {
    if (vars->count == 0) return;
    IRVariable *vs = vars->data;
    u64 i = 0;
    bool found = false;
    for (; i < vars->count; ++i) {
        if (vs[i].type == var->type && vs[i].temporary_id == var->temporary_id) {
            found = true;
            break;
        }
    }
    if (!found) return;
    *changed = true;
    --vars->count;
    if (vars->count - i == 0) return;
    memcpy((u8*)vars->data + i*vars->element_size, 
           (u8*)vars->data + (i+1)*vars->element_size, 
           vars->element_size * (vars->count - i));
}

void livenessAnalysisOneBlock(DynArray *ir, BasicBlock *block, DynArray *liveVars) {
    bool changed = false;
    IR *irs = (IR *)(ir->data);
    for (u64 i = block->end; i >= block->begin && i <= block->end; --i) {
        DynArray notLive;
        notLive.data = NULL;
        notLive.element_size = sizeof(IRVariable);
        notLive.count = 0;
        notLive.capacity = 0;
        IRVariable *live = liveVars->data;
        for (u64 j = 0; j < liveVars->count; ++j) {
            addVariable(&irs[i].liveVars, &live[j], &changed);
        }
        switch ((int)irs[i].instruction) {
            case OP_POP: {
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    addVariable(&notLive, &irs[i].operands[0], &changed);
                } else {
                    addVariable(&irs[i].liveVars, &irs[i].operands[0], &changed);
                }
                break;
            }
            case OP_PUSH: case OP_RETURN: {
                addVariable(&irs[i].liveVars, &irs[i].operands[0], &changed);
                break;
            }
            case OP_GET_RETURNED: case OP_GET_ARG: {
                if (irs[i].result.type == OT_TEMPORARY) {
                    addVariable(&notLive, &irs[i].result, &changed);
                } else {
                    addVariable(&irs[i].liveVars, &irs[i].result, &changed);
                }
                break;
            }
            case OP_LABEL: case OP_JUMP: case OP_ERROR: case OP_PRELUDE: case OP_CALL: {
                // do nothing
                break;
            }
            case '=': case '~': case '!': case OP_PLUS: case OP_MINUS: case OP_ADDRESS: {
                if (!((irs[i].operands[0].type == OT_TEMPORARY || irs[i].operands[0].type == OT_DEREF_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[0].temporary_id)) {
                    if (irs[i].result.type == OT_TEMPORARY) {
                        addVariable(&notLive, &irs[i].result, &changed);
                    } else {
                        addVariable(&irs[i].liveVars, &irs[i].result, &changed);
                    }
                }
                addVariable(&irs[i].liveVars, &irs[i].operands[0], &changed);
                break;
            }
            case OP_IF_JUMP: case OP_IFN_JUMP: {
                addVariable(&irs[i].liveVars, &irs[i].operands[0], &changed);
                break;
            }
            default: {
                if (!(((irs[i].operands[0].type == OT_TEMPORARY || irs[i].operands[0].type == OT_DEREF_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[0].temporary_id) || ((irs[i].operands[1].type == OT_TEMPORARY || irs[i].operands[1].type == OT_DEREF_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[1].temporary_id))) {
                    if (irs[i].result.type == OT_TEMPORARY) {
                        addVariable(&notLive, &irs[i].result, &changed);
                    } else {
                        addVariable(&irs[i].liveVars, &irs[i].result, &changed);
                    }
                }
                addVariable(&irs[i].liveVars, &irs[i].operands[0], &changed);
                addVariable(&irs[i].liveVars, &irs[i].operands[1], &changed);
            }
        }
        IRVariable *nl = notLive.data;
        for (u64 j = 0; j < notLive.count; ++j) {
            removeVariable(&irs[i].liveVars, &nl[j], &changed);
        }
        
        liveVars->count = 0;
        IRVariable *l = irs[i].liveVars.data;
        for (u64 j = 0; j < irs[i].liveVars.count; ++j){
            DynArray_add(liveVars, &l[j]);
        }
    }
    if (!changed) return;
    block->livenessDone = true;
    DynArray liveCopy;
    liveCopy.data = NULL;
    liveCopy.capacity = 0;
    liveCopy.element_size = sizeof(IRVariable);
    BasicBlock **bbs = block->in_blocks.data;
    IRVariable *l = liveVars->data;
    for (u64 i = 0; i < block->in_blocks.count; ++i) {
        liveCopy.count = 0;
        for (u64 j = 0; j < liveVars->count; ++j) {
            DynArray_add(&liveCopy, &l[j]);
        }
        livenessAnalysisOneBlock(ir, bbs[i], &liveCopy);
    }
    if (liveCopy.data) {
        free(liveCopy.data);
    }
}

void livenessAnalysis(DynArray *ir) {
    DynArray liveVars;
    liveVars.element_size = sizeof(IRVariable);
    liveVars.capacity = 0;

    IR *irs = (IR *)(ir->data);
    for (u64 i = ir->count-1; i > 0; --i) {
        if (irs[i].block->livenessDone) continue;
        liveVars.count = 0;
        livenessAnalysisOneBlock(ir, irs[i].block, &liveVars);
    }
}

#endif // BASIC_BLOCK_H
