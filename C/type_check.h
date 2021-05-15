/* date = May 13th 2021 3:07 pm */

#ifndef TYPE_CHECK_H
#define TYPE_CHECK_H

#include "../utils/common.h"
#include "symbol_table.h"
#include "token.h"
#include "type.h"

static inline Type *type_of(Node *AST) {
    return AST->type;
}

u64 max(u64 a, u64 b) {
    return a > b ? a : b;
}

// NOTE(mdizdar): in a real compiler this should depend on the target machine, but since we're targetting exactly one machine... 
u64 *size_of_type(Type *type) {
    if (type->pointer_count) {
        return 2;
    } else if (type->is_struct) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members->count; ++i) {
            u64 cur = size_of_type(((Type **)type->struct_type->members->data)[i]);
            size += cur;
            size += size%min(cur, 8);
        }
        return size;
    } else if (type->is_union) {
        u64 size = 0;
        for (u64 i = 0; i < type->struct_type->members->count; ++i) {
            max(size, size_of_type(((Type **)type->struct_type->members->data)[i]));
        }
        return size;
    } else if (type->is_typedef) {
        return size_of_type(type->typedef_type);
    } else if (type->is_array) {
        return type->array_type->size * size_of_type(type->array_type->element);
    } else if (type->is_function) {
        // should never happen
        internal_error(__FILE__, __LINE__);
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
    if (type->is_typedef) return is_pointer(type->typedef_type);
    return false;
}

bool is_scalar(Type *type) {
    if (type->pointer_count) return true;
    if (type->is_struct || type->is_union || type->is_array || type->function) return false;
    if (type->is_typedef) return is_scalar(type->typedef_type);
    if (type->basic_type == BASIC_VOID) return false;
    return true;
}

bool is_integer(Type *type) {
    if (type->pointer_count) return false;
    if (type->is_struct || type->is_union || type->is_array || type->function) return false;
    if (type->is_typedef) return is_integer(type->typedef_type);
    return !(type->basic_type == BASIC_FLOAT || type->basic_type == BASIC_DOUBLE || type->basic_type == BASIC_LDOUBLE);
}

bool is_lvalue(Node *node) {
    // TODO(mdizdar): implement me!
    return true;
}

bool types_are_equal(Type *t1, Type *t2) {
    // TODO(mdizdar): implement me!
    return true;
}

bool signedness(Type *t1) {
    // TODO(mdizdar): implement me!
    return true;
}

// NOTE(mdizdar): coerces type t into type target and returns type target
Type *coerce_to(Type *t, Type *target) {
    // TODO(mdizdar): implement me!
}

Type *coerce(Type *t1, Type *t2) {
    // TODO(mdizdar): implement me!
}

u64 type_check(Node *AST, Type *return_type);

