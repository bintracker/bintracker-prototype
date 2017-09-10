#include <iostream>
#include <fstream>

#include "bintracker.h"
#include "gui/gui.h"


using namespace std;


int main(int argc, char **argv){

	try {

		if (!al_init()) throw (string("Allegro initialization failed."));

		string initFile = "";

//		for (int i = 1; i < argc; i++) {
//
//			string arg = argv[i];
//			if (arg == "-i" && i < argc-1) initFile = argv[i+1];
//		}
        if (argc > 1) {
            ifstream INFILE(argv[1]);
            if (INFILE.is_open()) initFile = argv[1];
        }

		Main_Window gui(initFile);
		ALLEGRO_EVENT ev;

		while (gui.isUp) {

			al_wait_for_event(gui.eventQueue, &ev);

			if (ev.type == ALLEGRO_EVENT_TIMER) {

                al_get_keyboard_state(&gui.kbdstate);
                al_get_mouse_state(&gui.mouseState);
                gui.handle_keyboard_input();
                al_stop_timer(gui.timer);
                gui.handle_mouse_input();

				al_flip_display();
				//TODO: attempt to fix display glitches -> drop all but audio updates
                while (!al_is_event_queue_empty(gui.eventQueue)) {
                    al_peek_next_event(gui.eventQueue, &ev);
                    if (ev.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) gui.soundEmul.update_audio();
                    al_drop_next_event(gui.eventQueue);
                }
                al_start_timer(gui.timer);
			}
			else if (ev.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) gui.soundEmul.update_audio();
			else gui.handle_event(ev);
		}
	}
	catch(string &e) {

		cout << "FATAL ERROR: " << e << endl;
		return -1;
	}

	return 0;
}
