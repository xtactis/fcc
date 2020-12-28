/* date = December 28th 2020 10:08 pm */

#ifndef RESERVED_H
#define RESERVED_H

const char * const KEYWORDS[] = {
    "if", "while", "do", "for", "switch", "case",
    "break", "continue", "else", "struct", "typedef", 
    "union", "enum", "return", "goto", "default",
    "sizeof", "extern", "inline", "restrict", "_Alignas",
    "_Alignof", "_Atomic", "_Bool", "_Complex",
    "_Decimal128", "_Decimal32", "_Decimal64",
    "_Generic", "_Imaginary", "_Noreturn",
    "_Static_assert", "_Thread_local"
};

const char * const TYPES[] = {
    "int", "char", "void", "auto", "float", "double"
};

const char * const MODIFIERS[] = {
    "short", "long", "register", "volatile", "const",
    "signed", "unsigned", "static"
};

// TODO(mdizdar): trigraphs and alt tokens
const char * const MULTI_OPS[] = {
    "+=", "-=", "*=", "/=", "%=", "!=", "|=", "&=", 
    "^=", "==", "<=", ">=", "||", "&&", "~=", "++",
    "--", "->", "<<", ">>", ">>=", "<<="
};

#endif //RESERVED_H
