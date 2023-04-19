#include "liveness_analysis.h"

// FIXME(mdizdar): half of this file seems to be treating an array as a hash table, make it an actual hash table :)
void addHelper(IRVariableArray *vars, IRVariable *var, bool *changed) {
    for (ARRAY_EACH(IRVariable, it, vars)) {
        if (it->type == var->type && it->temporary_id == var->temporary_id) return;
    }
    *changed = true;
    IRVariableArray_push_ptr(vars, var);
}

void addVariable(IRVariableArray *vars, IRVariable *var, bool *changed) {
    if (var->type == OT_TEMPORARY) {
        addHelper(vars, var, changed);
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
        for (ARRAY_EACH(IRVariable, it, liveVars)) {
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
                if (!((irs[i].operands[0].type == OT_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[0].temporary_id)) {
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
                if (!(((irs[i].operands[0].type == OT_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[0].temporary_id) || ((irs[i].operands[1].type == OT_TEMPORARY) && irs[i].result.temporary_id == irs[i].operands[1].temporary_id))) {
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
        for (ARRAY_EACH(IRVariable, nl, &notLive)) {
            removeVariable(&irs[i].liveVars, nl, &changed);
        }
        
        IRVariableArray_clear(liveVars); // this obviously shouldn't stay
        for (ARRAY_EACH(IRVariable, l, &irs[i].liveVars)) {
            IRVariableArray_push_ptr(liveVars, l);
        }
    }
    if (block->loop) return;
    if (!changed) return;
    block->livenessDone = true;
    block->loop = true;
    IRVariableArray liveCopy;
    IRVariableArray_construct(&liveCopy);
    for (ARRAY_EACH(BasicBlockPtr, in_block, block->in_blocks)) {
        IRVariableArray_copy(&liveCopy, liveVars);
        livenessAnalysisOneBlock(ir, *in_block, &liveCopy);
    }
    block->loop = false;
    IRVariableArray_destruct(&liveCopy);
}

void livenessAnalysis(IRArray *ir) {
    IRVariableArray liveVars;
    IRVariableArray_construct(&liveVars);

    for (ARRAY_EACH_REV(IR, it, ir)) {
        if (it->block->livenessDone) continue;
        IRVariableArray_clear(&liveVars);
        livenessAnalysisOneBlock(ir, it->block, &liveVars);
    }

    IRVariableArray_destruct(&liveVars);
}
