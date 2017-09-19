#ifndef BINTRACKER_GUI__H__
#define BINTRACKER_GUI__H__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include "../bintracker.h"
#include "../libmdal/mdal.h"
#include "../sound_emul/sound_emul.h"
#include "worktune.h"


using namespace std;

enum MenuID {

	MENU_NONE,
	MENU_FILE_NEW,
	MENU_FILE_OPEN,
	MENU_FILE_SAVE,
	MENU_FILE_SAVEAS,
	MENU_FILE_EXPORT_ASM,
	MENU_FILE_EXPORT_ASM_MUSICDATA,
	MENU_FILE_EXPORT_BIN,
	MENU_FILE_EXPORT_BIN_MUSICDATA,
	MENU_FILE_EXPORT_EMULFORMAT,
	MENU_FILE_EXPORT_WAV,
	MENU_EDIT_UNDO,
	MENU_EDIT_REDO,
	MENU_EDIT_SELECTALL,
	MENU_EDIT_COPY,
	MENU_EDIT_CUT,
	MENU_EDIT_DELETE,
	MENU_EDIT_INSERT,
	MENU_EDIT_PASTE,
	MENU_EDIT_POROUSPASTE,
	MENU_EDIT_INVERSEPOROUSPASTE,
	MENU_EDIT_INTERPOLATE,
	MENU_EDIT_LOGINTERPOLATE,
	MENU_EDIT_EXPINTERPOLATE,
	MENU_EDIT_REVERSE,
	MENU_EDIT_RANDOMIZE,
	MENU_EDIT_FILL,
	MENU_EDIT_FILLPOROUS,
	MENU_EDIT_TRANSPOSE_SEMI_UP,
	MENU_EDIT_TRANSPOSE_SEMI_DOWN,
	MENU_EDIT_TRANSPOSE_OCT_UP,
	MENU_EDIT_TRANSPOSE_OCT_DOWN,
	MENU_PLAY_STOP,
	MENU_PLAY_PLAY,
	MENU_PLAY_PLAYFROMSTART,
	MENU_PLAY_PLAYPATTERN,
	MENU_HELP_HELP,
	MENU_HELP_ENGINE,
	MENU_HELP_ABOUT,
	MENU_FILE_EXIT,
	MENU_FILE_NEW_CONFIGSEL = 1000
};

enum Directions {DIR_NONE = 0x00, DIR_LEFT = 0x01, DIR_RIGHT = 0x02, DIR_TOP = 0x04, DIR_BOTTOM = 0x08};
enum Editor_Display_Limits {DISPLAY_MAX_ROWS = 999};
enum Default_Entry_Points {SPECTRUM48_ENTRY_POINT = 0x8000};
enum Selection_Types {INACTIVE, BLOCK, SEQUENCE};
enum Interpolation_Types {LINEAR, LOGARITHMIC, EXPONENTIAL};
enum Fill_Types {REPLACE, POROUS, INVERSE_POROUS};
enum Dimensions {SMALL_BUTTON_WIDTH = 20, MENU_BUTTON_SIZE = 36};


constexpr const float THICK_LINE() {return 2.0f;}
constexpr const float THICK_LINE_OFFSET() {return 1.0f;}
constexpr const float THIN_LINE() {return 1.0f;}
constexpr const float DEFAULT_MARGIN() {return 4.0f;}
constexpr const float BUTTON_PANEL_HEIGHT() {return 40.0f;}
constexpr const float REFERENCE_PANEL_WIDTH() {return 204.0f;}
constexpr const float MESSAGE_PANEL_HEIGHT() {return 24.0f;}
constexpr const float CHAR_HEIGHT() {return 14.0f;}
constexpr const float BT_CHAR_WIDTH() {return 10.0f;}
constexpr const float BUTTON_SIZE() {return 28.0f;}
constexpr const float SMALL_BUTTON_SIZE() {return 16.0f;}
constexpr const float SCROLL_PANE_SIZE() {return 16.0f;}
constexpr const float SCROLL_BAR_SIZE() {return 15.0f;}
constexpr const float ROW_COLUMN_WIDTH() {return 34.0f;}
constexpr const float DEFAULT_FPS() {return (1.0f / 22.5f);}


