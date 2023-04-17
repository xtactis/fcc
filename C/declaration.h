#ifndef DECLARATION_H
#define DECLARATION_H

#include "../utils/common.h"

struct Type;
typedef struct Type Type;

STRUCT_HEADER(Declaration, {
    Type *type; // NOTE(mdizdar): at one point we'll have common types multiple variables can point to, so we use a pointer
    String name;
});

#endif //DECLARATION_H
