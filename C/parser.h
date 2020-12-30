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

// TODO(mdizdar): add caching of already lexed tokens

typedef struct {
    SymbolTable *symbol_table;
    String code;
    
    u64 cur_line;
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

// finds the next token and returns it
Token Lexer_peekNextToken(Lexer *lexer) {
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
    
    for (u64 lookahead = lexer->peek; lookahead < lexer->code.count; ++lookahead) {
        char c = lexer->code.data[lookahead];
        //printf("%llu %c\n", lookahead, c);
        switch (state) {
            case UNKNOWN: {
                if (isspace(c)) {
                    if (c == '\n') ++lexer->cur_line;
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
                } else {
                    state = OP;
                }
                break;
            }
            case IDENT: {
                if (c == '_' || isalnum(c)) continue;
                Token t;
                u64 count = lookahead - lexer->peek;
                char *name = malloc(count+1);
                name[count] = 0;
                strncpy(name, lexer->code.data + lexer->peek, count);
                t.type = checkKeyword(name);
                if (t.type == TOKEN_IDENT) {
                    t.name = (String){.data = name, .count = count};
                    
                    // NOTE(mdizdar): type is -1 because we don't know it yet
                    SymbolTable_add(lexer->symbol_table, &t.name, -1, lexer->cur_line);
                }
                lexer->peek = lookahead;
                return t;
            }
            case DECINT: {
                if (was_zero) {
                    if (c == 'x' || c == 'X') {
                        state = HEXINT;
                    } else if (isdigit(c)) {
                        state = OCTINT;
                        --lookahead;
                    } else if (isalpha(c)) {
                        error("Unexpected alpha character (necu ti reci gdje idiote)");
                    } else {
                        lexer->peek = lookahead;
                        return (Token){.type = TOKEN_INT_LITERAL, .integer_value = 0};
                    }
                } else {
                    if (isdigit(c)) {
                        integer_value *= 10;
                        integer_value += c - '0';
                    } else if (isalpha(c)) {
                        // TODO(mdizdar): 'L' and 'LL' at the end are allowed though
                        error("Unexpected alpha character (necu ti reci gdje idiote)");
                    } else if (c == '.') {
                        double_value = integer_value;
                        state = FLOAT;
                    } else {
                        lexer->peek = lookahead; // NOTE(mdizdar): instead of making sure this is always here, write a function that'll create and return the token, while making sure the state is corrected accordingly. this will also allow us to fill in any auxilliary information e.g. the line and column number.
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
                    lexer->peek = lookahead;
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
                    lexer->peek = lookahead;
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
                    lexer->peek = lookahead;
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
                    lexer->peek = lookahead;
                    return (Token){.type = prev};
                } else if (lookahead - lexer->peek == 1) {
                    if ((c != '>' || prev != '>') && (c != '<' || prev != '<')) {
                        for (u32 i = 0; i < sizeof(MULTI_OPS)/sizeof(char *) - 4; ++i) {
                            if (prev == MULTI_OPS[i][0] && c == MULTI_OPS[i][1]) {
                                lexer->peek = lookahead+1;
                                return (Token){.type = TOKEN_OPERATOR+i+1}; // NOTE(mdizdar): this is a disgusting error prone hack, but it works...
                            }
                        }
                        lexer->peek = lookahead;
                        return (Token){.type = prev};
                    }
                } else {
                    if (c == '=') {
                        lexer->peek = lookahead+1;
                        if (prev == '<') {
                            return (Token){.type = TOKEN_BIT_L_ASSIGN};
                        } else {
                            return (Token){.type = TOKEN_BIT_R_ASSIGN};
                        }
                    } else {
                        lexer->peek = lookahead;
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
                    u64 count = lookahead - lexer->peek - 1;
                    char *str = malloc(count+1);
                    str[count] = 0;
                    strncpy(str, lexer->code.data + lexer->peek + 1, count);
                    lexer->peek = lookahead+1;
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

// consumes peeked tokens
void Lexer_eat(Lexer *lexer) {
    lexer->pos = lexer->peek;
}

// finds and consumes the next token, then returns it
Token Lexer_getNextToken(Lexer *lexer) {
    Token t = Lexer_peekNextToken(lexer);
    Lexer_eat(lexer);
    return t;
}

//~ PARSER

typedef struct {
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
    error("[Line %llu] Syntax error: expected token of type %llu, but got %llu", parser->lexer.cur_line, expected_type, token->type);
}

// TODO(mdizdar): this is stupid, split everything into header and code files
Node *Parser_expr(Parser *parser);

void Parser_eat(Parser *parser, Token *token, TokenType token_type) {
    if (token->type == token_type) {
        Lexer_eat(&parser->lexer);
    } else {
        Parser_error(parser, token, token_type);
    }
}

Node *Parser_operand(Parser *parser) {
    // factor ::= ident | integer | '(' expr ')'
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    if (token.type == TOKEN_INT_LITERAL) {
        Parser_eat(parser, &token, TOKEN_INT_LITERAL);
        Node *node = malloc(sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        return node;
    } else if (token.type == TOKEN_IDENT) {
        Parser_eat(parser, &token, TOKEN_IDENT);
        Node *node = malloc(sizeof(Node));
        node->token = token;
        node->left = NULL;
        node->right = NULL;
        return node;
    } else if (token.type == '(') {
        Parser_eat(parser, &token, '(');
        Node *node = Parser_expr(parser);
        token = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, &token, ')');
        return node;
    }
    return NULL;
}

Node *Parser_muls(Parser *parser) {
    // term ::= factor [('*'|'/'|'%') factor]*
    
    Node *node = Parser_operand(parser);
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    while (token.type == '*' || token.type == '/' || token.type == '%') {
        if (token.type == '*') {
            Parser_eat(parser, &token, '*');
        } else if (token.type == '/') {
            Parser_eat(parser, &token, '/');
        } else if (token.type == '%') {
            Parser_eat(parser, &token, '%');
        }
        
        Node *tmp = malloc(sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_operand(parser);
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    parser->lexer.peek = parser->lexer.pos; // NOTE(mdizdar): maybe make a Lexer_reset() for this
    
    return node;
}

Node *Parser_sums(Parser *parser) {
    // expr ::= term [('+'|'-') term]*
    
    Node *node = Parser_muls(parser);
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    while (token.type == '+' || token.type == '-') {
        if (token.type == '+') {
            Parser_eat(parser, &token, '+');
        } else if (token.type == '-') {
            Parser_eat(parser, &token, '-');
        }
        Node *tmp = malloc(sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_muls(parser);
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    parser->lexer.peek = parser->lexer.pos;
    
    return node;
}

// TODO(mdizdar): more things in between these two actually

Node *Parser_ternary(Parser *parser) {
    Node *node = Parser_sums(parser);
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    while (token.type == '?') {
        Parser_eat(parser, &token, '?');
        Node *ternary_true = Parser_expr(parser);
        Token colon = Lexer_peekNextToken(&parser->lexer);
        Parser_eat(parser, &colon, ':');
        
        Node *tmp = malloc(sizeof(Node));
        tmp->cond = node;
        tmp->left = ternary_true;
        tmp->token = token;
        tmp->right = Parser_sums(parser);
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    parser->lexer.peek = parser->lexer.pos;
    
    return node;
}

Node *Parser_assignment(Parser *parser) {
    Node *node = Parser_ternary(parser);
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    // TODO(mdizdar): doing this exact same thing for each level is kinda stupid, refactor it eventually
    
    while (token.type == '=' || 
           token.type == TOKEN_ADD_ASSIGN || token.type == TOKEN_SUB_ASSIGN ||
           token.type == TOKEN_MUL_ASSIGN || token.type == TOKEN_DIV_ASSIGN || 
           token.type == TOKEN_MOD_ASSIGN || token.type == TOKEN_OR_ASSIGN  || 
           token.type == TOKEN_AND_ASSIGN || token.type == TOKEN_XOR_ASSIGN || 
           token.type == TOKEN_BIT_L_ASSIGN || token.type == TOKEN_BIT_R_ASSIGN) {
        if (token.type == '=') {
            Parser_eat(parser, &token, '=');
        } else if (token.type == TOKEN_ADD_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_ADD_ASSIGN);
        } else if (token.type == TOKEN_SUB_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_SUB_ASSIGN);
        } else if (token.type == TOKEN_MUL_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_MUL_ASSIGN);
        } else if (token.type == TOKEN_DIV_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_DIV_ASSIGN);
        } else if (token.type == TOKEN_MOD_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_MOD_ASSIGN);
        } else if (token.type == TOKEN_OR_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_OR_ASSIGN);
        } else if (token.type == TOKEN_AND_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_AND_ASSIGN);
        } else if (token.type == TOKEN_XOR_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_XOR_ASSIGN);
        } else if (token.type == TOKEN_BIT_L_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_BIT_L_ASSIGN);
        } else if (token.type == TOKEN_BIT_R_ASSIGN) {
            Parser_eat(parser, &token, TOKEN_BIT_R_ASSIGN);
        }
        
        Node *tmp = malloc(sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_ternary(parser);
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    parser->lexer.peek = parser->lexer.pos;
    
    return node;
}

Node *Parser_comma(Parser *parser) {
    Node *node = Parser_assignment(parser);
    
    Token token = Lexer_peekNextToken(&parser->lexer);
    
    while (token.type == ',') {
        if (token.type == ',') {
            Parser_eat(parser, &token, ',');
        }
        
        Node *tmp = malloc(sizeof(Node));
        tmp->left = node;
        tmp->token = token;
        tmp->right = Parser_assignment(parser);
        node = tmp;
        
        token = Lexer_peekNextToken(&parser->lexer);
    }
    
    parser->lexer.peek = parser->lexer.pos;
    
    return node;
}

Node *Parser_expr(Parser *parser) {
    return Parser_comma(parser);
}

void Parser_parseStatement(Parser *parser) {
    NOT_IMPL;
}

Node *Parser_parse(Parser *parser) {
    // TODO(mdizdar): not just one expression.. not just expressions
    return Parser_expr(parser);
}

#endif // PARSER_H