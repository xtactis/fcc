/* date = May 12th 2021 6:42 am */
#ifndef IR_GEN_H
#define IR_GEN_H

#include <stdlib.h>

#include "../utils/common.h"
#include "symbol_table.h"
#include "token.h"
#include "type.h"
#include "arena.h"

#include "../IR/IR.h"

typedef struct {
    u64 loop_top, loop_end, loop_continue;
    
    bool in_loop;
} IRContext;

static inline bool Token_is_value(Token *token) {
    return token->type == TOKEN_IDENT || (token->type >= TOKEN_LITERAL && token->type < TOKEN_KEYWORD);
}

static inline Op Token_comp_assign_to_Op(TokenType type) {
    switch ((int)type) {
        case TOKEN_ADD_ASSIGN: case TOKEN_PREINC: case TOKEN_POSTINC: return (Op)'+';
        case TOKEN_SUB_ASSIGN: case TOKEN_PREDEC: case TOKEN_POSTDEC: return (Op)'-';
        case TOKEN_MUL_ASSIGN:    return (Op)'*';
        case TOKEN_DIV_ASSIGN:    return (Op)'/';
        case TOKEN_MOD_ASSIGN:    return (Op)'%';
        case TOKEN_OR_ASSIGN:     return (Op)'|';
        case TOKEN_AND_ASSIGN:    return (Op)'&';
        case TOKEN_XOR_ASSIGN:    return (Op)'^';
        case TOKEN_BITNOT_ASSIGN: return (Op)'~';
        case TOKEN_BIT_L_ASSIGN:  return OP_BITSHIFT_LEFT;
        case TOKEN_BIT_R_ASSIGN:  return OP_BITSHIFT_RIGHT;
        default: error(0, "look at dis dood (%llu)", type);
    }
}

static inline Op Token_unary_to_Op(TokenType type) {
    switch ((int)type) {
        case TOKEN_DEREF:   return OP_DEREF;
        case TOKEN_ADDRESS: return OP_ADDRESS;
        case '~':           return (Op)'~';
        case '!':           return (Op)'!';
        case TOKEN_PLUS:    return OP_PLUS;
        case TOKEN_MINUS:   return OP_MINUS;
        default: error(0, "look at dis dood (%llu)", type);
    }
}

inline u64 add_specific_label(DynArray *generated_IR, u64 index) {
    IR *ir = malloc(sizeof(IR));
    ir->instruction = OP_LABEL;
    ir->operands[0].type = OT_LABEL;
    ir->operands[0].named = false;
    ir->operands[0].label_index = index;
    ir->block = NULL;
    DynArray_add(generated_IR, ir);
    return index;
}

inline u64 add_label(DynArray *generated_IR) {
    IR *ir = malloc(sizeof(IR));
    ir->instruction = OP_LABEL;
    ir->operands[0].type = OT_LABEL;
    ir->operands[0].named = false;
    ir->operands[0].label_index = label_index++;
    ir->block = NULL;
    DynArray_add(generated_IR, ir);
    return label_index-1;
}

inline void add_named_label(DynArray *generated_IR, String *label_name) {
    IR *ir = malloc(sizeof(IR));
    ir->instruction = OP_LABEL;
    ir->operands[0].type = OT_LABEL;
    ir->operands[0].named = true;
    ir->operands[0].label_name = *label_name;
    ir->block = NULL;
    DynArray_add(generated_IR, ir);
}

IR *move_to_temp(IRVariable x) {
    IR *ir = malloc(sizeof(IR));
    ir->result.type = OT_TEMPORARY;
    ir->result.entry = 0;
    ir->result.temporary_id = ++temporary_index;
    ir->operands[0] = x;
    ir->instruction = (Op)'=';
    ir->block = NULL;
    return ir;
}

OperandType operand_from_type(Type *type) { // I'm just gonna pretend structs don't exist
    if (type->pointer_count) {
        return OT_POINTER;
    }
    switch (type->basic_type) {
        case BASIC_CHAR:
        case BASIC_SCHAR:
        case BASIC_UCHAR: return OT_INT8;
        case BASIC_SSHORT:
        case BASIC_USHORT:
        case BASIC_SINT:
        case BASIC_UINT: return OT_INT16;
        case BASIC_SLONG:
        case BASIC_ULONG: return OT_INT32;
        case BASIC_SLLONG:
        case BASIC_ULLONG: return OT_INT64;
        case BASIC_FLOAT: return OT_FLOAT;
        case BASIC_DOUBLE:
        case BASIC_LDOUBLE: return OT_DOUBLE;
        default: error(0, "stop.");
    }
}

