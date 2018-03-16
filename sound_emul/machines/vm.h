// Copyright 2017-2018 utz. See LICENSE for details.

#ifndef SOUND_EMUL_MACHINES_VM_H_
#define SOUND_EMUL_MACHINES_VM_H_

#include <sstream>
#include <string>
#include <vector>
#include "../../gui/worktune.h"

class Virtual_Machine {
 public:
    virtual void init(Work_Tune *tune) = 0;
    virtual void load_binary(char *code, const int &codeSize, const int &startAddress) = 0;
    virtual void load_raw_data(const std::vector<char> &data, const int &orgAddress) = 0;
    virtual void generate_audio_chunk(std::ostringstream &ASTREAM, const unsigned &audioChunkSize,
                                      const unsigned &playMode) = 0;
    virtual void set_breakpoints(const int64_t &initBP, const int64_t &exitBP, const int64_t &reloadBP) = 0;
    virtual bool has_stopped() = 0;
    virtual ~Virtual_Machine() = default;
};

#endif  // SOUND_EMUL_MACHINES_VM_H_
