#include <iostream>
#include "z80.h"

#pragma clang diagnostic ignored "-Wsign-conversion"

//done generic conditionals: evaluates as true if cond=!flag and flag is set
//done conditional still not working
//done 16-bit arithmetic can be made generic, register address decoding is same as for 8-bit... made generic, but also broke them
//done check sp arithmetic flags
//done 16-bit arithmetic flags
//done add/sub/sbc/adc_a_athl
//done add/adc/sbc hl,sp / add/adc/sbc ix,sp / add/adc/sbc iy,sp
//done All memory[]/MEMPTR access must make sure it doesn't go out of bound (& 0xffff)
//done check PO/PE behaviour
//done V flag wrong at least on CP ops (V flag is carry from bit 6 -> 7 ?), wrong on OR/... ->??? or sets parity, not overflow
//done optimize JP/CALL instructions (check block Ex)
//done cp operations set 5,3 flags by operand - but which operand??? answer: the second one
//done wrong H flag calculation on 16-bit ADC/SBC, N flag not set on SBC_r16
//done wrong H flag on inc/dec (possibly also V?)
//TODO ED instructions are missing some flag updates
//TODO (Block) IO flags
//done bit x,(hl) 5,3 flags, bit x,(iyx+d) all flags (check docs)
//done setting Z flag on INC/DEC instructions might be wrong
//done V flag on INC/DEC can be written unconditionally - see inc (ix+d)
//done add hl,sp add ix,sp
//done block ops should be executed one-at-a-time, setting PV flag while active and resetting PC
//done (index+displacement) offset calculation is wrong
//done rework inc/dec_athl
//done PV still not correct on cp_/sbx_a
//done parity table doesn't work reliably, and should be made static - tbl_gen output is correct though
//done daa
//done res (iyx+d) instruction timing wrong, sets flags <- instruction not triggered!
//TODO some instruction timings still seem wrong
//TODO seperate tick calculation, run instructions at the end of the count
//TODO with tick count known in advance, it should suffice to run execute_tick() only when tick count has expired
//TODO maybe: with instructions run at end of tick count, register pointer could be calculated in advance,
//	which would allow i* ops to be generic, but would overall slow down execution -> perhaps not worth it
//TODO add interrupts
//TODO regQ emulation


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                    REGULAR INSTRUCTIONS
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GENERIC 16-BIT REGISTER
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_r16_nn() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	*(reg + 1) = memory[(regPC + 1) & 0xffff];
	*reg = memory[(regPC + 2) & 0xffff];

	regPC += 3;
	return 10;
}

int z80cpu::add_hl_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

	regL += *reg;

	regF = (regF & 0xc4) | (((regL >> 8) + (regH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regH += ((regL >> 8) + (*(reg - 1)));
	regL &= 0xff;

	regF = regF | (regH & 0x28) | (regH >> 8);					//set 5,3,C

	regH &= 0xff;

	regPC++;
	return 11;
}

int z80cpu::inc_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	*(reg + 1) += 1;

	*reg = (*reg + (*(reg + 1) >> 8)) & 0xff;
	*(reg + 1) &= 0xff;

	regPC++;
	return 6;
}

int z80cpu::dec_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	*reg -= 1;

	*(reg - 1) = (*(reg - 1) + (*reg >> 8)) & 0xff;
	*reg &= 0xff;

	regPC++;
	return 6;
}


int z80cpu::pop_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	*(reg + 1) = memory[regSP];
	*reg = memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regPC++;
	return 10;
}

int z80cpu::push_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regSP = (regSP - 2) & 0xffff;
	memory[regSP] = *(reg + 1);
	memory[(regSP + 1) & 0xffff] = *reg;

	regPC++;
	return 11;
}

////////////////////////////////////////////////////////////////////////////////
//GENERIC 8-BIT REGISTER
////////////////////////////////////////////////////////////////////////////////

int z80cpu::inc_r8() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regF = (regF & 1) | (((*reg & 0xf) + 1) & 0x10);	//H,N,(C)

	*reg = (*reg + 1) & 0xff;

	regF |= (*reg & 0xa8);					//S,5,3
	if (*reg == 0x80) regF |= 4;				//V
	if (!*reg) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::dec_r8() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regF = (regF & 1) | (((*reg & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (*reg == 0x80) regF |= 4;				//V

	*reg = (*reg - 1) & 0xff;

	regF |= (*reg & 0xa8);					//S,5,3
	if (!*reg) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::ld_r8_n() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = memory[(regPC + 1) & 0xffff];

	regPC += 2;
	return 7;
}

int z80cpu::ld_r8_r8() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 4;
}

int z80cpu::ld_r8_athl() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = memory[regL + (regH << 8)];

	regPC++;
	return 7;
}

int z80cpu::ld_athl_r8() {

	memory[regL + (regH << 8)] = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 7;
}

