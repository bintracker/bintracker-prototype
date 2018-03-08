//uCPUlibs-z80 v0.1
//by utz

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "z80.h"

using namespace std;


z80cpu::z80cpu(std::array<int, 0x10000> *mem, Z80Type z80type) {

	memory = mem;
	cpuType = z80type;

    inputPorts.fill(0);
    inputPortsShort.fill(0);
    outputPorts.fill(0);
    outputPortsShort.fill(0);

	reset();
}


z80cpu::~z80cpu() {

}

const std::array<const int, 4> z80cpu::conditionCodes = {{0x40, 0x01, 0x04, 0x80}};
const std::array<int, 256> z80cpu::parityTable = z80cpu::init_parity_table();


std::array<int, 256> z80cpu::init_parity_table() {

    std::array<int, 256> parityTable;
	for (int i = 0; i <= 255; i++) {
		int j, par;
		for (j = i, par = 0; j != 0; j >>= 1) par ^= (j & 1);
		parityTable[i] = (par ? 0 : 4);
	}

	return parityTable;
}


void z80cpu::setPC(int startAddress) {

	regPC = startAddress & 0xffff;
	return;
}

void z80cpu::setSP(int address) {

	regSP = address & 0xffff;
	return;
}

int z80cpu::getPC() { return regPC; }


int z80cpu::non_maskable_interrupt() {

	//TODO
	return 0;
}

int z80cpu::maskable_interrupt() {

	//TODO
	return 0;
}


void z80cpu::execute_cycle() {

	if (instructionCycles) {

		instructionCycles--;
		return;
	}

	regR = ((regR & 0x80) | ((regR & 0x7f) + 1));

	instructionCycles = cpu_instructions[(memory->at(regPC)) & 0xff](this);
	regPC &= 0xffff;

	return;
}


void z80cpu::execute_debug() {

	cout << hex << "A:   " << setfill('0') << setw(2) << regA;
	cout << "\tF:   ";
	if (regF & 0x80) cout << "S";
	else cout << "-";
	if (regF & 0x40) cout << "Z";
	else cout << "-";
	if (regF & 0x20) cout << "5";
	else cout << "-";
	if (regF & 0x10) cout << "H";
	else cout << "-";
	if (regF & 0x8) cout << "3";
	else cout << "-";
	if (regF & 0x4) cout << "V";
	else cout << "-";
	if (regF & 0x2) cout << "N";
	else cout << "-";
	if (regF & 0x1) cout << "C";
	else cout << "-";
	cout << "\tA':  " << setfill('0') << setw(2) << regAs;
	cout << "\tF':  ";
	if (regFs & 0x80) cout << "S";
	else cout << "-";
	if (regFs & 0x40) cout << "Z";
	else cout << "-";
	if (regFs & 0x20) cout << "5";
	else cout << "-";
	if (regFs & 0x10) cout << "H";
	else cout << "-";
	if (regFs & 0x8) cout << "3";
	else cout << "-";
	if (regFs & 0x4) cout << "V";
	else cout << "-";
	if (regFs & 0x2) cout << "N";
	else cout << "-";
	if (regFs & 0x1) cout << "C";
	else cout << "-";

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
	cout << boolalpha << "EI: " << interruptsEnabled << "\tIM: " << interruptMode << "\tIFF1: " << regIFF1 << "\tIFF2: " << regIFF2
		<< "\tMEMPTR: " << setfill('0') << setw(4) << regMEMPTR << endl;


	instructionCycles = 0;

	cout << hex << "at " << setfill('0') << setw(4) << regPC;
	cout << " exec " << setfill('0') << setw(2) << memory->at(regPC);

	if ((memory->at(regPC) == 0xed) | (memory->at(regPC) == 0xcb)) cout << memory->at((regPC + 1) & 0xffff);
	if ((memory->at(regPC) == 0xdd) | (memory->at(regPC) == 0xfd)) {

		if (memory->at((regPC + 1) & 0xffff) == 0xcb) cout << "cb" << setfill('0') << setw(2)
			<< memory->at((regPC + 2) & 0xffff) << memory->at((regPC + 3) & 0xffff);
		else {
			int i = 1;

			while ((memory->at((regPC + i) & 0xffff) == 0xdd) | (memory->at((regPC + i) & 0xffff) == 0xfd)) {

				cout << setfill('0') << setw(2) << memory->at((regPC + i) & 0xffff);
				i++;
			}

			cout << setfill('0') << setw(2) << memory->at((regPC + i) & 0xffff);
		}
	}

	regR = ((regR & 0x80) | ((regR & 0x7f) + 1));

	int ticks = cpu_instructions[static_cast<unsigned>(memory->at(regPC))](this);

	cout << dec << ", ticks: " << ticks << hex << endl;

	regPC &= 0xffff;
}


void z80cpu::reset() {

	regA = 0;
	regB = 0;
	regC = 0;
	regD = 0;
	regE = 0;
	regF = 0;
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
	regSP = 0;
	regPC = 0;

	regMEMPTR = 0;
	regQ = 0;
	regDummy = 0;

	interruptsEnabled = false;
	interruptMode = 0;
	regIFF1 = false;
	regIFF2 = false;

	instructionCycles = 0;
}
