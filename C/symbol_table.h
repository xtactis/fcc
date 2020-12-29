/* date = December 29th 2020 0:02 am */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string.h>
#include <stdlib.h>

typedef struct {
    String name;
    u64 type;
    u64 definition_line;
} SymbolTableEntry;

typedef struct {
    SymbolTableEntry *hash_table;
    u64 size;
    u64 capacity;
    const double resize_threshold;
} SymbolTable;

void SymbolTable_init(SymbolTable *st, u64 capacity) {
    st->capacity = capacity;
    st->size = 0;
    st->hash_table = malloc(sizeof(SymbolTableEntry) * capacity); // use calloc instead?
    for (u64 i = 0; i < st->capacity; ++i) {
        st->hash_table[i].name.count = 0; // this will tell us whether the cell is occupied
        // I know it's stupid, but it is what it is
    }
}

u64 SymbolTable_hash(const SymbolTable *st, const String *name) {
    u64 result = 0;
    u64 ppow = 1;
    for (u64 i = 0; i < name->count; ++i) {
        result = (result + (name->data[i] - '0' + 1) * ppow) % st->capacity;
        ppow = (ppow * 79) % st->capacity; // a small prime roughly the size of the alphabet [_a-zA-Z0-9]
    }
    return result;
}

// returns how much we had to travel to find a vacant cell
u64 SymbolTable_add_helper(SymbolTable *st, const String *name, u64 type, u64 definition_line) {
    u64 hash = SymbolTable_hash(st, name);
    
    u64 travel = 0;
    // NOTE(mdizdar): this is checking whether a hash is in use
    while (st->hash_table[hash].name.count != 0) {
        if (strcmp(st->hash_table[hash].name.data, name->data) == 0) {
            // it's already in the table
            return 0;
        }
        hash = (hash+1)%st->capacity;
        ++travel;
    }
    st->hash_table[hash] = (SymbolTableEntry){.type = type, .definition_line = definition_line, .name.count = name->count};
    st->hash_table[hash].name.data = malloc(name->count);
    strcpy(st->hash_table[hash].name.data, name->data);
    
    ++st->size;
    
    return travel;
}

void SymbolTable_resize(SymbolTable *st) {
    SymbolTableEntry *old_table = st->hash_table;
    u64 old_capacity = st->capacity;
    SymbolTable_init(st, st->capacity*2);
    for (u64 i = 0; i < old_capacity; ++i) {
        if (old_table[i].name.count == 0) continue;
        SymbolTable_add_helper(st, &old_table[i].name, old_table[i].type, old_table[i].definition_line);
    }
}

void SymbolTable_add(SymbolTable *st, const String *name) {
    // assumes the table has been init-ed, if it hasn't it WILL explode
    const u64 travel = SymbolTable_add_helper(st, name, -1, -1);
    
    if ((travel > st->capacity/2) || (1.0 * st->size / st->capacity > st->resize_threshold)) {
        SymbolTable_resize(st);
    }
}

const SymbolTableEntry *SymbolTable_find(SymbolTable *st, const String *name) {
    u64 hash = SymbolTable_hash(st, name);
    
    u64 travel = 0;
    // check the hash exists
    while (st->hash_table[hash].name.count != 0) {
        // check the hash is correct
        if (strcmp(st->hash_table[hash].name.data, name->data) == 0) {
            return st->hash_table + hash;
        }
        hash = (hash+1)%st->capacity;
        ++travel;
    }
    
    // NOTE(mdizdar): not sure if I want this here, but I'll include it for now
    if (travel > st->capacity/2) {
        SymbolTable_resize(st);
    }
    
    return NULL;
}

const SymbolTableEntry *SymbolTable_find_cstr(SymbolTable *st, char *name) {
    const String sname = (String){.data = name, .count = strlen(name)};
    return SymbolTable_find(st, &sname);
}

#endif //SYMBOL_TABLE_H