int z80cpu::adx_a_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);	//performs add_a_r8 when bit 3 of opcode is reset, else adc_a_r8

	regF = (((regA + *reg + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (*reg & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~(*reg)) & (regA ^ (regA + *reg + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + *reg + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}

int z80cpu::sbx_a_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);	//performs sub_r8 when bit 3 of opcode is reset, else sbc_a_r8

	regF = (((regA - *reg - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (*reg & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ *reg) & (regA ^ (regA - *reg - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - *reg - oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flags
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}

int z80cpu::and_r8() {

	regA &= *regPtr[memory[regPC] & 0x7];
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::xor_r8() {

	regA = (regA ^ *regPtr[memory[regPC] & 0x7]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::or_r8() {

	regA = (regA | *regPtr[memory[regPC] & 0x7]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::cp_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	regF = (((regA - *reg) & 0x100) >> 8) | (((regA & 0xf) - (*reg & 0xf)) & 0x10)		//H,C
		| ((((regA ^ *reg) & (regA ^ (regA - *reg))) & 0x80) >> 5) | 2		//V,N
		| ((regA - *reg) & 0x80) | (*reg & 0x28);					//S,5,3 flags
	if (!(regA - *reg)) regF |= 0x40;							//Z

	regPC++;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//GENERIC CONDITIONAL
////////////////////////////////////////////////////////////////////////////////

int z80cpu::jr_cc_n() {

	if (((regF & conditionCodes[(memory[regPC] & 0x10) >> 4]) && (memory[regPC] & 8))
		|| ((!(regF & conditionCodes[(memory[regPC] & 0x10) >> 4])) && (!(memory[regPC] & 8)))) {

		regPC++;
		regPC = (regPC + 1 + (((-(memory[regPC] >> 7)) & 0xffffff00) | memory[regPC])) & 0xffff;

		regMEMPTR = regPC;
		return 12;
	}

	regPC += 2;
	return 7;
}

int z80cpu::ret_cc() {

	if (((regF & conditionCodes[(memory[regPC] & 0x30) >> 4]) && (memory[regPC] & 8))
		|| ((!(regF & conditionCodes[(memory[regPC] & 0x30) >> 4])) && (!(memory[regPC] & 8)))) {

		regPC = memory[regSP] + (memory[(regSP + 1) & 0xffff] << 8);
		regMEMPTR = regPC;
		regSP = (regSP + 2) & 0xffff;
		return 11;
	}

	regPC++;
	return 5;
}

int z80cpu::jp_cc_nn() {

	regMEMPTR = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	if (((regF & conditionCodes[(memory[regPC] & 0x30) >> 4]) && (memory[regPC] & 8))
		|| ((!(regF & conditionCodes[(memory[regPC] & 0x30) >> 4])) && (!(memory[regPC] & 8)))) regPC = regMEMPTR;

	else regPC += 3;

	return 10;
}

int z80cpu::call_cc_nn() {

	regMEMPTR = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	if (((regF & conditionCodes[(memory[regPC] & 0x30) >> 4]) && (memory[regPC] & 8))
		|| ((!(regF & conditionCodes[(memory[regPC] & 0x30) >> 4])) && (!(memory[regPC] & 8)))) {

		regSP = (regSP - 2) & 0xffff;
		memory[regSP] = (regPC + 3) & 0xff;
		memory[(regSP + 1) & 0xffff] = ((regPC + 3) & 0xff00) >> 8;

		regPC = regMEMPTR;
		return 17;
	}

	regPC += 3;
	return 10;
}


////////////////////////////////////////////////////////////////////////////////
//GENERIC MISC
////////////////////////////////////////////////////////////////////////////////

int z80cpu::rst_xx() {

	regPC = memory[regPC] & 0x38;
	regMEMPTR = regPC;

	return 11;
}

////////////////////////////////////////////////////////////////////////////////
//group 0x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::nop() {

	regPC++;
	return 4;
}

int z80cpu::ld_atbc_a() {

	memory[regC + (regB << 8)] = regA;

	regMEMPTR = ((regC + 1) & 0xff) | (regA * 0x100);
	if (cpuType == BM1) regMEMPTR &= 0xff;

	regPC++;
	return 7;
}

int z80cpu::rlca() {

	int carry = 0;
	if (regA & 0x80) carry = 1;

	regA = (regA << 1) + carry;

	regF = (regF & 0xd5) | (regA & 0x28) | carry;		//set 5,3,N,C flag

	regPC++;
	return 4;
}

int z80cpu::ex_af_af() {

	int temp = regA;
	regA = regAs;
	regAs = temp;
	temp = regF;
	regF = regFs;
	regFs = temp;
	regPC++;
	return 4;
}

int z80cpu::ld_a_atbc() {

	regA = memory[regC + (regB << 8)];

	regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

	regPC++;
	return 7;
}

int z80cpu::rrca() {

	int carry = regA & 1;
	regA = (regA >> 1) | (carry * 0x80);

	regF = (regF & 0xd5) | (regA & 0x28) | carry;		//set 5,3,N,C flag

	regPC++;
	return 4;
}

////////////////////////////////////////////////////////////////////////////////
//group 1x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::djnz() {

	regB = (regB - 1) & 0xff;

	if (regB) {

		regPC++;
		regPC = (regPC + 1 + (((-(memory[regPC] >> 7)) & 0xffffff00) | memory[regPC])) & 0xffff;

		regMEMPTR = regPC;
		return 13;
	}

	regPC += 2;

	regMEMPTR = regPC;
	return 8;
}

int z80cpu::ld_atde_a() {

	memory[regE + (regD << 8)] = regA;

	regMEMPTR = ((regE + 1) & 0xff) | (regA << 8);
	if (cpuType == BM1) regMEMPTR &= 0xff;

	regPC++;
	return 7;
}

int z80cpu::rla() {

	int carry = 0;
	if (regA & 0x80) carry = 1;

	regA = (regA << 1) + (regF & 1);

	regF = (regF & 0xd5) | (regA & 0x28) | carry;		//set 5,3,N,C flag

	regPC++;
	return 4;
}

int z80cpu::jr() {

	regPC++;
	regPC = (regPC + 1 + (((-(memory[regPC] >> 7)) & 0xffffff00) | memory[regPC])) & 0xffff;

	regMEMPTR = regPC;
	return 12;
}

int z80cpu::ld_a_atde() {

	regA = memory[regE + (regD << 8)];

	regMEMPTR = (regE + (regD << 8) + 1) & 0xffff;

	regPC++;
	return 7;
}

int z80cpu::rra() {

	int carry = regA & 1;
	regA = (regA >> 1) | ((regF & 1) * 0x80);

	regF = (regF & 0xd5) | (regA & 0x28) | carry;		//set 5,3,N,C flag

	regPC++;
	return 4;
}

////////////////////////////////////////////////////////////////////////////////
//group 2x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_atnn_hl() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regMEMPTR = (addr + 1) & 0xffff;

	memory[addr] = regL;
	memory[regMEMPTR] = regH;

	regPC += 3;
	return 16;
}

int z80cpu::daa() {

	int temp = 0;
	int carry = regF & 1;

	if ((regF & 0x10) || ((regA & 0xf) > 9)) temp = 6;
	if (carry || regA > 0x99) temp |= 0x60;
	if (regA > 0x99) carry |= 1;

	if (regF & 2) {

		regF = (((regA & 0xf) - (temp & 0xf)) & 0x10) | 2;
		regA = (regA - temp) & 0xff;
	}
	else {

		regF = (((regA & 0xf) + (temp & 0xf)) & 0x10);
		regA = (regA + temp) & 0xff;
	}

	regF = (regF & 0xfa) | (regA & 0xa8) | carry | parity_tbl[regA];
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}

int z80cpu::ld_hl_atnn() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regMEMPTR = (addr + 1) & 0xffff;

	regL = memory[addr];
	regH = memory[regMEMPTR];

	regPC += 3;
	return 16;
}

int z80cpu::cpl() {

	regA = (~regA) & 0xff;

	regF = ((regF & 0xd7) | (regA & 0x28)) | 0x12;		//set 5,H,3,N flags

	regPC++;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//block 3x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_sp_nn() {

	regSP = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regPC += 3;
	return 10;
}

int z80cpu::ld_atnn_a() {

	regMEMPTR = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	memory[regMEMPTR] = regA;
	regMEMPTR = (regMEMPTR + 1) & 0xffff;

	regPC += 3;
	return 13;
}

int z80cpu::inc_sp() {

	regSP = (regSP + 1) & 0xffff;

	regPC++;
	return 6;
}

int z80cpu::inc_athl() {

	int regHL = regL + (regH << 8);

	regF = (regF & 1) | (((memory[regHL] & 0xf) + 1) & 0x10);	//H,N,(C)

	memory[regHL] = (memory[regHL] + 1) & 0xff;

	regF |= (memory[regHL] & 0xa8);					//S,5,3
	if (memory[regHL] == 0x80) regF |= 4;				//V
	if (!memory[regHL]) regF |= 0x40;				//Z

	regPC++;
	return 11;
}

int z80cpu::dec_athl() {

	int regHL = regL + (regH << 8);

	regF = (regF & 1) | (((memory[regHL] & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (memory[regHL] == 0x80) regF |= 4;				//V

	memory[regHL] = (memory[regHL] - 1) & 0xff;

	regF |= (memory[regHL] & 0xa8);					//S,5,3
	if (!memory[regHL]) regF |= 0x40;				//Z

	regPC++;
	return 11;
}

int z80cpu::ld_athl_n() {

	memory[regL + (regH << 8)] = memory[(regPC + 1) & 0xffff];

	regPC++;
	return 10;
}

int z80cpu::scf() {

	regF = (regF & 0xc5) | (regA & 0x28) | 1;		//set 5,H,3,N,C flags

	regPC++;
	return 4;
}

int z80cpu::add_hl_sp() {

	int sp_lo = regSP & 0xff;
	int sp_hi = regSP >> 8;

	regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

	regL += sp_lo;

	regF = (regF & 0xc4) | (((regL >> 8) + (regH & 0xf) + (sp_hi & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regH += (regL >> 8) + sp_hi;
	regL &= 0xff;

	regF = regF | (regH & 0x28) | (regH >> 8);					//set 5,3,C

	regH &= 0xff;

	regPC++;
	return 11;

}

int z80cpu::ld_a_atnn() {

	regMEMPTR = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regA = memory[regMEMPTR];

	regMEMPTR = (regMEMPTR + 1) & 0xffff;

	regPC += 3;
	return 13;
}

int z80cpu::dec_sp() {

	regSP = (regSP - 1) & 0xffff;

	regPC++;
	return 6;
}

int z80cpu::ccf() {

	regF = ((regF & 0xc5) | (regA & 0x28) | ((regF & 1) << 4)) ^ 1;		//set 5,H,3,N,C flags

	regPC++;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//block 7x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::halt() {

	regPC++;	//TODO: instruction currently disabled, acts as nop
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//block 8x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::adx_a_athl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);
	int addval = memory[regL + (regH << 8)];

	regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~(addval)) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + addval + oldcarry) & 0xff;

	regF |= (regA & 0xa8);			//set S,5,3 flag
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 7;
}

////////////////////////////////////////////////////////////////////////////////
//block 9x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::sbx_a_athl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);
	int subval = memory[regL + (regH << 8)];

	regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - subval - oldcarry) & 0xff;

	regF |= (regA & 0xa8);							//set S,5,3 flags
	if (!regA) regF |= 0x40;						//set Z flag

	regPC++;
	return 7;
}

////////////////////////////////////////////////////////////////////////////////
//block Ax
////////////////////////////////////////////////////////////////////////////////

int z80cpu::and_athl() {

	regA &= memory[regL + (regH << 8)];
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 7;
}

int z80cpu::xor_athl() {

	regA = (regA ^ memory[regL + (regH << 8)]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 7;
}


////////////////////////////////////////////////////////////////////////////////
//block Bx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::or_athl() {

	regA = (regA | memory[regL + (regH << 8)]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 7;
}

int z80cpu::cp_athl() {

	int cpval = memory[regL + (regH << 8)];

	regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)	//H,C
		| ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2		//V,N
		| ((regA - cpval) & 0x80) | (cpval & 0x28);					//S,5,3 flags
	if (!(regA - cpval)) regF |= 0x40;							//Z

	regPC++;
	return 7;
}


////////////////////////////////////////////////////////////////////////////////
//block Cx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::jp_nn() {

	regPC = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regMEMPTR = regPC;
	return 10;
}

int z80cpu::ret() {

	regPC = memory[regSP] + (memory[(regSP + 1) & 0xffff] << 8);
	regMEMPTR = regPC;
	regSP = (regSP + 2) & 0xffff;
	return 11;
}

int z80cpu::prefix_cb() {

	regPC = (regPC + 1) & 0xffff;
	int ticks;

	if (memory[regPC] >= 0xc0) ticks = cpu_instructions_cb_set[memory[regPC] & 0x7](this);
	else if (memory[regPC] >= 0x80) ticks = cpu_instructions_cb_res[memory[regPC] & 0x7](this);
	else if (memory[regPC] >= 0x40) ticks = cpu_instructions_cb_bit[memory[regPC] & 0x7](this);
	else ticks = cpu_instructions_cb[static_cast<unsigned>(memory[regPC])](this);

	regPC++;

	return 4 + ticks;
}

int z80cpu::call_nn() {

	regSP = (regSP - 2) & 0xffff;
	memory[regSP] = (regPC + 3) & 0xff;
	memory[(regSP + 1) & 0xffff] = ((regPC + 3) & 0xff00) >> 8;
	regPC = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regMEMPTR = regPC;
	return 17;
}

int z80cpu::adx_a_n() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);
	int addval = memory[(regPC + 1) & 0xffff];

	regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~(addval)) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + addval + oldcarry) & 0xff;

	regF |= (regA & 0xa8);			//set S,5,3 flag
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 7;
}


////////////////////////////////////////////////////////////////////////////////
//block Dx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::out_atn_a() {

	regMEMPTR = (memory[(regPC + 1) & 0xffff] + 1) & 0xff;
	if (cpuType != BM1) regMEMPTR |= (regA * 0x100);

	outputPorts[memory[(regPC + 1) & 0xffff] + (regA << 8)] = regA;
	outputPortsShort[memory[(regPC + 1) & 0xffff]] = regA;
	regPC += 2;
	return 11;
}

int z80cpu::exx() {

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
	return 4;
}

int z80cpu::in_a_atn() {			//TODO: temporary solution!

	regMEMPTR = ((regA << 8) + memory[(regPC + 1) & 0xffff] + 1) & 0xffff;

	regA = inputPorts[memory[(regPC + 1) & 0xffff] + (regA << 8)] | inputPortsShort[memory[(regPC + 1) & 0xffff]];

	regPC += 2;
	return 11;
}

int z80cpu::prefix_dd() {

	regPC = (regPC + 1) & 0xffff;
	//regPC++;

	int ticks = cpu_instructions_dd[static_cast<unsigned>(memory[regPC])](this);

	return 4 + ticks;
}

int z80cpu::sbx_a_n() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);
	int subval = memory[(regPC + 1) & 0xffff];

	regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - subval - oldcarry) & 0xff;

	regF |= (regA & 0xa8);							//set S,5,3 flags
	if (!regA) regF |= 0x40;						//set Z flag

	regPC += 2;
	return 7;
}


////////////////////////////////////////////////////////////////////////////////
//block Ex
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ex_atsp_hl() {

	int temp = regL;
	regL = memory[regSP];
	memory[regSP] = temp;

	temp = regH;
	regH = memory[(regSP + 1) & 0xffff];
	memory[(regSP + 1) & 0xffff] = temp;

	regPC++;
	return 19;
}

int z80cpu::and_n() {

	regA &= memory[(regPC + 1) & 0xffff];
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 7;
}

int z80cpu::jp_athl() {

	regPC = regL + (regH << 8);

	return 4;
}

int z80cpu::ex_de_hl() {

	int temp = regD;
	regD = regH;
	regH = temp;
	temp = regE;
	regE = regL;
	regL = temp;

	regPC++;
	return 4;
}

int z80cpu::prefix_ed() {

	regPC++;
	regR = (regR & 0x80) | ((regR + 1) & 0x7f);

	int ticks;

	if ((memory[regPC] < 0x40) | (memory[regPC] > 0xbb)) {		//skip useless instructions

		ticks = 4;
		regPC++;
	}
	else ticks = cpu_instructions_ed[static_cast<unsigned>(memory[regPC] - 0x40)](this);

	return 4 + ticks;
}

int z80cpu::xor_n() {

	regA = (regA ^ memory[(regPC + 1) & 0xffff]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 7;
}


////////////////////////////////////////////////////////////////////////////////
//block Fx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::pop_af() {

	regF = memory[regSP];
	regA = memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regPC++;
	return 10;
}

int z80cpu::di() {

	interruptsEnabled = false;
	regIFF1 = false;
	regIFF2 = false;

	regPC++;
	return 4;
}

int z80cpu::push_af() {

	regSP = (regSP - 2) & 0xffff;
	memory[regSP] = regF;
	memory[(regSP + 1) & 0xffff] = regA;

	regPC++;
	return 11;
}

int z80cpu::or_n() {

	regA |= memory[(regPC + 1) & 0xffff];
	regF = (regA & 0xa8) | parity_tbl[regA];	//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;			//set Z flag

	regPC += 2;
	return 7;
}

int z80cpu::ld_sp_hl() {

	regSP = regL + (regH << 8);
	regPC++;
	return 6;
}

int z80cpu::ei() {

	interruptsEnabled = false;
	regIFF1 = true;
	regIFF2 = true;

	regPC++;
	return 4;
}

int z80cpu::prefix_fd() {

	regPC = (regPC + 1) & 0xffff;

	int ticks = cpu_instructions_fd[static_cast<unsigned>(memory[regPC])](this);

	return 4 + ticks;
}

int z80cpu::cp_n() {

	int cpval = memory[(regPC + 1) & 0xffff];

	regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)		//H,C
		| ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2		//V,N
		| ((regA - cpval) & 0x80) | (cpval & 0x28);					//S,5,3 flags
	if (!(regA - cpval)) regF |= 0x40;						//Z

	regPC += 2;
	return 7;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX ED
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GENERIC
////////////////////////////////////////////////////////////////////////////////

int z80cpu::in_r8_atc() {

	regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

	int inval = (inputPorts[regC + (regB << 8)] | inputPortsShort[regC]) & 0xff;
	*regPtr[(memory[regPC] >> 3) & 7] = inval;
	regF = (regF & 1) | (inval & 0xa8) | ((~(inval ^ (inval >> 4) ^ (inval >> 2) ^ (inval >> 1)) & 1) << 2);	//set flags

	regPC++;
	return 8;
}

int z80cpu::out_atc_r8() {

	regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

	if (cpuType == NMOS) regDummy = 0;
	else regDummy = 0xff;

	outputPorts[regC + (regB << 8)] = *regPtr[(memory[regPC] >> 3) & 7];
	outputPortsShort[regC] = *regPtr[(memory[regPC] >> 3) & 7];

	regPC++;
	return 8;
}

int z80cpu::adc_hl_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regL = regL + *reg + (regF & 1);

	regF = (((*(reg - 1) & 0xf) + (regH & 0xf) + ((regL & 0x100) >> 8)) & 0x10)
		| ((((regH ^ ~(*(reg - 1))) & (regH ^ (regH + *(reg - 1) + (regL >> 8)))) & 0x80) >> 5);	//H,V,N flags

	regH = *(reg - 1) + regH + (regL >> 8);

	regF = regF | (regH & 0xa8) | (regH >> 8);	//S,5,3,V,C flags

	regL &= 0xff;
	regH &= 0xff;

	if (!(regH | regL)) regF |= 0x40;	//set Z flag

	regPC++;
	return 11;
}

