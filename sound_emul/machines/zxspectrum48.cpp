#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdio>
//#include <samplerate.h>
//#include <chrono>

#include "zxspectrum48.h"

using namespace std;

//TODO: badness! by passing memory.data(), size information is lost
Virtual_ZX48::Virtual_ZX48() : cpu(&memory, Z80Type::NMOS) {

	memory.fill(0);
//	insertVirtualOSReturn(0x8000);		//TODO magic number
	cpu.inputPortsShort[0xfe] = 0xff;	//return no_key_down on keyboard checks

	ifstream ROMFILE("resources/roms/zxspectrum48.rom", ios::binary);
	if (ROMFILE.is_open()) {

		char buffer[0x4000];
		ROMFILE.seekg(0, ios::beg);
		ROMFILE.read(buffer, 0x4000);
		for (int i = 0; i < 0x4000; i++) memory[i] = buffer[i] & 0xff;
	}
	else {

        cout << "Warning: Could not load zxspectrum48.rom\n";
        vector<unsigned> randomData = generate_random_data(0x4000);
        for (int i = 0; i < 0x4000; i++) memory[i] = randomData[i] & 0xff;
	}

	prgmIsInitialized = false;
	prgmHasFinished = true;
	previousSample = 0;
	bpInit = 0;
	bpExit = 0;
	bpReload = 0;
	currentTune = nullptr;
}


//Virtual_ZX48::~Virtual_ZX48() {
//
//}


bool Virtual_ZX48::has_stopped() { return prgmHasFinished; }


//void Virtual_ZX48::insertVirtualOSReturn(int startAddress) {
//
//	memory[0xfffb] = 0xc3;			//jp startAddress
//	memory[0xfffc] = startAddress & 0xff;
//	memory[0xfffd] = (startAddress >> 8) & 0xff;
//	memory[0xfffe] = 0xff;
//	memory[0xffff] = 0xff;
//
//	cpu.setSP(0xfffe);
//}

void Virtual_ZX48::init(Work_Tune *tune) {

    currentTune = tune;
}


void Virtual_ZX48::init_prgm() {

//	insertVirtualOSReturn(0x8000);		//TODO TEMP, first 3 bytes of virtualOSReturn get overwritten at some point on/after first run
//	cpu.setPC(0xfffb);
//    bpExit = 0xffff;

    cpu.reset();
    cpu.setPC(bpInit);
//    bpExit = 0x801c;

	previousSample = 0;
	prgmIsInitialized = true;
	prgmHasFinished = false;

//    cout << "initialized, PC=" << hex << cpu.getPC() << endl;
}


void Virtual_ZX48::set_breakpoints(const long &initBP, const long &exitBP, const long &reloadBP) {

    bpInit = initBP;
    bpExit = exitBP;
    bpReload = reloadBP;
}


void Virtual_ZX48::load_binary(char *code, const int &codeSize, const int &startAddress) {

	for (int i = 0; i < codeSize; i++) memory[i + startAddress] = code[i] & 0xff;
	prgmIsInitialized = false;
}


void Virtual_ZX48::load_raw_data(const vector<char> &data, const int &orgAddress) {

    for (int i = 0; static_cast<unsigned>(i) < data.size(); i++) memory[i + orgAddress] = data[i] & 0xff;
}


void Virtual_ZX48::generate_audio_chunk(ostringstream &AUDIOSTREAM, const unsigned &audioChunkSize, const unsigned &playMode) {

	if (!prgmIsInitialized) init_prgm();
//	cout << "PC: " << cpu.getPC() << ", exit: " << bpExit << endl;

	long internalSample = 0;
	int internalSampleCount = 0;
	unsigned externalSampleCount = 0;
	int prevPC = cpu.getPC();

	while ((cpu.getPC() != bpExit) && (externalSampleCount < audioChunkSize)) {

		for (int i = 0; (i < 8) && (cpu.getPC() != bpExit); i++) {

            if (cpu.getPC() == bpReload && cpu.getPC() != prevPC) {

                vector<char> data = currentTune->reload_data(playMode);
                for (int j = 0; static_cast<unsigned>(j) < data.size(); j++)
                    memory[j + currentTune->engineSize + currentTune->orgAddress] = data[j] & 0xff;

                if (playMode > 2 && currentTune->musicdataBinary.symbols.count("loop_point_patch")) {

                    long lp = currentTune->musicdataBinary.symbols.at(currentTune->config.seqLoopLabel);
                    long lpPatchAddr = currentTune->musicdataBinary.symbols.at("loop_point_patch");
                    memory[lpPatchAddr] = lp & 0xff;
                    memory[lpPatchAddr + 1] = (lp>>8) & 0xff;
                }
            }

            prevPC = cpu.getPC();
            cpu.execute_cycle();
        }

		internalSample += ((cpu.outputPortsShort[0xfe] & 0x10) + ((cpu.outputPortsShort[0xfe] & 0x8) >> 3));
		internalSampleCount++;

		if (internalSampleCount == 11) {	//TODO: with 10 samples, we'd get 43750 Hz, which is close enough... so why 11?

			internalSample = static_cast<long>((((internalSample - 0x0f) << 4) / 11) * 8 * 15);

			//simple LP filter
			internalSample = previousSample + static_cast<long>(((internalSample - previousSample) << 3) / 10);
			previousSample = internalSample;

			AUDIOSTREAM << static_cast<char>(internalSample & 0xff) << static_cast<char>((internalSample >> 8) & 0xff)
				<< static_cast<char>(internalSample & 0xff) << static_cast<char>((internalSample >> 8) & 0xff);

			internalSampleCount = 0;
			internalSample = 0;
			externalSampleCount++;
		}
	}

	if (cpu.getPC() == bpExit) {

		prgmHasFinished = true;
		prgmIsInitialized = false;
		for (; externalSampleCount < audioChunkSize; externalSampleCount++) AUDIOSTREAM << 0 << 0 << 0 << 0;
	}
}

