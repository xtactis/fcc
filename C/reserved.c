#include "reserved.h"

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

const u64 KEYWORDS_count = sizeof(KEYWORDS) / sizeof(char *);
const u64 TYPES_count = sizeof(TYPES) / sizeof(char *);
const u64 MODIFIERS_count = sizeof(MODIFIERS) / sizeof(char *);
const u64 MULTI_OPS_count = sizeof(MULTI_OPS) / sizeof(char *);

