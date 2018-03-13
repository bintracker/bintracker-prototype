// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include "z80.h"

// done generic conditionals: evaluates as true if cond=!flag and flag is set
// done conditional still not working
// done 16-bit arithmetic can be made generic, register address decoding is same as for 8-bit...
//      made generic, but also broke them
// done check sp arithmetic flags
// done 16-bit arithmetic flags
// done add/sub/sbc/adc_a_athl
// done add/adc/sbc hl,sp / add/adc/sbc ix,sp / add/adc/sbc iy,sp
// done All memory[]/MEMPTR access must make sure it doesn't go out of bound (& 0xffff)
// done check PO/PE behaviour
// done V flag wrong at least on CP ops (V flag is carry from bit 6 -> 7 ?), wrong on OR/...
//      ->??? or sets parity, not overflow
// done optimize JP/CALL instructions (check block Ex)
// done cp operations set 5,3 flags by operand - but which operand??? answer: the second one
// done wrong H flag calculation on 16-bit ADC/SBC, N flag not set on SBC_r16
// done wrong H flag on inc/dec (possibly also V?)
// TODO(utz): ED instructions are missing some flag updates
// TODO(utz): (Block) IO flags
// done bit x,(hl) 5,3 flags, bit x,(iyx+d) all flags (check docs)
// done setting Z flag on INC/DEC instructions might be wrong
// done V flag on INC/DEC can be written unconditionally - see inc (ix+d)
// done add hl,sp add ix,sp
// done block ops should be executed one-at-a-time, setting PV flag while active and resetting PC
// done (index+displacement) offset calculation is wrong
// done rework inc/dec_athl
// done PV still not correct on cp_/sbx_a
// done parity table doesn't work reliably, and should be made static - tbl_gen output is correct though
// done daa
// done res (iyx+d) instruction timing wrong, sets flags <- instruction not triggered!
// TODO(utz): some instruction timings still seem wrong
// done: seperate tick calculation, run instructions at the end of the count
// TODO(utz): add interrupts
// TODO(utz): regQ emulation


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    REGULAR INSTRUCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// GENERIC 16-BIT REGISTER
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_r16_nn() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    *(reg + 1) = memory->at((regPC + 1) & 0xffff);
    *reg = memory->at((regPC + 2) & 0xffff);

    regPC += 3;
}

void z80cpu::add_hl_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

    regL += *reg;

    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regL >> 8) + (regH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);

    regH += ((regL >> 8) + (*(reg - 1)));
    regL &= 0xff;

    regF = regF | (regH & 0x28) | (regH >> 8);          // set 5,3,C

    regH &= 0xff;

    regPC++;
}

void z80cpu::inc_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    *(reg + 1) += 1;

    *reg = (*reg + (*(reg + 1) >> 8)) & 0xff;
    *(reg + 1) &= 0xff;

    regPC++;
}

void z80cpu::dec_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    *reg -= 1;

    *(reg - 1) = (*(reg - 1) + (*reg >> 8)) & 0xff;
    *reg &= 0xff;

    regPC++;
}


void z80cpu::pop_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    *(reg + 1) = memory->at(regSP);
    *reg = memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;

    regPC++;
}

void z80cpu::push_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = *(reg + 1);
    memory->at((regSP + 1) & 0xffff) = *reg;

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// GENERIC 8-BIT REGISTER
////////////////////////////////////////////////////////////////////////////////

void z80cpu::inc_r8() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regF = (regF & 1) | (((*reg & 0xf) + 1) & 0x10);        // H,N,(C)

    *reg = (*reg + 1) & 0xff;

    regF |= (*reg & 0xa8);                                  // S,5,3
    if (*reg == 0x80) regF |= 4;                            // V
    if (!*reg) regF |= 0x40;                                // Z

    regPC++;
}

void z80cpu::dec_r8() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regF = (regF & 1) | (((*reg & 0xf) - 1) & 0x10) | 2;    // H,N,(C)
    if (*reg == 0x80) regF |= 4;                            // V

    *reg = (*reg - 1) & 0xff;

    regF |= (*reg & 0xa8);                                  // S,5,3
    if (!*reg) regF |= 0x40;                                // Z

    regPC++;
}

void z80cpu::ld_r8_n() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = memory->at((regPC + 1) & 0xffff);
    regPC += 2;
}

void z80cpu::ld_r8_r8() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::ld_r8_athl() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = memory->at(regL + (regH << 8));
    regPC++;
}

void z80cpu::ld_athl_r8() noexcept {
    memory->at(regL + (regH << 8)) = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::adx_a_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];
    // performs add_a_r8 when bit 3 of opcode is reset, else adc_a_r8
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA + *reg + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (*reg & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~(*reg)) & (regA ^ (regA + *reg + oldcarry))) & 0x80) >> 5);        // set H,V,C flags

    regA = (regA + *reg + oldcarry) & 0xff;

    regF |= (regA & 0xa8);                          // set S,5,3 flag
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::sbx_a_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];
    // performs sub_r8 when bit 3 of opcode is reset, else sbc_a_r8
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA - *reg - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (*reg & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ *reg) & (regA ^ (regA - *reg - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - *reg - oldcarry) & 0xff;

    regF |= (regA & 0xa8);                          // set S,5,3 flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::and_r8() noexcept {
    regA &= *regPtr[memory->at(regPC) & 0x7];
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC++;
}

void z80cpu::xor_r8() noexcept {
    regA = (regA ^ *regPtr[memory->at(regPC) & 0x7]) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::or_r8() noexcept {
    regA = (regA | *regPtr[memory->at(regPC) & 0x7]) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::cp_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    regF = (((regA - *reg) & 0x100) >> 8) | (((regA & 0xf) - (*reg & 0xf)) & 0x10)      // H,C
        | ((((regA ^ *reg) & (regA ^ (regA - *reg))) & 0x80) >> 5) | 2                  // V,N
        | ((regA - *reg) & 0x80) | (*reg & 0x28);                                       // S,5,3 flags
    if (!(regA - *reg)) regF |= 0x40;                                                   // Z

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// GENERIC CONDITIONAL
////////////////////////////////////////////////////////////////////////////////

void z80cpu::jr_cc_n() noexcept {
    if (((regF & conditionCodes[(memory->at(regPC) & 0x10) >> 4]) && (memory->at(regPC) & 8))
        || ((!(regF & conditionCodes[(memory->at(regPC) & 0x10) >> 4])) && (!(memory->at(regPC) & 8)))) {
        regPC++;
        regPC = (regPC + 1 + (((-(memory->at(regPC) >> 7)) & 0xffffff00) | memory->at(regPC))) & 0xffff;

        regMEMPTR = regPC;
    } else {
        regPC += 2;
    }
}

void z80cpu::ret_cc() noexcept {
    if (((regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4]) && (memory->at(regPC) & 8))
        || ((!(regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4])) && (!(memory->at(regPC) & 8)))) {
        regPC = memory->at(regSP) + (memory->at((regSP + 1) & 0xffff) << 8);
        regMEMPTR = regPC;
        regSP = (regSP + 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::jp_cc_nn() noexcept {
    regMEMPTR = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    if (((regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4]) && (memory->at(regPC) & 8))
        || ((!(regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4])) && (!(memory->at(regPC) & 8)))) {
        regPC = regMEMPTR;
    } else {
        regPC += 3;
    }
}

void z80cpu::call_cc_nn() noexcept {
    regMEMPTR = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    if (((regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4]) && (memory->at(regPC) & 8))
        || ((!(regF & conditionCodes[(memory->at(regPC) & 0x30) >> 4])) && (!(memory->at(regPC) & 8)))) {
        regSP = (regSP - 2) & 0xffff;
        memory->at(regSP) = (regPC + 3) & 0xff;
        memory->at((regSP + 1) & 0xffff) = ((regPC + 3) & 0xff00) >> 8;

        regPC = regMEMPTR;
    } else {
        regPC += 3;
    }
}


////////////////////////////////////////////////////////////////////////////////
// GENERIC MISC
////////////////////////////////////////////////////////////////////////////////

void z80cpu::rst_xx() noexcept {
    regPC = memory->at(regPC) & 0x38;
    regMEMPTR = regPC;
}

////////////////////////////////////////////////////////////////////////////////
// group 0x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::nop() noexcept {
    regPC++;
}

void z80cpu::ld_atbc_a() noexcept {
    memory->at(regC + (regB << 8)) = regA;

    regMEMPTR = ((regC + 1) & 0xff) | (regA * 0x100);
    if (cpuType == Z80Type::BM1) regMEMPTR &= 0xff;

    regPC++;
}

void z80cpu::rlca() noexcept {
    int carry = 0;
    if (regA & 0x80) carry = 1;

    regA = (regA << 1) + carry;

    regF = (regF & 0xd5) | (regA & 0x28) | carry;       // set 5,3,N,C flag

    regPC++;
}

void z80cpu::ex_af_af() noexcept {
    int temp = regA;
    regA = regAs;
    regAs = temp;
    temp = regF;
    regF = regFs;
    regFs = temp;
    regPC++;
}

void z80cpu::ld_a_atbc() noexcept {
    regA = memory->at(regC + (regB << 8));

    regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

    regPC++;
}

