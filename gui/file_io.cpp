// Copyright 2017-2018 utz. See LICENSE for details.

#include <iostream>
#include <fstream>
#include <cstdio>

#include "gui.h"

// TODO(utz): Exceptions

void Main_Window::make_new_file(const string &config) {
    al_stop_timer(timer);
    soundEmul.stop();

    if (currentTune.hasUnsavedChanges) {
        al_uninstall_keyboard();
        al_install_keyboard();

        int doWhat = al_show_native_message_box(nullptr, "Save unsaved changes?", "",
                                    "The current tune has unsaved changes. Save changes before creating a new tune?",
                                    "Cancel|Discard Changes|Save Changes", ALLEGRO_MESSAGEBOX_WARN);

        if (doWhat == 3) {
            save_file();

            if (currentTune.hasUnsavedChanges) {
                al_resume_timer(timer);
                return;
            }
        } else if (doWhat < 2) {
            al_resume_timer(timer);
            return;
        }
    }

    settings.defaultConfig = config;
    currentTune.init("", settings, &status);

    soundEmul.setup_vm();

    init_track_view();
    set_display_title();
    al_flip_display();
    al_resume_timer(timer);
    al_flush_event_queue(eventQueue);
}


void Main_Window::open_file() {
    soundEmul.stop();
    al_uninstall_keyboard();
    al_install_keyboard();

    if (currentTune.hasUnsavedChanges) {
        int doWhat = al_show_native_message_box(nullptr, "Save unsaved changes?", "",
                                    "The current tune has unsaved changes. Save changes before opening a new tune?",
                                    "Cancel|Discard Changes|Save Changes", ALLEGRO_MESSAGEBOX_WARN);

        if (doWhat == 3) {
            save_file();
            if (currentTune.hasUnsavedChanges) return;
        } else if (doWhat < 2) {
            return;
        }
    }

    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, "Load File...", "*.mdal", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {
        const char *fp = al_get_native_file_dialog_path(fdialog, 0);
        string infilePath = fp;
        al_stop_timer(timer);
        soundEmul.stop();

        currentTune.init(infilePath, settings, &status);
        soundEmul.setup_vm();

        init_track_view();
        set_display_title();
        al_flip_display();
        al_resume_timer(timer);
    }

    al_destroy_native_file_dialog(fdialog);
    al_flush_event_queue(eventQueue);
}


void Main_Window::save_file() {
    if (!currentTune.hasUnsavedChanges && !currentTune.savefilePath.empty()) return;

    if (currentTune.savefilePath.empty()) {
        save_file_as();
        return;
    }

    currentTune.generate_module_lines();

    ofstream SAVEFILE(currentTune.savefilePath.data());
    if (!SAVEFILE.is_open()) throw ("Could not write " + currentTune.savefilePath + ".");

    for (auto&& it : currentTune.moduleLines) SAVEFILE << it << endl;

    currentTune.infilePath = currentTune.savefilePath;
    currentTune.hasUnsavedChanges = false;
    set_display_title();
}


void Main_Window::save_file_as() {
    // soundEmul.stop();
    // al_pause_event_queue(eventQueue, true);
    al_uninstall_keyboard();
    al_install_keyboard();

    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, "Save File...", "*.mdal", ALLEGRO_FILECHOOSER_SAVE);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {
        const char *fp = al_get_native_file_dialog_path(fdialog, 0);

        string path = fp;
        if (!path.empty() && ((path.size() <= 5) || (path.compare(path.size() - 5, 5, ".mdal")))) path += ".mdal";

        bool doSave = true;
        ifstream SAVEFILE(path.data());
        if (SAVEFILE.is_open()) {
            SAVEFILE.close();
            doSave = display_confirm_overwrite_msg();
        }

        if (doSave) {
            currentTune.savefilePath = path;
            save_file();
        }
    }

    al_destroy_native_file_dialog(fdialog);
    // al_pause_event_queue(eventQueue, false);
}


void Main_Window::export_asm(const bool &musicDataOnly) {
    string dialogHeader = (musicDataOnly) ? "Export .asm (Music Data only)..." : "Export .asm...";

    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, dialogHeader.data(), "*.asm", ALLEGRO_FILECHOOSER_SAVE);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {
        const char *fp = al_get_native_file_dialog_path(fdialog, 0);

        string path = fp;
        if (!path.empty() && ((path.size() <= 4) || (path.compare(path.size() - 4, 4, ".asm")))) path += ".asm";

        bool doSave = true;

        ifstream PROBE(path.data());
        if (PROBE.is_open()) {
            PROBE.close();
            doSave = display_confirm_overwrite_msg();
        }

        if (doSave) {
            ofstream SAVEFILE(path.data());
            if (!SAVEFILE.is_open()) throw ("Could not write " + currentTune.savefilePath + ".");

            currentTune.generate_module_lines();
            bool verbose = false;
            mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

            if (!musicDataOnly) {
                string engineSourceFile = "engines/" + currentTune.configName + "/main.asm";
                ifstream ENGINESOURCE(engineSourceFile.data());
                if (!ENGINESOURCE.is_open()) throw ("Could not read " + engineSourceFile + ".");
                SAVEFILE << ENGINESOURCE.rdbuf();
            }

            SAVEFILE << mdf;
        }
    }

    al_destroy_native_file_dialog(fdialog);
}


