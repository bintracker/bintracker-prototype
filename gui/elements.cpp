#include <iostream>
#include <fstream>
#include <string>
#include "gui.h"

Options_Dropdown::Options_Dropdown() : userEntry("") {

    isActive = false;
    selectedOption = 0;
}


Options_Dropdown::~Options_Dropdown() {}


void Options_Dropdown::activate(const vector<string> &_options, const string &_userEntry) {

    isActive = true;
    selectedOption = 0;
    options = _options;
    userEntry = _userEntry;
}


void Options_Dropdown::collapse() {

    if (!isActive) return;
    options.clear();
    userEntry = "";
    selectedOption = 0;
    isActive = false;
}


void Options_Dropdown::select_next() {

    selectedOption++;
    if (selectedOption >= options.size()) selectedOption = 0;
}


void Options_Dropdown::select_previous() {

    if (selectedOption == 0) selectedOption = options.size() - 1;
    else selectedOption--;
}


string Options_Dropdown::get_current_option() {

    return options[selectedOption];
}


Changelog_Entry::Changelog_Entry(Work_Tune *tune, Display_Status *status) {

    lastUsed = tune->lastUsed;
	globalConstants = tune->globalConstants;
	sequence = tune->sequence;
	blockTypes = tune->blockTypes;
	sequenceLoopPoint = tune->sequenceLoopPoint;

	currentTab = status->currentTab;
	visibleGlobalsFirst = status->visibleGlobalsFirst;
	currentGlobal = status->currentGlobal;
	focusBlock = status->focusBlock;

    visibleFirstColumns = status->visibleFirstColumns;
	visibleLastColumns = status->visibleLastColumns;
	visibleFirstRows = status->visibleFirstRows;
	currentBlocks = status->currentBlocks;
	currentCsrColumns = status->currentCsrColumns;
	currentCsrRows = status->currentCsrRows;
	visibleFirstReferenceRows = status->visibleFirstReferenceRows;
	currentReferenceRows = status->currentReferenceRows;
}

Changelog_Entry::Changelog_Entry(const Changelog_Entry &entry) {

    lastUsed = entry.lastUsed;
	globalConstants = entry.globalConstants;
	sequence = entry.sequence;
	blockTypes = entry.blockTypes;
	sequenceLoopPoint = entry.sequenceLoopPoint;

	currentTab = entry.currentTab;
	visibleGlobalsFirst = entry.visibleGlobalsFirst;
	currentGlobal = entry.currentGlobal;
	focusBlock = entry.focusBlock;

    visibleFirstColumns = entry.visibleFirstColumns;
	visibleLastColumns = entry.visibleLastColumns;
	visibleFirstRows = entry.visibleFirstRows;
	currentBlocks = entry.currentBlocks;
	currentCsrColumns = entry.currentCsrColumns;
	currentCsrRows = entry.currentCsrRows;
	visibleFirstReferenceRows = entry.visibleFirstReferenceRows;
	currentReferenceRows = entry.currentReferenceRows;
}

Changelog_Entry& Changelog_Entry::operator=(const Changelog_Entry &entry) {

    lastUsed = entry.lastUsed;
	globalConstants = entry.globalConstants;
	sequence = entry.sequence;
	blockTypes = entry.blockTypes;
	sequenceLoopPoint = entry.sequenceLoopPoint;

	currentTab = entry.currentTab;
	visibleGlobalsFirst = entry.visibleGlobalsFirst;
	currentGlobal = entry.currentGlobal;
	focusBlock = entry.focusBlock;

    visibleFirstColumns = entry.visibleFirstColumns;
	visibleLastColumns = entry.visibleLastColumns;
	visibleFirstRows = entry.visibleFirstRows;
	currentBlocks = entry.currentBlocks;
	currentCsrColumns = entry.currentCsrColumns;
	currentCsrRows = entry.currentCsrRows;
	visibleFirstReferenceRows = entry.visibleFirstReferenceRows;
	currentReferenceRows = entry.currentReferenceRows;

    return *this;
}

Changelog_Entry::~Changelog_Entry() {}


