#ifndef AVR_H
#define AVR_H

#include "../utils/common.h"

typedef u16 AVR;

// https://en.wikipedia.org/wiki/Atmel_AVR_instruction_set#Instruction_encoding
// http://ww1.microchip.com/downloads/cn/DeviceDoc/AVR-Instruction-Set-Manual-DS40002198A.pdf
static inline u16 NOP() {
    return 0x0;
}

static inline u16 MOVW(u8 rd, u8 rr) {
    // values should be in registers 16-31
    if (rd >= (1 << 5)) {
        warning(0, "MOVW Rd value greater than 0xF");
    }
    if (rr >= (1 << 5)) {
        warning(0, "MOVW Rr value greater than 0xF");
    }
    return 0x0100 | ((rd & 0xF) << 4) | (rr & 0xF);
}

static inline u16 MULS(u8 rd, u8 rr) {
    // values should be in registers 16-31
    if (rd >= (1 << 5)) {
        warning(0, "MULS Rd value greater than 0xF");
    }
    if (rr >= (1 << 5)) {
        warning(0, "MULS Rr value greater than 0xF");
    }
    return 0x0200 | ((rd & 0xF) << 4) | (rr & 0xF);
}

static inline u16 MULSU(u8 rd, u8 rr) {
    // values should be in registers 16-23
    if (rd >= (1 << 4)) {
        warning(0, "MULSU Rd value greater than 0x7");
    }
    if (rr >= (1 << 4)) {
        warning(0, "MULSU Rr value greater than 0x7");
    }
    return 0x0300 | ((rd & 0x7) << 4) | (rr & 0x7);
}

static inline u16 FMUL(u8 rd, u8 rr) {
    // values should be in registers 16-23
    if (rd >= (1 << 4)) {
        warning(0, "FMUL Rd value greater than 0x7");
    }
    if (rr >= (1 << 4)) {
        warning(0, "FMUL Rr value greater than 0x7");
    }
    return 0x0308 | ((rd & 0x7) << 4) | (rr & 0x7);
}

static inline u16 FMULS(u8 rd, u8 rr) {
    // values should be in registers 16-23
    if (rd >= (1 << 4)) {
        warning(0, "FMULS Rd value greater than 0x7");
    }
    if (rr >= (1 << 4)) {
        warning(0, "FMULS Rr value greater than 0x7");
    }
    return 0x0380 | ((rd & 0x7) << 4) | (rr & 0x7);
}

static inline u16 FMULSU(u8 rd, u8 rr) {
    // values should be in registers 16-23
    if (rd >= (1 << 4)) {
        warning(0, "FMULSU Rd value greater than 0x7");
    }
    if (rr >= (1 << 4)) {
        warning(0, "FMULSU Rr value greater than 0x7");
    }
    return 0x0388 | ((rd & 0x7) << 4) | (rr & 0x7);
}

static inline u16 CPC(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "CPC Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "CPC Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x0400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 CP(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "CP Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "CP Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x1400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 SBC(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "SBC Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "SBC Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x0800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 SUB(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "SUB Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "SUB Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x1800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 ADD(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ADD Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "ADD Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x0C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 ADC(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ADC Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "ADC Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x1C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 LSL(u8 rd) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LSL Rd value greater than 0x1F");
    }
    rd &= 0x1F;
    u8 rr = rd;
    return 0x0C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 ROL(u8 rd) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ROL Rd value greater than 0x1F");
    }
    rd &= 0x1F;
    u8 rr = rd;
    return 0x1C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 CPSE(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ADC Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "ADC Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x1000 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 AND(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "AND Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "AND Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x2000 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 EOR(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "EOR Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "EOR Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x2400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 OR(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "OR Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "OR Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x2800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 MOV(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "MOV Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "MOV Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x2C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 CPI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "CPI Rd value greater than 0xF");
    }
    return 0x3000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 SBCI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "SBCI Rd value greater than 0xF");
    }
    return 0x4000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 SUBI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "SUBI Rd value greater than 0xF");
    }
    return 0x5000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 ORI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "ORI Rd value greater than 0xF");
    }
    return 0x6000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 SBR(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "SBR Rd value greater than 0xF");
    }
    return 0x6000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 ANDI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "ANDI Rd value greater than 0xF");
    }
    return 0x7000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 CBR(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "CBR Rd value greater than 0xF");
    }
    return 0x7000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 LDDz(u8 rd, u8 K) {
    // registers 0-31, K 0-64
    if (rd >= (1 << 6)) {
        warning(0, "LDDz Rd value greater than 0x1F");
    }
    if (K >= (1 << 7)) {
        warning(0, "LDDz K value greater than 0x3F");
    }
    return 0x8000 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
}

static inline u16 LDDy(u8 rd, u8 K) {
    // registers 0-31, K 0-64
    if (rd >= (1 << 6)) {
        warning(0, "LDDy Rd value greater than 0x1F");
    }
    if (K >= (1 << 7)) {
        warning(0, "LDDy K value greater than 0x3F");
    }
    return 0x8008 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
}

static inline u16 STDz(u8 rd, u8 K) {
    // registers 0-31, K 0-64
    if (rd >= (1 << 6)) {
        warning(0, "STDz Rd value greater than 0x1F");
    }
    if (K >= (1 << 7)) {
        warning(0, "STDz K value greater than 0x3F");
    }
    return 0x8200 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
}

static inline u16 STDy(u8 rd, u8 K) {
    // registers 0-31, K 0-64
    if (rd >= (1 << 6)) {
        warning(0, "STDy Rd value greater than 0x1F");
    }
    if (K >= (1 << 7)) {
        warning(0, "STDy K value greater than 0x3F");
    }
    return 0x8208 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
}

static inline u32 LDS(u8 rd, u16 address) {
    // registers 0-31, address 0-65535
    if (rd >= (1 << 6)) {
        warning(0, "LDS Rd value greater than 0x1F");
    }
    return (u32)0x90000000 | address | ((rd & 0x1F) << 20);
}

static inline u32 STS(u8 rd, u16 address) {
    // registers 0-31, address 0-65535
    if (rd >= (1 << 6)) {
        warning(0, "STS Rd value greater than 0x1F");
    }
    return (u32)0x92000000 | address | ((rd & 0x1F) << 20);
}

