#ifndef LIVENESS_ANALYSIS_H
#define LIVENESS_ANALYSIS_H

#include "IRVariable.h"
#include "IR.h"
#include "basic_block.h"

void addHelper(IRVariableArray *vars, IRVariable *var, bool *changed);
void addVariable(IRVariableArray *vars, IRVariable *var, bool *changed);
void removeVariable(IRVariableArray *vars, IRVariable *var, bool *changed);
void livenessAnalysisOneBlock(IRArray *ir, BasicBlock *block, IRVariableArray *liveVars);
void livenessAnalysis(IRArray *ir);

#endif // LIVENESS_ANALYSIS_H
