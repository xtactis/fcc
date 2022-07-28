/* date = May 13th 2021 3:07 pm */

#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "../utils/common.h"
#include "symbol_table.h"
#include "token.h"
#include "type.h"

#define internal_error internal_error(__FILE__, __LINE__)

static inline Type *type_of(Node *AST) {
    return AST->type;
}

// NOTE(mdizdar): in a real compiler this should depend on the target machine, but since we're targetting exactly one machine... 
u64 size_of_type(Type *type) {
    if (type->pointer_count) {
        return 2;
    } else if (type->is_struct) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            u64 cur = size_of_type(((Declaration *)type->struct_type->members.data)[i].type);
            size += cur;
            size += size%min(cur, 8);
        }
        return size;
    } else if (type->is_union) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members.count; ++i) {
            max(size, size_of_type(((Declaration *)type->struct_type->members.data)[i].type));
        }
        return size;
    } else if (type->is_typedef) {
        return size_of_type(type->typedef_type);
    } else if (type->is_array) {
        return type->array_type->size * size_of_type(type->array_type->element);
    } else if (type->is_function) {
        // should never happen
        internal_error;
    } else { // it's a basic type
        switch (type->basic_type) {
            case BASIC_CHAR:
            case BASIC_SCHAR:
            case BASIC_UCHAR: return 1;
            case BASIC_SSHORT:
            case BASIC_USHORT:
            case BASIC_SINT:
            case BASIC_UINT: return 2;
            case BASIC_SLONG:
            case BASIC_ULONG:
            case BASIC_FLOAT:
            case BASIC_DOUBLE:
            case BASIC_LDOUBLE: return 4;
            case BASIC_SLLONG:
            case BASIC_ULLONG: return 8;
            case BASIC_VOID: return 1; // while techincally correct, I don't like it
            default: error(0, "sir, this is a wendy's");
        }
    }
}

bool is_pointer(Type *type) {
    if (type->pointer_count) return true;
    if (type->is_array) return true;
    if (type->is_typedef) return is_pointer(type->typedef_type);
    return false;
}

bool is_void_pointer(Type *type) {
    bool pointer = false;
    while (type->is_typedef) {
        pointer = pointer || type->pointer_count;
        type = type->typedef_type;
    }
    return type->basic_type == BASIC_VOID && pointer;
}

void do_deref(Type *type) {
    if (type->pointer_count) {
        --type->pointer_count;
        return;
    }
    if (type->is_typedef) do_deref(type->typedef_type);
}

bool is_array(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_array) return true;
    if (type->is_typedef) return is_array(type->typedef_type);
    return false;
}

bool is_struct(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_struct) return true;
    if (type->is_typedef) return is_struct(type->typedef_type);
    return false;
}

bool is_union(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_union) return true;
    if (type->is_typedef) return is_union(type->typedef_type);
    return false;
}

Type *get_base_type(Type *type) {
    if (type->pointer_count) warning(0, "not sure if I should be letting it slide that this is a pointer tbh");
    if (type->is_typedef) return get_base_type(type->typedef_type);
    return type;
}

bool is_scalar(Type *type) {
    if (type->pointer_count) return true;
    if (type->is_struct || type->is_union || type->is_array || type->is_function) return false;
    if (type->is_typedef) return is_scalar(type->typedef_type);
    if (type->basic_type == BASIC_VOID) return false;
    return true;
}

bool is_integer(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_struct || type->is_union || type->is_array || type->is_function) return false;
    if (type->is_typedef) return is_integer(type->typedef_type);
    return !(type->basic_type == BASIC_FLOAT || type->basic_type == BASIC_DOUBLE || type->basic_type == BASIC_LDOUBLE);
}

bool is_lvalue(Node *node) { // NOTE(mdizdar): should be good?
    if (node->token->type == '[') return true;
    if (node->token->type == '.') return true;
    if (node->token->type == TOKEN_ARROW) return true;
    
    Type *type = node->type;
    while (!type->pointer_count && type->is_typedef) {
        type = type->typedef_type;
    }
    return type->pointer_count != 0;
}