static inline u16 LDp(u8 rd, u8 y) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LD+ Rd value greater than 0x1F");
    }
    y = !!y;
    return 0x9001 | ((rd & 0x1F) << 4) | (y << 3);
}

static inline u16 STp(u8 rd, u8 y) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ST+ Rd value greater than 0x1F");
    }
    y = !!y;
    return 0x9201 | ((rd & 0x1F) << 4) | (y << 3);
}

static inline u16 LDm(u8 rd, u8 y) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LD- Rd value greater than 0x1F");
    }
    y = !!y;
    return 0x9001 | ((rd & 0x1F) << 4) | (y << 3);
}

static inline u16 STm(u8 rd, u8 y) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ST- Rd value greater than 0x1F");
    }
    y = !!y;
    return 0x9202 | ((rd & 0x1F) << 4) | (y << 3);
}

static inline u16 LPM(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LPM Rd value greater than 0x1F");
    }
    return 0x9004 | ((rd & 0x1F) << 4);
}

static inline u16 ELPM(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ELPM Rd value greater than 0x1F");
    }
    return 0x9006 | ((rd & 0x1F) << 4);
}

static inline u16 LPMp(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LPM+ Rd value greater than 0x1F");
    }
    return 0x9005 | ((rd & 0x1F) << 4);
}

static inline u16 ELPMp(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ELPM+ Rd value greater than 0x1F");
    }
    return 0x9007 | ((rd & 0x1F) << 4);
}

static inline u16 XCH(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "XCH Rd value greater than 0x1F");
    }
    return 0x9204 | ((rd & 0x1F) << 4);
}

static inline u16 LAS(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LAS Rd value greater than 0x1F");
    }
    return 0x9205 | ((rd & 0x1F) << 4);
}

static inline u16 LAC(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LAC Rd value greater than 0x1F");
    }
    return 0x9206 | ((rd & 0x1F) << 4);
}

static inline u16 LAT(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LAT Rd value greater than 0x1F");
    }
    return 0x9207 | ((rd & 0x1F) << 4);
}

static inline u16 LDx(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LDx Rd value greater than 0x1F");
    }
    return 0x900C | ((rd & 0x1F) << 4);
}

static inline u16 STx(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "STx Rd value greater than 0x1F");
    }
    return 0x920C | ((rd & 0x1F) << 4);
}

static inline u16 LDxp(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LDxp Rd value greater than 0x1F");
    }
    return 0x900D | ((rd & 0x1F) << 4);
}

static inline u16 STxp(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "STxp Rd value greater than 0x1F");
    }
    return 0x920D | ((rd & 0x1F) << 4);
}

static inline u16 LDxm(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LDxm Rd value greater than 0x1F");
    }
    return 0x900E | ((rd & 0x1F) << 4);
}

static inline u16 STxm(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "STxm Rd value greater than 0x1F");
    }
    return 0x920E | ((rd & 0x1F) << 4);
}

static inline u16 POP(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "POP Rd value greater than 0x1F");
    }
    return 0x900F | ((rd & 0x1F) << 4);
}

static inline u16 PUSH(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "PUSH Rd value greater than 0x1F");
    }
    return 0x920F | ((rd & 0x1F) << 4);
}

static inline u16 COM(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "COM Rd value greater than 0x1F");
    }
    return 0x9400 | ((rd & 0x1F) << 4);
}

static inline u16 NEG(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "NEG Rd value greater than 0x1F");
    }
    return 0x9401 | ((rd & 0x1F) << 4);
}

static inline u16 SWAP(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "SWAP Rd value greater than 0x1F");
    }
    return 0x9402 | ((rd & 0x1F) << 4);
}

static inline u16 INC(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "INC Rd value greater than 0x1F");
    }
    return 0x9403 | ((rd & 0x1F) << 4);
}

static inline u16 ASR(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "COM Rd value greater than 0x1F");
    }
    return 0x9405 | ((rd & 0x1F) << 4);
}

static inline u16 LSR(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "LSR Rd value greater than 0x1F");
    }
    return 0x9406 | ((rd & 0x1F) << 4);
}

static inline u16 ROR(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "ROR Rd value greater than 0x1F");
    }
    return 0x9407 | ((rd & 0x1F) << 4);
}

static inline u16 SER(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "SE Rd value greater than 0x1F");
    }
    return 0xCF0F | ((rd & 0xF) << 4);
}

static inline u16 CLR(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "CLR Rd value greater than 0x1F");
    }
    return 0x2400 | (rd & 0xF) | ((rd & 0xF) << 4);
}

static inline u16 SEC() {
    return 0x9408;
}

static inline u16 SEH() {
    return 0x9458;
}

static inline u16 SEI() {
    return 0x9478;
}

static inline u16 SEN() {
    return 0x9428;
}

static inline u16 SES() {
    return 0x9448;
}

static inline u16 SET() {
    return 0x9468;
}

static inline u16 SEV() {
    return 0x9438;
}

static inline u16 SEZ() {
    return 0x948;
}

static inline u16 CLC() {
    return 0x9488;
}

static inline u16 CLH() {
    return 0x94D8;
}

static inline u16 CLI() {
    return 0x94F8;
}

static inline u16 CLN() {
    return 0x94A8;
}

static inline u16 CLS() {
    return 0x94C8;
}

static inline u16 CLT() {
    return 0x94E8;
}

static inline u16 CLV() {
    return 0x94B8;
}

static inline u16 CLZ() {
    return 0x9498;
}

static inline u16 RET() {
    return 0x9508;
}

static inline u16 RETI() {
    return 0x9518;
}

static inline u16 SLEEP() {
    return 0x9588;
}

static inline u16 BREAK() {
    return 0x9598;
}

static inline u16 WDR() {
    return 0x95A8;
}

static inline u16 SPM() {
    return 0x95E8;
}

static inline u16 SPMzp() {
    return 0x95F8;
}

static inline u16 IJMP() {
    return 0x9409;
}

static inline u16 EIJMP() {
    return 0x9419;
}

