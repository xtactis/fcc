/* date = December 29th 2020 0:02 am */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string.h>
#include <stdlib.h>

#include "../utils/common.h"
#include "arena.h"
#include "scope.h"

STRUCT_HEADER(SymbolTable, {
    Arena *scopes_arena;
    Scope *scope;
});

void SymbolTable_pushScope(SymbolTable *st);
void SymbolTable_popScope(SymbolTable *st);
void SymbolTable_init(SymbolTable *st);
void SymbolTable_add(SymbolTable *st, const String *name, Type *type, u64 definition_line, u64 definition_column);
SymbolTableEntry *SymbolTable_find(const SymbolTable *st, const String *name);
SymbolTableEntry *SymbolTable_shallow_find(const SymbolTable *st, const String *name);
SymbolTableEntry *SymbolTable_find_cstr(SymbolTable *st, char *name);
SymbolTableEntry *SymbolTable_shallow_find_cstr(const SymbolTable *st, char *name);
SymbolTableEntry *SymbolTable_shallow_find_before(const SymbolTable *st, const String *name, u64 line, u64 col);
SymbolTableEntry *SymbolTable_find_before(const SymbolTable *st, const String *name, u64 line, u64 col);
SymbolTableEntry *SymbolTable_shallow_find_before_cstr(const SymbolTable *st, char *name, u64 line, u64 col);
SymbolTableEntry *SymbolTable_find_before_cstr(const SymbolTable *st, char *name, u64 line, u64 col);

#endif //SYMBOL_TABLE_H
