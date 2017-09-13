#include <sstream>
#include <iostream>
#include "gui.h"


//ALT-INSERT/DELETE : ins/del row
//INSERT/DELETE: ins/del field (shifting consecutive fields)
//BACKSPACE: = SPACE?

void Main_Window::handle_keyboard_input() {

	//prevent accidental key presses by delaying key repeat
	if (status.keyLock) {
		status.keyLock--;
		return;
	}
	if (!status.keyDown) status.keyLock = settings.keyRepeatDelay;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HANDLE GERMAN KEYBOARDS
//	KEY_EQUALS = accent key,
//	KEY_TILDE = ?ß
//	KEY_QUOTE = ä
//	KEY_OPENBRACE = ü
//	KEY_SLASH = '#
//	KEY_BACKSLASH = ^°
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (settings.kbdLang == "DE") {
		if (al_key_down(&kbdstate, ALLEGRO_KEY_LCTRL) || al_key_down(&kbdstate, ALLEGRO_KEY_RCTRL)
			|| al_key_down(&kbdstate, ALLEGRO_KEY_COMMAND)) {

            if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.focusBlock) {

				if (status.editLock) cancel_data_input();
				status.currentTab = (status.currentTab < currentTune.blockTypes.size()) ? (status.currentTab + 1) : 0;
				print_block_tabs();
			}
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();

			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) copy_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    porous_paste_selection();
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) inverse_porous_paste_selection();
                else paste_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) cut_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    interpolate_selection(LOGARITHMIC);
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) interpolate_selection(EXPONENTIAL);
                else interpolate_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) reverse_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    fill_data(POROUS);
                else fill_data();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(12);
                else transpose(1);
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(-12);
                else transpose(-1);
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) undo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) redo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) open_file();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) save_file_as();
                else save_file();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) {
                status.select_all();
                if (status.focusBlock) print_block_data();
                else print_reference_data();
            }

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) {	//TODO: should ALTGR be allowed?

			if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT) || al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_MINUS) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)) remove_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS) || al_key_down(&kbdstate, ALLEGRO_KEY_CLOSEBRACE)) add_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) set_loop_point();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) rename_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) clone_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_0)) {
                status.currentOctave = 0;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_1)) {
                status.currentOctave = 1;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_2)) {
                status.currentOctave = 2;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_3)) {
                status.currentOctave = 3;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_4)) {
                status.currentOctave = 4;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_5)) {
                status.currentOctave = 5;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_6)) {
                status.currentOctave = 6;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_7)) {
                status.currentOctave = (status.currentOctave == 0) ? 0 : status.currentOctave - 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_8)) {
                status.currentOctave = (status.currentOctave == 6) ? 6 : status.currentOctave + 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {
                status.autoInc = (status.autoInc - 1) & 0x3f;
                print_autoinc_setting();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {
                status.autoInc = (status.autoInc + 1) & 0x3f;
                print_autoinc_setting();
            }
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    shrink_block();
                else expand_block();
			}

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT)) insert_field();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_field();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) {

			if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_6)) receive_data_input('&');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_7)) receive_data_input('/');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_CLOSEBRACE)) receive_data_input('*');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_0)) receive_data_input('=');
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) {

                status.start_selection();
                handle_key_down();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) {

                status.start_selection();
                handle_key_up();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) {

                status.start_selection();
                handle_key_left();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) {

                status.start_selection();
                handle_key_right();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {

                status.start_selection();
                handle_key_pgdown();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {

                status.start_selection();
                handle_key_pgup();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) {

                status.start_selection();
                handle_key_home();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) {

                status.start_selection();
                handle_key_end();
			}
			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALTGR)) {

			if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_BACKSLASH2)) receive_data_input('|');
			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		//TODO test code
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_0) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_0)) receive_data_input('0');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_1) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_1)) receive_data_input('1');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_2) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_2)) receive_data_input('2');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_3) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_3)) receive_data_input('3');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_4) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_4)) receive_data_input('4');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_5) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_5)) receive_data_input('5');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_6) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_6)) receive_data_input('6');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_7) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_7)) receive_data_input('7');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_8) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_8)) receive_data_input('8');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_9) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_9)) receive_data_input('9');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) receive_data_input('a');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_B)) receive_data_input('b');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) receive_data_input('c');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) receive_data_input('d');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_E)) receive_data_input('e');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) receive_data_input('f');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_G)) receive_data_input('g');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_H)) receive_data_input('h');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) receive_data_input('i');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_J)) receive_data_input('j');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_K)) receive_data_input('k');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) receive_data_input('l');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_M)) receive_data_input('m');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_N)) receive_data_input('n');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) receive_data_input('o');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) receive_data_input('p');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Q)) receive_data_input('q');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) receive_data_input('r');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) receive_data_input('s');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_T)) receive_data_input('t');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) receive_data_input('u');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) receive_data_input('v');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_W)) receive_data_input('w');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) receive_data_input('x');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) receive_data_input('y');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) receive_data_input('z');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_FULLSTOP)) receive_data_input('.');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_MINUS) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)) receive_data_input('-');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_SLASH)) receive_data_input('/');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_ASTERISK)) receive_data_input('*');
		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_BACKSLASH)) receive_data_input('^');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_CLOSEBRACE) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS)) receive_data_input('+');

		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_SPACE)) {

			if (status.currentTab == 0) receive_data_input(' ');
				else {

				if (status.editLock) cancel_data_input();

				status.get_current_field_pointer()->set("", settings.hexMode);

				if (status.autoInc > 0) {
					status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
			}
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_BACKSPACE)) erase_data_input();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ENTER)) {

			if (status.editLock) {
				complete_data_input();
				print_block_tabs();		//TODO lazy version
			}
			else {
				status.set_current_block_from_cursor();
				print_block_tabs();
				if (status.get_current_blocktype() == 0)
					currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
			}
		}

        else if (al_key_down(&kbdstate, ALLEGRO_KEY_F2)) display_engine_info();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F5)) play_from_start();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F6)) play_from_current_pos();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F7)) play_pattern();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F8)) soundEmul.stop();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.currentTab != 0) {

			if (status.editLock) cancel_data_input();
			status.focusBlock = !status.focusBlock;
            print_block_data();
            print_reference_data();
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) handle_key_up();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) handle_key_down();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) handle_key_left();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) handle_key_right();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) handle_key_pgup();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) handle_key_pgdown();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) handle_key_home();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) handle_key_end();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE)) {

            if (status.editLock) cancel_data_input();
            print_block_data();
            print_reference_data();
		}

		//detect key repeat
		else {
			status.keyLock = 0;
			status.keyDown = false;
			return;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HANDLE FRENCH KEYBOARDS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	else if (settings.kbdLang == "FR") {
		if (al_key_down(&kbdstate, ALLEGRO_KEY_LCTRL) || al_key_down(&kbdstate, ALLEGRO_KEY_RCTRL)
			|| al_key_down(&kbdstate, ALLEGRO_KEY_COMMAND)) {

			if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.focusBlock) {

				if (status.editLock) cancel_data_input();
				status.currentTab = (status.currentTab < currentTune.blockTypes.size()) ? (status.currentTab + 1) : 0;
				print_block_tabs();
			}
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();

			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) copy_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    porous_paste_selection();
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) inverse_porous_paste_selection();
                else paste_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) cut_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    interpolate_selection(LOGARITHMIC);
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) interpolate_selection(EXPONENTIAL);
                else interpolate_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) reverse_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    fill_data(POROUS);
                else fill_data();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(12);
                else transpose(1);
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(-12);
                else transpose(-1);
			}
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) undo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) redo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) open_file();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) save_file_as();
                else save_file();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) {
                status.select_all();
                if (status.focusBlock) print_block_data();
                else print_reference_data();
            }

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) {	//TODO: should ALTGR be allowed?

			if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT) || al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_row();
			//the following two should check ALLEGRO_KEY_*SHIFT, but let's kindly ignore that for now
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)
                || (al_key_down(&kbdstate, ALLEGRO_KEY_6)
                && (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)))) remove_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS) || al_key_down(&kbdstate, ALLEGRO_KEY_EQUALS)) add_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) set_loop_point();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) rename_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) clone_block();
						else if (al_key_down(&kbdstate, ALLEGRO_KEY_0)) {
                status.currentOctave = 0;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_1)) {
                status.currentOctave = 1;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_2)) {
                status.currentOctave = 2;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_3)) {
                status.currentOctave = 3;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_4)) {
                status.currentOctave = 4;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_5)) {
                status.currentOctave = 5;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_6)) {
                status.currentOctave = 6;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_7)) {
                status.currentOctave = (status.currentOctave == 0) ? 0 : status.currentOctave - 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_8)) {
                status.currentOctave = (status.currentOctave == 6) ? 6 : status.currentOctave + 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {
                status.autoInc = (status.autoInc - 1) & 0xff;
                print_autoinc_setting();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {
                status.autoInc = (status.autoInc + 1) & 0xff;
                print_autoinc_setting();
            }
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    shrink_block();
                else expand_block();
			}

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT)) insert_field();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_field();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) {

			if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_1)) receive_data_input('&');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_FULLSTOP)) receive_data_input('/');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_EQUALS)) receive_data_input('+');
			else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_6)) receive_data_input('-');
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) {

                status.start_selection();
                handle_key_down();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) {

                status.start_selection();
                handle_key_up();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) {

                status.start_selection();
                handle_key_left();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) {

                status.start_selection();
                handle_key_right();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {

                status.start_selection();
                handle_key_pgdown();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {

                status.start_selection();
                handle_key_pgup();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) {

                status.start_selection();
                handle_key_home();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) {

                status.start_selection();
                handle_key_end();
			}
			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALTGR)) {

			if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_6)) receive_data_input('|');
			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		//TODO test code
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_0) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_0)) receive_data_input('0');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_1) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_1)) receive_data_input('1');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_2) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_2)) receive_data_input('2');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_3) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_3)) receive_data_input('3');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_4) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_4)) receive_data_input('4');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_5) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_5)) receive_data_input('5');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_6) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_6)) receive_data_input('6');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_7) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_7)) receive_data_input('7');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_8) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_8)) receive_data_input('8');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_9) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_9)) receive_data_input('9');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) receive_data_input('a');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_B)) receive_data_input('b');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) receive_data_input('c');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) receive_data_input('d');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_E)) receive_data_input('e');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) receive_data_input('f');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_G)) receive_data_input('g');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_H)) receive_data_input('h');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) receive_data_input('i');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_J)) receive_data_input('j');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_K)) receive_data_input('k');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) receive_data_input('l');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_M)) receive_data_input('m');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_N)) receive_data_input('n');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) receive_data_input('o');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) receive_data_input('p');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Q)) receive_data_input('q');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) receive_data_input('r');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) receive_data_input('s');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_T)) receive_data_input('t');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) receive_data_input('u');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) receive_data_input('v');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_W)) receive_data_input('w');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) receive_data_input('x');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) receive_data_input('y');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) receive_data_input('z');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_COMMA)) receive_data_input(',');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_FULLSTOP)) receive_data_input('.');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)) receive_data_input('-');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_SLASH)) receive_data_input('/');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_ASTERISK)) receive_data_input('*');
		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_MINUS)) receive_data_input('^');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS)) receive_data_input('+');
		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_BACKSLASH)) receive_data_input('*');
		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_EQUALS)) receive_data_input('=');

		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_SPACE)) {

			if (status.currentTab == 0) receive_data_input(' ');
				else {

				if (status.editLock) cancel_data_input();

				status.get_current_field_pointer()->set("", settings.hexMode);

				if (status.autoInc > 0) {
					status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
			}
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_BACKSPACE)) erase_data_input();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ENTER)) {

			if (status.editLock) {
				complete_data_input();
				print_block_tabs();		//TODO lazy version
			}
			else {
				status.set_current_block_from_cursor();
				print_block_tabs();
				if (status.get_current_blocktype() == 0)
					currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
			}
		}

        else if (al_key_down(&kbdstate, ALLEGRO_KEY_F2)) display_engine_info();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F5)) play_from_start();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F6)) play_from_current_pos();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F7)) play_pattern();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F8)) soundEmul.stop();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.currentTab != 0) {

			if (status.editLock) cancel_data_input();
			status.focusBlock = !status.focusBlock;
            print_block_data();
            print_reference_data();
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) handle_key_up();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) handle_key_down();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) handle_key_left();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) handle_key_right();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) handle_key_pgup();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) handle_key_pgdown();
        else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) handle_key_home();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) handle_key_end();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE)) {

            if (status.editLock) cancel_data_input();
            print_block_data();
            print_reference_data();
		}

		//detect key repeat
		else {
			status.keyLock = 0;
			status.keyDown = false;
			return;
		}
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HANDLE EN/US KEYBOARDS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	else {
		if (al_key_down(&kbdstate, ALLEGRO_KEY_LCTRL) || al_key_down(&kbdstate, ALLEGRO_KEY_RCTRL)
			|| al_key_down(&kbdstate, ALLEGRO_KEY_COMMAND)) {

			if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.focusBlock) {

				if (status.editLock) cancel_data_input();
				status.currentTab = (status.currentTab < currentTune.blockTypes.size()) ? (status.currentTab + 1) : 0;
				print_block_tabs();
			}
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
//			else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();

			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) copy_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    porous_paste_selection();
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) inverse_porous_paste_selection();
                else paste_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) cut_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    interpolate_selection(LOGARITHMIC);
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) interpolate_selection(EXPONENTIAL);
                else interpolate_selection();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) reverse_selection();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    fill_data(POROUS);
                else fill_data();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(12);
                else transpose(1);
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) {
                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    transpose(-12);
                else transpose(-1);
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) undo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) redo();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) open_file();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) save_file_as();
                else save_file();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) {
                status.select_all();
                if (status.focusBlock) print_block_data();
                else print_reference_data();
            }

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ALT)) {	//TODO: should ALTGR be allowed?

			if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) select_prev_block();
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) select_next_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT) || al_key_down(&kbdstate, ALLEGRO_KEY_V)) insert_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_MINUS) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)) remove_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS) || (al_key_down(&kbdstate, ALLEGRO_KEY_MINUS)
					&& (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)))) add_row();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) set_loop_point();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) rename_block();
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) clone_block();
						else if (al_key_down(&kbdstate, ALLEGRO_KEY_0)) {
                status.currentOctave = 0;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_1)) {
                status.currentOctave = 1;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_2)) {
                status.currentOctave = 2;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_3)) {
                status.currentOctave = 3;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_4)) {
                status.currentOctave = 4;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_5)) {
                status.currentOctave = 5;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_6)) {
                status.currentOctave = 6;
                print_base_octave();
			}
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_7)) {
                status.currentOctave = (status.currentOctave == 0) ? 0 : status.currentOctave - 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_8)) {
                status.currentOctave = (status.currentOctave == 6) ? 6 : status.currentOctave + 1;
                print_base_octave();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {
                status.autoInc = (status.autoInc - 1) & 0xff;
                print_autoinc_setting();
            }
            else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {
                status.autoInc = (status.autoInc + 1) & 0xff;
                print_autoinc_setting();
            }
			else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) {

                if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT))
                    shrink_block();
                else expand_block();
			}

			else {
				status.keyLock = 0;
				status.keyDown = false;
				return;
			}
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_INSERT)) insert_field();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DELETE)) delete_field();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT) || al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)) {

				if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_BACKSLASH)) receive_data_input('|');
				else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_6)) receive_data_input('^');
				else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_7)) receive_data_input('&');
				else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_8)) receive_data_input('*');
				else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_EQUALS)) receive_data_input('+');
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) {

                    status.start_selection();
                    handle_key_down();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) {

                    status.start_selection();
                    handle_key_up();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) {

                    status.start_selection();
                    handle_key_left();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) {

                    status.start_selection();
                    handle_key_right();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) {

                    status.start_selection();
                    handle_key_pgdown();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) {

                    status.start_selection();
                    handle_key_pgup();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) {

                    status.start_selection();
                    handle_key_home();
                }
                else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) {

                    status.start_selection();
                    handle_key_end();
                }
				else {
					status.keyLock = 0;
					status.keyDown = false;
					return;
				}
		}

		//TODO test code
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_0) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_0)) receive_data_input('0');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_1) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_1)) receive_data_input('1');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_2) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_2)) receive_data_input('2');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_3) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_3)) receive_data_input('3');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_4) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_4)) receive_data_input('4');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_5) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_5)) receive_data_input('5');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_6) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_6)) receive_data_input('6');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_7) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_7)) receive_data_input('7');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_8) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_8)) receive_data_input('8');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_9) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_9)) receive_data_input('9');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_A)) receive_data_input('a');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_B)) receive_data_input('b');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_C)) receive_data_input('c');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_D)) receive_data_input('d');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_E)) receive_data_input('e');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F)) receive_data_input('f');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_G)) receive_data_input('g');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_H)) receive_data_input('h');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_I)) receive_data_input('i');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_J)) receive_data_input('j');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_K)) receive_data_input('k');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_L)) receive_data_input('l');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_M)) receive_data_input('m');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_N)) receive_data_input('n');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_O)) receive_data_input('o');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_P)) receive_data_input('p');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Q)) receive_data_input('q');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_R)) receive_data_input('r');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_S)) receive_data_input('s');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_T)) receive_data_input('t');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_U)) receive_data_input('u');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_V)) receive_data_input('v');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_W)) receive_data_input('w');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_X)) receive_data_input('x');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Y)) receive_data_input('y');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_Z)) receive_data_input('z');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_FULLSTOP)) receive_data_input('.');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_MINUS) || al_key_down(&kbdstate, ALLEGRO_KEY_PAD_MINUS)) receive_data_input('-');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_SLASH) || al_key_down(&kbdstate, ALLEGRO_KEY_SLASH)) receive_data_input('/');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_ASTERISK)) receive_data_input('*');
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PAD_PLUS)) receive_data_input('+');

		else if (status.focusBlock && al_key_down(&kbdstate, ALLEGRO_KEY_SPACE)) {

			if (status.currentTab == 0) receive_data_input(' ');
				else {

				if (status.editLock) cancel_data_input();

				status.get_current_field_pointer()->set("", settings.hexMode);

				if (status.autoInc > 0) {
					status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
			}
		}
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_BACKSPACE)) erase_data_input();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ENTER)) {

			if (status.editLock) {
				complete_data_input();
				print_block_tabs();		//TODO lazy version
			}
			else {
				status.set_current_block_from_cursor();
				print_block_tabs();
				if (status.get_current_blocktype() == 0)
					currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
			}
		}

        else if (al_key_down(&kbdstate, ALLEGRO_KEY_F2)) display_engine_info();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F5)) play_from_start();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F6)) play_from_current_pos();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F7)) play_pattern();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_F8)) soundEmul.stop();

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_TAB) && status.currentTab != 0) {

			if (status.editLock) cancel_data_input();
			status.focusBlock = !status.focusBlock;
            print_block_data();
            print_reference_data();
		}

		else if (al_key_down(&kbdstate, ALLEGRO_KEY_UP)) handle_key_up();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_DOWN)) handle_key_down();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_LEFT)) handle_key_left();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_RIGHT)) handle_key_right();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGUP)) handle_key_pgup();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_PGDN)) handle_key_pgdown();
        else if (al_key_down(&kbdstate, ALLEGRO_KEY_HOME)) handle_key_home();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_END)) handle_key_end();
		else if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE)) {

            if (status.editLock) cancel_data_input();
            print_block_data();
            print_reference_data();
		}

		//detect key repeat
		else {
			status.keyLock = 0;
			status.keyDown = false;
			return;
		}
	}

    //TODO temp solution for cancelling selection
    if (status.selectionType != INACTIVE && !al_key_down(&kbdstate, ALLEGRO_KEY_LSHIFT)
        && !al_key_down(&kbdstate, ALLEGRO_KEY_RSHIFT)
        && !((al_key_down(&kbdstate, ALLEGRO_KEY_LCTRL) || al_key_down(&kbdstate, ALLEGRO_KEY_RCTRL))
        && (al_key_down(&kbdstate, ALLEGRO_KEY_C) || al_key_down(&kbdstate, ALLEGRO_KEY_U)
            || al_key_down(&kbdstate, ALLEGRO_KEY_A) || al_key_down(&kbdstate, ALLEGRO_KEY_D)))) {

            status.cancel_selection();
            (status.focusBlock) ? print_block_data() : print_reference_data();
    }

	status.keyDown = true;
}


void Main_Window::handle_key_release() {
	status.keyLock = 0;
	status.keyDown = false;
}
