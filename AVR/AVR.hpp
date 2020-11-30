#ifndef AVR_H
#define AVR_H

#include <vector>
#include <string>
#include "../utils/common.hpp"

namespace AVR {
    // https://en.wikipedia.org/wiki/Atmel_AVR_instruction_set#Instruction_encoding
    // http://ww1.microchip.com/downloads/cn/DeviceDoc/AVR-Instruction-Set-Manual-DS40002198A.pdf
    inline u16 NOP() {
        return 0x0;
    }
    
    inline u16 MOVW(u8 rd, u8 rr) {
        // values should be in registers 16-31
        if (rd >= (1 << 5)) {
            warning("MOVW Rd value greater than 0xF");
        }
        if (rr >= (1 << 5)) {
            warning("MOVW Rr value greater than 0xF");
        }
        return 0x0100 | ((rd & 0xF) << 4) | (rr & 0xF);
    }
    
    inline u16 MULS(u8 rd, u8 rr) {
        // values should be in registers 16-31
        if (rd >= (1 << 5)) {
            warning("MULS Rd value greater than 0xF");
        }
        if (rr >= (1 << 5)) {
            warning("MULS Rr value greater than 0xF");
        }
        return 0x0200 | ((rd & 0xF) << 4) | (rr & 0xF);
    }
    
    inline u16 MULSU(u8 rd, u8 rr) {
        // values should be in registers 16-23
        if (rd >= (1 << 4)) {
            warning("MULSU Rd value greater than 0x7");
        }
        if (rr >= (1 << 4)) {
            warning("MULSU Rr value greater than 0x7");
        }
        return 0x0300 | ((rd & 0x7) << 4) | (rr & 0x7);
    }
    
    inline u16 FMUL(u8 rd, u8 rr) {
        // values should be in registers 16-23
        if (rd >= (1 << 4)) {
            warning("FMUL Rd value greater than 0x7");
        }
        if (rr >= (1 << 4)) {
            warning("FMUL Rr value greater than 0x7");
        }
        return 0x0308 | ((rd & 0x7) << 4) | (rr & 0x7);
    }
    
    inline u16 FMULS(u8 rd, u8 rr) {
        // values should be in registers 16-23
        if (rd >= (1 << 4)) {
            warning("FMULS Rd value greater than 0x7");
        }
        if (rr >= (1 << 4)) {
            warning("FMULS Rr value greater than 0x7");
        }
        return 0x0380 | ((rd & 0x7) << 4) | (rr & 0x7);
    }
    
    inline u16 FMULSU(u8 rd, u8 rr) {
        // values should be in registers 16-23
        if (rd >= (1 << 4)) {
            warning("FMULSU Rd value greater than 0x7");
        }
        if (rr >= (1 << 4)) {
            warning("FMULSU Rr value greater than 0x7");
        }
        return 0x0388 | ((rd & 0x7) << 4) | (rr & 0x7);
    }
    