bool is_signed(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_struct || type->is_union || type->is_array || type->is_function) internal_error;
    if (type->is_typedef) return is_signed(type->typedef_type);
    if (type->basic_type >= BASIC_UCHAR) return false;
    return true;
}

typedef enum Signedness {
    NA = 0,
    SIGNED = 1,
    UNSIGNED = 2,
} Signedness;

Signedness signedness(Type *type) {
    if (type->pointer_count) return UNSIGNED;
    if (type->is_array || type->is_function) return UNSIGNED;
    if (type->is_typedef) return signedness(type->typedef_type);
    if (type->basic_type >= BASIC_UCHAR) return UNSIGNED;
    if (type->basic_type >= BASIC_SCHAR && type->basic_type <= BASIC_SLLONG) return SIGNED;
    return NA;
}

bool types_are_equal_or_coercible(Type *t1, Type *t2) {
    bool p1 = is_pointer(t1);
    bool p2 = is_pointer(t2);
    Type *b1 = get_base_type(t1);
    Type *b2 = get_base_type(t2);
    if (p1 && p2) {
        if (!types_are_equal_or_coercible(b1, b2)) {
            warning(0, "performing operations on pointers with different base types is sus");
        }
        return true;
    }
    if (p1 || p2) {
        if (p2) {
            Type *tmp = b1;
            b1 = b2;
            b2 = tmp;
            p1 ^= p2 ^= p1 ^= p2;
        }
        if (b2->is_struct || b2->is_union || b2->is_function) return false;
        return b2->basic_type != BASIC_FLOAT && b2->basic_type != BASIC_DOUBLE && b2->basic_type != BASIC_LDOUBLE;
    }
    if ((b1->is_struct && b2->is_struct) || (b1->is_union && b2->is_union) || (b1->is_function && b2->is_function)) {
        return b1 == b2; // actually comparing the pointers because I don't care if the structure itself is the same, it needs to be the exact same type
    }
    return true; // basic type I guess
}

// assumes scalar types (basic or pointer)
Type *coerce(Type *t1, Type *t2) {
    bool p1 = is_pointer(t1);
    bool p2 = is_pointer(t2);
    Type *b1 = get_base_type(t1);
    Type *b2 = get_base_type(t2);
    
    if (p1 && p2) {
        if (!types_are_equal_or_coercible(b1, b2)) {
            Type *ret = malloc(sizeof(Type));
            ret->is_static   = false;
            ret->is_struct   = false;
            ret->is_union    = false;
            ret->is_typedef  = false;
            ret->is_array    = false;
            ret->is_function = false;
            ret->basic_type  = BASIC_VOID;
            ++ret->pointer_count;
            return ret;
        }
        return t1;
    }
    if (p1) {
        if (!is_integer(b2)) {
            error(0, "a pointer and a double walk into a bar...");
        }
        return t1;
    }
    if (p2) {
        if (!is_integer(b1)) {
            error(0, "a pointer and a double walk into a bar...");
        }
        return t2;
    }
    
    if (!is_integer(b1) && !is_integer(b2)) {
        if (size_of_type(b1) > size_of_type(b2)) return t1;
        else return t2;
    }
    if (!is_integer(b1)) {
        return t1;
    }
    if (!is_integer(b2)) {
        return t2;
    }
    u64 s1 = size_of_type(b1);
    u64 s2 = size_of_type(b2);
    
#define RETURN_INT \
Type *ret = malloc(sizeof(Type)); \
ret->is_static   = false;         \
ret->is_struct   = false;         \
ret->is_union    = false;         \
ret->is_typedef  = false;         \
ret->is_array    = false;         \
ret->is_function = false;         \
ret->basic_type  = BASIC_SINT;    \
return ret;
    
    if (s1 > s2) {
        if (s1 < 2) {
            RETURN_INT;
        }
        return t1;
    }
    if (s2 > s1) {
        if (s2 < 2) {
            RETURN_INT;
        }
        return t2;
    }
    if (s1 < 2) {
        RETURN_INT;
    }
#undef RETURN_INT
    // they're the same size, so if one is unsigned, I should return that one
    if (b1->basic_type > b2->basic_type) return t1;
    return t2;
}

