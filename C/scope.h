#ifndef SCOPE_H
#define SCOPE_H

#include "../utils/common.h"
#include "symbol_table_entry.h"

_generate_hash_map_header(String, SymbolTableEntry);

STRUCT_HEADER(Scope, {
    struct Scope *previous;
    
    StringSymbolTableEntryHashMap hash_table;
});

void Scope_init(Scope *scope);
SymbolTableEntry *Scope_shallow_find(const Scope *scope, const String *name);
SymbolTableEntry *Scope_find(const Scope *scope, const String *name);
SymbolTableEntry *Scope_shallow_find_before(const Scope *scope, const String *name, u64 line, u64 col);
SymbolTableEntry *Scope_find_before(const Scope *scope, const String *name, u64 line, u64 col);

#endif //SCOPE_H
