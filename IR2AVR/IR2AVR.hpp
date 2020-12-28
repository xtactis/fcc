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
    for (size_t i = 0; i < ir.size(); ++i) {
        switch (ir[i].instruction) {
            case IRt::nop: {
                AVRinstructions.push_back(0);
                break;
            }
            case IRt::mov: {
                NOT_IMPL;
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