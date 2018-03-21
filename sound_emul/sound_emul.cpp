// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <memory>
#include <samplerate.h>
#include "sound_emul.h"


Sound_Emul::Sound_Emul() {
    audioStream = nullptr;
    audioVoice = nullptr;
    audioMixer = nullptr;
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
        al_detach_audio_stream(audioStream);
        al_detach_voice(audioVoice);
        al_destroy_audio_stream(audioStream);
        al_destroy_mixer(audioMixer);
        al_destroy_voice(audioVoice);
        al_uninstall_audio();
    }
}


void Sound_Emul::init(Work_Tune *tune, const unsigned &_audioChunkSize, unsigned audioSampleRate) {
    currentTune = tune;
    audioChunkSize = _audioChunkSize;

    if (!al_install_audio()) throw(string("Failed to initialize audio system."));
    audioVoice = al_create_voice(audioSampleRate, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if (!audioVoice) throw(string("Failed to create audio voice."));
    if (al_get_voice_frequency(audioVoice) != audioSampleRate) cout << "WARNING: sound driver did not accept sample "
                                                                       "rate chosen by user." << endl;

    audioMixer = al_create_mixer(al_get_voice_frequency(audioVoice),
                                 ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if (!audioMixer) throw(string("Failed to create audio mixer."));
    if (!al_set_default_mixer(audioMixer)) throw(string("Failed to set default mixer."));

    if (!al_attach_mixer_to_voice(audioMixer, audioVoice)) throw(string("Failed to attach mixer to voice."));

    if (!al_reserve_samples(1)) throw (string("Failed to initialize audio system."));
    // TODO(utz): temp solution before fixing emul
    audioStream = al_create_audio_stream(2, audioChunkSize, 45600, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if (audioStream == nullptr) throw(string("Failed to create audio stream."));
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


void Sound_Emul::render_wav(const string &filename, unsigned maxLength, unsigned sampleRate) {
    if (playMode != PM_STOPPED) stop();

    ofstream WAVEFILE(filename.data(), ios::binary|ios::trunc);
    if (!WAVEFILE.is_open()) throw (string("Could not write .wav file"));

    vm->load_binary(currentTune->engineCode, currentTune->engineSize, currentTune->orgAddress);
    vm->load_binary(currentTune->musicdataBinary.binData, currentTune->musicdataBinary.binLength,
                    currentTune->orgAddress + currentTune->engineSize);
    vm->set_breakpoints(currentTune->engineInitAddress, 0, currentTune->engineReloadAddress);

    std::ostringstream rawPCM;
    uint64_t chunkSize = maxLength * static_cast<uint64_t>(vm->get_audio_sample_rate());
    vm->generate_audio_chunk(rawPCM, chunkSize, PM_SONG);

    rawPCM.seekp(0, ios::end);
    uint64_t rawsize = static_cast<uint64_t>(rawPCM.tellp());

    int16_t *shortInput;
    shortInput = new int16_t[rawsize / 2];
    memcpy(shortInput, rawPCM.str().c_str(), rawsize);

    std::vector<float> floatInput(rawsize / 2, 0.0f);
    src_short_to_float_array(shortInput, floatInput.data(), rawsize / 2);

    delete[] shortInput;

    std::vector<float> floatOutput(rawsize, 0.0f);
    std::vector<int16_t> shortOutput(rawsize, 0);

    SRC_DATA resampleData;
    resampleData.data_in = floatInput.data();
    resampleData.data_out = floatOutput.data();
    resampleData.input_frames = rawsize/4;
    resampleData.output_frames = rawsize/2;
    resampleData.src_ratio = static_cast<float>(sampleRate) / vm->get_audio_sample_rate();
    src_simple(&resampleData, SRC_SINC_BEST_QUALITY, 2);
    src_float_to_short_array(floatOutput.data(), shortOutput.data(), resampleData.output_frames_gen * 2);

    uint64_t filesize = (resampleData.output_frames_gen * 4) + 36;
    const char nil = 0;
    const uint64_t byteRate = sampleRate * 4;

    // write PCM WAV file header
    WAVEFILE << "RIFF";
    WAVEFILE << static_cast<char>(filesize & 0xff) << static_cast<char>((filesize >> 8) & 0xff)
             << static_cast<char>((filesize >> 16) & 0xff) << static_cast<char>((filesize >> 24) & 0xff);
    WAVEFILE << "WAVEfmt \x10" << nil << nil << nil << "\x1" << nil << "\x2" << nil
             << static_cast<char>(sampleRate & 0xff) << static_cast<char>((sampleRate >> 8) & 0xff)
             << nil << nil << static_cast<char>(byteRate & 0xff) << static_cast<char>((byteRate >> 8) & 0xff)
             << static_cast<char>((byteRate >> 16) & 0xff) << nil;
    filesize -= 36;
    WAVEFILE << "\x4" << nil << "\x10" << nil << "data" << static_cast<char>(filesize & 0xff)
             << static_cast<char>((filesize >> 8) & 0xff) << static_cast<char>((filesize >> 16) & 0xff)
             << static_cast<char>((filesize >> 24) & 0xff);

    for (int64_t i = 0; i < resampleData.output_frames_gen * 2; ++i) {
        WAVEFILE << static_cast<char>(shortOutput[i] & 0xff) << static_cast<char>((shortOutput[i] >> 8) & 0xff);
    }
}
