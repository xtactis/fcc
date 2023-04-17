#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define _generate_dynamic_array_header(name) \
    typedef struct name##Array { \
        name *data; \
        u64 count; \
        u64 capacity; \
    } name##Array, *name##ArrayPtr; \
    \
    name##Array *name##Array_push_ptr(name##Array *array, const name *element); \
    name *name##Array_pop_back(name##Array *array); \
    name##Array *name##Array_push_back(name##Array *array, name element); \
    name *name##Array_at(const name##Array *array, u64 index); \
    name *name##Array_front(const name##Array *array); \
    name *name##Array_back(const name##Array *array); \
    name *name##Array_begin(const name##Array *array); \
    name *name##Array_end(const name##Array *array); \
    name *name##Array_next(const name##Array *array, name *el); \
    name *name##Array_rbegin(const name##Array *array); \
    name *name##Array_rend(const name##Array *array); \
    name *name##Array_previous(const name##Array *array, name *el); \
    void name##Array_erase(name##Array *array, u64 index); \
    name##Array *name##Array_insert_ptr(name##Array *array, const name *new_element, u64 position); \
    void name##Array_reserve(name##Array *array, u64 new_cap); \
    void name##Array_clear(name##Array *array); \
    void name##Array_shrink_to_fit(name##Array *array); \
    void name##Array_construct(name##Array *array); \
    void name##Array_destruct(name##Array *array); \
    name##Array *name##Array_extend(name##Array *dest, const name##Array *source); \
    name##Array *name##Array_copy(name##Array *dest, const name##Array *source);

// TODO(mdizdar): add a sorting function

#define _generate_dynamic_array_source(name) \
    name##Array *name##Array_push_ptr(name##Array *array, const name *element) { \
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
    name##Array *name##Array_insert_ptr(name##Array *array, const name *new_element, u64 position) { \
        assert(array); \
        assert(array->count >= position); \
        if (array->count == position) { \
            return name##Array_push_ptr(array, new_element); \
        } \
        \
        if (array->capacity == array->count) { \
            array->capacity += array->capacity; \
            array->data = realloc(array->data, array->capacity * sizeof(name)); \
        } \
        memcpy(array->data + position + 1, array->data + position, (array->count - position) * sizeof(name)); \
        /* I'm still unsure if memcpy is the thing we want to be doing here */ \
        memcpy(array->data + position, new_element, sizeof(name)); \
        ++array->count; \
        return array; \
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
    name##Array *name##Array_extend(name##Array *dest, const name##Array *source) { \
        name##Array_reserve(dest, dest->count+source->count); \
        for (ARRAY_EACH(name, it, source)) { \
            name##Array_push_ptr(dest, it); \
        } \
        return dest; \
    } \
    \
    name##Array *name##Array_copy(name##Array *dest, const name##Array *source) { \
        name##Array_clear(dest); \
        return name##Array_extend(dest, source); \
    }

#define _generate_dynamic_array(type) \
    _generate_dynamic_array_header(type); \
    _generate_dynamic_array_source(type);

#define ARRAY_EACH(type, it, array) \
    type *it = type##Array_begin(array); \
    it != type##Array_end(array); \
    it = type##Array_next(array, it)

#define ARRAY_EACH_REV(type, it, array) \
    type *it = type##Array_rbegin(array); \
    it != type##Array_rend(array); \
    it = type##Array_previous(array, it)

#endif // DYN_ARRAY_H