int z80cpu::sbc_hl_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regL = regL - *(reg + 1) - (regF & 1);

	regF = (((regH & 0xf) - (*reg & 0xf) - ((regL >> 8) & 1)) & 0x10) | 2
		| ((((regH ^ *reg) & (regH ^ (regH - *reg - ((regL >> 8) & 1)))) & 0x80) >> 5);	//H,V,N flags

	regH = regH - *reg - ((regL >> 8) & 1);

	regF = regF | (regH & 0xa8) | ((regH >> 8) & 1);	//S,5,3,C flags

	regL &= 0xff;
	regH &= 0xff;

	if (!(regH | regL)) regF |= 0x40;	//set Z flag

	regPC++;
	return 11;
}

int z80cpu::ld_atnn_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	memory[addr] = *(reg + 1);
	memory[regMEMPTR] = *reg;

	regPC += 3;
	return 16;
}

int z80cpu::ld_r16_atnn() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	*reg = memory[addr];
	*(reg - 1) = memory[regMEMPTR];

	regPC += 3;
	return 16;
}

////////////////////////////////////////////////////////////////////////////////
//block ED 4x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::neg() {

	regF = (((((~regA) & 0xff) + 1) & 0x100) >> 8)
		| (((((~regA) & 0xf) + 1) & 0x10) >> 4) | (((((~regA) & 0x7f) + 1) & 0x80) >> 4); //set H,V,C flags

	regA = (-(regA)) & 0xff;

	regF |= ((regA & 0xa8) | 2);		//set S,5,3,N flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::retn() {

	regPC = memory[regSP] + memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regIFF1 = regIFF2;

	return 10;
}

int z80cpu::im0() {

	interruptMode = 0;

	regPC++;
	return 4;
}

int z80cpu::ld_i_a() {

	regI = regA;

	regPC++;
	return 5;
}

int z80cpu::reti() {

	regPC = memory[regSP] + memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regIFF1 = regIFF2;

	return 10;
}

int z80cpu::ld_r_a() {

	regR = regA;

	regPC++;
	return 5;
}


////////////////////////////////////////////////////////////////////////////////
//block ED 5x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::im1() {

	interruptMode = 1;

	regPC++;
	return 4;
}

int z80cpu::ld_a_i() {

	regA = regI;

	regF = (regF & 1) | (regA & 0xa8);	//set S,Z,5,H,3,N flags
	if (!regA) regF |= 0x40;		//set Z flag
	if (regIFF2) regF |= 4;			//set P/V flag

	regPC++;
	return 5;
}

int z80cpu::im2() {

	interruptMode = 2;

	regPC++;
	return 4;
}

int z80cpu::ld_a_r() {

	regA = regR;

	regF = (regF & 1) | (regA & 0xa8);	//set S,Z,5,H,3,N flags
	if (!regA) regF |= 0x40;		//set Z flag
	if (regIFF2) regF |= 4;			//set P/V flag

	regPC++;
	return 5;
}


