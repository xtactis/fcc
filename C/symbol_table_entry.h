#ifndef SYMBOL_TABLE_ENTRY_H
#define SYMBOL_TABLE_ENTRY_H

#include "../utils/common.h"
#include "line_tid.h"
#include "address.h"

struct SymbolTableEntry;
typedef struct SymbolTableEntry SymbolTableEntry;

#include "type.h"

STRUCT_HEADER(SymbolTableEntry, {
    String name;
    Type *type;
    u64 definition_line;
    u64 definition_column;
    LineTemporaryIDArray all_temp_ids; // indexed by line number in IR
    TemporaryID temporary_id; // NOTE(mdizdar): this is used in IR generation to keep track
    Address location_in_memory;

    bool is_typename;
});

char *SymbolTableEntry_toStr(char *s, const SymbolTableEntry *entry);
void SymbolTableEntry_print(const SymbolTableEntry *entry);
bool SymbolTableEntry_eq(const SymbolTableEntry *a, const SymbolTableEntry *b);
void SymbolTableEntry_copy(SymbolTableEntry *dest, const SymbolTableEntry *src, ...);

#endif //SYMBOL_TABLE_ENTRY_H