void z80cpu::rrca() noexcept {
    int carry = regA & 1;
    regA = (regA >> 1) | (carry * 0x80);

    regF = (regF & 0xd5) | (regA & 0x28) | carry;       // set 5,3,N,C flag

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// group 1x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::djnz() noexcept {
    regB = (regB - 1) & 0xff;

    if (regB) {
        regPC++;
        regPC = (regPC + 1 + (((-(memory->at(regPC) >> 7)) & 0xffffff00) | memory->at(regPC))) & 0xffff;
    } else {
        regPC += 2;
    }

    regMEMPTR = regPC;
}

void z80cpu::ld_atde_a() noexcept {
    memory->at(regE + (regD << 8)) = regA;

    regMEMPTR = ((regE + 1) & 0xff) | (regA << 8);
    if (cpuType == Z80Type::BM1) regMEMPTR &= 0xff;

    regPC++;
}

void z80cpu::rla() noexcept {
    int carry = 0;
    if (regA & 0x80) carry = 1;

    regA = (regA << 1) + (regF & 1);
    regF = (regF & 0xd5) | (regA & 0x28) | carry;       // set 5,3,N,C flag
    regPC++;
}

void z80cpu::jr() noexcept {
    regPC++;
    regPC = (regPC + 1 + (((-(memory->at(regPC) >> 7)) & 0xffffff00) | memory->at(regPC))) & 0xffff;
    regMEMPTR = regPC;
}

void z80cpu::ld_a_atde() noexcept {
    regA = memory->at(regE + (regD << 8));
    regMEMPTR = (regE + (regD << 8) + 1) & 0xffff;
    regPC++;
}

void z80cpu::rra() noexcept {
    int carry = regA & 1;

    regA = (regA >> 1) | ((regF & 1) * 0x80);
    regF = (regF & 0xd5) | (regA & 0x28) | carry;       // set 5,3,N,C flag
    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// group 2x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_atnn_hl() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regMEMPTR = (addr + 1) & 0xffff;

    memory->at(addr) = regL;
    memory->at(regMEMPTR) = regH;

    regPC += 3;
}

void z80cpu::daa() noexcept {
    int temp = 0;
    int carry = regF & 1;

    if ((regF & 0x10) || ((regA & 0xf) > 9)) temp = 6;
    if (carry || regA > 0x99) temp |= 0x60;
    if (regA > 0x99) carry |= 1;

    if (regF & 2) {
        regF = (((regA & 0xf) - (temp & 0xf)) & 0x10) | 2;
        regA = (regA - temp) & 0xff;
    } else {
        regF = (((regA & 0xf) + (temp & 0xf)) & 0x10);
        regA = (regA + temp) & 0xff;
    }

    regF = (regF & 0xfa) | (regA & 0xa8) | carry | parityTable[regA];
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::ld_hl_atnn() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regMEMPTR = (addr + 1) & 0xffff;

    regL = memory->at(addr);
    regH = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::cpl() noexcept {
    regA = (~regA) & 0xff;
    regF = ((regF & 0xd7) | (regA & 0x28)) | 0x12;      // set 5,H,3,N flags
    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block 3x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_sp_nn() noexcept {
    regSP = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regPC += 3;
}

void z80cpu::ld_atnn_a() noexcept {
    regMEMPTR = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    memory->at(regMEMPTR) = regA;
    regMEMPTR = (regMEMPTR + 1) & 0xffff;
    regPC += 3;
}

void z80cpu::inc_sp() noexcept {
    regSP = (regSP + 1) & 0xffff;
    regPC++;
}

void z80cpu::inc_athl() noexcept {
    int regHL = regL + (regH << 8);

    regF = (regF & 1) | (((memory->at(regHL) & 0xf) + 1) & 0x10);       // H,N,(C)

    memory->at(regHL) = (memory->at(regHL) + 1) & 0xff;

    regF |= (memory->at(regHL) & 0xa8);                                 // S,5,3
    if (memory->at(regHL) == 0x80) regF |= 4;                           // V
    if (!memory->at(regHL)) regF |= 0x40;                               // Z

    regPC++;
}

void z80cpu::dec_athl() noexcept {
    int regHL = regL + (regH << 8);

    regF = (regF & 1) | (((memory->at(regHL) & 0xf) - 1) & 0x10) | 2;   // H,N,(C)
    if (memory->at(regHL) == 0x80) regF |= 4;                           // V

    memory->at(regHL) = (memory->at(regHL) - 1) & 0xff;

    regF |= (memory->at(regHL) & 0xa8);                                 // S,5,3
    if (!memory->at(regHL)) regF |= 0x40;                               // Z

    regPC++;
}

void z80cpu::ld_athl_n() noexcept {
    memory->at(regL + (regH << 8)) = memory->at((regPC + 1) & 0xffff);
    regPC++;
}

void z80cpu::scf() noexcept {
    regF = (regF & 0xc5) | (regA & 0x28) | 1;       // set 5,H,3,N,C flags
    regPC++;
}

void z80cpu::add_hl_sp() noexcept {
    int sp_lo = regSP & 0xff;
    int sp_hi = regSP >> 8;

    regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

    regL += sp_lo;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regL >> 8) + (regH & 0xf) + (sp_hi & 0xf)) & 0x10);

    regH += (regL >> 8) + sp_hi;
    regL &= 0xff;

    regF = regF | (regH & 0x28) | (regH >> 8);      // set 5,3,C

    regH &= 0xff;

    regPC++;
}

void z80cpu::ld_a_atnn() noexcept {
    regMEMPTR = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regA = memory->at(regMEMPTR);

    regMEMPTR = (regMEMPTR + 1) & 0xffff;

    regPC += 3;
}

void z80cpu::dec_sp() noexcept {
    regSP = (regSP - 1) & 0xffff;

    regPC++;
}

void z80cpu::ccf() noexcept {
    regF = ((regF & 0xc5) | (regA & 0x28) | ((regF & 1) << 4)) ^ 1;     // set 5,H,3,N,C flags

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block 7x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::halt() noexcept {
    regPC++;    // TODO(utz): instruction currently disabled, acts as nop
}


////////////////////////////////////////////////////////////////////////////////
// block 8x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::adx_a_athl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);
    int addval = memory->at(regL + (regH << 8));

    regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~(addval)) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);        // set H,V,C flags

    regA = (regA + addval + oldcarry) & 0xff;

    regF |= (regA & 0xa8);          // set S,5,3 flag
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// block 9x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::sbx_a_athl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);
    int subval = memory->at(regL + (regH << 8));

    regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - subval - oldcarry) & 0xff;

    regF |= (regA & 0xa8);                          // set S,5,3 flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// block Ax
////////////////////////////////////////////////////////////////////////////////

void z80cpu::and_athl() noexcept {
    regA &= memory->at(regL + (regH << 8));
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC++;
}

void z80cpu::xor_athl() noexcept {
    regA = (regA ^ memory->at(regL + (regH << 8))) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];   // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                    // set Z flag

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block Bx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::or_athl() noexcept {
    regA = (regA | memory->at(regL + (regH << 8))) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::cp_athl() noexcept {
    int cpval = memory->at(regL + (regH << 8));

    regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)    // H,C
        | ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2                // V,N
        | ((regA - cpval) & 0x80) | (cpval & 0x28);                                     // S,5,3 flags
    if (!(regA - cpval)) regF |= 0x40;                                                  // Z

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block Cx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::jp_nn() noexcept {
    regPC = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regMEMPTR = regPC;
}

void z80cpu::ret() noexcept {
    regPC = memory->at(regSP) + (memory->at((regSP + 1) & 0xffff) << 8);
    regMEMPTR = regPC;
    regSP = (regSP + 2) & 0xffff;
}

void z80cpu::prefix_cb() noexcept {
    regPC = (regPC + 1) & 0xffff;

    if (memory->at(regPC) >= 0xc0) {
        cpu_instructions_cb_set[memory->at(regPC) & 0x7](this);
    } else if (memory->at(regPC) >= 0x80) {
        cpu_instructions_cb_res[memory->at(regPC) & 0x7](this);
    } else if (memory->at(regPC) >= 0x40) {
        cpu_instructions_cb_bit[memory->at(regPC) & 0x7](this);
    } else {
        cpu_instructions_cb[static_cast<unsigned>(memory->at(regPC))](this);
    }

    regPC++;
}

void z80cpu::call_nn() noexcept {
    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = (regPC + 3) & 0xff;
    memory->at((regSP + 1) & 0xffff) = ((regPC + 3) & 0xff00) >> 8;
    regPC = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regMEMPTR = regPC;
}

void z80cpu::adx_a_n() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);
    int addval = memory->at((regPC + 1) & 0xffff);

    regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~(addval)) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);        // set H,V,C flags

    regA = (regA + addval + oldcarry) & 0xff;

    regF |= (regA & 0xa8);          // set S,5,3 flag
    if (!regA) regF |= 0x40;        // set Z flag

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// block Dx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::out_atn_a() noexcept {
    regMEMPTR = (memory->at((regPC + 1) & 0xffff) + 1) & 0xff;
    if (cpuType != Z80Type::BM1) regMEMPTR |= (regA * 0x100);

    outputPorts[memory->at((regPC + 1) & 0xffff) + (regA << 8)] = regA;
    outputPortsShort[memory->at((regPC + 1) & 0xffff)] = regA;
    regPC += 2;
}

void z80cpu::exx() noexcept {
    int temp = regB;
    regB = regBs;
    regBs = temp;
    temp = regC;
    regC = regCs;
    regCs = temp;
    temp = regD;
    regD = regDs;
    regDs = temp;
    temp = regE;
    regE = regEs;
    regEs = temp;
    temp = regH;
    regH = regHs;
    regHs = temp;
    temp = regL;
    regL = regLs;
    regLs = temp;

    regPC++;
}