u64 type_check(Node *AST, Type *return_type);

void type_check_params(Node *AST, Declaration **params, u64 param_count) {
    if (param_count == 1) {
        type_check(AST, NULL);
        if (!types_are_equal_or_coercible(params[0]->type, type_of(AST))) {
            error(AST->token->line, "type of parameter %lu doesn't match expected type", param_count);
        }
        return;
    }
    // NOTE(mdizdar): return_type can be NULL because it doesn't matter here
    type_check(AST->right, NULL);
    if (!types_are_equal_or_coercible(params[--param_count]->type, type_of(AST->right))) {
        error(AST->right->token->line, "type of parameter %lu doesn't match expected type", param_count);
    }
    type_check_params(AST->left, params, param_count);
}

u64 type_check(Node *AST, Type *return_type) {
    if (!AST) {
        warning(0, "I'm not sure we should ever be seeing this");
        return 0;
    }
    u64 size_of = 0;
    AST->type = NULL;
    switch ((int)AST->token->type) {
        case TOKEN_DECLARATION: {
            SymbolTableEntry *entry = AST->token->entry;
            assert(entry != NULL);
            if (entry->type->is_function) {
                entry->type->function_type->size_of = type_check(entry->type->function_type->block, entry->type->function_type->return_type);
            } else {
                size_of = size_of_type(entry->type);
            }
            break;
        }
        case TOKEN_IF: {
            type_check(AST->cond, return_type);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left, return_type);
            if (AST->right) {
                size_of += type_check(AST->right, return_type);
            }
            break;
        }
        case TOKEN_WHILE: {
            type_check(AST->cond, return_type);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left, return_type);
            break;
        }
        case TOKEN_FOR: {
            Node *init_cond_iter = AST->cond;
            type_check(init_cond_iter->left, return_type);
            type_check(init_cond_iter->cond, return_type);
            type_check(init_cond_iter->right, return_type);
            if (is_scalar(type_of(init_cond_iter->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left, return_type);
            break;
        }
        case TOKEN_RETURN: {
            type_check(AST->left, return_type);
            if (!types_are_equal_or_coercible(return_type, type_of(AST->left))) {
                error(AST->token->line, "return type and returned type not equal or coercible");
            }
            break;
        }
        case TOKEN_NEXT: {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            break;
        }
        case TOKEN_FUNCTION_CALL: {
            SymbolTableEntry *entry = AST->token->entry;
            assert(entry != NULL);
            type_check_params(AST->right, (Declaration **)&entry->type->function_type->parameters.data, entry->type->function_type->parameters.count);
            AST->type = entry->type->function_type->return_type;
            break;
        }
        case TOKEN_BREAK:
        case TOKEN_CONTINUE: {
            // there's nothing for me here, so I will disappear
            break;
        }
        case '?': {
            type_check(AST->cond, return_type);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            if (!types_are_equal_or_coercible(type_of(AST->left), type_of(AST->right))) {
                error(AST->left->token->line, "the types bro, they're not good.");
            }
            AST->type = coerce(type_of(AST->left), type_of(AST->right));
            break;
        }
        case '[': {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_integer(right)) {
                error(AST->right->token->line, "array index is a non integral type");
            }
            if (is_array(left)) {
                left = get_base_type(left);
                AST->type = left->array_type->element;
            } else if (is_pointer(left)) {
                AST->type = malloc(sizeof(Type));
                
            } else {
                error(AST->left->token->line, "only pointers and arrays are indexable");
            }
            break;
        }
        case TOKEN_ARROW: {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            if (AST->right->token->type != TOKEN_IDENT) {
                error(AST->right->token->line, "right hand side of -> operator must be a member variable of the struct pointed to by the left hand side");
            }
            while (!left->pointer_count && left->is_typedef) {
                left = left->typedef_type;
            }
            if (left->pointer_count != 1) {
                error(AST->left->token->line, "left hand side of -> operator must be a pointer to a struct or union type");
            }
            // NOTE(mdizdar): we want to get the base type of left here, but doing that is hard when there's a pointer in the way
            --left->pointer_count;
            if (is_struct(left)) {
                left = get_base_type(left);
                bool is_member = false;
                for (u64 i = 0; i < left->struct_type->members.count; ++i) {
                    if (!strcmp(AST->right->token->name.data, ((Declaration *)left->struct_type->members.data)[i].name.data)) {
                        is_member = true;
                        AST->type = ((Declaration *)left->union_type->members.data)[i].type;
                        break;
                    }
                }
                if (!is_member) {
                    error(AST->right->token->line, "right hand side of -> operator must be a member variable of the struct pointed to by the left hand side");
                }
            } else if (is_union(left)) {
                left = get_base_type(left);
                bool is_member = false;
                for (u64 i = 0; i < left->union_type->members.count; ++i) {
                    if (!strcmp(AST->right->token->name.data, ((Declaration *)left->union_type->members.data)[i].name.data)) {
                        is_member = true;
                        AST->type = ((Declaration *)left->union_type->members.data)[i].type;
                        break;
                    }
                }
                if (!is_member) {
                    error(AST->right->token->line, "right hand side of -> operator must be a member variable of the struct pointed to by the left hand side");
                }
            } else {
                error(AST->left->token->line, "left hand side of -> operator must be a pointer to a struct or union type");
            }
            ++left->pointer_count;
            break;
        }
        case '.': {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            if (AST->right->token->type != TOKEN_IDENT) {
                error(AST->right->token->line, "right hand side of . operator must be a member variable of the struct on the left hand side");
            }
            while (!left->pointer_count && left->is_typedef) {
                left = left->typedef_type;
            }
            if (left->pointer_count != 0) {
                error(AST->left->token->line, "left hand side of . operator must be a struct or union type (did you mean to use `->`?)");
            }
            if (is_struct(left)) {
                left = get_base_type(left);
                bool is_member = false;
                for (u64 i = 0; i < left->struct_type->members.count; ++i) {
                    if (!strcmp(AST->right->token->name.data, ((Declaration *)left->struct_type->members.data)[i].name.data)) {
                        is_member = true;
                        AST->type = ((Declaration *)left->union_type->members.data)[i].type;
                        break;
                    }
                }
                if (!is_member) {
                    error(AST->right->token->line, "right hand side of . operator must be a member variable of the struct on the left hand side");
                }
            } else if (is_union(left)) {
                left = get_base_type(left);
                bool is_member = false;
                for (u64 i = 0; i < left->union_type->members.count; ++i) {
                    if (!strcmp(AST->right->token->name.data, ((Declaration *)left->union_type->members.data)[i].name.data)) {
                        is_member = true;
                        AST->type = ((Declaration *)left->union_type->members.data)[i].type;
                        break;
                    }
                }
                if (!is_member) {
                    error(AST->right->token->line, "right hand side of . operator must be a member variable of the struct on the left hand side");
                }
            } else {
                error(AST->left->token->line, "left hand side of . operator must be a struct or union type");
            }
            break;
        }
        case '!': {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            
            if (!is_scalar(left)) {
                error(AST->token->line, "wrong argument to unary operator, should be scalar");
            }
            
            // NOTE(mdizdar): the type is just a bool so we set it to any basic integral type
            AST->type = malloc(sizeof(Type));
            AST->type->is_function = false;
            AST->type->is_static = false;
            AST->type->is_struct = false;
            AST->type->is_union = false;
            AST->type->is_typedef = false;
            AST->type->is_array = false;
            AST->type->is_function = false;
            AST->type->basic_type = BASIC_UINT;
            break;
        }
        case '~': {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            
            if (!is_integer(left)) {
                error(AST->token->line, "wrong argument to unary operator, should be an integral type");
            }
            
            AST->type = left;
            break;
        }
        case TOKEN_PLUS:
        case TOKEN_MINUS: {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            
            if (!is_scalar(left) || is_pointer(left)) {
                error(AST->token->line, "wrong argument to unary operator, should be scalar and not a pointer");
            }
            
            AST->type = left;
            break;
        }
        case TOKEN_PREINC:
        case TOKEN_PREDEC: {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            if (!is_integer(left) && !is_pointer(left)) {
                error(AST->token->line, "invalid expression: trying to increment a non integer or (non-void) pointer");
            }
            if (!is_lvalue(AST->left)) {
                error(AST->token->line, "can't increment/decrement a non lvalue");
            }
            if (is_void_pointer(left)) {
                error(AST->left->token->line, "can't do pointer arithmetic on void pointers");
            }
            AST->type = left;
            break;
        }
        case TOKEN_POSTINC:
        case TOKEN_POSTDEC: {
            type_check(AST->right, return_type);
            Type *right = type_of(AST->right);
            if (!is_integer(right) && !is_pointer(right)) {
                error(AST->token->line, "trying to increment a non integer or (non-void) pointer is invalid");
            }
            if (!is_lvalue(AST->right)) {
                error(AST->token->line, "can't increment/decrement a non lvalue");
            }
            if (is_void_pointer(right)) {
                error(AST->right->token->line, "can't do pointer arithmetic on void pointers");
            }
            AST->type = right;
            break;
        }
        case TOKEN_DEREF: {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            
            if (!is_pointer(left)) {
                error(AST->token->line, "cannot dereference a non-pointer");
            }
            if (is_void_pointer(left)) {
                error(AST->token->line, "you do not want to know what happens when you dereference a void pointer so I stopped you");
            }
            
            AST->type = malloc(sizeof(Type));
            memcpy(AST->type, left, sizeof(Type));
            do_deref(AST->type);
            
            break;
        }
        case TOKEN_ADDRESS: {
            type_check(AST->left, return_type);
            Type *left = type_of(AST->left);
            
            if (!is_lvalue(AST->left)) {
                error(AST->token->line, "can't take the address of a non lvalue");
            }
            AST->type = malloc(sizeof(Type));
            memcpy(AST->type, left, sizeof(Type));
            ++AST->type->pointer_count;
            
            break;
        }
        case '+': { // NOTE(mdizdar): one can be a pointer
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            if (is_pointer(left)) {
                if (is_void_pointer(left)) {
                    error(AST->token->line, "can't perform pointer arithmetic on void pointer");
                }
                if (!is_pointer(right)) {
                    if (!is_integer(right)) {
                        error(AST->right->token->line, "can't perform summation on a pointer and a non integral type");
                    }
                    AST->type = left;
                } else {
                    error(AST->token->line, "pointer summation isn't a thing in this language (or any other for that matter)");
                }
            } else if (is_pointer(right)) {
                error(AST->token->line, "adding a pointer to a scalar doesn't make sense");
            } else {
                AST->type = left;
            }
            break;
        }
        case '-': {
            // NOTE(mdizdar): can be pointer - scalar (returns pointer), but not scalar - pointer
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            if (is_pointer(left)) {
                if (is_void_pointer(left)) {
                    error(AST->token->line, "can't perform pointer arithmetic on void pointer");
                }
                if (!is_pointer(right)) {
                    if (!is_integer(right)) {
                        error(AST->right->token->line, "can't perform summation on a pointer and a non integral type");
                    }
                    AST->type = left;
                } else {
                    if (is_void_pointer(right)) {
                        error(AST->token->line, "can't perform pointer arithmetic on void pointer");
                    }
                    AST->type = malloc(sizeof(Type));
                    AST->type->is_function = false;
                    AST->type->is_static = false;
                    AST->type->is_struct = false;
                    AST->type->is_union = false;
                    AST->type->is_typedef = false;
                    AST->type->is_array = false;
                    AST->type->is_function = false;
                    AST->type->basic_type = BASIC_UINT; // NOTE(mdizdar): any word-sized integer type
                }
            } else if (is_pointer(right)) {
                error(AST->token->line, "subtracting a pointer from a scalar doesn't make sense");
            } else {
                AST->type = left;
            }
            break;
        }
        case TOKEN_LOGICAL_OR:
        case TOKEN_LOGICAL_AND: { // NOTE(mdizdar): can be pointers, return type scalar (int)
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            AST->type = malloc(sizeof(Type));
            AST->type->is_function = false;
            AST->type->is_static = false;
            AST->type->is_struct = false;
            AST->type->is_union = false;
            AST->type->is_typedef = false;
            AST->type->is_array = false;
            AST->type->is_function = false;
            AST->type->basic_type = BASIC_UINT;
            break;
        }
        case '*':
        case '/':
        case '%':
        case '&':
        case '|':
        case '^':
        case TOKEN_BITSHIFT_LEFT:
        case TOKEN_BITSHIFT_RIGHT: { // NOTE(mdizdar): can't be pointers, return type scalar
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            if (is_pointer(left)) {
                error(AST->left->token->line, "you doing weird shit to pointers");
            }
            if (is_pointer(right)) {
                error(AST->right->token->line, "you doing weird shit to pointers");
            }
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            AST->type = coerce(left, right);
            break;
        }
        case TOKEN_EQUALS:
        case TOKEN_NOT_EQ:
        case '>':
        case '<':
        case TOKEN_LESS_EQ:
        case TOKEN_GREATER_EQ: {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't compare non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't compare non-scalar types");
            }
            if (signedness(left) != signedness(right)) {
                warning(AST->token->line, "comparison between two values of different signedness");
            }
            
            coerce(left, right);
            
            AST->type = malloc(sizeof(Type));
            AST->type->is_function = false;
            AST->type->is_static = false;
            AST->type->is_struct = false;
            AST->type->is_union = false;
            AST->type->is_typedef = false;
            AST->type->is_array = false;
            AST->type->is_function = false;
            AST->type->basic_type = BASIC_UINT;
        }
        case TOKEN_ADD_ASSIGN:
        case TOKEN_SUB_ASSIGN: {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            if (is_pointer(left)) {
                if (is_void_pointer(left)) {
                    error(AST->token->line, "can't perform pointer arithmetic on void pointer");
                }
                if (!is_pointer(right)) {
                    if (!is_integer(right)) {
                        error(AST->right->token->line, "can't perform summation on a pointer and a non integral type");
                    }
                } else {
                    error(AST->token->line, "pointer summation isn't a thing in this language (or any other for that matter)");
                }
            } else if (is_pointer(right)) {
                error(AST->token->line, "adding a pointer to a scalar doesn't make sense");
            }
            coerce(left, right);
            AST->type = left;
            break;
        }
        case TOKEN_MUL_ASSIGN:
        case TOKEN_DIV_ASSIGN:
        case TOKEN_MOD_ASSIGN:
        case TOKEN_OR_ASSIGN:
        case TOKEN_AND_ASSIGN:
        case TOKEN_XOR_ASSIGN:
        case TOKEN_BITNOT_ASSIGN:
        case TOKEN_BIT_L_ASSIGN:
        case TOKEN_BIT_R_ASSIGN: {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            if (is_pointer(left)) {
                error(AST->left->token->line, "you doing weird shit to pointers");
            }
            if (is_pointer(right)) {
                error(AST->right->token->line, "you doing weird shit to pointers");
            }
            if (!is_lvalue(AST->left)) {
                error(AST->left->token->line, "left hand side of assignment needs to be an lvalue");
            }
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            coerce(left, right);
            AST->type = left;
            break;
        }
        case '=': {
            type_check(AST->left, return_type);
            type_check(AST->right, return_type);
            if (!is_lvalue(AST->left)) {
                error(AST->left->token->line, "left hand side of assignment needs to be an lvalue");
            }
            if (!types_are_equal_or_coercible(type_of(AST->left), type_of(AST->right))) {
                error(AST->token->line, "you assigning some weird shit..");
            }
            AST->type = AST->left->type;
            break;
        }
        default: internal_error;
    }
    
    return size_of;
}

#endif //TYPE_CHECK_H
