#include "string.h"

STRUCT_SOURCE(String);

void String_construct(String *str) {
    str->data = NULL;
    str->count = 0;
}

u64 String_hash(const String *name) {
    u64 result = 5381;
    for (u64 i = 0; i < name->count; ++i) {
        result = ((result << 5) + result) + name->data[i]; /* hash * 33 + c */
    }
    return result;
}

bool String_eq(const String *a, const String *b) {
    return !strcmp(a->data, b->data);
}

void String_copy(String *dest, const String *src, ...) {
    // TODO(mdizdar): not freeing here might cause a memory leak, I'll deal with it later
    dest->data = malloc(src->count);
    dest->count = src->count;
    memcpy(dest->data, src->data, src->count+1);
}

void String_print(const String *s) {
    printf("`%s` (%lu)", s->data, s->count);
}