////TODO: write new void run_prgm() with fixed start/exit addr, no debug
//int Virtual_ZX48::run_prgm(int startAddress, int exitPoint, const long long &recLength, bool stepDebug) {
//
//	cpu.setPC(startAddress & 0xffff);
//
//	int internalSample = 0;
// 	int internalSampleCount = 0;
//	long long externalSample = 0;
//	int breakpoint = exitPoint;
//
//	ofstream RAWFILE("music.raw.tmp", ios::binary|ios::trunc);
//	if (!RAWFILE.is_open()) {
//
//		cout << "Error writing temporary file." << endl;
//		return -1;
//	}
//
//
//	while ((cpu.getPC() != exitPoint) && (externalSample < recLength)) {
//
//		for (int i = 0; (i < 8) & (cpu.getPC() != exitPoint); i++) {
//
//			if (!stepDebug) {
//
//				cpu.execute_cycle();
//				if ((cpu.getPC() == breakpoint) && (cpu.getPC() != exitPoint)) stepDebug = true;
//// 				if ((cpu.getPC() < 0x8000) || (cpu.getPC() > 0x98ff)) return -1;
//			}
//
//			else {
//				char kInput;
//				cin.get(kInput);
//
//				if (kInput == 'b') {
//
//					string strInput = "";
//
//					cout << "Set breakpoint to: ";
//					cin >> strInput;
//
//					while (strInput.find_first_not_of("0123456789abcdefABCDEF") != string::npos) {
//
//						cout << "Not a valid hex address." << endl << "Set breakpoint to: ";
//						cin >> strInput;
//					}
//
//					stringstream str(strInput);
//					if (!(str >> hex >> breakpoint)) cout << "Hex address out of range." << endl;
//					breakpoint &= 0xffff;
//					cout << "Breakpoint set to " << hex << breakpoint << endl;
//				}
//
//				else if (kInput == 'c') stepDebug = false;
//
//				else if (kInput == 'q') {
//
//					cout << hex << "CPU stopped at 0x" << cpu.getPC() << endl;
//					return 0;
//				}
//
//				else if (kInput == 'd') {
//
//					ofstream MEMDUMP("memdump.bin", ios::out | ios::trunc | ios::binary);
//
//					if (!MEMDUMP.is_open()) {
//
//						cout << "Error writing memdump." << endl;
//						return -1;
//					}
//
//					char dumpmem[0x10000];
//					for (int j = 0; j < 0x10000; j++) dumpmem[j] = static_cast<char>(memory[j]);
//
//					MEMDUMP.write(dumpmem, 0x10000);
//
//					cout << "Memory dumped to memdump.bin." << endl << endl;
//					cin.get(kInput);
//				}
//
//				else cpu.execute_debug();
//			}
//		}
//
//		internalSample += ((cpu.outputPortsShort[0xfe] & 0x10) + ((cpu.outputPortsShort[0xfe] & 0x8) >> 3));
//		internalSampleCount++;
//
//		if (internalSampleCount == 11) {	//with 10 samples, we'd get 43750 Hz, which is close enough... so why 11?
//							//TODO: should be 10 after all, offset is probably due to mem contention.. eh no, output is already too low
//
//			RAWFILE << static_cast<char>(internalSample);
//			internalSampleCount = 0;
//			internalSample = 0;
//			externalSample++;
//		}
//	}
//
//	return 0;
//}
