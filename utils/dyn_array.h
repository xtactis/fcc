#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include <string.h>
#include <assert.h>
#include "common.h"

typedef struct {
    void *data;
    u64 element_size;
    u64 count;
    u64 capacity;
} DynArray;

DynArray *DynArray_add(DynArray *array, void *element) {
    if (!array->capacity) {
        array->data = malloc(array->element_size * 2);
        memcpy(array->data, element, array->element_size);
        array->count = 1;
        array->capacity = 2;
        return array;
    }
    if (array->capacity == array->count) {
        array->capacity += array->capacity;
        array->data = realloc(array->data, array->capacity * array->element_size);
    }
    memcpy((u8 *)array->data + array->count*array->element_size, element, array->element_size);
    ++array->count;
    return array;
}

// NOTE(mdizdar): this doesn't work for some reason, figure it out bitch
void *DynArray_at(DynArray *array, u64 index) {
    assert(index < array->count);
    return (void *)((u8 *)array->data + index*array->element_size);
}

void *DynArray_back(DynArray *array) {
    assert(array->count > 0);
    return DynArray_at(array, array->count-1);
}

inline void DynArray_construct(DynArray *array, u64 element_size) {
    array->element_size = element_size;
    array->capacity = 0;
    array->count = 0;
}

#endif // DYN_ARRAY_H