    inline u16 CPC(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("CPC Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("CPC Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x0400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 CP(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("CP Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("CP Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x1400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 SBC(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("SBC Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("SBC Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x0800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 SUB(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("SUB Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("SUB Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x1800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 ADD(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("ADD Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("ADD Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x0C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 ADC(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("ADC Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("ADC Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x1C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 LSL(u8 rd) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("LSL Rd value greater than 0x1F");
        }
        rd &= 0x1F;
        u8 rr = rd;
        return 0x0C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 ROL(u8 rd) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("ROL Rd value greater than 0x1F");
        }
        rd &= 0x1F;
        u8 rr = rd;
        return 0x1C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 CPSE(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("ADC Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("ADC Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x1000 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 AND(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("AND Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("AND Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x2000 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 EOR(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("EOR Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("EOR Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x2400 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 OR(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("OR Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("OR Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x2800 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 MOV(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("MOV Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("MOV Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x2C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 CPI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("CPI Rd value greater than 0xF");
        }
        return 0x3000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 SBCI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("SBCI Rd value greater than 0xF");
        }
        return 0x4000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 SUBI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("SUBI Rd value greater than 0xF");
        }
        return 0x5000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 ORI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("ORI Rd value greater than 0xF");
        }
        return 0x6000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 SBR(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("SBR Rd value greater than 0xF");
        }
        return 0x6000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 ANDI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("ANDI Rd value greater than 0xF");
        }
        return 0x7000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 CBR(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("CBR Rd value greater than 0xF");
        }
        return 0x7000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 LDDz(u8 rd, u8 K) {
        // registers 0-31, K 0-64
        if (rd >= (1 << 6)) {
            warning("LDDz Rd value greater than 0x1F");
        }
        if (K >= (1 << 7)) {
            warning("LDDz K value greater than 0x3F");
        }
        return 0x8000 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
    }
    
    inline u16 LDDy(u8 rd, u8 K) {
        // registers 0-31, K 0-64
        if (rd >= (1 << 6)) {
            warning("LDDy Rd value greater than 0x1F");
        }
        if (K >= (1 << 7)) {
            warning("LDDy K value greater than 0x3F");
        }
        return 0x8008 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
    }
    
    inline u16 STDz(u8 rd, u8 K) {
        // registers 0-31, K 0-64
        if (rd >= (1 << 6)) {
            warning("STDz Rd value greater than 0x1F");
        }
        if (K >= (1 << 7)) {
            warning("STDz K value greater than 0x3F");
        }
        return 0x8200 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
    }
    
    inline u16 STDy(u8 rd, u8 K) {
        // registers 0-31, K 0-64
        if (rd >= (1 << 6)) {
            warning("STDy Rd value greater than 0x1F");
        }
        if (K >= (1 << 7)) {
            warning("STDy K value greater than 0x3F");
        }
        return 0x8208 | (K & 0x7) | ((K & 0x18) << 7) | ((K & 0x20) << 11) | ((rd & 0x1F) << 4);
    }
    
    inline u32 LDS(u8 rd, u16 address) {
        // registers 0-31, address 0-65535
        if (rd >= (1 << 6)) {
            warning("LDS Rd value greater than 0x1F");
        }
        return (u32)0x90000000 | address | ((rd & 0x1F) << 20);
    }
    
    inline u32 STS(u8 rd, u16 address) {
        // registers 0-31, address 0-65535
        if (rd >= (1 << 6)) {
            warning("STS Rd value greater than 0x1F");
        }
        return (u32)0x92000000 | address | ((rd & 0x1F) << 20);
    }
    
    inline u16 LDp(u8 rd, u8 y) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LD+ Rd value greater than 0x1F");
        }
        y = !!y;
        return 0x9001 | ((rd & 0x1F) << 4) | (y << 3);
    }
    
    inline u16 STp(u8 rd, u8 y) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("ST+ Rd value greater than 0x1F");
        }
        y = !!y;
        return 0x9201 | ((rd & 0x1F) << 4) | (y << 3);
    }
    
    inline u16 LDm(u8 rd, u8 y) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LD- Rd value greater than 0x1F");
        }
        y = !!y;
        return 0x9001 | ((rd & 0x1F) << 4) | (y << 3);
    }
    
    inline u16 STm(u8 rd, u8 y) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("ST- Rd value greater than 0x1F");
        }
        y = !!y;
        return 0x9202 | ((rd & 0x1F) << 4) | (y << 3);
    }
    
    inline u16 LPM(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LPM Rd value greater than 0x1F");
        }
        return 0x9004 | ((rd & 0x1F) << 4);
    }
    
    inline u16 ELPM(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("ELPM Rd value greater than 0x1F");
        }
        return 0x9006 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LPMp(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LPM+ Rd value greater than 0x1F");
        }
        return 0x9005 | ((rd & 0x1F) << 4);
    }
    
    inline u16 ELPMp(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("ELPM+ Rd value greater than 0x1F");
        }
        return 0x9007 | ((rd & 0x1F) << 4);
    }
    
    inline u16 XCH(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("XCH Rd value greater than 0x1F");
        }
        return 0x9204 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LAS(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LAS Rd value greater than 0x1F");
        }
        return 0x9205 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LAC(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LAC Rd value greater than 0x1F");
        }
        return 0x9206 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LAT(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LAT Rd value greater than 0x1F");
        }
        return 0x9207 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LDx(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LDx Rd value greater than 0x1F");
        }
        return 0x900C | ((rd & 0x1F) << 4);
    }
    
    inline u16 STx(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("STx Rd value greater than 0x1F");
        }
        return 0x920C | ((rd & 0x1F) << 4);
    }
    
    inline u16 LDxp(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LDxp Rd value greater than 0x1F");
        }
        return 0x900D | ((rd & 0x1F) << 4);
    }
    
    inline u16 STxp(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("STxp Rd value greater than 0x1F");
        }
        return 0x920D | ((rd & 0x1F) << 4);
    }
    
