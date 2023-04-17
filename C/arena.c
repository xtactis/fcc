#include "arena.h"

_Arena *_Arena_init(u64 capacity, _Arena *prev) {
    _Arena *arena = malloc(sizeof(_Arena));
    arena->prev = prev;
    arena->used = 0;
    arena->capacity = capacity;
    arena->data = malloc(capacity);
    return arena;
}

Arena *Arena_init(u64 capacity) {
    Arena *arena = malloc(sizeof(Arena));
    arena->current = _Arena_init(capacity, NULL);
    arena->total_capacity = capacity;
    
    return arena;
}

void *Arena_alloc(Arena *arena, u64 size) {
    _Arena *cur = arena->current;
    
    if (size + cur->used > cur->capacity) {
        u64 new_cap = max(arena->total_capacity, size); // NOTE(mdizdar): this is slightly dumb and I don't currently know how this affects alignment...
        arena->current = _Arena_init(new_cap, cur);
        arena->total_capacity += new_cap;
        cur = arena->current;
    }
    void *ptr = &cur->data[cur->used];
    cur->used += size;
    
    return ptr;
}

void _Arena_freeall(_Arena *arena) {
    free(arena);
}

void Arena_freeall(Arena *arena) {
    for (_Arena *prev = arena->current->prev; 
         arena->current != NULL; 
         arena->current = prev) {
        _Arena_freeall(arena->current);
    }
    free(arena);
}
