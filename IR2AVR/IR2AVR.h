#ifndef IR2AVR_H
#define IR2AVR_H

#include "../utils/common.h"
#include "../IR/label.h"
#include "../IR/IR.h"
#include "../IR/basic_block.h"
#include "../AVR/AVR.h"
#include "../C/type.h"

// TODO(mdizdar): regs should probably be a typedef and not a raw u64
void clashGraph(u64 current_reg, u64Array *regs, u8 *real_reg, u64 reg_number) {
    u64 *reg = regs[current_reg].data; // this can't be correct lol?
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
#define APPEND_CMD(CMD, ...) AVRArray_push_back(AVR_instructions, CMD(__VA_ARGS__));
#define APPEND_LONG_CMD(CMD, ...) { \
    u32 c = CMD(__VA_ARGS__); \
    AVRArray_push_back(AVR_instructions, c >> 16); \
    AVRArray_push_back(AVR_instructions, c & 0xFFF); \
}

u8 load_to_reg(u8 reg, const IRVariable *var, const u8 *real_reg, AVRArray *AVR_instructions) {
    // TODO(mdizdar): handle things other than temporaries and literals. I'm not sure if the following
    // block is useful or not since IIRC the way I implemented derefs is REALLY stupid
    //           if (irs[i].operands[0].type == OT_DEREF_TEMPORARY) {
    //               APPEND_CMD(MOV, 30, rd);
    //               APPEND_CMD(LDI, 31, 0);
    //               APPEND_CMD(LDDz, 24, 0);
    if (var->type == OT_TEMPORARY) {
        APPEND_CMD(MOV, reg, real_reg[var->temporary_id]);
    } else {
        APPEND_CMD(LDI, reg, var->integer_value);
    }
    return reg;
}