void z80cpu::in_a_atn() noexcept {                    // TODO(utz): temporary solution!
    regMEMPTR = ((regA << 8) + memory->at((regPC + 1) & 0xffff) + 1) & 0xffff;

    regA = inputPorts[memory->at((regPC + 1) & 0xffff) + (regA << 8)]
        | inputPortsShort[memory->at((regPC + 1) & 0xffff)];

    regPC += 2;
}

void z80cpu::prefix_dd() noexcept {
    regPC = (regPC + 1) & 0xffff;
    cpu_instructions_dd[static_cast<unsigned>(memory->at(regPC))](this);
}

void z80cpu::sbx_a_n() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);
    int subval = memory->at((regPC + 1) & 0xffff);

    regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - subval - oldcarry) & 0xff;

    regF |= (regA & 0xa8);                          // set S,5,3 flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// block Ex
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ex_atsp_hl() noexcept {
    int temp = regL;
    regL = memory->at(regSP);
    memory->at(regSP) = temp;

    temp = regH;
    regH = memory->at((regSP + 1) & 0xffff);
    memory->at((regSP + 1) & 0xffff) = temp;

    regPC++;
}

void z80cpu::and_n() noexcept {
    regA &= memory->at((regPC + 1) & 0xffff);
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC += 2;
}

void z80cpu::jp_athl() noexcept {
    regPC = regL + (regH << 8);
}

void z80cpu::ex_de_hl() noexcept {
    int temp = regD;
    regD = regH;
    regH = temp;
    temp = regE;
    regE = regL;
    regL = temp;

    regPC++;
}

void z80cpu::prefix_ed() noexcept {
    regPC++;
    regR = (regR & 0x80) | ((regR + 1) & 0x7f);

    if ((memory->at(regPC) < 0x40) | (memory->at(regPC) > 0xbb)) {      // skip useless instructions
        regPC++;
    } else {
        cpu_instructions_ed[static_cast<unsigned>(memory->at(regPC) - 0x40)](this);
    }
}

void z80cpu::xor_n() noexcept {
    regA = (regA ^ memory->at((regPC + 1) & 0xffff)) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// block Fx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::pop_af() noexcept {
    regF = memory->at(regSP);
    regA = memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;

    regPC++;
}

void z80cpu::di() noexcept {
    interruptsEnabled = false;
    regIFF1 = false;
    regIFF2 = false;

    regPC++;
}

void z80cpu::push_af() noexcept {
    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = regF;
    memory->at((regSP + 1) & 0xffff) = regA;

    regPC++;
}

void z80cpu::or_n() noexcept {
    regA |= memory->at((regPC + 1) & 0xffff);
    regF = (regA & 0xa8) | parityTable[regA];   // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                    // set Z flag

    regPC += 2;
}

void z80cpu::ld_sp_hl() noexcept {
    regSP = regL + (regH << 8);
    regPC++;
}

void z80cpu::ei() noexcept {
    interruptsEnabled = false;
    regIFF1 = true;
    regIFF2 = true;

    regPC++;
}

void z80cpu::prefix_fd() noexcept {
    regPC = (regPC + 1) & 0xffff;
    cpu_instructions_fd[static_cast<unsigned>(memory->at(regPC))](this);
}

void z80cpu::cp_n() noexcept {
    int cpval = memory->at((regPC + 1) & 0xffff);

    regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)    // H,C
           | ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2             // V,N
           | ((regA - cpval) & 0x80) | (cpval & 0x28);                                  // S,5,3 flags
    if (!(regA - cpval)) regF |= 0x40;                                                  // Z

    regPC += 2;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX ED
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// GENERIC
////////////////////////////////////////////////////////////////////////////////

void z80cpu::in_r8_atc() noexcept {
    regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

    int inval = (inputPorts[regC + (regB << 8)] | inputPortsShort[regC]) & 0xff;
    *regPtr[(memory->at(regPC) >> 3) & 7] = inval;
    // set flags
    regF = (regF & 1) | (inval & 0xa8) | ((~(inval ^ (inval >> 4) ^ (inval >> 2) ^ (inval >> 1)) & 1) << 2);

    regPC++;
}

void z80cpu::out_atc_r8() noexcept {
    regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

    if (cpuType == Z80Type::NMOS) {
        regDummy = 0;
    } else {
        regDummy = 0xff;
    }

    outputPorts[regC + (regB << 8)] = *regPtr[(memory->at(regPC) >> 3) & 7];
    outputPortsShort[regC] = *regPtr[(memory->at(regPC) >> 3) & 7];

    regPC++;
}

void z80cpu::adc_hl_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regL = regL + *reg + (regF & 1);

    regF = (((*(reg - 1) & 0xf) + (regH & 0xf) + ((regL & 0x100) >> 8)) & 0x10)
           | ((((regH ^ ~(*(reg - 1))) & (regH ^ (regH + *(reg - 1) + (regL >> 8)))) & 0x80) >> 5);     // H,V,N flags

    regH = *(reg - 1) + regH + (regL >> 8);

    regF = regF | (regH & 0xa8) | (regH >> 8);  // S,5,3,V,C flags

    regL &= 0xff;
    regH &= 0xff;

    if (!(regH | regL)) regF |= 0x40;           // set Z flag

    regPC++;
}

void z80cpu::sbc_hl_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regL = regL - *(reg + 1) - (regF & 1);

    regF = (((regH & 0xf) - (*reg & 0xf) - ((regL >> 8) & 1)) & 0x10) | 2
           | ((((regH ^ *reg) & (regH ^ (regH - *reg - ((regL >> 8) & 1)))) & 0x80) >> 5);  // H,V,N flags

    regH = regH - *reg - ((regL >> 8) & 1);

    regF = regF | (regH & 0xa8) | ((regH >> 8) & 1);    // S,5,3,C flags

    regL &= 0xff;
    regH &= 0xff;

    if (!(regH | regL)) regF |= 0x40;                   // set Z flag

    regPC++;
}

void z80cpu::ld_atnn_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    memory->at(addr) = *(reg + 1);
    memory->at(regMEMPTR) = *reg;

    regPC += 3;
}

void z80cpu::ld_r16_atnn() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    *reg = memory->at(addr);
    *(reg - 1) = memory->at(regMEMPTR);

    regPC += 3;
}

////////////////////////////////////////////////////////////////////////////////
// block ED 4x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::neg() noexcept {
    regF = (((((~regA) & 0xff) + 1) & 0x100) >> 8)
        | (((((~regA) & 0xf) + 1) & 0x10) >> 4) | (((((~regA) & 0x7f) + 1) & 0x80) >> 4);    // set H,V,C flags

    regA = (-(regA)) & 0xff;

    regF |= ((regA & 0xa8) | 2);    // set S,5,3,N flags
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

void z80cpu::retn() noexcept {
    regPC = memory->at(regSP) + memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;

    regIFF1 = regIFF2;
}

void z80cpu::im0() noexcept {
    interruptMode = 0;
    regPC++;
}

void z80cpu::ld_i_a() noexcept {
    regI = regA;
    regPC++;
}

void z80cpu::reti() noexcept {
    regPC = memory->at(regSP) + memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;
    regIFF1 = regIFF2;
}

void z80cpu::ld_r_a() noexcept {
    regR = regA;
    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block ED 5x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::im1() noexcept {
    interruptMode = 1;
    regPC++;
}

void z80cpu::ld_a_i() noexcept {
    regA = regI;

    regF = (regF & 1) | (regA & 0xa8);  // set S,Z,5,H,3,N flags
    if (!regA) regF |= 0x40;            // set Z flag
    if (regIFF2) regF |= 4;             // set P/V flag

    regPC++;
}

void z80cpu::im2() noexcept {
    interruptMode = 2;
    regPC++;
}

void z80cpu::ld_a_r() noexcept {
    regA = regR;

    regF = (regF & 1) | (regA & 0xa8);  // set S,Z,5,H,3,N flags
    if (!regA) regF |= 0x40;            // set Z flag
    if (regIFF2) regF |= 4;             // set P/V flag

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block ED 6x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::rrd() noexcept {
    regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

    int temp = regA & 0xf;

    regA = (regA & 0xf0) | (memory->at(regL + (regH << 8)) & 0xf);
    memory->at(regL + (regH << 8)) = ((memory->at(regL + (regH << 8)) >> 4) | (temp << 4)) & 0xff;

    regF = (regF & 1) | (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);     // set flags
}

void z80cpu::rld() noexcept {
    regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

    int temp = regA & 0xf;

    regA = (regA & 0xf0) | ((memory->at(regL + (regH << 8)) & 0xf0) >> 4);
    memory->at(regL + (regH << 8)) = ((memory->at(regL + (regH << 8)) << 4) | temp) & 0xff;

    regF = (regF & 1) | (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);     // set flags
}


////////////////////////////////////////////////////////////////////////////////
// block ED 7x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::sbc_hl_sp() noexcept {
    int sp_lo = regSP & 0xff;
    int sp_hi = regSP >> 8;

    regL = regL - sp_lo - (regF & 1);

    regF = (((regH & 0xf) - (sp_hi & 0xf) - ((regL >> 8) & 1)) & 0x10) | 2
           | ((((regH ^ sp_hi) & (regH ^ (regH - sp_hi - ((regL >> 8) & 1)))) & 0x80) >> 5);    // H,V,N flags

    regH = regH - sp_hi - ((regL >> 8) & 1);

    regF = regF | (regH & 0xa8) | ((regH >> 8) & 1);    // S,5,3,C flags

    regL &= 0xff;
    regH &= 0xff;

    if (!(regH | regL)) regF |= 0x40;                   // set Z flag

    regPC++;
}

void z80cpu::ld_atnn_sp() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    memory->at(addr) = regSP & 0xff;
    memory->at(regMEMPTR) = (regSP & 0xff00) >> 8;

    regPC += 3;
}

