#ifndef IR2AVR_H
#define IR2AVR_H

#include <vector>

#include "../utils/common.hpp"
#include "../IR/IR.hpp"

/*
currently the IR registers are 1 to 1 with AVR registers, meaning there's only 32 of them
 not entirely convinced this is the best way of going about it
*/

// NOTE(mdizdar): at some point we might want/need AVR instructions to be >32 bit, I am unaware of the existence of such instructions though
void printAVR(u32 instruction) {
    if (instruction == 0) {
        // NOP
        printf("[0x0000]\tnop\n");
    } else if ((instruction >> 24) == 1) {
        // MOVW
        NOT_IMPL;
    } else if ((instruction >> 26) == 0x0B) {
        // MOVRR
        instruction >>= 16;
        // TODO(mdizdar): make extracting the two registers, a register and a value, and just a value into functions... I guess?
        const u16 r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
        const u16 r2 = (instruction & 0x1F0) >> 4;
        printf("[0x%x]\tmov r%d, r%d\n", instruction, r2, r1);
    } else if ((instruction >> 28) == 0xE) {
        // LDI
        instruction >>= 16;
        const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
        const u16 reg = (instruction >> 4) & 0xF;
        printf("[0x%x]\tldi r%d, 0x%x\n", instruction, reg+16, value);
    } else if ((instruction & 0xFE0F0000) == 0x900F0000) {
        // POP
        instruction >>= 16;
        const u16 reg = (instruction & 0x01F0) >> 4;
        printf("[0x%x]\tpop r%d\n", instruction, reg);
    } else if ((instruction & 0xFE0F0000) == 0x920F0000) {
        // PUSH
        instruction >>= 16;
        const u16 reg = (instruction & 0x01F0) >> 4;
        printf("[0x%x]\tpush r%d\n", instruction, reg);
    } else if ((instruction >> 26) == 0x7) {
        // ADC
        instruction >>= 16;
        const u16 r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
        const u16 r2 = (instruction & 0x1F0) >> 4;
        printf("[0x%x]\tadc r%d, r%d\n", instruction, r1, r2);
    } else if ((instruction >> 28) == 0x4) {
        // SBCI
        instruction >>= 16;
        const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
        const u16 reg = (instruction >> 4) & 0xF;
        printf("[0x%x]\tsbci r%d, 0x%x\n", instruction, reg+16, value);
    } else if (instruction == 0x95080000) {
        // RET
        printf("[0x9508]\tret\n");
    } else {
        error("You have invented a new AVR instruction, gz");
    }
}

void IR2AVR(const std::vector<IR> &ir) {
    // https://en.wikipedia.org/wiki/Atmel_AVR_instruction_set#Instruction_encoding
    // http://ww1.microchip.com/downloads/cn/DeviceDoc/AVR-Instruction-Set-Manual-DS40002198A.pdf
    for (size_t i = 0; i < ir.size(); ++i) {
        switch (ir[i].instruction) {
            // TODO(mdizdar): currently an IR is just an int, this definitely won't work when we start doing things for non AVR chips - it works for those because their instruction word is at most 32 bit and a value is at most 16 bit. We'll need a better way of storing the instruction and its operands; likely 32-64 bit for each.
            case IRt::nop: {
                printAVR(0);
                break;
            }
            case IRt::mov: {
                if (ir[i].imm == 0) { // rr
                    const u32 r1 = ir[i].operands[0], r2 = ir[i].operands[1];
                    printAVR((0x2C00 | (r1 & 0xF) | ((r1 & 0x10) << 4) | (r2 << 4)) << 16);
                } else if (ir[i].imm == 2) { // cr
                    const u32 value = ir[i].operands[0];
                    const u32 reg = ir[i].operands[1];
                    printAVR((0xE000 | ((value & 0xF0) << 4) | (value & 0xF) | (reg << 4)) << 16);
                } else {
                    error("imm corrupt or your mov operands don't make sense");
                }
                break;
            }
            case IRt::movrm: { // TODO(mdizdar): these should be ld/st type operations
                NOT_IMPL;
                break;
            }
            case IRt::movcm: {
                NOT_IMPL;
                break;
            }
            case IRt::movmr: {
                NOT_IMPL;
                break;
            }
            case IRt::movmm: {
                NOT_IMPL;
                break;
            }
            case IRt::AND: {
                NOT_IMPL;
                break;
            }
            case IRt::OR: {
                NOT_IMPL;
                break;
            }
            case IRt::XOR: {
                NOT_IMPL;
                break;
            }
            case IRt::add: {
                if (ir[i].imm == 0) { // rr
                    const u32 r1 = ir[i].operands[0], r2 = ir[i].operands[1];
                    printAVR((0x1C00 | (r1 & 0xF) | ((r1 & 0x10) << 4) | (r2 << 4)) << 16);
                } else if (ir[i].imm == 2) { // cr
                    const u32 value = ir[i].operands[0];
                    const u32 reg = ir[i].operands[1];
                    printAVR((0x4000 | ((value & 0xF0) << 4) | (value & 0xF) | (reg << 4)) << 16);
                } else {
                    error("imm corrupt or your add operands don't make sense");
                }
                break;
            }
            case IRt::sub: {
                NOT_IMPL;
                break;
            }
            case IRt::mul: {
                NOT_IMPL;
                break;
            }
            case IRt::div: {
                NOT_IMPL;
                break;
            }
            case IRt::mod: {
                NOT_IMPL;
                break;
            }
            case IRt::NOT: {
                NOT_IMPL;
                break;
            }
            // ...
            case IRt::jmp: {
                NOT_IMPL;
                break;
            }
            case IRt::jmpz: {
                NOT_IMPL;
                break;
            }
            case IRt::jmpnz: {
                NOT_IMPL;
                break;
            }
            case IRt::jmpgz: {
                NOT_IMPL;
                break;
            }
            case IRt::jmpeq: {
                NOT_IMPL;
                break;
            }
            case IRt::jmpne: { // ??
                NOT_IMPL;
                break;
            }
            case IRt::jmplt: {
                NOT_IMPL;
                break;
            }
            case IRt::jmple: {
                NOT_IMPL;
                break;
            }
            // ...
            case IRt::call: {
                NOT_IMPL;
                break;
            }
            case IRt::ret: {
                // if it's already in the registers, just return?
                printAVR(0x95080000);
                break;
            }
            case IRt::retc: {
                u32 value = ir[i].operands[0];
                printAVR((0xE080 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                value >>= 8;
                printAVR((0xE090 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                printAVR(0x95080000);
                break;
            }
            case IRt::pop: { // pop into register
                const u32 reg = ir[i].operands[0];
                printAVR((0x900F | (reg << 4)) << 16);
                break;
            }
            case IRt::push: { // push from register, consider adding push from constant
                const u32 reg = ir[i].operands[0];
                printAVR((0x920F | (reg << 4)) << 16);
                break;
            }
            case IRt::cmp: { // do we need this?
                NOT_IMPL;
                break;
            }
            default: {
                error("si puka?");
            }
        }
    }
}

#endif // IR2AVR_H