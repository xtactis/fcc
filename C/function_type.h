#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include "../utils/common.h"
#include "declaration.h"

struct Type;
typedef struct Type Type;
struct Node;
typedef struct Node Node;

STRUCT_HEADER(FunctionType, {
    u64 size_of;
    Type *return_type;
    DeclarationArray parameters;
    Node *block;
});


#endif //FUNCTION_TYPE_H