void z80cpu::adc_hl_sp() noexcept {
    int sp_lo = regSP & 0xff;
    int sp_hi = regSP >> 8;

    regL = regL + sp_lo + (regF & 1);

    regF = (((sp_hi & 0xf) + (regH & 0xf) + ((regL & 0x100) >> 8)) & 0x10)
           | ((((regH ^ ~(sp_hi)) & (regH ^ (regH + sp_hi + (regL >> 8)))) & 0x80) >> 5);   // H,V,N flags

    regH = sp_hi + regH + (regL >> 8);

    regF = regF | (regH & 0xa8) | (regH >> 8);  // S,5,3,V,C flags

    regL &= 0xff;
    regH &= 0xff;

    if (!(regH | regL)) regF |= 0x40;           // set Z flag

    regPC++;
}

void z80cpu::ld_sp_atnn() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    regSP = memory->at(addr) + (memory->at(regMEMPTR) << 8);

    regPC += 3;
}

////////////////////////////////////////////////////////////////////////////////
// block ED Ax
////////////////////////////////////////////////////////////////////////////////


void z80cpu::ldi() noexcept {
    int temp = regE + (regD << 8);
    int ftr = memory->at(regL + (regH << 8)) + regA;

    memory->at(temp) = memory->at(regL + (regH << 8));

    temp = (temp + 1) & 0xffff;
    regE = temp & 0xff;
    regD = temp >> 8;
    temp = regL + (regH << 8) + 1;
    regL = temp & 0xff;
    regH = (temp >> 8) & 0xff;

    temp = regC + (regB * 0x100) - 1;
    regC = temp & 0xff;
    regB = (temp >> 8) & 0xff;

    regF = (regF & 0xc1) | (ftr & 8) | ((ftr & 2) << 4);    // set 5,3 flags
    if (!(regC | regB)) regF |= 4;                          // set P/V flag

    regPC++;
}

void z80cpu::cpi() noexcept {
    int cp = (regA - memory->at(regL + (regH << 8))) & 0xff;

    regMEMPTR = (regMEMPTR + 1) & 0xffff;
    // S,H,N flags
    regF = (regF & 1) | (cp & 0x80) | (((regA & 0xf) - (memory->at(regL + (regH << 8)) & 0xf)) & 0x10) | 2;
    regF |= ((cp - regA - ((regF >> 4) & 1)) & 0x28);   // 5,3 flags
    if (!cp) regF |= 0x40;                              // Z flag

    regH = (regH + ((regL + 1) >> 8)) & 0xff;
    regL = (regL + 1) & 0xff;

    regB = (regB - ((regL + 1) >> 8)) & 0xff;
    regC = (regC - 1) & 0xff;

    if (!(regC | regB)) regF |= 4;                      // PV flag

    regPC++;
}

void z80cpu::ini() noexcept {
    regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

    int regHL = regL + (regH << 8);

    memory->at(regHL) = inputPorts[regC + (regB << 8)] | inputPortsShort[regC];

    int ftr = memory->at(regHL);

    regB = (regB - 1) & 0xff;
    regHL += 1;

    regL = regHL & 0xff;
    regH = (regHL >> 8) & 0xff;

    ftr = ftr + ((regC + 1) & 0x100);

    regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);    // set S,5,H,3,N,C flags
    if (!regB) regF &= 0xbf;                                        // set Z flag
    // TODO(utz): P/V flag

    regPC++;
}

void z80cpu::outi() noexcept {
    regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

    int regHL = regL + (regH << 8);
    int ftr = memory->at(regHL);

    outputPorts[regC + (regB << 8)] = memory->at(regHL);
    outputPortsShort[regC] = memory->at(regHL);

    regB = (regB - 1) & 0xff;
    regHL += 1;

    regL = regHL & 0xff;
    regH = (regHL >> 8) & 0xff;

    ftr = ftr + ((regC + 1) & 0x100);

    regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);    // set S,5,H,3,N,C flags
    if (!regB) regF &= 0xbf;                                        // set Z flag
    // TODO(utz): P/V flag

    regPC++;
}

void z80cpu::ldd() noexcept {
    int temp = regE + (regD * 0x100);
    int ftr = memory->at(regL + (regH << 8)) + regA;

    memory->at(temp) = memory->at(regL + (regH << 8));

    temp = (temp - 1) & 0xffff;
    regE = temp & 0xff;
    regD = temp >> 8;
    temp = regL + (regH << 8) - 1;
    regL = temp & 0xff;
    regH = (temp >> 8) & 0xff;

    temp = regC + (regB * 0x100) - 1;
    regC = temp & 0xff;
    regB = (temp >> 8) & 0xff;

    regF = (regF & 0xc1) | (ftr & 8) | ((ftr & 2) << 4);    // set 5,3 flags
    if (!(regC | regB)) regF |= 4;                          // set P/V flag

    regPC++;
}

void z80cpu::cpd() noexcept {
    int cp = (regA - memory->at(regL + (regH << 8))) & 0xff;

    regMEMPTR = (regMEMPTR - 1) & 0xffff;
    // S,H,N flags
    regF = (regF & 1) | (cp & 0x80) | (((regA & 0xf) - (memory->at(regL + (regH << 8)) & 0xf)) & 0x10) | 2;
    regF |= ((cp - regA - ((regF >> 4) & 1)) & 0x28);   // 5,3 flags
    if (!cp) regF |= 0x40;                              // Z flag

    regH = (regH - ((regL - 1) >> 8)) & 0xff;
    regL = (regL - 1) & 0xff;

    regB = (regB - ((regC - 1) >> 8)) & 0xff;
    regC = (regC - 1) & 0xff;


    if (!(regC | regB)) regF |= 4;                      // PV flag

    regPC++;
}

void z80cpu::ind() noexcept {
    regMEMPTR = (regC + (regB << 8) - 1) & 0xffff;

    int regHL = regL + (regH << 8);

    memory->at(regHL) = inputPorts[regC + (regB << 8)] | inputPortsShort[regC];

    int ftr = memory->at(regHL);

    regB = (regB - 1) & 0xff;
    regHL -= 1;

    regL = regHL & 0xff;
    regH = (regHL >> 8) & 0xff;

    ftr = ftr + ((regC - 1) & 0x100);

    regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);    // set S,5,H,3,N,C flags
    if (!regB) regF &= 0xbf;                                        // set Z flag
    // TODO(utz): P/V flag

    regPC++;
}

void z80cpu::outd() noexcept {
    regMEMPTR = (regC + (regB << 8) - 1) & 0xffff;

    int regHL = regL + (regH << 8);

    outputPorts[regC + (regB << 8)] = memory->at(regHL);
    outputPortsShort[regC] = memory->at(regHL);


    int ftr = memory->at(regHL);

    regB = (regB - 1) & 0xff;
    regHL -= 1;

    regL = regHL & 0xff;
    regH = (regHL >> 8) & 0xff;

    ftr = ftr + ((regC - 1) & 0x100);

    regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);    // set S,5,H,3,N,C flags
    if (!regB) regF &= 0xbf;                                        // set Z flag
    // TODO(utz): P/V flag

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// block ED Bx
////////////////////////////////////////////////////////////////////////////////


