#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "label.h"
#include "IR.h"
#include "../utils/common.h"

STRUCT_DECLARATION(BasicBlock);

BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label);
BasicBlock *makeBasicBlock(IRArray *ir, u64 index, LabelArray *labels);
void makeBasicBlocks(IRArray *ir, LabelArray *labels);
BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label);
void addHelper(IRVariableArray *vars, IRVariable *var, bool *changed);
void addVariable(IRVariableArray *vars, IRVariable *var, bool *changed);
void removeVariable(IRVariableArray *vars, IRVariable *var, bool *changed);
void livenessAnalysisOneBlock(IRArray *ir, BasicBlock *block, IRVariableArray *liveVars);
void livenessAnalysis(IRArray *ir);

#endif // BASIC_BLOCK_H
