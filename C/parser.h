#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../utils/common.h"
#include "symbol_table.h"
#include "token.h"
#include "type.h"
#include "reserved.h"
#include "arena.h"

#include "../IR/IR.h"

#define parse_error error("Parse error!")

//#define Arena_alloc(x, y) malloc((y))

typedef struct {
    Token *token;
    u64 lookahead;
    u64 cur_line;
} CachedToken;

typedef struct {
    String code;
    
    CachedToken *token_at;
    Arena *token_arena;
    
    u64 cur_line;
    u64 prev_line;
    u64 pos;
    u64 peek;
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

inline Token *Lexer_returnToken(Lexer *lexer, u64 lookahead, Token *t) {
    //printf("peek %llu / %llu\n", lexer->peek, lexer->code.count);
    if (t != NULL && t->type != TOKEN_ERROR) {
        //printf("%p\n", lexer->token_at[lexer->peek]);
        for (u64 i = lexer->peek; i < lookahead; ++i) {
            lexer->token_at[i].token = t;
            lexer->token_at[i].lookahead = lookahead;
            lexer->token_at[i].cur_line = lexer->cur_line;
        }
    }
    lexer->peek = lookahead;
    return t;
}

inline Token *Lexer_currentToken(Lexer *lexer) {
    return lexer->token_at[lexer->pos - 1].token;
}

inline Token *Lexer_currentPeekedToken(Lexer *lexer) {
    return lexer->token_at[lexer->peek - 1].token;
}

inline void Lexer_resetPeek(Lexer *lexer) {
    lexer->peek = lexer->pos;
    lexer->cur_line = lexer->prev_line;
}

inline void Lexer_confirmPeek(Lexer *lexer) {
    lexer->pos = lexer->peek;
    lexer->prev_line = lexer->cur_line;
}

// finds the next token and returns it
Token *Lexer_peekNextToken(Lexer *lexer) {
    if (lexer->token_at[lexer->peek].token != NULL) {
        CachedToken *ct = &lexer->token_at[lexer->peek];
        lexer->peek = ct->lookahead;
        lexer->cur_line = ct->cur_line;
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
    
    for (u64 lookahead = lexer->peek; lookahead < lexer->code.count; ++lookahead) {
        char c = lexer->code.data[lookahead];
        //printf("%llu %c\n", lookahead, c);
        switch (state) {
            case UNKNOWN: {
                if (isspace(c)) {
                    if (c == '\n') {
                        ++lexer->cur_line;
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
                        for (u32 i = 0; i < sizeof(MULTI_OPS)/sizeof(char *) - 4; ++i) {
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

//~ PARSER

typedef struct {
    Arena *arena;
    Arena *type_arena;
    SymbolTable *symbol_table;
    Lexer lexer;
} Parser;

typedef enum {
    PRECEDENCE_NONE = 0,
    PRECEDENCE_COMMA = 1,
    PRECEDENCE_ASSIGNMENT = 2,
    PRECEDENCE_TERNARY = 3,
    PRECEDENCE_LOG_OR = 4,
    PRECEDENCE_LOG_AND = 5,
    PRECEDENCE_BIT_OR = 6,
    PRECEDENCE_BIT_AND = 7,
    PRECEDENCE_REL_EQ = 8,
    PRECEDENCE_REL_OP = 9,
    PRECEDENCE_BITSHIFT = 10,
    PRECEDENCE_ADD = 11,
    PRECEDENCE_MUL = 12,
    PRECEDENCE_HIGH = 13, // NOTE(mdizdar): bad names
    PRECEDENCE_HIGHEST = 14
} Precedence;

_Noreturn void Parser_error(Parser *parser, Token *token, TokenType expected_type) {
    char s[123], p[123];
    Token_toStr(s, *token);
    Token_toStr(p, (Token){.type = expected_type});
    error(parser->lexer.cur_line, "Error: expected '%s', but got '%s'", p, s);
}

_Noreturn void Parser_duplicateError(Parser *parser, SymbolTableEntry *previous) {
    error(parser->lexer.cur_line, "Error: redefinition of %s; previous definition on line %llu", previous->name.data, previous->definition_line);
}

_Noreturn void Parser_conflictingTypesError(Parser *parser, TokenType current, TokenType conflicting) {
    error(parser->lexer.cur_line, "Error: can't combine type %llu with previously defined %llu", conflicting, current);
}

_Noreturn void Parser_notATypeError(Parser *parser, u64 longs, u64 shorts, TokenType type) {
    error(parser->lexer.cur_line, "Error: '%s %s%d' is not a valid type", longs?"long":shorts?"short":"", longs == 2?"long ":"", type);
}

// TODO(mdizdar): compound literals
// TODO(mdizdar): the bodies of these precedence based functions are very similar, generalize maybe
// TODO(mdizdar): this is stupid, split everything into header and code files
Node *Parser_expr(Parser *parser);

inline void Parser_eat(Parser *parser, Token *token, TokenType token_type) {
    if (token->type == token_type) {
        Lexer_eat(&parser->lexer);
    } else {
        Parser_error(parser, token, token_type);
    }
}

//#define Parser_eat(parser, token, type) { printf("%s: %u\n", __FILE__, __LINE__); Parser_eat((parser), (token), (type)); }

Node *Parser_operand(Parser *parser) {
    // factor ::= ident | literal | '(' expr ')'
    
    Token *token = Lexer_peekNextToken(&parser->lexer); // TODO(mdizdar): I should really be handling the pointer but it's annoying right now
    Node *node = NULL;
    if (token->type == TOKEN_CHAR_LITERAL) {
        Parser_eat(parser, token, TOKEN_CHAR_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_INT_LITERAL) {
        Parser_eat(parser, token, TOKEN_INT_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_LONG_LITERAL) {
        Parser_eat(parser, token, TOKEN_LONG_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_LLONG_LITERAL) {
        Parser_eat(parser, token, TOKEN_LLONG_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_FLOAT_LITERAL) {
        Parser_eat(parser, token, TOKEN_FLOAT_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_DOUBLE_LITERAL) {
        Parser_eat(parser, token, TOKEN_DOUBLE_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_STRING_LITERAL) {
        Parser_eat(parser, token, TOKEN_STRING_LITERAL);
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == TOKEN_IDENT) {
        Parser_eat(parser, token, TOKEN_IDENT);
        node = Arena_alloc(parser->arena, sizeof(Node));
        SymbolTableEntry *entry = SymbolTable_find(parser->symbol_table, &token->name);
        if (entry) {
            token->entry = entry;
        }
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        node->scope = NULL;
    } else if (token->type == '(') {
        Parser_eat(parser, token, '(');
        node = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ')');
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_postfix(Parser *parser) {
    Node *node = Parser_operand(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_INC || token->type == TOKEN_DEC || token->type == TOKEN_ARROW || token->type == '.' || token->type == '[' || token->type == '(') {
        Node *right = NULL;
        if (token->type == TOKEN_INC) {
            Parser_eat(parser, token, TOKEN_INC);
            token->type = TOKEN_POSTINC;
        } else if (token->type == TOKEN_DEC) {
            Parser_eat(parser, token, TOKEN_DEC);
            token->type = TOKEN_POSTDEC;
        } else if (token->type == TOKEN_ARROW) {
            Parser_eat(parser, token, TOKEN_ARROW);
            right = Parser_operand(parser);
        } else if (token->type == '.') {
            Parser_eat(parser, token, '.');
            right = Parser_operand(parser);
        } else if (token->type == '[') {
            Parser_eat(parser, token, '[');
            right = Parser_expr(parser);
            token = Lexer_peekNextToken(&parser->lexer);
            Parser_eat(parser, token, ']');
        } else if (token->type == '(') {
            Parser_eat(parser, token, '(');
            // NOTE(mdizdar): the commas are treated differently here
            right = Parser_expr(parser);
            token = Lexer_peekNextToken(&parser->lexer);
            Parser_eat(parser, token, ')');
            token->type = TOKEN_FUNCTION_CALL;
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = right;
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_prefix(Parser *parser) {
    Node *node;
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    // TODO(mdizdar): casting
    if (token->type == TOKEN_INC || token->type == TOKEN_DEC ||
        token->type == TOKEN_SIZEOF || token->type == TOKEN_ALIGNOF || 
        token->type == '+' || token->type == '-' || token->type == '!' || 
        token->type == '~' || token->type == '*' || token->type == '&') {
        if (token->type == TOKEN_INC) {
            Parser_eat(parser, token, TOKEN_INC);
            token->type = TOKEN_PREINC;
        } else if (token->type == TOKEN_DEC) {
            Parser_eat(parser, token, TOKEN_DEC);
            token->type = TOKEN_PREDEC;
        } else if (token->type == TOKEN_SIZEOF) {
            Parser_eat(parser, token, TOKEN_SIZEOF);
        } else if (token->type == TOKEN_ALIGNOF) {
            Parser_eat(parser, token, TOKEN_ALIGNOF);
        } else if (token->type == '+') {
            Parser_eat(parser, token, '+');
            token->type = TOKEN_PLUS;
        } else if (token->type == '-') {
            Parser_eat(parser, token, '-');
            token->type = TOKEN_MINUS;
        } else if (token->type == '!') {
            Parser_eat(parser, token, '!');
        } else if (token->type == '~') {
            Parser_eat(parser, token, '~');
        } else if (token->type == '*') {
            Parser_eat(parser, token, '*');
            token->type = TOKEN_DEREF;
        } else if (token->type == '&') {
            Parser_eat(parser, token, '&');
            token->type = TOKEN_ADDRESS;
        }
        
        node = Arena_alloc(parser->arena, sizeof(Node));
        node->left = Parser_prefix(parser);
        node->token = token;
        node->right = NULL;
        node->scope = NULL;
    } else {
        parser->lexer.peek = parser->lexer.pos;
        
        node = Parser_postfix(parser);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_muls(Parser *parser) {
    // term ::= factor [('*'|'/'|'%') factor]*
    Node *node = Parser_prefix(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == '*' || token->type == '/' || token->type == '%') {
        if (token->type == '*') {
            Parser_eat(parser, token, '*');
        } else if (token->type == '/') {
            Parser_eat(parser, token, '/');
        } else if (token->type == '%') {
            Parser_eat(parser, token, '%');
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_prefix(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_sums(Parser *parser) {
    // expr ::= term [('+'|'-') term]*
    
    Node *node = Parser_muls(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    while (token->type == '+' || token->type == '-') {
        if (token->type == '+') {
            Parser_eat(parser, token, '+');
        } else if (token->type == '-') {
            Parser_eat(parser, token, '-');
        }
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_muls(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_bitshift(Parser *parser) {
    Node *node = Parser_sums(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_BITSHIFT_LEFT || token->type == TOKEN_BITSHIFT_RIGHT) {
        if (token->type == TOKEN_BITSHIFT_LEFT) {
            Parser_eat(parser, token, TOKEN_BITSHIFT_LEFT);
        } else if (token->type == TOKEN_BITSHIFT_RIGHT) {
            Parser_eat(parser, token, TOKEN_BITSHIFT_RIGHT);
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_sums(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_rel_op(Parser *parser) {
    Node *node = Parser_bitshift(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_LESS_EQ || token->type == TOKEN_GREATER_EQ ||
           token->type == '<' || token->type == '>') {
        if (token->type == TOKEN_LESS_EQ) {
            Parser_eat(parser, token, TOKEN_LESS_EQ);
        } else if (token->type == TOKEN_GREATER_EQ) {
            Parser_eat(parser, token, TOKEN_GREATER_EQ);
        } else if (token->type == '<') {
            Parser_eat(parser, token, '<');
        } else if (token->type == '>') {
            Parser_eat(parser, token, '>');
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_bitshift(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_rel_eq(Parser *parser) {
    Node *node = Parser_rel_op(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_EQUALS || token->type == TOKEN_NOT_EQ) {
        if (token->type == TOKEN_EQUALS) {
            Parser_eat(parser, token, TOKEN_EQUALS);
        } else if (token->type == TOKEN_NOT_EQ) {
            Parser_eat(parser, token, TOKEN_NOT_EQ);
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_rel_op(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_bit_and(Parser *parser) {
    Node *node = Parser_rel_eq(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == '&') {
        Parser_eat(parser, token, '&');
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_rel_eq(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_bit_xor(Parser *parser) {
    Node *node = Parser_bit_and(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == '^') {
        Parser_eat(parser, token, '^');
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_bit_and(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_bit_or(Parser *parser) {
    Node *node = Parser_bit_xor(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == '|') {
        Parser_eat(parser, token, '|');
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_bit_xor(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_log_and(Parser *parser) {
    Node *node = Parser_bit_or(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_LOGICAL_AND) {
        Parser_eat(parser, token, TOKEN_LOGICAL_AND);
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_bit_or(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_log_or(Parser *parser) {
    Node *node = Parser_log_and(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == TOKEN_LOGICAL_OR) {
        Parser_eat(parser, token, TOKEN_LOGICAL_OR);
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_log_and(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_ternary(Parser *parser) {
    Node *node = Parser_log_or(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    if (token->type == '?') {
        Parser_eat(parser, token, '?');
        Node *ternary_true = Parser_expr(parser);
        Token *colon = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, colon, ':');
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->cond = node;
        tmp->left = ternary_true;
        tmp->token = token;
        tmp->right = Parser_ternary(parser);
        tmp->scope = NULL;
        node = tmp;
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_assignment(Parser *parser) {
    Node *node = Parser_ternary(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    // TODO(mdizdar): doing this exact same thing for each level is kinda stupid, refactor it eventually
    
    if (token->type == '=' || 
        token->type == TOKEN_ADD_ASSIGN || token->type == TOKEN_SUB_ASSIGN ||
        token->type == TOKEN_MUL_ASSIGN || token->type == TOKEN_DIV_ASSIGN || 
        token->type == TOKEN_MOD_ASSIGN || token->type == TOKEN_OR_ASSIGN  || 
        token->type == TOKEN_AND_ASSIGN || token->type == TOKEN_XOR_ASSIGN || 
        token->type == TOKEN_BIT_L_ASSIGN || token->type == TOKEN_BIT_R_ASSIGN) {
        if (token->type == '=') {
            Parser_eat(parser, token, '=');
        } else if (token->type == TOKEN_ADD_ASSIGN) {
            Parser_eat(parser, token, TOKEN_ADD_ASSIGN);
        } else if (token->type == TOKEN_SUB_ASSIGN) {
            Parser_eat(parser, token, TOKEN_SUB_ASSIGN);
        } else if (token->type == TOKEN_MUL_ASSIGN) {
            Parser_eat(parser, token, TOKEN_MUL_ASSIGN);
        } else if (token->type == TOKEN_DIV_ASSIGN) {
            Parser_eat(parser, token, TOKEN_DIV_ASSIGN);
        } else if (token->type == TOKEN_MOD_ASSIGN) {
            Parser_eat(parser, token, TOKEN_MOD_ASSIGN);
        } else if (token->type == TOKEN_OR_ASSIGN) {
            Parser_eat(parser, token, TOKEN_OR_ASSIGN);
        } else if (token->type == TOKEN_AND_ASSIGN) {
            Parser_eat(parser, token, TOKEN_AND_ASSIGN);
        } else if (token->type == TOKEN_XOR_ASSIGN) {
            Parser_eat(parser, token, TOKEN_XOR_ASSIGN);
        } else if (token->type == TOKEN_BIT_L_ASSIGN) {
            Parser_eat(parser, token, TOKEN_BIT_L_ASSIGN);
        } else if (token->type == TOKEN_BIT_R_ASSIGN) {
            Parser_eat(parser, token, TOKEN_BIT_R_ASSIGN);
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_assignment(parser);
        tmp->scope = NULL;
        node = tmp;
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_comma(Parser *parser) {
    Node *node = Parser_assignment(parser);
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type == ',') {
        if (token->type == ',') {
            Parser_eat(parser, token, ',');
        }
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_assignment(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_expr(Parser *parser) {
    return Parser_comma(parser);
}

Node *Parser_statement(Parser *parser);

Node *Parser_functionBlock(Parser *parser) {
    Token *token = Lexer_peekNextToken(&parser->lexer);
    Parser_eat(parser, token, '{');
    
    Node *node = Parser_statement(parser);
    token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type != '}') {
        Lexer_resetPeek(&parser->lexer);
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = Arena_alloc(parser->lexer.token_arena, sizeof(Token));
        tmp->token->type = TOKEN_NEXT;
        tmp->right = Parser_statement(parser);
        tmp->scope = NULL;
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    Parser_eat(parser, token, '}');
    
    Lexer_resetPeek(&parser->lexer);
    
    node->scope = parser->symbol_table->scope;
    // TODO(mdizdar): this is wrong, the scope should be changed at the function declaration, or when opening a new block. i.e. in lines 1027 and when creating a function declaration node, but it should only refer to the scope within it, so I'm not sure how to handle that rn Sadge
    
    return node;
}

Node *Parser_block(Parser *parser) {
    SymbolTable_pushScope(parser->symbol_table);
    
    Node *node = Parser_functionBlock(parser);
    
    SymbolTable_popScope(parser->symbol_table);
    
    return node;
}

Declaration *Parser_declaration(Parser *parser, bool can_be_static);

Declaration *Parser_struct(Parser *parser, Type **type) {
    Type *_type = *type;
    _type->is_struct = true;
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    String *type_name = NULL;
    
    if (token->type == TOKEN_IDENT) {
        Parser_eat(parser, token, TOKEN_IDENT);
        type_name = &token->name;
        token = Lexer_peekNextToken(&parser->lexer);
    }
    if (token->type == '{') {
        Parser_eat(parser, token, '{');
        
        SymbolTable_pushScope(parser->symbol_table); // NOTE(mdizdar): this shouldn't be done for anonymous structs/unions that are within other structures
        
        _type->struct_type = Arena_alloc(parser->type_arena, sizeof(StructType));
        
        //type->struct_type->members.data         = NULL;
        DynArray_construct(&_type->struct_type->members, sizeof(Declaration));
        
        token = Lexer_peekNextToken(&parser->lexer);
        while (token->type != '}') {
            Lexer_resetPeek(&parser->lexer);
            Declaration *member = Parser_declaration(parser, false);
            if (member) {
                DynArray_add(&_type->struct_type->members, member);
            }
            token = Lexer_peekNextToken(&parser->lexer);
            while (token->type == ';') {
                Parser_eat(parser, token, ';');
                token = Lexer_peekNextToken(&parser->lexer);
            }
        }
        SymbolTable_popScope(parser->symbol_table);
    } else if (token->type == TOKEN_IDENT) {
        Lexer_resetPeek(&parser->lexer);
        SymbolTableEntry *entry = SymbolTable_find(parser->symbol_table, type_name);
        token->entry = entry;
        type = &entry->type;
        
        return NULL;
    } else {
        error(parser->lexer.cur_line, "bruh"); // NOTE(mdizdar): idk what to tell this dude tbh
    }
    
    Declaration *declaration = Arena_alloc(parser->type_arena, sizeof(Declaration));
    declaration->type = _type;
    if (type_name) {
        declaration->name = *type_name;
        SymbolTable_add(parser->symbol_table, type_name, _type, parser->lexer.cur_line);
    } else {
        declaration->name.data = "";
        declaration->name.count = 0;
    }
    return declaration;
}

Type *Parser_function(Parser *parser, Type *type) {
    Token *token = Lexer_peekNextToken(&parser->lexer);
    if (token->type != '(') {
        Lexer_resetPeek(&parser->lexer);
        return NULL;
    }
    Parser_eat(parser, token, '('); // uhh
    
    Type *ftype = Arena_alloc(parser->type_arena, sizeof(Type));
    ftype->is_function = true;
    ftype->function_type = Arena_alloc(parser->type_arena, sizeof(FunctionType));
    DynArray_construct(&ftype->function_type->parameters, sizeof(Declaration));
    ftype->function_type->return_type = type;
    
    SymbolTable_pushScope(parser->symbol_table);
    
    // get parameter list
    token = Lexer_peekNextToken(&parser->lexer);
    while (token->type != ')') {
        Lexer_resetPeek(&parser->lexer);
        Declaration *decl = Parser_declaration(parser, false);
        DynArray_add(&ftype->function_type->parameters, decl);
        token = Lexer_peekNextToken(&parser->lexer);
        if (token->type == ',') {
            Parser_eat(parser, token, ',');
            token = Lexer_peekNextToken(&parser->lexer);
        }
    }
    Parser_eat(parser, token, ')');
    
    ftype->function_type->block = Parser_functionBlock(parser);
    
    SymbolTable_popScope(parser->symbol_table);
    
    Lexer_resetPeek(&parser->lexer);
    
    return ftype;
}

inline Type *Parser_cvp(Parser *parser, Type *type) {
    // TODO(mdizdar): this doesn't yet take into account function pointers
    
    Token *token = Lexer_currentPeekedToken(&parser->lexer);
    
    Type *new_type = Arena_alloc(parser->type_arena, sizeof(Type));;
    memcpy(new_type, type, sizeof(Type));
    
    while (token->type == '*') {
        Parser_eat(parser, token, '*');
        ++new_type->pointer_count;
        
        token = Lexer_peekNextToken(&parser->lexer);
        
        while (token->type == TOKEN_CONST ||
               token->type == TOKEN_VOLATILE /*||
   token->type == TOKEN_RESTRICT*/) {
            Parser_eat(parser, token, token->type); // uhh
            
            switch (token->type) {
                case TOKEN_CONST: {
                    Bitset_set(new_type->is_const, new_type->pointer_count);
                    break;
                }
                case TOKEN_VOLATILE: {
                    Bitset_set(new_type->is_volatile, new_type->pointer_count);
                    break;
                }
                // TODO(mdizdar): case TOKEN_RESTRICT: Bitset_set(new_type->is_restrict, new_type->pointer_count); break;
            }
            
            token = Lexer_peekNextToken(&parser->lexer);
        }
    }
    
    return new_type;
}

Declaration *Parser_declaration(Parser *parser, bool can_be_static) {
    Token *token = Lexer_peekNextToken(&parser->lexer);
    if (!((token->type > TOKEN_TYPE && token->type < TOKEN_MODIFIER) ||
          (token->type > TOKEN_MODIFIER && token->type < TOKEN_OPERATOR) ||
          token->type == TOKEN_STRUCT)) {
        Lexer_resetPeek(&parser->lexer);
        return NULL;
    }
    
    Type *type = Arena_alloc(parser->type_arena, sizeof(Type));
    
    type->is_static = false;
    type->is_struct = false;
    type->is_union = false;
    type->is_typedef = false;
    type->is_array = false;
    type->is_function = false;
    type->pointer_count = 0;
    for (int i = 0; i < sizeof(type->is_const) / sizeof(*type->is_const); ++i) {
        type->is_const[i] = 0;
        type->is_volatile[i] = 0;
        type->is_restrict[i] = 0;
    }
    type->basic_type = BASIC_ERROR;
    
    u8 longs = 0;
    u8 shorts = 0;
    bool is_signed = false;
    bool is_unsigned = false;
    
    while ((token->type > TOKEN_TYPE && token->type < TOKEN_MODIFIER) ||
           (token->type > TOKEN_MODIFIER && token->type < TOKEN_OPERATOR) ||
           token->type == TOKEN_STRUCT) {
        Parser_eat(parser, token, token->type); // uhh
        
        switch (token->type) {
            case TOKEN_CHAR: {
                type->basic_type = BASIC_CHAR;
                break;
            }
            case TOKEN_INT: {
                type->basic_type = BASIC_SINT;
                break;
            }
            case TOKEN_VOID: {
                type->basic_type = BASIC_VOID;
                break;
            }
            case TOKEN_FLOAT: {
                type->basic_type = BASIC_FLOAT;
                break;
            }
            case TOKEN_DOUBLE: {
                type->basic_type = BASIC_DOUBLE;
                break;
            }
            case TOKEN_STATIC: {
                if (!can_be_static) {
                    error(parser->lexer.cur_line, "Error: cannot specify static storage in this context");
                }
                type->is_static = 1;
                break;
            }
            case TOKEN_CONST: {
                Bitset_set(type->is_const, type->pointer_count);
                break;
            }
            case TOKEN_VOLATILE: {
                Bitset_set(type->is_volatile, type->pointer_count);
                break;
            }
            case TOKEN_SIGNED: {
                if (is_unsigned) {
                    Parser_conflictingTypesError(parser, TOKEN_SIGNED, TOKEN_UNSIGNED);
                }
                is_signed = true;
                break;
            }
            case TOKEN_UNSIGNED: {
                if (is_signed) {
                    Parser_conflictingTypesError(parser, TOKEN_UNSIGNED, TOKEN_SIGNED);
                }
                is_unsigned = true;
                break;
            }
            case TOKEN_STRUCT: {
                Parser_struct(parser, &type);
                break;
            }
            case TOKEN_LONG: {
                if (++longs > 2) {
                    error(parser->lexer.cur_line, "Error: that's too many 'long's, this isn't gcc, you won't get a funny error message.");
                }
                if (shorts) {
                    Parser_conflictingTypesError(parser, TOKEN_SHORT, TOKEN_LONG);
                }
                break;
            }
            case TOKEN_SHORT: {
                if (++shorts > 1) {
                    error(parser->lexer.cur_line, "Error: what's a 'short short'");
                }
                if (longs) {
                    Parser_conflictingTypesError(parser, TOKEN_LONG, TOKEN_SHORT);
                }
                break;
            }
            case TOKEN_AUTO: {
                warning(parser->lexer.cur_line, "auto does nothing, it never has done anything and it never will.");
            }
        }
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    if (!is_signed && !is_unsigned) {
        is_signed = true;
    } else if (type->basic_type != BASIC_ERROR && type->basic_type != BASIC_SINT && type->basic_type != BASIC_CHAR) {
        error(parser->lexer.cur_line, "Error: %d can't be signed or unsigned", type->basic_type);
    }
    if (type->basic_type != BASIC_SINT && type->basic_type != BASIC_ERROR && (longs || shorts)) {
        Parser_notATypeError(parser, longs, shorts, token->type);
    }
    if ((type->basic_type == BASIC_SINT || type->basic_type == BASIC_ERROR) && (shorts || longs || is_signed || is_unsigned)) {
        // taking care of implicit int in e.g. 'long long x'
        if (longs == 1)      type->basic_type = is_signed ? BASIC_SLONG : BASIC_ULONG;
        else if (longs == 2) type->basic_type = is_signed ? BASIC_SLLONG : BASIC_ULLONG;
        else if (shorts)     type->basic_type = is_signed ? BASIC_SSHORT : BASIC_USHORT;
        else                 type->basic_type = is_signed ? BASIC_SINT : BASIC_UINT;
    } else if (type->basic_type == BASIC_CHAR && (is_signed || is_unsigned)) {
        if (is_signed)        type->basic_type = BASIC_SCHAR;
        else if (is_unsigned) type->basic_type = BASIC_UCHAR;
    }
    
    Declaration *declaration = Arena_alloc(parser->type_arena, sizeof(Declaration));
    
    declaration->type = Parser_cvp(parser, type);
    
    token = Lexer_currentPeekedToken(&parser->lexer);
    
    Parser_eat(parser, token, TOKEN_IDENT);
    
    SymbolTableEntry *entry = SymbolTable_find(parser->symbol_table, &token->name);
    if (entry != NULL) Parser_duplicateError(parser, entry);
    
    u64 fline = parser->lexer.cur_line;
    Type *ftype = Parser_function(parser, declaration->type);
    
    if (ftype) {
        SymbolTable_add(parser->symbol_table, &token->name, ftype, fline);
        token->entry = SymbolTable_find(parser->symbol_table, &token->name);
        declaration->type = ftype;
        declaration->name = token->name;
        return declaration;
    } /*else {
// TODO(mdizdar): I don't yet know how I'll handle multiple declarations on one line,
I'll figure that out when I do the second pass on the parser

token = Lexer_peekNextToken(&parser->lexer);
while (token->type == ',') {
            Parser_cvp(parser, base_type);
        }
    }
*/
    
    SymbolTable_add(parser->symbol_table, &token->name, type, parser->lexer.cur_line);
    
    token->entry = SymbolTable_find(parser->symbol_table, &token->name);
    
    declaration->type = type;
    declaration->name = token->name;
    
    Lexer_resetPeek(&parser->lexer);
    
    //Type_print(type, 0);
    
    return declaration;
}

Node *Parser_statement(Parser *parser) {
    Declaration *decl = Parser_declaration(parser, true);
    if (decl) {
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->token = Arena_alloc(parser->lexer.token_arena, sizeof(Token));
        tmp->token->type = TOKEN_DECLARATION;
        tmp->token->name = decl->name;
        tmp->left = NULL;
        tmp->right = NULL;
        tmp->scope = NULL;
        
        Token *token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
        return tmp;
    }
    
    Node *node = Arena_alloc(parser->arena, sizeof(Node));
    node->scope = NULL;
    node->left = NULL;
    node->right = NULL;
    node->cond = NULL;
    
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    if (token->type == TOKEN_IF) {
        Parser_eat(parser, token, TOKEN_IF);
        
        node->token = token;
        
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, '(');
        Node *cond = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ')');
        
        node->cond = cond;
        node->right = NULL;
        
        node->left = Parser_statement(parser);
        
        token = Lexer_peekNextToken(&parser->lexer);
        if (token->type == TOKEN_ELSE) {
            Parser_eat(parser, token, TOKEN_ELSE);
            node->right = Parser_statement(parser);
        }
    } else if (token->type == TOKEN_WHILE) {
        Parser_eat(parser, token, TOKEN_WHILE);
        
        node->token = token;
        
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, '(');
        Node *cond = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ')');
        
        node->cond = cond;
        node->right = NULL;
        
        node->left = Parser_statement(parser);
    } else if (token->type == TOKEN_FOR) {
        Parser_eat(parser, token, TOKEN_FOR);
        
        node->token = token;
        
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, '(');
        Node *init = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
        Node *cond = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
        Node *iter = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ')');
        
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->cond = cond;
        tmp->left = init;
        tmp->right = iter;
        tmp->scope = NULL;
        tmp->token = Arena_alloc(parser->lexer.token_arena, sizeof(Token));
        tmp->token->type = TOKEN_FOR_COND;
        
        node->cond = tmp;
        node->right = NULL;
        
        node->left = Parser_statement(parser);
    } else if (token->type == TOKEN_DO) {
        Parser_eat(parser, token, TOKEN_DO);
        
        node->token = token;
        
        token = Lexer_peekNextToken(&parser->lexer);
        
        node->left = Parser_statement(parser);
        
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, TOKEN_WHILE);
        
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, '(');
        Node *cond = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ')');
        token = Lexer_peekNextToken(&parser->lexer);
        
        node->cond = cond;
    } else if (token->type == TOKEN_RETURN) {
        Parser_eat(parser, token, TOKEN_RETURN);
        
        node->token = token;
        //token = Lexer_peekNextToken(&parser->lexer);
        node->left = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
    } else if (token->type == TOKEN_CONTINUE) {
        Parser_eat(parser, token, TOKEN_CONTINUE);
        
        node->token = token;
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
    } else if (token->type == TOKEN_BREAK) {
        Parser_eat(parser, token, TOKEN_BREAK);
        
        node->token = token;
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
    } else if (token->type == '{') {
        Lexer_resetPeek(&parser->lexer);
        node = Parser_block(parser);
    } else {
        // we aren't using the node we allocated up there, but since our arena doesn't support freeing memory at the moment, we won't free it here
        Lexer_resetPeek(&parser->lexer);
        node = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, token, ';');
    }
    
    Lexer_resetPeek(&parser->lexer);
    
    return node;
}

Node *Parser_topLevel(Parser *parser) {
    Declaration *decl = Parser_declaration(parser, true);
    Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
    tmp->token = Arena_alloc(parser->lexer.token_arena, sizeof(Token));
    tmp->token->type = TOKEN_DECLARATION;
    tmp->token->name = decl->name;
    tmp->left = NULL;
    tmp->right = NULL;
    tmp->scope = NULL;
    return tmp;
}

Node *Parser_parse(Parser *parser) {
    Node *node = Parser_topLevel(parser);
    Token *token = Lexer_peekNextToken(&parser->lexer);
    
    while (token->type != TOKEN_ERROR) {
        parser->lexer.peek = parser->lexer.pos;
        Node *tmp = Arena_alloc(parser->arena, sizeof(Node));
        tmp->left = node;
        tmp->token = Arena_alloc(parser->lexer.token_arena, sizeof(Token));
        tmp->token->type = TOKEN_NEXT;
        tmp->right = Parser_topLevel(parser);
        tmp->scope = NULL;
        node = tmp;
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    return node;
}

#endif // PARSER_H