void z80cpu::ldir() noexcept {
    if ((regB | regC)) {
        ldi();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::cpir() noexcept {
    if ((regB | regC)) {
        cpi();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::inir() noexcept {
    if (regB) {
        ini();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::otir() noexcept {
    if (regB) {
        outi();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::lddr() noexcept {
    if ((regB | regC)) {
        ldd();
        regPC = (regPC - 2) & 0xffff;
    } else {
         regPC++;
    }
}

void z80cpu::cpdr() noexcept {
    if ((regB | regC)) {
        cpd();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::indr() noexcept {
    if (regB) {
        ind();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}

void z80cpu::otdr() noexcept {
    if (regB) {
        outd();
        regPC = (regPC - 2) & 0xffff;
    } else {
        regPC++;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX CB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CB 0x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::rlc_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    *reg = *reg << 1;
    *reg = (*reg | (*reg >> 8)) & 0xff;

    regF = (*reg & 0xa9) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);      // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;        // Z
}

void z80cpu::rlc_athl() noexcept {
    (memory->at(regL + (regH << 8))) = (memory->at(regL + (regH << 8))) << 1;
    (memory->at(regL + (regH << 8))) = ((memory->at(regL + (regH << 8)))
        | ((memory->at(regL + (regH << 8))) >> 8)) & 0xff;

    // S,5,H,3,P,N,C
    regF = ((memory->at(regL + (regH << 8))) & 0xa9)
           | ((~((memory->at(regL + (regH << 8))) ^ ((memory->at(regL + (regH << 8))) >> 4)
           ^ ((memory->at(regL + (regH << 8))) >> 2) ^ ((memory->at(regL + (regH << 8))) >> 1)) & 1) << 2);
    if (!(memory->at(regL + (regH << 8)))) regF |= 0x40;        // Z
}

void z80cpu::rrc_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    int oldcarry = *reg & 1;
    *reg = *reg >> 1;
    *reg |= (oldcarry << 8);

    regF = (*reg & 0xa8) | oldcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);   // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;        // Z
}

void z80cpu::rrc_athl() noexcept {
    int oldcarry = memory->at(regL + (regH << 8)) & 1;
    memory->at(regL + (regH << 8)) = memory->at(regL + (regH << 8)) >> 1;
    memory->at(regL + (regH << 8)) |= (oldcarry << 8);

    regF = (memory->at(regL + (regH << 8)) & 0xa8) | oldcarry
        | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
        ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);    // S,5,H,3,P,N,C
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;      // Z
}


////////////////////////////////////////////////////////////////////////////////
// CB 1x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::rl_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    int oldcarry = regF & 1;

    *reg = ((*reg << 1) | oldcarry);

    oldcarry = *reg >> 8;
    *reg &= 0xff;

    regF = (*reg & 0xa8) | oldcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);   // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;        // Z
}

void z80cpu::rl_athl() noexcept {
    int oldcarry = regF & 1;

    memory->at(regL + (regH << 8)) = ((memory->at(regL + (regH << 8)) << 1) | oldcarry);

    oldcarry = memory->at(regL + (regH << 8)) >> 8;
    memory->at(regL + (regH << 8)) &= 0xff;
    // S,5,H,3,P,N,C
    regF = (memory->at(regL + (regH << 8)) & 0xa8) | oldcarry
           | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
           ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;  // Z
}

void z80cpu::rr_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    int newcarry = *reg & 1;

    *reg = ((*reg >> 1) | ((regF & 1) << 7));

    regF = (*reg & 0xa8) | newcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);   // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;    // Z
}

void z80cpu::rr_athl() noexcept {
    int newcarry = memory->at(regL + (regH << 8)) & 1;

    memory->at(regL + (regH << 8)) = ((memory->at(regL + (regH << 8)) >> 1) | ((regF & 1) << 7));

    regF = (memory->at(regL + (regH << 8)) & 0xa8) | newcarry
        | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
        ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);    // S,5,H,3,P,N,C
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;      // Z
}


////////////////////////////////////////////////////////////////////////////////
// CB 2x/CB 3x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::slx_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    regF = *reg >> 7;

    *reg = ((*reg << 1) | ((memory->at(regPC) >> 4) & 1)) & 0xff;

    regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);       // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;        // Z
}

void z80cpu::slx_athl() noexcept {
    regF = memory->at(regL + (regH << 8)) >> 7;

    memory->at(regL + (regH << 8)) = ((memory->at(regL + (regH << 8)) << 1) | ((memory->at(regPC) >> 4) & 1)) & 0xff;

    regF = regF | (memory->at(regL + (regH << 8)) & 0xa8)
        | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
        ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);    // S,5,H,3,P,N,C
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;      // Z
}

void z80cpu::sra_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    regF = *reg & 1;

    *reg = (*reg & 0x80) | (*reg >> 1);

    regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);       // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;                // Z
}

void z80cpu::sra_athl() noexcept {
    regF = memory->at(regL + (regH << 8)) & 1;

    memory->at(regL + (regH << 8)) = (memory->at(regL + (regH << 8)) & 0x80) | (memory->at(regL + (regH << 8)) >> 1);

    regF = regF | (memory->at(regL + (regH << 8)) & 0xa8)
        | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
        ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);    // S,5,H,3,P,N,C
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;      // Z
}


void z80cpu::srl_r8() noexcept {
    int *reg = regPtr[memory->at(regPC) & 0x7];

    regF = *reg & 1;

    *reg = (*reg >> 1);

    regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);       // S,5,H,3,P,N,C
    if (!*reg) regF |= 0x40;        // Z
}

void z80cpu::srl_athl() noexcept {
    regF = memory->at(regL + (regH << 8)) & 1;

    memory->at(regL + (regH << 8)) = (memory->at(regL + (regH << 8)) >> 1);

    regF = regF | (memory->at(regL + (regH << 8)) & 0xa8)
        | ((~(memory->at(regL + (regH << 8)) ^ (memory->at(regL + (regH << 8)) >> 4)
        ^ (memory->at(regL + (regH << 8)) >> 2) ^ (memory->at(regL + (regH << 8)) >> 1)) & 1) << 2);    // S,5,H,3,P,N,C
    if (!memory->at(regL + (regH << 8))) regF |= 0x40;      // Z
}


////////////////////////////////////////////////////////////////////////////////
// CB 4x..7x (BIT)
////////////////////////////////////////////////////////////////////////////////

void z80cpu::bit_x_r8() noexcept {
    regF = (regF & 1) | 0x10;       // set H flag
    // set Z,PV flags
    if (!(*regPtr[memory->at(regPC) & 0x7] & (1 << ((memory->at(regPC) - 0x40) >> 3)))) {
        regF |= 0x44;
    } else {
        if (((memory->at(regPC) - 0x40) >> 3) == 7) regF |= 0x80;
        else if (((memory->at(regPC) - 0x40) >> 3) == 5) regF |= 0x20;
        else if (((memory->at(regPC) - 0x40) >> 3) == 3) regF |= 0x8;
    }
}

void z80cpu::bit_x_athl() noexcept {
    regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);           // set 5,H,3, preserve C
    // set Z,PV flags
    if (!(memory->at(regL + (regH << 8)) & (1 << ((memory->at(regPC) - 0x40) >> 3)))) regF |= 0x44;
    else if (((memory->at(regPC) - 0x40) >> 3) == 7) regF |= 0x80;
}


////////////////////////////////////////////////////////////////////////////////
// CB 8x..Bx (RES)
////////////////////////////////////////////////////////////////////////////////

void z80cpu::res_x_r8() noexcept {
    *regPtr[memory->at(regPC) & 0x7] &= (((1 << ((memory->at(regPC) - 0x80) >> 3)) ^ 0xff) & 0xff);
}

void z80cpu::res_x_athl() noexcept {
    memory->at(regL + (regH << 8)) &= (((1 << ((memory->at(regPC) - 0x80) >> 3)) ^ 0xff) & 0xff);
}


////////////////////////////////////////////////////////////////////////////////
// CB Cx..Fx (SET)
////////////////////////////////////////////////////////////////////////////////

void z80cpu::set_x_r8() noexcept {
    *regPtr[memory->at(regPC) & 0x7] |= (1 << ((memory->at(regPC) - 0xc0) >> 3));
}

void z80cpu::set_x_athl() noexcept {
    memory->at(regL + (regH << 8)) |= (1 << ((memory->at(regPC) - 0xc0) >> 3));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX DD
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// GENERIC
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_r8_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    *regPtr[(memory->at(regPC) >> 3) & 0x7] = memory->at(regMEMPTR);

    regPC += 2;
}

void z80cpu::ld_r8_ixh() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = regIXH;
    regPC++;
}

void z80cpu::ld_r8_ixl() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = regIXL;
    regPC++;
}

void z80cpu::add_ix_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

    regIXL += *reg;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);

    regIXH += ((regIXL >> 8) + (*(reg - 1)));
    regIXL &= 0xff;

    regF = regF | (regIXH & 0x28) | (regIXH >> 8);          // set 5,3,C

    regIXH &= 0xff;

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// DD 2x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_ix_nn() noexcept {
    regIXL = memory->at((regPC + 1) & 0xffff);
    regIXH = memory->at((regPC + 2) & 0xffff);
    regPC += 3;
}

void z80cpu::ld_atnn_ix() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);
    regMEMPTR = (addr + 1) & 0xffff;

    memory->at(addr) = regIXL;
    memory->at(regMEMPTR) = regIXH;

    regPC += 3;
}

void z80cpu::inc_ix() noexcept {
    regIXL++;

    regIXH = (regIXH + (regIXL >> 8)) & 0xff;
    regIXL &= 0xff;

    regPC++;
}

void z80cpu::inc_ixh() noexcept {
    regF = (regF & 1) | (((regIXH & 0xf) + 1) & 0x10);      // H,N,(C)

    regIXH = (regIXH + 1) & 0xff;

    regF |= (regIXH & 0xa8);                                // S,5,3
    if (regIXH == 0x80) regF |= 4;                          // V
    if (!regIXH) regF |= 0x40;                              // Z

    regPC++;
}

void z80cpu::dec_ixh() noexcept {
    regF = (regF & 1) | (((regIXH & 0xf) - 1) & 0x10) | 2;  // H,N,(C)
    if (regIXH == 0x80) regF |= 4;                          // V

    regIXH = (regIXH - 1) & 0xff;

    regF |= (regIXH & 0xa8);                                // S,5,3
    if (!regIXH) regF |= 0x40;                              // Z

    regPC++;
}

void z80cpu::ld_ixh_n() noexcept {
    regIXH = memory->at((regPC + 1) & 0xffff);
    regPC += 2;
}

void z80cpu::add_ix_ix() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

    int temp = regIXL;
    regIXL += regIXL;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + (temp & 0xf)) & 0x10);

    regIXH += ((regIXL >> 8) + regIXH);
    regIXL &= 0xff;

    regF = regF | (regIXH & 0x28) | (regIXH >> 8);              // set 5,3,C

    regIXH &= 0xff;

    regPC++;
}

