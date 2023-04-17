#include "symbol_table_entry.h"

STRUCT_SOURCE(SymbolTableEntry);

char *SymbolTableEntry_toStr(char *s, const SymbolTableEntry *entry) {
    char ts[256];
    sprintf(s, "name: %s; type: %s; line: %lu; col: %lu; typename?: %u", 
            entry->name.data, Type_toStr(ts, entry->type, false, 0), entry->definition_line, entry->definition_column, entry->is_typename);
    return s;
}

void SymbolTableEntry_print(const SymbolTableEntry *entry) {
    char s[512];
    printf("%s", SymbolTableEntry_toStr(s, entry));
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
