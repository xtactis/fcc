#include "symbol_table.h"

STRUCT_SOURCE(SymbolTable);

void SymbolTable_pushScope(SymbolTable *st) {
    Scope *new_scope = Arena_alloc(st->scopes_arena, sizeof(Scope));
    Scope_init(new_scope);
    new_scope->previous = st->scope;
    st->scope = new_scope;
}

void SymbolTable_popScope(SymbolTable *st) {
    // NOTE(mdizdar): we don't want to delete the reference to the scope because the data is used in other parts, such as the Declaration struct
    st->scope = st->scope->previous;
}

void SymbolTable_init(SymbolTable *st) {
    st->scopes_arena = Arena_init(4096);
    st->scope = Arena_alloc(st->scopes_arena, sizeof(Scope));
    Scope_init(st->scope);
}

void SymbolTable_add(SymbolTable *st, const String *name, Type *type, u64 definition_line, u64 definition_column) {
    SymbolTableEntry ste = {
        .type = type,
        .definition_line = definition_line,
        .definition_column = definition_column
    };
    String_construct(&ste.name);
    String_copy(&ste.name, name);
    StringSymbolTableEntryHashMap_add(&st->scope->hash_table, name, &ste);
}

SymbolTableEntry *SymbolTable_find(const SymbolTable *st, const String *name) {
    return Scope_find(st->scope, name);
}

SymbolTableEntry *SymbolTable_shallow_find(const SymbolTable *st, const String *name) {
    return Scope_shallow_find(st->scope, name);
}

SymbolTableEntry *SymbolTable_find_cstr(SymbolTable *st, char *name) {
    const String sname = (String){.data = name, .count = strlen(name)};
    return SymbolTable_find(st, &sname);
}

SymbolTableEntry *SymbolTable_shallow_find_cstr(const SymbolTable *st, char *name) {
    const String sname = (String){.data = name, .count = strlen(name)};
    return SymbolTable_shallow_find(st, &sname);
}

SymbolTableEntry *SymbolTable_shallow_find_before(const SymbolTable *st, const String *name, u64 line, u64 col) {
    return Scope_shallow_find_before(st->scope, name, line, col);
}

SymbolTableEntry *SymbolTable_find_before(const SymbolTable *st, const String *name, u64 line, u64 col) {
    return Scope_find_before(st->scope, name, line, col);
}

SymbolTableEntry *SymbolTable_shallow_find_before_cstr(const SymbolTable *st, char *name, u64 line, u64 col) {
    const String sname = (String){.data = name, .count = strlen(name)};
    return SymbolTable_shallow_find_before(st, &sname, line, col);
}

SymbolTableEntry *SymbolTable_find_before_cstr(const SymbolTable *st, char *name, u64 line, u64 col) {
    const String sname = (String){.data = name, .count = strlen(name)};
    return SymbolTable_find_before(st, &sname, line, col);
}
