#include <iostream>
#include <string>
#include "gui.h"

using namespace std;


Main_Window::Main_Window(const string &initFile): userInputString("") {

	mainDisplay = nullptr;
	eventQueue = nullptr;
	timer = nullptr;
	currentField = nullptr;
	engineInfo = nullptr;

	if (!al_init_image_addon()) throw (string("Failed to initialize images."));
	if (!al_init_font_addon()) throw (string("Failed to initialize fonts."));
	if (!al_init_primitives_addon()) throw (string("Failed to initialize primitives."));
	if (!al_init_native_dialog_addon()) throw (string("Failed to initialize native dialogues."));

#ifdef ALLEGRO_GTK_TOPLEVEL
	al_set_new_display_flags(ALLEGRO_GTK_TOPLEVEL);
#endif

    //switch to single-buffered (no-flip) drawing method on request (to mitigate flicker on linux)
    if (settings.simpleGfxBuffer) al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 1, ALLEGRO_REQUIRE);

	mainDisplay = al_create_display(settings.xres, settings.yres);
	if (!mainDisplay) throw (string("Failed to create window."));


	timer = al_create_timer(DEFAULT_FPS());
	if (!timer) throw (string("Failed to initialize timer."));

	if (!al_install_keyboard()) throw (string("Failed to initialize keyboard."));
	if (!al_install_mouse()) throw (string("Failed to initialize mouse."));


	configListMenu = al_create_menu();
	for (unsigned i = 0; i < settings.configList.size(); i++) {

		al_append_menu_item(configListMenu, settings.configList[i].c_str(), (MENU_FILE_NEW_CONFIGSEL + i), 0, nullptr, nullptr);
	}
	newFilePopupMenu = al_clone_menu_for_popup(configListMenu);

	exportMenu = al_create_menu();
	al_append_menu_item(exportMenu, ".asm...", MENU_FILE_EXPORT_ASM, 0, nullptr, nullptr);
	al_append_menu_item(exportMenu, ".asm (Music Data only)...", MENU_FILE_EXPORT_ASM_MUSICDATA, 0, nullptr, nullptr);
	al_append_menu_item(exportMenu, ".bin...", MENU_FILE_EXPORT_BIN, 0, nullptr, nullptr);
	al_append_menu_item(exportMenu, ".bin (Music Data only)...", MENU_FILE_EXPORT_BIN_MUSICDATA, 0, nullptr, nullptr);
	al_append_menu_item(exportMenu, ".tap...", MENU_FILE_EXPORT_EMULFORMAT, 0, nullptr, nullptr);
	al_append_menu_item(exportMenu, ".wav...", MENU_FILE_EXPORT_WAV, ALLEGRO_MENU_ITEM_DISABLED, nullptr, nullptr);

	fileMenu = al_create_menu();
	al_append_menu_item(fileMenu, "New...", MENU_FILE_NEW, 0, nullptr, configListMenu);
	al_append_menu_item(fileMenu, "Open...", MENU_FILE_OPEN, 0, nullptr, nullptr);
	al_append_menu_item(fileMenu, "Save", MENU_FILE_SAVE, 0, nullptr, nullptr);
	al_append_menu_item(fileMenu, "Save As...", MENU_FILE_SAVEAS, 0, nullptr, nullptr);
	al_append_menu_item(fileMenu, "Export...", 0, 0, nullptr, exportMenu);
	al_append_menu_item(fileMenu, "Exit", MENU_FILE_EXIT, 0, nullptr, nullptr);

	interpolateMenu = al_create_menu();
	al_append_menu_item(interpolateMenu, "Linear (Ctrl+I)", MENU_EDIT_INTERPOLATE, 0, nullptr, nullptr);
	al_append_menu_item(interpolateMenu, "Cubic (Ctrl+Alt+I)", MENU_EDIT_EXPINTERPOLATE, 0, nullptr, nullptr);
	al_append_menu_item(interpolateMenu, "Reciprocal (Ctrl+Shift+I)", MENU_EDIT_LOGINTERPOLATE, 0, nullptr, nullptr);

	fillMenu = al_create_menu();
	al_append_menu_item(fillMenu, "Overwrite (Ctrl+F)", MENU_EDIT_FILL, 0, nullptr, nullptr);
	al_append_menu_item(fillMenu, "Porous (Ctrl+Shift+F)", MENU_EDIT_FILLPOROUS, 0, nullptr, nullptr);

	transposeMenu = al_create_menu();
	al_append_menu_item(transposeMenu, "+1 semitone (Ctrl+U)", MENU_EDIT_TRANSPOSE_SEMI_UP, 0, nullptr, nullptr);
	al_append_menu_item(transposeMenu, "-1 semitone (Ctrl+D)", MENU_EDIT_TRANSPOSE_SEMI_DOWN, 0, nullptr, nullptr);
	al_append_menu_item(transposeMenu, "+1 octave (Ctrl+Shift+U)", MENU_EDIT_TRANSPOSE_OCT_UP, 0, nullptr, nullptr);
	al_append_menu_item(transposeMenu, "-1 octave (Ctrl+Shift+D)", MENU_EDIT_TRANSPOSE_OCT_DOWN, 0, nullptr, nullptr);

	editMenu = al_create_menu();
	al_append_menu_item(editMenu, "Undo (Ctrl+Z)", MENU_EDIT_UNDO, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Redo (Ctrl+Y)", MENU_EDIT_REDO, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Select All (Ctrl+A)", MENU_EDIT_SELECTALL, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Copy (Ctrl+C)", MENU_EDIT_COPY, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Cut (Ctrl+X)", MENU_EDIT_CUT, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Delete (Ctrl+Del)", MENU_EDIT_DELETE, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Insert (Ctrl+P)", MENU_EDIT_INSERT, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Paste (Ctrl+V)", MENU_EDIT_PASTE, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Porous Paste (Ctrl+Shift+P)", MENU_EDIT_POROUSPASTE, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Inverse Porous Paste (Ctrl+Alt+P)", MENU_EDIT_INVERSEPOROUSPASTE, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Interpolate...", 0, 0, nullptr, interpolateMenu);
	al_append_menu_item(editMenu, "Fill...", 0, 0, nullptr, fillMenu);
	al_append_menu_item(editMenu, "Transpose...", 0, 0, nullptr, transposeMenu);
	al_append_menu_item(editMenu, "Reverse (Ctrl+R)", MENU_EDIT_REVERSE, 0, nullptr, nullptr);
	al_append_menu_item(editMenu, "Randomize", MENU_EDIT_RANDOMIZE, 0, nullptr, nullptr);

	playMenu = al_create_menu();
	al_append_menu_item(playMenu, "Play from Start (F5)", MENU_PLAY_PLAYFROMSTART, 0, nullptr, nullptr);
	al_append_menu_item(playMenu, "Play from Current Position (F6)", MENU_PLAY_PLAY, 0, nullptr, nullptr);
	al_append_menu_item(playMenu, "Play Pattern (F7)", MENU_PLAY_PLAYPATTERN, 0, nullptr, nullptr);
	al_append_menu_item(playMenu, "Stop (F8)", MENU_PLAY_STOP, 0, nullptr, nullptr);

	helpMenu = al_create_menu();
	al_append_menu_item(helpMenu, "Help (F1)", MENU_HELP_HELP, ALLEGRO_MENU_ITEM_DISABLED, nullptr, nullptr);
	al_append_menu_item(helpMenu, "Engine Info (F2)", MENU_HELP_ENGINE, 0, nullptr, nullptr);
	al_append_menu_item(helpMenu, "About", MENU_HELP_ABOUT, 0, nullptr, nullptr);

	menu = al_create_menu();
	al_append_menu_item(menu, "&File", 0, 0, nullptr, fileMenu);
	al_append_menu_item(menu, "&Edit", 0, 0, nullptr, editMenu);
	al_append_menu_item(menu, "&Play", 0, 0, nullptr, playMenu);
	al_append_menu_item(menu, "&Help", 0, 0, nullptr, helpMenu);
	al_set_display_menu(mainDisplay, menu);


    ALLEGRO_PATH *rPath = al_get_standard_path(ALLEGRO_EXENAME_PATH);
    al_set_path_filename(rPath, "");
	al_append_path_component(rPath, "resources/gui");
	al_change_directory(al_path_cstr(rPath, '/'));
	al_destroy_path(rPath);


	ALLEGRO_BITMAP *newFile;
	newFile = al_load_bitmap("new_file.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("open.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("save.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("save_as.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("play_from_start.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("play.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("play_pattern.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("stop.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("undo.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("redo.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("copy.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("cut.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("cut_and_move_up.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("paste_over.tga");
	menuButtons.push_back(newFile);
	newFile = al_load_bitmap("insert.tga");
	menuButtons.push_back(newFile);

	arrowLeft = al_load_bitmap("arrow_left.tga");
	arrowRight = al_load_bitmap("arrow_right.tga");
	arrowUp = al_load_bitmap("arrow_up.tga");
	arrowDown = al_load_bitmap("arrow_down.tga");
	buttonClone = al_load_bitmap("button_clone.tga");
	buttonExpand = al_load_bitmap("button_expand.tga");
	buttonInsert = al_load_bitmap("button_insert.tga");
	buttonMinus = al_load_bitmap("button_minus.tga");
	buttonPlus = al_load_bitmap("button_plus.tga");
	buttonRemove = al_load_bitmap("button_remove.tga");
	buttonShrink = al_load_bitmap("button_shrink.tga");
	buttonLooppoint = al_load_bitmap("button_looppoint.tga");


	eventQueue = al_create_event_queue();
	if(!eventQueue) throw (string("Failed to initialize event queue."));
	al_register_event_source(eventQueue, al_get_display_event_source(mainDisplay));
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));
//	al_register_event_source(eventQueue, al_get_keyboard_event_source());
//	al_register_event_source(eventQueue, al_get_mouse_event_source());
	al_register_event_source(eventQueue, al_get_default_menu_event_source());

	font = al_load_bitmap_font("gohufont.tga");
	if (!font) throw (string("Failed to load font."));

	al_change_directory("../../");


	currentTune.init(initFile, settings, &status);
    soundEmul.init(&currentTune, settings.audioChunkSize);

	al_register_event_source(eventQueue, al_get_audio_stream_event_source(soundEmul.audioStream));

	init_track_view();
	set_display_title();

	al_flip_display();

	//work-around for "hanging" keys on startup
	al_uninstall_keyboard();
	al_install_keyboard();

	al_start_timer(timer);

	isUp = true;

}


Main_Window::~Main_Window() {

    al_close_native_text_log(engineInfo);
    if (newFilePopupMenu) al_destroy_menu(newFilePopupMenu);
	al_set_display_menu(mainDisplay, nullptr);
	al_shutdown_native_dialog_addon();
	al_destroy_event_queue(eventQueue);
	al_destroy_timer(timer);
	al_destroy_display(mainDisplay);
	al_uninstall_system();
}


void Main_Window::init_track_view() {

    if (engineInfo) {
        al_unregister_event_source(eventQueue, al_get_native_text_log_event_source(engineInfo));
        al_close_native_text_log(engineInfo);
        engineInfo = nullptr;
    }

	tabPanel.clear();
    undoStack.clear();
	redoStack.clear();
	dropdown.collapse();

	al_clear_to_color(settings.bgColor);

	buttonPanel.create(settings.buttonPanelArea, DIR_BOTTOM, DIR_NONE, settings.sysColor, this);
	referencePanel.create(settings.referencePanelArea, DIR_LEFT, DIR_RIGHT, settings.sysColor, this);
	blockPanel.create(settings.blockPanelArea, DIR_NONE, DIR_RIGHT|DIR_BOTTOM, settings.sysColor, this);
	messagePanel.create(settings.messagePanelArea, DIR_TOP, DIR_NONE, settings.sysColor, this);

	float xpos = DEFAULT_MARGIN();
	for (auto&& it: menuButtons) {

		al_draw_tinted_bitmap(it, settings.sysColor, xpos, 6.0f, 0);
		xpos += (BUTTON_SIZE() + 2.0f * DEFAULT_MARGIN());			//36.0;
	}

	al_draw_text(font, settings.sysColor, xpos, buttonPanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "BASE OCT");
	print_base_octave();
	al_draw_text(font, settings.sysColor, xpos,
        buttonPanel.printArea.bottomRight.y - CHAR_HEIGHT() + 2.0f, ALLEGRO_ALIGN_LEFT, "AUTOINC");
	print_autoinc_setting();
	xpos += (10 * BT_CHAR_WIDTH() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(arrowDown, settings.sysColor, xpos, buttonPanel.printArea.topLeft.y - 1.0f, 0);
	al_draw_tinted_bitmap(arrowDown, settings.sysColor, xpos, buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
	xpos += (SMALL_BUTTON_SIZE() + 2.0f);
	al_draw_tinted_bitmap(arrowUp, settings.sysColor, xpos, buttonPanel.printArea.topLeft.y - 1.0f, 0);
	al_draw_tinted_bitmap(arrowUp, settings.sysColor, xpos, buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
// 	al_draw_text(font, settings.rowHlColor, xpos + (8 * BT_CHAR_WIDTH()), buttonPanel.printArea.topLeft.y + CHAR_HEIGHT() + DEFAULT_MARGIN(),
// 		ALLEGRO_ALIGN_LEFT, numToStr(status.autoInc, (settings.hexMode) ? 2 : 3, settings.hexMode).data());

	xpos = 0.0f;
	tabPanel.emplace_back(Block_Tab(xpos, "General"));

	for (auto&& it: currentTune.config.blockTypes) {

		xpos += (tabPanel.back().xsize + THICK_LINE());
		tabPanel.emplace_back(Block_Tab(xpos, it.blockConfigID.data()));
	}

	status.init(&currentTune, settings);
	print_block_tabs();

	currentTune.update_last_used(0, 0);

//    cout << userInputString << "\n";
//	userInputString = "";

}


void Main_Window::print_base_octave() {

	float xpos = DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (9 * BT_CHAR_WIDTH());

	al_draw_filled_rectangle(xpos, buttonPanel.printArea.topLeft.y, xpos + BT_CHAR_WIDTH(),
		buttonPanel.printArea.topLeft.y + CHAR_HEIGHT(), settings.bgColor);

	al_draw_text(font, settings.rowHlColor, xpos, buttonPanel.printArea.topLeft.y,
		ALLEGRO_ALIGN_LEFT, numToStr(status.currentOctave, 0, settings.hexMode).data());
}


void Main_Window::print_autoinc_setting() {

	float xpos = DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (8 * BT_CHAR_WIDTH());

	al_draw_filled_rectangle(xpos, buttonPanel.printArea.bottomRight.y - CHAR_HEIGHT() + 2.0f, xpos + (2 * BT_CHAR_WIDTH()),
		buttonPanel.printArea.bottomRight.y, settings.bgColor);

	al_draw_text(font, settings.rowHlColor, xpos, buttonPanel.printArea.bottomRight.y - CHAR_HEIGHT() + 2.0f,
		ALLEGRO_ALIGN_LEFT, numToStr(status.autoInc, (settings.hexMode) ? 2 : 3, settings.hexMode).data());
}


void Main_Window::print_block_name() {
	settings.blockNameArea.clear(settings.bgColor);
	al_draw_text(font, settings.rowHlColor, blockPanel.printArea.topLeft.x + SMALL_BUTTON_SIZE() + DEFAULT_MARGIN(),
		settings.tabArea.bottomRight.y + DEFAULT_MARGIN() + 2.0f, ALLEGRO_ALIGN_LEFT,
		(status.editBlockName) ? userInputString.data() : status.get_current_block_pointer()->name.data());
}


void Main_Window::print_block_tabs() {

    //bottom border
    al_draw_line(0.0f, settings.tabArea.bottomRight.y, blockPanel.printArea.bottomRight.x + DEFAULT_MARGIN(),
        settings.tabArea.bottomRight.y, settings.sysColor, THICK_LINE());

	for (size_t i = 0; i < tabPanel.size(); i++) {

		al_draw_text(font, settings.sysColor, (tabPanel[i].xpos + DEFAULT_MARGIN()),
			settings.tabArea.topLeft.y + DEFAULT_MARGIN(), ALLEGRO_ALIGN_LEFT, tabPanel[i].title.data());

		//vertical seperator
		al_draw_line((tabPanel[i].xpos + tabPanel[i].xsize + THIN_LINE()), settings.tabArea.topLeft.y,
            (tabPanel[i].xpos + tabPanel[i].xsize + THIN_LINE()),
			settings.tabArea.bottomRight.y, settings.sysColor, THICK_LINE());

		//erase bottom border on active tab
        if (i == status.currentTab) al_draw_line(tabPanel[i].xpos, settings.tabArea.bottomRight.y,
            tabPanel[i].xpos + tabPanel[i].xsize, settings.tabArea.bottomRight.y, settings.bgColor, THICK_LINE());
	}

	if (status.currentTab == 0) print_globals();
	else {
		print_reference_panel();
		print_block();
	}
}


void Main_Window::print_block() {

	float xpos = blockPanel.printArea.topLeft.x;
	al_draw_tinted_bitmap(arrowLeft, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + 12 * BT_CHAR_WIDTH() + 3 * DEFAULT_MARGIN());
	al_draw_tinted_bitmap(arrowRight, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	//TODO: fields for block length and expand/shrink param
	al_draw_text(font, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN() + 2.0f, ALLEGRO_ALIGN_LEFT, "LENGTH");
	xpos += ((10 * BT_CHAR_WIDTH()) + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonPlus, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonMinus, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonInsert, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonRemove, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	al_draw_text(font, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN() + 2.0f, ALLEGRO_ALIGN_LEFT, "x");
	xpos += BT_CHAR_WIDTH();
	al_draw_text(font, settings.rowHlColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN() + 2.0f, ALLEGRO_ALIGN_LEFT,
		numToStr(status.expandFactor, 0, settings.hexMode).data());
	xpos += (BT_CHAR_WIDTH() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonExpand, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
	al_draw_tinted_bitmap(buttonShrink, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);

	al_draw_text(font, settings.sysColor, settings.rowNumberArea.topLeft.x, settings.rowNumberArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "ROW");

	print_column_titles();
	print_block_data();
}


void Main_Window::print_globals() {

	settings.blockArea.clear(settings.bgColor);		//TODO this is the only occurance of "blockArea" outside of settings -> remove and make private
	float ypos = settings.blockDataArea.topLeft.y;
	unsigned pos = 0;

	for (auto&& it: currentTune.globalConstants) {

		al_draw_text(font, settings.sysColor, settings.blockDataArea.topLeft.x, ypos, ALLEGRO_ALIGN_LEFT, it.command->mdCmdName.data());
		if (pos == status.currentGlobal) al_draw_filled_rectangle(settings.blockDataArea.topLeft.x + (12 * BT_CHAR_WIDTH()), ypos,
			settings.blockDataArea.topLeft.x + ((12 + it.printString.size()) * BT_CHAR_WIDTH()), ypos + CHAR_HEIGHT(),
			settings.cursorColor);
		al_draw_text(font, (pos == status.currentGlobal) ? settings.rowActColor : settings.rowColor,
		settings.blockDataArea.topLeft.x + (12 * BT_CHAR_WIDTH()), ypos, ALLEGRO_ALIGN_LEFT, it.printString.data());
		ypos += CHAR_HEIGHT();
		pos++;
	}

	al_draw_filled_rectangle(settings.blockPanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
        settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE(), settings.blockPanelArea.bottomRight.x - THIN_LINE(),
        settings.blockPanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);

    al_draw_filled_rectangle(SMALL_BUTTON_SIZE(), settings.blockPanelArea.bottomRight.y - SCROLL_BAR_SIZE(),
        settings.blockPanelArea.bottomRight.x - (2 * SMALL_BUTTON_SIZE()),
        settings.blockPanelArea.bottomRight.y - THIN_LINE(), settings.bgColor);

    print_stats();
}


void Main_Window::print_stats() {

    referencePanel.printArea.clear(settings.bgColor);
    settings.referenceDataArea.clear(settings.bgColor);

    float xpos = referencePanel.printArea.topLeft.x;
    float ypos = referencePanel.printArea.topLeft.y;

    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "INFO");

    ypos += (2 * CHAR_HEIGHT());
    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Engine");
    ypos += CHAR_HEIGHT();
    al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, currentTune.configName.data());
    ypos += (2 * CHAR_HEIGHT());
    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Platform");
    ypos += CHAR_HEIGHT();
    al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, currentTune.config.targetPlatform.data());

    for (unsigned i = 0; i < currentTune.config.blockTypes.size(); i++) {

        ypos += (2 * CHAR_HEIGHT());
        al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
            currentTune.config.blockTypes[i].blockConfigID.data());
        ypos += CHAR_HEIGHT();
        al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
            numToStr(currentTune.blockTypes[i].blocks.size(), 0, settings.hexMode).data());
        if (!i) {
            ypos += (2 * CHAR_HEIGHT());
            al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Sequence Length");
            ypos += CHAR_HEIGHT();
            al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
                numToStr(currentTune.sequence.size(), 0, settings.hexMode).data());
        }
    }

    currentTune.generate_module_lines();
	bool verbose = false;
	mdModule mdf(currentTune.moduleLines, currentTune.config, verbose);

	vector<string> asmLines;
	istringstream is(mdf.MUSICASM.str());
	string tempstr;
	while (getline(is, tempstr)) asmLines.push_back(tempstr);

	currentTune.musicdataBinary.assemble(asmLines, currentTune.orgAddress + currentTune.engineSize, true);

    ypos += (2 * CHAR_HEIGHT());
    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Binsize (looped)");
    ypos += CHAR_HEIGHT();
    al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
        numToStr(currentTune.musicdataBinary.binLength + currentTune.engineSize, 0, settings.hexMode).data());
    ypos += (2 * CHAR_HEIGHT());
    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Binsize (no loop)");
    ypos += CHAR_HEIGHT();
    al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
        numToStr(currentTune.musicdataBinary.binLength + currentTune.engineSizeNoLoop, 0, settings.hexMode).data());
    ypos += (2 * CHAR_HEIGHT());
    al_draw_text(font, settings.sysColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, "Music Data Size");
    ypos += CHAR_HEIGHT();
    al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
        numToStr(currentTune.musicdataBinary.binLength, 0, settings.hexMode).data());
}


void Main_Window::print_reference_panel() {
	//TODO: blockHeader ypos is calculated from tabPanel, but referenceHeader x is calculated as referencePanel.printArea.topLeft.x + 3 * BT_CHAR_WIDTH()

	referencePanel.printArea.clear(settings.bgColor);

	float xpos = referencePanel.printArea.topLeft.x;

	if (status.currentTab == 1) {

		al_draw_text(font, settings.sysColor, referencePanel.printArea.topLeft.x, referencePanel.printArea.topLeft.y,
			ALLEGRO_ALIGN_LEFT, "SEQUENCE");

		al_draw_tinted_bitmap(buttonPlus, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
//		al_draw_tinted_bitmap(buttonMinus, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
//		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonInsert, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonRemove, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonClone, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonLooppoint, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	}
	else if (status.currentTab > 1) {

		al_draw_text(font, settings.sysColor, referencePanel.printArea.topLeft.x, referencePanel.printArea.topLeft.y,
			ALLEGRO_ALIGN_LEFT, currentTune.config.blockTypes[status.get_current_blocktype()].blockConfigID.data());

		al_draw_tinted_bitmap(buttonPlus, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonRemove, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
		xpos += (SMALL_BUTTON_SIZE() + DEFAULT_MARGIN());
		al_draw_tinted_bitmap(buttonClone, settings.sysColor, xpos, settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
	}

	print_reference_data();
}


void Main_Window::print_reference_data() {

    if (status.currentTab == 0) {
        print_stats();
        return;
    }

    settings.referenceDataArea.clear(settings.bgColor);
	float ypos = settings.referenceDataArea.topLeft.y;

	//TODO printing sequence should depend on blocktype config, not status == 0
	if (status.get_current_blocktype() == 0) {

        //print sequence
        for (size_t pos = status.get_visible_first_reference_row(); pos < currentTune.sequence.size(); pos++) {

            ALLEGRO_COLOR bgColor = (status.is_selected(pos)) ? settings.selectionColor : settings.bgColor;
            if ((!status.focusBlock) && (pos == status.get_current_reference_row())) bgColor = settings.cursorColor;

            al_draw_filled_rectangle(settings.referenceDataArea.topLeft.x, ypos,
                settings.referenceDataArea.topLeft.x + (16 * BT_CHAR_WIDTH()), ypos + CHAR_HEIGHT(), bgColor);

            ALLEGRO_COLOR color = (currentTune.sequence[pos] == status.get_current_block_pointer()->name)
                ? settings.rowHlColor : settings.rowColor;
            if (pos == status.get_current_reference_row()) color = settings.rowActColor;

            string posstr = numToStr(static_cast<long>(pos), 3, settings.hexMode);
            al_draw_text(font, color, settings.referenceDataArea.topLeft.x + 3 * BT_CHAR_WIDTH(), ypos, ALLEGRO_ALIGN_RIGHT, posstr.data());

            if (pos == currentTune.sequenceLoopPoint)
                al_draw_text(font, color, settings.referenceDataArea.topLeft.x + 3 * BT_CHAR_WIDTH(), ypos, ALLEGRO_ALIGN_LEFT, "~");

            al_draw_text(font, color, settings.referenceDataArea.topLeft.x + 4 * BT_CHAR_WIDTH(), ypos, ALLEGRO_ALIGN_LEFT, currentTune.sequence[pos].data());
            ypos += CHAR_HEIGHT();

            if ((ypos + CHAR_HEIGHT()) > settings.referenceDataArea.bottomRight.y) break;
        }

        al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
            settings.referencePanelArea.topLeft.y + SMALL_BUTTON_SIZE(), settings.referencePanelArea.bottomRight.x,
            settings.referencePanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);

        status.vScrollbarRefSize = (static_cast<float>(status.visibleReferenceRowsMax)
            / static_cast<float>(currentTune.sequence.size()));

        if (status.vScrollbarRefSize < 1.0f) {

            status.vScrollbarRefStep = ((settings.referencePanelArea.bottomRight.y
            - settings.referencePanelArea.topLeft.y - (2* SMALL_BUTTON_SIZE()))
            / static_cast<float>(currentTune.sequence.size()));

            status.vScrollbarRefSize *= (settings.referencePanelArea.bottomRight.y - settings.referencePanelArea.topLeft.y
                - (2* SMALL_BUTTON_SIZE()));

            status.vScrollbarRefStart = static_cast<float>(status.get_visible_first_reference_row())
                * ((settings.blockPanelArea.bottomRight.y - settings.blockPanelArea.topLeft.y - (2* SMALL_BUTTON_SIZE()))
                / static_cast<float>(currentTune.sequence.size()));
            status.vScrollbarRefStart += settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE();

            al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
                status.vScrollbarRefStart, settings.referencePanelArea.bottomRight.x,
                status.vScrollbarRefStart + status.vScrollbarRefSize, settings.sysColor);
        }
        else status.vScrollbarRefSize = 0.0f;
	}
	else {

        //print reference list
        for (size_t pos = status.get_visible_first_reference_row(); pos < currentTune.blockTypes[status.get_current_blocktype()].blocks.size(); pos++) {

            al_draw_filled_rectangle(settings.referenceDataArea.topLeft.x, ypos,
                settings.referenceDataArea.topLeft.x + (12 * BT_CHAR_WIDTH()), ypos + CHAR_HEIGHT(),
                (((!status.focusBlock) && (pos == status.get_current_reference_row())) ? settings.cursorColor : settings.bgColor));

            ALLEGRO_COLOR color = (currentTune.blockTypes[status.get_current_blocktype()].blocks[pos].name == status.get_current_block_pointer()->name)
                ? settings.rowHlColor : settings.rowColor;
            if (!status.focusBlock && pos == status.get_current_reference_row()) color = settings.rowActColor;

            al_draw_text(font, color, settings.referenceDataArea.topLeft.x, ypos, ALLEGRO_ALIGN_LEFT,
                currentTune.blockTypes[status.get_current_blocktype()].blocks[pos].name.data());
            ypos += CHAR_HEIGHT();

            if ((ypos + CHAR_HEIGHT()) > settings.referenceDataArea.bottomRight.y) break;
        }

        al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_PANE_SIZE(),
            settings.referencePanelArea.topLeft.y + SMALL_BUTTON_SIZE(), settings.referencePanelArea.bottomRight.x,
            settings.referencePanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);


        status.vScrollbarRefSize = (static_cast<float>(status.visibleReferenceRowsMax)
            / static_cast<float>(currentTune.blockTypes[status.get_current_blocktype()].blocks.size()));

        if (status.vScrollbarRefSize < 1.0f) {

            status.vScrollbarRefStep = ((settings.referencePanelArea.bottomRight.y
            - settings.referencePanelArea.topLeft.y - (2* SMALL_BUTTON_SIZE()))
            / static_cast<float>(currentTune.blockTypes[status.get_current_blocktype()].blocks.size()));

            status.vScrollbarRefSize *= (settings.blockPanelArea.bottomRight.y - settings.blockPanelArea.topLeft.y
                - (2* SMALL_BUTTON_SIZE()));

            status.vScrollbarRefStart = static_cast<float>(status.get_visible_first_reference_row())
                * ((settings.blockPanelArea.bottomRight.y - settings.blockPanelArea.topLeft.y - (2* SMALL_BUTTON_SIZE()))
                / static_cast<float>(currentTune.blockTypes[status.get_current_blocktype()].blocks.size()));
            status.vScrollbarRefStart += settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE();

            al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_PANE_SIZE(),
                status.vScrollbarRefStart, settings.referencePanelArea.bottomRight.x,
                status.vScrollbarRefStart + status.vScrollbarRefSize, settings.sysColor);
        }
        else status.vScrollbarRefSize = 0.0f;
	}

    if (dropdown.isActive) print_dropdown();
}


void Main_Window::print_column_titles() {

	settings.columnHeaderArea.clear(settings.bgColor);

	float xpos = settings.columnHeaderArea.topLeft.x;
	status.calculate_visible_last_column();

	for (unsigned cmd = status.get_visible_first_column(); cmd <= status.get_visible_last_column(); cmd++) {

		al_draw_text(font, settings.sysColor, xpos, settings.columnHeaderArea.topLeft.y, ALLEGRO_ALIGN_LEFT,
			currentTune.blockTypes[status.get_current_blocktype()].commands[cmd]->mdCmdName.data());

		xpos += (status.get_current_block_pointer()->columns[cmd].width + BT_CHAR_WIDTH());
	}
}


void Main_Window::print_block_data() {

	if (status.currentTab == 0) {

		print_globals();
		return;
	}

    print_block_name();

	settings.blockLengthArea.clear(settings.bgColor);

	al_draw_text(font, settings.rowHlColor, settings.blockLengthArea.topLeft.x, settings.blockLengthArea.topLeft.y,
		ALLEGRO_ALIGN_LEFT, numToStr(status.get_current_block_pointer()->columns[0].columnData.size(), 3,
		settings.hexMode).data());

	settings.blockDataArea.clear(settings.bgColor);

	for (size_t row = status.get_visible_first_row(); row < (status.get_visible_first_row() + status.visibleRowsMax)
		&& (row < status.get_current_block_pointer()->columns[0].columnData.size()); row++)
		print_row(row);

    al_draw_filled_rectangle(settings.blockPanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
        settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE(), settings.blockPanelArea.bottomRight.x - THIN_LINE(),
        settings.blockPanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);

    al_draw_filled_rectangle(SMALL_BUTTON_SIZE(), settings.blockPanelArea.bottomRight.y - SCROLL_BAR_SIZE(),
        settings.blockPanelArea.bottomRight.x - (2 * SMALL_BUTTON_SIZE()),
        settings.blockPanelArea.bottomRight.y - THIN_LINE(), settings.bgColor);

    //generate vertical scrollbar
    status.vScrollbarBlkSize = (static_cast<float>(status.visibleRowsMax)
        / static_cast<float>(status.get_current_block_pointer()->columns[0].columnData.size()));
    if (status.vScrollbarBlkSize < 1.0f) {
        status.vScrollbarBlkSize *= (settings.blockPanelArea.bottomRight.y - settings.blockPanelArea.topLeft.y
            - (2* SMALL_BUTTON_SIZE()));

        status.vScrollbarBlkStep = ((settings.blockPanelArea.bottomRight.y
            - settings.blockPanelArea.topLeft.y - (2* SMALL_BUTTON_SIZE()))
            / static_cast<float>(status.get_current_block_pointer()->columns[0].columnData.size()));

        status.vScrollbarBlkStart = static_cast<float>(status.get_visible_first_row()) * status.vScrollbarBlkStep;
        status.vScrollbarBlkStart += settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE();

        al_draw_filled_rectangle(settings.blockPanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
            status.vScrollbarBlkStart, settings.blockPanelArea.bottomRight.x - THIN_LINE(),
            status.vScrollbarBlkStart + status.vScrollbarBlkSize, settings.sysColor);
    }
    else status.vScrollbarBlkSize = 0.0f;

    //generate horizontal scrollbar
    float hTotalSize = 0;
    for (auto&& it: status.get_current_block_pointer()->columns) hTotalSize += (it.width + BT_CHAR_WIDTH());
    status.hScrollbarBlkSize = 0;
    for (unsigned col = status.get_visible_first_column(); col <= status.get_visible_last_column(); col++)
        status.hScrollbarBlkSize += (status.get_current_block_pointer()->columns[col].width + BT_CHAR_WIDTH());
    status.hScrollbarBlkSize /= hTotalSize;

    if (status.hScrollbarBlkSize < 1.0f) {

        status.hScrollbarBlkSize *= (settings.blockPanelArea.bottomRight.x - (3* SMALL_BUTTON_SIZE()));
        status.hScrollbarBlkStart = 0.0f;

        for (unsigned col = 0; col < status.get_visible_first_column(); col++)
            status.hScrollbarBlkStart += (status.get_current_block_pointer()->columns[col].width + BT_CHAR_WIDTH());
        status.hScrollbarBlkStart /= hTotalSize;
        status.hScrollbarBlkStart *= (settings.blockPanelArea.bottomRight.x - (3* SMALL_BUTTON_SIZE()));
        status.hScrollbarBlkStart += SMALL_BUTTON_SIZE();

        al_draw_filled_rectangle(status.hScrollbarBlkStart, settings.blockPanelArea.bottomRight.y - SCROLL_BAR_SIZE(),
            status.hScrollbarBlkStart + status.hScrollbarBlkSize, settings.blockPanelArea.bottomRight.y - THIN_LINE(),
            settings.sysColor);
    }
    else status.hScrollbarBlkSize = 0.0f;

    if (dropdown.isActive) print_dropdown();
}


void Main_Window::print_row(const unsigned &row) {

	ALLEGRO_COLOR txtColor = (row % status.rowHighlight) ? settings.rowColor : settings.rowHlColor;

	al_draw_text(font, ((row == status.get_current_cursor_row()) && status.focusBlock) ? settings.rowActColor : txtColor,
		settings.rowNumberArea.topLeft.x + 3 * BT_CHAR_WIDTH(),
		settings.blockDataArea.topLeft.y + (static_cast<float>(row - status.get_visible_first_row()) * CHAR_HEIGHT()), ALLEGRO_ALIGN_RIGHT,
		numToStr(static_cast<long>(row), 3, settings.hexMode).data());

	for (unsigned col = status.get_visible_first_column(); (col <= status.get_visible_last_column())
		&& (col < status.get_current_block_pointer()->columns.size()); col++) print_field(col, row);
}


void Main_Window::print_field(const unsigned &col, const unsigned &row) {

	float xpos = settings.columnHeaderArea.topLeft.x;

	for (unsigned vcol = status.get_visible_first_column(); vcol < col; vcol++)
		xpos += (status.get_current_block_pointer()->columns[vcol].width + BT_CHAR_WIDTH());
	float ypos = settings.blockDataArea.topLeft.y + (static_cast<float>(row - status.get_visible_first_row()) * CHAR_HEIGHT());


    ALLEGRO_COLOR bgColor = (status.focusBlock && status.is_selected(row, col)) ? settings.selectionColor : settings.bgColor;
    if (status.focusBlock && (col == status.get_current_cursor_column()) && (row == status.get_current_cursor_row()))
        bgColor = settings.cursorColor;

	al_draw_filled_rectangle(xpos, ypos, xpos + status.get_current_block_pointer()->columns[col].width,
		ypos + CHAR_HEIGHT(), bgColor);

	ALLEGRO_COLOR txtColor = (row % status.rowHighlight) ? settings.rowColor : settings.rowHlColor;
	al_draw_text(font, (status.focusBlock && (row == status.get_current_cursor_row())) ? settings.rowActColor : txtColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
		status.get_current_block_pointer()->columns[col].columnData[row].printString.data());
}

void Main_Window::print_global_field(const unsigned &row) {

	float xpos = settings.blockDataArea.topLeft.x + (12 * BT_CHAR_WIDTH());
	float ypos = settings.blockDataArea.topLeft.y + (static_cast<float>(row * CHAR_HEIGHT()));

	al_draw_filled_rectangle(xpos, ypos, settings.blockDataArea.bottomRight.x, ypos + CHAR_HEIGHT(),
		((status.focusBlock &&  (row == status.currentGlobal)) ? settings.cursorColor : settings.bgColor));

	al_draw_text(font, (status.focusBlock && row == status.currentGlobal) ? settings.rowActColor : settings.rowColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
		currentTune.globalConstants[row].printString.data());
}


void Main_Window::print_dropdown() {

	//TODO: limit list output if ypos out of blockDataArea bounds
//	cout << "display_options_list\n";
	if (status.focusBlock) {

		if (status.currentTab > 0) {

			float xpos = settings.columnHeaderArea.topLeft.x;
			for (unsigned vcol = status.get_visible_first_column(); vcol < status.get_current_cursor_column(); vcol++)
				xpos += (status.get_current_block_pointer()->columns[vcol].width + BT_CHAR_WIDTH());

			float ypos = settings.blockDataArea.topLeft.y + (static_cast<float>(status.get_current_cursor_row()
				- status.get_visible_first_row()) * CHAR_HEIGHT());

			if (status.get_current_cursor_row() - status.get_visible_first_row() >= status.visibleRowsMax / 2) {

				//print upwards
				//TODO: make window size dependant on command type, see downwards print
				ypos -= static_cast<float>(dropdown.options.size() * CHAR_HEIGHT());

				al_draw_filled_rectangle(xpos, (ypos >= settings.blockDataArea.topLeft.y) ? ypos : settings.blockDataArea.topLeft.y,
					xpos + ((currentField->command->mdCmdType == MD_WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					(ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())), settings.sysColor);
				al_draw_filled_rectangle(xpos + 1.0f, (ypos >= settings.blockDataArea.topLeft.y)
					? ypos + 1.0f : settings.blockDataArea.topLeft.y + 1.0f,
					xpos + ((currentField->command->mdCmdType == MD_WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())) - 1.0f,
					(ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())) - 1.0f, settings.bgColor);

//				for (auto&& it: options) {
                for (unsigned i = 0; i < dropdown.options.size(); i++) {

                    ALLEGRO_COLOR color = (i == dropdown.selectedOption) ? settings.rowHlColor : settings.rowColor;
					if (ypos >= settings.blockDataArea.topLeft.y)
						al_draw_text(font, color, xpos, ypos, ALLEGRO_ALIGN_LEFT, dropdown.options[i].data());
					ypos += CHAR_HEIGHT();
				}

				al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
                    (dropdown.userEntry == "") ? "_" : dropdown.userEntry.data());
			}
			else {

				//print downwards
				al_draw_filled_rectangle(xpos, ypos, xpos + ((currentField->command->mdCmdType == MD_WORD)
					? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					((ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) < settings.blockDataArea.bottomRight.y)
					? (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())) : settings.blockDataArea.bottomRight.y),
					settings.sysColor);
				al_draw_filled_rectangle(xpos + 1.0f, ypos + 1.0f, xpos - 1.0f
					+ ((currentField->command->mdCmdType == MD_WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					(ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) < settings.blockDataArea.bottomRight.y)
					? (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) - 1.0f)
					: (settings.blockDataArea.bottomRight.y - 1.0f), settings.bgColor);

				al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
                    (dropdown.userEntry == "") ? "_" : dropdown.userEntry.data());

				for (unsigned i = 0; i < dropdown.options.size(); i++) {

                    ALLEGRO_COLOR color = (i == dropdown.selectedOption) ? settings.rowHlColor : settings.rowColor;

					ypos += CHAR_HEIGHT();

					if ((ypos + CHAR_HEIGHT()) > settings.blockDataArea.bottomRight.y) break;

					al_draw_text(font, color, xpos, ypos, ALLEGRO_ALIGN_LEFT, dropdown.options[i].data());
				}
			}
		}
		else {

			//TODO: options list for global input
		}
	}

	else {

		float xpos = settings.referenceDataArea.topLeft.x;
		if (status.get_current_blocktype() == 0) xpos += (4 * BT_CHAR_WIDTH());

		float ypos = settings.referenceDataArea.topLeft.y + (static_cast<float>(status.get_current_reference_row()
				- status.get_visible_first_reference_row()) * CHAR_HEIGHT());


		if (status.get_current_reference_row() - status.get_visible_first_reference_row() >= status.visibleReferenceRowsMax / 2) {

			//print upwards
			ypos -= static_cast<float>(dropdown.options.size() * CHAR_HEIGHT());

			al_draw_filled_rectangle(xpos, (ypos >= settings.referenceDataArea.topLeft.y) ? ypos : settings.referenceDataArea.topLeft.y,
				xpos + (12.0 * BT_CHAR_WIDTH()), (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())), settings.sysColor);
			al_draw_filled_rectangle(xpos + 1.0f, (ypos >= settings.referenceDataArea.topLeft.y)
				? ypos + 1.0f : settings.referenceDataArea.topLeft.y + 1.0f,
				xpos + (12.0 * BT_CHAR_WIDTH()) - 1.0f, (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())) - 1.0f,
				settings.bgColor);

			for (unsigned i = 0; i < dropdown.options.size(); i++) {

                ALLEGRO_COLOR color = (i == dropdown.selectedOption) ? settings.rowHlColor : settings.rowColor;

				if (ypos >= settings.referenceDataArea.topLeft.y)
					al_draw_text(font, color, xpos, ypos, ALLEGRO_ALIGN_LEFT, dropdown.options[i].data());
				ypos += CHAR_HEIGHT();
			}

			al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
                (dropdown.userEntry == "") ? "_" : dropdown.userEntry.data());

		}
		else {

			//print downwards
			al_draw_filled_rectangle(xpos, ypos, xpos + (12.0 * BT_CHAR_WIDTH()),
				((ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) < settings.referenceDataArea.bottomRight.y)
				? (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT())) : settings.referenceDataArea.bottomRight.y),
				settings.sysColor);
			al_draw_filled_rectangle(xpos + 1.0f, ypos + 1.0f, xpos - 1.0f + (12.0 * BT_CHAR_WIDTH()),
				(ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) < settings.referenceDataArea.bottomRight.y)
				? (ypos + static_cast<float>((dropdown.options.size() + 1) * CHAR_HEIGHT()) - 1.0f)
				: (settings.referenceDataArea.bottomRight.y - 1.0f), settings.bgColor);

			al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT,
                (dropdown.userEntry == "") ? "_" : dropdown.userEntry.data());

			for (unsigned i = 0; i < dropdown.options.size(); i++) {

                ALLEGRO_COLOR color = (i == dropdown.selectedOption) ? settings.rowHlColor : settings.rowColor;
				ypos += CHAR_HEIGHT();

				if ((ypos + CHAR_HEIGHT()) > settings.referenceDataArea.bottomRight.y) break;
				al_draw_text(font, color, xpos, ypos, ALLEGRO_ALIGN_LEFT, dropdown.options[i].data());
			}
		}
	}
}