static inline u16 ICALL() {
    return 0x9509;
}

static inline u16 EICALL() {
    return 0x9519;
}

static inline u16 DEC(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "DEC Rd value greater than 0x1F");
    }
    return 0x940A | ((rd & 0x1F) << 4);
}

static inline u16 DES(u8 K) {
    // registers 0-31
    if (K >= (1 << 5)) {
        warning(0, "DES K value greater than 0xF");
    }
    return 0x940B | ((K & 0xF) << 4);
}

static inline u32 JMP(u32 address) {
    // address 0-4194303
    if (address >= (1 << 23)) {
        warning(0, "JMP address value greater than 0x3FFFFF");
    }
    return (u32)0x940C0000 | (address & 0x1FFFF) | ((address & 0x3E0000) << 3);
}

static inline u32 CALL(u32 address) {
    // address 0-4194303
    if (address >= (1 << 23)) {
        warning(0, "CALL address value greater than 0x3FFFFF");
    }
    return (u32)0x940E0000 | (address & 0x1FFFF) | ((address & 0x3E0000) << 3);
}

static inline u16 ADIW(u8 rp, u8 K) {
    // pp = Register pair, W, X, Y or Z
    // K 0-63
    if (rp >= (1 << 3)) {
        warning(0, "ADIW Rp value greater than 0x3");
    }
    if (K >= (1 << 7)) {
        warning(0, "ADIW K value greater than 0x3F");
    }
    return 0x960B | (K & 0xF) | ((rp & 0x3) << 4) | ((K & 0x30) << 2);
}

static inline u16 SBIW(u8 rp, u8 K) {
    // pp = Register pair, W, X, Y or Z
    // K 0-63
    if (rp >= (1 << 3)) {
        warning(0, "SBIW Rp value greater than 0x3");
    }
    if (K >= (1 << 7)) {
        warning(0, "SBIW K value greater than 0x3F");
    }
    return 0x970B | (K & 0xF) | ((rp & 0x3) << 4) | ((K & 0x30) << 2);
}

static inline u16 CBI(u8 A, u8 B) {
    // A 0-63, B 0-8
    if (A >= (1 << 6)) {
        warning(0, "CBI A value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "CBI B value greater than 0x7");
    }
    return 0x9800 | (B & 0x7) | ((A & 0x1F) << 3);
}

static inline u16 SBI(u8 A, u8 B) {
    // A 0-63, B 0-8
    if (A >= (1 << 6)) {
        warning(0, "SBI A value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "SBI B value greater than 0x7");
    }
    return 0x9A00 | (B & 0x7) | ((A & 0x1F) << 3);
}

static inline u16 SBIC(u8 A, u8 B) {
    // A 0-63, B 0-8
    if (A >= (1 << 6)) {
        warning(0, "SBIC A value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "SBIC B value greater than 0x7");
    }
    return 0x9900 | (B & 0x7) | ((A & 0x1F) << 3);
}

static inline u16 SBIS(u8 A, u8 B) {
    // A 0-63, B 0-8
    if (A >= (1 << 6)) {
        warning(0, "SBIS A value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "SBIS B value greater than 0x7");
    }
    return 0x9C00 | (B & 0x7) | ((A & 0x1F) << 3);
}

static inline u16 MUL(u8 rd, u8 rr) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "MUL Rd value greater than 0x1F");
    }
    if (rr >= (1 << 6)) {
        warning(0, "MUL Rr value greater than 0x1F");
    }
    rd &= 0x1F;
    rr &= 0x1F;
    return 0x9C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
}

static inline u16 IN(u8 rd, u8 a) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "IN Rd value greater than 0x1F");
    }
    if (a >= (1 << 6)) {
        warning(0, "IN A value greater than 0x3F");
    }
    rd &= 0x1F;
    a &= 0x3F;
    return 0xB000 | (rd << 4) | (a & 0xF) | ((a & 0x30) << 5);
}

static inline u16 OUT(u8 rd, u8 a) {
    // values should be in registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "OUT Rd value greater than 0x1F");
    }
    if (a >= (1 << 6)) {
        warning(0, "OUT A value greater than 0x3F");
    }
    rd &= 0x1F;
    a &= 0x3F;
    return 0xB800 | (rd << 4) | (a & 0xF) | ((a & 0x30) << 5);
}

static inline u16 RJMP(u16 offset) {
    // offset is a signed 12 bit integer, we'll treat it as an uint for ease
    // please prepare it beforehand uwu
    if (offset >= (1 << 13)) {
        warning(0, "RJMP offset value greater than 0xFFF");
    }
    return 0xC000 | (offset & 0x0FFF);
}

static inline u16 RCALL(u16 offset) {
    // offset is a signed 12 bit integer, we'll treat it as an uint for ease
    // please prepare it beforehand uwu
    if (offset >= (1 << 13)) {
        warning(0, "RCALL offset value greater than 0xFFF");
    }
    return 0xD000 | (offset & 0x0FFF);
}

static inline u16 LDI(u8 rd, u8 K) {
    // registers 16-31, K 0-255
    if (rd >= (1 << 5)) {
        warning(0, "LDI Rd value greater than 0xF");
    }
    return 0xE000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
}

static inline u16 BRBC(u8 K, u8 B) {
    // K 0-127 (7 bit signed int, treated as uint), B 0-8
    if (K >= (1 << 8)) {
        warning(0, "BRBC K value greater than 0x7F");
    }
    if (B >= (1 << 4)) {
        warning(0, "BRBC B value greater than 0x7");
    }
    return 0xF400 | ((K & 0x7F) << 3) | (B & 0x7);
}

static inline u16 BRBS(u8 K, u8 B) {
    // K 0-127 (7 bit signed int, treated as uint), B 0-8
    if (K >= (1 << 8)) {
        warning(0, "BRBS K value greater than 0x7F");
    }
    if (B >= (1 << 4)) {
        warning(0, "BRBS B value greater than 0x7");
    }
    return 0xF000 | ((K & 0x7F) << 3) | (B & 0x7);
}

static inline u16 BRCC(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRCC K value greater than 0x7F");
    }
    return 0xF400 | ((K & 0x7F) << 3);
}