void z80cpu::ld_ix_atnn() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    regIXL = memory->at(addr);
    regIXH = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::dec_ix() noexcept {
    regIXL--;
    regIXH -= (regIXL >> 8);
    regIXL &= 0xff;
    regIXH &= 0xff;

    regPC++;
}

void z80cpu::inc_ixl() noexcept {
    regF = (regF & 1) | (((regIXL & 0xf) + 1) & 0x10);      // H,N,(C)

    regIXL = (regIXL + 1) & 0xff;

    regF |= (regIXL & 0xa8);                                // S,5,3
    if (regIXL == 0x80) regF |= 4;                          // V
    if (!regIXL) regF |= 0x40;                              // Z

    regPC++;
}

void z80cpu::dec_ixl() noexcept {
    regF = (regF & 1) | (((regIXL & 0xf) - 1) & 0x10) | 2;  // H,N,(C)
    if (regIXL == 0x80) regF |= 4;                          // V

    regIXL = (regIXL - 1) & 0xff;

    regF |= (regIXL & 0xa8);                                // S,5,3
    if (!regIXL) regF |= 0x40;                              // Z

    regPC++;
}

void z80cpu::ld_ixl_n() noexcept {
    regIXL = memory->at((regPC + 1) & 0xffff);
    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// DD 3x
////////////////////////////////////////////////////////////////////////////////


void z80cpu::inc_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | (((memory->at(regMEMPTR) & 0xf) + 1) & 0x10);   // H,N,(C)

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) + 1) & 0xff;

    regF |= (memory->at(regMEMPTR) & 0xa8);                             // S,5,3
    if (memory->at(regMEMPTR) == 0x80) regF |= 4;                       // V
    if (!memory->at(regMEMPTR)) regF |= 0x40;                           // Z

    regPC += 2;
}

void z80cpu::dec_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | (((memory->at(regMEMPTR) & 0xf) - 1) & 0x10) | 2;   // H,N,(C)
    if (memory->at(regMEMPTR) == 0x80) regF |= 4;                           // V

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) - 1) & 0xff;

    regF |= (memory->at(regMEMPTR) & 0xa8);                                 // S,5,3
    if (!memory->at(regMEMPTR)) regF |= 0x40;                               // Z

    regPC += 2;
}

void z80cpu::ld_atixpd_n() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = memory->at((regPC + 1) & 0xffff);

    regPC += 2;
}

void z80cpu::add_ix_sp() noexcept {
    int sp_lo = regSP & 0xff;
    int sp_hi = regSP >> 8;

    regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

    regIXL += sp_lo;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + (sp_hi & 0xf)) & 0x10);

    regIXH += (regIXL >> 8) + sp_hi;
    regIXL &= 0xff;

    regF = regF | (regIXH & 0x28) | (regIXH >> 8);      // set 5,3,C

    regIXH &= 0xff;

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// DD 6x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_ixh_r8() noexcept {
    regIXH = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::ld_ixh_ixl() noexcept {
    regIXH = regIXL;
    regPC++;
}

void z80cpu::ld_ixl_r8() noexcept {
    regIXL = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::ld_ixl_ixh() noexcept {
    regIXL = regIXH;
    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// DD 7x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_atixpd_r8() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = *regPtr[memory->at(regPC) & 0x7];

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// DD 8x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::adx_a_ixh() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA + regIXH + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIXH & 0xf) + oldcarry) & 0x10)
        | ((((regA ^ ~regIXH) & (regA ^ (regA + regIXH + oldcarry))) & 0x80) >> 5);     // set H,V,C flags

    regA = (regA + regIXH + oldcarry) & 0xff;

    regF |= (regA & 0xa8);              // set S,5,3 flag
    if (!regA) regF |= 0x40;            // set Z flag

    regPC++;
}


void z80cpu::adx_a_ixl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA + regIXL + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIXL & 0xf) + oldcarry) & 0x10)
        | ((((regA ^ ~regIXL) & (regA ^ (regA + regIXL + oldcarry))) & 0x80) >> 5);     // set H,V,C flags

    regA = (regA + regIXL + oldcarry) & 0xff;

    regF |= (regA & 0xa8);              // set S,5,3 flag
    if (!regA) regF |= 0x40;            // set Z flag

    regPC++;
}

void z80cpu::adx_a_atixpd() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int addval = memory->at(regMEMPTR);

    regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~addval) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);      // set H,V,C flags

    regA = (regA + addval + oldcarry) & 0xff;

    regF |= (regA & 0xa8);              // set S,5,3 flag
    if (!regA) regF |= 0x40;            // set Z flag

    regPC += 2;
}

////////////////////////////////////////////////////////////////////////////////
// DD 9x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::sbx_a_ixh() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA - regIXH - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIXH & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ regIXH) & (regA ^ (regA - regIXH - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - regIXH - oldcarry) & 0xff;

    regF |= (regA & 0xa8);              // set S,5,3 flags
    if (!regA) regF |= 0x40;            // set Z flag

    regPC++;
}

void z80cpu::sbx_a_ixl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA - regIXL - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIXL & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ regIXL) & (regA ^ (regA - regIXL - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - regIXL - oldcarry) & 0xff;

    regF |= (regA & 0xa8);          // set S,5,3 flags
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

void z80cpu::sbx_a_atixpd() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int subval = memory->at(regMEMPTR);

    regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - subval - oldcarry) & 0xff;

    regF |= (regA & 0xa8);              // set S,5,3 flags
    if (!regA) regF |= 0x40;

    regPC += 2;
}

////////////////////////////////////////////////////////////////////////////////
// DD Ax
////////////////////////////////////////////////////////////////////////////////

void z80cpu::and_ixh() noexcept {
    regA &= regIXH;
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC++;
}

void z80cpu::and_ixl() noexcept {
    regA &= regIXL;
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC++;
}

void z80cpu::and_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA &= memory->at(regMEMPTR);
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;    // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                            // set Z flag

    regPC += 2;
}

void z80cpu::xor_ixh() noexcept {
    regA = (regA ^ regIXH) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::xor_ixl() noexcept {
    regA = (regA ^ regIXL) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::xor_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA = (regA ^ memory->at(regMEMPTR)) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// DD Bx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::or_ixh() noexcept {
    regA = (regA | regIXH) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::or_ixl() noexcept {
    regA = (regA | regIXL) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::or_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA = (regA | memory->at(regMEMPTR)) & 0xff;
    regF = (regA & 0xa8) | parityTable[regA];       // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                        // set Z flag

    regPC++;
}

void z80cpu::cp_ixh() noexcept {
    regF = (((regA - regIXH) & 0x100) >> 8) | (((regA & 0xf) - (regIXH & 0xf)) & 0x10)  // H,C
        | ((((regA ^ regIXH) & (regA ^ (regA - regIXH))) & 0x80) >> 5) | 2              // V,N
        | ((regA - regIXH) & 0x80) | (regIXH & 0x28);                                   // S,5,3 flags
    if (!(regA - regIXH)) regF |= 0x40;                                                 // Z

    regPC++;
}

void z80cpu::cp_ixl() noexcept {
    regF = (((regA - regIXL) & 0x100) >> 8) | (((regA & 0xf) - (regIXL & 0xf)) & 0x10)  // H,C
        | ((((regA ^ regIXL) & (regA ^ (regA - regIXL))) & 0x80) >> 5) | 2              // V,N
        | ((regA - regIXL) & 0x80) | (regIXL & 0x28);                                   // S,5,3 flags
    if (!(regA - regIXL)) regF |= 0x40;                                                 // Z

    regPC++;
}

void z80cpu::cp_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int cpval = memory->at(regMEMPTR);

    regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)    // H,C
        | ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2                // V,N
        | ((regA - cpval) & 0x80) | (cpval & 0x28);                                     // S,5,3 flags
    if (!(regA - cpval)) regF |= 0x40;                                                  // Z

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// DD Cx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::prefix_ddcb() noexcept {
    if (memory->at((regPC + 2) & 0xffff) >= 0xc0) {
        set_x_atixpd();
    } else if (memory->at((regPC + 2) & 0xffff) >= 0x80) {
        res_x_atixpd();
    } else if (memory->at((regPC + 2) & 0xffff) >= 0x40) {
        bit_x_atixpd();
    } else {
        cpu_instructions_ddcb[static_cast<unsigned>(memory->at((regPC + 2) & 0xffff) >> 3)](this);
    }
}


////////////////////////////////////////////////////////////////////////////////
// DD Ex
////////////////////////////////////////////////////////////////////////////////

void z80cpu::pop_ix() noexcept {
    regIXL = memory->at(regSP);
    regIXH = memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;

    regPC++;
}

void z80cpu::ex_atsp_ix() noexcept {
    int temp = regIXL;
    regIXL = memory->at(regSP);
    memory->at(regSP) = temp;

    temp = regIXH;
    regIXH = memory->at((regSP + 1) & 0xffff);
    memory->at((regSP + 1) & 0xffff) = temp;

    regPC++;
}

void z80cpu::push_ix() noexcept {
    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = regIXL;
    memory->at((regSP + 1) & 0xffff) = regIXH;

    regPC++;
}

void z80cpu::jp_atix() noexcept {
    regPC = regIXL + (regIXH << 8);
}


