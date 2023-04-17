#include "lexer.h"
#include <ctype.h>

STRUCT_SOURCE(Lexer);

TokenType checkKeyword(const char *name) {
    for (u32 i = 0; i < KEYWORDS_count; ++i) {
        if (strcmp(name, KEYWORDS[i]) == 0) {
            return (TokenType)(TOKEN_KEYWORD + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    for (u32 i = 0; i < TYPES_count; ++i) {
        if (strcmp(name, TYPES[i]) == 0) {
            return (TokenType)(TOKEN_TYPE + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    for (u32 i = 0; i < MODIFIERS_count; ++i) {
        if (strcmp(name, MODIFIERS[i]) == 0) {
            return (TokenType)(TOKEN_MODIFIER + i + 1); // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
        }
    }
    return TOKEN_IDENT;
}

Token *Lexer_returnToken(Lexer *lexer, u64 lookahead, Token *t) {
    if (t != NULL && t->type != TOKEN_ERROR) {
        for (u64 i = lexer->peek; i < lookahead; ++i) {
            lexer->token_at[i].token = t;
            lexer->token_at[i].lookahead = lookahead;
            lexer->token_at[i].cur_line = lexer->cur_line;
            lexer->token_at[i].cur_col = lexer->cur_col - lookahead + lexer->peek;
        }
    }
    t->line = lexer->cur_line;
    t->col = lexer->cur_col - lookahead + lexer->peek;
    lexer->peek = lookahead;
    return t;
}

inline Token *Lexer_currentToken(Lexer *lexer) {
    return lexer->token_at[lexer->pos - 1].token;
}

inline Token *Lexer_currentPeekedToken(Lexer *lexer) {
    assert(lexer->token_at[lexer->peek - 1].token != NULL);
    return lexer->token_at[lexer->peek - 1].token;
}

inline void Lexer_resetPeek(Lexer *lexer) {
    lexer->peek = lexer->pos;
    lexer->cur_line = lexer->prev_line;
    lexer->cur_col = lexer->prev_col;
}

inline void Lexer_confirmPeek(Lexer *lexer) {
    lexer->pos = lexer->peek;
    lexer->prev_line = lexer->cur_line;
    lexer->prev_col = lexer->cur_col;
}

// finds the next token and returns it
Token *Lexer_peekNextToken(Lexer *lexer) {
    if (lexer->token_at[lexer->peek].token != NULL) {
        CachedToken *ct = &lexer->token_at[lexer->peek];
        lexer->peek = ct->lookahead;
        lexer->cur_line = ct->cur_line;
        lexer->cur_col = ct->cur_col;
        return ct->token;
    }
    enum State {
        UNKNOWN = 0,
        IDENT,      // [_a-zA-Z][_a-zA-Z0-9]*
        DECINT,     // [1-9][0-9]* | 0
        OCTINT,     // 0[0-9]*
        HEXINT,     // 0(x|X)[a-fA-F0-9]+
        INTSUF,     // *nothing*, L, l, U, u, LL, ll, LLU, ULL, LLu, uLL, llu, ull, llU, Ull
        FLOAT,      // [0-9]+\.[0-9]* // TODO(mdizdar): add scientific notation (pain in the ass)
        OP,         // ++ | -- | >= | == | <= | || | && | ^= | != | += | -= | *= | /= | %= | |= | &= | << | >> | <<= | >>= | ->
        CHAR,       // '.'
        STRING,     // ".*"
    } state  = UNKNOWN;
    
    bool was_zero       = false;
    bool escaped        = false;
    u64 escaped_count   = 0;
    u64 integer_value   = 0;
    //u64 long_count      = 0;
    double double_value = 0;
    double dec_digit    = 1;
    
    Token *t = Arena_alloc(lexer->token_arena, sizeof(Token)); // the token we return
    t->type = TOKEN_ERROR;
    
    for (u64 lookahead = lexer->peek; lookahead < lexer->code.count; ++lookahead, ++lexer->cur_col) {
        char c = lexer->code.data[lookahead];
        //printf("%llu %c\n", lookahead, c);
        switch (state) {
            case UNKNOWN: {
                if (isspace(c)) {
                    if (c == '\n') {
                        ++lexer->cur_line;
                        lexer->cur_col = -1;
                    }
                    ++lexer->peek;
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
                } else if (c == '\'') {
                    state = CHAR;
                } else {
                    state = OP;
                }
                break;
            }
            case IDENT: {
                if (c == '_' || isalnum(c)) continue;
                u64 count = lookahead - lexer->peek;
                char *name = malloc(count+1);
                name[count] = 0;
                strncpy(name, lexer->code.data + lexer->peek, count);
                t->type = checkKeyword(name);
                if (t->type == TOKEN_IDENT) {
                    t->name = (String){.data = name, .count = count};
                }
                return Lexer_returnToken(lexer, lookahead, t);
            }
            case DECINT: {
                if (was_zero) {
                    if (c == 'x' || c == 'X') {
                        state = HEXINT;
                    } else if (isdigit(c)) {
                        state = OCTINT;
                        --lookahead;
                    } else if (isalpha(c)) {
                        error(lexer->cur_line, "Parse error: Unexpected alpha character");
                    } else {
                        t->type = TOKEN_INT_LITERAL;
                        t->integer_value = 0;
                        return Lexer_returnToken(lexer, lookahead, t);
                    }
                } else {
                    if (isdigit(c)) {
                        integer_value *= 10;
                        integer_value += c - '0';
                    } else if (isalpha(c)) {
                        error(lexer->cur_line, "Parse error: Unexpected alpha character");
                    } else if (c == '.') {
                        double_value = (double)integer_value;
                        state = FLOAT;
                    } else {
                        t->type = TOKEN_INT_LITERAL;
                        t->integer_value = integer_value;
                        return Lexer_returnToken(lexer, lookahead, t);
                    }
                }
                break;
            }
            case OCTINT: {
                integer_value *= 8;
                if (c >= '0' && c <= '7') {
                    integer_value += c - '0';
                } else if (isdigit(c)) {
                    error(lexer->cur_line, "Parse error: Did you write a '9' in an octal literal?");
                } else if (isalpha(c) || c == '.') {
                    error(lexer->cur_line, "Look at this dude.");
                } else {
                    t->type = TOKEN_INT_LITERAL;
                    t->integer_value = integer_value;
                    return Lexer_returnToken(lexer, lookahead, t);
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
                    error(lexer->cur_line, "Look at this dude.");
                } else {
                    t->type = TOKEN_INT_LITERAL;
                    t->integer_value = integer_value;
                    return Lexer_returnToken(lexer, lookahead, t);
                }
                break;
            }
            case FLOAT: { // TODO(mdizdar): differentiate floats and doubles
                if (isdigit(c)) {
                    dec_digit /= 10;
                    double_value += (c - '0') * dec_digit;
                } else if (isalpha(c) || c == '.') {
                    error(lexer->cur_line, "Look at this dude.");
                } else {
                    t->type = TOKEN_DOUBLE_LITERAL;
                    t->double_value = double_value;
                    return Lexer_returnToken(lexer, lookahead, t);
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
                    t->type = prev;
                    return Lexer_returnToken(lexer, lookahead, t);
                } else if (lookahead - lexer->peek == 1) {
                    if ((c != '>' || prev != '>') && (c != '<' || prev != '<')) {
                        for (u32 i = 0; i < MULTI_OPS_count - 4; ++i) {
                            if (prev == MULTI_OPS[i][0] && c == MULTI_OPS[i][1]) {
                                t->type = TOKEN_OPERATOR+i+1; // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
                                return Lexer_returnToken(lexer, lookahead+1, t);
                            }
                        }
                        t->type = prev;
                        return Lexer_returnToken(lexer, lookahead, t);
                    }
                } else {
                    if (c == '=') {
                        if (prev == '<') {
                            t->type = TOKEN_BIT_L_ASSIGN;
                        } else {
                            t->type = TOKEN_BIT_R_ASSIGN;
                        }
                        return Lexer_returnToken(lexer, lookahead+1, t);
                    } else {
                        if (prev == '<') {
                            t->type = TOKEN_BITSHIFT_LEFT;
                        } else {
                            t->type = TOKEN_BITSHIFT_RIGHT;
                        }
                        return Lexer_returnToken(lexer, lookahead, t);
                    }
                }
                break;
            }
            case CHAR: {
                if (escaped) {
                    escaped = false;
                    ++escaped_count;
                    if (c == '\'') {
                        continue;
                    } else if (c == '"') {
                        continue;
                    } else if (c == '?') {
                        continue;
                    } else if (c == '\\') {
                        continue;
                    } else if (c == 'a') {
                        lexer->code.data[lookahead] = 0x07;
                    } else if (c == 'b') {
                        lexer->code.data[lookahead] = 0x08;
                    } else if (c == 'f') {
                        lexer->code.data[lookahead] = 0x0c;
                    } else if (c == 'n') {
                        lexer->code.data[lookahead] = 0x0a;
                    } else if (c == 'r') {
                        lexer->code.data[lookahead] = 0x0d;
                    } else if (c == 't') {
                        lexer->code.data[lookahead] = 0x09;
                    } else if (c == 'v') {
                        lexer->code.data[lookahead] = 0x0b;
                    } else {
                        error(lexer->cur_line, "Error: escape sequence not recognized");
                    }
                    continue;
                }
                if (c == '\\') {
                    escaped = true;
                    continue;
                }
                if (c == '\'') {
                    u64 count = lookahead - lexer->peek - 1 - escaped_count;
                    if (count > 1) {
                        error(lexer->cur_line, "Error: character literals can't be longer than 1 character.");
                    }
                    t->type = TOKEN_CHAR_LITERAL;
                    t->integer_value = lexer->code.data[lookahead-1];;
                    return Lexer_returnToken(lexer, lookahead+1, t);
                }
                break;
            }
            case STRING: {
                if (escaped) {
                    escaped = false;
                    ++escaped_count;
                    if (c == '\'') {
                        continue;
                    } else if (c == '"') {
                        continue;
                    } else if (c == '?') {
                        continue;
                    } else if (c == '\\') {
                        continue;
                    } else if (c == 'a') {
                        lexer->code.data[lookahead] = 0x07;
                    } else if (c == 'b') {
                        lexer->code.data[lookahead] = 0x08;
                    } else if (c == 'f') {
                        lexer->code.data[lookahead] = 0x0c;
                    } else if (c == 'n') {
                        lexer->code.data[lookahead] = 0x0a;
                    } else if (c == 'r') {
                        lexer->code.data[lookahead] = 0x0d;
                    } else if (c == 't') {
                        lexer->code.data[lookahead] = 0x09;
                    } else if (c == 'v') {
                        lexer->code.data[lookahead] = 0x0b;
                    } else {
                        error(lexer->cur_line, "Error: escape sequence not recognized");
                    }
                    continue;
                }
                if (c == '\\') {
                    escaped = true;
                    continue;
                }
                if (c == '"') {
                    u64 count = lookahead - lexer->peek - 1 - escaped_count;;
                    char *str = Arena_alloc(lexer->token_arena, count+1);
                    str[count] = 0;
                    for (u64 i = lexer->peek+1, j = 0; i < lookahead; ++i) {
                        if (lexer->code.data[i] == '\\' && lexer->code.data[i-1] != '\\') continue;
                        str[j++] = lexer->code.data[i];
                    }
                    t->type = TOKEN_STRING_LITERAL;
                    t->string_value = (String){.data = str, .count = count};
                    return Lexer_returnToken(lexer, lookahead+1, t);
                }
                break;
            }
            default: {
                printf("Internal compiler warning: unhandled lexer state %d\n", state);
            }
        }
    }
    return Lexer_returnToken(lexer, lexer->code.count, t);
}

// consumes peeked tokens
void Lexer_eat(Lexer *lexer) {
    Lexer_confirmPeek(lexer);
}

// finds and consumes the next token, then returns it
Token *Lexer_getNextToken(Lexer *lexer) {
    Token *t = Lexer_peekNextToken(lexer);
    Lexer_eat(lexer);
    return t;
}
