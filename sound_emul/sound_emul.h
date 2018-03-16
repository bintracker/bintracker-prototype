// Copyright 2017-2018 utz. See LICENSE for details.

#ifndef SOUND_EMUL_SOUND_EMUL_H_
#define SOUND_EMUL_SOUND_EMUL_H_

#include <sstream>
#include <string>
#include <allegro5/allegro_audio.h>
#include "machines/vm.h"
#include "machines/zxspectrum48.h"
#include "../gui/worktune.h"


class Sound_Emul {
 public:
    ALLEGRO_AUDIO_STREAM *audioStream;
    ostringstream audioBuffer;
    unsigned audioChunkSize;
    Virtual_Machine *vm;

    int playMode;

    Sound_Emul();
    Sound_Emul(const Sound_Emul &emul) = delete;
    Sound_Emul& operator=(const Sound_Emul &emul) = delete;
    ~Sound_Emul();

    void init(Work_Tune *tune, const unsigned &_audioChunkSize);
    void setup_vm();
    void update_audio();
    void start(const int &playMode_);
    void stop();
    void render_wav(const std::string &filename, char *songData, const int &dataSize);

 private:
    char *engineCode;
    char *engineCodeNoLoop;
    int engineSize;
    int engineSizeNoLoop;
    int startAddress;

    Work_Tune *currentTune;
    Virtual_ZX48 zx48;
};

#endif  // SOUND_EMUL_SOUND_EMUL_H_