class Main_Window;

class Point {

public:
	float x;
	float y;

	Point(const float &xpos = 0.0f, const float &ypos = 0.0f);
	Point(const Point &pt);
	Point& operator=(const Point &pt);
	~Point();
};


class Area {

public:
	Point topLeft;
	Point bottomRight;

	Area();
	Area(const Area &area);
	Area& operator=(const Area &area);
	~Area();
	void set(const Point &top_left, const Point &bottom_right);
	void clear(const ALLEGRO_COLOR &bgcolor);
};


class Panel {

public:
	Area totalArea;
	Area printArea;

	bool horizontalScroll;
	bool verticalScroll;

	Panel();
	void create(const Area &total_area, const int &border_mask, const int &scrollbar_mask,
        const ALLEGRO_COLOR &sysColor, const Main_Window *win);
};


class Block_Tab {

public:
	float xpos;
	float xsize;
	string title;

	Block_Tab(const float &xpos = 0.0f, const string &title_ = "");
	Block_Tab(const Block_Tab &tab);
	~Block_Tab();
};



class Global_Settings {

public:
	int xres;
	int yres;

	string kbdLang;
	char keyC1;
	char keyB1;
	char keyC2;
	char keyD2;
	char keyA2;

	ALLEGRO_COLOR bgColor;
	ALLEGRO_COLOR sysColor;
	ALLEGRO_COLOR rowColor;
	ALLEGRO_COLOR rowHlColor;
	ALLEGRO_COLOR rowActColor;
	ALLEGRO_COLOR cursorColor;
	ALLEGRO_COLOR selectionColor;

	unsigned defaultRowHighlight;
	unsigned defaultBlockLength;
	bool hexMode;
	string defaultConfig;
	unsigned audioChunkSize;
	unsigned keyRepeatDelay;

	vector<string> configList;

	Area buttonPanelArea;
	Area referencePanelArea;
	Area blockPanelArea;
	Area messagePanelArea;
	Area tabArea;
	Area blockArea;			//can probably be private, see Main_Window::print_globals()
	Area blockHeaderArea;		//can probably be private
	Area blockNameArea;
	Area blockLengthArea;
	Area rowNumberArea;
	Area columnHeaderArea;
	Area blockDataArea;
	Area referenceDataArea;

	Global_Settings();
	Global_Settings(const Global_Settings &settings) =delete;
	Global_Settings& operator=(const Global_Settings &settings) =delete;
	~Global_Settings();
};


class Display_Status {

public:
	//TODO: should Display_Status handle auto_inc?
	//TODO: make stuff private
	unsigned currentTab;
	unsigned rowHighlight;
	unsigned visibleRowsMax;
	unsigned visibleReferenceRowsMax;
	unsigned visibleGlobalsFirst;
	unsigned currentGlobal;
	unsigned currentOctave;
	unsigned autoInc;
	unsigned expandFactor;
	bool editBlockName;
	bool focusBlock;
	bool editLock;
	bool renameLock;
	unsigned keyLock;
	bool keyDown;
	bool leftMouseButtonDown;
	bool trackMouseButtons;

    float hScrollbarBlkStart;
    float hScrollbarBlkSize;
    float hScrollbarBlkStep;
    float vScrollbarBlkStart;
    float vScrollbarBlkSize;
    float vScrollbarBlkStep;
    float vScrollbarRefStart;
    float vScrollbarRefSize;
    float vScrollbarRefStep;
    int mouseDragBeginX;
    int mouseDragBeginY;
    bool mouseDragLockH;
    bool mouseDragLockV;
    bool mouseDragLockRefV;
    int menuButtonHighlighted;
    int panelButtonHighlighted;

    unsigned selectionType;
	unsigned selectionRowFirst;
	unsigned selectionRowLast;
	unsigned selectionColumnFirst;
	unsigned selectionColumnLast;

	vector<unsigned> visibleFirstColumns;
	vector<unsigned> visibleLastColumns;
	vector<unsigned> visibleFirstRows;
	vector<unsigned> currentBlocks;
	vector<unsigned> currentCsrColumns;
	vector<unsigned> currentCsrRows;
	vector<unsigned> visibleFirstReferenceRows;
	vector<unsigned> currentReferenceRows;

