#ifndef TOKEN_H
#define TOKEN_H

#include "../utils/common.h"
#include "reserved.h" 
#include "symbol_table_entry.h"

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

STRUCT_HEADER(Token, {
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
    u64 col;
    
    TokenType type;
});

// please for the love of god have s be large enough
char *Token_toStr_long(char *s, Token t);
char *Token_toStr(char *s, Token t);

#endif // TOKEN_H