////////////////////////////////////////////////////////////////////////////////
// DD Fx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_sp_ix() noexcept {
    regSP = regIXL + (regIXH << 8);
    regPC++;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX FD
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// GENERIC
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_r8_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    *regPtr[(memory->at(regPC) >> 3) & 0x7] = memory->at(regMEMPTR);

    regPC += 2;
}


void z80cpu::ld_r8_iyh() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = regIYH;

    regPC++;
}

void z80cpu::ld_r8_iyl() noexcept {
    *regPtr[(memory->at(regPC) >> 3) & 0x7] = regIYL;

    regPC++;
}

void z80cpu::add_iy_r16() noexcept {
    int *reg = regPtr[(memory->at(regPC) >> 3) & 0x7];

    regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

    regIYL += *reg;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);

    regIYH += ((regIYL >> 8) + (*(reg - 1)));
    regIYL &= 0xff;

    regF = regF | (regIYH & 0x28) | (regIYH >> 8);      // set 5,3,C

    regIYH &= 0xff;

    regPC++;
}

////////////////////////////////////////////////////////////////////////////////
// FD 2x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_iy_nn() noexcept {
    regIYL = memory->at((regPC + 1) & 0xffff);
    regIYH = memory->at((regPC + 2) & 0xffff);
    regPC += 3;
}

void z80cpu::ld_atnn_iy() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    memory->at(addr) = regIYL;
    memory->at(regMEMPTR) = regIYH;

    regPC += 3;
}

void z80cpu::inc_iy() noexcept {
    regIYL++;
    regIYH += (regIYL >> 8);
    regIYL &= 0xff;
    regIYH &= 0xff;

    regPC++;
}

void z80cpu::inc_iyh() noexcept {
    regF = (regF & 1) | (((regIYH & 0xf) + 1) & 0x10);      // H,N,(C)

    regIYH = (regIYH + 1) & 0xff;

    regF |= (regIYH & 0xa8);                // S,5,3
    if (regIYH == 0x80) regF |= 4;          // V
    if (!regIYH) regF |= 0x40;              // Z

    regPC++;
}

void z80cpu::dec_iyh() noexcept {
    regF = (regF & 1) | (((regIYH & 0xf) - 1) & 0x10) | 2;      // H,N,(C)
    if (regIYH == 0x80) regF |= 4;          // V

    regIYH = (regIYH - 1) & 0xff;

    regF |= (regIYH & 0xa8);                // S,5,3
    if (!regIYH) regF |= 0x40;              // Z

    regPC++;
}

void z80cpu::ld_iyh_n() noexcept {
    regIYH = memory->at((regPC + 1) & 0xffff);
    regPC += 2;
}

void z80cpu::add_iy_iy() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

    int temp = regIYL;
    regIYL += regIYL;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + (temp & 0xf)) & 0x10);

    regIYH += ((regIYL >> 8) + regIYH);
    regIYL &= 0xff;

    regF = regF | (regIYH & 0x28) | (regIYH >> 8);      // set 5,3,C

    regIYH &= 0xff;

    regPC++;
}

void z80cpu::ld_iy_atnn() noexcept {
    int addr = memory->at((regPC + 1) & 0xffff) + (memory->at((regPC + 2) & 0xffff) << 8);

    regMEMPTR = (addr + 1) & 0xffff;
    regIYL = memory->at(addr);
    regIYH = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::dec_iy() noexcept {
    regIYL--;
    regIYH -= (regIYL >> 8);
    regIYL &= 0xff;
    regIYH &= 0xff;

    regPC++;
}

void z80cpu::inc_iyl() noexcept {
    regF = (regF & 1) | (((regIYL & 0xf) + 1) & 0x10);      // H,N,(C)

    regIYL = (regIYL + 1) & 0xff;

    regF |= (regIYL & 0xa8);                // S,5,3
    if (regIYL == 0x80) regF |= 4;          // V
    if (!regIYL) regF |= 0x40;              // Z

    regPC++;
}

void z80cpu::dec_iyl() noexcept {
    regF = (regF & 1) | (((regIYL & 0xf) - 1) & 0x10) | 2;      // H,N,(C)
    if (regIYL == 0x80) regF |= 4;          // V

    regIYL = (regIYL - 1) & 0xff;

    regF |= (regIYL & 0xa8);                // S,5,3
    if (!regIYL) regF |= 0x40;              // Z

    regPC++;
}

void z80cpu::ld_iyl_n() noexcept {
    regIYL = memory->at((regPC + 1) & 0xffff);
    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// FD 3x
////////////////////////////////////////////////////////////////////////////////


void z80cpu::inc_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | (((memory->at(regMEMPTR) & 0xf) + 1) & 0x10);   // H,N,(C)

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) + 1) & 0xff;

    regF |= (memory->at(regMEMPTR) & 0xa8);             // S,5,3
    if (memory->at(regMEMPTR) == 0x80) regF |= 4;       // V
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    regPC += 2;
}

void z80cpu::dec_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | (((memory->at(regMEMPTR) & 0xf) - 1) & 0x10) | 2;   // H,N,(C)
    if (memory->at(regMEMPTR) == 0x80) regF |= 4;       // V

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) - 1) & 0xff;

    regF |= (memory->at(regMEMPTR) & 0xa8);             // S,5,3
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    regPC += 2;
}

void z80cpu::ld_atiypd_n() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = memory->at((regPC + 1) & 0xffff);

    regPC += 2;
}

void z80cpu::add_iy_sp() noexcept {
    int sp_lo = regSP & 0xff;
    int sp_hi = regSP >> 8;

    regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

    regIYL += sp_lo;
    // clear 5,3,N; preserve S,Z,PV; set H
    regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + (sp_hi & 0xf)) & 0x10);

    regIYH += (regIYL >> 8) + sp_hi;
    regIYL &= 0xff;

    regF = regF | (regIYH & 0x28) | (regIYH >> 8);      // set 5,3,C

    regIYH &= 0xff;

    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// FD 6x
////////////////////////////////////////////////////////////////////////////////


void z80cpu::ld_iyh_r8() noexcept {
    regIYH = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::ld_iyh_iyl() noexcept {
    regIYH = regIYL;
    regPC++;
}

void z80cpu::ld_iyl_r8() noexcept {
    regIYL = *regPtr[memory->at(regPC) & 0x7];
    regPC++;
}

void z80cpu::ld_iyl_iyh() noexcept {
    regIYL = regIYH;
    regPC++;
}


////////////////////////////////////////////////////////////////////////////////
// FD 7x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_atiypd_r8() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = *regPtr[memory->at(regPC) & 0x7];

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// FD 8x
////////////////////////////////////////////////////////////////////////////////


void z80cpu::adx_a_iyh() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA + regIYH + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIYH & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~regIYH) & (regA ^ (regA + regIYH + oldcarry))) & 0x80) >> 5);      // set H,V,C flags

    regA = (regA + regIYH + oldcarry) & 0xff;

    regF |= (regA & 0xa8);                  // set S,5,3 flag
    if (!regA) regF |= 0x40;                // set Z flag

    regPC++;
}


void z80cpu::adx_a_iyl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA + regIYL + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIYL & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~regIYL) & (regA ^ (regA + regIYL + oldcarry))) & 0x80) >> 5);      // set H,V,C flags

    regA = (regA + regIYL + oldcarry) & 0xff;

    regF |= (regA & 0xa8);                  // set S,5,3 flag
    if (!regA) regF |= 0x40;                // set Z flag

    regPC++;
}