	Display_Status();
	~Display_Status();
	Display_Status(const Display_Status &status) = delete;
	void init(Work_Tune *currentTune, const Global_Settings &settings);
	void reset();
	Datablock* get_current_block_pointer();
	Datablock_Field* get_current_field_pointer();
	Datablock_Column* get_current_column_pointer();
	unsigned get_visible_first_column();
	unsigned get_visible_last_column();
	unsigned get_visible_first_row();
	unsigned get_current_blocktype();
	unsigned get_current_block();
	void set_current_block_from_cursor();
	unsigned get_current_cursor_column();
	void inc_current_cursor_column();
	void dec_current_cursor_column();
	void calculate_visible_first_column();
	void calculate_visible_last_column();
	unsigned get_current_cursor_row();
	void inc_current_cursor_row(unsigned amount = 1);
	void dec_current_cursor_row(unsigned amount = 1);
	void set_cursor_to_start();
	void set_cursor_to_end();

	unsigned get_current_reference_row();
	void inc_current_reference_row(unsigned amount = 1);
	void dec_current_reference_row(unsigned amount = 1);
	unsigned get_visible_first_reference_row();

	bool is_selected(const unsigned &row, const unsigned &col = 0);
	void start_selection();
	void update_selection();
	void select_all();
	void cancel_selection();

	unsigned get_reference_iterator(const string &blockID);		//TODO: temporarily made public, make private again later

private:
	Work_Tune *workTune;
	Global_Settings const* globalSettings;

	bool selectionDirDown;
	bool selectionDirRight;
};


class Changelog_Entry {

public:
	Changelog_Entry(Work_Tune *tune = nullptr, Display_Status *status = nullptr);
	Changelog_Entry(const Changelog_Entry &entry);
	Changelog_Entry& operator=(const Changelog_Entry &entry);
	~Changelog_Entry();
	void retrieve(Work_Tune *tune, Display_Status *status);

private:
    //data copied from Work_Tune
    vector<Datablock_Field> lastUsed;		//contains values set before the current line
	vector<Datablock_Field> globalConstants;
	vector<string> sequence;
	vector<Datablock_Type> blockTypes;
	unsigned sequenceLoopPoint;

    //values copied from Display_Status
	unsigned currentTab;
	unsigned visibleGlobalsFirst;
	unsigned currentGlobal;
	bool focusBlock;
    vector<unsigned> visibleFirstColumns;
	vector<unsigned> visibleLastColumns;
	vector<unsigned> visibleFirstRows;
	vector<unsigned> currentBlocks;
	vector<unsigned> currentCsrColumns;
	vector<unsigned> currentCsrRows;
	vector<unsigned> visibleFirstReferenceRows;
	vector<unsigned> currentReferenceRows;
};


class Options_Dropdown {

public:
    bool isActive;
    vector<string> options;
    string userEntry;
    unsigned selectedOption;

    Options_Dropdown();
    ~Options_Dropdown();
    void activate(const vector<string> &_options, const string &_userEntry);
    void collapse();
    void select_next();
    void select_previous();
    string chose_current_option();

private:


};


class Main_Window {

public:
	bool isUp;

	ALLEGRO_EVENT_QUEUE *eventQueue;	//TODO consider several event queues for mouse, kbd, other
	ALLEGRO_KEYBOARD_STATE kbdstate;
	ALLEGRO_MOUSE_STATE mouseState;

	ALLEGRO_BITMAP *arrowLeft;
	ALLEGRO_BITMAP *arrowRight;
	ALLEGRO_BITMAP *arrowUp;
	ALLEGRO_BITMAP *arrowDown;
	ALLEGRO_BITMAP *buttonClone;
	ALLEGRO_BITMAP *buttonExpand;
	ALLEGRO_BITMAP *buttonInsert;
	ALLEGRO_BITMAP *buttonMinus;
	ALLEGRO_BITMAP *buttonPlus;
	ALLEGRO_BITMAP *buttonRemove;
	ALLEGRO_BITMAP *buttonShrink;
	ALLEGRO_BITMAP *buttonLooppoint;


