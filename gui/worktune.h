#ifndef WORKTUNE_H_INCLUDED
#define WORKTUNE_H_INCLUDED

#include <vector>
#include "../bintracker.h"
#include "../libmdal/mdal.h"

enum Play_Mode {PM_STOPPED, PM_ROW, PM_PATTERN, PM_SONG};

class Global_Settings;
class Display_Status;

class Datablock_Field {

public:
	mdCommand *command;
	string dataString;
	string printString;
	string arg1;
	string arg2;
	string modifier;

	Datablock_Field(mdCommand *command_ = nullptr);
	Datablock_Field(const Datablock_Field &field) = default;
	Datablock_Field& operator=(const Datablock_Field &field) = default;
	~Datablock_Field() = default;
	void set(const string &dataString_, const bool &hexMode);
	void remove_modifier();
};


class Datablock_Column {

public:
	mdCommand *command;
	float width;
	vector<Datablock_Field> columnData;

	Datablock_Column(mdCommand *command_ = nullptr);
	Datablock_Column(const Datablock_Column &col) = default;
	Datablock_Column& operator=(const Datablock_Column &col) = default;
	~Datablock_Column() = default;
};


class Datablock {

public:
	string name;
	vector<Datablock_Column> columns;

	Datablock(string title = "");
	Datablock(const Datablock &blk) = default;
	Datablock& operator=(const Datablock &blk) = default;
	~Datablock() = default;
};


class Datablock_Type {

public:
	vector<Datablock> blocks;
	vector<mdCommand*> commands;

//	Datablock_Type() = default;
//	Datablock_Type(const Datablock_Type &blkType);
//	Datablock_Type& operator=(const Datablock_Type &blkType) = default;
//	~Datablock_Type() = default;
};


class Work_Tune {

public:
	char *engineCode;
	char *engineCodeNoLoop;
	int engineSize;
	int engineSizeNoLoop;

	bool hasUnsavedChanges;
	string infilePath;
	string savefilePath;
	string configName;
	string configPath;

	mdConfig config;

	vector<string> moduleLines;
	vector<string> patternLines;
	vector<string> singleLine;

	Data_Assembly musicdataBinary;

	long loopPatchAddr;
    long orgAddress;
    long engineInitAddress;
    long engineExitAddress;
    long engineReloadAddress;

	vector<Datablock_Field> lastUsed;		//contains values set before the current line
	vector<Datablock_Field> globalConstants;
	vector<string> sequence;
	vector<Datablock_Type> blockTypes;
	unsigned sequenceLoopPoint;

	unsigned clipboardStatus;
	vector<Datablock_Column> clipboard;
	vector<string> clipboardSequence;

	vector<unsigned> freqDividers;
	const string noteNames[12] = {"c", "cis", "d", "dis", "e", "f", "fis", "g", "gis", "a", "ais", "b"};

	Work_Tune();
	~Work_Tune();
	void init(const string &infile, const Global_Settings &settings, Display_Status *status);
	void replace_reference(const string &original, const string &replacement, const string &blocktypeID);
	void generate_module_lines();
	void generate_pattern_lines(unsigned pattern);
	vector<string> generate_lines_from_pos(const unsigned &block, const unsigned &row, const unsigned &sequencePos);
	void generate_single_line(const unsigned &block, const unsigned &row);
	vector<char> reload_data(const int &playMode);
	string generate_unique_block_name();
	void update_last_used(const unsigned &block, const unsigned &row);

	unsigned get_frequency_divider(const string &noteName);
	unsigned get_note_index(const string &noteNames);
	string get_note_data_string(const unsigned &freqDivider);


private:
	unsigned uniqueSeed;
	Display_Status *status;

//	unsigned firstDividerIndex;
//	unsigned lastDividerIndex;

	void reset();
	void generate_empty_module(const unsigned &defaultBlockLength);
	bool is_block_name_unique(const string &blockName);
};



#endif // WORKTUNE_H_INCLUDED
