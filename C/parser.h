#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../utils/common.h"
#include "lexer.h"
#include "symbol_table.h"
#include "token.h"
#include "type.h"
#include "reserved.h"
#include "arena.h"
#include "node.h"

#include "../IR/IR.h"

#define parse_error error("Parse error!")

//#define Arena_alloc(x, y) malloc((y))

STRUCT_HEADER(Parser, {
    Arena *arena;
    Arena *type_arena;
    SymbolTable *symbol_table;
    Lexer lexer;
});

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

_Noreturn void Parser_error(Parser *parser, Token *token, TokenType expected_type);
_Noreturn void Parser_duplicateError(Parser *parser, SymbolTableEntry *previous);
_Noreturn void Parser_conflictingTypesError(Parser *parser, TokenType current, TokenType conflicting);
_Noreturn void Parser_notATypeError(Parser *parser, u64 longs, u64 shorts, TokenType type);
// TODO(mdizdar): compound literals
// TODO(mdizdar): the bodies of these precedence based functions are very similar, generalize maybe

void Parser_eat(Parser *parser, Token *token, TokenType token_type);
Node *Parser_operand(Parser *parser);
Node *Parser_postfix(Parser *parser);
Node *Parser_prefix(Parser *parser);
Node *Parser_muls(Parser *parser);
Node *Parser_sums(Parser *parser);
Node *Parser_bitshift(Parser *parser);
Node *Parser_rel_op(Parser *parser);
Node *Parser_rel_eq(Parser *parser);
Node *Parser_bit_and(Parser *parser);
Node *Parser_bit_xor(Parser *parser);
Node *Parser_bit_or(Parser *parser);
Node *Parser_log_and(Parser *parser);
Node *Parser_log_or(Parser *parser);
Node *Parser_ternary(Parser *parser);
Node *Parser_assignment(Parser *parser);
Node *Parser_comma(Parser *parser);
Node *Parser_expr(Parser *parser);
Node *Parser_functionBlock(Parser *parser);
Node *Parser_block(Parser *parser);
Declaration *Parser_struct(Parser *parser, Type **type);
Declaration *Parser_union(Parser *parser, Type **type);
Type *Parser_function(Parser *parser, Type *type);
Type *Parser_cvp(Parser *parser, Type *type);
Declaration *Parser_declaration(Parser *parser, bool can_be_static);
Node *Parser_statement(Parser *parser);
Node *Parser_topLevel(Parser *parser);
Node *Parser_parse(Parser *parser);

#endif // PARSER_H
