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

extern TemporaryID temporary_index;
extern LabelID label_index;

#define ADD_PHI(...) NOT_IMPL;

typedef SymbolTableEntryPtr STEPtr;
typedef TemporaryID TempID;

u64 STEPtr_hash(const STEPtr *key) {
    return u64_hash((const u64 *)key);
}

void STEPtr_copy(STEPtr *dest, const STEPtr *src) {
    *dest = *src;
}

bool STEPtr_eq(const STEPtr *a, const STEPtr *b) {
    return *a == *b;
}

void STEPtr_print(const STEPtr *p) {
    printf("%p", p);
}

void TempID_copy(TempID *dest, const TempID *src) {
    *dest = *src;
}

bool TempID_eq(const TempID *a, const TempID *b) {
    return *a == *b;
}

u64 TempID_hash(const TempID *a) {
    return u64_hash(a);
}

void TempID_print(const TempID *a) {
    printf("%lu", *a);
}

_generate_hash_map(STEPtr, TempID);
_generate_hash_map(TempID, TempID);

typedef struct IRContext {
    u64 loop_top;
    u64 loop_end;
    u64 loop_continue;

    u64 declaration_relative_address;
    
    bool in_loop;
    bool global;
    bool lhs;
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

u64 add_specific_label(IRArray *generated_IR, u64 index) {
    IRArray_push_back(generated_IR, (IR){
        .instruction = OP_LABEL,
        .operands[0] = {
            .type = OT_LABEL,
            .named = false,
            .label_index = index
        },
        .block = NULL
    });
    return index;
}

u64 add_label(IRArray *generated_IR) {
    if (generated_IR->count > 0) {
        IR *last = IRArray_back(generated_IR);
        // NOTE(mdizdar): this is a bit stupid, I should make labels more generic so we can jump to named labels as well if need be
        if (last->instruction == OP_LABEL && !last->operands[0].named) {
            return last->operands[0].label_index;
        }
    }
    IRArray_push_back(generated_IR, (IR){
        .instruction = OP_LABEL,
        .operands[0] = {
            .type = OT_LABEL,
            .named = false,
            .label_index = label_index++
        },
        .block = NULL
    });
    return label_index-1;
}

void add_named_label(IRArray *generated_IR, String *label_name) {
    IRArray_push_back(generated_IR, (IR) {
        .instruction = OP_LABEL,
        .operands[0] = {
            .type = OT_LABEL,
            .named = true,
            .label_name = *label_name
        },
        .block = NULL
    });
}

IR move_to_temp(IRVariable x) {
    return (IR) {
        .result = {
            .type = OT_TEMPORARY,
            .entry = 0,
            .temporary_id = temporary_index++
        },
        .operands[0] = x,
        .instruction = (Op)'='
    };
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

void find_changed_variables(u64 start_index, u64 end_index, const Scope *current_scope, const IRArray *generated_IR, STEPtrTempIDHashMap *changed_vars) {
    for (u64 i = start_index; i < end_index; ++i) {
        IR *ir = IRArray_at(generated_IR, i);
        if (ir->result.type != OT_TEMPORARY) {
            continue;
        }
        if (!ir->result.entry) {
            continue;
        }
        SymbolTableEntry *entry = (SymbolTableEntry *)ir->result.entry;
        SymbolTableEntry *same_entry = Scope_find(current_scope, &entry->name);
        if (entry == same_entry) {
            STEPtrTempIDHashMap_set(changed_vars, &entry, &entry->temporary_id);
        }
    }
}

TemporaryID get_id_before(const LineTemporaryIDArray *all_ids, Line line) {
    TemporaryID found = -1;
    for (ARRAY_EACH(LineTemporaryID, it, all_ids)) {
        if (it->line > line) break;
        found = it->id;
    }
    assert(found != (u64)-1);
    return found;
}

// TODO(mdizdar): not sure how to handle struct type scopes
// NOTE(mdizdar): returns the id of the result variable 
IRVariable IR_generate(Node *AST, IRArray *generated_IR, const Scope *current_scope, IRContext *context) {
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
            u64 i = 0;
            for (ARRAY_EACH_REV(Declaration, parameter, &entry->type->function_type->parameters)) {
                IR param = {
                    .instruction = OP_GET_ARG,
                    .result = {
                        .type = OT_TEMPORARY,
                        .temporary_id = temporary_index++
                    },
                    .operands[0] = {
                        .type = OT_INT64,
                        .integer_value = i
                    },
                    .block = NULL
                };
                SymbolTableEntry *dentry = Scope_find(current_scope, &parameter->name);
                dentry->temporary_id = param.result.temporary_id;
                IRArray_push_back(generated_IR, param);
                ++i;
            }
            IRArray_push_back(generated_IR, (IR) {
                .instruction = OP_PRELUDE,
                .operands[0] = {
                    .type = OT_INT16,
                    .integer_value = entry->type->function_type->size_of
                },
                .result.type = OT_NONE,
                .block = NULL
            });
            u64 old_relative_address = context->declaration_relative_address;
            context->declaration_relative_address = 0;
            context->global = false;
            IRVariable ret = IR_generate(AST->token->entry->type->function_type->block, generated_IR, current_scope, context);
            context->declaration_relative_address = old_relative_address;
            context->global = true;

            return ret;
        } else {
            IRVariable var = {
                .type = OT_TEMPORARY,
                .entry = (uintptr_t)AST->token->entry,
                .temporary_id = temporary_index++
            };
            SymbolTableEntry *entry = (SymbolTableEntry *)var.entry;
            entry->temporary_id = var.temporary_id;
            LineTemporaryIDArray_construct(&entry->all_temp_ids);
            LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                .id = var.temporary_id,
                .line = generated_IR->count,
            });
            AST->token->entry->location_in_memory = (Address) {
                .global = context->global,
                .offset = context->declaration_relative_address
            };
            context->declaration_relative_address += Type_sizeof(AST->token->entry->type);

            return var;
        }
    }
    
    if (AST->token->type == TOKEN_INT_LITERAL) {
        return (IRVariable) {
            .type = OT_INT16,
            .integer_value = AST->token->integer_value
        };
    } else if (AST->token->type == TOKEN_FLOAT_LITERAL) {
        return (IRVariable) {
            .type = OT_FLOAT,
            .float_value = AST->token->float_value
        };
    } else if (AST->token->type == TOKEN_DOUBLE_LITERAL) {
        return (IRVariable) {
            .type = OT_DOUBLE,
            .double_value = AST->token->double_value
        };
    } else if (AST->token->type == TOKEN_IDENT) {
        return (IRVariable) {
            .type = OT_TEMPORARY,
            .entry = (uintptr_t)AST->token->entry,
            .temporary_id = AST->token->entry->temporary_id
        };
    }
    context->lhs = false;
    
    IR ir = (IR){.block = NULL};
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
            ir.instruction = (Op)AST->token->type;
            ir.result.type = OT_TEMPORARY;
            ir.result.entry = 0;
            ir.result.temporary_id = temporary_index++;
            ir.operands[0] = IR_generate(AST->left, generated_IR, current_scope, context);
            ir.operands[1] = IR_generate(AST->right, generated_IR, current_scope, context);
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case '=': {
            ir.instruction = (Op)AST->token->type;
            context->lhs = true;
            ir.result = IR_generate(AST->left, generated_IR, current_scope, context);
            context->lhs = false;
            ir.operands[0] = IR_generate(AST->right, generated_IR, current_scope, context);
            if (ir.result.type == OT_TEMPORARY && ir.result.entry != 0) {
                ir.result.temporary_id = temporary_index++;
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            } 
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_ADD_ASSIGN: case TOKEN_SUB_ASSIGN:
        case TOKEN_MUL_ASSIGN: case TOKEN_DIV_ASSIGN: case TOKEN_MOD_ASSIGN:
        case TOKEN_OR_ASSIGN: case TOKEN_AND_ASSIGN: case TOKEN_XOR_ASSIGN:
        case TOKEN_BIT_L_ASSIGN: case TOKEN_BIT_R_ASSIGN: {
            ir.instruction = Token_comp_assign_to_Op(AST->token->type);
            ir.result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir.operands[0] = ir.result;
            ir.operands[1] = IR_generate(AST->right, generated_IR, current_scope, context);
            if (ir.result.type == OT_TEMPORARY && ir.result.entry != 0) {
                ir.result.temporary_id = temporary_index++;
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_BITNOT_ASSIGN: {
            ir.instruction = Token_comp_assign_to_Op(AST->token->type);
            ir.result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir.operands[0] = ir.result;
            if (ir.result.type == OT_TEMPORARY && ir.result.entry != 0) {
                ir.result.temporary_id = temporary_index++;
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_PREINC: case TOKEN_PREDEC: {
            ir.instruction = Token_comp_assign_to_Op(AST->token->type);
            ir.result = IR_generate(AST->left, generated_IR, current_scope, context);
            ir.operands[0] = ir.result;
            ir.operands[1].type = OT_INT8;
            ir.operands[1].integer_value = 1ULL;
            if (ir.result.type == OT_TEMPORARY && ir.result.entry != 0) {
                ir.result.temporary_id = temporary_index++;
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_POSTINC: case TOKEN_POSTDEC: {
            IR ir2 = (IR) {
                .instruction = (Op)'=',
                .result = {
                    .type = OT_TEMPORARY,
                    .entry = 0,
                    .temporary_id = temporary_index++
                },
                .operands[0] = IR_generate(AST->left, generated_IR, current_scope, context)
            };
            
            IRArray_push_ptr(generated_IR, &ir2);
            ir.instruction = Token_comp_assign_to_Op(AST->token->type);
            ir.result      = ir2.operands[0];
            ir.operands[0] = ir2.operands[0];
            ir.operands[1].type = OT_INT8;
            ir.operands[1].integer_value = 1ULL;
            if (ir.result.type == OT_TEMPORARY && ir.result.entry != 0) {
                ir.result.temporary_id = temporary_index++;
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }
            IRArray_push_ptr(generated_IR, &ir);
            ir = ir2; // this is done so we can return the original value as are the semantics of post inc/dec
            break;
        }
        case TOKEN_DEREF: {
            ir = (IR){
                .instruction = OP_DEREF,
                .result = {
                    .type = OT_TEMPORARY,
                    .entry = 0,
                    .temporary_id = temporary_index++
                },
                .operands[0] = IR_generate(AST->left, generated_IR, current_scope, context), 
            };
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_ADDRESS: // TODO(mdizdar): I'm not sure if address-of should stay as it is, or get the value of the address right now
        case '~': case '!': 
        case TOKEN_PLUS: case TOKEN_MINUS: {
            ir.instruction = Token_unary_to_Op(AST->token->type);
            ir.result.type = OT_TEMPORARY;
            ir.result.entry = 0;
            ir.result.temporary_id = temporary_index++;
            ir.operands[0] = IR_generate(AST->left, generated_IR, current_scope, context);
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case '?': {
            u64 before_if = add_label(generated_IR);

            // condition
            ir.instruction = OP_IF_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir.operands[1].type = OT_LABEL;
            ir.operands[1].named = false;
            
            IRArray_push_ptr(generated_IR, &ir);
            u64 top_of_ternary = generated_IR->count - 2;
            
            STEPtrTempIDHashMap changed_vars_left, changed_vars_right;
            STEPtrTempIDHashMap_construct(&changed_vars_left);
            STEPtrTempIDHashMap_construct(&changed_vars_right);

            // if false
            IRVariable Fres = IR_generate(AST->right, generated_IR, current_scope, context);
            if (Fres.type != OT_TEMPORARY) {
                IR moved = move_to_temp(Fres);
                IRArray_push_ptr(generated_IR, &moved);
                Fres = moved.result;
            }
            
            find_changed_variables(top_of_ternary+1, generated_IR->count, current_scope, generated_IR, &changed_vars_left);

            IR ir2 = {
                .instruction = OP_JUMP,
                .result.type = OT_NONE,
                .operands[0] = {
                    .type = OT_LABEL,
                    .named = false
                }
            };
            
            IRArray_push_ptr(generated_IR, &ir2);
            
            u64 left_bottom = add_label(generated_IR);
            IRArray_at(generated_IR, top_of_ternary+1)->operands[1].label_index = add_label(generated_IR); // after F
            u64 index_of_jmp = generated_IR->count - 2;
            
            // if true
            IRVariable Tres = IR_generate(AST->left, generated_IR, current_scope, context);
            if (Tres.type != OT_TEMPORARY) {
                IR moved = move_to_temp(Tres);
                IRArray_push_ptr(generated_IR, &moved);
                Tres = moved.result;
            }
            
            find_changed_variables(index_of_jmp+1, generated_IR->count, current_scope, generated_IR, &changed_vars_right);

            u64 right_bottom = add_label(generated_IR);
            IRArray_at(generated_IR, index_of_jmp)->operands[0].label_index = add_label(generated_IR); // after T

            STEPtrTempIDHashMap changed_vars_union;
            STEPtrTempIDHashMap_construct(&changed_vars_union);

            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_left)) {
                STEPtrTempIDHashMap_add(&changed_vars_union, &variable->key, &variable->value);
            }
            
            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_right)) {
                STEPtrTempIDHashMap_add(&changed_vars_union, &variable->key, &variable->value);
            }
            
            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_union)) {
                TempID *found_in_left = STEPtrTempIDHashMap_get(&changed_vars_left, &variable->key);
                TempID *found_in_right = STEPtrTempIDHashMap_get(&changed_vars_right, &variable->key);
                TempID last_id_before_if = get_id_before(&variable->key->all_temp_ids, top_of_ternary);

                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++,
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .named = false,
                        .entry = (uintptr_t)variable->key,
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .named = false,
                        .entry = (uintptr_t)variable->key,
                    },
                };

                if (found_in_right != NULL && found_in_left != NULL) {
                    ir.operands[0].temporary_id = *found_in_left;
                    ir.operands[0].label_index  = left_bottom;
                    ir.operands[1].temporary_id = *found_in_right;
                    ir.operands[1].label_index  = right_bottom;
                } else if (found_in_right != NULL) {
                    ir.operands[0].temporary_id = last_id_before_if;
                    ir.operands[0].label_index  = before_if;
                    ir.operands[1].temporary_id = *found_in_right;
                    ir.operands[1].label_index  = right_bottom;
                } else { // must be in left
                    ir.operands[0].temporary_id = *found_in_left;
                    ir.operands[0].label_index  = left_bottom;
                    ir.operands[1].temporary_id = last_id_before_if;
                    ir.operands[1].label_index  = before_if;
                }

                IRArray_push_ptr(generated_IR, &ir);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }

            ir = (IR) {
                .instruction = OP_PHI,
                .result = {
                    .type = OT_TEMPORARY,
                    .entry = 0,
                    .temporary_id = temporary_index++
                },
                .operands[0] = {
                    .type = OT_PHI_VAR,
                    .temporary_id = Tres.temporary_id,
                    .named = false,
                    .label_index = right_bottom,
                },
                .operands[1] = {
                    .type = OT_PHI_VAR,
                    .temporary_id = Fres.temporary_id,
                    .named = false,
                    .label_index = left_bottom,
                },
            };
            IRArray_push_ptr(generated_IR, &ir);

            STEPtrTempIDHashMap_destruct(&changed_vars_union);
            STEPtrTempIDHashMap_destruct(&changed_vars_left);
            STEPtrTempIDHashMap_destruct(&changed_vars_right);
            break;
        }
        case TOKEN_IF: {
            u64 before_if = add_label(generated_IR);
            // condition
            ir.instruction = OP_IFN_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir.operands[1].type = OT_LABEL;
            ir.operands[1].named = false;
            
            IRArray_push_ptr(generated_IR, &ir);
            u64 ifn_jump_pos = generated_IR->count-1;
            
            IR_generate(AST->left, generated_IR, current_scope, context);

            STEPtrTempIDHashMap changed_vars_left, changed_vars_right;
            STEPtrTempIDHashMap_construct(&changed_vars_left);
            STEPtrTempIDHashMap_construct(&changed_vars_right);

            find_changed_variables(ifn_jump_pos+1, generated_IR->count, current_scope, generated_IR, &changed_vars_left);

            u64 jump_out_pos = -1;
            if (AST->right) {
                ir = (IR) {
                    .instruction = OP_JUMP,
                    .operands[0] = {
                        .type = OT_LABEL,
                        .named = false
                    }
                };
                IRArray_push_ptr(generated_IR, &ir);
                jump_out_pos = generated_IR->count-1;
            }

            u64 left_bottom = add_label(generated_IR);
            IRArray_at(generated_IR, ifn_jump_pos)->operands[1].label_index = add_label(generated_IR);
            
            u64 right_bottom = -1;
            if (AST->right) { // else
                IR_generate(AST->right, generated_IR, current_scope, context);

                IRArray_at(generated_IR, jump_out_pos)->operands[0].label_index = add_label(generated_IR);
                right_bottom = add_label(generated_IR);

                find_changed_variables(jump_out_pos+1, generated_IR->count, current_scope, generated_IR, &changed_vars_right);
            }

            STEPtrTempIDHashMap changed_vars_union;
            STEPtrTempIDHashMap_construct(&changed_vars_union);

            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_left)) {
                STEPtrTempIDHashMap_add(&changed_vars_union, &variable->key, &variable->value);
            }
            
            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_right)) {
                STEPtrTempIDHashMap_add(&changed_vars_union, &variable->key, &variable->value);
            }

            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars_union)) {
                TempID *found_in_left = STEPtrTempIDHashMap_get(&changed_vars_left, &variable->key);
                TempID *found_in_right = STEPtrTempIDHashMap_get(&changed_vars_right, &variable->key);
                TempID last_id_before_if = get_id_before(&variable->key->all_temp_ids, ifn_jump_pos);

                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++,
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .named = false,
                        .entry = (uintptr_t)variable->key,
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .named = false,
                        .entry = (uintptr_t)variable->key,
                    },
                };

                if (found_in_right != NULL && found_in_left != NULL) {
                    ir.operands[0].temporary_id = *found_in_left;
                    ir.operands[0].label_index  = left_bottom;
                    ir.operands[1].temporary_id = *found_in_right;
                    ir.operands[1].label_index  = right_bottom;
                } else if (found_in_right != NULL) {
                    ir.operands[0].temporary_id = last_id_before_if;
                    ir.operands[0].label_index  = before_if;
                    ir.operands[1].temporary_id = *found_in_right;
                    ir.operands[1].label_index  = right_bottom;
                } else { // must be in left
                    ir.operands[0].temporary_id = *found_in_left;
                    ir.operands[0].label_index  = left_bottom;
                    ir.operands[1].temporary_id = last_id_before_if;
                    ir.operands[1].label_index  = before_if;
                }
                IRArray_push_ptr(generated_IR, &ir);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }

            STEPtrTempIDHashMap_destruct(&changed_vars_union);
            STEPtrTempIDHashMap_destruct(&changed_vars_left);
            STEPtrTempIDHashMap_destruct(&changed_vars_right);
            break;
        }
        case TOKEN_WHILE: {
            u64 loop_top = add_label(generated_IR);
            
            STEPtrTempIDHashMap changed_vars;
            STEPtrTempIDHashMap_construct(&changed_vars);
            // condition
            ir.instruction = OP_IFN_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0] = IR_generate(AST->cond, generated_IR, current_scope, context);
            ir.operands[1].type = OT_LABEL;
            ir.operands[1].named = false;
            
            IRArray_push_ptr(generated_IR, &ir);
            u64 ifn_jump_pos = generated_IR->count-1;
            
            context->in_loop = true;
            context->loop_top = loop_top;
            context->loop_continue = loop_top;
            u64 loop_end = label_index++;
            context->loop_end = loop_end;
            
            IR_generate(AST->left, generated_IR, current_scope, context);
            context->in_loop = false; // no longer in the loop lol
            
            find_changed_variables(ifn_jump_pos+1, generated_IR->count, current_scope, generated_IR, &changed_vars);

            IR ir2 = (IR) {};
            ir2.block = NULL;
            ir2.instruction = OP_JUMP;
            ir2.result.type = OT_NONE;
            ir2.operands[0].type = OT_LABEL;
            ir2.operands[0].named = false;
            ir2.operands[0].label_index = loop_top;
            
            IRArray_push_ptr(generated_IR, &ir2);
            
            IRArray_at(generated_IR, ifn_jump_pos)->operands[1].label_index = add_specific_label(generated_IR, loop_end);

            TempIDTempIDHashMap old2phi;
            TempIDTempIDHashMap_construct(&old2phi);

            Line insertion_point = ifn_jump_pos - 1;
            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars)) {
                TempID old_id = get_id_before(&variable->key->all_temp_ids, ifn_jump_pos);
                TempIDTempIDHashMap_add(&old2phi, &old_id, &temporary_index);
                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = variable->value,
                        .named = false,
                        .label_index = loop_end,
                        .entry = (uintptr_t)variable->key
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = old_id,
                        .named = false,
                        .label_index = loop_top,
                        .entry = (uintptr_t)variable->key
                    },
                };
                IRArray_insert_ptr(generated_IR, &ir, insertion_point);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = insertion_point,
                });
                ++insertion_point;
            }

            for (u64 i = insertion_point; i < generated_IR->count; ++i) {
                IR *it = IRArray_at(generated_IR, i);
                if (it->operands[0].type == OT_TEMPORARY) {
                    TempID *replacement = TempIDTempIDHashMap_get(&old2phi, &it->operands[0].temporary_id);
                    if (replacement != NULL) {
                        it->operands[0].temporary_id = *replacement;
                    }
                }
                if (it->operands[1].type == OT_TEMPORARY) {
                    TempID *replacement = TempIDTempIDHashMap_get(&old2phi, &it->operands[1].temporary_id);
                    if (replacement != NULL) {
                        it->operands[1].temporary_id = *replacement;
                    }
                }
            }

            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars)) {
                TempID old_id = get_id_before(&variable->key->all_temp_ids, ifn_jump_pos);
                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = variable->value,
                        .named = false,
                        .label_index = loop_end,
                        .entry = (uintptr_t)variable->key
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = old_id,
                        .named = false,
                        .label_index = loop_top,
                        .entry = (uintptr_t)variable->key
                    },
                };
                IRArray_push_ptr(generated_IR, &ir);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }

            TempIDTempIDHashMap_destruct(&old2phi);
            STEPtrTempIDHashMap_destruct(&changed_vars);
            break;
        }
        case TOKEN_FOR: {
            STEPtrTempIDHashMap changed_vars;
            STEPtrTempIDHashMap_construct(&changed_vars);

            Node *init_cond_iter = AST->cond;
            
            IR_generate(init_cond_iter->left, generated_IR, current_scope, context);
            
            u64 loop_top = add_label(generated_IR);
            
            // condition
            ir.instruction = OP_IFN_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0] = IR_generate(init_cond_iter->cond, generated_IR, current_scope, context);
            ir.operands[1].type = OT_LABEL;
            ir.operands[1].named = false;
            
            IRArray_push_ptr(generated_IR, &ir);
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
            
            find_changed_variables(ifn_jump_pos+1, generated_IR->count, current_scope, generated_IR, &changed_vars);

            IR ir2 = {
                .block = NULL,
                .instruction = OP_JUMP,
                .result.type = OT_NONE,
                .operands[0] = {
                    .type = OT_LABEL,
                    .named = false,
                    .label_index = loop_top
                }
            };
            
            IRArray_push_ptr(generated_IR, &ir2);
            
            IRArray_at(generated_IR, ifn_jump_pos)->operands[1].label_index = add_specific_label(generated_IR, loop_end);

            TempIDTempIDHashMap old2phi;
            TempIDTempIDHashMap_construct(&old2phi);

            Line insertion_point = ifn_jump_pos - 1;
            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars)) {
                TempID old_id = get_id_before(&variable->key->all_temp_ids, ifn_jump_pos);
                TempIDTempIDHashMap_add(&old2phi, &old_id, &temporary_index);
                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = variable->value,
                        .named = false,
                        .label_index = loop_end,
                        .entry = (uintptr_t)variable->key
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = old_id,
                        .named = false,
                        .label_index = loop_top,
                        .entry = (uintptr_t)variable->key
                    },
                };
                IRArray_insert_ptr(generated_IR, &ir, insertion_point);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = insertion_point,
                });
                ++insertion_point;
            }

            for (u64 i = insertion_point; i < generated_IR->count; ++i) {
                IR *it = IRArray_at(generated_IR, i);
                if (it->operands[0].type == OT_TEMPORARY) {
                    TempID *replacement = TempIDTempIDHashMap_get(&old2phi, &it->operands[0].temporary_id);
                    if (replacement != NULL) {
                        it->operands[0].temporary_id = *replacement;
                    }
                }
                if (it->operands[1].type == OT_TEMPORARY) {
                    TempID *replacement = TempIDTempIDHashMap_get(&old2phi, &it->operands[1].temporary_id);
                    if (replacement != NULL) {
                        it->operands[1].temporary_id = *replacement;
                    }
                }
            }

            for (HASH_MAP_EACH(STEPtr, TempID, variable, &changed_vars)) {
                TempID old_id = get_id_before(&variable->key->all_temp_ids, ifn_jump_pos);
                IR ir = {
                    .instruction = OP_PHI,
                    .result = {
                        .type = OT_TEMPORARY,
                        .entry = (uintptr_t)variable->key,
                        .temporary_id = temporary_index++
                    },
                    .operands[0] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = variable->value,
                        .named = false,
                        .label_index = loop_end,
                        .entry = (uintptr_t)variable->key
                    },
                    .operands[1] = {
                        .type = OT_PHI_VAR,
                        .temporary_id = old_id,
                        .named = false,
                        .label_index = loop_top,
                        .entry = (uintptr_t)variable->key
                    },
                };
                IRArray_push_ptr(generated_IR, &ir);
                SymbolTableEntry *entry = (SymbolTableEntry *)ir.result.entry;
                entry->temporary_id = ir.result.temporary_id;
                LineTemporaryIDArray_push_back(&entry->all_temp_ids, (LineTemporaryID) {
                    .id = ir.result.temporary_id,
                    .line = generated_IR->count,
                });
            }

            TempIDTempIDHashMap_destruct(&old2phi);
            STEPtrTempIDHashMap_destruct(&changed_vars);
            break;
        }
        case TOKEN_BREAK: {
            if (!context->in_loop) {
                error(0, "Error: you can't have breaks outside of loops and switches my guy.");
            }
            ir.instruction = OP_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0].type = OT_LABEL;
            ir.operands[0].named = false;
            ir.operands[0].label_index = context->loop_end;
            
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_CONTINUE: {
            if (!context->in_loop) {
                error(0, "Error: you can't have continue outside of loops my guy.");
            }
            ir.instruction = OP_JUMP;
            ir.result.type = OT_NONE;
            ir.operands[0].type = OT_LABEL;
            ir.operands[0].named = false;
            ir.operands[0].label_index = context->loop_continue;
            
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_FUNCTION_CALL: {
            bool ret_is_void = is_void(AST->left->token->entry->type->function_type->return_type);
            
            Node *arg = AST->right;
            u64 argcnt = 0;
            if (arg) { // NOTE(mdizdar): only go through the params if they exist
                while (arg->token->type == ',') {
                    ++argcnt;
                    IR param = (IR) {
                        .instruction = OP_PUSH,
                        .operands[0] = IR_generate(arg->right, generated_IR, current_scope, context),
                        .operands[1] = {
                            .type = OT_SIZE,
                            .integer_value = 1
                        },
                        .result.type = OT_NONE
                    };
                    arg = arg->left;
                    IRArray_push_ptr(generated_IR, &param);
                }
                ++argcnt;
                IR param = (IR) {
                    .instruction = OP_PUSH,
                    .operands[0] = IR_generate(arg, generated_IR, current_scope, context),
                    .operands[1] = {
                        .type = OT_SIZE,
                        .integer_value = 1
                    },
                    .result.type = OT_NONE
                };
                IRArray_push_ptr(generated_IR, &param);
            }
            
            ir.instruction = OP_CALL;
            ir.operands[0].type = OT_LABEL;
            ir.operands[0].named = true;
            ir.operands[0].label_name = AST->left->token->entry->name;
            // NOTE(mdizdar): this is stupid and not how function calls actually work but lets pretend it isn't
            ir.result.type = OT_NONE;
            IRArray_push_ptr(generated_IR, &ir);
            
            for (u64 i = 0; i < argcnt; ++i) {
                IR pop = (IR) {
                    .instruction = OP_POP,
                    .operands[0].type = OT_NONE,
                    .operands[1] = {
                        .type = OT_SIZE,
                        .integer_value = 1 // TODO(mdizdar): this should depend on the size of the arguments
                    },
                    .result.type = OT_NONE
                };
                IRArray_push_ptr(generated_IR, &pop);
            }
            if (!ret_is_void) {
                IR ret = (IR) {
                    .instruction = OP_GET_RETURNED,
                    .result = {
                        .type = OT_TEMPORARY,
                        .temporary_id = temporary_index++
                    }
                };
                IRArray_push_ptr(generated_IR, &ret);
                return ret.result;
            }
            break;
        }
        case TOKEN_RETURN: {
            ir.instruction = OP_RETURN;
            if (AST->left == NULL) {
                ir.operands[0].type = OT_NONE;
            } else {
                ir.operands[0] = IR_generate(AST->left, generated_IR, current_scope, context); 
            }
            ir.result = ir.operands[0];
            IRArray_push_ptr(generated_IR, &ir);
            break;
        }
        case TOKEN_NEXT: {
            IR_generate(AST->left, generated_IR, current_scope, context);
            return IR_generate(AST->right, generated_IR, current_scope, context);
        }
        default: {
            error(0, "uh oh sister %d\n", AST->token->type);
        }
    }
    return ir.result;
}

#endif //IR_GEN_H