static inline u16 BRCS(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRCS K value greater than 0x7F");
    }
    return 0xF000 | ((K & 0x7F) << 3);
}

static inline u16 BREQ(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BREQ K value greater than 0x7F");
    }
    return 0xF001 | ((K & 0x7F) << 3);
}

static inline u16 BRGE(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRGE K value greater than 0x7F");
    }
    return 0xF404 | ((K & 0x7F) << 3);
}

static inline u16 BRHC(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRHC K value greater than 0x7F");
    }
    return 0xF405 | ((K & 0x7F) << 3);
}

static inline u16 BRHS(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRHS K value greater than 0x7F");
    }
    return 0xF005 | ((K & 0x7F) << 3);
}

static inline u16 BRID(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRID K value greater than 0x7F");
    }
    return 0xF407 | ((K & 0x7F) << 3);
}

static inline u16 BRIE(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRIE K value greater than 0x7F");
    }
    return 0xF007 | ((K & 0x7F) << 3);
}

static inline u16 BRLO(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRLO K value greater than 0x7F");
    }
    return 0xF000 | ((K & 0x7F) << 3);
}

static inline u16 BRLT(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRLT K value greater than 0x7F");
    }
    return 0xF004 | ((K & 0x7F) << 3);
}

static inline u16 BRMI(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRMI K value greater than 0x7F");
    }
    return 0xF002 | ((K & 0x7F) << 3);
}

static inline u16 BRNE(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRNE K value greater than 0x7F");
    }
    return 0xF401 | ((K & 0x7F) << 3);
}

static inline u16 BRPL(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRPL K value greater than 0x7F");
    }
    return 0xF402 | ((K & 0x7F) << 3);
}

static inline u16 BRSH(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRSH K value greater than 0x7F");
    }
    return 0xF400 | ((K & 0x7F) << 3);
}

static inline u16 BRTC(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRTC K value greater than 0x7F");
    }
    return 0xF406 | ((K & 0x7F) << 3);
}

static inline u16 BRTS(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRTS K value greater than 0x7F");
    }
    return 0xF006 | ((K & 0x7F) << 3);
}

static inline u16 BRVC(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRVC K value greater than 0x7F");
    }
    return 0xF403 | ((K & 0x7F) << 3);
}

static inline u16 BRVS(u8 K) {
    // K 0-127 (7 bit signed int, treated as uint)
    if (K >= (1 << 8)) {
        warning(0, "BRVS K value greater than 0x7F");
    }
    return 0xF003 | ((K & 0x7F) << 3);
}

static inline u16 BSET(u8 S) {
    // S 0-7 
    if (S >= (1 << 8)) {
        warning(0, "BSET S value greater than 0x7");
    }
    return 0x9408 | ((S & 0x7) << 4);
}

static inline u16 BCLR(u8 S) {
    // S 0-7 
    if (S >= (1 << 8)) {
        warning(0, "BCLR S value greater than 0x7");
    }
    return 0x9488 | ((S & 0x7) << 4);
}