	Sound_Emul soundEmul;

	Main_Window(const string &initFile = "");
	~Main_Window();
	void handle_event(ALLEGRO_EVENT &ev);
	void handle_keyboard_input();
	void handle_key_release();
    void handle_mouse_input();

    ALLEGRO_TIMER *timer;

private:
	ALLEGRO_DISPLAY *mainDisplay;

	ALLEGRO_MENU *menu;
	ALLEGRO_MENU *configListMenu;
	ALLEGRO_MENU *exportMenu;
	ALLEGRO_MENU *fileMenu;
	ALLEGRO_MENU *playMenu;
	ALLEGRO_MENU *interpolateMenu;
	ALLEGRO_MENU *fillMenu;
	ALLEGRO_MENU *transposeMenu;
	ALLEGRO_MENU *editMenu;
	ALLEGRO_MENU *helpMenu;
	ALLEGRO_MENU *newFilePopupMenu;

	ALLEGRO_FONT *font;

	ALLEGRO_TEXTLOG *engineInfo;

	vector<ALLEGRO_BITMAP*> menuButtons;

	Global_Settings settings;
	Work_Tune currentTune;
	Display_Status status;

	Panel buttonPanel;
	Panel referencePanel;
	Panel blockPanel;
	Panel messagePanel;

	vector<Block_Tab> tabPanel;		//TODO: legacy code

	Datablock_Field *currentField;

	string userInputString;

    vector<Changelog_Entry> undoStack;
	vector<Changelog_Entry> redoStack;

	Options_Dropdown dropdown;

	void init_track_view();
	void print_base_octave();
	void print_autoinc_setting();
	void print_block_name();
	void print_block_tabs();
	void print_reference_panel();
    void print_reference_data();
	void print_block();
	void print_block_data();
	void print_globals();
	void print_stats();
	void print_column_titles();
	void print_row(const unsigned &row);
	void print_field(const unsigned &col, const unsigned &row);
	void print_global_field(const unsigned &row);
	unsigned get_reference_iterator(const string &blockID);
	void make_new_file(const string &config);
	void open_file();
	void save_file();
	void save_file_as();
	void export_asm(const bool &musicDataOnly);
	void export_bin(const bool &musicDataOnly);
	void export_wav();
	void export_zxspectrum_tap();
	void display_about_msg();
	bool display_confirm_overwrite_msg();
	void play_from_start();
	void play_from_current_pos();
	void play_pattern();
	void play_row();

	void receive_data_input(const char &data);
	string receive_string_input();
	void erase_data_input();
	void complete_data_input();
	void cancel_data_input();
    void display_options_dropdown();
	string auto_complete(const vector<string> &options);

	void add_row();
	void remove_row();
	void insert_row();
	void delete_row();
	void insert_field();
	void delete_field();
	void set_loop_point();
	void rename_block();
	void clone_block();
	void expand_block();
	void shrink_block();

    void copy_selection();
    void cut_selection();
    void delete_selection();
	void insert_selection();
	void paste_selection();
	void porous_paste_selection();
	void inverse_porous_paste_selection();
	void interpolate_selection(const unsigned &interpolationType = LINEAR);
	unsigned find_next_used_field(const unsigned &col, const unsigned &startPosition);
	void reverse_selection();
	void randomize_selection();
	void fill_data(const unsigned &fillType = REPLACE);
	void transpose(const int &amount);

	void undo();
	void redo();
	void push_changelog();

	void handle_key_up();
	void handle_key_down();
	void handle_key_left();
	void handle_key_right();
	void handle_key_pgup();
	void handle_key_pgdown();
	void handle_key_home();
	void handle_key_end();

	void select_next_block();
	void select_prev_block();

	void scroll_down(const unsigned &amount = 1);
	void scroll_up(const unsigned &amount = 1);
	void scroll_left();
	void scroll_right();
	void scroll_drag_v(const int &mouseY);
	void scroll_drag_h(const int &mouseX);

	void shutdown();

	void set_display_title();

	bool display_msg_confirm_out_of_range_transpose(const unsigned &outOfRangeCount);
	void display_engine_info();
};

#endif