    inline u16 LDxm(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LDxm Rd value greater than 0x1F");
        }
        return 0x900E | ((rd & 0x1F) << 4);
    }
    
    inline u16 STxm(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("STxm Rd value greater than 0x1F");
        }
        return 0x920E | ((rd & 0x1F) << 4);
    }
    
    inline u16 POP(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("POP Rd value greater than 0x1F");
        }
        return 0x900F | ((rd & 0x1F) << 4);
    }
    
    inline u16 PUSH(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("PUSH Rd value greater than 0x1F");
        }
        return 0x920F | ((rd & 0x1F) << 4);
    }
    
    inline u16 COM(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("COM Rd value greater than 0x1F");
        }
        return 0x9400 | ((rd & 0x1F) << 4);
    }
    
    inline u16 NEG(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("NEG Rd value greater than 0x1F");
        }
        return 0x9401 | ((rd & 0x1F) << 4);
    }
    
    inline u16 SWAP(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("SWAP Rd value greater than 0x1F");
        }
        return 0x9402 | ((rd & 0x1F) << 4);
    }
    
    inline u16 INC(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("INC Rd value greater than 0x1F");
        }
        return 0x9403 | ((rd & 0x1F) << 4);
    }
    
    inline u16 ASR(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("COM Rd value greater than 0x1F");
        }
        return 0x9405 | ((rd & 0x1F) << 4);
    }
    
    inline u16 LSR(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("LSR Rd value greater than 0x1F");
        }
        return 0x9406 | ((rd & 0x1F) << 4);
    }
    
    inline u16 ROR(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("ROR Rd value greater than 0x1F");
        }
        return 0x9407 | ((rd & 0x1F) << 4);
    }
    
    inline u16 SER(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("SE Rd value greater than 0x1F");
        }
        return 0xCF0F | ((rd & 0xF) << 4);
    }
    
    inline u16 CLR(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("CLR Rd value greater than 0x1F");
        }
        return 0x2400 | (rd & 0xF) | ((rd & 0xF) << 4);
    }
    
    inline u16 SEC() {
        return 0x9408;
    }
    
    inline u16 SEH() {
        return 0x9458;
    }
    
    inline u16 SEI() {
        return 0x9478;
    }
    
    inline u16 SEN() {
        return 0x9428;
    }
    
    inline u16 SES() {
        return 0x9448;
    }
    
    inline u16 SET() {
        return 0x9468;
    }
    
    inline u16 SEV() {
        return 0x9438;
    }
    
    inline u16 SEZ() {
        return 0x948;
    }
    
    inline u16 CLC() {
        return 0x9488;
    }
    
    inline u16 CLH() {
        return 0x94D8;
    }
    
    inline u16 CLI() {
        return 0x94F8;
    }
    
    inline u16 CLN() {
        return 0x94A8;
    }
    
    inline u16 CLS() {
        return 0x94C8;
    }
    
    inline u16 CLT() {
        return 0x94E8;
    }
    
    inline u16 CLV() {
        return 0x94B8;
    }
    
    inline u16 CLZ() {
        return 0x9498;
    }
    
    inline u16 RET() {
        return 0x9508;
    }
    
    inline u16 RETI() {
        return 0x9518;
    }
    
    inline u16 SLEEP() {
        return 0x9588;
    }
    
    inline u16 BREAK() {
        return 0x9598;
    }
    
    inline u16 WDR() {
        return 0x95A8;
    }
    
    inline u16 LPM() {
        return 0x95C8;
    }
    
    inline u16 ELPM() {
        return 0x95D8;
    }
    
    inline u16 SPM() {
        return 0x95E8;
    }
    
    inline u16 SPMzp() {
        return 0x95F8;
    }
    
    inline u16 IJMP() {
        return 0x9409;
    }
    
    inline u16 EIJMP() {
        return 0x9419;
    }
    
    inline u16 ICALL() {
        return 0x9509;
    }
    
    inline u16 EICALL() {
        return 0x9519;
    }
    
    inline u16 DEC(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("DEC Rd value greater than 0x1F");
        }
        return 0x940A | ((rd & 0x1F) << 4);
    }
    
    inline u16 DES(u8 K) {
        // registers 0-31
        if (K >= (1 << 5)) {
            warning("DES K value greater than 0xF");
        }
        return 0x940B | ((K & 0xF) << 4);
    }
    
    inline u32 JMP(u32 address) {
        // address 0-4194303
        if (address >= (1 << 23)) {
            warning("JMP address value greater than 0x3FFFFF");
        }
        return (u32)0x940C0000 | (address & 0x1FFFF) | ((address & 0x3E0000) << 3);
    }
    
    inline u32 CALL(u32 address) {
        // address 0-4194303
        if (address >= (1 << 23)) {
            warning("CALL address value greater than 0x3FFFFF");
        }
        return (u32)0x940E0000 | (address & 0x1FFFF) | ((address & 0x3E0000) << 3);
    }
    
    inline u16 ADIW(u8 rp, u8 K) {
        // pp = Register pair, W, X, Y or Z
        // K 0-63
        if (rp >= (1 << 3)) {
            warning("ADIW Rp value greater than 0x3");
        }
        if (K >= (1 << 7)) {
            warning("ADIW K value greater than 0x3F");
        }
        return 0x960B | (K & 0xF) | ((rp & 0x3) << 4) | ((K & 0x30) << 2);
    }
    
    inline u16 SBIW(u8 rp, u8 K) {
        // pp = Register pair, W, X, Y or Z
        // K 0-63
        if (rp >= (1 << 3)) {
            warning("SBIW Rp value greater than 0x3");
        }
        if (K >= (1 << 7)) {
            warning("SBIW K value greater than 0x3F");
        }
        return 0x970B | (K & 0xF) | ((rp & 0x3) << 4) | ((K & 0x30) << 2);
    }
    
    inline u16 CBI(u8 A, u8 B) {
        // A 0-63, B 0-8
        if (A >= (1 << 6)) {
            warning("CBI A value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("CBI B value greater than 0x7");
        }
        return 0x9800 | (B & 0x7) | ((A & 0x1F) << 3);
    }
    
    inline u16 SBI(u8 A, u8 B) {
        // A 0-63, B 0-8
        if (A >= (1 << 6)) {
            warning("SBI A value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("SBI B value greater than 0x7");
        }
        return 0x9A00 | (B & 0x7) | ((A & 0x1F) << 3);
    }
    
    inline u16 SBIC(u8 A, u8 B) {
        // A 0-63, B 0-8
        if (A >= (1 << 6)) {
            warning("SBIC A value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("SBIC B value greater than 0x7");
        }
        return 0x9900 | (B & 0x7) | ((A & 0x1F) << 3);
    }
    
    inline u16 SBIS(u8 A, u8 B) {
        // A 0-63, B 0-8
        if (A >= (1 << 6)) {
            warning("SBIS A value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("SBIS B value greater than 0x7");
        }
        return 0x9C00 | (B & 0x7) | ((A & 0x1F) << 3);
    }
    
    inline u16 MUL(u8 rd, u8 rr) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("MUL Rd value greater than 0x1F");
        }
        if (rr >= (1 << 6)) {
            warning("MUL Rr value greater than 0x1F");
        }
        rd &= 0x1F;
        rr &= 0x1F;
        return 0x9C00 | (rd << 4) | (rr & 0xF) | ((rr & 0x10) << 5);
    }
    
    inline u16 IN(u8 rd, u8 a) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("IN Rd value greater than 0x1F");
        }
        if (a >= (1 << 6)) {
            warning("IN A value greater than 0x3F");
        }
        rd &= 0x1F;
        a &= 0x3F;
        return 0xB000 | (rd << 4) | (a & 0xF) | ((a & 0x30) << 5);
    }
    
    inline u16 OUT(u8 rd, u8 a) {
        // values should be in registers 0-31
        if (rd >= (1 << 6)) {
            warning("OUT Rd value greater than 0x1F");
        }
        if (a >= (1 << 6)) {
            warning("OUT A value greater than 0x3F");
        }
        rd &= 0x1F;
        a &= 0x3F;
        return 0xB800 | (rd << 4) | (a & 0xF) | ((a & 0x30) << 5);
    }
    
    inline u16 RJMP(u16 offset) {
        // offset is a signed 12 bit integer, we'll treat it as an uint for ease
        // please prepare it beforehand uwu
        if (offset >= (1 << 13)) {
            warning("RJMP offset value greater than 0xFFF");
        }
        return 0xC000 | (offset & 0x0FFF);
    }
    
    inline u16 RCALL(u16 offset) {
        // offset is a signed 12 bit integer, we'll treat it as an uint for ease
        // please prepare it beforehand uwu
        if (offset >= (1 << 13)) {
            warning("RCALL offset value greater than 0xFFF");
        }
        return 0xD000 | (offset & 0x0FFF);
    }
    
    inline u16 LDI(u8 rd, u8 K) {
        // registers 16-31, K 0-255
        if (rd >= (1 << 5)) {
            warning("LDI Rd value greater than 0xF");
        }
        return 0xE000 | ((rd & 0xF) << 4) | (K & 0xF) | ((K & 0xF0) << 4);
    }
    
    inline u16 BRBC(u8 K, u8 B) {
        // K 0-127 (7 bit signed int, treated as uint), B 0-8
        if (K >= (1 << 8)) {
            warning("BRBC K value greater than 0x7F");
        }
        if (B >= (1 << 4)) {
            warning("BRBC B value greater than 0x7");
        }
        return 0xF400 | ((K & 0x7F) << 3) | (B & 0x7);
    }
    
    inline u16 BRBS(u8 K, u8 B) {
        // K 0-127 (7 bit signed int, treated as uint), B 0-8
        if (K >= (1 << 8)) {
            warning("BRBS K value greater than 0x7F");
        }
        if (B >= (1 << 4)) {
            warning("BRBS B value greater than 0x7");
        }
        return 0xF000 | ((K & 0x7F) << 3) | (B & 0x7);
    }
    
    inline u16 BRCC(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRCC K value greater than 0x7F");
        }
        return 0xF400 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRCS(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRCS K value greater than 0x7F");
        }
        return 0xF000 | ((K & 0x7F) << 3);
    }
    
    inline u16 BREQ(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BREQ K value greater than 0x7F");
        }
        return 0xF001 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRGE(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRGE K value greater than 0x7F");
        }
        return 0xF404 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRHC(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRHC K value greater than 0x7F");
        }
        return 0xF405 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRHS(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRHS K value greater than 0x7F");
        }
        return 0xF005 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRID(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRID K value greater than 0x7F");
        }
        return 0xF407 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRIE(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRIE K value greater than 0x7F");
        }
        return 0xF007 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRLO(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRLO K value greater than 0x7F");
        }
        return 0xF000 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRLT(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRLT K value greater than 0x7F");
        }
        return 0xF004 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRMI(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRMI K value greater than 0x7F");
        }
        return 0xF002 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRNE(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRNE K value greater than 0x7F");
        }
        return 0xF401 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRPL(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRPL K value greater than 0x7F");
        }
        return 0xF402 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRSH(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRSH K value greater than 0x7F");
        }
        return 0xF400 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRTC(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRTC K value greater than 0x7F");
        }
        return 0xF406 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRTS(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRTS K value greater than 0x7F");
        }
        return 0xF006 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRVC(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRVC K value greater than 0x7F");
        }
        return 0xF403 | ((K & 0x7F) << 3);
    }
    
    inline u16 BRVS(u8 K) {
        // K 0-127 (7 bit signed int, treated as uint)
        if (K >= (1 << 8)) {
            warning("BRVS K value greater than 0x7F");
        }
        return 0xF003 | ((K & 0x7F) << 3);
    }
    
    inline u16 BSET(u8 S) {
        // S 0-7 
        if (S >= (1 << 8)) {
            warning("BSET S value greater than 0x7");
        }
        return 0x9408 | ((S & 0x7) << 4);
    }
    
    inline u16 BCLR(u8 S) {
        // S 0-7 
        if (S >= (1 << 8)) {
            warning("BCLR S value greater than 0x7");
        }
        return 0x9488 | ((S & 0x7) << 4);
    }
    
    inline u16 BLD(u8 rd, u8 B) {
        // registers 16-31, B 0-8
        if (rd >= (1 << 6)) {
            warning("BLD Rd value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("BLD B value greater than 0x7");
        }
        return 0xF800 | ((rd & 0x1F) << 4) | (B & 0x7);
    }
    
    inline u16 BST(u8 rd, u8 B) {
        // registers 16-31, B 0-8
        if (rd >= (1 << 6)) {
            warning("BST Rd value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("BST B value greater than 0x7");
        }
        return 0xFA00 | ((rd & 0x1F) << 4) | (B & 0x7);
    }
    
    inline u16 SBRC(u8 rd, u8 B) {
        // registers 16-31, B 0-8
        if (rd >= (1 << 6)) {
            warning("SBRC Rd value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("SBRC B value greater than 0x7");
        }
        return 0xFE00 | ((rd & 0x1F) << 4) | (B & 0x7);
    }
    
    inline u16 SBRS(u8 rd, u8 B) {
        // registers 16-31, B 0-8
        if (rd >= (1 << 6)) {
            warning("SBRS Rd value greater than 0x1F");
        }
        if (B >= (1 << 4)) {
            warning("SBRS B value greater than 0x7");
        }
        return 0xFF00 | ((rd & 0x1F) << 4) | (B & 0x7);
    }
    
    inline u16 TST(u8 rd) {
        // registers 0-31
        if (rd >= (1 << 6)) {
            warning("SBRS Rd value greater than 0x1F");
        }
        return 0x2000 | ((rd & 0x1F) << 5) | (rd & 0x1F);
    }
    
    void print(const std::vector<u16> &instructions) {
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
    
    inline u16 swapEndiannes16(u16 x) {
        return ((x & 0xFF00) >> 8) | ((x & 0x00FF) << 8);
    }
    
    inline u32 swapEndiannes32(u32 x) {
        return (swapEndiannes16((x & 0xFFFF0000) >> 16) << 16) | swapEndiannes16(x & 0x0000FFFF);
    }
    
    void printIntelHex(const std::vector<u16> &instructions) {
        u8 checksum;
        for (size_t i = 0; i < instructions.size(); ++i) {
            if (i % 8 == 0) {
                if (i) {
                    printf("%02X\n", u8(~checksum+1));
                }
                u8 numberOfRecords = std::min(16, int(instructions.size()-i)*2);
                printf(":%02X%04X00", numberOfRecords, i*2);
                checksum = numberOfRecords+i*2;
            }
            u16 x = swapEndiannes16(instructions[i]);
            printf("%04X", x);
            checksum += x & 0xFFFF;
            checksum += x >> 8;
        }
        if (instructions.size() % 8) {
            printf("%02X\n", u8(~checksum+1));
        }
        printf(":00000001FF\n");
    }
};

#endif