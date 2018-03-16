// Copyright 2017-2018 utz. See LICENSE for details.

#ifndef SOUND_EMUL_MACHINES_ZXSPECTRUM48_H_
#define SOUND_EMUL_MACHINES_ZXSPECTRUM48_H_

#include <vector>
#include "vm.h"
#include "CPU/z80.h"


class Virtual_ZX48 : public Virtual_Machine {
 public:
    std::array<int, 0x10000> memory;

    void init(Work_Tune *tune = nullptr) override;
    void load_binary(char *code, const int &codeSize, const int &startAddress) override;
    void load_raw_data(const std::vector<char> &data, const int &orgAddress) override;
    void generate_audio_chunk(std::ostringstream &AUDIOSTREAM, const unsigned &audioChunkSize,
                              const unsigned &playMode) override;
    void set_breakpoints(const int64_t &initBP, const int64_t &exitBP, const int64_t &reloadBP) override;
    bool has_stopped() override;

    Virtual_ZX48();
    ~Virtual_ZX48() override = default;

 private:
    z80cpu cpu;
    Work_Tune *currentTune;
    bool prgmIsInitialized;
    bool prgmHasFinished;
    int64_t previousSample;
    int bpInit;
    int bpExit;
    int bpReload;

    void insertVirtualOSReturn(int startAddress);
    void init_prgm();
};


#endif  // SOUND_EMUL_MACHINES_ZXSPECTRUM48_H_
