#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "../IR/label.h"
#include "../IR/IR.h"
#include "../utils/common.h"

static u64 basic_block_index = 0;

STRUCT(BasicBlock, {
    bool livenessDone;
    
    u64 id;
    u64 begin;
    u64 end;
    struct BasicBlock *jump;
    struct BasicBlock *next;
    
    struct BasicBlockArray in_blocks;
});

BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label);

BasicBlock *makeBasicBlock(IRArray *ir, u64 index, LabelArray *labels) {
    BasicBlock *bb = malloc(sizeof(BasicBlock));
    bb->begin = index;
    bb->jump = NULL;
    bb->next = NULL;
    bb->id = basic_block_index++;
    BasicBlockArray_construct(bb->in_blocks);
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
            BasicBlockArray_push_ptr(&bb->jump->in_blocks, &bb);
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
                BasicBlockArray_push_ptr(&bb->next->in_blocks, &bb);
            }
            BasicBlockArray_push_ptr(&bb->jump->in_blocks, &bb);
            return bb;
        } else if (i != index && irs[i].instruction == OP_LABEL) {
            bb->next = findBasicBlock(ir, i, labels, NULL);
            BasicBlockArray_push_ptr(&bb->next->in_blocks, &bb);
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
    FOR_EACH (IR, it, ir) {
        ++i;
        it->liveVars.element_size = sizeof(IRVariable);
        it->liveVars.count = 0;
        it->liveVars.capacity = 0;
        if (it->block) continue;
        makeBasicBlock(ir, i, labels);
    }
}

BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label) {
    if (label) {
        // looking for a label, not an index
        FOR_EACH (Label, it, LabelArray) {
            if (label->named != it->named) continue;
            if (it->named) {
                if (strcmp(label->label_name.data, it->label_name.data)) continue;
            } else {
                if (label->label_index != it->label_index) continue;
            }
            // found it
            if (irs[it->ir_index].block) {
                return irs[it->ir_index].block;
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

void addHelper(IRVariableArray *vars, IRVariable *var, bool *changed) {
    FOR_EACH (IRVariable, it, vars) {
        if (it->type == var->type && it->temporary_id == var->temporary_id) return;
    }
    *changed = true;
    IRVariableArray_push_ptr(vars, var);
}

void addVariable(IRVariableArray *vars, IRVariable *var, bool *changed) {
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

void removeVariable(IRVariableArray *vars, IRVariable *var, bool *changed) {
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
    IRVariableArray_erase(vars, i);
}

void livenessAnalysisOneBlock(IRArray *ir, BasicBlock *block, IRVariableArray *liveVars) {
    bool changed = false;
    IR *irs = ir->data;
    for (u64 i = block->end; i >= block->begin && i <= block->end; --i) {
        IRVariableArray notLive;
        IRVariableArray_construct(&notLive);
        IRVariable *live = liveVars->data;
        FOR_EACH (IRVariable, it, liveVars) {
            addVariable(&irs[i].liveVars, it, &changed);
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
        FOR_EACH (IRVariable, nl, &notLive) {
            removeVariable(&irs[i].liveVars, &nl[j], &changed);
        }
        
        IRVariableArray_clear(liveVars); // this obviously shouldn't stay
        FOR_EACH (IRVariable, l, &irs[i].liveVars) {
            IRVariableArray_push_ptr(liveVars, l);
        }
    }
    if (!changed) return;
    block->livenessDone = true;
    IRVariableArray liveCopy;
    IRVariableArray_construct(liveCopy);
    FOR_EACH (BasicBlockPtr, in_block, &blocks->in_blocks) {
        IRVariableArray_copy(&liveCopy, liveVars);
        livenessAnalysisOneBlock(ir, in_block, &liveCopy);
    }
    IRVariableArray_destruct(liveCopy);
}

void livenessAnalysis(IRArray *ir) {
    IRVariableArray liveVars;
    IRVariableArray_construct(liveVars);

    FOR_EACH_REV (IR, it, ir) {
        if (it->block->livenessDone) continue;
        IRVariableArray_clear(liveVars);
        livenessAnalysisOneBlock(ir, it->.block, &liveVars);
    }

    IRVariableArray_destruct(liveVars);
}

#endif // BASIC_BLOCK_H
