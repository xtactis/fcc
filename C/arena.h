/* date = December 30th 2020 9:36 pm */

#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "../utils/common.h"

// TODO(mdizdar): we are currently not concerned with freeing individual pieces of memory given out
// TODO(mdizdar): add alignment and stuff

typedef struct _Arena {
    u8 *data;
    u64 capacity;
    u64 used;
    struct _Arena *prev;
} _Arena;

typedef struct {
    _Arena *current;
    u64 total_capacity;
} Arena;

_Arena *_Arena_init(u64 capacity, _Arena *prev);
Arena *Arena_init(u64 capacity);
void *Arena_alloc(Arena *arena, u64 size);
void _Arena_freeall(_Arena *arena);
void Arena_freeall(Arena *arena);

#endif //ARENA_H
