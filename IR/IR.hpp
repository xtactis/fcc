using IR = uint32_t;

enum class IRt {
    nop = 0,
    movrr, movrm, movcr, movcm, movmr, movmm,
    AND, OR, XOR, add, sub, mul, div, mod, 
    NOT, neg, inc, dec, ror, rol, lsr, asr, lsl,  // .. expand
    jmp, jmpz, jmpnz, jmpgz, jmpeq, jmpne, jmplt, jmple, // .. expand
    call, ret, retr, retm, retc,
    pop, push,
    cmp
};
