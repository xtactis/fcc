#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <string.h>
#include <assert.h>
#include "common.h"

#define _generate_hash_map(key_type, value_type) \
    typedef struct key_type##value_type##KVPair { \
        key_type key; \
        value_type value; \
    } key_type##value_type##KVPair; \
    \
    typedef struct key_type##value_type##HashMap { \
        key_type##value_type##KVPair *table; \
        bool *occupied; \
        u64 size; \
        u64 capacity; \
        double resize_threshold; \
    } key_type##value_type##HashMap; \
    \
    void key_type##value_type##HashMap_construct(key_type##value_type##HashMap *map) { \
        map->capacity = 0; \
        map->size = 0; \
        map->table = NULL; \
        map->occupied = NULL; \
        map->resize_threshold = 0.7; \
    } \
    \
    void key_type##value_type##HashMap_destruct(key_type##value_type##HashMap *map) { \
        if (map->table != NULL) { \
            free(map->table); \
            free(map->occupied); \
            map->table = NULL; \
            map->occupied = NULL; \
        } \
    } \
    \
    u64 key_type##value_type##HashMap_add_helper(key_type##value_type##HashMap *map, const key_type *key, const value_type *value) { \
        u64 hash = key_type##_hash(key) % map->capacity; \
        \
        u64 travel = 0; \
        /* NOTE(mdizdar): this is checking whether a hash is in use */ \
        while (map->occupied[hash] != 0) { \
            if (value_type##_eq(&map->table[hash].value, value)) { \
                /* it's already in the table */ \
                return 0; \
            } \
            hash = (hash+1) % map->capacity; \
            ++travel; \
        } \
        key_type##_copy(&map->table[hash].key, key); \
        value_type##_copy(&map->table[hash].value, value); \
        map->occupied[hash] = 1; \
        \
        ++map->size; \
        \
        return travel; \
    } \
    \
    void key_type##value_type##HashMap_resize(key_type##value_type##HashMap *map) { \
        key_type##value_type##KVPair *old_table = map->table; \
        bool *old_occupied = map->occupied; \
        u64 old_capacity = map->capacity; \
        key_type##value_type##HashMap_construct(map); \
        map->capacity = old_capacity * 2; \
        if (old_capacity == 0) { \
            map->capacity = 10; \
        } \
        \
        map->table = calloc(map->capacity, sizeof(key_type##value_type##KVPair)); \
        map->occupied = calloc(map->capacity, sizeof(bool)); \
        for (u64 i = 0; i < old_capacity; ++i) { \
            if (old_occupied[i] == 0) continue; \
            key_type##value_type##HashMap_add_helper(map, &old_table[i].key, &old_table[i].value); \
        } \
        if (old_table != NULL) { \
            free(old_table); \
            free(old_occupied); \
        } \
    } \
    \
    void key_type##value_type##HashMap_add(key_type##value_type##HashMap *map, const key_type *key, const value_type *value) { \
        if (map->capacity == 0) { \
            key_type##value_type##HashMap_resize(map); \
        } \
        const u64 travel = key_type##value_type##HashMap_add_helper(map, key, value); \
        \
        if ((travel > map->capacity/2) || (1.0 * map->size / map->capacity > map->resize_threshold)) { \
            key_type##value_type##HashMap_resize(map); \
        } \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_get_helper(const key_type##value_type##HashMap *map, const key_type *key) { \
        if (map->size == 0) { \
            return NULL; \
        } \
        u64 hash = key_type##_hash(key) % map->capacity; \
        u64 travel = 0; \
        while (map->occupied[hash] != 0) { \
            if (key_type##_eq(&map->table[hash].key, key)) { \
                return map->table + hash; \
            } \
            hash = (hash+1) % map->capacity; \
            ++travel; \
        } \
        return NULL; \
    } \
    \
    value_type *key_type##value_type##HashMap_get(const key_type##value_type##HashMap *map, const key_type *key) { \
        key_type##value_type##KVPair *entry = key_type##value_type##HashMap_get_helper(map, key); \
        if (entry == NULL) { \
            return NULL; \
        } \
        return &entry->value; \
    } \
    \
    void key_type##value_type##HashMap_set(key_type##value_type##HashMap *map, const key_type *key, const value_type *value) { \
        key_type##value_type##KVPair *entry = key_type##value_type##HashMap_get_helper(map, key); \
        if (entry == NULL) { \
            key_type##value_type##HashMap_add(map, key, value); \
            return; \
        } \
        key_type##value_type##KVPair new_entry = {*key, *value}; \
        memcpy(entry, &new_entry, sizeof *entry); \
    } \
    \
    void key_type##value_type##HashMap_erase(key_type##value_type##HashMap *map, const key_type *key) { \
        u64 hash = key_type##_hash(key) % map->capacity; \
        u64 travel = 0; \
        while (map->occupied[hash] != 0) { \
            if (key_type##_eq(&map->table[hash].key, key)) { \
                map->occupied[hash] = 0; \
                --map->size; \
                return; \
            } \
            hash = (hash+1) % map->capacity; \
            ++travel; \
        } \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_begin(const key_type##value_type##HashMap *map) { \
        if (map->size == 0) return NULL; \
        for (u64 i = 0; i < map->capacity; ++i) { \
            if (map->occupied[i]) { \
                return map->table + i; \
            } \
        } \
        assert(false); \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_end(const key_type##value_type##HashMap *map) { \
        if (map->size == 0) return NULL; \
        return map->table + map->capacity; \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_next(const key_type##value_type##HashMap *map, \
                                                                     key_type##value_type##KVPair *el) { \
        assert(el >= map->table); \
        assert(el < map->table+map->capacity); \
        for (u64 i = 1 + (u64)(el - map->table); i < map->capacity; ++i) { \
            if (map->occupied[i]) { \
                return map->table + i; \
            } \
        } \
        return key_type##value_type##HashMap_end(map); \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_rbegin(const key_type##value_type##HashMap *map) { \
        if (map->size == 0) return NULL; \
        for (u64 i = map->capacity; i > 0; --i) { \
            if (map->occupied[i-1]) { \
                return map->table + i - 1; \
            } \
        } \
        assert(false); \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_rend(const key_type##value_type##HashMap *map) { \
        if (map->size == 0) return NULL; \
        return map->table-1; \
    } \
    \
    key_type##value_type##KVPair *key_type##value_type##HashMap_previous(const key_type##value_type##HashMap *map, \
                                                                     key_type##value_type##KVPair *el) { \
        assert(el >= map->table); \
        assert(el < map->table+map->capacity); \
        for (u64 i = el - map->table; i > 0; --i) { \
            if (map->occupied[i-1]) { \
                return map->table + i - 1; \
            } \
        } \
        return key_type##value_type##HashMap_rend(map); \
    }

#define HASH_MAP_EACH(key_type, value_type, it, map) \
    key_type##value_type##KVPair *it = key_type##value_type##HashMap_begin(map); \
    it != key_type##value_type##HashMap_end(map); \
    it = key_type##value_type##HashMap_next(map, it)

#define HASH_MAP_EACH_REV(key_type, value_type, it, map) \
    key_type##value_type##KVPair *it = key_type##value_type##HashMap_rbegin(map); \
    it != key_type##value_type##HashMap_rend(map); \
    it = key_type##value_type##HashMap_previous(map, it)


#endif // HASH_MAP_H
