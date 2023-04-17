#ifndef STRING_H
#define STRING_H

#include "types.h"
#include "dyn_array.h"

STRUCT_HEADER(String, {
    char *data;
    u64 count;
});

void String_construct(String *str);
u64 String_hash(const String *name);
bool String_eq(const String *a, const String *b);
void String_copy(String *dest, const String *src, ...);
void String_print(const String *s);

#endif // STRING_H
