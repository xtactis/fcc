#include <vector>

#include "../utils/common.hpp"
#include "../IR/IR.hpp"

/*
  currently the IR registers are 1 to 1 with AVR registers, meaning there's only 32 of them
not entirely convinced this is the best way of going about it
*/

void printAVR(uint32_t instruction) {
    if (instruction == 0) {
        // NOP
        printf("[0x0000]\tnop\n");
    } else if ((instruction >> 24) == 1) {
        // MOVW
        NOT_IMPL;
    } else if ((instruction >> 26) == 0x0B) {
        // MOVRR
        instruction >>= 16;
        int r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5),
        r2 = (instruction & 0x1F0) >> 4;
        printf("[0x%x]\tmov r%d, r%d\n", instruction, r2, r1);
    } else if ((instruction >> 28) == 0xE) {
        // LDI
        instruction >>= 16;
        int value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
        int reg = (instruction >> 4) & 0xF;
        printf("[0x%x]\tldi r%d, 0x%x\n", instruction, reg+16, value);
    } else if ((instruction & 0xFE0F0000) == 0x900F0000) {
        // POP
        instruction >>= 16;
        int reg = (instruction & 0x01F0) >> 4;
        printf("[0x%x]\tpop r%d\n", instruction, reg);
    } else if ((instruction & 0xFE0F0000) == 0x920F0000) {
        // PUSH
        instruction >>= 16;
        int reg = (instruction & 0x01F0) >> 4;
        printf("[0x%x]\tpush r%d\n", instruction, reg);
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
        switch (IRt(ir[i] >> 24)) {
            case IRt::nop: {
                printAVR(0);
                break;
            }
            case IRt::movrr: {
                int value = (ir[i] & 0x00FFC000) >> 14;
                int r1 = value & 0x1F, r2 = (value >> 5) & 0x1F;
                printAVR((0x2C00 | (r1 & 0xF) | ((r1 & 0x10) << 4) | (r2 << 4)) << 16);
                break;
            }
            case IRt::movrm: {
                NOT_IMPL;
                break;
            }
            case IRt::movcr: {
                int value = (ir[i] & 0x00FF0000) >> 16;
                int reg   = (ir[i] & 0x0000F000) >> 12;
                printAVR((0xE000 | ((value & 0xF0) << 4) | (value & 0xF) | (reg << 4)) << 16);
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
                NOT_IMPL;
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
                int value = (ir[i] & 0x00FFFF00) >> 8;
                printAVR((0xE080 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                value >>= 8;
                printAVR((0xE090 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                printAVR(0x95080000);
                break;
            }
            case IRt::pop: { // pop into register
                int reg = (ir[i] & 0x00F80000) >> 19;
                printAVR((0x900F | (reg << 4)) << 16);
                break;
            }
            case IRt::push: { // push from register, consider adding push from constant
                int reg = (ir[i] & 0x00F80000) >> 19;
                printAVR((0x920F | (reg << 4)) << 16);
                break;
            }
            case IRt::cmp: { // do we need this?
                NOT_IMPL;
                break;
            }
            default: {
                error("bro... what even is that IR");
            }
        }
    }
}
