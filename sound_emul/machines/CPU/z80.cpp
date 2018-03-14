// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "z80.h"

using std::array;
using std::cout;
using std::setfill;
using std::setw;
using std::hex;
using std::dec;
using std::endl;
using std::boolalpha;

z80cpu::z80cpu(std::array<int, 0x10000> *mem, Z80Type z80type): memory(mem) {
    cpuType = z80type;
    inputPorts.fill(0);
    inputPortsShort.fill(0);
    outputPorts.fill(0);
    outputPortsShort.fill(0);

    reset();
}


const std::array<const int, 4> z80cpu::conditionCodes = {{0x40, 0x01, 0x04, 0x80}};
const std::array<int, 256> z80cpu::parityTable = z80cpu::init_parity_table();


std::array<int, 256> z80cpu::init_parity_table() {
    std::array<int, 256> pTab;
    for (int i = 0; i <= 255; i++) {
        int j, par;
        for (j = i, par = 0; j != 0; j >>= 1) par ^= (j & 1);
        pTab[i] = (par ? 0 : 4);
    }

    return pTab;
}

const std::array<unsigned, 256> z80cpu::instructionTimingsRegular = {{
    4, 10, 7, 6, 4, 4, 7, 4, 4, 11, 7, 6, 4, 4, 7, 4,
    8, 10, 7, 6, 4, 4, 7, 4, 12, 11, 7, 6, 4, 4, 7, 4,
    7, 10, 16, 6, 4, 4, 7, 4, 7, 11, 16, 6, 4, 4, 7, 4,
    7, 10, 13, 6, 7, 7, 10, 4, 7, 11, 13, 6, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    7, 7, 7, 7, 7, 7, 4, 7, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
    5, 10, 10, 10, 10, 11, 7, 11, 5, 10, 10, 4, 10, 17, 7, 11,
    5, 10, 10, 11, 10, 11, 7, 11, 5, 4, 10, 11, 10, 4, 7, 11,
    5, 10, 10, 19, 10, 11, 7, 11, 5, 4, 10, 4, 10, 4, 7, 11,
    5, 10, 10, 4, 10, 11, 7, 11, 5, 6, 10, 4, 10, 4, 7, 11
}};
const std::array<unsigned, 256> z80cpu::instructionTimingsDD_FD = {{
    4, 10, 7, 6, 4, 4, 7, 4, 4, 11, 7, 6, 4, 4, 7, 4,
    8, 10, 7, 6, 4, 4, 7, 4, 12, 11, 7, 6, 4, 4, 7, 4,
    7, 10, 16, 6, 4, 4, 7, 4, 7, 11, 16, 6, 4, 4, 7, 4,
    7, 10, 13, 6, 19, 19, 15, 4, 7, 11, 7, 6, 4, 4, 7, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    15, 15, 15, 15, 15, 15, 4, 15, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    4, 4, 4, 4, 4, 4, 15, 4, 4, 4, 4, 4, 4, 4, 15, 4,
    5, 10, 10, 10, 5, 11, 7, 11, 5, 10, 10, 4, 10, 17, 7, 11,
    5, 10, 10, 11, 5, 11, 7, 11, 5, 4, 10, 11, 10, 4, 7, 11,
    5, 10, 10, 19, 10, 11, 7, 11, 5, 4, 10, 4, 10, 4, 7, 11,
    5, 10, 10, 4, 10, 11, 7, 11, 10, 6, 10, 4, 10, 4, 7, 11
}};
const std::array<unsigned, 256> z80cpu::instructionTimingsED = {{
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    8, 8, 11, 16, 4, 10, 4, 5, 8, 8, 11, 16, 4, 10, 4, 5,
    8, 8, 11, 16, 4, 10, 4, 5, 8, 8, 11, 16, 4, 10, 4, 5,
    8, 8, 11, 16, 4, 10, 4, 14, 8, 8, 11, 16, 4, 10, 4, 14,
    8, 8, 11, 16, 4, 10, 4, 4, 8, 8, 11, 16, 4, 10, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    12, 12, 12, 12, 4, 4, 4, 4, 12, 12, 12, 12, 4, 4, 4, 4,
    12, 12, 12, 12, 4, 4, 4, 4, 12, 12, 12, 12, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
}};


unsigned z80cpu::get_instruction_timing(int instructionPointer) noexcept {
    int instruction = memory->at(instructionPointer);

    if (instruction == 0xcb) {
        return 8;
    } else if (instruction == 0xeb) {
        instructionPointer = (instructionPointer + 1) & 0xffff;
        instruction = memory->at(instructionPointer);
        unsigned cycles = 4;
        if (instruction == 0xb0 || instruction == 0xb1 || instruction == 0xb8 || instruction == 0xb9) {
            if (regB | regC) cycles += 5;
        } else if (instruction == 0xb2 || instruction == 0xb3 || instruction == 0xba || instruction == 0xbb) {
            if (regB) cycles += 5;
        }
        return cycles + instructionTimingsED[instruction];
    } else if (instruction == 0xdd || instruction == 0xfd) {
        unsigned cycles = 0;
        do {
            ++instructionPointer;
            instruction = memory->at(instructionPointer);
            cycles += 4;
        } while (instructionPointer >= 0xffff && (instruction == 0xdd || instruction == 0xfd));
        if (instruction == 0xcb) {          // ddcb/fdcb prefix
            instructionPointer = (instructionPointer + 1) & 0xffff;
            instruction = memory->at(instructionPointer);
            if (instruction < 0x40 || instruction > 0x7f) return cycles + 19;
            return cycles + 16;
        }
        return cycles + instructionTimingsDD_FD[instruction] + get_conditional_timing(instruction);     // dd/fd prefix
    }

    return instructionTimingsRegular[instruction] + get_conditional_timing(instruction);
}