void Changelog_Entry::retrieve(Work_Tune *tune, Display_Status *status) {

    tune->lastUsed = lastUsed;
    tune->globalConstants = globalConstants;
    tune->sequence = sequence;
    tune->blockTypes = blockTypes;
    tune->sequenceLoopPoint = sequenceLoopPoint;

    status->currentTab = currentTab;
    status->visibleGlobalsFirst = visibleGlobalsFirst;
    status->currentGlobal = currentGlobal;
    status->focusBlock = focusBlock;

    status->visibleFirstColumns = visibleFirstColumns;
    status->visibleLastColumns = visibleLastColumns;
    status->visibleFirstRows = visibleFirstRows;
    status->currentBlocks = currentBlocks;
    status->currentCsrColumns = currentCsrColumns;
    status->currentCsrRows = currentCsrRows;
    status->visibleFirstReferenceRows = visibleFirstReferenceRows;
    status->currentReferenceRows = currentReferenceRows;
}


Point::Point(const float &xpos, const float &ypos): x(xpos), y(ypos) {}
Point::Point(const Point &pt): x(pt.x), y(pt.y) {}
Point& Point::operator=(const Point& pt) {

	x = pt.x;
	y = pt.y;
	return *this;
}
Point::~Point() {}



Area::Area(): topLeft(0.0f, 0.0f), bottomRight(0.0f, 0.0f) {}
Area::Area(const Area &area): topLeft(area.topLeft), bottomRight(area.bottomRight) {}

Area& Area::operator=(const Area& area) {

	if(&area == this) return *this;
	topLeft = area.topLeft;
	bottomRight = area.bottomRight;
	return *this;
}
Area::~Area() {}

void Area::set(const Point &tl, const Point &br) {

	topLeft = tl;
	bottomRight = br;
}

void Area::clear(const ALLEGRO_COLOR &bgcolor) {

	al_draw_filled_rectangle(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y, bgcolor);
}



Panel::Panel() {

	horizontalScroll = false;
	verticalScroll = false;
}

void Panel::create(const Area &total_area, const int &border_mask, const int &scrollbar_mask, const ALLEGRO_COLOR &sysColor, const Main_Window *win) {

	totalArea = total_area;
	printArea.set(Point(totalArea.topLeft.x + DEFAULT_MARGIN(), totalArea.topLeft.y + DEFAULT_MARGIN()),
		Point(totalArea.bottomRight.x - DEFAULT_MARGIN(), totalArea.bottomRight.y - DEFAULT_MARGIN()));

	switch (border_mask) {

		case DIR_TOP:

			al_draw_line(totalArea.topLeft.x, totalArea.topLeft.y, totalArea.bottomRight.x, totalArea.topLeft.y, sysColor, THICK_LINE());
			printArea.topLeft.y += THICK_LINE();
			break;

		case DIR_BOTTOM:

			al_draw_line(totalArea.topLeft.x, totalArea.bottomRight.y - THICK_LINE_OFFSET(), totalArea.bottomRight.x,
				totalArea.bottomRight.y - THICK_LINE_OFFSET(), sysColor, THICK_LINE());
			printArea.bottomRight.y -= THICK_LINE();
			break;

		case DIR_LEFT:

			al_draw_line(totalArea.topLeft.x, totalArea.topLeft.y, totalArea.topLeft.x, totalArea.bottomRight.y, sysColor, THICK_LINE());
			printArea.topLeft.x += THICK_LINE();
			break;

		case DIR_RIGHT:

			al_draw_line(totalArea.bottomRight.x - THICK_LINE_OFFSET(), totalArea.topLeft.y, totalArea.bottomRight.x - THICK_LINE_OFFSET(),
				totalArea.bottomRight.y, sysColor, THICK_LINE());
			printArea.bottomRight.x -= THICK_LINE();
			break;

		default:
			break;
	}


	if (scrollbar_mask & DIR_RIGHT) {

		verticalScroll = true;
		al_draw_line(totalArea.bottomRight.x - SCROLL_PANE_SIZE(), totalArea.topLeft.y,
			totalArea.bottomRight.x - SCROLL_PANE_SIZE(), totalArea.bottomRight.y, sysColor, THIN_LINE());
		al_draw_tinted_bitmap(win->arrowUp, sysColor, totalArea.bottomRight.x - SCROLL_PANE_SIZE(), totalArea.topLeft.y, 0);
		al_draw_tinted_bitmap(win->arrowDown, sysColor, totalArea.bottomRight.x  - SCROLL_PANE_SIZE(),
			totalArea.bottomRight.y - SCROLL_PANE_SIZE(), 0);

		printArea.bottomRight.x -= SCROLL_PANE_SIZE();
	}

	if (scrollbar_mask & DIR_BOTTOM) {

		horizontalScroll = true;

		if (verticalScroll) {

			al_draw_line(totalArea.topLeft.x, totalArea.bottomRight.y - SCROLL_BAR_SIZE(), totalArea.bottomRight.x - SCROLL_BAR_SIZE(),
				totalArea.bottomRight.y - SCROLL_BAR_SIZE(), sysColor, THIN_LINE());
			al_draw_tinted_bitmap(win->arrowLeft, sysColor, totalArea.topLeft.x, totalArea.bottomRight.y - SCROLL_PANE_SIZE(), 0);
			al_draw_tinted_bitmap(win->arrowRight, sysColor, totalArea.bottomRight.x  - 2 * SCROLL_PANE_SIZE(),
				totalArea.bottomRight.y - SCROLL_PANE_SIZE(), 0);
		}
		else {

			al_draw_line(totalArea.topLeft.x, totalArea.bottomRight.y - SCROLL_BAR_SIZE(), totalArea.bottomRight.x,
				totalArea.bottomRight.y - SCROLL_BAR_SIZE(), sysColor, THIN_LINE());
			al_draw_tinted_bitmap(win->arrowLeft, sysColor, totalArea.topLeft.x, totalArea.bottomRight.y - SCROLL_PANE_SIZE(), 0);
			al_draw_tinted_bitmap(win->arrowRight, sysColor, totalArea.bottomRight.x  - SCROLL_PANE_SIZE(),
				totalArea.bottomRight.y - SCROLL_PANE_SIZE(), 0);
		}

		printArea.bottomRight.y -= SCROLL_PANE_SIZE();
	}
}


