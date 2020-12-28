#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../utils/common.h"
#include "token.h"
#include "reserved.h"
#include "symbol_table.h"

#define parse_error error("Parse error!")

typedef struct {
    SymbolTable *symbol_table;
    
    u64 pos;
    String code;
} Lexer;

TokenType checkKeyword(const char *name) {
    u32 kw_len = sizeof(KEYWORDS) / sizeof(char*);
    for (u32 i = 0; i < kw_len; ++i) {
        if (strcmp(name, KEYWORDS[i]) == 0) {
            return (TokenType)(TOKEN_KEYWORD + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    u32 type_len = sizeof(TYPES) / sizeof(char*);
    for (u32 i = 0; i < type_len; ++i) {
        if (strcmp(name, TYPES[i]) == 0) {
            return (TokenType)(TOKEN_TYPE + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    u32 mod_len = sizeof(MODIFIERS) / sizeof(char*);
    for (u32 i = 0; i < mod_len; ++i) {
        if (strcmp(name, MODIFIERS[i]) == 0) {
            return (TokenType)(TOKEN_MODIFIER + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    return TOKEN_IDENT;
}

Token getNextToken(Lexer *lexer) {
    enum State {
        UNKNOWN = 0,
        IDENT   = 1, // [_a-zA-Z][_a-zA-Z0-9]*
        DECINT  = 2, // [1-9][0-9]* | 0
        OCTINT  = 3, // 0[0-9]*
        HEXINT  = 4, // 0(x|X)[a-fA-F0-9]+
        FLOAT   = 5, // [0-9]+\.[0-9]* // TODO(mdizdar): add scientific notation (pain in the ass)
        OP      = 6, // ++ | -- | >= | == | <= | || | && | ^= | != | += | -= | *= | /= | %= | |= | &= | << | >> | <<= | >>= | ->
        STRING  = 7, // ".*"
    } state  = UNKNOWN;
    
    bool was_zero       = false;
    bool escaped        = false;
    u64 integer_value   = 0;
    double double_value = 0;
    double dec_digit    = 1;
    
    for (u64 lookahead = lexer->pos; lookahead < lexer->code.count; ++lookahead) {
        char c = lexer->code.data[lookahead];
        //printf("%d %c\n", lookahead, c);
        switch (state) {
            case UNKNOWN: {
                if (isspace(c)) {
                    ++lexer->pos;
                    continue;
                }
                if (c == '_' || isalpha(c)) {
                    state = IDENT;
                } else if (isdigit(c)) {
                    state = DECINT;
                    integer_value = c - '0';
                    was_zero = c == '0';
                } else if (c == '"') {
                    state = STRING;
                } else {
                    state = OP;
                }
                break;
            }
            case IDENT: {
                if (c == '_' || isalnum(c)) continue;
                Token t;
                u64 count = lookahead - lexer->pos;
                char *name = malloc(count+1);
                name[count] = 0;
                strncpy(name, lexer->code.data + lexer->pos, count);
                t.type = checkKeyword(name);
                if (t.type == TOKEN_IDENT) {
                    t.type = TOKEN_IDENT;
                    t.name = (String){.data = name, .count = count};
                    SymbolTable_add(lexer->symbol_table, &t.name);
                }
                lexer->pos = lookahead;
                return t;
            }
            case DECINT: {
                integer_value *= 10;
                if (was_zero) {
                    if (c == 'x' || c == 'X') {
                        state = HEXINT;
                    } else if (isdigit(c)) {
                        state = OCTINT;
                    } else if (isalpha(c)) {
                        error("Unexpected alpha character (necu ti reci gdje idiote)");
                    } else {
                        lexer->pos = lookahead;
                        return (Token){.type = TOKEN_INT_LITERAL, .integer_value = 0};
                    }
                } else {
                    if (isdigit(c)) {
                        integer_value += c - '0';
                    } else if (isalpha(c)) {
                        // TODO(mdizdar): 'L' and 'LL' at the end are allowed though
                        error("Unexpected alpha character (necu ti reci gdje idiote)");
                    } else if (c == '.') {
                        double_value = integer_value;
                        state = FLOAT;
                    } else {
                        lexer->pos = lookahead; // NOTE(mdizdar): instead of making sure this is always here, write a function that'll create and return the token, while making sure the state is corrected accordingly. this will also allow us to fill in any auxilliary information e.g. the line and column number.
                        return (Token){.type = TOKEN_INT_LITERAL, .integer_value = integer_value};
                    }
                }
                break;
            }
            case OCTINT: {
                integer_value *= 8;
                if (c >= '0' && c <= '7') {
                    integer_value += c - '0';
                } else if (isdigit(c)) {
                    error("Did you write a '9' in an octal literal?");
                } else if (isalpha(c) || c == '.') {
                    error("Look at this dude.");
                } else {
                    lexer->pos = lookahead;
                    return (Token){.type = TOKEN_INT_LITERAL, .integer_value = integer_value};
                }
                break;
            }
            case HEXINT: {
                integer_value *= 16;
                if (isdigit(c)) {
                    integer_value += c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    integer_value += c - 'a';
                } else if (c >= 'A' && c <= 'F') {
                    integer_value += c - 'A';
                } else if (isalpha(c) || c == '.') {
                    error("Look at this dude.");
                } else {
                    lexer->pos = lookahead;
                    return (Token){.type = TOKEN_INT_LITERAL, .integer_value = integer_value};
                }
                break;
            }
            case FLOAT: {
                if (isdigit(c)) {
                    dec_digit /= 10;
                    double_value += (c - '0') * dec_digit;
                } else if (isalpha(c) || c == '.') {
                    error("Look at this dude.");
                } else {
                    lexer->pos = lookahead;
                    return (Token){.type = TOKEN_DOUBLE_LITERAL, .double_value = double_value};
                }
                break;
            }
            case OP: {
                char prev = lexer->code.data[lookahead-1];
                if (prev == '.' && isdigit(c)) {
                    dec_digit /= 10;
                    double_value += (c - '0') * dec_digit;
                    state = FLOAT;
                } else if (prev == '?' || prev == ':' || prev == '.' || prev == '(' || prev == ')' || prev == '[' || prev == ']' || prev == '{' || prev == '}') {
                    lexer->pos = lookahead;
                    return (Token){.type = prev};
                } else if (lookahead - lexer->pos == 1) {
                    if ((c != '>' || prev != '>') && (c != '<' || prev != '<')) {
                        for (u32 i = 0; i < sizeof(MULTI_OPS)/sizeof(char *) - 4; ++i) {
                            if (prev == MULTI_OPS[i][0] && c == MULTI_OPS[i][1]) {
                                lexer->pos = lookahead+1;
                                return (Token){.type = TOKEN_OPERATOR+i+1}; // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
                            }
                        }
                        lexer->pos = lookahead;
                        return (Token){.type = prev};
                    }
                } else {
                    if (c == '=') {
                        lexer->pos = lookahead+1;
                        if (prev == '<') {
                            return (Token){.type = TOKEN_BIT_L_ASSIGN};
                        } else {
                            return (Token){.type = TOKEN_BIT_R_ASSIGN};
                        }
                    } else {
                        lexer->pos = lookahead;
                        if (prev == '<') {
                            return (Token){.type = TOKEN_BITSHIFT_LEFT};
                        } else {
                            return (Token){.type = TOKEN_BITSHIFT_RIGHT};
                        }
                    }
                }
                break;
            }
            case STRING: {
                if (escaped) {
                    escaped = false;
                    continue;
                }
                if (c == '\\') {
                    escaped = true;
                    continue;
                }
                if (c == '"') {
                    u64 count = lookahead - lexer->pos - 1;
                    char *str = malloc(count+1);
                    str[count] = 0;
                    strncpy(str, lexer->code.data + lexer->pos + 1, count);
                    lexer->pos = lookahead+1;
                    return (Token){
                        .type = TOKEN_STRING_LITERAL,
                        .string_value = {.data = str, .count = count}
                    };
                }
                break;
            }
        }
    }
    return (Token){.type = TOKEN_ERROR};
}

#endif // PARSER_H