// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include <fstream>
#include <string>

#include "bintracker.h"
#include "gui/gui.h"


int main(int argc, char **argv) {
    chdir(argv[0]);

    try {
        if (!al_init()) throw(string("Allegro initialization failed."));

        string initFile;

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

                // TODO(utz): attempt to fix display glitches -> drop all but audio updates
                while (!al_is_event_queue_empty(gui.eventQueue)) {
                    al_peek_next_event(gui.eventQueue, &ev);
                    if (ev.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) gui.soundEmul.update_audio();
                    al_drop_next_event(gui.eventQueue);
                }
                al_start_timer(gui.timer);

                al_flip_display();
            } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                al_get_keyboard_state(&gui.kbdstate);
                gui.handle_keyboard_input();
            } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                gui.handle_key_release();
            } else if (ev.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) {
                gui.soundEmul.update_audio();
            } else {
                gui.handle_event(ev);
            }
        }
    } catch(string &e) {
        cout << "FATAL ERROR: " << e << endl;
        return -1;
    }

    return 0;
}
