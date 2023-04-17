#ifndef ARRAY_TYPE_H
#define ARRAY_TYPE_H

#include "../utils/common.h"

struct Type;
typedef struct Type Type;

STRUCT_HEADER(ArrayType, {
    Type *element;
    u64 size;
});


#endif //ARRAY_TYPE_H