// TODO(mdizdar): not sure how to handle struct type scopes
// NOTE(mdizdar): returns the id of the result variable 
IRVariable IR_generate(Node *AST, DynArray *generated_IR, const Scope *current_scope, IRContext *context) {
    if (AST->scope != NULL) {
        // NOTE(mdizdar): I have a suspicion this isn't what I want to be doing here... does the scope get reverted once we're out? (the answer is no)
        current_scope = AST->scope;
    }
    
    if (AST->token->type == TOKEN_DECLARATION) {
        SymbolTableEntry *entry = AST->token->entry;
        assert(entry != NULL);
        if (entry->type->is_function) {
            current_scope = AST->token->entry->type->function_type->block->scope;
            assert(current_scope != NULL);
            
            add_named_label(generated_IR, &entry->name);
            IR *ir = malloc(sizeof(IR));
            ir->instruction = OP_PRELUDE;
            ir->operands[0].type = OT_INT16;
            ir->operands[0].integer_value = entry->type->function_type->size_of;
            ir->result.type = OT_NONE;
            ir->block = NULL;
            DynArray_add(generated_IR, ir);
            for (s64 i = entry->type->function_type->parameters.count-1;
                 i >= 0;
                 --i) {
                IR *param = malloc(sizeof(IR));
                param->block = NULL;
                param->instruction = OP_GET_ARG;
                param->result.type = OT_TEMPORARY;
                param->result.temporary_id = temporary_index++;
                param->operands[0].type = OT_INT64;
                param->operands[0].integer_value = (u64)(entry->type->function_type->parameters.count - i);
                SymbolTableEntry *dentry = Scope_find(current_scope, &((Declaration*)(entry->type->function_type->parameters.data))[i].name);
                dentry->temporary_id = param->result.temporary_id;
                DynArray_add(generated_IR, param);
            }
            return IR_generate(AST->token->entry->type->function_type->block, generated_IR, current_scope, context);
        } else {
            IRVariable var;
            var.type = OT_TEMPORARY;
            var.entry = (uintptr_t)AST->token->entry;
            var.temporary_id = temporary_index++;
            ((SymbolTableEntry *)var.entry)->temporary_id = var.temporary_id;
            return var;
        }
    }
    
    if (AST->token->type == TOKEN_INT_LITERAL) {
        IRVariable var;
        var.type = OT_INT16;
        var.integer_value = AST->token->integer_value;
        return var;
    } else if (AST->token->type == TOKEN_FLOAT_LITERAL) {
        IRVariable var;
        var.type = OT_FLOAT;
        var.float_value = AST->token->float_value;
        return var;
    } else if (AST->token->type == TOKEN_DOUBLE_LITERAL) {
        IRVariable var;
        var.type = OT_DOUBLE;
        var.double_value = AST->token->double_value;
        return var;
    } else if (AST->token->type == TOKEN_IDENT) {
        IRVariable var;
        var.type = OT_TEMPORARY;
        var.entry = (uintptr_t)AST->token->entry;
        var.temporary_id = ((SymbolTableEntry *)var.entry)->temporary_id;
        return var;
    }
    
    IR *ir = malloc(sizeof(IR));
    ir->block = NULL;
    switch ((int)AST->token->type) {
        case '+': case '-':
        case '*': case '/': case '%':
        case '^': case '&': case '|':
        case '<': case '>': 
        case TOKEN_NOT_EQ: case TOKEN_EQUALS: 
        case TOKEN_LESS_EQ: case TOKEN_GREATER_EQ:
        case TOKEN_LOGICAL_OR: case TOKEN_LOGICAL_AND:
        case TOKEN_BITSHIFT_LEFT: case TOKEN_BITSHIFT_RIGHT: {
            // TODO(mdizdar): add checks for calculations on literals that can be done at compile time
            ir->instruction = (Op)AST->token->type;
            ir->result.type = OT_TEMPORARY;
            ir->result.entry = 0;
            ir->result.temporary_id = temporary_index++;
            ir->operands[0] = IR_generate(AST->left, generated_IR, current_scope, context);
            ir->operands[1] = IR_generate(AST->right, generated_IR, current_scope, context);
            DynArray_add(generated_IR, ir);
            break;
        }
        case '=': {
            ir->instruction = (Op)AST->token->type;
            ir->result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir->operands[0] = IR_generate(AST->right, generated_IR, current_scope, context);
            if (ir->result.type == OT_TEMPORARY && ir->result.entry != 0) {
                ir->result.temporary_id = temporary_index++;
                ((SymbolTableEntry *)ir->result.entry)->temporary_id = ir->result.temporary_id;
            }
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_ADD_ASSIGN: case TOKEN_SUB_ASSIGN:
        case TOKEN_MUL_ASSIGN: case TOKEN_DIV_ASSIGN: case TOKEN_MOD_ASSIGN:
        case TOKEN_OR_ASSIGN: case TOKEN_AND_ASSIGN: case TOKEN_XOR_ASSIGN:
        case TOKEN_BIT_L_ASSIGN: case TOKEN_BIT_R_ASSIGN: {
            ir->instruction = Token_comp_assign_to_Op(AST->token->type);
            ir->result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir->operands[0] = ir->result;
            ir->operands[1] = IR_generate(AST->right, generated_IR, current_scope, context);
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_BITNOT_ASSIGN: {
            ir->instruction = Token_comp_assign_to_Op(AST->token->type);
            ir->result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir->operands[0] = ir->result;
            if (ir->result.type == OT_TEMPORARY && ir->result.entry != 0) {
                ir->result.temporary_id = temporary_index++;
                ((SymbolTableEntry *)ir->result.entry)->temporary_id = ir->result.temporary_id;
            }
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_PREINC: case TOKEN_PREDEC: {
            ir->instruction = Token_comp_assign_to_Op(AST->token->type);
            ir->result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir->operands[0] = ir->result;
            ir->operands[1].type = OT_INT8;
            ir->operands[1].integer_value = 1ULL;
            if (ir->result.type == OT_TEMPORARY && ir->result.entry != 0) {
                ir->result.temporary_id = temporary_index++;
                ((SymbolTableEntry *)ir->result.entry)->temporary_id = ir->result.temporary_id;
            }
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_POSTINC: case TOKEN_POSTDEC: {
            IR *ir2 = malloc(sizeof(IR));
            ir2->block = NULL;
            ir2->instruction = (Op)'=';
            ir2->result.type = OT_TEMPORARY;
            ir2->result.entry = 0;
            ir2->result.temporary_id = temporary_index++;
            ir2->operands[0] = IR_generate(AST->left, generated_IR, current_scope, context);
            
            DynArray_add(generated_IR, ir2);
            ir->instruction = Token_comp_assign_to_Op(AST->token->type);
            ir->result      = ir2->operands[0];
            ir->operands[0] = ir2->operands[0];
            ir->operands[1].type = OT_INT8;
            ir->operands[1].integer_value = 1ULL;
            if (ir->result.type == OT_TEMPORARY && ir->result.entry != 0) {
                ir->result.temporary_id = temporary_index++;
                ((SymbolTableEntry *)ir->result.entry)->temporary_id = ir->result.temporary_id;
            }
            DynArray_add(generated_IR, ir);
            ir = ir2; // this is done so we can return the original value as are the semantics of post inc/dec
            break;
        }
        case TOKEN_DEREF: {
            // TODO(mdizdar): this is stupid and won't work if you deref more than once, should handle that as well
            IRVariable derefd = IR_generate(AST->left, generated_IR, current_scope, context);
            derefd.type = OT_DEREF_TEMPORARY;
            return derefd;
        }
        case TOKEN_ADDRESS: 
        case '~': case '!': 
        case TOKEN_PLUS: case TOKEN_MINUS: {
            ir->instruction = Token_unary_to_Op(AST->token->type);
            ir->result.type = OT_TEMPORARY;
            ir->result.entry = 0;
            ir->result.temporary_id = temporary_index++;
            ir->operands[0] = IR_generate(AST->left, generated_IR, current_scope, context);
            DynArray_add(generated_IR, ir);
            break;
        }
        case '?': {
            // condition
            ir->instruction = OP_IF_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir->operands[1].type = OT_LABEL;
            ir->operands[1].named = false;
            
            DynArray_add(generated_IR, ir);
            
            // if false
            IRVariable Fres = IR_generate(AST->right, generated_IR, current_scope, context);
            IR *F = ((IR **)generated_IR->data)[generated_IR->count-1];
            if (Fres.type != OT_TEMPORARY) {
                F = move_to_temp(Fres);
                DynArray_add(generated_IR, F);
            }
            
            IR *ir2 = malloc(sizeof(IR));
            ir2->block = NULL;
            ir2->instruction = OP_JUMP;
            ir2->result.type = OT_NONE;
            ir2->operands[0].type = OT_LABEL;
            ir2->operands[0].named = false;
            
            DynArray_add(generated_IR, ir2);
            
            // NOTE(mdizdar): assuming the dynarray stores pointers so modifying through this variable will modify the one stored in the dynarray
            ir->operands[1].label_index = add_label(generated_IR); // after F
            
            // if true
            IRVariable Tres = IR_generate(AST->left, generated_IR, current_scope, context);
            IR *T = ((IR **)generated_IR->data)[generated_IR->count-1];
            if (Tres.type != OT_TEMPORARY) {
                T = move_to_temp(Tres);
                DynArray_add(generated_IR, T);
            }
            
            F->result.temporary_id = T->result.temporary_id;
            
            ir2->operands[0].label_index = add_label(generated_IR); // after T
            
            ir = T;
            break;
        }
        case TOKEN_IF: {
            // condition
            ir->instruction = OP_IFN_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir->operands[1].type = OT_LABEL;
            ir->operands[1].named = false;
            
            DynArray_add(generated_IR, ir);
            u64 ifn_jump_pos = generated_IR->count-1;
            
            IR_generate(AST->left, generated_IR, current_scope, context);
            
            u64 jump_out_pos = -1;
            if (AST->right) {
                ir = malloc(sizeof(IR));
                ir->instruction = OP_JUMP;
                ir->operands[0].type = OT_LABEL;
                ir->operands[0].named = false;
                DynArray_add(generated_IR, ir);
                jump_out_pos = generated_IR->count-1;
            }

            ((IR *)generated_IR->data)[ifn_jump_pos].operands[1].label_index = add_label(generated_IR);
            
            if (AST->right) { // else
                IR_generate(AST->right, generated_IR, current_scope, context);

                ((IR *)DynArray_at(generated_IR, jump_out_pos))->operands[0].label_index = add_label(generated_IR);
            }
            break;
        }
        case TOKEN_WHILE: {
            u64 loop_top = add_label(generated_IR);
            
            // condition
            ir->instruction = OP_IFN_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir->operands[1].type = OT_LABEL;
            ir->operands[1].named = false;
            
            DynArray_add(generated_IR, ir);
            u64 ifn_jump_pos = generated_IR->count-1;
            
            context->in_loop = true;
            context->loop_top = loop_top;
            context->loop_continue = loop_top;
            u64 loop_end = label_index++;
            context->loop_end = loop_end;
            
            IR_generate(AST->left, generated_IR, current_scope, context);
            context->in_loop = false; // no longer in the loop lol
            
            IR *ir2 = malloc(sizeof(IR));
            ir2->block = NULL;
            ir2->instruction = OP_JUMP;
            ir2->result.type = OT_NONE;
            ir2->operands[0].type = OT_LABEL;
            ir2->operands[0].named = false;
            ir2->operands[0].label_index = loop_top;
            
            DynArray_add(generated_IR, ir2);
            
            ((IR *)generated_IR->data)[ifn_jump_pos].operands[1].label_index = add_specific_label(generated_IR, loop_end);
            break;
        }
        case TOKEN_FOR: {
            Node *init_cond_iter = AST->cond;
            
            IR_generate(init_cond_iter->left, generated_IR, current_scope, context);
            
            u64 loop_top = add_label(generated_IR);
            
            // condition
            ir->instruction = OP_IFN_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0] = IR_generate(init_cond_iter->cond, generated_IR, current_scope, context);
            ir->operands[1].type = OT_LABEL;
            ir->operands[1].named = false;
            
            DynArray_add(generated_IR, ir);
            u64 ifn_jump_pos = generated_IR->count-1;
            
            context->in_loop = true;
            context->loop_top = loop_top;
            u64 loop_end = label_index++;
            context->loop_end = loop_end;
            u64 loop_continue = label_index++;
            
            IR_generate(AST->left, generated_IR, current_scope, context);
            add_specific_label(generated_IR, loop_continue);
            IR_generate(init_cond_iter->right, generated_IR, current_scope, context);
            context->in_loop = false; // no longer in the loop lol
            
            IR *ir2 = malloc(sizeof(IR));
            ir2->block = NULL;
            ir2->instruction = OP_JUMP;
            ir2->result.type = OT_NONE;
            ir2->operands[0].type = OT_LABEL;
            ir2->operands[0].named = false;
            ir2->operands[0].label_index = loop_top;
            
            DynArray_add(generated_IR, ir2);
            
            ((IR *)generated_IR->data)[ifn_jump_pos].operands[1].label_index = add_specific_label(generated_IR, loop_end);
            break;
        }
        case TOKEN_BREAK: {
            if (!context->in_loop) {
                error(0, "Error: you can't have breaks outside of loops and switches my guy.");
            }
            ir->instruction = OP_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0].type = OT_LABEL;
            ir->operands[0].named = false;
            ir->operands[0].label_index = context->loop_end;
            
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_CONTINUE: {
            if (!context->in_loop) {
                error(0, "Error: you can't have continue outside of loops my guy.");
            }
            ir->instruction = OP_JUMP;
            ir->result.type = OT_NONE;
            ir->operands[0].type = OT_LABEL;
            ir->operands[0].named = false;
            ir->operands[0].label_index = context->loop_continue;
            
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_FUNCTION_CALL: {
            bool ret_is_void = is_void(AST->left->token->entry->type->function_type->return_type);
            
            Node *arg = AST->right;
            u64 argcnt = 0;
            if (arg) { // NOTE(mdizdar): only go through the params if they exist
                while (arg->token->type == ',') {
                    ++argcnt;
                    IR *param = malloc(sizeof(IR));
                    param->block = NULL;
                    param->instruction = OP_PUSH;
                    param->operands[0] = IR_generate(arg->right, generated_IR, current_scope, context);
                    param->operands[1].type = OT_SIZE;
                    param->operands[1].integer_value = 1;
                    param->result.type = OT_NONE;
                    arg = arg->left;
                    DynArray_add(generated_IR, param);
                }
                ++argcnt;
                IR *param = malloc(sizeof(IR));
                param->block = NULL;
                param->instruction = OP_PUSH;
                param->operands[0] = IR_generate(arg, generated_IR, current_scope, context);
                param->operands[1].type = OT_SIZE;
                param->operands[1].integer_value = 1;
                param->result.type = OT_NONE;
                DynArray_add(generated_IR, param);
            }
            
            ir->instruction = OP_CALL;
            ir->operands[0].type = OT_LABEL;
            ir->operands[0].named = true;
            ir->operands[0].label_name = AST->left->token->entry->name;
            // NOTE(mdizdar): this is stupid and not how function calls actually work but lets pretend it isn't
            ir->result.type = OT_NONE;
            DynArray_add(generated_IR, ir);
            
            for (u64 i = 0; i < argcnt; ++i) {
                IR *pop = malloc(sizeof(IR));
                pop->block = NULL;
                pop->instruction = OP_POP;
                pop->operands[0].type = OT_NONE;
                pop->operands[1].type = OT_SIZE;
                pop->operands[1].integer_value = 1; // TODO(mdizdar): this should depend on the size of the arguments
                pop->result.type = OT_NONE;
                DynArray_add(generated_IR, pop);
            }
            if (!ret_is_void) {
                IR *ret = malloc(sizeof(IR));
                ret->block = NULL;
                ret->instruction = OP_GET_RETURNED;
                ret->result.type = OT_TEMPORARY;
                ret->result.temporary_id = temporary_index++;
                DynArray_add(generated_IR, ret);
                return ret->result;
            }
            break;
        }
        case TOKEN_RETURN: {
            ir->instruction = OP_RETURN;
            if (AST->left == NULL) {
                ir->operands[0].type = OT_NONE;
            } else {
                ir->operands[0] = IR_generate(AST->left, generated_IR, current_scope, context); 
            }
            ir->result = ir->operands[0];
            DynArray_add(generated_IR, ir);
            break;
        }
        case TOKEN_NEXT: {
            free(ir); // yikes
            IR_generate(AST->left, generated_IR, current_scope, context);
            return IR_generate(AST->right, generated_IR, current_scope, context);
        }
        default: {
            error(0, "uh oh sister %d\n", AST->token->type);
        }
    }
    return ir->result;
}

#endif //IR_GEN_H
