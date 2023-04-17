#include "token.h"

STRUCT_SOURCE(Token);
// please for the love of god have s be large enough
char *Token_toStr_long(char *s, Token t) {
    if (t.type < 256) {
        sprintf(s, "Token: { type: operator %c (%d) }", (char)t.type, t.type);
    } else if (t.type > 500 && t.type < 600) {
        sprintf(s, "Token: { type: keyword %s (%d) }",  KEYWORDS[t.type-TOKEN_KEYWORD-1], t.type);
    } else if (t.type > 600 && t.type < 700) {
        sprintf(s, "Token: { type: typename %s (%d) }", TYPES[t.type-TOKEN_TYPE-1], t.type);
    } else if (t.type > 700 && t.type < 800) {
        sprintf(s, "Token: { type: modifier %s (%d) }", MODIFIERS[t.type-TOKEN_MODIFIER-1], t.type);
    } else if (t.type > 800 && t.type < 900) {
        sprintf(s, "Token: { type: operator %s (%d) }", MULTI_OPS[t.type-TOKEN_OPERATOR-1], t.type);
    } else {
        switch (t.type) {
            case TOKEN_CHAR_LITERAL: {
                sprintf(s, "Token: { type: char literal (%d); value: \'%c\' }", t.type, (char)t.integer_value);
                break;
            }
            case TOKEN_INT_LITERAL: {
                sprintf(s, "Token: { type: int literal (%d); value: %u }", t.type, (unsigned int)t.integer_value);
                break;
            }
            case TOKEN_LONG_LITERAL: {
                sprintf(s, "Token: { type: long literal (%d); value: %lu }", t.type, (unsigned long)t.integer_value);
                break;
            }
            case TOKEN_LLONG_LITERAL: {
                sprintf(s, "Token: { type: long long literal (%d); value: %lu }", t.type, t.integer_value);
                break;
            }
            case TOKEN_FLOAT_LITERAL: {
                sprintf(s, "Token: { type: float literal (%d); value: %f }", t.type, t.float_value);
                break;
            }
            case TOKEN_DOUBLE_LITERAL: {
                sprintf(s, "Token: { type: double literal (%d); value: %lf }", t.type, t.double_value);
                break;
            }
            case TOKEN_STRING_LITERAL: {
                sprintf(s, "Token: { type: string literal (%d); value: \"%s\" }", t.type, t.string_value.data);
                break;
            }
            case TOKEN_IDENT: {
                sprintf(s, "Token: { type: identifier (%d); name: \"%s\" }", t.type, t.name.data);
                break;
            }
            case TOKEN_FUNCTION_CALL: {
                sprintf(s, "Token: { type: function call (%d) }", t.type);
                break;
            }
            case TOKEN_FOR_COND: {
                sprintf(s, "Token: { type: for cond (%d) }", t.type);
                break;
            }
            case TOKEN_ERROR: {
                sprintf(s, "Token: { type: ERROR (%d) }", t.type);
                break;
            }
            default: {
                sprintf(s, "Token: { type: (%d) }", t.type);
            }
        }
    }
    return s;
}

char *Token_toStr(char *s, Token t) {
    if (t.type < 256) {
        sprintf(s, "%c (%d)", (char)t.type, t.type);
    } else if (t.type > 500 && t.type < 600) {
        sprintf(s, "%s (%d)",  KEYWORDS[t.type-TOKEN_KEYWORD-1], t.type);
    } else if (t.type > 600 && t.type < 700) {
        sprintf(s, "%s (%d)", TYPES[t.type-TOKEN_TYPE-1], t.type);
    } else if (t.type > 700 && t.type < 800) {
        sprintf(s, "%s (%d)", MODIFIERS[t.type-TOKEN_MODIFIER-1], t.type);
    } else if (t.type > 800 && t.type < 823) {
        sprintf(s, "%s (%d)", MULTI_OPS[t.type-TOKEN_OPERATOR-1], t.type);
    } else {
        switch (t.type) {
            case TOKEN_CHAR_LITERAL: {
                sprintf(s, "'%c' (%d)", (char)t.integer_value, t.type);
                break;
            }
            case TOKEN_INT_LITERAL: {
                sprintf(s, "%u (%d)", (unsigned int)t.integer_value, t.type);
                break;
            }
            case TOKEN_LONG_LITERAL: {
                sprintf(s, "%luL (%d)", (unsigned long)t.integer_value, t.type);
                break;
            }
            case TOKEN_LLONG_LITERAL: {
                sprintf(s, "%luLL (%d)", t.integer_value, t.type);
                break;
            }
            case TOKEN_FLOAT_LITERAL: {
                sprintf(s, "%ff (%d)", t.float_value, t.type);
                break;
            }
            case TOKEN_DOUBLE_LITERAL: {
                sprintf(s, "%lf (%d)", t.double_value, t.type);
                break;
            }
            case TOKEN_STRING_LITERAL: {
                sprintf(s, "\"%s\" (%d)", t.string_value.data, t.type);
                break;
            }
            case TOKEN_IDENT: {
                sprintf(s, "ident %s (%d)", t.entry->name.data, t.type);
                break;
            }
            case TOKEN_FUNCTION_CALL: {
                sprintf(s, "() (%d)", t.type);
                break;
            }
            case TOKEN_FOR_COND: {
                sprintf(s, "for cond (%d)", t.type);
                break;
            }
            case TOKEN_PREINC: {
                sprintf(s, "pre ++ (%d)", t.type);
                break;
            }
            case TOKEN_PREDEC: {
                sprintf(s, "pre -- (%d)", t.type);
                break;
            }
            case TOKEN_POSTINC: {
                sprintf(s, "post ++ (%d)", t.type);
                break;
            }
            case TOKEN_POSTDEC: {
                sprintf(s, "post -- (%d)", t.type);
                break;
            }
            case TOKEN_DEREF: {
                sprintf(s, "deref * (%d)", t.type);
                break;
            }
            case TOKEN_ADDRESS: {
                sprintf(s, "address & (%d)", t.type);
                break;
            }
            case TOKEN_PLUS: {
                sprintf(s, "unary + (%d)", t.type);
                break;
            }
            case TOKEN_MINUS: {
                sprintf(s, "unary - (%d)", t.type);
                break;
            }
            case TOKEN_DECLARATION: {
                SymbolTableEntry *entry = t.entry;
                assert(entry);
                sprintf(s, "decl %s (%d)", entry->name.data, t.type);
                break;
            }
            case TOKEN_ERROR: {
                sprintf(s, "ERROR (%d)", t.type);
                break;
            }
            default: {
                sprintf(s, "(%d)", t.type);
            }
        }
    }
    return s;
}
