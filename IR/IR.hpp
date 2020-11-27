#ifndef IR_H
#define IR_H

#include "../utils/types.h"

enum class IRt {
    nop = 0,
    mov, movrm, movcm, movmr, movmm,
    AND, OR, XOR, add, sub, mul, div, mod, 
    NOT, neg, inc, dec, ror, rol, lsr, asr, lsl,  // .. expand
    jmp, jmpz, jmpnz, jmpgz, jmpeq, jmpne, jmplt, jmple, // .. expand
    call, ret, retr, retm, retc,
    pop, push,
    cmp
};

struct IR {
    IRt instruction;
    u32 operands[3]; // TODO(mdizdar): find a better way to handle multivariate instructions; having to carry around heavy empty operands for ops that don't need it is bad, malloc-ing is annoying (but may be the only option)... handle it
    u8 imm; // NOTE(mdizdar): a bitset where the i-th element corresponds to the i-th element in operands; 0: register, 1: immediate value
};

#endif // IR_H