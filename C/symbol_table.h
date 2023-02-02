/* date = December 29th 2020 0:02 am */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string.h>
#include <stdlib.h>

#include "../utils/common.h"
#include "arena.h"

// NOTE(mdizdar): forward declarations because of cyclic imports
struct Type;
typedef struct Type Type;
char *Type_toStr(char *, const Type *, bool, u64);

STRUCT(Address, {
    bool global;
    u64 offset;
});

STRUCT(SymbolTableEntry, {
    String name;
    Type *type;
    u64 definition_line;
    u64 definition_column;
    u64 temporary_id; // NOTE(mdizdar): this is used in IR generation to keep track
    Address location_in_memory;

    bool is_typename;
});

char *SymbolTableEntry_toStr(char *s, const SymbolTableEntry *entry) {
    char ts[256];
    sprintf(s, "name: %s; type: %s; line: %lu; col: %lu; typename?: %u", 
            entry->name.data, Type_toStr(ts, entry->type, false, 0), entry->definition_line, entry->definition_column, entry->is_typename);
    return s;
}

bool SymbolTableEntry_eq(const SymbolTableEntry *a, const SymbolTableEntry *b) {
    return String_eq(&a->name, &b->name);
}

void SymbolTableEntry_copy(SymbolTableEntry *dest, const SymbolTableEntry *src, ...) {
    *dest = (SymbolTableEntry){
        .type               = src->type,
        .definition_line    = src->definition_line,
        .definition_column  = src->definition_column,
        .location_in_memory = src->location_in_memory,
        .temporary_id       = src->temporary_id,
        .is_typename        = src->is_typename
    };
    String_copy(&dest->name, &src->name);
}

_generate_hash_map(String, SymbolTableEntry);

STRUCT(Scope, {
    struct Scope *previous;
    
    StringSymbolTableEntryHashMap hash_table;
});

STRUCT(SymbolTable, {
    Arena *scopes_arena;
    Scope *scope;
});

void Scope_init(Scope *scope) {
    scope->previous = NULL;
    StringSymbolTableEntryHashMap_construct(&scope->hash_table);
}

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
#endif //SYMBOL_TABLE_H