////////////////////////////////////////////////////////////////////////////////
//block ED 6x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::rrd() {

	regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

	int temp = regA & 0xf;

	regA = (regA & 0xf0) | (memory[regL + (regH << 8)] & 0xf);
	memory[regL + (regH << 8)] = ((memory[regL + (regH << 8)] >> 4) | (temp << 4)) & 0xff;

	regF = (regF & 1) | (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);	//set flags

	return 14;
}

int z80cpu::rld() {

	regMEMPTR = (regL + (regH << 8) + 1) & 0xffff;

	int temp = regA & 0xf;

	regA = (regA & 0xf0) | ((memory[regL + (regH << 8)] & 0xf0) >> 4);
	memory[regL + (regH << 8)] = ((memory[regL + (regH << 8)] << 4) | temp) & 0xff;

	regF = (regF & 1) | (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);	//set flags

	return 14;
}


////////////////////////////////////////////////////////////////////////////////
//block ED 7x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::sbc_hl_sp() {

	int sp_lo = regSP & 0xff;
	int sp_hi = regSP >> 8;

	regL = regL - sp_lo - (regF & 1);

	regF = (((regH & 0xf) - (sp_hi & 0xf) - ((regL >> 8) & 1)) & 0x10) | 2
		| ((((regH ^ sp_hi) & (regH ^ (regH - sp_hi - ((regL >> 8) & 1)))) & 0x80) >> 5);	//H,V,N flags

	regH = regH - sp_hi - ((regL >> 8) & 1);

	regF = regF | (regH & 0xa8) | ((regH >> 8) & 1);	//S,5,3,C flags

	regL &= 0xff;
	regH &= 0xff;

	if (!(regH | regL)) regF |= 0x40;	//set Z flag

	regPC++;
	return 11;
}

int z80cpu::ld_atnn_sp() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	memory[addr] = regSP & 0xff;
	memory[regMEMPTR] = (regSP & 0xff00) >> 8;

	regPC += 3;
	return 16;
}

int z80cpu::adc_hl_sp() {

	int sp_lo = regSP & 0xff;
	int sp_hi = regSP >> 8;

	regL = regL + sp_lo + (regF & 1);

	regF = (((sp_hi & 0xf) + (regH & 0xf) + ((regL & 0x100) >> 8)) & 0x10)
		| ((((regH ^ ~(sp_hi)) & (regH ^ (regH + sp_hi + (regL >> 8)))) & 0x80) >> 5);	//H,V,N flags

	regH = sp_hi + regH + (regL >> 8);

	regF = regF | (regH & 0xa8) | (regH >> 8);	//S,5,3,V,C flags

	regL &= 0xff;
	regH &= 0xff;

	if (!(regH | regL)) regF |= 0x40;	//set Z flag

	regPC++;
	return 11;
}

int z80cpu::ld_sp_atnn() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	regSP = memory[addr] + (memory[regMEMPTR] << 8);

	regPC += 3;
	return 16;
}

////////////////////////////////////////////////////////////////////////////////
//block ED Ax
////////////////////////////////////////////////////////////////////////////////


int z80cpu::ldi() {

	int temp = regE + (regD << 8);
	int ftr = memory[regL + (regH << 8)] + regA;

	memory[temp] = memory[regL + (regH << 8)];

	temp = (temp + 1) & 0xffff;
	regE = temp & 0xff;
	regD = temp >> 8;
	temp = regL + (regH << 8) + 1;
	regL = temp & 0xff;
	regH = (temp >> 8) & 0xff;

	temp = regC + (regB * 0x100) - 1;
	regC = temp & 0xff;
	regB = (temp >> 8) & 0xff;

	regF = (regF & 0xc1) | (ftr & 8) | ((ftr & 2) << 4);	//set 5,3 flags
	if (!(regC | regB)) regF |= 4;				//set P/V flag

	regPC++;
	return 12;
}

int z80cpu::cpi() {

	int cp = (regA - memory[regL + (regH << 8)]) & 0xff;

	regMEMPTR = (regMEMPTR + 1) & 0xffff;

	regF = (regF & 1) | (cp & 0x80) | (((regA & 0xf) - (memory[regL + (regH << 8)] & 0xf)) & 0x10) | 2;		//S,H,N flags
	regF |= ((cp - regA - ((regF >> 4) & 1)) & 0x28);	//5,3 flags
	if (!cp) regF |= 0x40;					//Z flag

	regH = (regH + ((regL + 1) >> 8)) & 0xff;
	regL = (regL + 1) & 0xff;

	regB = (regB - ((regL + 1) >> 8)) & 0xff;
	regC = (regC - 1) & 0xff;


	if (!(regC | regB)) regF |= 4;				//PV flag

	regPC++;
	return 12;
}

int z80cpu::ini() {

	regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

	int regHL = regL + (regH << 8);

	memory[regHL] = inputPorts[regC + (regB << 8)] | inputPortsShort[regC];

	int ftr = memory[regHL];

	regB = (regB - 1) & 0xff;
	regHL += 1;

	regL = regHL & 0xff;
	regH = (regHL >> 8) & 0xff;

	ftr = ftr + ((regC + 1) & 0x100);

	regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);		//set S,5,H,3,N,C flags
	if (!regB) regF &= 0xbf;						//set Z flag
							//TODO: P/V flag

	regPC++;
	return 12;
}

int z80cpu::outi() {

	regMEMPTR = (regC + (regB << 8) + 1) & 0xffff;

	int regHL = regL + (regH << 8);
	int ftr = memory[regHL];

	outputPorts[regC + (regB << 8)] = memory[regHL];
	outputPortsShort[regC] = memory[regHL];

	regB = (regB - 1) & 0xff;
	regHL += 1;

	regL = regHL & 0xff;
	regH = (regHL >> 8) & 0xff;

	ftr = ftr + ((regC + 1) & 0x100);

	regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);		//set S,5,H,3,N,C flags
	if (!regB) regF &= 0xbf;						//set Z flag
							//TODO: P/V flag

	regPC++;
	return 12;
}

int z80cpu::ldd() {

	int temp = regE + (regD * 0x100);
	int ftr = memory[regL + (regH << 8)] + regA;

	memory[temp] = memory[regL + (regH << 8)];

	temp = (temp - 1) & 0xffff;
	regE = temp & 0xff;
	regD = temp >> 8;
	temp = regL + (regH << 8) - 1;
	regL = temp & 0xff;
	regH = (temp >> 8) & 0xff;

	temp = regC + (regB * 0x100) - 1;
	regC = temp & 0xff;
	regB = (temp >> 8) & 0xff;

	regF = (regF & 0xc1) | (ftr & 8) | ((ftr & 2) << 4);	//set 5,3 flags
	if (!(regC | regB)) regF |= 4;				//set P/V flag

	regPC++;
	return 12;
}

int z80cpu::cpd() {

	int cp = (regA - memory[regL + (regH << 8)]) & 0xff;

	regMEMPTR = (regMEMPTR - 1) & 0xffff;

	regF = (regF & 1) | (cp & 0x80) | (((regA & 0xf) - (memory[regL + (regH << 8)] & 0xf)) & 0x10) | 2;		//S,H,N flags
	regF |= ((cp - regA - ((regF >> 4) & 1)) & 0x28);	//5,3 flags
	if (!cp) regF |= 0x40;					//Z flag

	regH = (regH - ((regL - 1) >> 8)) & 0xff;
	regL = (regL - 1) & 0xff;

	regB = (regB - ((regC - 1) >> 8)) & 0xff;
	regC = (regC - 1) & 0xff;


	if (!(regC | regB)) regF |= 4;				//PV flag

	regPC++;
	return 12;
}

int z80cpu::ind() {

	regMEMPTR = (regC + (regB << 8) - 1) & 0xffff;

	int regHL = regL + (regH << 8);

	memory[regHL] = inputPorts[regC + (regB << 8)] | inputPortsShort[regC];

	int ftr = memory[regHL];

	regB = (regB - 1) & 0xff;
	regHL -= 1;

	regL = regHL & 0xff;
	regH = (regHL >> 8) & 0xff;

	ftr = ftr + ((regC - 1) & 0x100);

	regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);		//set S,5,H,3,N,C flags
	if (!regB) regF &= 0xbf;						//set Z flag
							//TODO: P/V flag

	regPC++;
	return 12;
}

int z80cpu::outd() {

	regMEMPTR = (regC + (regB << 8) - 1) & 0xffff;

	int regHL = regL + (regH << 8);

	outputPorts[regC + (regB << 8)] = memory[regHL];
	outputPortsShort[regC] = memory[regHL];


	int ftr = memory[regHL];

	regB = (regB - 1) & 0xff;
	regHL -= 1;

	regL = regHL & 0xff;
	regH = (regHL >> 8) & 0xff;

	ftr = ftr + ((regC - 1) & 0x100);

	regF = (regB & 0xa8) | (ftr >> 7) | (ftr >> 4) | (ftr >> 8);		//set S,5,H,3,N,C flags
	if (!regB) regF &= 0xbf;						//set Z flag
							//TODO: P/V flag

	regPC++;
	return 12;
}


////////////////////////////////////////////////////////////////////////////////
//block ED Bx
////////////////////////////////////////////////////////////////////////////////


