#ifndef LEXER_H
#define LEXER_H

#include "../utils/common.h"
#include "cached_token.h"
#include "arena.h"

STRUCT_HEADER(Lexer, {
    String code;
    
    CachedToken *token_at;
    Arena *token_arena;
    
    u64 cur_col;
    u64 prev_col;
    u64 cur_line;
    u64 prev_line;
    u64 pos;
    u64 peek;
});

TokenType checkKeyword(const char *name);
Token *Lexer_returnToken(Lexer *lexer, u64 lookahead, Token *t);
Token *Lexer_currentToken(Lexer *lexer);
Token *Lexer_currentPeekedToken(Lexer *lexer);
void Lexer_resetPeek(Lexer *lexer);
void Lexer_confirmPeek(Lexer *lexer);
// finds the next token and returns it
Token *Lexer_peekNextToken(Lexer *lexer);
// consumes peeked tokens
void Lexer_eat(Lexer *lexer);
// finds and consumes the next token, then returns it
Token *Lexer_getNextToken(Lexer *lexer);
#endif //LEXER_H