void Main_Window::set_display_title() {

    string title = "bintracker - ";
    string trackname;
    if (currentTune.hasUnsavedChanges) trackname = "*";
    if (currentTune.savefilePath == "") trackname += "unnamed";
    else if (currentTune.savefilePath.size() < 237) trackname += currentTune.savefilePath;
    else trackname += ("..." + currentTune.savefilePath.substr(currentTune.savefilePath.size() - 237, 237));
    title += trackname;
    al_set_window_title(mainDisplay, title.data());
}


void Main_Window::shutdown() {

    cancel_data_input();

    soundEmul.stop();

    if (!currentTune.hasUnsavedChanges) {

        isUp = false;
        return;
    }

//    int doWhat = al_show_native_message_box(nullptr, "Save unsaved changes?", "",
//        "The current tune has unsaved changes. Save changes before exiting?", "Cancel|Discard Changes|Save Changes",
//        ALLEGRO_MESSAGEBOX_QUESTION);
//
//    if (doWhat < 2) return;
//    else if (doWhat == 3) {
//        save_file();
//        if (currentTune.hasUnsavedChanges) return;
//    }

    //TODO temporary work-around because Windows version of liballeg ignores custom buttons,
    // and is not able to return the correct button pressed either
    int doWhat = al_show_native_message_box(nullptr, "Save unsaved changes?", "",
        "The current tune has unsaved changes. Save changes before exiting?", nullptr,
        ALLEGRO_MESSAGEBOX_QUESTION|ALLEGRO_MESSAGEBOX_YES_NO);

//    if (doWhat == 0) return;
//    else if (doWhat == 1) {
    if (doWhat == 1) {
        save_file();
        if (currentTune.hasUnsavedChanges) return;
    }

    isUp = false;
    return;
}