void IR2AVR(IRArray *ir, AVRArray *AVR_instructions, LabelArray *labels, u64 reg_number) {
    IR *irs = (IR *)(ir->data);
    
    makeBasicBlocks(ir, labels);

    livenessAnalysis(ir);
    
    u64Array *regs = malloc(sizeof(u64Array) * reg_number);
    for (u64 i = 0; i < reg_number; ++i) {
        u64Array_construct(&regs[i]);
    }
    for (ARRAY_EACH(IR, it, ir)) {
        IRVariable *live = it->liveVars.data;
        for (u64 j = 0; j < it->liveVars.count; ++j) {
            for (u64 k = j+1; k < it->liveVars.count; ++k) {
                u64Array_push_ptr(&regs[live[j].temporary_id], &live[k].temporary_id);
                u64Array_push_ptr(&regs[live[k].temporary_id], &live[j].temporary_id);
            }
        }
    }
    
    u8 *real_reg = malloc(sizeof(u8) * reg_number);
    for (u64 i = 0; i < reg_number; ++i) {
        real_reg[i] = 255;
    }
    for (u64 i = 0; i < reg_number; ++i) {
        if (real_reg[i] != 255) continue;
        clashGraph(i, regs, real_reg, reg_number);
    }
    
    /*
    u64 j = 0;
    for (ARRAY_EACH(IR, it, ir)) {
        printf("%lu:\t", j);
        for (ARRAY_EACH(IRVariable, live, &it->liveVars)) {
            char s[40];
            printf("%s, ", IRVariable_toStr(live, s));
        }
        printf("\n");
        ++j;
    }
    for (u64 i = 0; i < reg_number; ++i) {
        printf("t%lu -> r%u\n", i, real_reg[i]);
    }
    //*/
    
    Label *ls = (Label *)labels->data;
    u64 skip_label_swap = (u64)-1;
    for (u64 i = 0; i < ir->count; ++i) {
        switch ((int)irs[i].instruction) {
            case OP_LOGICAL_OR: {
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(LDI, res, 0);
                u8 rd = load_to_reg(24, &irs[i].operands[0], real_reg, AVR_instructions);
                APPEND_CMD(TST, rd);
                APPEND_CMD(BREQ, 1);
                APPEND_CMD(LDI, res, 1);
                u8 rr = load_to_reg(24, &irs[i].operands[1], real_reg, AVR_instructions);
                APPEND_CMD(TST, rr);
                APPEND_CMD(BREQ, 1);
                APPEND_CMD(LDI, res, 1);

                break;
            }
            case OP_LOGICAL_AND: {
                // TODO(mdizdar): it'd probaby be better to just not do a test if one of the operands is 
                // a literal value, i.e. known at compile time. But we'll optimize later...
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(LDI, res, 1);
                u8 rd = load_to_reg(24, &irs[i].operands[0], real_reg, AVR_instructions);
                APPEND_CMD(TST, rd);
                APPEND_CMD(BRNE, 1);
                APPEND_CMD(LDI, res, 0);
                u8 rr = load_to_reg(24, &irs[i].operands[1], real_reg, AVR_instructions);
                APPEND_CMD(TST, rr);
                APPEND_CMD(BRNE, 1);
                APPEND_CMD(LDI, res, 0);
                
                break;
            }
            case '|': {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);
                if (irs[i].operands[1].type != OT_TEMPORARY) {
                    APPEND_CMD(ORI, res, irs[i].operands[1].integer_value);
                } else {
                    APPEND_CMD(OR, res, real_reg[irs[i].operands[1].temporary_id]);
                }
                break;
            }
            case '&': {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);
                if (irs[i].operands[1].type != OT_TEMPORARY) {
                    APPEND_CMD(ANDI, res, irs[i].operands[1].integer_value);
                } else {
                    APPEND_CMD(AND, res, real_reg[irs[i].operands[1].temporary_id]);
                }
                break;
            }
            case '^': {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);
                u8 rr = load_to_reg(24, &irs[i].operands[1], real_reg, AVR_instructions);
                APPEND_CMD(EOR, res, rr);

                break;
            }
            case '+': {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);

                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(ADD, res, rr);
                } else {
                    u16 k = (u16)irs[i].operands[1].integer_value;
                    APPEND_CMD(SUBI, res, -(k & 0xEF));
                }
                break;
            }
            case '-': {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(SBC, res, rr);
                } else {
                    u16 k = (u16)irs[i].operands[1].integer_value;
                    APPEND_CMD(SUBI, res, k & 0xFF);
                }
                break;
            }
            case OP_PLUS: {
                u8 res = load_to_reg(real_reg[irs[i].result.temporary_id], &irs[i].operands[0], real_reg, AVR_instructions);
                break;
            }
            case OP_MINUS: {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    if (rd != res) {
                        APPEND_CMD(MOV, res, rd);
                    }
                } else {
                    APPEND_CMD(LDI, res, (u16)irs[i].operands[0].integer_value);
                }
                APPEND_CMD(NEG, res);

                break;
            }
            case '*': {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, res, rd);
                } else {
                    u16 k = (u16)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, res, k);
                }
                if (irs[i].operands[1].type == OT_TEMPORARY) {
                    u8 rr = real_reg[irs[i].operands[1].temporary_id];
                    APPEND_CMD(MUL, res, rr);
                } else {
                    u16 k = (u16)irs[i].operands[1].integer_value;
                    APPEND_CMD(LDI, 25, k);
                    APPEND_CMD(MUL, res, 25);
                }
                break;
            }
            case '=': {
                u8 res = real_reg[irs[i].result.temporary_id];
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, res, rd);
                } else {
                    u16 k = (u16)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, res, k & 0xFF);
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
            case '>': {
                u8 rd = real_reg[irs[i].operands[0].temporary_id];
                u8 res = real_reg[irs[i].result.temporary_id];
                APPEND_CMD(LDI, res, 1);
                // TODO(mdizdar): this only works in a very niche case
                u16 k = (u16)irs[i].operands[1].integer_value;
                APPEND_CMD(CPI, rd, k & 0xFF);
                APPEND_CMD(BRGE, 1);
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
            case OP_DEREF: {
                u8 res = real_reg[irs[i].result.temporary_id];
                u8 r0 = load_to_reg(24, &irs[i].operands[0], real_reg, AVR_instructions);
                APPEND_CMD(MOV, 30, 24);
                APPEND_CMD(LDI, 31, 0); // TODO(mdizdar): this is incorrect, pointers are always 2 bytes wide, but there are many things wrong with this code
                APPEND_CMD(LDDz, res, 0); // TODO(mdizdar): this will only give us the correct behavior when dereferencing on the right side of an assignment
                                          // left side requires *storage* not loading; regrettably, such dereferencing appears to be extremely common
                                          // i.e. array indexing and struct member accessing through a pointer
                                          // I'll need to figure out a good way of either having the information (on which side of an assignment am I) at all times
                                          // or making it so it doesn't matter, and the actual loading/storing will happen exclusively with a '=' IR OP
                break;
            }
            case OP_LABEL: {
                for (u64 j = 0; j < labels->count; ++j) {
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
                for (u64 j = 0; j < labels->count; ++j) {
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
            case OP_IF_JUMP: {
                if (irs[i].operands[0].type == OT_TEMPORARY) {
                    u8 rd = real_reg[irs[i].operands[0].temporary_id];
                    APPEND_CMD(MOV, 24, rd);
                } else {
                    u8 k = (u8)irs[i].operands[0].integer_value;
                    APPEND_CMD(LDI, 24, k);
                }
                APPEND_CMD(AND, 24, 24);
                APPEND_CMD(BREQ, 2);
                
                for (u64 j = 0; j < labels->count; ++j) {
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
                
                for (u64 j = 0; j < labels->count; ++j) {
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
                for (u64 j = 0; j < labels->count; ++j) {
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
                if ((int)irs[i].instruction < 'z') {
                    error(0, "si puka? '%c'", (char)irs[i].instruction);
                } else {
                    error(0, "si puka? %d", (int)irs[i].instruction);
                }
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
#undef APPEND_CMD
#undef APPEND_LONG_CMD
}

#endif // IR2AVR_H
