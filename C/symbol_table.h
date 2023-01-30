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

static const double resize_threshold = 0.7;
static const u64 NEW_SCOPE_CAPACITY = 10;

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

STRUCT(Scope, {
    struct Scope *previous;
    
    SymbolTableEntry *hash_table;
    u64 size;
    u64 capacity;
});

STRUCT(SymbolTable, {
    Arena *scopes_arena;
    Scope *scope;
});

void Scope_init(Scope *scope, u64 capacity) {
    scope->capacity = capacity;
    scope->size = 0;
    scope->hash_table = malloc(sizeof(SymbolTableEntry) * capacity); // use calloc instead?
    scope->previous = NULL;
    for (u64 i = 0; i < scope->capacity; ++i) {
        scope->hash_table[i].name.count = 0; // this will tell us whether the cell is occupied
        // I know it's stupid, but it is what it is
    }
}

void SymbolTable_pushScope(SymbolTable *st) {
    Scope *new_scope = Arena_alloc(st->scopes_arena, sizeof(Scope));
    Scope_init(new_scope, NEW_SCOPE_CAPACITY);
    new_scope->previous = st->scope;
    st->scope = new_scope;
}

void SymbolTable_popScope(SymbolTable *st) {
    // NOTE(mdizdar): we don't want to delete the reference to the scope because the data is used in other parts, such as the Declaration struct
    st->scope = st->scope->previous;
}

void SymbolTable_init(SymbolTable *st, u64 capacity) {
    st->scopes_arena = Arena_init(4096);
    st->scope = Arena_alloc(st->scopes_arena, sizeof(Scope));
    Scope_init(st->scope, capacity);
}

u64 Scope_hash(const Scope *scope, const String *name) {
    u64 result = 0;
    u64 ppow = 1;
    for (u64 i = 0; i < name->count; ++i) {
        result = (result + (name->data[i] - '0' + 1) * ppow) % scope->capacity;
        ppow = (ppow * 79) % scope->capacity; // a small prime roughly the size of the alphabet [_a-zA-Z0-9]
    }
    return result;
}

u64 SymbolTable_hash(const SymbolTable *st, const String *name) {
    return Scope_hash(st->scope, name);
}

// returns how much we had to travel to find a vacant cell
u64 SymbolTable_add_helper(SymbolTable *st, const String *name, Type *type, u64 definition_line, u64 definition_column) {
    u64 hash = SymbolTable_hash(st, name);
    
    u64 travel = 0;
    // NOTE(mdizdar): this is checking whether a hash is in use
    while (st->scope->hash_table[hash].name.count != 0) {
        if (strcmp(st->scope->hash_table[hash].name.data, name->data) == 0) {
            // it's already in the table
            return 0;
        }
        hash = (hash+1)%st->scope->capacity;
        ++travel;
    }
    st->scope->hash_table[hash] = (SymbolTableEntry){
        .type = type,
        .definition_line = definition_line,
        .definition_column = definition_column,
        .name.count = name->count
    };
    st->scope->hash_table[hash].name.data = malloc(name->count);
    strcpy(st->scope->hash_table[hash].name.data, name->data);
    
    ++st->scope->size;
    
    return travel;
}

void SymbolTable_resize(SymbolTable *st) {
    SymbolTableEntry *old_table = st->scope->hash_table;
    u64 old_capacity = st->scope->capacity;
    SymbolTable_init(st, st->scope->capacity*2);
    for (u64 i = 0; i < old_capacity; ++i) {
        if (old_table[i].name.count == 0) continue;
        SymbolTable_add_helper(st, &old_table[i].name, old_table[i].type, old_table[i].definition_line, old_table[i].definition_column);
    }
    // free(old_table); // NOTE(mdizdar): freeing is expensive, for now we don't care
}

void SymbolTable_add(SymbolTable *st, const String *name, Type *type, u64 definition_line, u64 definition_column) {
    // assumes the table has been init-ed, if it hasn't it WILL explode
    const u64 travel = SymbolTable_add_helper(st, name, type, definition_line, definition_column);
    
    if ((travel > st->scope->capacity/2) || (1.0 * st->scope->size / st->scope->capacity > resize_threshold)) {
        SymbolTable_resize(st);
    }
}

SymbolTableEntry *Scope_shallow_find(const Scope *scope, const String *name) {
    u64 hash = Scope_hash(scope, name);
    
    u64 travel = 0;
    // check the hash exists
    while (scope->hash_table[hash].name.count != 0) {
        // check the hash is correct
        if (strcmp(scope->hash_table[hash].name.data, name->data) == 0) {
            return scope->hash_table + hash;
        }
        hash = (hash+1)%scope->capacity;
        ++travel;
    }

    return NULL;
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
