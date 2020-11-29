#ifndef IR2AVR_H
#define IR2AVR_H

#include <vector>

#include "../utils/common.hpp"
#include "../IR/IR.hpp"

/*
currently the IR registers are 1 to 1 with AVR registers, meaning there's only 32 of them
 not entirely convinced this is the best way of going about it
*/

void IR2AVR(const std::vector<IR> &ir, std::vector<u32> &AVRinstructions) {
    // NOTE(mdizdar): currently IR instructions are 1 to 1 with AVR ones, this shouldn't be the case since the IR should assume e.g. multiplication can be done between any two registers (of which there's an infinite number)
    // TODO(mdizdar): ... so, there should be a table of identifiers so the mapping can be done correctly
    for (size_t i = 0; i < ir.size(); ++i) {
        switch (ir[i].instruction) {
            case IRt::nop: {
                AVRinstructions.push_back(0);
                break;
            }
            case IRt::mov: {
                if (ir[i].imm == 0) { // rr
                    const u32 r1 = ir[i].operands[0], r2 = ir[i].operands[1];
                    AVRinstructions.push_back((0x2C00 | (r1 & 0xF) | ((r1 & 0x10) << 4) | (r2 << 4)) << 16);
                } else if (ir[i].imm == 2) { // cr
                    const u32 value = ir[i].operands[0];
                    const u32 reg = ir[i].operands[1];
                    AVRinstructions.push_back((0xE000 | ((value & 0xF0) << 4) | (value & 0xF) | (reg << 4)) << 16);
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
                    AVRinstructions.push_back((0x1C00 | (r1 & 0xF) | ((r1 & 0x10) << 4) | (r2 << 4)) << 16);
                } else if (ir[i].imm == 2) { // cr
                    const u32 value = ir[i].operands[0];
                    const u32 reg = ir[i].operands[1];
                    AVRinstructions.push_back((0x4000 | ((value & 0xF0) << 4) | (value & 0xF) | (reg << 4)) << 16);
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
                AVRinstructions.push_back(0x95080000);
                break;
            }
            case IRt::retc: {
                u32 value = ir[i].operands[0];
                AVRinstructions.push_back((0xE080 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                value >>= 8;
                AVRinstructions.push_back((0xE090 | ((value & 0xF0) << 4) | (value & 0xF)) << 16);
                AVRinstructions.push_back(0x95080000);
                break;
            }
            case IRt::pop: { // pop into register
                const u32 reg = ir[i].operands[0];
                AVRinstructions.push_back((0x900F | (reg << 4)) << 16);
                break;
            }
            case IRt::push: { // push from register, consider adding push from constant
                const u32 reg = ir[i].operands[0];
                AVRinstructions.push_back((0x920F | (reg << 4)) << 16);
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