void Main_Window::display_about_msg() {

	al_show_native_message_box(nullptr, "About Bintracker", "Bintracker 0.1.2", "by utz 2017\nwww.irrlichtproject.de", nullptr, 0);
}

bool Main_Window::display_confirm_overwrite_msg() {

	if (al_show_native_message_box(nullptr, "Overwrite File?", "", "This file already exists. Overwrite it?",
		nullptr, ALLEGRO_MESSAGEBOX_QUESTION|ALLEGRO_MESSAGEBOX_YES_NO) == 1) return true;

	return false;
}

bool Main_Window::display_msg_confirm_out_of_range_transpose(const unsigned &outOfRangeCount) {

    string message = "This operation will cause " + numToStr(outOfRangeCount, 0, false)
        + " note" + ((outOfRangeCount == 1) ? "" : "s") + " to fall out of range and be replaced with rests. Continue?";

    bool answer = false;

    //TODO temporary fix to work around keyboard_state desync bug in Allegro5
    al_uninstall_keyboard();
    al_install_keyboard();

	if (al_show_native_message_box(mainDisplay, "Transpose out of range", "", message.data(),
		nullptr, ALLEGRO_MESSAGEBOX_QUESTION|ALLEGRO_MESSAGEBOX_YES_NO) == 1) answer = true;

	return answer;
}


