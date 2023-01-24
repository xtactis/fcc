#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/common.h"
#include "reserved.h" 
#include "symbol_table.h"

typedef enum {
    // single character Tokens will just be their ascii value
    TOKEN_ERROR = 256,
    
    TOKEN_IDENT = 260,
    
    TOKEN_LITERAL        = 270,
    TOKEN_CHAR_LITERAL   = 271,
    TOKEN_INT_LITERAL    = 272,
    TOKEN_LONG_LITERAL   = 273,
    TOKEN_LLONG_LITERAL  = 274,
    TOKEN_FLOAT_LITERAL  = 280,
    TOKEN_DOUBLE_LITERAL = 281,
    TOKEN_STRING_LITERAL = 290,
    
    TOKEN_KEYWORD       = 500,
    TOKEN_IF            = 501,
    TOKEN_WHILE         = 502,
    TOKEN_DO            = 503,
    TOKEN_FOR           = 504,
    TOKEN_SWITCH        = 505,
    TOKEN_CASE          = 506,
    TOKEN_BREAK         = 507,
    TOKEN_CONTINUE      = 508,
    TOKEN_ELSE          = 509,
    TOKEN_STRUCT        = 510,
    TOKEN_TYPEDEF       = 511,
    TOKEN_UNION         = 512,
    TOKEN_ENUM          = 513,
    TOKEN_RETURN        = 514,
    TOKEN_GOTO          = 515,
    TOKEN_DEFAULT       = 516,
    TOKEN_SIZEOF        = 517,
    TOKEN_EXTERN        = 518,
    TOKEN_INLINE        = 519,
    TOKEN_RESTRICT      = 520,
    TOKEN_ALIGNAS       = 521,
    TOKEN_ALIGNOF       = 522,
    TOKEN_ATOMIC        = 523,
    TOKEN_BOOL          = 524,
    TOKEN_COMPLEX       = 525,
    TOKEN_DECIMAL128    = 526,
    TOKEN_DECIMAL32     = 527,
    TOKEN_DECOMAL64     = 528,
    TOKEN_GENERIC       = 529,
    TOKEN_IMAGINARY     = 530,
    TOKEN_NORETURN      = 531,
    TOKEN_STATIC_ASSERT = 532,
    TOKEN_THREAD_LOCAL  = 533,
    
    TOKEN_TYPE   = 600,
    TOKEN_INT    = 601,
    TOKEN_CHAR   = 602,
    TOKEN_VOID   = 603,
    TOKEN_AUTO   = 604, 
    TOKEN_FLOAT  = 605,
    TOKEN_DOUBLE = 606,
    
    TOKEN_MODIFIER = 700,
    TOKEN_SHORT    = 701,
    TOKEN_LONG     = 702,
    TOKEN_REGISTER = 703,
    TOKEN_VOLATILE = 704,
    TOKEN_CONST    = 705,
    TOKEN_SIGNED   = 706,
    TOKEN_UNSIGNED = 707,
    TOKEN_STATIC   = 708,
    
    TOKEN_OPERATOR       = 800,
    TOKEN_ADD_ASSIGN     = 801,
    TOKEN_SUB_ASSIGN     = 802,
    TOKEN_MUL_ASSIGN     = 803,
    TOKEN_DIV_ASSIGN     = 804,
    TOKEN_MOD_ASSIGN     = 805,
    TOKEN_NOT_EQ         = 806,
    TOKEN_OR_ASSIGN      = 807,
    TOKEN_AND_ASSIGN     = 808,
    TOKEN_XOR_ASSIGN     = 809,
    TOKEN_EQUALS         = 810,
    TOKEN_LESS_EQ        = 811,
    TOKEN_GREATER_EQ     = 812,
    TOKEN_LOGICAL_OR     = 813,
    TOKEN_LOGICAL_AND    = 814,
    TOKEN_BITNOT_ASSIGN  = 815,
    TOKEN_INC            = 816,
    TOKEN_DEC            = 817,
    TOKEN_ARROW          = 818,
    TOKEN_BITSHIFT_LEFT  = 819,
    TOKEN_BITSHIFT_RIGHT = 820,
    TOKEN_BIT_L_ASSIGN   = 821,
    TOKEN_BIT_R_ASSIGN   = 822,
    
    TOKEN_PREINC         = 823,
    TOKEN_PREDEC         = 824,
    TOKEN_POSTINC        = 825,
    TOKEN_POSTDEC        = 826,
    TOKEN_DEREF          = 827,
    TOKEN_ADDRESS        = 828,
    TOKEN_PLUS           = 829,
    TOKEN_MINUS          = 830,
    
    TOKEN_FUNCTION_CALL  = 900,
    TOKEN_FOR_COND       = 901, // used in `for (<this bit>)`
    TOKEN_NEXT           = 902, // for chaining statements together
    TOKEN_DECLARATION    = 903,
} TokenType;

STRUCT(Token, {
    union {
        String name;
        SymbolTableEntry *entry;
        u64 integer_value;
        float float_value;
        double double_value;
        String string_value;
    };
    
    u64 lookahead;
    u64 line;
    
    TokenType type;
});

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

struct _Type;
typedef struct _Type Type;

STRUCT (Node, {
    Token *token;
    struct Node *left, *right;
    struct Node *cond; // this is only used for ternary
    
    const Scope *scope; // NOTE(mdizdar): usually NULL, except on nodes that change the scope
    Type *type; // filled in by type checker
});

#endif // TOKEN_H
