#include <iostream>
#include <fstream>
#include <sstream>

#include "gui.h"

void Main_Window::handle_event(ALLEGRO_EVENT &ev) {

	switch (ev.type) {

		case ALLEGRO_EVENT_MENU_CLICK:

			switch (ev.user.data1) {

				case MENU_FILE_OPEN:
					open_file();
					break;

				case MENU_FILE_SAVE:
					(currentTune.savefilePath.empty()) ? save_file_as() : save_file();
					break;

				case MENU_FILE_SAVEAS:
					save_file_as();
					break;

				case MENU_FILE_EXPORT_ASM:
					export_asm(false);
					break;

				case MENU_FILE_EXPORT_ASM_MUSICDATA:
					export_asm(true);
					break;

				case MENU_FILE_EXPORT_BIN:
					export_bin(false);
					break;

				case MENU_FILE_EXPORT_BIN_MUSICDATA:
					export_bin(true);
					break;

				case MENU_FILE_EXPORT_EMULFORMAT:
					export_zxspectrum_tap();
					break;

				case MENU_FILE_EXPORT_WAV:
					export_wav();
					break;

				case MENU_FILE_EXIT:
                    shutdown();
					break;

                case MENU_EDIT_UNDO:
                    undo();
                    break;

                case MENU_EDIT_REDO:
                    redo();
                    break;

                case MENU_EDIT_SELECTALL:
                    status.select_all();
                    (status.focusBlock) ? print_block_data() : print_reference_data();
                    break;

                case MENU_EDIT_COPY:
                    copy_selection();
                    break;

                case MENU_EDIT_CUT:
                    cut_selection();
                    break;

                case MENU_EDIT_DELETE:
                    delete_selection();
                    break;

                case MENU_EDIT_PASTE:
                    paste_selection();
                    break;

                case MENU_EDIT_POROUSPASTE:
                    porous_paste_selection();
                    break;

                case MENU_EDIT_INVERSEPOROUSPASTE:
                    inverse_porous_paste_selection();
                    break;

                case MENU_EDIT_INSERT:
                    insert_selection();
                    break;

                case MENU_EDIT_INTERPOLATE:
                    interpolate_selection();
                    break;

                case MENU_EDIT_LOGINTERPOLATE:
                    interpolate_selection(LOGARITHMIC);
                    break;

                case MENU_EDIT_EXPINTERPOLATE:
                    interpolate_selection(EXPONENTIAL);
                    break;

                case MENU_EDIT_REVERSE:
                    reverse_selection();
                    break;

                case MENU_EDIT_RANDOMIZE:
                    randomize_selection();
                    break;

                case MENU_EDIT_FILL:
                    fill_data();
                    break;

                case MENU_EDIT_TRANSPOSE_SEMI_UP:
                    transpose(1);
                    break;

                case MENU_EDIT_TRANSPOSE_SEMI_DOWN:
                    transpose(-1);
                    break;

                case MENU_EDIT_TRANSPOSE_OCT_UP:
                    transpose(12);
                    break;

                case MENU_EDIT_TRANSPOSE_OCT_DOWN:
                    transpose(-12);
                    break;

                case MENU_EDIT_FILLPOROUS:
                    fill_data(POROUS);
                    break;

				case MENU_PLAY_STOP:
					soundEmul.stop();
					break;

				case MENU_PLAY_PLAYFROMSTART:
					play_from_start();
					break;

                case MENU_PLAY_PLAY:
                    play_from_current_pos();
                    break;

				case MENU_PLAY_PLAYPATTERN:
					play_pattern();
					break;

                case MENU_HELP_ENGINE:
                    display_engine_info();
                    break;

				case MENU_HELP_ABOUT:

					display_about_msg();
					break;

				default: {
						if ((ev.user.data1 >= MENU_FILE_NEW_CONFIGSEL) &&
							(ev.user.data1 < static_cast<int>(MENU_FILE_NEW_CONFIGSEL + settings.configList.size())))
							make_new_file(settings.configList[ev.user.data1 - MENU_FILE_NEW_CONFIGSEL]);
					}
					break;
			}

			break;


        case ALLEGRO_EVENT_NATIVE_DIALOG_CLOSE:
            al_unregister_event_source(eventQueue, al_get_native_text_log_event_source(engineInfo));
            al_close_native_text_log(engineInfo);
            engineInfo = nullptr;
            break;

		case ALLEGRO_EVENT_DISPLAY_CLOSE:
            shutdown();
			break;

        default:
            break;
	}

//	al_flush_event_queue(eventQueue);

	}


void Main_Window::play_from_start() {	//TODO fixed asm address, really???

	currentTune.generate_module_lines();
	bool verbose = false;
	mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

	//TODO: TEMP convert sstream to vector
	vector<string> asmLines;
	istringstream is(mdf.MUSICASM.str());
	string tempstr;
	while (getline(is, tempstr)) asmLines.push_back(tempstr);

	//TODO: looping/no looping
	currentTune.musicdataBinary.assemble(asmLines, currentTune.orgAddress + currentTune.engineSize, true);

	//verify size (TODO: make machine-independent)
	if (currentTune.orgAddress + currentTune.engineSize + currentTune.musicdataBinary.binLength > 0xfff0) {

        al_draw_text(font, settings.rowActColor, settings.messagePanelArea.topLeft.x,
            settings.messagePanelArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "ERROR: Music data too large.");
        return;
	}

	//if a valid loop patch address was registered, patch loop point
	if (currentTune.loopPatchAddr != -1) {
		long lp = currentTune.musicdataBinary.symbols.at(currentTune.config.seqLoopLabel);
		currentTune.engineCode[currentTune.loopPatchAddr] = static_cast<char>(lp);
		currentTune.engineCode[currentTune.loopPatchAddr + 1] = static_cast<char>(lp >> 8);
	}

	soundEmul.vm->load_binary(currentTune.engineCode, currentTune.engineSize, currentTune.orgAddress);
	soundEmul.vm->load_binary(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength,
		currentTune.orgAddress + currentTune.engineSize);
    soundEmul.vm->set_breakpoints(currentTune.engineInitAddress, 0, currentTune.engineReloadAddress);
	soundEmul.start(PM_SONG);
}


