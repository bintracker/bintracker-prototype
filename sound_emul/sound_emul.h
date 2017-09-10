#ifndef BINTRACKER_SOUND_EMUL__H__
#define BINTRACKER_SOUND_EMUL__H__

#include <sstream>
#include <allegro5/allegro_audio.h>
#include "machines/vm.h"
#include "machines/zxspectrum48.h"
#include "../gui/worktune.h"

//using namespace std;

class Sound_Emul {

public:
	ALLEGRO_AUDIO_STREAM *audioStream;
	ostringstream audioBuffer;
	unsigned audioChunkSize;
	Virtual_Machine *vm;

	int playMode;

	Sound_Emul();
	Sound_Emul(const Sound_Emul &emul) =delete;
	Sound_Emul& operator=(const Sound_Emul &emul) =delete;
	~Sound_Emul();
//	void init(const string &machine, char *engineCode_, char *engineCodeNoLoop_,
//		const int &engineSize_, const int &engineSizeNoLoop_, const unsigned &audioChunkSize_);
//	void setup_vm(const string &machine, char *engineCode_, char *engineCodeNoLoop_, const int &engineSize_,
//		const int &engineSizeNoLoop_);
    void init(Work_Tune *tune, const unsigned &audioChunkSize);
    void setup_vm();
	void update_audio();
	void start(const int &playMode_);
	void stop();
	void render_wav(const string &filename, char *songData, const int &dataSize);

private:
	char *engineCode;
	char *engineCodeNoLoop;
	int engineSize;
	int engineSizeNoLoop;
	int startAddress;

	Work_Tune *currentTune;

//	void (*emulCallback)();

	Virtual_ZX48 zx48;
};

#endif