unsigned z80cpu::get_conditional_timing(const int instruction) noexcept {
    switch (instruction) {
        case 0x10:  // djnz
            if (regB) return 5;
            break;
        case 0x20:  // jr nz
            if (!(regF & 0x40)) return 5;
            break;
        case 0x28:  // jr z
            if (regF & 0x40) return 5;
            break;
        case 0x30:  // jr nc
            if (!(regF & 1)) return 5;
            break;
        case 0x38:  // jr c
            if (regF & 1) return 5;
            break;
        case 0xc0:  // ret nz
            if (!(regF & 0x40)) return 6;
            break;
        case 0xc4:  // call nz
            if (!(regF & 0x40)) return 7;
            break;
        case 0xc8:  // ret z
            if (regF & 0x40) return 6;
            break;
        case 0xcc:  // call z
            if (regF & 0x40) return 7;
            break;
        case 0xd0:  // ret nc
            if (!(regF & 1)) return 6;
            break;
        case 0xd4:  // call nc
            if (!(regF & 1)) return 7;
            break;
        case 0xd8:  // ret c
            if (regF & 1) return 6;
            break;
        case 0xdc:  // call c
            if (regF & 1) return 7;
            break;
        case 0xe0:  // ret po
            if (!(regF & 4)) return 6;
            break;
        case 0xe4:  // call po
            if (!(regF & 4)) return 7;
            break;
        case 0xe8:  // ret pe
            if (regF & 4) return 6;
            break;
        case 0xec:  // call pe
            if (regF & 4) return 7;
            break;
        case 0xf0:  // ret p
            if (!(regF & 0x80)) return 6;
            break;
        case 0xf4:  // call p
            if (!(regF & 0x80)) return 7;
            break;
        case 0xf8:  // ret m
            if (regF & 0x80) return 6;
            break;
        case 0xfc:  // call m
            if (regF & 0x80) return 7;
            break;
    }

    return 0;
}

void z80cpu::setPC(int startAddress) noexcept {
    regPC = startAddress & 0xffff;
    instructionCycles = get_instruction_timing(regPC);
}

void z80cpu::setSP(int address) noexcept {
    regSP = address & 0xffff;
    return;
}

int z80cpu::getPC() noexcept {
    return regPC;
}


void z80cpu::request_non_maskable_interrupt() noexcept {
    interruptTypeRequested = Z80InterruptType::NMI;
}

void z80cpu::request_maskable_interrupt() noexcept {
    if (interruptTypeRequested != Z80InterruptType::NMI) interruptTypeRequested = Z80InterruptType::REGULAR;
}

unsigned z80cpu::acknowledge_interrupt_and_get_timing() noexcept {
    interruptTypeAcknowledged = interruptTypeRequested;
    interruptTypeRequested = Z80InterruptType::NONE;
    if (interruptTypeAcknowledged == Z80InterruptType::NMI) return 11;
    if (interruptMode == 0 || !regIFF1) {
        interruptTypeAcknowledged = Z80InterruptType::NONE;
        return get_instruction_timing(regPC);
    } else if (interruptMode == 1) {
        return 13;
    } else {
        return 19;
    }
}

void z80cpu::do_interrupt() noexcept {
    regSP = (regSP - 2) & 0xffff;
    memory->at(regSP) = regPC & 0xff;
    memory->at((regSP + 1) & 0xffff) = (regPC & 0xff00) >> 8;

    if (interruptTypeRequested == Z80InterruptType::NMI) {
        regPC = 0x66;
        regMEMPTR = regPC;
        regIFF1 = false;
    } else {
        if (interruptMode == 1) {
            regPC = 0x38;
            regMEMPTR = regPC;
        } else {
            regMEMPTR = (regI << 8) | 0xff;
            regPC = (memory->at(regMEMPTR) << 8) | memory->at(regMEMPTR);
            regMEMPTR = regPC;
        }
    }

    interruptTypeAcknowledged = Z80InterruptType::NONE;
    instructionCycles = get_instruction_timing(regPC);
}

