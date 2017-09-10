#ifndef BINTRACKER_VM__H__
#define BINTRACKER_VM__H__

#include <sstream>
#include <string>
#include <vector>
#include "../../gui/worktune.h"

using namespace std;

class Virtual_Machine {

public:
    virtual void init(Work_Tune *tune) = 0;
	virtual void load_binary(char *code, const int &codeSize, const int &startAddress) = 0;
	virtual void load_raw_data(const vector<char> &data, const int &orgAddress) = 0;
//	virtual int run_prgm(int startAddress, int exitPoint, const long long &recLength, bool stepDebug) = 0;
	virtual void generate_audio_chunk(ostringstream &ASTREAM, const unsigned &audioChunkSize, const unsigned &playMode) = 0;
	virtual void set_breakpoints(const long &initBP, const long &exitBP, const long &reloadBP) = 0;
	virtual bool has_stopped() = 0;
	virtual ~Virtual_Machine() {}
};

#endif