void Main_Window::display_engine_info() {

    if (engineInfo) return;

    al_uninstall_keyboard();
    al_install_keyboard();

    engineInfo = al_open_native_text_log("Engine Info", ALLEGRO_TEXTLOG_MONOSPACE);
    if (!engineInfo) throw (string("Failed to create native text log"));
    al_register_event_source(eventQueue, al_get_native_text_log_event_source(engineInfo));
    al_append_native_text_log(engineInfo, "%s", currentTune.configName.data());
    al_append_native_text_log(engineInfo, "%s", "\n");
    al_append_native_text_log(engineInfo, "%s", "=============================================================\n");
    al_append_native_text_log(engineInfo, "%s", "\n");
    al_append_native_text_log(engineInfo, "%s", currentTune.config.description.data());
    al_append_native_text_log(engineInfo, "%s", "\n");
    al_append_native_text_log(engineInfo, "%s", "\n");
    al_append_native_text_log(engineInfo, "%s", "General Commands\n");
    al_append_native_text_log(engineInfo, "%s", "=============================================================\n");
    al_append_native_text_log(engineInfo, "%s", "\n");
    for (auto&& it: currentTune.globalConstants) {

        al_append_native_text_log(engineInfo, "%s", it.command->mdCmdName.data());
        al_append_native_text_log(engineInfo, "%s", "\n");
        al_append_native_text_log(engineInfo, "%s", it.command->description.data());
        al_append_native_text_log(engineInfo, "%s", "\n");
        al_append_native_text_log(engineInfo, "%s", "Flags: ");
        if (it.command->mdCmdAuto) al_append_native_text_log(engineInfo, "%s", "auto_cmd ");
        if (it.command->useNoteNames) al_append_native_text_log(engineInfo, "%s", "note_cmd ");
        if (it.command->isBlkReference) al_append_native_text_log(engineInfo, "%s", "reference_cmd ");
        else if (it.command->mdCmdForceString) al_append_native_text_log(engineInfo, "%s", "text_cmd ");
        if (it.command->allowModifiers) al_append_native_text_log(engineInfo, "%s", "use_modifiers ");
        if (it.command->mdCmdForceSubstitution) al_append_native_text_log(engineInfo, "%s", "use_substitution ");
        al_append_native_text_log(engineInfo, "%s", "\n");
        if (it.command->limitRange) {
            al_append_native_text_log(engineInfo, "%s", "Range limits: ");
            al_append_native_text_log(engineInfo, "%x", it.command->lowerRangeLimit);
            al_append_native_text_log(engineInfo, "%s", " - ");
            al_append_native_text_log(engineInfo, "%x", it.command->upperRangeLimit);
            al_append_native_text_log(engineInfo, "%s", "\n");
        }
        al_append_native_text_log(engineInfo, "%s", "\n");
    }
    al_append_native_text_log(engineInfo, "%s", "\n");
    for (unsigned i = 0; i < currentTune.config.blockTypes.size(); i++) {

        al_append_native_text_log(engineInfo, "%s", currentTune.config.blockTypes[i].blockConfigID.data());
        al_append_native_text_log(engineInfo, "%s", " Commands\n");
        al_append_native_text_log(engineInfo, "%s", "=============================================================\n");
        al_append_native_text_log(engineInfo, "%s", "\n");
        for (auto&& it: currentTune.blockTypes[i].commands) {

            al_append_native_text_log(engineInfo, "%s", it->mdCmdName.data());
            al_append_native_text_log(engineInfo, "%s", "\n");
            al_append_native_text_log(engineInfo, "%s", it->description.data());
            al_append_native_text_log(engineInfo, "%s", "\n");
            al_append_native_text_log(engineInfo, "%s", "Default: ");
            if (it->defaultSubstitute) {
                al_append_native_text_log(engineInfo, "%s", "substituted from ");
                al_append_native_text_log(engineInfo, "%s", it->defaultSubstitute->mdCmdName.data());
            }
            else al_append_native_text_log(engineInfo, "%s", it->mdCmdDefaultValString.data());
            al_append_native_text_log(engineInfo, "%s", "\n");
            al_append_native_text_log(engineInfo, "%s", "Flags: ");
            if (it->mdCmdAuto) al_append_native_text_log(engineInfo, "%s", "auto_cmd ");
            if (it->useNoteNames) al_append_native_text_log(engineInfo, "%s", "note_cmd ");
            if (it->isBlkReference) al_append_native_text_log(engineInfo, "%s", "reference_cmd ");
            else if (it->mdCmdForceString) al_append_native_text_log(engineInfo, "%s", "text_cmd ");
            if (it->allowModifiers) al_append_native_text_log(engineInfo, "%s", "use_modifiers ");
            if (it->mdCmdForceSubstitution) al_append_native_text_log(engineInfo, "%s", "use_substitution ");
            al_append_native_text_log(engineInfo, "%s", "\n");
            if (it->limitRange) {
                al_append_native_text_log(engineInfo, "%s", "Range limits: ");
                al_append_native_text_log(engineInfo, "%x", it->lowerRangeLimit);
                al_append_native_text_log(engineInfo, "%s", " - ");
                al_append_native_text_log(engineInfo, "%x", it->upperRangeLimit);
                al_append_native_text_log(engineInfo, "%s", "\n");
            }
            al_append_native_text_log(engineInfo, "%s", "\n");
        }
        al_append_native_text_log(engineInfo, "%s", "\n");
    }
}
