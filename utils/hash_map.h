#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <string.h>
#include <assert.h>
#include "common.h"

#define _generate_hash_map(key_type, value_type) \
    typedef struct key_type##value_type##KVPair { \
        key_type key; \
        value_type value; \
    } \
    \
    typedef struct key_type##value_type##HashMap { \
        key_type##value_type##KVPair *table; \
        bool *occupied; \
        u64 size; \
        u64 capacity; \
        double resize_threshold; \
        u64 (* hash_function)(const key_type *, ...); \
        bool (* eq_function)(const value_type *, const value_type *, ...); \
    } key_type##value_type##HashMap; \
    \
    void key_type##value_type##HashMap_construct(key_type##value_type##HashMap *map, \
                                                 u64 (* hash_function)(const key_type *, ...), \
                                                 bool (* eq_function)(const value_type *, const value_type *, ...)) { \
        map->capacity = 0; \
        map->size = 0; \
        map->table = NULL; \
        map->occupied = NULL; \
        map->resize_threshold = 0.7; \
        map->hash_function = hash_function; \
        map->eq_function = eq_function; \
    } \
    \
    void key_type##value_type##HashMap_destruct(key_type##value_type##HashMap *map) { \
        if (map->table != NULL) { \
            free(map->table); \
        } \
    } \
    \
    u64 key_type##value_type##HashMap_add_helper(key_type##value_type##HashMap *map, const key_type *key, const value_type *value) { \
        u64 hash = map->hash_function(key) % map->capacity; \
        \
        u64 travel = 0; \
        /* NOTE(mdizdar): this is checking whether a hash is in use */ \
        while (map->occupied[hash] != 0) { \
            /* TODO(mdizdar): no idea how to do `_eq` in a good way, might just generate it externally */ \
            if (eq_function(&map->table[hash].value, value)) { \
                /* it's already in the table */ \
                return 0; \
            } \
            hash = (hash+1) % map->capacity; \
            ++travel; \
        } \
        memcpy(&map->table[hash], (key_type##value_type##KVPair) {key, value}); \
        \
        ++st->scope->size; \
        \
        return travel; \
    } \
    \
    void key_type##value_type##HashMap_resize(key_type##value_type##HashMap *map) { \
        key_type##value_type##KVPair *old_table = map->table; \
        u64 old_capacity = map->capacity; \
        key_type##value_type##HashMap_construct(map, map->hash_function, map->eq_function); \
        map->capacity = old_capacity * 2; \
        if (old_capacity == 0) { \
            map->capacity = 10; \
        } \
        \
        map->table = calloc(sizeof(key_type##value_type##KVPair), map->capacity); \
        for (u64 i = 0; i < old_capacity; ++i) { \
            if (map->occupied[i] == 0) continue; \
            key_type##value_type##HashMap_add_helper(map, &old_table[i].key, &old_table[i].value); \
        } \
        \
    }




#define _generate_dynamic_array(name) \
    typedef struct name##Array { \
        name *data; \
        u64 count; \
        u64 capacity; \
    } name##Array; \
    \
    name##Array *name##Array_push_ptr(name##Array *array, name *element) { \
        if (!array->capacity) { \
            array->data = malloc(sizeof(name) * 2); \
            memcpy(array->data, element, sizeof(name)); \
            array->count = 1; \
            array->capacity = 2; \
            return array; \
        } \
        if (array->capacity == array->count) { \
            array->capacity += array->capacity; \
            array->data = realloc(array->data, array->capacity * sizeof(name)); \
        } \
        /* I'm still unsure if memcpy is the thing we want to be doing here */ \
        memcpy(array->data + array->count, element, sizeof(name)); \
        ++array->count; \
        return array; \
    } \
    \
    /* pop back actually just reduces the count, we never free the memory */ \
    name *name##Array_pop_back(name##Array *array) { \
        assert(array->count > 0); \
        --array->count; \
        return array->data + array->count; \
    } \
    \
    name##Array *name##Array_push_back(name##Array *array, name element) { \
        return name##Array_push_ptr(array, &element); \
    } \
    \
    name *name##Array_at(const name##Array *array, u64 index) { \
        assert(index < array->count); \
        return array->data + index; \
    } \
    \
    name *name##Array_front(const name##Array *array) { \
        assert(array->count > 0); \
        return name##Array_at(array, 0); \
    } \
    \
    name *name##Array_back(const name##Array *array) { \
        assert(array->count > 0); \
        return name##Array_at(array, array->count-1); \
    } \
    \
    name *name##Array_begin(const name##Array *array) { \
        if (array->count == 0) return NULL; \
        return name##Array_at(array, 0); \
    } \
    \
    name *name##Array_end(const name##Array *array) { \
        if (array->count == 0) return NULL; \
        return array->data+array->count; \
    } \
    \
    name *name##Array_next(const name##Array *array, name *el) { \
        assert(el >= array->data); \
        assert(el < array->data+array->count); \
        return ++el; \
    } \
    \
    name *name##Array_rbegin(const name##Array *array) { \
        if (array->count == 0) return NULL; \
        return name##Array_back(array); \
    } \
    \
    name *name##Array_rend(const name##Array *array) { \
        if (array->count == 0) return NULL; \
        return array->data-1; \
    } \
    \
    name *name##Array_previous(const name##Array *array, name *el) { \
        assert(el >= array->data); \
        assert(el < array->data+array->count); \
        return --el; \
    } \
    \
    void name##Array_erase(name##Array *array, u64 index) { \
        assert(array->count > 0); \
        --array->count; \
        if (array->count == index) return; \
        memcpy(array->data + index, array->data + index + 1, sizeof(name) * (array->count - index)); \
    } \
    \
    void name##Array_reserve(name##Array *array, u64 new_cap) { \
        array->capacity = new_cap; \
        if (array->data) { \
            array->data = realloc(array->data, array->capacity * sizeof(name)); \
        } else { \
            array->data = malloc(array->capacity * sizeof(name)); \
        } \
    } \
    \
    void name##Array_clear(name##Array *array) { \
        array->count = 0; \
    } \
    \
    void name##Array_shrink_to_fit(name##Array *array) { \
        array->capacity = array->count; \
        array->data = realloc(array->data, array->count * sizeof(name)); \
    } \
    \
    void name##Array_construct(name##Array *array) { \
        array->capacity = 0; \
        array->count = 0; \
        array->data = NULL; \
    } \
    \
    void name##Array_destruct(name##Array *array) { \
        assert(array); \
        if (array->data) { \
            free(array->data); \
        } \
    } \
    \
    name##Array *name##Array_copy(name##Array *dest, const name##Array *source) { \
        name##Array_clear(dest); \
        FOR_EACH (name, it, source) { \
            name##Array_push_ptr(dest, it); \
        } \
        return dest; \
    }

#define FOR_EACH(type, it, array) \
    for (type *it = type##Array_begin(array); \
            it != type##Array_end(array); \
            it = type##Array_next(array, it))

#define FOR_EACH_REV(type, it, array) \
    for (type *it = type##Array_rbegin(array); \
            it != type##Array_rend(array); \
            it = type##Array_previous(array, it))

_generate_type(u8);
_generate_type(u16);
_generate_type(u32);
_generate_type(u64);
_generate_type(s8);
_generate_type(s16);
_generate_type(s32);
_generate_type(s64);
_generate_type(b8);
_generate_type(b16);
_generate_type(b32);
_generate_type(b64);
_generate_type(bool);
_generate_type(char);
_generate_type(int);
_generate_type(f32);
_generate_type(f64);

#endif // HASH_MAP_H
