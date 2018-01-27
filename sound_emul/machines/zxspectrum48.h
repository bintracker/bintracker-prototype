#ifndef ZXSPECTRUM48__H__
#define ZXSPECTRUM48__H__

#include "vm.h"
#include "CPU/z80.h"


class Virtual_ZX48 : public Virtual_Machine {

public:
	std::array<int, 0x10000> memory;

	void init(Work_Tune *tune = nullptr);
	void load_binary(char *code, const int &codeSize, const int &startAddress);
	void load_raw_data(const std::vector<char> &data, const int &orgAddress);
//	int run_prgm(int startAddress, int exitPoint, const long long &recLength, bool stepDebug);
	void generate_audio_chunk(ostringstream &AUDIOSTREAM, const unsigned &audioChunkSize, const unsigned &playMode);
	virtual void set_breakpoints(const long &initBP, const long &exitBP, const long &reloadBP);
	bool has_stopped();

	Virtual_ZX48();
	~Virtual_ZX48();

private:
	z80cpu cpu;
	Work_Tune *currentTune;
	bool prgmIsInitialized;
	bool prgmHasFinished;
	long previousSample;
	int bpInit;
	int bpExit;
	int bpReload;

	void insertVirtualOSReturn(int startAddress);
	void init_prgm();
};


#endif
