#include "scope.h"

_generate_hash_map_header(String, SymbolTableEntry);

STRUCT_SOURCE(Scope);

void Scope_init(Scope *scope) {
    scope->previous = NULL;
    StringSymbolTableEntryHashMap_construct(&scope->hash_table);
}

SymbolTableEntry *Scope_shallow_find(const Scope *scope, const String *name) {
    return StringSymbolTableEntryHashMap_get(&scope->hash_table, name);
}

SymbolTableEntry *Scope_find(const Scope *scope, const String *name) {
    SymbolTableEntry *maybe_found = Scope_shallow_find(scope, name);
    
    if (maybe_found != NULL) {
        return maybe_found;
    }

    if (scope->previous != NULL) {
        return Scope_find(scope->previous, name);
    }
    
    return NULL;
}

SymbolTableEntry *Scope_shallow_find_before(const Scope *scope, const String *name, u64 line, u64 col) {
    SymbolTableEntry *maybe_found = Scope_shallow_find(scope, name);
    
    if (maybe_found == NULL) {
        return NULL;
    }
    if (maybe_found->definition_line > line) {
        return NULL;
    }
    if (maybe_found->definition_line == line && maybe_found->definition_column > col) {
        return NULL;
    }

    return maybe_found;
}

SymbolTableEntry *Scope_find_before(const Scope *scope, const String *name, u64 line, u64 col) {
    SymbolTableEntry *maybe_found = Scope_shallow_find_before(scope, name, line, col);

    if (maybe_found != NULL) {
        return maybe_found;
    }

    if (scope->previous != NULL) {
        return Scope_find(scope->previous, name);
    }

    return NULL;
}
