// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include <fstream>
// #include <thread>
#include <cstring>
#include <memory>
#include "sound_emul.h"


Sound_Emul::Sound_Emul() {
    audioStream = nullptr;
    engineCode = nullptr;
    engineCodeNoLoop = nullptr;
    currentTune = nullptr;
    vm = nullptr;
    playMode = PM_STOPPED;
    engineSize = 0;
    engineSizeNoLoop = 0;
    startAddress = 0;
    audioChunkSize = 256;
}

Sound_Emul::~Sound_Emul() {
    if (al_is_audio_installed()) {
        stop();
        al_destroy_audio_stream(audioStream);
        al_uninstall_audio();
    }
}


void Sound_Emul::init(Work_Tune *tune, const unsigned &_audioChunkSize, unsigned /*audioSampleRate*/) {
    currentTune = tune;
    audioChunkSize = _audioChunkSize;

    if (!al_install_audio() | !al_reserve_samples(1)) throw (string("Failed to initialize audio system."));
    // TODO(utz): temp solution before fixing emul
    audioStream = al_create_audio_stream(2, audioChunkSize, 45600, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if (audioStream == nullptr) throw (string("Failed to create audio stream."));
    if (!al_attach_audio_stream_to_mixer(audioStream, al_get_default_mixer()))
        throw(string("Failed to attach audio stream to mixer."));

    setup_vm();
    stop();
}


void Sound_Emul::setup_vm() {
    startAddress = 0;

    if (currentTune->config.targetPlatform == "zxspectrum48") {
        vm = &zx48;
        vm->init(currentTune);
        startAddress = 0x8000;
    } else {
        throw(string("Unknown machine configuration, failed to initialize sound emulation."));
    }
}


void Sound_Emul::update_audio() {
    if (al_get_audio_stream_playing(audioStream)) {
        void *aBuffer;
        aBuffer = al_get_audio_stream_fragment(audioStream);

        if (aBuffer != nullptr) {
#if __GNUC__ < 5
            audioBuffer.str("");
#else
            // clear stream, c++11 style
            ostringstream().swap(audioBuffer);
#endif
            vm->generate_audio_chunk(audioBuffer, audioChunkSize, playMode);
            memcpy(aBuffer, audioBuffer.str().c_str(), audioChunkSize * 4);
            al_set_audio_stream_fragment(audioStream, aBuffer);
        }

        if (vm->has_stopped()) stop();
    }
}


void Sound_Emul::start(const int &playMode_) {
    if (playMode_ <= PM_ROW && playMode_ < playMode) return;
    stop();
    al_set_audio_stream_playing(audioStream, true);
    playMode = playMode_;
}


void Sound_Emul::stop() {
    if (al_get_audio_stream_playing(audioStream)) {
        al_drain_audio_stream(audioStream);
        al_set_audio_stream_playing(audioStream, false);
        playMode = PM_STOPPED;
    }
}


void Sound_Emul::render_wav(const string & /* filename */, char * /*songData*/, const int & /*dataSize */) {
//  vm->load_binary(engineCode, engineSize, startAddress);
//  vm->load_binary(songData, dataSize, startAddress + engineSize);
//  vm->run_prgm(0xfffb, 0xffff, 180 * 44100, false);       // true = enable debugger
//
//
//  ifstream RAW_IN("music.raw.tmp", ios::binary|ios::ate);
//  if (!RAW_IN.is_open()) throw (string("Could not read raw data"));
//
//  ofstream WAVEFILE(filename.data(), ios::binary|ios::trunc);
//  if (!WAVEFILE.is_open()) throw (string("Could not write .wav file"));
//
//
//  streampos size = RAW_IN.tellg();
//
//  unique_ptr<char[]> rawdata{new char[size]};
//  RAW_IN.seekg(0, ios::beg);
//  RAW_IN.read(rawdata.get(), size);
//  RAW_IN.close();
//
//  long long filesize = (size * 2) + 36;
//  const char nil = 0;
//
//  WAVEFILE << "RIFF";
//  WAVEFILE << static_cast<char>(filesize & 0xff) << static_cast<char>((filesize >> 8) & 0xff)
//      << static_cast<char>((filesize >> 16) & 0xff) << static_cast<char>((filesize >> 24) & 0xff);
//  WAVEFILE << "WAVEfmt \x10" << nil << nil << nil << "\x1" << nil << "\x1" << nil << "\x44\xac"
//      << nil << nil << "\x88\x58\x1" << nil;
//  filesize -= 36;
//  WAVEFILE << "\x2" << nil << "\x10" << nil << "data" << static_cast<char>(filesize & 0xff)
//      << static_cast<char>((filesize >> 8) & 0xff) << static_cast<char>((filesize >> 16) & 0xff)
//      << static_cast<char>((filesize >> 24) & 0xff);
//
//  long previousSmp = 0;
//
//  for (long long i = 0; i < size; i++) {
//
//      long smp = rawdata[i] & 0xff;
//      smp = long((((smp - 0x0f) << 4) / 11) * 8 * 15);
//
//      // simple LP filter
//      smp = previousSmp + long(((smp - previousSmp) << 3) / 10);
//      previousSmp = smp;
//
//      WAVEFILE << static_cast<char>(smp & 0xff) << static_cast<char>((smp >> 8) & 0xff);
//  }
}