void Main_Window::export_bin(const bool &musicDataOnly) {
    string dialogHeader = (musicDataOnly) ? "Export .bin (Music Data only)..." : "Export .bin...";

    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, dialogHeader.data(), "*.bin", ALLEGRO_FILECHOOSER_SAVE);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {
        const char *fp = al_get_native_file_dialog_path(fdialog, 0);

        string path = fp;
        if (!path.empty() && ((path.size() <= 4) || (path.compare(path.size() - 4, 4, ".bin")))) path += ".bin";

        bool doSave = true;

        ifstream PROBE(path.data());
        if (PROBE.is_open()) {
            PROBE.close();
            doSave = display_confirm_overwrite_msg();
        }

        if (doSave) {
            remove(path.data());

            ofstream SAVEFILE(path.data(), ios::binary|ios::app);
            if (!SAVEFILE.is_open()) throw ("Could not write " + path + ".");

            currentTune.generate_module_lines();
            bool verbose = false;
            mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

            // TODO(utz): TEMP convert sstream to vector
            vector<string> asmLines;
            istringstream is(mdf.MUSICASM.str());
            string tempstr;
            while (getline(is, tempstr)) asmLines.push_back(tempstr);

            // TODO(utz): looping/no looping
            currentTune.musicdataBinary.assemble(asmLines, 0x8000 + currentTune.engineSize, true);

            if (!musicDataOnly) {
                // patch loop point
                int64_t lp = currentTune.musicdataBinary.symbols[currentTune.config.seqLoopLabel];
                currentTune.engineCode[currentTune.loopPatchAddr] = static_cast<char>(lp);
                currentTune.engineCode[currentTune.loopPatchAddr + 1] = static_cast<char>(lp >> 8);

                SAVEFILE.write(currentTune.engineCode, currentTune.engineSize);
            }

            SAVEFILE.write(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength);
        }
    }

    al_destroy_native_file_dialog(fdialog);
}


void Main_Window::export_wav() {
    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, "Export .wav...", "*.wav", ALLEGRO_FILECHOOSER_SAVE);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {

        const char *fp = al_get_native_file_dialog_path(fdialog, 0);

        string path = fp;
        if (path != "" && ((path.size() <= 4) || (path.compare(path.size() - 4, 4, ".wav")))) path += ".wav";

        currentTune.generate_module_lines();
        bool verbose = false;
        mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

        // TODO(utz): TEMP convert sstream to vector
        vector<string> asmLines;
        istringstream is(mdf.MUSICASM.str());
        string tempstr;
        while (getline(is, tempstr)) asmLines.push_back(tempstr);

        // TODO(utz): looping/no looping
        currentTune.musicdataBinary.assemble(asmLines, 0x8000 + currentTune.engineSize, true);

        // verify size (TODO(utz): make machine-independent)
        if (currentTune.orgAddress + currentTune.engineSize + currentTune.musicdataBinary.binLength > 0xfff0) {
            al_draw_text(font, settings.rowActColor, settings.messagePanelArea.topLeft.x,
                         settings.messagePanelArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "ERROR: Music data too large.");
            return;
        }

        // if a valid loop patch address was registered, patch loop point
        if (currentTune.loopPatchAddr != -1) {
            int64_t lp = currentTune.musicdataBinary.symbols.at(currentTune.config.seqLoopLabel);
            currentTune.engineCode[currentTune.loopPatchAddr] = static_cast<char>(lp);
            currentTune.engineCode[currentTune.loopPatchAddr + 1] = static_cast<char>(lp >> 8);
        }

        soundEmul.render_wav(path, settings.recordingLength, settings.audioSampleRate);
    }

    al_destroy_native_file_dialog(fdialog);
}