Block_Tab::Block_Tab(const float &hpos, const string &title_): title(title_) {

	xpos = hpos;
	xsize = (title.size() * BT_CHAR_WIDTH()) + 2 * DEFAULT_MARGIN();
}

Block_Tab::Block_Tab(const Block_Tab &tab): title(tab.title) {

	xpos = tab.xpos;
	xsize = tab.xsize;
}

Block_Tab::~Block_Tab() {}



Global_Settings::Global_Settings(): kbdLang("EN"), defaultConfig("BetaPhase") {

	xres = 800;
	yres = 640;

	keyC1 = 'z';
	keyB1 = 'm';
	keyC2 = 'q';
	keyD2 = 'w';
	keyA2 = 'y';

	bgColor = al_map_rgb(0, 0, 0);
	sysColor = al_map_rgb(0, 0xff, 0);
	rowColor = al_map_rgb(0x80, 0x80, 0x80);
	rowHlColor = al_map_rgb(0xe0, 0xe0, 0xe0);
	rowActColor = al_map_rgb(0xff, 0xff, 0);
	cursorColor = al_map_rgb(0x40, 0x40, 0x40);
	selectionColor = al_map_rgb(0x00, 0x00, 0xff);
	defaultRowHighlight = 8;
	defaultBlockLength = 64;
	hexMode = true;
	audioChunkSize = 256;
	keyRepeatDelay = 3;

	ifstream DEFAULTINI("settings.ini");
	if (!DEFAULTINI.is_open()) {

		cout << "Error: Failed to load settings.ini." << endl;
		return;
	}

	string tempstr;
	vector<string> ini_lines;

	while (getline(DEFAULTINI, tempstr)) ini_lines.push_back(tempstr);

	for (auto&& it: ini_lines) {

		if (it.substr(it.find_first_of("="), string::npos).size() < 2) {
			cout << "Error: Invalid argument in settings.ini." << endl;
			break;
		}

		if (it.find("XRES=") != string::npos) {

			tempstr = it.substr(5, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid XRES argument in settings.ini." << endl;
			else xres = static_cast<int>(strToNum(tempstr));
			if (xres < 720) xres = 720;
		}
		else if (it.find("YRES=") != string::npos) {

			tempstr = it.substr(5, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid YRES argument in settings.ini." << endl;
			else yres = static_cast<int>(strToNum(tempstr));
			if (yres < 480) yres = 480;
		}
		else if (it.find("KBDLANG=") != string::npos) {

			tempstr = it.substr(8, string::npos);
			if (tempstr == "DE") {
				kbdLang = "DE";
				keyC1 = 'y';
				keyA2 = 'z';
			}
			else if (tempstr == "FR") {
				kbdLang = "FR";
				keyC1 = 'w';
				keyB1 = ',';
				keyC2 = 'a';
				keyD2 = 'z';
			}
		}
		else if (it.find("BGCOLOR=") != string::npos) {

			tempstr = it.substr(8, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid BGCOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				bgColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("SYSCOLOR=") != string::npos) {

			tempstr = it.substr(9, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid SYSCOLOR argument in settings.ini." << endl;
			long rgb = strToNum(tempstr);
			sysColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
		}
		else if (it.find("ROWCOLOR=") != string::npos) {

			tempstr = it.substr(9, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid ROWCOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				rowColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("ROWHIGHLIGHTCOLOR=") != string::npos) {

			tempstr = it.substr(18, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid ROWHIGHLIGHTCOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				rowHlColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("ROWACTIVECOLOR=") != string::npos) {

			tempstr = it.substr(15, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid ROWACTIVECOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				rowActColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("CURSORCOLOR=") != string::npos) {

			tempstr = it.substr(12, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid CURSORCOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				cursorColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("SELECTIONCOLOR=") != string::npos) {

			tempstr = it.substr(15, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid SELECTIONCOLOR argument in settings.ini." << endl;
			else {
				long rgb = strToNum(tempstr);
				selectionColor = al_map_rgb(static_cast<unsigned char>(rgb >> 16),
					static_cast<unsigned char>(rgb >> 8), static_cast<unsigned char>(rgb));
			}
		}
		else if (it.find("DEFAULTROWHIGHLIGHT=") != string::npos) {

			tempstr = it.substr(20, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid DEFAULTROWHIGHLIGHT argument in settings.ini." << endl;
			else defaultRowHighlight = static_cast<unsigned>(strToNum(tempstr));
		}
		else if (it.find("DEFAULTBLOCKLENGTH=") != string::npos) {

			tempstr = it.substr(19, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid DEFAULTBLOCKLENGTH argument in settings.ini." << endl;
			else defaultBlockLength = static_cast<unsigned>(strToNum(tempstr));
		}
		else if (it.find("DEFAULTCONFIG=") != string::npos) {

			tempstr = it.substr(14, string::npos);
			ifstream CONFIGFILE("engines/config/" + tempstr + ".cfg");
			if (CONFIGFILE.is_open()) defaultConfig = tempstr;
		}
		//DEC numberbase is unstable, disable it for now
//		else if (it.find("DEFAULTNUMBASE=dec") != string::npos) hexMode = false;
		else if (it.find("CHUNKSIZE=") != string::npos) {

			tempstr = it.substr(10, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid CHUNKSIZE argument in settings.ini." << endl;
			else audioChunkSize = static_cast<unsigned>(strToNum(tempstr));
			if ((audioChunkSize > 4096) || (audioChunkSize < 128)) {

				audioChunkSize = 256;
				cout << "Warning: Invalid CHUNKSIZE specified in settings.ini." << endl;
			}
		}
		else if (it.find("KEYREPEATDELAY=") != string::npos) {

			tempstr = it.substr(15, string::npos);
			if (!isNumber(tempstr)) cout << "Error: Invalid KEYREPEATDELAY argument in settings.ini." << endl;
			else keyRepeatDelay = static_cast<unsigned>(strToNum(tempstr));
		}
	}


	ifstream CONFIGLIST("engines/config/configlist");
	if (!CONFIGLIST.is_open()) throw (string("Could not read configlist."));

	while (getline(CONFIGLIST, tempstr)) {

		if (tempstr != "") configList.push_back(tempstr);		//TODO: validate entries, ie. check if configs are present
	}
	CONFIGLIST.close();


	buttonPanelArea.set(Point(0.0f, 0.0f), Point(xres, BUTTON_PANEL_HEIGHT()));
	referencePanelArea.set(Point(xres - REFERENCE_PANEL_WIDTH(), buttonPanelArea.bottomRight.y), Point(xres, yres - MESSAGE_PANEL_HEIGHT()));
	blockPanelArea.set(Point(0.0f, BUTTON_PANEL_HEIGHT()), Point(xres - REFERENCE_PANEL_WIDTH(), yres - MESSAGE_PANEL_HEIGHT()));
	messagePanelArea.set(Point(0.0f, yres - MESSAGE_PANEL_HEIGHT()), Point(xres, yres));

	//TODO: add THICK_LINE offsets
 	tabArea.set(Point(0.0f, BUTTON_PANEL_HEIGHT()), Point(blockPanelArea.bottomRight.x,
		BUTTON_PANEL_HEIGHT() + CHAR_HEIGHT() + 2 * DEFAULT_MARGIN()));

  	blockArea.set(Point(DEFAULT_MARGIN(), tabArea.bottomRight.y + DEFAULT_MARGIN()),
		Point(blockPanelArea.bottomRight.x - DEFAULT_MARGIN() - SCROLL_PANE_SIZE(),
		blockPanelArea.bottomRight.y - DEFAULT_MARGIN() - SCROLL_PANE_SIZE()));
  	blockHeaderArea.set(blockArea.topLeft, Point(blockArea.bottomRight.x, blockArea.topLeft.y + CHAR_HEIGHT() + DEFAULT_MARGIN()));

	blockNameArea.set(Point(blockPanelArea.topLeft.x + SMALL_BUTTON_SIZE() + (2 * DEFAULT_MARGIN()), tabArea.bottomRight.y + DEFAULT_MARGIN()),
		Point(blockPanelArea.topLeft.x + SMALL_BUTTON_SIZE() + (2 * DEFAULT_MARGIN()) + (12 * BT_CHAR_WIDTH()),
		tabArea.bottomRight.y + (2 * DEFAULT_MARGIN()) + CHAR_HEIGHT()));

	blockLengthArea.set(Point(blockPanelArea.topLeft.x + (2 * SMALL_BUTTON_SIZE()) + (19 * BT_CHAR_WIDTH()) + (5 * DEFAULT_MARGIN()),
		tabArea.bottomRight.y + 2.0f + DEFAULT_MARGIN()),
		Point(blockPanelArea.topLeft.x + (2 * SMALL_BUTTON_SIZE()) + (22 * BT_CHAR_WIDTH()) + (5 * DEFAULT_MARGIN()),
		tabArea.bottomRight.y + 2.0f + DEFAULT_MARGIN() + CHAR_HEIGHT()));

 	rowNumberArea.set(Point(blockPanelArea.topLeft.x + DEFAULT_MARGIN(), blockHeaderArea.bottomRight.y + DEFAULT_MARGIN()),
		Point(blockPanelArea.topLeft.x + ROW_COLUMN_WIDTH() + DEFAULT_MARGIN(), blockPanelArea.bottomRight.y - DEFAULT_MARGIN()));

 	columnHeaderArea.set(Point(rowNumberArea.bottomRight.x + DEFAULT_MARGIN(), blockHeaderArea.bottomRight.y + DEFAULT_MARGIN()),
		Point(blockPanelArea.bottomRight.x - DEFAULT_MARGIN() - SCROLL_PANE_SIZE(),
		blockHeaderArea.bottomRight.y + CHAR_HEIGHT() + DEFAULT_MARGIN()));

	blockDataArea.set(Point(blockPanelArea.topLeft.x + DEFAULT_MARGIN(), columnHeaderArea.bottomRight.y + DEFAULT_MARGIN()),
		Point(blockPanelArea.bottomRight.x - DEFAULT_MARGIN() - SCROLL_PANE_SIZE(),
		blockPanelArea.bottomRight.y - DEFAULT_MARGIN() - SCROLL_PANE_SIZE()));

 	referenceDataArea.set(Point(referencePanelArea.topLeft.x + DEFAULT_MARGIN(), columnHeaderArea.topLeft.y),
		Point(referencePanelArea.bottomRight.x - DEFAULT_MARGIN() - SCROLL_PANE_SIZE(),
		referencePanelArea.bottomRight.y - DEFAULT_MARGIN()));

}

Global_Settings::~Global_Settings() {}

