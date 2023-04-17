#ifndef NODE_H
#define NODE_H

#include "../utils/common.h"
#include "token.h"

STRUCT_HEADER(Node, {
    Token *token;
    struct Node *left;
    struct Node *right;
    struct Node *cond; // this is only used for ternary
    
    const Scope *scope; // NOTE(mdizdar): usually NULL, except on nodes that change the scope
    Type *type; // filled in by type checker
});


#endif //NODE_H