void Main_Window::export_zxspectrum_tap() {
    ALLEGRO_FILECHOOSER *fdialog;
    fdialog = al_create_native_file_dialog(nullptr, "Export .tap...", "*.tap", ALLEGRO_FILECHOOSER_SAVE);

    al_show_native_file_dialog(nullptr, fdialog);
    int fc = al_get_native_file_dialog_count(fdialog);

    if (fc) {
        const char *fp = al_get_native_file_dialog_path(fdialog, 0);

        string path = fp;
        if (!path.empty() && ((path.size() <= 4) || (path.compare(path.size() - 4, 4, ".tap")))) path += ".tap";

        bool doSave = true;

        ifstream PROBE(path.data());
        if (PROBE.is_open()) {
            PROBE.close();
            doSave = display_confirm_overwrite_msg();
        }

        if (doSave) {
            remove(path.data());

            ofstream SAVEFILE(path.data(), ios::binary|ios::app);
            if (!SAVEFILE.is_open()) throw ("Could not write " + path + ".");

            currentTune.generate_module_lines();
            bool verbose = false;
            mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

            // TODO(utz): TEMP convert sstream to vector
            vector<string> asmLines;
            istringstream is(mdf.MUSICASM.str());
            string tempstr;
            while (getline(is, tempstr)) asmLines.push_back(tempstr);

            // TODO(utz): looping/no looping
            currentTune.musicdataBinary.assemble(asmLines, 0x8000 + currentTune.engineSize, true);

            // patch loop point
            int64_t lp = currentTune.musicdataBinary.symbols[currentTune.config.seqLoopLabel];
            currentTune.engineCode[currentTune.loopPatchAddr] = static_cast<char>(lp);
            currentTune.engineCode[currentTune.loopPatchAddr + 1] = static_cast<char>(lp >> 8);

            // generate BASIC loader
            vector<char> basicLoader;
            // header block
            basicLoader.push_back(0x13);            // size_lo
            basicLoader.push_back(0);               // size_hi
            basicLoader.push_back(0);               // flag byte: header
            basicLoader.push_back(0);               // block type: BASIC prgm
            basicLoader.push_back('b');             // file name
            basicLoader.push_back('i');
            basicLoader.push_back('n');
            basicLoader.push_back('t');
            basicLoader.push_back('r');
            basicLoader.push_back('a');
            basicLoader.push_back('c');
            basicLoader.push_back('k');
            basicLoader.push_back('e');
            basicLoader.push_back('r');
            basicLoader.push_back(0);               // datablock size_lo
            basicLoader.push_back(0);               // datablock size_hi
            basicLoader.push_back(0xa);             // param1_lo (BASIC start line)
            basicLoader.push_back(0);               // param1_hi
            basicLoader.push_back(0);               // param2_lo (VARS relative to BASIC start)
            basicLoader.push_back(0);               // param2_hi
            basicLoader.push_back(0);               // header checksum placeholder

            basicLoader.push_back(0);               // block_size2_lo
            basicLoader.push_back(0);               // block_size2_hi
            basicLoader.push_back(-1);              // flag byte
            // basic code
            unsigned basicStart = basicLoader.size();
            basicLoader.push_back(0);               // line no. MSB
            basicLoader.push_back(0xa);             // line no. LSB
            basicLoader.push_back(0x18);            // line len LSB
            basicLoader.push_back(0);               // line len MSB
            basicLoader.push_back(-25);             // BORDER
            basicLoader.push_back(-61);             // NOT
            basicLoader.push_back(-89);             // PI
            basicLoader.push_back(':');             // :
            basicLoader.push_back(-38);             // PAPER
            basicLoader.push_back(-61);             // NOT
            basicLoader.push_back(-89);             // PI
            basicLoader.push_back(':');             // :
            basicLoader.push_back(-39);             // INK
            basicLoader.push_back(-80);             // VAL
            basicLoader.push_back('"');             // "
            basicLoader.push_back('7');             // 7
            basicLoader.push_back('"');             // "
            basicLoader.push_back(':');             // :
            basicLoader.push_back(-3);              // CLEAR
            basicLoader.push_back(-80);             // VAL
            basicLoader.push_back('"');             // "
            string address = numToStr(currentTune.engineInitAddress - 1, 0, false);
            for (char addressByte : address) basicLoader.push_back(addressByte);
            basicLoader.push_back('"');             // "
            basicLoader.push_back(0xd);             // newline
            basicLoader.push_back(0);               // line no. MSB
            basicLoader.push_back(0x14);            // line no. LSB
            basicLoader.push_back(0x5);             // line len LSB
            basicLoader.push_back(0);               // line len MSB
            basicLoader.push_back(-17);             // LOAD
            basicLoader.push_back('"');             // "
            basicLoader.push_back('"');             // "
            basicLoader.push_back(-81);             // CODE
            basicLoader.push_back(0xd);             // newline
            basicLoader.push_back(0);               // line no. MSB
            basicLoader.push_back(0x1e);            // line no. LSB

            unsigned titleLinePos = basicLoader.size();
            basicLoader.push_back(0x5);             // line len LSB
            basicLoader.push_back(0);               // line len MSB
            basicLoader.push_back(-5);              // CLS
            basicLoader.push_back(':');             // :
            basicLoader.push_back(-11);             // PRINT
            if (currentTune.globalConstants[1].command->mdCmdName == "TITLE") {
                basicLoader.push_back(':');         // :
                basicLoader.push_back(-11);         // PRINT
                basicLoader.push_back('"');         // "
                basicLoader.push_back(' ');         // _

                for (char pos : currentTune.globalConstants[1].dataString) basicLoader.push_back(pos);

                basicLoader.push_back('"');         // "
            }
            if (currentTune.globalConstants[0].command->mdCmdName == "AUTHOR") {
                basicLoader.push_back(':');         // :
                basicLoader.push_back(-11);         // PRINT
                basicLoader.push_back('"');         // "
                basicLoader.push_back(' ');         // _
                basicLoader.push_back('b');         // b
                basicLoader.push_back('y');         // y
                basicLoader.push_back(' ');         // _

                for (char pos : currentTune.globalConstants[0].dataString) basicLoader.push_back(pos);

                basicLoader.push_back('"');         // "
            }
            basicLoader.push_back(0xd);             // newline
            unsigned llen = basicLoader.size() - titleLinePos - 2;
            basicLoader[titleLinePos] = llen & 0xff;
            basicLoader[titleLinePos + 1] = (llen >> 8) & 0xff;

            basicLoader.push_back(0);               // line no. MSB
            basicLoader.push_back(0x28);            // line no. LSB
            basicLoader.push_back(0xb);             // line len LSB
            basicLoader.push_back(0);               // line len MSB
            basicLoader.push_back(-7);              // RANDOMIZE
            basicLoader.push_back(-64);             // USR
            basicLoader.push_back(-80);             // VAL
            basicLoader.push_back('"');             // "
            address = numToStr(currentTune.engineInitAddress, 0, false);
            for (char addressByte : address) basicLoader.push_back(addressByte);
            basicLoader.push_back('"');             // "
            basicLoader.push_back(0xd);             // newline

            // fix datablock_size
            unsigned basicDataSize = basicLoader.size() - basicStart;
            basicLoader[14] = basicDataSize & 0xff;
            basicLoader[15] = (basicDataSize >> 8) & 0xff;

            // fix param2 (VARS start)
            basicLoader[18] = basicDataSize & 0xff;
            basicLoader[19] = (basicDataSize >> 8) & 0xff;

            // fix header checksum
            char checksum = 0;
            for (unsigned pos = 2; pos < 20; pos++) checksum ^= basicLoader[pos];
            basicLoader[20] = checksum;

            // fix block2_size
            unsigned blk2size = basicDataSize + 2;
            basicLoader[21] = blk2size & 0xff;
            basicLoader[22] = (blk2size >> 8) & 0xff;


            // fix basic code block checksum
            checksum = 0;
            for (unsigned pos = basicStart - 1; pos < basicLoader.size(); pos++) checksum ^= basicLoader[pos];
            basicLoader.push_back(checksum);        // checksum


            // generate CODE block header
            char cbheader[24] = {
                0x13, 0x00, 0x00,                                               // size word, flag byte
                0x03,                                                           // code block
                0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,     // file name
                0x00, 0x00,                                                     // data block size
                0x00, -128, 0x00, -128,                                         // param1, param2
                0x00,                                                           // header checksum
                0x00, 0x00, -1                                                  // size word datablock, flag byte
            };

            cbheader[14] = static_cast<char>(currentTune.engineSize + currentTune.musicdataBinary.binLength);
            cbheader[15] = static_cast<char>((currentTune.engineSize + currentTune.musicdataBinary.binLength) >> 8);
            checksum = 0;
            for (int i = 2; i < 20; i++) checksum ^= cbheader[i];
            cbheader[20] = checksum;
            cbheader[21] = static_cast<char>(currentTune.engineSize + currentTune.musicdataBinary.binLength + 2);
            cbheader[22] = static_cast<char>((currentTune.engineSize + currentTune.musicdataBinary.binLength + 2) >> 8);

            for (auto&& it : basicLoader) SAVEFILE << it;
            SAVEFILE.write(&cbheader[0], 24);
            SAVEFILE.write(currentTune.engineCode, currentTune.engineSize);
            SAVEFILE.write(currentTune.musicdataBinary.binData, currentTune.musicdataBinary.binLength);

            // generate checksum
            checksum = -1;
            for (int i = 0; i < currentTune.engineSize; i++) checksum ^= currentTune.engineCode[i];
            for (int i = 0; i < currentTune.musicdataBinary.binLength; i++)
                checksum ^= currentTune.musicdataBinary.binData[i];
            SAVEFILE.write(&checksum, 1);
        }
    }

    al_destroy_native_file_dialog(fdialog);
}
