#ifndef CFG_H
#define CFG_H

#include "IR.h"
#include "IRVariable.h"
#include "basic_block.h"
#include "label.h"

BasicBlock *findBasicBlock(IRArray *ir, u64 index, LabelArray *labels, IRVariable *label);
BasicBlock *makeBasicBlock(IRArray *ir, u64 index, LabelArray *labels);
void makeBasicBlocks(IRArray *ir, LabelArray *labels);

#endif // CFG_H
