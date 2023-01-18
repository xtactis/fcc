#ifndef IR2AVR_H
#define IR2AVR_H

#include "../utils/common.h"
#include "../IR/label.h"
#include "../IR/IR.h"
#include "../IR/basic_block.h"
#include "../AVR/AVR.h"
#include "../C/type.h"


DynArray findLabels(DynArray *ir) {
    DynArray labels;
    labels.element_size = sizeof(Label);
    labels.capacity = 0;
    labels.count = 0;
    
    IR *irs = (IR *)(ir->data);
    for (u64 i = 0; i < ir->count; ++i) {
        if (irs[i].instruction == OP_LABEL) {
            Label *newlabel = malloc(sizeof(Label));
            newlabel->named = irs[i].operands[0].named;
            if (irs[i].operands[0].named) {
                newlabel->label_name = irs[i].operands[0].label_name;
            } else {
                newlabel->label_index = irs[i].operands[0].label_index;
            }
            newlabel->ir_index = i;
            DynArray_add(&labels, newlabel);
        }
    }
    return labels;
}

void clashGraph(u64 current_reg, DynArray *regs, u8 *real_reg, u64 reg_number) {
    u64 *reg = regs[current_reg].data;
    bool found;
    // TODO(mdizdar): this should be all registers, but let's keep it simple for now
    for (u8 j = 16; j < 24; ++j) {
        found = false;
        for (u64 i = 0; i < regs[current_reg].count; ++i) {
            if (real_reg[reg[i]] == j) {
                found = true;
                break;
            }
        }
        if (found) continue;
        real_reg[current_reg] = j;
        break;
    }
    if (found) {
        error(0, "too many registers required");
    }
    for (u64 i = 0; i < regs[current_reg].count; ++i) {
        if (real_reg[reg[i]] != 255) continue;
        clashGraph(reg[i], regs, real_reg, reg_number);
    }
}