void Main_Window::play_from_current_pos() {

//    if (status.currentTab != 1) return;

    bool verbose = false;
    //TODO actually, we should derive the bock from sequence position when !focusBlock
    mdModule mdf(currentTune.generate_lines_from_pos(status.get_current_block(),
        ((status.focusBlock && status.currentTab != 0) ? status.get_current_cursor_row(): 0), status.get_current_reference_row()),
        currentTune.config, verbose);

    vector<string> asmLines;
    istringstream is(mdf.MUSICASM.str());
    string tempstr;
    while (getline(is, tempstr)) asmLines.push_back(tempstr);

    currentTune.musicdataBinary.assemble(asmLines, currentTune.orgAddress + currentTune.engineSize, true);

    if (currentTune.orgAddress + currentTune.engineSize + currentTune.musicdataBinary.binLength > 0xfff0) {

        al_draw_text(font, settings.rowActColor, settings.messagePanelArea.topLeft.x,
            settings.messagePanelArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "ERROR: Music data too large.");
        return;
	}

    soundEmul.vm->load_binary(currentTune.engineCode, currentTune.engineSize, currentTune.orgAddress);
	soundEmul.vm->load_binary(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength,
		currentTune.orgAddress + currentTune.engineSize);
    soundEmul.vm->set_breakpoints(currentTune.engineInitAddress, 0, currentTune.engineReloadAddress);
	soundEmul.start(PM_SONG);
}


void Main_Window::play_pattern() {	//TODO fixed asm address, really???

	currentTune.generate_pattern_lines(status.get_current_block());		//TODO: this will give problems when currentTab != 1;
	bool verbose = false;
	mdModule mdf(currentTune.patternLines, currentTune.config, verbose);

	//TODO: TEMP convert sstream to vector
	vector<string> asmLines;
	istringstream is(mdf.MUSICASM.str());
	string tempstr;
	while (getline(is, tempstr)) asmLines.push_back(tempstr);

//	for (auto&& it: asmLines) cout << it << endl;

	//TODO: looping/no looping
	currentTune.musicdataBinary.assemble(asmLines, currentTune.orgAddress + currentTune.engineSize, true);

    if (currentTune.orgAddress + currentTune.engineSize + currentTune.musicdataBinary.binLength > 0xfff0) {

        al_draw_text(font, settings.rowActColor, settings.messagePanelArea.topLeft.x,
            settings.messagePanelArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "ERROR: Music data too large.");
        return;
	}

	//if a valid loop patch address was registered, patch loop point
	if (currentTune.loopPatchAddr != -1) {
		long lp = currentTune.musicdataBinary.symbols.at(currentTune.config.seqLoopLabel);
		currentTune.engineCode[currentTune.loopPatchAddr] = static_cast<char>(lp);
		currentTune.engineCode[currentTune.loopPatchAddr + 1] = static_cast<char>(lp >> 8);
	}

	soundEmul.vm->load_binary(currentTune.engineCode, currentTune.engineSize, currentTune.orgAddress);
	soundEmul.vm->load_binary(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength,
		currentTune.orgAddress + currentTune.engineSize);
    //TEMP
    soundEmul.vm->set_breakpoints(currentTune.engineInitAddress, 0, currentTune.engineReloadAddress);
	soundEmul.start(PM_PATTERN);
}


void Main_Window::play_row() {
    if (soundEmul.playMode >= PM_PATTERN) return;

	currentTune.generate_single_line(status.get_current_block(), status.get_current_cursor_row());
	bool verbose = false;
	mdModule mdf(currentTune.singleLine, currentTune.config, verbose);

//	cout << mdf << endl;

	//TODO: TEMP convert sstream to vector
	vector<string> asmLines;
	istringstream is(mdf.MUSICASM.str());
	string tempstr;
	while (getline(is, tempstr)) asmLines.push_back(tempstr);

//	for (auto&& it: asmLines) cout << it << endl;

	//TODO: looping/no looping
	currentTune.musicdataBinary.assemble(asmLines, currentTune.orgAddress + currentTune.engineSizeNoLoop, true);

//	for (int i = 0; i < currentTune.musicdataBinary.binLength; i++)
//        cout << hex << (static_cast<int>(currentTune.musicdataBinary.binData[i]) & 0xff);
//    cout << endl;

	soundEmul.vm->load_binary(currentTune.engineCodeNoLoop, currentTune.engineSizeNoLoop, currentTune.orgAddress);
	soundEmul.vm->load_binary(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength,
		currentTune.orgAddress + currentTune.engineSizeNoLoop);
    //TEMP
    soundEmul.vm->set_breakpoints(currentTune.engineInitAddress, currentTune.engineExitAddress, 0);
	soundEmul.start(PM_ROW);
//	if (soundEmul.playMode != 1) cout << "denied\n";
}