int z80cpu::ldir() {

	if ((regB | regC)) {

		ldi();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::cpir() {

	if ((regB | regC)) {

		cpi();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::inir() {

	if (regB) {

		ini();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::otir() {

	if (regB) {

		outi();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::lddr() {

	if ((regB | regC)) {

		ldd();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::cpdr() {

	if ((regB | regC)) {

		cpd();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::indr() {

	if (regB) {

		ind();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}

int z80cpu::otdr() {

	if (regB) {

		outd();
		regPC = (regPC - 2) & 0xffff;
		return 17;
	}

	regPC++;
	return 12;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX CB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//CB 0x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::rlc_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	*reg = *reg << 1;
	*reg = (*reg | (*reg >> 8)) & 0xff;

	regF = (*reg & 0xa9) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::rlc_athl() {

	(memory[regL + (regH << 8)]) = (memory[regL + (regH << 8)]) << 1;
	(memory[regL + (regH << 8)]) = ((memory[regL + (regH << 8)]) | ((memory[regL + (regH << 8)]) >> 8)) & 0xff;

	regF = ((memory[regL + (regH << 8)]) & 0xa9)
		| ((~((memory[regL + (regH << 8)]) ^ ((memory[regL + (regH << 8)]) >> 4) ^ ((memory[regL + (regH << 8)]) >> 2)
		^ ((memory[regL + (regH << 8)]) >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!(memory[regL + (regH << 8)])) regF |= 0x40;	//Z

	return 11;
}

int z80cpu::rrc_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	int oldcarry = *reg & 1;
	*reg = *reg >> 1;
	*reg |= (oldcarry << 8);

	regF = (*reg & 0xa8) | oldcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::rrc_athl() {

	int oldcarry = memory[regL + (regH << 8)] & 1;
	memory[regL + (regH << 8)] = memory[regL + (regH << 8)] >> 1;
	memory[regL + (regH << 8)] |= (oldcarry << 8);

	regF = (memory[regL + (regH << 8)] & 0xa8) | oldcarry
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//CB 1x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::rl_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	int oldcarry = regF & 1;

	*reg = ((*reg << 1) | oldcarry);

	oldcarry = *reg >> 8;
	*reg &= 0xff;

	regF = (*reg & 0xa8) | oldcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::rl_athl() {

	int oldcarry = regF & 1;

	memory[regL + (regH << 8)] = ((memory[regL + (regH << 8)] << 1) | oldcarry);

	oldcarry = memory[regL + (regH << 8)] >> 8;
	memory[regL + (regH << 8)] &= 0xff;

	regF = (memory[regL + (regH << 8)] & 0xa8) | oldcarry
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}

int z80cpu::rr_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	int newcarry = *reg & 1;

	*reg = ((*reg >> 1) | ((regF & 1) << 7));

	regF = (*reg & 0xa8) | newcarry | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::rr_athl() {

	int newcarry = memory[regL + (regH << 8)] & 1;

	memory[regL + (regH << 8)] = ((memory[regL + (regH << 8)] >> 1) | ((regF & 1) << 7));

	regF = (memory[regL + (regH << 8)] & 0xa8) | newcarry
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//CB 2x/CB 3x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::slx_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	regF = *reg >> 7;

	*reg = ((*reg << 1) | ((memory[regPC] >> 4) & 1)) & 0xff;

	regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::slx_athl() {

	regF = memory[regL + (regH << 8)] >> 7;

	memory[regL + (regH << 8)] = ((memory[regL + (regH << 8)] << 1) | ((memory[regPC] >> 4) & 1)) & 0xff;

	regF = regF | (memory[regL + (regH << 8)] & 0xa8)
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}

int z80cpu::sra_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	regF = *reg & 1;

	*reg = (*reg & 0x80) | (*reg >> 1);

	regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::sra_athl() {

	regF = memory[regL + (regH << 8)] & 1;

	memory[regL + (regH << 8)] = (memory[regL + (regH << 8)] & 0x80) | (memory[regL + (regH << 8)] >> 1);

	regF = regF | (memory[regL + (regH << 8)] & 0xa8)
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}


int z80cpu::srl_r8() {

	int *reg = regPtr[memory[regPC] & 0x7];

	regF = *reg & 1;

	*reg = (*reg >> 1);

	regF = regF | (*reg & 0xa8) | ((~(*reg ^ (*reg >> 4) ^ (*reg >> 2) ^ (*reg >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!*reg) regF |= 0x40;	//Z

	return 4;
}

int z80cpu::srl_athl() {

	regF = memory[regL + (regH << 8)] & 1;

	memory[regL + (regH << 8)] = (memory[regL + (regH << 8)] >> 1);

	regF = regF | (memory[regL + (regH << 8)] & 0xa8)
		| ((~(memory[regL + (regH << 8)] ^ (memory[regL + (regH << 8)] >> 4) ^ (memory[regL + (regH << 8)] >> 2)
		^ (memory[regL + (regH << 8)] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regL + (regH << 8)]) regF |= 0x40;	//Z

	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//CB 4x..7x (BIT)
////////////////////////////////////////////////////////////////////////////////

int z80cpu::bit_x_r8() {

	regF = (regF & 1) | 0x10;						//set H flag

	if (!(*regPtr[memory[regPC] & 0x7] & (1 << ((memory[regPC] - 0x40) >> 3)))) regF |= 0x44;	//set Z,PV flags
	else {
		if (((memory[regPC] - 0x40) >> 3) == 7) regF |= 0x80;
		else if (((memory[regPC] - 0x40) >> 3) == 5) regF |= 0x20;
		else if (((memory[regPC] - 0x40) >> 3) == 3) regF |= 0x8;
	}

	return 4;
}

int z80cpu::bit_x_athl() {

	regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);						//set 5,H,3, preserve C

	if (!(memory[regL + (regH << 8)] & (1 << ((memory[regPC] - 0x40) >> 3)))) regF |= 0x44;	//set Z,PV flags
 	else if (((memory[regPC] - 0x40) >> 3) == 7) regF |= 0x80;

	return 8;
}


////////////////////////////////////////////////////////////////////////////////
//CB 8x..Bx (RES)
////////////////////////////////////////////////////////////////////////////////

int z80cpu::res_x_r8() {

	*regPtr[memory[regPC] & 0x7] &= (((1 << ((memory[regPC] - 0x80) >> 3)) ^ 0xff) & 0xff);

	return 4;
}

int z80cpu::res_x_athl() {

	memory[regL + (regH << 8)] &= (((1 << ((memory[regPC] - 0x80) >> 3)) ^ 0xff) & 0xff);

	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//CB Cx..Fx (SET)
////////////////////////////////////////////////////////////////////////////////

int z80cpu::set_x_r8() {

	*regPtr[memory[regPC] & 0x7] |= (1 << ((memory[regPC] - 0xc0) >> 3));

	return 4;
}

int z80cpu::set_x_athl() {

	memory[regL + (regH << 8)] |= (1 << ((memory[regPC] - 0xc0) >> 3));

	return 11;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX DD
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GENERIC
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_r8_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	*regPtr[(memory[regPC] >> 3) & 0x7] = memory[regMEMPTR];

	regPC += 2;
	return 15;
}

int z80cpu::ld_r8_ixh() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = regIXH;

	regPC++;
	return 4;
}

int z80cpu::ld_r8_ixl() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = regIXL;

	regPC++;
	return 4;
}

int z80cpu::add_ix_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

	regIXL += *reg;

	regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIXH += ((regIXL >> 8) + (*(reg - 1)));
	regIXL &= 0xff;

	regF = regF | (regIXH & 0x28) | (regIXH >> 8);					//set 5,3,C

	regIXH &= 0xff;

	regPC++;
	return 11;
}

////////////////////////////////////////////////////////////////////////////////
//DD 2x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_ix_nn() {

	regIXL = memory[(regPC + 1) & 0xffff];
	regIXH = memory[(regPC + 2) & 0xffff];
	regPC += 3;
	return 10;
}

int z80cpu::ld_atnn_ix() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);
	regMEMPTR = (addr + 1) & 0xffff;

	memory[addr] = regIXL;
	memory[regMEMPTR] = regIXH;

	regPC += 3;
	return 16;
}

int z80cpu::inc_ix() {

	regIXL++;

	regIXH = (regIXH + (regIXL >> 8)) & 0xff;
	regIXL &= 0xff;

	regPC++;
	return 6;
}

int z80cpu::inc_ixh() {

	regF = (regF & 1) | (((regIXH & 0xf) + 1) & 0x10);	//H,N,(C)

	regIXH = (regIXH + 1) & 0xff;

	regF |= (regIXH & 0xa8);				//S,5,3
	if (regIXH == 0x80) regF |= 4;				//V
	if (!regIXH) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::dec_ixh() {

	regF = (regF & 1) | (((regIXH & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (regIXH == 0x80) regF |= 4;				//V

	regIXH = (regIXH - 1) & 0xff;

	regF |= (regIXH & 0xa8);				//S,5,3
	if (!regIXH) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::ld_ixh_n() {

	regIXH = memory[(regPC + 1) & 0xffff];
	regPC += 2;
	return 4;
}

int z80cpu::add_ix_ix() {

	regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

	int temp = regIXL;
	regIXL += regIXL;

	regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + (temp & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIXH += ((regIXL >> 8) + regIXH);
	regIXL &= 0xff;

	regF = regF | (regIXH & 0x28) | (regIXH >> 8);					//set 5,3,C

	regIXH &= 0xff;

	regPC++;
	return 11;
}

int z80cpu::ld_ix_atnn() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	regIXL = memory[addr];
	regIXH = memory[regMEMPTR];

	regPC += 3;
	return 16;
}

int z80cpu::dec_ix() {

	regIXL--;
	regIXH -= (regIXL >> 8);
	regIXL &= 0xff;
	regIXH &= 0xff;

	regPC++;
	return 6;
}

int z80cpu::inc_ixl() {

	regF = (regF & 1) | (((regIXL & 0xf) + 1) & 0x10);	//H,N,(C)

	regIXL = (regIXL + 1) & 0xff;

	regF |= (regIXL & 0xa8);					//S,5,3
	if (regIXL == 0x80) regF |= 4;				//V
	if (!regIXL) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::dec_ixl() {

	regF = (regF & 1) | (((regIXL & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (regIXL == 0x80) regF |= 4;				//V

	regIXL = (regIXL - 1) & 0xff;

	regF |= (regIXL & 0xa8);				//S,5,3
	if (!regIXL) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::ld_ixl_n() {

	regIXL = memory[(regPC + 1) & 0xffff];
	regPC += 2;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//DD 3x
////////////////////////////////////////////////////////////////////////////////


int z80cpu::inc_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | (((memory[regMEMPTR] & 0xf) + 1) & 0x10);	//H,N,(C)

	memory[regMEMPTR] = (memory[regMEMPTR] + 1) & 0xff;

	regF |= (memory[regMEMPTR] & 0xa8);				//S,5,3
	if (memory[regMEMPTR] == 0x80) regF |= 4;			//V
	if (!memory[regMEMPTR]) regF |= 0x40;				//Z

	regPC += 2;
	return 19;
}

int z80cpu::dec_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | (((memory[regMEMPTR] & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (memory[regMEMPTR] == 0x80) regF |= 4;			//V

	memory[regMEMPTR] = (memory[regMEMPTR] - 1) & 0xff;

	regF |= (memory[regMEMPTR] & 0xa8);				//S,5,3
	if (!memory[regMEMPTR]) regF |= 0x40;				//Z

	regPC += 2;
	return 19;
}

int z80cpu::ld_atixpd_n() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = memory[(regPC + 1) & 0xffff];

	regPC += 2;
	return 15;
}

int z80cpu::add_ix_sp() {

	int sp_lo = regSP & 0xff;
	int sp_hi = regSP >> 8;

	regMEMPTR = (regIXL + (regIXH << 8) + 1) & 0xffff;

	regIXL += sp_lo;

	regF = (regF & 0xc4) | (((regIXL >> 8) + (regIXH & 0xf) + (sp_hi & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIXH += (regIXL >> 8) + sp_hi;
	regIXL &= 0xff;

	regF = regF | (regIXH & 0x28) | (regIXH >> 8);					//set 5,3,C

	regIXH &= 0xff;

	regPC++;
	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//DD 6x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_ixh_r8() {

	regIXH = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 4;
}

int z80cpu::ld_ixh_ixl() {

	regIXH = regIXL;

	regPC++;
	return 4;
}

int z80cpu::ld_ixl_r8() {

	regIXL = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 4;
}

int z80cpu::ld_ixl_ixh() {

	regIXL = regIXH;

	regPC++;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//DD 7x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_atixpd_r8() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = *regPtr[memory[regPC] & 0x7];

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//DD 8x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::adx_a_ixh() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA + regIXH + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIXH & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~regIXH) & (regA ^ (regA + regIXH + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + regIXH + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}


int z80cpu::adx_a_ixl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA + regIXL + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIXL & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~regIXL) & (regA ^ (regA + regIXL + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + regIXL + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}

int z80cpu::adx_a_atixpd() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int addval = memory[regMEMPTR];

	regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~addval) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + addval + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC += 2;
	return 19;
}

////////////////////////////////////////////////////////////////////////////////
//DD 9x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::sbx_a_ixh() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA - regIXH - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIXH & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ regIXH) & (regA ^ (regA - regIXH - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - regIXH - oldcarry) & 0xff;

	regF |= (regA & 0xa8);							//set S,5,3 flags
	if (!regA) regF |= 0x40;							//set Z flag

	regPC++;
	return 4;
}

int z80cpu::sbx_a_ixl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA - regIXL - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIXL & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ regIXL) & (regA ^ (regA - regIXL - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - regIXL - oldcarry) & 0xff;

	regF |= (regA & 0xa8);		//set S,5,3 flags
	if (!regA) regF |= 0x40;							//set Z flag

	regPC++;
	return 4;
}

int z80cpu::sbx_a_atixpd() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int subval = memory[regMEMPTR];

	regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - subval - oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flags
	if (!regA) regF |= 0x40;

	regPC += 2;
	return 15;
}

////////////////////////////////////////////////////////////////////////////////
//DD Ax
////////////////////////////////////////////////////////////////////////////////

int z80cpu::and_ixh() {

	regA &= regIXH;
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::and_ixl() {

	regA &= regIXL;
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::and_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA &= memory[regMEMPTR];
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 15;
}

int z80cpu::xor_ixh() {

	regA = (regA ^ regIXH) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::xor_ixl() {

	regA = (regA ^ regIXL) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::xor_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA = (regA ^ memory[regMEMPTR]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//DD Bx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::or_ixh() {

	regA = (regA | regIXH) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::or_ixl() {

	regA = (regA | regIXL) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::or_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA = (regA | memory[regMEMPTR]) & 0xff;
	regF = (regA & 0xa8) | parity_tbl[regA];		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 15;
}

int z80cpu::cp_ixh() {

	regF = (((regA - regIXH) & 0x100) >> 8) | (((regA & 0xf) - (regIXH & 0xf)) & 0x10)		//H,C
		| ((((regA ^ regIXH) & (regA ^ (regA - regIXH))) & 0x80) >> 5) | 2		//V,N
		| ((regA - regIXH) & 0x80) | (regIXH & 0x28);					//S,5,3 flags
	if (!(regA - regIXH)) regF |= 0x40;						//Z

	regPC++;
	return 4;
}

int z80cpu::cp_ixl() {

	regF = (((regA - regIXL) & 0x100) >> 8) | (((regA & 0xf) - (regIXL & 0xf)) & 0x10)		//H,C
		| ((((regA ^ regIXL) & (regA ^ (regA - regIXL))) & 0x80) >> 5) | 2		//V,N
		| ((regA - regIXL) & 0x80) | (regIXL & 0x28);					//S,5,3 flags
	if (!(regA - regIXL)) regF |= 0x40;						//Z

	regPC++;
	return 4;
}

int z80cpu::cp_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int cpval = memory[regMEMPTR];

	regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)		//H,C
		| ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2		//V,N
		| ((regA - cpval) & 0x80) | (cpval & 0x28);					//S,5,3 flags
	if (!(regA - cpval)) regF |= 0x40;						//Z

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//DD Cx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::prefix_ddcb() {

	int ticks;

	if (memory[(regPC + 2) & 0xffff] >= 0xc0) ticks = set_x_atixpd();
	else if (memory[(regPC + 2) & 0xffff] >= 0x80) ticks = res_x_atixpd();
	else if (memory[(regPC + 2) & 0xffff] >= 0x40) ticks = bit_x_atixpd();
	else ticks = cpu_instructions_ddcb[static_cast<unsigned>(memory[(regPC + 2) & 0xffff] >> 3)](this);

	return ticks;
}


////////////////////////////////////////////////////////////////////////////////
//DD Ex
////////////////////////////////////////////////////////////////////////////////

int z80cpu::pop_ix() {

	regIXL = memory[regSP];
	regIXH = memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regPC++;
	return 10;
}

int z80cpu::ex_atsp_ix() {

	int temp = regIXL;
	regIXL = memory[regSP];
	memory[regSP] = temp;

	temp = regIXH;
	regIXH = memory[(regSP + 1) & 0xffff];
	memory[(regSP + 1) & 0xffff] = temp;

	regPC++;
	return 19;
}

int z80cpu::push_ix() {

	regSP = (regSP - 2) & 0xffff;
	memory[regSP] = regIXL;
	memory[(regSP + 1) & 0xffff] = regIXH;

	regPC++;
	return 11;
}

int z80cpu::jp_atix() {

	regPC = regIXL + (regIXH << 8);
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//DD Fx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_sp_ix() {

	regSP = regIXL + (regIXH << 8);

	regPC++;
	return 6;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX FD
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//GENERIC
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_r8_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	*regPtr[(memory[regPC] >> 3) & 0x7] = memory[regMEMPTR];

	regPC += 2;
	return 15;
}


int z80cpu::ld_r8_iyh() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = regIYH;

	regPC++;
	return 4;
}

int z80cpu::ld_r8_iyl() {

	*regPtr[(memory[regPC] >> 3) & 0x7] = regIYL;

	regPC++;
	return 4;
}

int z80cpu::add_iy_r16() {

	int *reg = regPtr[(memory[regPC] >> 3) & 0x7];

	regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

	regIYL += *reg;

	regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + ((*(reg - 1)) & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIYH += ((regIYL >> 8) + (*(reg - 1)));
	regIYL &= 0xff;

	regF = regF | (regIYH & 0x28) | (regIYH >> 8);					//set 5,3,C

	regIYH &= 0xff;

	regPC++;
	return 11;
}

////////////////////////////////////////////////////////////////////////////////
//FD 2x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_iy_nn() {

	regIYL = memory[(regPC + 1) & 0xffff];
	regIYH = memory[(regPC + 2) & 0xffff];
	regPC += 3;
	return 10;
}

int z80cpu::ld_atnn_iy() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	memory[addr] = regIYL;
	memory[regMEMPTR] = regIYH;

	regPC += 3;
	return 16;
}

int z80cpu::inc_iy() {

	regIYL++;
	regIYH += (regIYL >> 8);
	regIYL &= 0xff;
	regIYH &= 0xff;

	regPC++;
	return 6;
}

int z80cpu::inc_iyh() {

	regF = (regF & 1) | (((regIYH & 0xf) + 1) & 0x10);	//H,N,(C)

	regIYH = (regIYH + 1) & 0xff;

	regF |= (regIYH & 0xa8);					//S,5,3
	if (regIYH == 0x80) regF |= 4;				//V
	if (!regIYH) regF |= 0x40;				//Z					//Z

	regPC++;
	return 4;
}

int z80cpu::dec_iyh() {

	regF = (regF & 1) | (((regIYH & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (regIYH == 0x80) regF |= 4;				//V

	regIYH = (regIYH - 1) & 0xff;

	regF |= (regIYH & 0xa8);				//S,5,3
	if (!regIYH) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::ld_iyh_n() {

	regIYH = memory[(regPC + 1) & 0xffff];
	regPC += 2;
	return 4;
}

int z80cpu::add_iy_iy() {

	regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

	int temp = regIYL;
	regIYL += regIYL;

	regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + (temp & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIYH += ((regIYL >> 8) + regIYH);
	regIYL &= 0xff;

	regF = regF | (regIYH & 0x28) | (regIYH >> 8);					//set 5,3,C

	regIYH &= 0xff;

	regPC++;
	return 11;
}

int z80cpu::ld_iy_atnn() {

	int addr = memory[(regPC + 1) & 0xffff] + (memory[(regPC + 2) & 0xffff] << 8);

	regMEMPTR = (addr + 1) & 0xffff;
	regIYL = memory[addr];
	regIYH = memory[regMEMPTR];

	regPC += 3;
	return 16;
}

int z80cpu::dec_iy() {

	regIYL--;
	regIYH -= (regIYL >> 8);
	regIYL &= 0xff;
	regIYH &= 0xff;

	regPC++;
	return 6;
}

int z80cpu::inc_iyl() {

	regF = (regF & 1) | (((regIYL & 0xf) + 1) & 0x10);	//H,N,(C)

	regIYL = (regIYL + 1) & 0xff;

	regF |= (regIYL & 0xa8);				//S,5,3
	if (regIYL == 0x80) regF |= 4;				//V
	if (!regIYL) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::dec_iyl() {

	regF = (regF & 1) | (((regIYL & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (regIYL == 0x80) regF |= 4;				//V

	regIYL = (regIYL - 1) & 0xff;

	regF |= (regIYL & 0xa8);				//S,5,3
	if (!regIYL) regF |= 0x40;				//Z

	regPC++;
	return 4;
}

int z80cpu::ld_iyl_n() {

	regIYL = memory[(regPC + 1) & 0xffff];
	regPC += 2;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//FD 3x
////////////////////////////////////////////////////////////////////////////////


int z80cpu::inc_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | (((memory[regMEMPTR] & 0xf) + 1) & 0x10);	//H,N,(C)

	memory[regMEMPTR] = (memory[regMEMPTR] + 1) & 0xff;

	regF |= (memory[regMEMPTR] & 0xa8);				//S,5,3
	if (memory[regMEMPTR] == 0x80) regF |= 4;			//V
	if (!memory[regMEMPTR]) regF |= 0x40;				//Z

	regPC += 2;
	return 19;
}

int z80cpu::dec_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | (((memory[regMEMPTR] & 0xf) - 1) & 0x10) | 2;	//H,N,(C)
	if (memory[regMEMPTR] == 0x80) regF |= 4;			//V

	memory[regMEMPTR] = (memory[regMEMPTR] - 1) & 0xff;

	regF |= (memory[regMEMPTR] & 0xa8);				//S,5,3
	if (!memory[regMEMPTR]) regF |= 0x40;				//Z

	regPC += 2;
	return 19;
}

int z80cpu::ld_atiypd_n() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = memory[(regPC + 1) & 0xffff];

	regPC += 2;
	return 15;
}

int z80cpu::add_iy_sp() {

	int sp_lo = regSP & 0xff;
	int sp_hi = regSP >> 8;

	regMEMPTR = (regIYL + (regIYH << 8) + 1) & 0xffff;

	regIYL += sp_lo;

	regF = (regF & 0xc4) | (((regIYL >> 8) + (regIYH & 0xf) + (sp_hi & 0xf)) & 0x10);	//clear 5,3,N; preserve S,Z,PV; set H

	regIYH += (regIYL >> 8) + sp_hi;
	regIYL &= 0xff;

	regF = regF | (regIYH & 0x28) | (regIYH >> 8);					//set 5,3,C

	regIYH &= 0xff;

	regPC++;
	return 11;
}


////////////////////////////////////////////////////////////////////////////////
//FD 6x
////////////////////////////////////////////////////////////////////////////////


int z80cpu::ld_iyh_r8() {

	regIYH = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 4;
}

int z80cpu::ld_iyh_iyl() {

	regIYH = regIYL;

	regPC++;
	return 4;
}

int z80cpu::ld_iyl_r8() {

	regIYL = *regPtr[memory[regPC] & 0x7];

	regPC++;
	return 4;
}

int z80cpu::ld_iyl_iyh() {

	regIYL = regIYH;

	regPC++;
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//FD 7x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_atiypd_r8() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = *regPtr[memory[regPC] & 0x7];

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//FD 8x
////////////////////////////////////////////////////////////////////////////////


int z80cpu::adx_a_iyh() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA + regIYH + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIYH & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~regIYH) & (regA ^ (regA + regIYH + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + regIYH + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}


int z80cpu::adx_a_iyl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA + regIYL + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (regIYL & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~regIYL) & (regA ^ (regA + regIYL + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + regIYL + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC++;
	return 4;
}

int z80cpu::adx_a_atiypd() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int addval = memory[regMEMPTR];

	regF = (((regA + addval + oldcarry) & 0x100) >> 8) | (((regA & 0xf) + (addval & 0xf) + oldcarry) & 0x10)
		| ((((regA ^ ~addval) & (regA ^ (regA + addval + oldcarry))) & 0x80) >> 5);		//set H,V,C flags

	regA = (regA + addval + oldcarry) & 0xff;

	regF |= (regA & 0xa8);					//set S,5,3 flag
	if (!regA) regF |= 0x40;				//set Z flag

	regPC += 2;
	return 15;
}



////////////////////////////////////////////////////////////////////////////////
//FD 9x
////////////////////////////////////////////////////////////////////////////////

int z80cpu::sbx_a_iyh() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA - regIYH - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIYH & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ regIYH) & (regA ^ (regA - regIYH - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - regIYH - oldcarry) & 0xff;

	regF |= (regA & 0xa8);		//set S,5,3 flags
	if (!regA) regF |= 0x40;							//set Z flag

	regPC++;
	return 4;
}

int z80cpu::sbx_a_iyl() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regF = (((regA - regIYL - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (regIYL & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ regIYL) & (regA ^ (regA - regIYL - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - regIYL - oldcarry) & 0xff;

	regF |= (regA & 0xa8);		//set S,5,3 flags
	if (!regA) regF |= 0x40;							//set Z flag

	regPC++;
	return 4;
}

int z80cpu::sbx_a_atiypd() {

	int oldcarry = (regF & 1) & (memory[regPC] >> 3);

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int subval = memory[regMEMPTR];

	regF = (((regA - subval - oldcarry) & 0x100) >> 8) | (((regA & 0xf) - (subval & 0xf) - oldcarry) & 0x10)
		| ((((regA ^ subval) & (regA ^ (regA - subval - oldcarry))) & 0x80) >> 5) | 2;		//set H,V,C flags

	regA = (regA - subval - oldcarry) & 0xff;

	regF |= (regA & 0xa8);		//set S,5,3 flags
	if (!regA) regF |= 0x40;

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//FD Ax
////////////////////////////////////////////////////////////////////////////////

int z80cpu::and_iyh() {

	regA &= regIYH;
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::and_iyl() {

	regA &= regIYL;
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::and_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA &= memory[regMEMPTR];
	regF = (regA & 0xa8) | parity_tbl[regA] | 0x10;		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 15;
}

int z80cpu::xor_iyh() {

	regA = (regA ^ regIYH) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::xor_iyl() {

	regA = (regA ^ regIYL) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::xor_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA = (regA ^ memory[regMEMPTR]) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//FD Bx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::or_iyh() {

	regA = (regA | regIYH) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::or_iyl() {

	regA = (regA | regIYL) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 4;
}

int z80cpu::or_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regA = (regA | memory[regMEMPTR]) & 0xff;
	regF = (regA & 0xa8) | ((~(regA ^ (regA >> 4) ^ (regA >> 2) ^ (regA >> 1)) & 1) << 2);		//set S,5,H,3,P,N,C flags
	if (!regA) regF |= 0x40;		//set Z flag

	regPC++;
	return 15;
}

int z80cpu::cp_iyh() {

	regF = (((regA - regIYH) & 0x100) >> 8) | (((regA & 0xf) - (regIYH & 0xf)) & 0x10)		//H,C
		| ((((regA ^ regIYH) & (regA ^ (regA - regIYH))) & 0x80) >> 5) | 2		//V,N
		| ((regA - regIYH) & 0x80) | (regIYH & 0x28);					//S,5,3 flags
	if (!(regA - regIYH)) regF |= 0x40;						//Z

	regPC++;
	return 4;
}

int z80cpu::cp_iyl() {

	regF = (((regA - regIYL) & 0x100) >> 8) | (((regA & 0xf) - (regIYL & 0xf)) & 0x10)		//H,C
		| ((((regA ^ regIYL) & (regA ^ (regA - regIYL))) & 0x80) >> 5) | 2		//V,N
		| ((regA - regIYL) & 0x80) | (regIYL & 0x28);					//S,5,3 flags
	if (!(regA - regIYL)) regF |= 0x40;						//Z

	regPC++;
	return 4;
}

int z80cpu::cp_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int cpval = memory[regMEMPTR];

	regF = (((regA - cpval) & 0x100) >> 8) | (((regA & 0xf) - (cpval & 0xf)) & 0x10)		//H,C
		| ((((regA ^ cpval) & (regA ^ (regA - cpval))) & 0x80) >> 5) | 2		//V,N
		| ((regA - cpval) & 0x80) | (cpval & 0x28);					//S,5,3 flags
	if (!(regA - cpval)) regF |= 0x40;						//Z

	regPC += 2;
	return 15;
}


////////////////////////////////////////////////////////////////////////////////
//FD Cx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::prefix_fdcb() {

	int ticks;

	if (memory[(regPC + 2) & 0xffff] >= 0xc0) ticks = set_x_atiypd();
	else if (memory[(regPC + 2) & 0xffff] >= 0x80) ticks = res_x_atiypd();
	else if (memory[(regPC + 2) & 0xffff] >= 0x40) ticks = bit_x_atiypd();
	else ticks = cpu_instructions_fdcb[static_cast<unsigned>(memory[(regPC + 2) & 0xffff] >> 3)](this);

	return ticks;
}


////////////////////////////////////////////////////////////////////////////////
//FD Ex
////////////////////////////////////////////////////////////////////////////////

int z80cpu::pop_iy() {

	regIYL = memory[regSP];
	regIYH = memory[(regSP + 1) & 0xffff];
	regSP = (regSP + 2) & 0xffff;

	regPC++;
	return 10;
}

int z80cpu::ex_atsp_iy() {

	int temp = regIYL;
	regIYL = memory[regSP];
	memory[regSP] = temp;

	temp = regIYH;
	regIYH = memory[(regSP + 1) & 0xffff];
	memory[(regSP + 1) & 0xffff] = temp;

	regPC++;
	return 19;
}

int z80cpu::push_iy() {

	regSP = (regSP - 2) & 0xffff;
	memory[regSP] = regIYL;
	memory[(regSP + 1) & 0xffff] = regIYH;

	regPC++;
	return 11;
}

int z80cpu::jp_atiy() {

	regPC = regIYL + (regIYH << 8);
	return 4;
}


////////////////////////////////////////////////////////////////////////////////
//FD Fx
////////////////////////////////////////////////////////////////////////////////

int z80cpu::ld_sp_iy() {

	regSP = regIYL + (regIYH << 8);

	regPC++;
	return 6;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX DDCB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int z80cpu::rlc_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = memory[regMEMPTR] << 1;
	memory[regMEMPTR] = (memory[regMEMPTR] | (memory[regMEMPTR] >> 8)) & 0xff;

	regF = (memory[regMEMPTR] & 0xa9)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rrc_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int oldcarry = memory[regMEMPTR] & 1;
	memory[regMEMPTR] = memory[regMEMPTR] >> 1;
	memory[regMEMPTR] |= (oldcarry << 8);

	regF = (memory[regMEMPTR] & 0xa8) | oldcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 1) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rl_atixpd() {

	int oldcarry = regF & 1;
	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = ((memory[regMEMPTR] << 1) | oldcarry);

	oldcarry = memory[regMEMPTR] >> 8;
	memory[regMEMPTR] &= 0xff;

	regF = (memory[regMEMPTR] & 0xa8) | oldcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rr_atixpd() {

	int newcarry = memory[regMEMPTR] & 1;
	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = ((memory[regMEMPTR] >> 1) | ((regF & 1) << 7));

	regF = (memory[regMEMPTR] & 0xa8) | newcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sla_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] >> 7;

	memory[regMEMPTR] = (memory[regMEMPTR] << 1) & 0xff;

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sra_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] & 1;

	memory[regMEMPTR] = (memory[regMEMPTR] & 0x80) | (memory[regMEMPTR] >> 1);

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sll_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] >> 7;

	memory[regMEMPTR] = ((memory[regMEMPTR] << 1) | 1) & 0xff;

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::srl_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] & 1;

	memory[regMEMPTR] = (memory[regMEMPTR] >> 1);

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::bit_x_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);						//set 5,H,3, preserve C

	if (!(memory[regMEMPTR] & (1 << ((memory[(regPC + 2) & 0xffff] - 0x40) >> 3)))) regF |= 0x44;	//set Z,PV flags
	else if (((memory[(regPC + 2) & 0xffff] - 0x40) >> 3) == 7) regF |= 0x80;

	regPC += 3;
	return 16;
}

int z80cpu::res_x_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] &= (((1 << ((memory[(regPC + 2) & 0xffff] - 0x80) >> 3)) ^ 0xff) & 0xff);

	regPC += 3;
	return 19;
}

int z80cpu::set_x_atixpd() {

	regMEMPTR = (regIXL + (regIXH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] |= (1 << ((memory[(regPC + 2) & 0xffff] - 0xc0) >> 3));

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                       PREFIX FDCB
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////


int z80cpu::rlc_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = memory[regMEMPTR] << 1;
	memory[regMEMPTR] = (memory[regMEMPTR] | (memory[regMEMPTR] >> 8)) & 0xff;

	regF = (memory[regMEMPTR] & 0xa9)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rrc_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	int oldcarry = memory[regMEMPTR] & 1;
	memory[regMEMPTR] = memory[regMEMPTR] >> 1;
	memory[regMEMPTR] |= (oldcarry << 8);

	regF = (memory[regMEMPTR] & 0xa8) | oldcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 1) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rl_atiypd() {

	int oldcarry = regF & 1;
	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = ((memory[regMEMPTR] << 1) | oldcarry);

	oldcarry = memory[regMEMPTR] >> 8;
	memory[regMEMPTR] &= 0xff;

	regF = (memory[regMEMPTR] & 0xa8) | oldcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::rr_atiypd() {

	int newcarry = memory[regMEMPTR] & 1;
	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] = ((memory[regMEMPTR] >> 1) | ((regF & 1) << 7));

	regF = (memory[regMEMPTR] & 0xa8) | newcarry
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sla_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] >> 7;

	memory[regMEMPTR] = (memory[regMEMPTR] << 1) & 0xff;

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sra_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] & 1;

	memory[regMEMPTR] = (memory[regMEMPTR] & 0x80) | (memory[regMEMPTR] >> 1);

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::sll_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] >> 7;

	memory[regMEMPTR] = ((memory[regMEMPTR] << 1) | 1) & 0xff;

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::srl_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = memory[regMEMPTR] & 1;

	memory[regMEMPTR] = (memory[regMEMPTR] >> 1);

	regF = regF | (memory[regMEMPTR] & 0xa8)
		| ((~(memory[regMEMPTR] ^ (memory[regMEMPTR] >> 4) ^ (memory[regMEMPTR] >> 2) ^ (memory[regMEMPTR] >> 1)) & 1) << 2);		//S,5,H,3,P,N,C
	if (!memory[regMEMPTR]) regF |= 0x40;	//Z

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}

int z80cpu::bit_x_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	regF = (regF & 1) | 0x10 | ((regMEMPTR >> 8) & 0x28);						//set 5,H,3, preserve C

	if (!(memory[regMEMPTR] & (1 << ((memory[(regPC + 2) & 0xffff] - 0x40) >> 3)))) regF |= 0x44;	//set Z,PV flags
	else if (((memory[(regPC + 2) & 0xffff] - 0x40) >> 3) == 7) regF |= 0x80;

	regPC += 3;
	return 16;
}

int z80cpu::res_x_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] &= (((1 << ((memory[(regPC + 2) & 0xffff] - 0x80) >> 3)) ^ 0xff) & 0xff);

	regPC += 3;
	return 19;
}

int z80cpu::set_x_atiypd() {

	regMEMPTR = (regIYL + (regIYH << 8) + (((-(memory[(regPC + 1) & 0xffff] >> 7)) & 0xffffff00) | memory[(regPC + 1) & 0xffff])) & 0xffff;

	memory[regMEMPTR] |= (1 << ((memory[(regPC + 2) & 0xffff] - 0xc0) >> 3));

	*regPtr[memory[(regPC + 2) & 0xffff] & 0x7] = memory[regMEMPTR];

	regPC += 3;
	return 19;
}