void IR2AVR(DynArray *ir, DynArray *AVR_instructions, u64 reg_number) {
#define APPEND_CMD(CMD, ...) { AVR *cmd = malloc(sizeof(AVR)); \
*cmd = CMD(__VA_ARGS__); \
DynArray_add(AVR_instructions, cmd); }
#define APPEND_LONG_CMD(CMD, ...) { u32 c = CMD(__VA_ARGS__); \
AVR *cmd = malloc(sizeof(AVR)); \
*cmd = (u16)(c >> 16); \
DynArray_add(AVR_instructions, cmd); \
cmd = malloc(sizeof(AVR)); \
*cmd = (u16)(c & 0xFFFF); \
DynArray_add(AVR_instructions, cmd); }
    
    IR *irs = (IR *)(ir->data);
    
    DynArray labels = findLabels(ir);
    makeBasicBlocks(ir, &labels);
    return;

    livenessAnalysis(ir);
    
    DynArray *regs = malloc(sizeof(DynArray) * reg_number);
    for (u64 i = 0; i < reg_number; ++i) {
        regs[i].count = 0;
        regs[i].capacity = 0;
        regs[i].data = NULL;
        regs[i].element_size = sizeof(u64);
    }
    for (u64 i = 0; i < ir->count; ++i) {
        IRVariable *live = irs[i].liveVars.data;
        for (u64 j = 0; j < irs[i].liveVars.count; ++j) {
            for (u64 k = j+1; k < irs[i].liveVars.count; ++k) {
                DynArray_add(&regs[live[j].temporary_id], &live[k].temporary_id);
                DynArray_add(&regs[live[k].temporary_id], &live[j].temporary_id);
            }
        }
    }
    
    u8 *real_reg = malloc(sizeof(reg_number));
    for (u64 i = 0; i < reg_number; ++i) {
        real_reg[i] = 255;
    }
    for (u64 i = 0; i < reg_number; ++i) {
        if (real_reg[i] != 255) continue;
        clashGraph(i, regs, real_reg, reg_number);
    }
    
    //*
    for (u64 j = 0; j < ir->count; ++j) {
        IRVariable* live = irs[j].liveVars.data;
        printf("%lu:\t", j);
        for (u64 i = 0; i < irs[j].liveVars.count; ++i) {
            char s[40];
            printf("%s, ", IRVariable_toStr(&live[i], s));
        }
        printf("\n");
    }
    for (u64 i = 0; i < reg_number; ++i) {
        printf("t%lu -> r%u\n", i, real_reg[i]);
    }
    //*/
    
    Label *ls = (Label *)labels.data;
    u64 skip_label_swap = (u64)-1;
    for (u64 i = 0; i < ir->count; ++i) {
        switch ((int)irs[i].instruction) {
            case '+': {
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(MOV, 24, rd);
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(ADD, 24, rr);
                } else {
                    u16 k = (u16)irs[i].operands[1].integer_value;
                    APPEND_CMD(SUBI, 24, -(k & 0xEF));
                }
                APPEND_CMD(MOV, res, 24);
                break;
            }
            case '-': {
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(MOV, 24, rd);
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(SBC, 24, rr);
                } else {
                    u16 k = (u16)irs[i].operands[1].integer_value;
                    APPEND_CMD(SUBI, 24, k & 0xFF);
                }
                APPEND_CMD(MOV, res, 24);
                break;
            }
            case '=': {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].result.type == OT_DEREF_TEMPORARY) {
                    APPEND_CMD(MOV, 30, res);
                    APPEND_CMD(LDI, 31, 0);
                    if (irs[i].operands[0].type == OT_TEMPORARY) {
                        u8 rd = real_reg[irs[i].operands[0].temporary_id];
                        APPEND_CMD(STDz, rd, 0);
                    } else {
                        u16 k = (u16)irs[i].operands[0].integer_value;
                        APPEND_CMD(LDI, 24, k & 0xFF);
                        APPEND_CMD(STDz, 24, 0);
                    }
                } else {
                    if (irs[i].operands[0].type == OT_TEMPORARY) {
                        u8 rd = real_reg[irs[i].operands[0].temporary_id];
                        APPEND_CMD(MOV, res, rd);
                    } else {
                        u16 k = (u16)irs[i].operands[0].integer_value;
                        APPEND_CMD(LDI, res, k & 0xFF);
                    }
                }
                break;
            }
            case '<': {
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(LDI, res, 1);
                // TODO(mdizdar): this only works in a very niche case
                u16 k = (u16)irs[i].operands[1].integer_value;
                APPEND_CMD(CPI, rd, k & 0xFF);
                APPEND_CMD(BRLO, 1);
                APPEND_CMD(LDI, res, 0);
                break;
            }
            case OP_EQUALS: {
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(LDI, res, 1);
                // TODO(mdizdar): this only works in a very niche case
                u16 k = (u16)irs[i].operands[1].integer_value;
                APPEND_CMD(CPI, rd, k & 0xFF);
                APPEND_CMD(BREQ, 1);
                APPEND_CMD(LDI, res, 0);
                break;
            }
            case '!': {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    if (rd != res) {
                        APPEND_CMD(MOV, res, rd);
                    }
                } else {
                    u16 k = (u16)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, res, k & 0xFF);
                }
                APPEND_CMD(COM, res);
                APPEND_CMD(ANDI, res, 1);
                break;
            }
            case '~': {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].result.type == OT_DEREF_TEMPORARY) {
                    APPEND_CMD(MOV, 30, res);
                    APPEND_CMD(LDI, 31, 0);
                    APPEND_CMD(LDDz, 24, 0);
                    res = 24;
                }
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    if (rd != res) {
                        APPEND_CMD(MOV, res, rd);
                    }
                } else {
                    u16 k = (u16)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, res, k & 0xFF);
                }
                APPEND_CMD(COM, res);
                break;
            }
            case '&': {
                u8 res = real_reg[irs[i].result.temporary_id];
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                if (irs[i].operands[0].type == OT_DEREF_TEMPORARY) {
                    APPEND_CMD(MOV, 30, rd);
                    APPEND_CMD(LDI, 31, 0);
                    APPEND_CMD(LDDz, 24, 0);
                } else {
                    APPEND_CMD(MOV, 24, rd);
                }
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(AND, 24, rr);
                } else {
                    u8 k = (u8)irs[i].operands[1].integer_value;
                    APPEND_CMD(ANDI, 24, k);
                }
                APPEND_CMD(MOV, res, 24);
                break;
            }
            case OP_BITSHIFT_LEFT: {
                // TODO(mdizdar): works only sometimes
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, 26, rd); // TODO(mdizdar): I don't like that this uses 26 as a temp register
                } else {
                    u8 k = (u8)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, 26, k);
                }
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(MOV, 24, rr);
                } else {
                    u8 k = (u8)irs[i].operands[1].integer_value;
                    APPEND_CMD(LDI, 24, k);
                }
                APPEND_CMD(MOV, 25, 24);
                APPEND_CMD(AND, 25, 25);
                APPEND_CMD(BREQ, 7);
                u32 jump_back = (u32)AVR_instructions->count;
                APPEND_CMD(LSL, 26);
                APPEND_CMD(DEC, 24);
                APPEND_CMD(MOV, 25, 24);
                APPEND_CMD(AND, 25, 25);
                APPEND_CMD(BREQ, 2);
                skip_label_swap = AVR_instructions->count;
                APPEND_LONG_CMD(JMP, jump_back);
                APPEND_CMD(MOV, res, 22);
                break;
            }
            case OP_LABEL: {
                for (u64 j = 0; j < labels.count; ++j) {
                    if (irs[i].operands[0].named != ls[j].named) continue;
                    if (ls[j].named) {
                        if (strcmp(irs[i].operands[0].label_name.data, ls[j].label_name.data)) continue;
                    } else {
                        if (irs[i].operands[0].label_index != ls[j].label_index) continue;
                    }
                    ls[j].correct_address = (u32)(AVR_instructions->count);
                    if (irs[i].operands[0].named && !strcmp(irs[i].operands[0].label_name.data, "__start")) {
                        APPEND_CMD(LDI, 28, 0x5f);
                        APPEND_CMD(LDI, 29, 0x04);
                        APPEND_CMD(OUT, 29, 0x3e);
                        APPEND_CMD(OUT, 28, 0x3d);
                    }
                    break;
                }
                break;
            }
            case OP_JUMP: {
                for (u64 j = 0; j < labels.count; ++j) {
                    if (irs[i].operands[0].named != ls[j].named) continue;
                    if (ls[j].named) {
                        if (strcmp(irs[i].operands[0].label_name.data, ls[j].label_name.data)) continue;
                    } else {
                        if (irs[i].operands[0].label_index != ls[j].label_index) continue;
                    }
                    APPEND_LONG_CMD(JMP, (u32)j); 
                    break;
                }
                break;
            }
            case OP_IFN_JUMP: {
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, 24, rd);
                } else {
                    u8 k = (u8)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, 24, k);
                }
                APPEND_CMD(AND, 24, 24);
                APPEND_CMD(BRNE, 2);
                
                for (u64 j = 0; j < labels.count; ++j) {
                    if (irs[i].operands[1].named != ls[j].named) continue;
                    if (ls[j].named) {
                        if (strcmp(irs[i].operands[1].label_name.data, ls[j].label_name.data)) continue;
                    } else {
                        if (irs[i].operands[1].label_index != ls[j].label_index) continue;
                    }
                    APPEND_LONG_CMD(JMP, (u32)j); 
                    break;
                }
                break;
            }
            case OP_CALL: {
                for (u64 j = 0; j < labels.count; ++j) {
                    if (irs[i].operands[0].named != ls[j].named) continue;
                    if (ls[j].named) {
                        if (strcmp(irs[i].operands[0].label_name.data, ls[j].label_name.data)) continue;
                    } else {
                        if (irs[i].operands[0].label_index != ls[j].label_index) continue;
                    }
                    APPEND_LONG_CMD(CALL, (u32)j); 
                    break;
                }
                break;
            }
            case OP_RETURN: {
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, 24, rd);
                } else if (irs[i].operands[0].type != OT_NONE) {
                    u16 k = (u16)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, 24, k & 0xFF);
                }
                APPEND_CMD(POP, 19);
                APPEND_CMD(POP, 18);
                APPEND_CMD(POP, 17);
                APPEND_CMD(POP, 16);
                
                APPEND_CMD(POP, 29);
                APPEND_CMD(POP, 28);
                APPEND_CMD(RET);
                break;
            }
            case OP_GET_RETURNED: {
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(MOV, res, 24);
                break;
            }
            case OP_PRELUDE: {
                // this isn't done, but it depends on the function
                APPEND_CMD(PUSH, 28);
                APPEND_CMD(PUSH, 29);
                APPEND_CMD(IN, 28, 0x3D);
                APPEND_CMD(IN, 29, 0x3E);
                
                APPEND_CMD(PUSH, 16);
                APPEND_CMD(PUSH, 17);
                APPEND_CMD(PUSH, 18);
                APPEND_CMD(PUSH, 19);
                
                break;
            }
            case OP_GET_ARG: {
                u8 res = real_reg[irs[i].result.temporary_id];
                u8 k = (u8)irs[i].operands[0].integer_value;;
                APPEND_CMD(LDDy, res, k+4);
                break;
            }
            case OP_PUSH: {
                if (irs[i].operands[0].type == OT_NONE) {
                    for (u64 j = 0; j < irs[i].operands[1].integer_value; ++j) {
                        APPEND_CMD(PUSH, 0);
                    }
                } else if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(PUSH, rd);
                } else {
                    u8 k = (u8)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, 24, k & 0xFF);
                    APPEND_CMD(PUSH, 24);
                }
                break;
            }
            case OP_POP: {
                if (irs[i].operands[0].type == OT_NONE) {
                    for (u64 j = 0; j < irs[i].operands[1].integer_value; ++j) {
                        APPEND_CMD(POP, 0);
                    }
                } else if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(POP, rd);
                }
                break;
            }
            default: {
                error(0, "si puka?");
            }
        }
    }
    AVR *ins = AVR_instructions->data;
    for (u64 i = 0; i < AVR_instructions->count; ++i) {
        if (i == skip_label_swap) {
            ++i;
            continue;
        }
        if ((ins[i] & 0xFE0E) == 0x940E) {
            u32 address = ((u32)(ins[i] & 0x01F1) << 16) | ins[i+1];
            u32 c = CALL(ls[address].correct_address);
            ins[i] = c >> 16;
            ins[i+1] = c & 0xFFFF;
            ++i;
        } else if ((ins[i] & 0xFE0E) == 0x940C) {
            u32 address = ((u32)(ins[i] & 0x01F1) << 16) | ins[i+1];
            u32 c = JMP(ls[address].correct_address);
            ins[i] = c >> 16;
            ins[i+1] = c & 0xFFFF;
            ++i;
        }
    }
}

#endif // IR2AVR_H