void type_check_params(Node *AST, Type **params, u64 param_count) {
    if (param_count == 1) {
        type_check(AST, NULL);
        if (!types_are_equal(params[0], type_of(AST))) {
            error(AST->token->line, "type of parameter %llu doesn't match expected type", param_count);
        }
        return;
    }
    // NOTE(mdizdar): return_type can be NULL because it doesn't matter here
    type_check(AST->right, NULL);
    if (!types_are_equal(params[--param_count], type_of(AST->right))) {
        error(AST->right->token->line, "type of parameter %llu doesn't match expected type", param_count);
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
    switch (AST->token->type) {
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
            type_check(AST->cond);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left);
            if (AST->right) {
                size_of += type_check(AST->right);
            }
            break;
        }
        case TOKEN_WHILE: {
            type_check(AST->cond);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left);
            break;
        }
        case TOKEN_FOR: {
            Node *init_cond_iter = AST->cond;
            type_check(init_cond_iter->left);
            type_check(init_cond_iter->cond);
            type_check(init_cond_iter->right);
            if (is_scalar(type_of(init_cond_iter->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            size_of += type_check(AST->left);
            break;
        }
        case TOKEN_RETURN: {
            type_check(AST->left);
            if (!types_are_equal(return_type, type_of(AST->left))) {
                error(AST->token->line, "return type and returned type not equal or coercible");
            }
            break;
        }
        case TOKEN_NEXT: {
            type_check(AST->left);
            type_check(AST->right);
            break;
        }
        case TOKEN_FUNCTION_CALL: {
            SymbolTableEntry *entry = AST->token->entry;
            assert(entry != NULL);
            type_check_params(AST->right, &entry->type->function_type->parameters.data, entry->type->function_type->parameters.count);
            AST->type = entry->type->function_type->return_type;
            break;
        }
        case TOKEN_BREAK:
        case TOKEN_CONTINUE: {
            // there's nothing for me here, so I will disappear
            break;
        }
        case '?': {
            type_check(AST->cond);
            if (is_scalar(type_of(AST->cond))) {
                error(AST->cond->token->line, "conditions should be of a scalar type");
            }
            type_check(AST->left);
            type_check(AST->right);
            if (!types_are_equal(type_of(AST->left), type_of(AST->right))) {
                error(AST->left->token->line, "the types bro, they're not good.");
            }
            AST->type = coerce(type_of(AST->left), type_of(AST->right));
            break;
        }
        case '[': {
            type_check(AST->right);
            // TODO(mdizdar): 
            break;
        }
        case '->': {
            type_check(AST->left);
            type_check(AST->right); // NOTE(mdizdar): what does it mean to type check the right side here? I don't think this should be done
            // TODO(mdizdar): 
            break;
        }
        case '.': {
            type_check(AST->left);
            type_check(AST->right);
            // TODO(mdizdar): 
            break;
        }
        case '!':
        case '~':
        case TOKEN_PLUS:
        case TOKEN_MINUS: {
            type_check(AST->left);
            // TODO(mdizdar): 
            break;
        }
        case TOKEN_PREINC:
        case TOKEN_PREDEC: {
            type_check(AST->left);
            // TODO(mdizdar): 
        }
        case TOKEN_POSTINC:
        case TOKEN_POSTDEC: {
            type_check(AST->right);
            // TODO(mdizdar): 
            break;
        }
        case TOKEN_DEREF:
        case TOKEN_ADDRESS: {
            type_check(AST->left);
            // TODO(mdizdar): 
            break;
        }
        case '+': { // NOTE(mdizdar): one can be a pointer
            type_check(AST->left);
            type_check(AST->right);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            if (is_pointer(left)) {
                if (!is_pointer(right)) {
                    if (!is_integer(right)) {
                        error(AST->right->token->line, "");
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
            type_check(AST->left);
            type_check(AST->right);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            
            if (is_pointer(left)) {
                if (!is_pointer(right)) {
                    if (!is_integer(right)) {
                        error(AST->right->token->line, "");
                    }
                    AST->type = left;
                } else {
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
            type_check(AST->left);
            type_check(AST->right);
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
            AST->type->basic_type = BASIC_INT;
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
            type_check(AST->left);
            type_check(AST->right);
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
            type_check(AST->left);
            type_check(AST->right);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't compare non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't compare non-scalar types");
            }
            if (signedness(left) != signednes(right)) {
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
            AST->type->basic_type = BASIC_INT;
        }
        case TOKEN_ADD_ASSIGN:
        case TOKEN_SUB_ASSIGN: {
            
        }
        case TOKEN_MUL_ASSIGN:
        case TOKEN_DIV_ASSIGN:
        case TOKEN_MOD_ASSIGN:
        case TOKEN_OR_ASSIGN:
        case TOKEN_AND_ASSIGN:
        case TOKEN_XOR_ASSIGN:
        case TOKEN_BITNOT_ASSIGN: // TODO(mdizdar): some of these can't be done with pointers
        case TOKEN_BIT_L_ASSIGN:
        case TOKEN_BIT_R_ASSIGN: {
            type_check(AST->left);
            type_check(AST->right);
            Type *left = type_of(AST->left);
            Type *right = type_of(AST->right);
            if (!is_lvalue(AST->left)) {
                error(AST->left->token->line, "left hand side of assignment needs to be an lvalue");
            }
            if (!is_scalar(left)) {
                error(AST->left->token->line, "can't perform arithmetic on non-scalar types");
            }
            if (!is_scalar(right)) {
                error(AST->right->token->line, "can't perform arithmetic on non-scalar types");
            }
            coerce_to(right, left);
            AST->type = left;
            break;
        }
        case '=': {
            type_check(AST->left);
            type_check(AST->right);
            if (!is_lvalue(AST->left)) {
                error(AST->left->token->line, "left hand side of assignment needs to be an lvalue");
            }
            if (!types_are_equal(type_of(AST->left), type_of(AST->right))) {
                error(AST->token->line, "you assigning some weird shit..");
            }
            AST->type = AST->left->type;
            break;
        }
    }
    
    return size_of;
}

#endif //TYPE_CHECK_H
