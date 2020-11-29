#ifndef IR2AVR_H
#define IR2AVR_H

#include <vector>

#include "../utils/common.hpp"
#include "../IR/IR.hpp"

/*
currently the IR registers are 1 to 1 with AVR registers, meaning there's only 32 of them
 not entirely convinced this is the best way of going about it
*/
namespace AVR {
    void print(const std::vector<u16> &instructions) {
        // https://en.wikipedia.org/wiki/Atmel_AVR_instruction_set#Instruction_encoding
        // http://ww1.microchip.com/downloads/cn/DeviceDoc/AVR-Instruction-Set-Manual-DS40002198A.pdf
        
        // NOTE(mdizdar): I wonder if there's a nicer way of going about this... (there is)
        for (size_t i = 0; i < instructions.size(); ++i) {
            const u16 &instruction = instructions[i];
            if ((instruction & 0xFC0F) == 0x9000) {
                // LDS/STS - 32-bit instruction
                const u16 SRAMaddress = instructions[++i];
                const u16 reg = instruction & 0x003F;
                if (instruction & 0x0200) {
                    // STS
                    printf("[0x%04x%04x]\tsts r%d, 0x%x\n", instruction, SRAMaddress, reg, SRAMaddress << 1);
                } else {
                    // LDS
                    printf("[0x%04x%04x]\tlds 0x%x, r%d\n", instruction, SRAMaddress, SRAMaddress << 1, reg);
                }
                continue;
            }
            if ((instruction & 0xFE0C) == 0x940C) {
                // JMP/CALL - 32-bit instruction
                const u16 hi = instructions[++i];
                const u32 SRAMaddress = hi | ((instruction & 0x01F0) << 14) | ((instruction & 0x0001) << 16);
                if (instruction & 0x0002) {
                    // CALL
                    printf("[0x%04x%04x]\tcall 0x%x\n", instruction, hi, SRAMaddress << 1);
                } else {
                    // JMP
                    printf("[0x%04x%04x]\tjmp 0x%x\n", instruction, hi, SRAMaddress << 1);
                }
                continue;
            }
            
            // from here on it's only 16 bit instructions
            
            if (instruction == 0) {
                // NOP
                printf("[0x0000]\tnop\n");
            } else if ((instruction >> 8) == 1) {
                // MOVW
                const u16 r1 = instruction & 0xF;
                const u16 r2 = (instruction & 0xF0) >> 4;
                printf("[0x%04x]\tmovw r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction >> 8) == 2) {
                // MULS
                const u16 r1 = instruction & 0xF;
                const u16 r2 = (instruction & 0xF0) >> 4;
                printf("[0x%04x]\tmuls r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction & 0xFF88) == 0x0300) {
                // MULSU
                const u16 r1 = instruction & 0x7;
                const u16 r2 = (instruction & 0x70) >> 4;
                printf("[0x%04x]\tmulsu r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction & 0xFF88) == 0x0308) {
                // FMUL
                const u16 r1 = instruction & 0x7;
                const u16 r2 = (instruction & 0x70) >> 4;
                printf("[0x%04x]\tfmul r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction & 0xFF88) == 0x0380) {
                // FMULS
                const u16 r1 = instruction & 0x7;
                const u16 r2 = (instruction & 0x70) >> 4;
                printf("[0x%04x]\tfmuls r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction & 0xFF88) == 0x0388) {
                // FMULSU
                const u16 r1 = instruction & 0x7;
                const u16 r2 = (instruction & 0x70) >> 4;
                printf("[0x%04x]\tfmulsu r%d, r%d\n", instruction, r2+16, r1+16);
            } else if ((instruction >> 14) == 0) {
                // 2-op instruction
                const u16 r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
                const u16 r2 = (instruction & 0x1F0) >> 4;
                const u8 op = (instruction & 0x3C00) >> 10;
                const char * const opnames[] = {"", "cpc", "sbc", "add", "cpse", "cp", "sub", "adc", "and", "eor", "or", "mov"};
                printf("[0x%04x]\t%s r%d, r%d\n", instruction, opnames[op], r1, r2);
            } else if ((instruction & 0xF000) == 0x3000) {
                // CPI
                const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
                const u16 reg = (instruction >> 4) & 0xF;
                printf("[0x%04x]\tcpi r%d, 0x%x\n", instruction, reg+16, value);
            } else if ((instruction >> 14) == 0x1) {
                // register-immediate instructions
                const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
                const u16 reg = (instruction >> 4) & 0xF;
                const u16 op = (instruction & 0x3000) >> 12;
                const char * const opnames[] = {"sbci", "subi", "ori", "andi"};
                printf("[0x%04x]\t%s r%d, 0x%x\n", instruction, opnames[op], reg+16, value);
            } else if ((instruction >> 10) == 0x24) {
                // load/store instructions
                const u16 reg = (instruction & 0x01F0) >> 4;
                const u8 op = (instruction & 0xF) | ((instruction & 0x200) >> 5);
                const char * const opnames[] = {
                    "lds", "ld", "ld", "", "lpm",    "lpm",    "elpm",    "elpm",
                    "",    "ld", "ld", "", "ld",     "ld",     "ld",      "pop",
                    "sts", "st", "st", "", "xch z,", "las z,", "lac z,", "lat z,",
                    "",    "st", "st", "", "st",     "st",     "st",     "push"
                };
                const char * const after[] = {
                    "", ", y+", ", -y", "", ", z", ", z+", ", z",  ", z+",
                    "", ", z+", ", -z", "", ", x", ", x+", ", -x", "",
                    "", ", y+", ", -y", "", "",    "",     "",     "",
                    "", ", z+", ", -z", "", ", x", ", x+", ", -x", ""
                };
                printf("[0x%04x]\t%s r%d%s\n", instruction, opnames[op], reg, after[op]);
            } else if ((instruction & 0xD000) == 0x8000) {
                // LDD/STD
                const u16 reg = (instruction & 0x01F0) >> 4;
                const u16 value = (instruction & 0x7) | ((instruction >> 7) & 0x0018) | ((instruction >> 7) & 0x0020);
                const u8 y = (instruction & 0x8) >> 3;
                const u8 s = (instruction & 0x0200) >> 9;
                const char * const opnames[] = {"ldd", "std"};
                const char * const after[] = {", z", ", y"};
                printf("[0x%04x]\t%s r%d%s+%d\n", instruction, opnames[s], reg, after[y], value);
            } else if ((instruction & 0xFE08) == 0x9400) {
                // 1-op instructions
                const u16 reg = (instruction & 0x01F0) >> 4;
                const u8 op = instruction & 0x7;
                const char * const opnames[] = {"com", "neg", "swap", "inc", "", "asr", "lsr", "ror"};
                if (op == 4) {
                    error("opcode 0x9408 doesn't exist ");
                }
                printf("[0x%04x]\t%s r%d\n", instruction, opnames[op], reg);
            } else if ((instruction & 0xFF0F) == 0x9408) {
                // SEx/CLx
                const u16 bit = (instruction & 0x0070) >> 4;
                const u8 op = (instruction & 0x0080) >> 7;
                const char * const bitnames = "cznvshti";
                const char * const opnames[] = {"se", "cl"};
                printf("[0x%04x]\t%s%c\n", instruction, opnames[op], bitnames[bit]);
            } else if ((instruction & 0xFF0F) == 0x9508) {
                // 0-op instructions
                const u8 op = (instruction & 0xF0) >> 4;
                const char * const opnames[] = {
                    "ret", "reti", "", "", "", "", "", "",
                    "sleep", "break", "wdr", "", "lpm", "elpm", "spm", "spm z+"};
                if (opnames[op] == "") {
                    error("0-op opcode doesn't exist");
                }
                printf("[0x%04x]\t%s\n", instruction, opnames[op]);
            } else if ((instruction & 0xFEEF) == 0x9409) {
                // Indirect jump/call to Z or EIND:Z
                const u8 op = ((instruction & 0x0100) >> 7) | (instruction & 0x0010) >> 4;
                const char * const opnames[] = {"ijmp", "eijmp", "icall", "eicall"};
                printf("[0x%04x]\t%s\n", instruction, opnames[op]);
            } else if ((instruction & 0xFE0F) == 0x940A) {
                // DEC
                const u16 reg = (instruction & 0x01F0) >> 4;
                printf("[0x%04x]\tdec r%d\n", instruction, reg);
            } else if ((instruction & 0xFF0F) == 0x940B) {
                // DES
                const u16 value = (instruction & 0x00F0) >> 4;
                printf("[0x%04x]\tdes 0x%x\n", instruction, value);
            } else if ((instruction & 0xFE00) == 0x9600) {
                // ADIW/SBIW
                const u8 value = (instruction & 0x000F) | ((instruction & 0x00C0) >> 2);
                const u8 reg = (instruction & 0x0030) >> 4;
                const u8 op = (instruction & 0x0100) >> 8;
                const char * const opnames[] = {"adiw", "sbiw"};
                printf("[0x%04x]\t%s r%d, 0x%x\t", instruction, opnames[op], (reg+12)*2, value);
            } else if ((instruction & 0xFC00) == 0x9800) {
                // CBI/SBI/SBIC/SBIS
                const u8 value = (instruction & 0x00F8) >> 3;
                const u8 bit = instruction & 0x7;
                const u8 op = (instruction & 0x0300) >> 8;
                const char * const opnames[] = {"cbi", "sbic", "sbi", "sbis"};
                printf("[0x%04x]\t%s 0x%x, 0x%x\n", instruction, opnames[op], value, bit);
            } else if ((instruction & 0xFC00) == 0x9C00) {
                // MUL, unsigned: R1:R0 = Rr × Rd
                const u16 r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
                const u16 r2 = (instruction & 0x1F0) >> 4;
                printf("[0x%04x]\tmul r%d, r%d\n", instruction, r2, r1);
            } else if ((instruction & 0xF000) == 0xB000) {
                // IN/OUT to I/O space
                const u16 address = (instruction & 0xF) | ((instruction & 0x600) >> 5);
                const u16 reg = (instruction & 0x1F0) >> 4;
                if (instruction & 0x0800) {
                    printf("[0x%04x]\tout 0x%x, r%d\n", instruction, address, reg);
                } else {
                    printf("[0x%04x]\tin r%d, 0x%x\n", instruction, reg, address);
                }
            } else if ((instruction & 0xE000) == 0xC000) {
                // RJMP/RCALL
                s16 offset = instruction & 0x0FFF;
                if (offset & 0x0800) {
                    offset -= 1 << 12;
                }
                if (instruction & 0x1000) {
                    printf("[0x%04x]\trcall %d\n", instruction, offset*2);
                } else {
                    printf("[0x%04x]\trjmp %d\n", instruction, offset*2); // NOTE(mdizdar): the offset may be incorrect, figure it out
                }
            } else if ((instruction >> 12) == 0xE) {
                // LDI
                const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
                const u16 reg = (instruction >> 4) & 0xF;
                printf("[0x%04x]\tldi r%d, 0x%x\n", instruction, reg+16, value);
            } else if ((instruction & 0xF800) == 0xF000) {
                // breaks
                s8 offset = (instruction & 0x03F8) >> 3;
                if (offset & 0x40) {
                    offset -= 1 << 7;
                }
                const u8 op = (instruction & 0x7) | ((instruction & 0x0400) >> 7);
                const char * const opnames[] = {
                    "brcs", "breq", "brmi", "brvs", "brlt", "brhs", "brts", "brie",
                    "brcc", "brne", "brpl", "brvc", "brge", "brhc", "brtc", "brid",
                };
                printf("[0x%04x]\t%s %d\n", instruction, opnames[op], offset*2);
            } else if ((instruction & 0xFC08) == 0xF800) {
                //BLD/BST
                const u8 bit = (instruction & 0x7);
                const u16 reg = (instruction >> 4) & 0xF;
                if (instruction & 0x0200) {
                    printf("[0x%04x]\tbst r%d, %u\n", instruction, reg, bit);
                } else {
                    printf("[0x%04x]\tbld r%d, %u\n", instruction, reg, bit);
                }
            } else if ((instruction & 0xFC08) == 0xFC00) {
                // SBRC/SBRS
                const u8 bit = (instruction & 0x7);
                const u16 reg = (instruction >> 4) & 0xF;
                if (instruction & 0x0200) {
                    printf("[0x%04x]\tsbrs r%d, %u\n", instruction, reg, bit);
                } else {
                    printf("[0x%04x]\tsbrc r%d, %u\n", instruction, reg, bit);
                }
            } else {
                error("You have invented a new AVR instruction, gz");
            }
        }
    }
};

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