void z80cpu::execute_cycle() noexcept {
    if (instructionCycles) {
        instructionCycles--;
        return;
    }

    regR = ((regR & 0x80) | ((regR & 0x7f) + 1));

    if (interruptTypeAcknowledged == Z80InterruptType::NONE) {
        cpu_instructions[(memory->at(regPC)) & 0xff](this);
    } else {
        do_interrupt();
    }
    if (interruptTypeRequested == Z80InterruptType::NONE) {
        instructionCycles = get_instruction_timing(regPC);
    } else {
        instructionCycles = acknowledge_interrupt_and_get_timing();
    }
    regPC &= 0xffff;
}


void z80cpu::execute_debug() {
    cout << hex << "A:   " << setfill('0') << setw(2) << regA;
    cout << "\tF:   ";
    debugger_print_flags(regF);
    cout << "\tA':  " << setfill('0') << setw(2) << regAs;
    cout << "\tF':  ";
    debugger_print_flags(regFs);

    cout << "\tI:   ";
    cout << setfill('0') << setw(2) << regI;
    cout << "\tR:   ";
    cout << setfill('0') << setw(2) << regR;
    cout << "\tIX:  " << setfill('0') << setw(2) << regIXH << setfill('0') << setw(2) << regIXL;
    cout << "\tIY:  " << setfill('0') << setw(2) << regIYH << setfill('0') << setw(2) << regIYL;
    cout << "\tSP:  " << setfill('0') << setw(4) << regSP << endl;
    cout << "PC:  " << setfill('0') << setw(4) << regPC;
    cout << "\tBC:  " << setfill('0') << setw(2) << regB << setfill('0') << setw(2) << regC;
    cout << "\tDE:  " << setfill('0') << setw(2) << regD << setfill('0') << setw(2) << regE;
    cout << "\tHL:  " << setfill('0') << setw(2) << regH << setfill('0') << setw(2) << regL;
    cout << "\tBC': " << setfill('0') << setw(2) << regBs << setfill('0') << setw(2) << regCs;
    cout << "\tDE': " << setfill('0') << setw(2) << regDs << setfill('0') << setw(2) << regEs;
    cout << "\tHL': " << setfill('0') << setw(2) << regHs << setfill('0') << setw(2) << regLs << endl;
    cout << boolalpha << "EI: " << regIFF1 << "\tIM: " << interruptMode << "\tIFF1: " << regIFF1
        << "\tIFF2: " << regIFF2 << "\tMEMPTR: " << setfill('0') << setw(4) << regMEMPTR << endl;


    instructionCycles = get_instruction_timing(regPC);

    cout << hex << "at " << setfill('0') << setw(4) << regPC;
    cout << " exec " << setfill('0') << setw(2) << memory->at(regPC);

    if ((memory->at(regPC) == 0xed) | (memory->at(regPC) == 0xcb)) cout << memory->at((regPC + 1) & 0xffff);
    if ((memory->at(regPC) == 0xdd) | (memory->at(regPC) == 0xfd)) {
        if (memory->at((regPC + 1) & 0xffff) == 0xcb) {
            cout << "cb" << setfill('0') << setw(2) << memory->at((regPC + 2) & 0xffff)
            << memory->at((regPC + 3) & 0xffff);
        } else {
            int i = 1;

            while ((memory->at((regPC + i) & 0xffff) == 0xdd) | (memory->at((regPC + i) & 0xffff) == 0xfd)) {
                cout << setfill('0') << setw(2) << memory->at((regPC + i) & 0xffff);
                i++;
            }

            cout << setfill('0') << setw(2) << memory->at((regPC + i) & 0xffff);
        }
    }

    regR = ((regR & 0x80) | ((regR & 0x7f) + 1));

    cpu_instructions[static_cast<unsigned>(memory->at(regPC))](this);

    cout << dec << ", ticks: " << instructionCycles << hex << endl;

    regPC &= 0xffff;
}

void z80cpu::debugger_print_flags(const int flagRegister) noexcept {
    if (flagRegister & 0x80) {
        cout << "S";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x40) {
        cout << "Z";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x20) {
        cout << "5";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x10) {
        cout << "H";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x8) {
        cout << "3";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x4) {
        cout << "V";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x2) {
        cout << "N";
    } else {
        cout << "-";
    }
    if (flagRegister & 0x1) {
        cout << "C";
    } else {
        cout << "-";
    }
}


void z80cpu::reset() noexcept {
    regA = 0xff;
    regB = 0;
    regC = 0;
    regD = 0;
    regE = 0;
    regF = 0xff;
    regH = 0;
    regL = 0;
    regAs = 0;
    regBs = 0;
    regCs = 0;
    regDs = 0;
    regEs = 0;
    regFs = 0;
    regHs = 0;
    regLs = 0;
    regI = 0;
    regR = 0;
    regIXH = 0;
    regIXL = 0;
    regIYH = 0;
    regIYL = 0;
    regSP = 0xffff;
    regPC = 0;

    regMEMPTR = 0;
    regQ = 0;
    regDummy = 0;

    interruptMode = 0;
    interruptTypeRequested = Z80InterruptType::NONE;
    interruptTypeAcknowledged = Z80InterruptType::NONE;
    regIFF1 = false;
    regIFF2 = false;

    instructionCycles = get_instruction_timing(regPC);
}