static inline u16 BLD(u8 rd, u8 B) {
    // registers 16-31, B 0-8
    if (rd >= (1 << 6)) {
        warning(0, "BLD Rd value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "BLD B value greater than 0x7");
    }
    return 0xF800 | ((rd & 0x1F) << 4) | (B & 0x7);
}

static inline u16 BST(u8 rd, u8 B) {
    // registers 16-31, B 0-8
    if (rd >= (1 << 6)) {
        warning(0, "BST Rd value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "BST B value greater than 0x7");
    }
    return 0xFA00 | ((rd & 0x1F) << 4) | (B & 0x7);
}

static inline u16 SBRC(u8 rd, u8 B) {
    // registers 16-31, B 0-8
    if (rd >= (1 << 6)) {
        warning(0, "SBRC Rd value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "SBRC B value greater than 0x7");
    }
    return 0xFE00 | ((rd & 0x1F) << 4) | (B & 0x7);
}

static inline u16 SBRS(u8 rd, u8 B) {
    // registers 16-31, B 0-8
    if (rd >= (1 << 6)) {
        warning(0, "SBRS Rd value greater than 0x1F");
    }
    if (B >= (1 << 4)) {
        warning(0, "SBRS B value greater than 0x7");
    }
    return 0xFF00 | ((rd & 0x1F) << 4) | (B & 0x7);
}

static inline u16 TST(u8 rd) {
    // registers 0-31
    if (rd >= (1 << 6)) {
        warning(0, "SBRS Rd value greater than 0x1F");
    }
    return 0x2000 | ((rd & 0x1F) << 5) | (rd & 0x1F);
}

void printAVR(const DynArray *instructions) {
    // NOTE(mdizdar): I wonder if there's a nicer way of going about this... (there is)
    AVR *ins = instructions->data;
    for (u64 i = 0; i < instructions->count; ++i) {
        const AVR instruction = ins[i];
        if ((instruction & 0xFC0F) == 0x9000) {
            // LDS/STS - 32-bit instruction
            const u16 SRAMaddress = ins[++i];
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
            const u16 hi = ins[++i];
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
        } else if ((instruction & 0xF000) == 0x3000) {
            // CPI
            const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
            const u16 reg = (instruction >> 4) & 0xF;
            printf("[0x%04x]\tcpi r%d, 0x%x\n", instruction, reg+16, value);
        } else if ((instruction >> 14) == 0) {
            // 2-op instruction
            const u16 r2 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
            const u16 r1 = (instruction & 0x1F0) >> 4;
            const u8 op = (instruction & 0x3C00) >> 10;
            const char * opnames[12];
            opnames[0] = ""; opnames[1] = "cpc"; opnames[2] = "sbc"; opnames[3] = "add";
            opnames[4] = "cpse"; opnames[5] = "cp"; opnames[6] = "sub"; opnames[7] = "adc";
            opnames[8] = "and"; opnames[9] = "eor"; opnames[10] = "or"; opnames[11] = "mov";
            printf("[0x%04x]\t%s r%d, r%d\n", instruction, opnames[op], r1, r2);
        } else if ((instruction >> 14) == 0x1) {
            // register-immediate instructions
            const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
            const u16 reg = (instruction >> 4) & 0xF;
            const u16 op = (instruction & 0x3000) >> 12;
            const char * opnames[4];
            opnames[0] = "sbci"; opnames[1] = "subi"; opnames[2] = "ori"; opnames[3] = "andi";
            printf("[0x%04x]\t%s r%d, 0x%x\n", instruction, opnames[op], reg+16, value);
        } else if ((instruction >> 10) == 0x24) {
            // load/store instructions
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u8 op = (u8)((instruction & 0xF) | ((instruction & 0x200) >> 5));
            const char * opcodes[32];
            const char * after[32];
            opcodes[0] = "lds"; opcodes[1] = "ld"; opcodes[2] = "ld"; opcodes[3] = "";
            opcodes[4] = "lpm"; opcodes[5] = "lpm"; opcodes[6] = "elpm"; opcodes[7] = "elpm";
            opcodes[8] = ""; opcodes[9] = "ld"; opcodes[10] = "ld"; opcodes[11] = "";
            opcodes[12] = "ld"; opcodes[13] = "ld"; opcodes[14] = "ld"; opcodes[15] = "pop";
            opcodes[16] = "sts"; opcodes[17] = "st"; opcodes[18] = "st"; opcodes[19] = "";
            opcodes[20] = "xch z,"; opcodes[21] = "las z,"; opcodes[22] = "lac z,";
            opcodes[23] = "lat z,"; opcodes[24] = ""; opcodes[25] = "st"; opcodes[26] = "st";
            opcodes[27] = ""; opcodes[28] = "st"; opcodes[29] = "st"; opcodes[30] = "st";
            opcodes[31] = "push";
            after[0] = ""; after[1] = ", y+"; after[2] = ", -y"; after[3] = "";
            after[4] = ", z"; after[5] = ", z+"; after[6] = ", z"; after[7] = ", z+";
            after[8] = ""; after[9] = ", z+"; after[10] = ", -z"; after[11] = "";
            after[12] = ", x"; after[13] = ", x+"; after[14] = ", -x"; after[15] = "";
            after[16] = ""; after[17] = ", y+"; after[18] = ", -y"; after[19] = "";
            after[20] = ""; after[21] = ""; after[22] = ""; after[23] = ""; after[24] = "";
            after[25] = ", z+"; after[26] = ", -z"; after[27] = ""; after[28] = ", x";
            after[29] = ", x+"; after[30] = ", -x"; after[31] = "";
            printf("[0x%04x]\t%s r%d%s\n", instruction, opcodes[op], reg, after[op]);
        } else if ((instruction & 0xD000) == 0x8000) {
            // LDD/STD
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u16 value = (instruction & 0x7) | ((instruction >> 7) & 0x0018) | ((instruction >> 7) & 0x0020);
            const u8 y = (instruction & 0x8) >> 3;
            const u8 s = (instruction & 0x0200) >> 9;
            const char * opnames[2];
            opnames[0] = "ldd";
            opnames[1] = "std";
            const char * after[2];
            after[0] = ", z";
            after[1] = ", y";
            printf("[0x%04x]\t%s r%d%s+%d\n", instruction, opnames[s], reg, after[y], value);
        } else if ((instruction & 0xFE08) == 0x9400) {
            // 1-op instructions
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u8 op = instruction & 0x7;
            const char * opnames[8];
            opnames[0] = "com"; opnames[1] = "neg"; opnames[2] = "swap"; opnames[3] = "inc";
            opnames[4] = ""; opnames[5] = "asr"; opnames[6] = "lsr"; opnames[7] = "ror";
            if (op == 4) {
                error(0, "opcode 0x9408 doesn't exist ");
            }
            printf("[0x%04x]\t%s r%d\n", instruction, opnames[op], reg);
        } else if ((instruction & 0xFF0F) == 0x9408) {
            // SEx/CLx
            const u16 bit = (instruction & 0x0070) >> 4;
            const u8 op = (instruction & 0x0080) >> 7;
            const char * bitnames = "cznvshti";
            const char * opnames[] = {"se", "cl"};
            printf("[0x%04x]\t%s%c\n", instruction, opnames[op], bitnames[bit]);
        } else if ((instruction & 0xFF0F) == 0x9508) {
            // 0-op instructions
            const u8 op = (instruction & 0xF0) >> 4;
            const char * opnames[16];
            opnames[0] = "ret"; opnames[1] = "reti"; opnames[2] = ""; opnames[3] = "";
            opnames[4] = ""; opnames[5] = ""; opnames[6] = ""; opnames[7] = "";
            opnames[8] = "sleep"; opnames[9] = "break"; opnames[10] = "wdr"; opnames[11] = "";
            opnames[12] = "lpm"; opnames[13] = "elpm"; opnames[14] = "spm"; opnames[15] = "spm z+";
            if (!strcmp(opnames[op], "")) {
                error(0, "0-op opcode doesn't exist");
            }
            printf("[0x%04x]\t%s\n", instruction, opnames[op]);
        } else if ((instruction & 0xFEEF) == 0x9409) {
            // Indirect jump/call to Z or EIND:Z
            const u8 op = (u8)(((instruction & 0x0100) >> 7) | (instruction & 0x0010) >> 4);
            const char * opnames[4];
            opnames[0] = "ijmp";
            opnames[1] = "eijmp";
            opnames[2] = "icall";
            opnames[3] = "eicall";
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
            const char * opnames[2];
            opnames[0] = "adiw";
            opnames[1] = "sbiw";
            printf("[0x%04x]\t%s r%d, 0x%x\t", instruction, opnames[op], (reg+12)*2, value);
        } else if ((instruction & 0xFC00) == 0x9800) {
            // CBI/SBI/SBIC/SBIS
            const u8 value = (instruction & 0x00F8) >> 3;
            const u8 bit = instruction & 0x7;
            const u8 op = (instruction & 0x0300) >> 8;
            const char * opnames[4];
            opnames[0] = "cbi";
            opnames[1] = "sbic";
            opnames[2] = "sbi";
            opnames[3] = "sbis";
            printf("[0x%04x]\t%s 0x%x, 0x%x\n", instruction, opnames[op], value, bit);
        } else if ((instruction & 0xFC00) == 0x9C00) {
            // MUL, unsigned: R1:R0 = Rr x Rd
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
            s8 offset = (s8)((instruction & 0x03F8) >> 3);
            if (offset & 0x40) {
                offset -= 1 << 7;
            }
            const u8 op = (u8)((instruction & 0x7) | ((instruction & 0x0400) >> 7));
            const char * opnames[16];
            opnames[0] = "brcs"; opnames[1] = "breq"; opnames[2] = "brmi"; opnames[3] = "brvs";
            opnames[4] = "brlt"; opnames[5] = "brhs"; opnames[6] = "brts"; opnames[7] = "brie";
            opnames[8] = "brcc"; opnames[9] = "brne"; opnames[10] = "brpl";
            opnames[11] = "brvc"; opnames[12] = "brge"; opnames[13] = "brhc"; 
            opnames[14] = "brtc"; opnames[15] = "brid";
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
            error(0, "You have invented a new AVR instruction, gz");
        }
    }
}

void saveAVR(const DynArray *instructions, char *outfile) {
    u64 len = strlen(outfile);
    char *of = malloc(sizeof(char) * len+5);
    strcpy(of, outfile);
    strcat(of, ".asm");
    FILE *fp = fopen(of, "w");
    AVR *ins = instructions->data;
    for (u64 i = 0; i < instructions->count; ++i) {
        fprintf(fp, "%4lx:\t", i*2);
        const AVR instruction = ins[i];
        if ((instruction & 0xFC0F) == 0x9000) {
            // LDS/STS - 32-bit instruction
            const u16 SRAMaddress = ins[++i];
            const u16 reg = instruction & 0x003F;
            if (instruction & 0x0200) {
                // STS
                fprintf(fp, "[0x%04x%04x]\tsts r%d, 0x%x\n", instruction, SRAMaddress, reg, SRAMaddress << 1);
            } else {
                // LDS
                fprintf(fp, "[0x%04x%04x]\tlds 0x%x, r%d\n", instruction, SRAMaddress, SRAMaddress << 1, reg);
            }
            continue;
        }
        if ((instruction & 0xFE0C) == 0x940C) {
            // JMP/CALL - 32-bit instruction
            const u16 hi = ins[++i];
            const u32 SRAMaddress = hi | ((instruction & 0x01F0) << 14) | ((instruction & 0x0001) << 16);
            if (instruction & 0x0002) {
                // CALL
                fprintf(fp, "[0x%04x%04x]\tcall 0x%x\n", instruction, hi, SRAMaddress << 1);
            } else {
                // JMP
                fprintf(fp, "[0x%04x%04x]\tjmp 0x%x\n", instruction, hi, SRAMaddress << 1);
            }
            continue;
        }
        
        // from here on it's only 16 bit instructions
        
        if (instruction == 0) {
            // NOP
            fprintf(fp, "[0x0000]\tnop\n");
        } else if ((instruction >> 8) == 1) {
            // MOVW
            const u16 r1 = instruction & 0xF;
            const u16 r2 = (instruction & 0xF0) >> 4;
            fprintf(fp, "[0x%04x]\tmovw r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction >> 8) == 2) {
            // MULS
            const u16 r1 = instruction & 0xF;
            const u16 r2 = (instruction & 0xF0) >> 4;
            fprintf(fp, "[0x%04x]\tmuls r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction & 0xFF88) == 0x0300) {
            // MULSU
            const u16 r1 = instruction & 0x7;
            const u16 r2 = (instruction & 0x70) >> 4;
            fprintf(fp, "[0x%04x]\tmulsu r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction & 0xFF88) == 0x0308) {
            // FMUL
            const u16 r1 = instruction & 0x7;
            const u16 r2 = (instruction & 0x70) >> 4;
            fprintf(fp, "[0x%04x]\tfmul r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction & 0xFF88) == 0x0380) {
            // FMULS
            const u16 r1 = instruction & 0x7;
            const u16 r2 = (instruction & 0x70) >> 4;
            fprintf(fp, "[0x%04x]\tfmuls r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction & 0xFF88) == 0x0388) {
            // FMULSU
            const u16 r1 = instruction & 0x7;
            const u16 r2 = (instruction & 0x70) >> 4;
            fprintf(fp, "[0x%04x]\tfmulsu r%d, r%d\n", instruction, r2+16, r1+16);
        } else if ((instruction & 0xF000) == 0x3000) {
            // CPI
            const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
            const u16 reg = (instruction >> 4) & 0xF;
            fprintf(fp, "[0x%04x]\tcpi r%d, 0x%x\n", instruction, reg+16, value);
        } else if ((instruction >> 14) == 0) {
            // 2-op instruction
            const u16 r2 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
            const u16 r1 = (instruction & 0x1F0) >> 4;
            const u8 op = (instruction & 0x3C00) >> 10;
            const char * opnames[12];
            opnames[0] = ""; opnames[1] = "cpc"; opnames[2] = "sbc"; opnames[3] = "add";
            opnames[4] = "cpse"; opnames[5] = "cp"; opnames[6] = "sub"; opnames[7] = "adc";
            opnames[8] = "and"; opnames[9] = "eor"; opnames[10] = "or"; opnames[11] = "mov";
            fprintf(fp, "[0x%04x]\t%s r%d, r%d\n", instruction, opnames[op], r1, r2);
        } else if ((instruction >> 14) == 0x1) {
            // register-immediate instructions
            const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
            const u16 reg = (instruction >> 4) & 0xF;
            const u16 op = (instruction & 0x3000) >> 12;
            const char * opnames[4];
            opnames[0] = "sbci"; opnames[1] = "subi"; opnames[2] = "ori"; opnames[3] = "andi";
            fprintf(fp, "[0x%04x]\t%s r%d, 0x%x\n", instruction, opnames[op], reg+16, value);
        } else if ((instruction >> 10) == 0x24) {
            // load/store instructions
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u8 op = (u8)((instruction & 0xF) | ((instruction & 0x200) >> 5));
            const char * opcodes[32];
            const char * after[32];
            opcodes[0] = "lds"; opcodes[1] = "ld"; opcodes[2] = "ld"; opcodes[3] = "";
            opcodes[4] = "lpm"; opcodes[5] = "lpm"; opcodes[6] = "elpm"; opcodes[7] = "elpm";
            opcodes[8] = ""; opcodes[9] = "ld"; opcodes[10] = "ld"; opcodes[11] = "";
            opcodes[12] = "ld"; opcodes[13] = "ld"; opcodes[14] = "ld"; opcodes[15] = "pop";
            opcodes[16] = "sts"; opcodes[17] = "st"; opcodes[18] = "st"; opcodes[19] = "";
            opcodes[20] = "xch z,"; opcodes[21] = "las z,"; opcodes[22] = "lac z,";
            opcodes[23] = "lat z,"; opcodes[24] = ""; opcodes[25] = "st"; opcodes[26] = "st";
            opcodes[27] = ""; opcodes[28] = "st"; opcodes[29] = "st"; opcodes[30] = "st";
            opcodes[31] = "push";
            after[0] = ""; after[1] = ", y+"; after[2] = ", -y"; after[3] = "";
            after[4] = ", z"; after[5] = ", z+"; after[6] = ", z"; after[7] = ", z+";
            after[8] = ""; after[9] = ", z+"; after[10] = ", -z"; after[11] = "";
            after[12] = ", x"; after[13] = ", x+"; after[14] = ", -x"; after[15] = "";
            after[16] = ""; after[17] = ", y+"; after[18] = ", -y"; after[19] = "";
            after[20] = ""; after[21] = ""; after[22] = ""; after[23] = ""; after[24] = "";
            after[25] = ", z+"; after[26] = ", -z"; after[27] = ""; after[28] = ", x";
            after[29] = ", x+"; after[30] = ", -x"; after[31] = "";
            fprintf(fp, "[0x%04x]\t%s r%d%s\n", instruction, opcodes[op], reg, after[op]);
        } else if ((instruction & 0xD000) == 0x8000) {
            // LDD/STD
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u16 value = (instruction & 0x7) | ((instruction >> 7) & 0x0018) | ((instruction >> 7) & 0x0020);
            const u8 y = (instruction & 0x8) >> 3;
            const u8 s = (instruction & 0x0200) >> 9;
            const char * opnames[2];
            opnames[0] = "ldd";
            opnames[1] = "std";
            const char * after[2];
            after[0] = ", z";
            after[1] = ", y";
            fprintf(fp, "[0x%04x]\t%s r%d%s+%d\n", instruction, opnames[s], reg, after[y], value);
        } else if ((instruction & 0xFE08) == 0x9400) {
            // 1-op instructions
            const u16 reg = (instruction & 0x01F0) >> 4;
            const u8 op = instruction & 0x7;
            const char * opnames[8];
            opnames[0] = "com"; opnames[1] = "neg"; opnames[2] = "swap"; opnames[3] = "inc";
            opnames[4] = ""; opnames[5] = "asr"; opnames[6] = "lsr"; opnames[7] = "ror";
            if (op == 4) {
                error(0, "opcode 0x9408 doesn't exist ");
            }
            fprintf(fp, "[0x%04x]\t%s r%d\n", instruction, opnames[op], reg);
        } else if ((instruction & 0xFF0F) == 0x9408) {
            // SEx/CLx
            const u16 bit = (instruction & 0x0070) >> 4;
            const u8 op = (instruction & 0x0080) >> 7;
            const char * bitnames = "cznvshti";
            const char * opnames[] = {"se", "cl"};
            fprintf(fp, "[0x%04x]\t%s%c\n", instruction, opnames[op], bitnames[bit]);
        } else if ((instruction & 0xFF0F) == 0x9508) {
            // 0-op instructions
            const u8 op = (instruction & 0xF0) >> 4;
            const char * opnames[16];
            opnames[0] = "ret"; opnames[1] = "reti"; opnames[2] = ""; opnames[3] = "";
            opnames[4] = ""; opnames[5] = ""; opnames[6] = ""; opnames[7] = "";
            opnames[8] = "sleep"; opnames[9] = "break"; opnames[10] = "wdr"; opnames[11] = "";
            opnames[12] = "lpm"; opnames[13] = "elpm"; opnames[14] = "spm"; opnames[15] = "spm z+";
            if (!strcmp(opnames[op], "")) {
                error(0, "0-op opcode doesn't exist");
            }
            fprintf(fp, "[0x%04x]\t%s\n", instruction, opnames[op]);
        } else if ((instruction & 0xFEEF) == 0x9409) {
            // Indirect jump/call to Z or EIND:Z
            const u8 op = (u8)(((instruction & 0x0100) >> 7) | (instruction & 0x0010) >> 4);
            const char * opnames[4];
            opnames[0] = "ijmp";
            opnames[1] = "eijmp";
            opnames[2] = "icall";
            opnames[3] = "eicall";
            fprintf(fp, "[0x%04x]\t%s\n", instruction, opnames[op]);
        } else if ((instruction & 0xFE0F) == 0x940A) {
            // DEC
            const u16 reg = (instruction & 0x01F0) >> 4;
            fprintf(fp, "[0x%04x]\tdec r%d\n", instruction, reg);
        } else if ((instruction & 0xFF0F) == 0x940B) {
            // DES
            const u16 value = (instruction & 0x00F0) >> 4;
            fprintf(fp, "[0x%04x]\tdes 0x%x\n", instruction, value);
        } else if ((instruction & 0xFE00) == 0x9600) {
            // ADIW/SBIW
            const u8 value = (instruction & 0x000F) | ((instruction & 0x00C0) >> 2);
            const u8 reg = (instruction & 0x0030) >> 4;
            const u8 op = (instruction & 0x0100) >> 8;
            const char * opnames[2];
            opnames[0] = "adiw";
            opnames[1] = "sbiw";
            fprintf(fp, "[0x%04x]\t%s r%d, 0x%x\t", instruction, opnames[op], (reg+12)*2, value);
        } else if ((instruction & 0xFC00) == 0x9800) {
            // CBI/SBI/SBIC/SBIS
            const u8 value = (instruction & 0x00F8) >> 3;
            const u8 bit = instruction & 0x7;
            const u8 op = (instruction & 0x0300) >> 8;
            const char * opnames[4];
            opnames[0] = "cbi";
            opnames[1] = "sbic";
            opnames[2] = "sbi";
            opnames[3] = "sbis";
            fprintf(fp, "[0x%04x]\t%s 0x%x, 0x%x\n", instruction, opnames[op], value, bit);
        } else if ((instruction & 0xFC00) == 0x9C00) {
            // MUL, unsigned: R1:R0 = Rr x Rd
            const u16 r1 = (instruction & 0xF) | ((instruction & 0x200) >> 5);
            const u16 r2 = (instruction & 0x1F0) >> 4;
            fprintf(fp, "[0x%04x]\tmul r%d, r%d\n", instruction, r2, r1);
        } else if ((instruction & 0xF000) == 0xB000) {
            // IN/OUT to I/O space
            const u16 address = (instruction & 0xF) | ((instruction & 0x600) >> 5);
            const u16 reg = (instruction & 0x1F0) >> 4;
            if (instruction & 0x0800) {
                fprintf(fp, "[0x%04x]\tout 0x%x, r%d\n", instruction, address, reg);
            } else {
                fprintf(fp, "[0x%04x]\tin r%d, 0x%x\n", instruction, reg, address);
            }
        } else if ((instruction & 0xE000) == 0xC000) {
            // RJMP/RCALL
            s16 offset = instruction & 0x0FFF;
            if (offset & 0x0800) {
                offset -= 1 << 12;
            }
            if (instruction & 0x1000) {
                fprintf(fp, "[0x%04x]\trcall %d\n", instruction, offset*2);
            } else {
                fprintf(fp, "[0x%04x]\trjmp %d\n", instruction, offset*2); // NOTE(mdizdar): the offset may be incorrect, figure it out
            }
        } else if ((instruction >> 12) == 0xE) {
            // LDI
            const u16 value = ((instruction >> 4) & 0xF0) + (instruction & 0xF);
            const u16 reg = (instruction >> 4) & 0xF;
            fprintf(fp, "[0x%04x]\tldi r%d, 0x%x\n", instruction, reg+16, value);
        } else if ((instruction & 0xF800) == 0xF000) {
            // breaks
            s8 offset = (s8)((instruction & 0x03F8) >> 3);
            if (offset & 0x40) {
                offset -= 1 << 7;
            }
            const u8 op = (u8)((instruction & 0x7) | ((instruction & 0x0400) >> 7));
            const char * opnames[16];
            opnames[0] = "brcs"; opnames[1] = "breq"; opnames[2] = "brmi"; opnames[3] = "brvs";
            opnames[4] = "brlt"; opnames[5] = "brhs"; opnames[6] = "brts"; opnames[7] = "brie";
            opnames[8] = "brcc"; opnames[9] = "brne"; opnames[10] = "brpl";
            opnames[11] = "brvc"; opnames[12] = "brge"; opnames[13] = "brhc"; 
            opnames[14] = "brtc"; opnames[15] = "brid";
            fprintf(fp, "[0x%04x]\t%s %d\n", instruction, opnames[op], offset*2);
        } else if ((instruction & 0xFC08) == 0xF800) {
            //BLD/BST
            const u8 bit = (instruction & 0x7);
            const u16 reg = (instruction >> 4) & 0xF;
            if (instruction & 0x0200) {
                fprintf(fp, "[0x%04x]\tbst r%d, %u\n", instruction, reg, bit);
            } else {
                fprintf(fp, "[0x%04x]\tbld r%d, %u\n", instruction, reg, bit);
            }
        } else if ((instruction & 0xFC08) == 0xFC00) {
            // SBRC/SBRS
            const u8 bit = (instruction & 0x7);
            const u16 reg = (instruction >> 4) & 0xF;
            if (instruction & 0x0200) {
                fprintf(fp, "[0x%04x]\tsbrs r%d, %u\n", instruction, reg, bit);
            } else {
                fprintf(fp, "[0x%04x]\tsbrc r%d, %u\n", instruction, reg, bit);
            }
        } else {
            error(0, "You have invented a new AVR instruction, gz");
        }
    }
    fclose(fp);
}

static inline u16 swapEndiannes16(u16 x) {
    return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
}

static inline u32 swapEndiannes32(u32 x) {
    return (swapEndiannes16((x & 0xFFFF0000) >> 16) << 16) | swapEndiannes16(x & 0x0000FFFF);
}

void saveIntelHex_helper(const DynArray *instructions, FILE *fp) {
    u8 checksum;
    AVR *ins = instructions->data;
    for (u64 i = 0; i < instructions->count; ++i) {
        if (i % 8 == 0) {
            if (i) {
                fprintf(fp, "%02X\n", (u8)(-checksum));
            }
            u8 numberOfRecords = (u8)(min(16, (int)(instructions->count-i)*2));
            fprintf(fp, ":%02X%04X00", numberOfRecords, (u16)(i*2));
            checksum = numberOfRecords;
            checksum += (u8)((2*i) & 0xFFFF);
            checksum += (u8)((2*i) >> 8);
            //checksum = (u8)(numberOfRecords+i*2);
        }
        u16 x = swapEndiannes16(ins[i]);
        fprintf(fp, "%04X", x);
        checksum += (u8)(x & 0xFFFF);
        checksum += (u8)(x >> 8);
    }
    fprintf(fp, "%02X\n", (u8)(-checksum));
    fprintf(fp, ":00000001FF\n");
}

void printIntelHex(const DynArray *instructions) {
    saveIntelHex_helper(instructions, stdout);
}

void saveIntelHex(const DynArray *instructions, char *outfile) {
    u64 len = strlen(outfile);
    char *of = malloc(sizeof(char) * len+5);
    strcpy(of, outfile);
    strcat(of, ".hex");
    FILE *fp = fopen(of, "w");
    saveIntelHex_helper(instructions, fp);
    fclose(fp);
}

#endif