void z80cpu::adx_a_atiypd() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int addval = memory->at(regMEMPTR);

    regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
           | ((((regA ^ ~addval) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);      // set H,V,C flags

    regA = (regA + addval + oldcarry) & 0xff;

    regF |= (regA & 0xa8);                  // set S,5,3 flag
    if (!regA) regF |= 0x40;                // set Z flag

    regPC += 2;
}



////////////////////////////////////////////////////////////////////////////////
// FD 9x
////////////////////////////////////////////////////////////////////////////////

void z80cpu::sbx_a_iyh() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA - regIYH - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIYH & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ regIYH) & (regA ^ (regA - regIYH - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - regIYH - oldcarry) & 0xff;

    regF |= (regA & 0xa8);          // set S,5,3 flags
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

void z80cpu::sbx_a_iyl() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regF = (((regA - regIYL - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIYL & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ regIYL) & (regA ^ (regA - regIYL - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - regIYL - oldcarry) & 0xff;

    regF |= (regA & 0xa8);          // set S,5,3 flags
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

void z80cpu::sbx_a_atiypd() noexcept {
    int oldcarry = (regF & 1) & (memory->at(regPC) >> 3);

    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int subval = memory->at(regMEMPTR);

    regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
           | ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;       // set H,V,C flags

    regA = (regA - subval - oldcarry) & 0xff;

    regF |= (regA & 0xa8);                          // set S,5,3 flags
    if (!regA) regF |= 0x40;

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// FD Ax
////////////////////////////////////////////////////////////////////////////////

void z80cpu::and_iyh() noexcept {
    regA &= regIYH;
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;        // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                // set Z flag

    regPC++;
}

void z80cpu::and_iyl() noexcept {
    regA &= regIYL;
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;        // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                // set Z flag

    regPC++;
}

void z80cpu::and_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA &= memory->at(regMEMPTR);
    regF = (regA & 0xa8) | parityTable[regA] | 0x10;        // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                // set Z flag

    regPC += 2;
}

void z80cpu::xor_iyh() noexcept {
    regA = (regA ^ regIYH) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                                                // set Z flag

    regPC++;
}

void z80cpu::xor_iyl() noexcept {
    regA = (regA ^ regIYL) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                                                // set Z flag

    regPC++;
}

void z80cpu::xor_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA = (regA ^ memory->at(regMEMPTR)) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                                                // set Z flag

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// FD Bx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::or_iyh() noexcept {
    regA = (regA | regIYH) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;        // set Z flag

    regPC++;
}

void z80cpu::or_iyl() noexcept {
    regA = (regA | regIYL) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                                                // set Z flag

    regPC++;
}

void z80cpu::or_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regA = (regA | memory->at(regMEMPTR)) & 0xff;
    regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);  // set S,5,H,3,P,N,C flags
    if (!regA) regF |= 0x40;                                                                // set Z flag

    regPC++;
}

void z80cpu::cp_iyh() noexcept {
    regF = (((regA - regIYH) & 0x100) >> 8) | (((regA & 0xf) - (regIYH & 0xf)) & 0x10)      // H,C
           | ((((regA ^ regIYH) & (regA ^ (regA - regIYH))) & 0x80) >> 5) | 2               // V,N
           | ((regA - regIYH) & 0x80) | (regIYH & 0x28);                                    // S,5,3 flags
    if (!(regA - regIYH)) regF |= 0x40;                                                     // Z

    regPC++;
}

void z80cpu::cp_iyl() noexcept {
    regF = (((regA - regIYL) & 0x100) >> 8) | (((regA & 0xf) - (regIYL & 0xf)) & 0x10)      // H,C
           | ((((regA ^ regIYL) & (regA ^ (regA - regIYL))) & 0x80) >> 5) | 2               // V,N
           | ((regA - regIYL) & 0x80) | (regIYL & 0x28);                                    // S,5,3 flags
    if (!(regA - regIYL)) regF |= 0x40;                                                     // Z

    regPC++;
}

void z80cpu::cp_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int cpval = memory->at(regMEMPTR);

    regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)        // H,C
           | ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2                 // V,N
           | ((regA - cpval) & 0x80) | (cpval & 0x28);                                      // S,5,3 flags
    if (!(regA - cpval)) regF |= 0x40;                                                      // Z

    regPC += 2;
}


////////////////////////////////////////////////////////////////////////////////
// FD Cx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::prefix_fdcb() noexcept {
    if (memory->at((regPC + 2) & 0xffff) >= 0xc0) {
        set_x_atiypd();
    } else if (memory->at((regPC + 2) & 0xffff) >= 0x80) {
        res_x_atiypd();
    } else if (memory->at((regPC + 2) & 0xffff) >= 0x40) {
        bit_x_atiypd();
    } else {
        cpu_instructions_fdcb[static_cast<unsigned>(memory->at((regPC + 2) & 0xffff) >> 3)](this);
    }
}


////////////////////////////////////////////////////////////////////////////////
// FD Ex
////////////////////////////////////////////////////////////////////////////////

void z80cpu::pop_iy() noexcept {
    regIYL = memory->at(regSP);
    regIYH = memory->at((regSP + 1) & 0xffff);
    regSP = (regSP + 2) & 0xffff;

    regPC++;
}

void z80cpu::ex_atsp_iy() noexcept {
    int temp = regIYL;
    regIYL = memory->at(regSP);
    memory->at(regSP) = temp;

    temp = regIYH;
    regIYH = memory->at((regSP + 1) & 0xffff);
    memory->at((regSP + 1) & 0xffff) = temp;

    regPC++;
}

void z80cpu::push_iy() noexcept {
    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = regIYL;
    memory->at((regSP + 1) & 0xffff) = regIYH;

    regPC++;
}

void z80cpu::jp_atiy() noexcept {
    regPC = regIYL + (regIYH << 8);
}


////////////////////////////////////////////////////////////////////////////////
// FD Fx
////////////////////////////////////////////////////////////////////////////////

void z80cpu::ld_sp_iy() noexcept {
    regSP = regIYL + (regIYH << 8);
    regPC++;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX DDCB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void z80cpu::rlc_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = memory->at(regMEMPTR) << 1;
    memory->at(regMEMPTR) = (memory->at(regMEMPTR) | (memory->at(regMEMPTR) >> 8)) & 0xff;

    regF = (memory->at(regMEMPTR) & 0xa9)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rrc_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int oldcarry = memory->at(regMEMPTR) & 1;
    memory->at(regMEMPTR) = memory->at(regMEMPTR) >> 1;
    memory->at(regMEMPTR) |= (oldcarry << 8);

    regF = (memory->at(regMEMPTR) & 0xa8) | oldcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 1) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rl_atixpd() noexcept {
    int oldcarry = regF & 1;
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) << 1) | oldcarry);

    oldcarry = memory->at(regMEMPTR) >> 8;
    memory->at(regMEMPTR) &= 0xff;

    regF = (memory->at(regMEMPTR) & 0xa8) | oldcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rr_atixpd() noexcept {
    int newcarry = memory->at(regMEMPTR) & 1;
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) >> 1) | ((regF & 1) << 7));

    regF = (memory->at(regMEMPTR) & 0xa8) | newcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sla_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) >> 7;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) << 1) & 0xff;

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sra_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) & 1;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) & 0x80) | (memory->at(regMEMPTR) >> 1);

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sll_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
    | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) >> 7;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) << 1) | 1) & 0xff;

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
           | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
           ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);      // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;               // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::srl_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) & 1;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) >> 1);

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::bit_x_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);       // set 5,H,3, preserve C
    // set Z,PV flags
    if (!(memory->at(regMEMPTR) & (1 << ((memory->at((regPC + 2) & 0xffff) - 0x40) >> 3)))) regF |= 0x44;
    else if (((memory->at((regPC + 2) & 0xffff) - 0x40) >> 3) == 7) regF |= 0x80;

    regPC += 3;
}

void z80cpu::res_x_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) &= (((1 << ((memory->at((regPC + 2) & 0xffff) - 0x80) >> 3)) ^ 0xff) & 0xff);

    regPC += 3;
}

void z80cpu::set_x_atixpd() noexcept {
    regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) |= (1 << ((memory->at((regPC + 2) & 0xffff) - 0xc0) >> 3));

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX FDCB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


void z80cpu::rlc_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = memory->at(regMEMPTR) << 1;
    memory->at(regMEMPTR) = (memory->at(regMEMPTR) | (memory->at(regMEMPTR) >> 8)) & 0xff;

    regF = (memory->at(regMEMPTR) & 0xa9)
           | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
           ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);  // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rrc_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    int oldcarry = memory->at(regMEMPTR) & 1;
    memory->at(regMEMPTR) = memory->at(regMEMPTR) >> 1;
    memory->at(regMEMPTR) |= (oldcarry << 8);

    regF = (memory->at(regMEMPTR) & 0xa8) | oldcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 1) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rl_atiypd() noexcept {
    int oldcarry = regF & 1;
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) << 1) | oldcarry);

    oldcarry = memory->at(regMEMPTR) >> 8;
    memory->at(regMEMPTR) &= 0xff;

    regF = (memory->at(regMEMPTR) & 0xa8) | oldcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::rr_atiypd() noexcept {
    int newcarry = memory->at(regMEMPTR) & 1;
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) >> 1) | ((regF & 1) << 7));

    regF = (memory->at(regMEMPTR) & 0xa8) | newcarry
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sla_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) >> 7;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) << 1) & 0xff;

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sra_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
    | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) & 1;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) & 0x80) | (memory->at(regMEMPTR) >> 1);

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::sll_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) >> 7;

    memory->at(regMEMPTR) = ((memory->at(regMEMPTR) << 1) | 1) & 0xff;

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::srl_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = memory->at(regMEMPTR) & 1;

    memory->at(regMEMPTR) = (memory->at(regMEMPTR) >> 1);

    regF = regF | (memory->at(regMEMPTR) & 0xa8)
        | ((~(memory->at(regMEMPTR) ^ (memory->at(regMEMPTR) >> 4) ^ (memory->at(regMEMPTR) >> 2)
        ^ (memory->at(regMEMPTR) >> 1)) & 1) << 2);     // S,5,H,3,P,N,C
    if (!memory->at(regMEMPTR)) regF |= 0x40;           // Z

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}

void z80cpu::bit_x_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);   // set 5,H,3, preserve C
    // set Z,PV flags
    if (!(memory->at(regMEMPTR) & (1 << ((memory->at((regPC + 2) & 0xffff) - 0x40) >> 3)))) regF |= 0x44;
    else if (((memory->at((regPC + 2) & 0xffff) - 0x40) >> 3) == 7) regF |= 0x80;

    regPC += 3;
}

void z80cpu::res_x_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) &= (((1 << ((memory->at((regPC + 2) & 0xffff) - 0x80) >> 3)) ^ 0xff) & 0xff);

    regPC += 3;
}

void z80cpu::set_x_atiypd() noexcept {
    regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory->at((regPC + 1) & 0xffff) >> 7)) & 0xffffff00)
        | memory->at((regPC + 1) & 0xffff))) & 0xffff;

    memory->at(regMEMPTR) |= (1 << ((memory->at((regPC + 2) & 0xffff) - 0xc0) >> 3));

    *regPtr[memory->at((regPC + 2) & 0xffff) & 0x7] = memory->at(regMEMPTR);

    regPC += 3;
}
