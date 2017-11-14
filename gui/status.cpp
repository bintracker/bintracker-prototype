#include <iostream>
#include "gui.h"


Display_Status::Display_Status() {

	currentTab = 1;
	rowHighlight = 8;
	visibleRowsMax = 0;
	visibleReferenceRowsMax = 0;
	visibleGlobalsFirst = 0;
	currentGlobal = 0;
	currentOctave = 4;
	autoInc = 1;
	expandFactor = 2;
	editBlockName = false;
	focusBlock = true;
	editLock = false;
	renameLock = false;
	keyLock = 0;
	keyDown = false;
	leftMouseButtonDown = false;
	trackMouseButtons = false;
	vScrollbarBlkSize = 0.0f;
	vScrollbarBlkStart = 0.0f;
	vScrollbarBlkStep = 0.0f;
	hScrollbarBlkSize = 0.0f;
	hScrollbarBlkStart = 0.0f;
	hScrollbarBlkStep = 0.0f;
	vScrollbarRefSize = 0.0f;
	vScrollbarRefStart = 0.0f;
	vScrollbarRefStep = 0.0f;
	mouseDragBeginX = -1;
	mouseDragBeginY = -1;
	mouseDragLockH = false;
	mouseDragLockV = false;
	mouseDragLockRefV = false;
	menuButtonHighlighted = -1;
	panelButtonHighlighted = -1;
	selectionType = INACTIVE;
	selectionRowFirst = 0;
	selectionRowLast = 0;
	selectionColumnFirst = 0;
	selectionColumnLast = 0;
	selectionDirDown = true;
	selectionDirRight = true;
	workTune = nullptr;
	globalSettings = nullptr;
}

Display_Status::~Display_Status() {

	//TODO ...
	globalSettings = nullptr;
	workTune = nullptr;
}

void Display_Status::init(Work_Tune *currentTune, const Global_Settings &settings) {

	workTune = currentTune;
	globalSettings = &settings;

	//TODO: if note range max < 5th octave change currentOctave to note_range_max_oct - 1

	reset();
}

void Display_Status::reset() {

	currentTab = 1;
	rowHighlight = globalSettings->defaultRowHighlight;

	visibleRowsMax = static_cast<unsigned>((globalSettings->blockDataArea.bottomRight.y
		- globalSettings->blockDataArea.topLeft.y) / CHAR_HEIGHT());
	visibleReferenceRowsMax = visibleRowsMax + 3;
	visibleGlobalsFirst = 0;

	currentGlobal = 0;
	editBlockName = false;
	focusBlock = true;
	editLock = false;
	keyLock = 0;
	keyDown = false;
	leftMouseButtonDown = false;
	trackMouseButtons = false;
	vScrollbarBlkSize = 0.0f;
	vScrollbarBlkStart = 0.0f;
	vScrollbarBlkStep = 0.0f;
	hScrollbarBlkSize = 0.0f;
	hScrollbarBlkStart = 0.0f;
	hScrollbarBlkStep = 0.0f;
	vScrollbarRefSize = 0.0f;
	vScrollbarRefStart = 0.0f;
	vScrollbarRefStep = 0.0f;
	mouseDragBeginX = -1;
	mouseDragBeginY = -1;
    mouseDragLockH = false;
    mouseDragLockV = false;
    mouseDragLockRefV = false;
    menuButtonHighlighted = -1;
	panelButtonHighlighted = -1;

	currentBlocks.clear();
	visibleFirstColumns.clear();
	visibleLastColumns.clear();
	visibleFirstRows.clear();
	currentCsrColumns.clear();
	currentCsrRows.clear();
	visibleFirstReferenceRows.clear();
	currentReferenceRows.clear();

	for (unsigned i = 0; i < workTune->blockTypes.size(); i++) {

		if (currentTab == 1) currentBlocks.push_back(get_reference_iterator(workTune->sequence[0]));
		else currentBlocks.push_back(0);

		visibleFirstColumns.push_back(0);
		visibleLastColumns.push_back(0);
		visibleFirstRows.push_back(0);
		currentCsrColumns.push_back(0);
		currentCsrRows.push_back(0);
		visibleFirstReferenceRows.push_back(0);
		currentReferenceRows.push_back(0);
	}

    selectionType = INACTIVE;
	selectionRowFirst = 0;
	selectionRowLast = 0;
	selectionColumnFirst = 0;
	selectionColumnLast = 0;
	selectionDirDown = true;
	selectionDirRight = true;
}

Datablock* Display_Status::get_current_block_pointer() {

	return &workTune->blockTypes[get_current_blocktype()].blocks[currentBlocks[get_current_blocktype()]];
}

Datablock_Field* Display_Status::get_current_field_pointer() {

	if (!focusBlock) return nullptr;

	if (currentTab == 0) return &workTune->globalConstants[currentGlobal];

	return &(get_current_block_pointer()->columns[get_current_cursor_column()].columnData[get_current_cursor_row()]);
}

Datablock_Column* Display_Status::get_current_column_pointer() {

	if (!focusBlock) return nullptr;
	return &(get_current_block_pointer()->columns[get_current_cursor_column()]);
}


unsigned Display_Status::get_visible_first_column() {

	return visibleFirstColumns[get_current_blocktype()];
}

unsigned Display_Status::get_visible_last_column() {

	return visibleLastColumns[get_current_blocktype()];
}

unsigned Display_Status::get_visible_first_row() {

	return visibleFirstRows[get_current_blocktype()];
}

unsigned Display_Status::get_current_blocktype() {

	return (currentTab == 0) ? 0 : (currentTab - 1);
}

unsigned Display_Status::get_current_block() {

	return currentBlocks[get_current_blocktype()];
}

unsigned Display_Status::get_current_block_size() {

    return get_current_block_pointer()->columns[0].columnData.size();
}


//TODO: also handle things if focusBlock = true
void Display_Status::set_current_block_from_cursor() {

	if (focusBlock) {

		Datablock_Field* currentField = get_current_field_pointer();

		if (!currentField->command->isBlkReference) return;

		unsigned bt = 0;

		for (; bt < workTune->blockTypes.size()
			&& currentField->command->referenceBlkID != workTune->config.blockTypes[bt].blockConfigID; bt++) {};

		currentTab = bt + 1;
		currentBlocks[bt] = get_reference_iterator(currentField->dataString);

		visibleFirstRows[bt] = 0;
		visibleFirstColumns[bt] = 0;
		currentCsrColumns[bt] = 0;
		currentCsrRows[bt] = 0;
	}
	else {
		unsigned bt = get_current_blocktype();

		visibleFirstRows[bt] = 0;
//		visibleFirstColumns[bt] = 0;

		if (bt == 0) currentBlocks[bt] = get_reference_iterator(workTune->sequence[get_current_reference_row()]);
		else currentBlocks[bt] = get_reference_iterator(workTune->blockTypes[bt].blocks[get_current_reference_row()].name);
//		currentCsrColumns[bt] = 0;
		currentCsrRows[bt] = 0;
		focusBlock = true;
	}

	calculate_visible_last_column();
}


unsigned Display_Status::get_current_cursor_column() {

	return currentCsrColumns[get_current_blocktype()];
}

void Display_Status::inc_current_cursor_column() {

	if (currentTab == 0) return;

	if (currentCsrColumns[get_current_blocktype()] < visibleLastColumns[get_current_blocktype()]) {

		currentCsrColumns[get_current_blocktype()]++;
	}

	else {
		if (visibleLastColumns[get_current_blocktype()] < (get_current_block_pointer()->columns.size() - 1)) {

			unsigned prevLastColumn = visibleLastColumns[get_current_blocktype()];
			visibleFirstColumns[get_current_blocktype()]++;

			calculate_visible_last_column();
			if (visibleLastColumns[get_current_blocktype()] == prevLastColumn) visibleFirstColumns[get_current_blocktype()]++;

			currentCsrColumns[get_current_blocktype()] = visibleLastColumns[get_current_blocktype()];
		}
		else {

			visibleFirstColumns[get_current_blocktype()] = 0;
			currentCsrColumns[get_current_blocktype()] = 0;		//TODO: delete old highlight if all columns visible)
		}
	}
}

void Display_Status::dec_current_cursor_column() {

	if (currentTab == 0) return;

	if (currentCsrColumns[get_current_blocktype()] > visibleFirstColumns[get_current_blocktype()]) {

		currentCsrColumns[get_current_blocktype()]--;
	}

	else {
		if (visibleFirstColumns[get_current_blocktype()] > 0) {

			visibleFirstColumns[get_current_blocktype()]--;
			currentCsrColumns[get_current_blocktype()]--;
		}

		else {

			visibleLastColumns[get_current_blocktype()] = get_current_block_pointer()->columns.size() - 1;
			calculate_visible_first_column();
			currentCsrColumns[get_current_blocktype()] = visibleLastColumns[get_current_blocktype()];
		}
	}
}

void Display_Status::calculate_visible_last_column() {

	float xpos;
	unsigned bt = get_current_blocktype();

	//TODO: a while loop might be a better idea here
	for (xpos = globalSettings->columnHeaderArea.topLeft.x, visibleLastColumns[bt] = visibleFirstColumns[bt];
		(visibleLastColumns[bt] < workTune->blockTypes[bt].commands.size()) && ((xpos + DEFAULT_MARGIN()
		+ get_current_block_pointer()->columns[visibleLastColumns[bt]].width)
		<= globalSettings->blockDataArea.bottomRight.x);
		xpos += (get_current_block_pointer()->columns[visibleLastColumns[bt]].width + BT_CHAR_WIDTH()),
		visibleLastColumns[bt]++) {}

	visibleLastColumns[bt]--;
}

void Display_Status::calculate_visible_first_column() {

	float xpos;
	unsigned bt = get_current_blocktype();

	//TODO: a while loop might be a better idea here
	for (visibleFirstColumns[bt] = visibleLastColumns[bt],
		xpos = globalSettings->blockDataArea.bottomRight.x - (DEFAULT_MARGIN()
		+ get_current_block_pointer()->columns[visibleFirstColumns[bt]].width);
//		(visibleFirstColumn >= 0) && (xpos >= settings.columnHeaderArea.topLeft.x);
		xpos >= globalSettings->columnHeaderArea.topLeft.x;	//TODO this might potentially cause trouble, but "visibleFirstColumn >=" is always true
		xpos -= (get_current_block_pointer()->columns[visibleFirstColumns[bt]].width + BT_CHAR_WIDTH()),
			visibleFirstColumns[bt]--) {}

	visibleFirstColumns[bt]++;
}

unsigned Display_Status::get_current_cursor_row() {

	return currentCsrRows[get_current_blocktype()];
}

void Display_Status::inc_current_cursor_row(unsigned amount) {

	if (currentTab == 0) {
		//TODO handle cases when not all globals fit on screen
		currentGlobal = (currentGlobal + 1 < workTune->globalConstants.size()) ? (currentGlobal + 1) : 0;
		return;
	}

	unsigned blockLength = get_current_block_pointer()->columns[0].columnData.size();

	if (visibleRowsMax < blockLength) {

		if (currentCsrRows[get_current_blocktype()] < (visibleFirstRows[get_current_blocktype()] + visibleRowsMax - amount)) {

			currentCsrRows[get_current_blocktype()] += amount;
		}

		else if (currentCsrRows[get_current_blocktype()] < (blockLength - amount)) {

			visibleFirstRows[get_current_blocktype()] += amount;
			currentCsrRows[get_current_blocktype()] += amount;
		}

		else {
//			visibleFirstRows[get_current_blocktype()]
//				= amount - (blockLength - currentCsrRows[get_current_blocktype()]);
			currentCsrRows[get_current_blocktype()]
				= amount - (blockLength - currentCsrRows[get_current_blocktype()]);
            visibleFirstRows[get_current_blocktype()] = 0;
            if (visibleRowsMax < currentCsrRows[get_current_blocktype()])
                visibleFirstRows[get_current_blocktype()] = (currentCsrRows[get_current_blocktype()] - visibleRowsMax) / 2;
		}
	}

	else {
		if (currentCsrRows[get_current_blocktype()] + amount < blockLength) currentCsrRows[get_current_blocktype()] += amount;
		else {
			if (amount < blockLength) currentCsrRows[get_current_blocktype()]
				= amount - (blockLength - currentCsrRows[get_current_blocktype()]);
			else {
				currentCsrRows[get_current_blocktype()] = amount + currentCsrRows[get_current_blocktype()];
				while (currentCsrRows[get_current_blocktype()] >= blockLength) currentCsrRows[get_current_blocktype()] -= blockLength;
			}
		}
	}
}

void Display_Status::dec_current_cursor_row(unsigned amount) {

	if (currentTab == 0) {
		//TODO handle cases when not all globals fit on screen
		currentGlobal = (currentGlobal == 0) ? (workTune->globalConstants.size() - 1) : (currentGlobal - 1);
		return;
	}

	if (amount > visibleRowsMax) return;

	unsigned blockLength = get_current_block_pointer()->columns[0].columnData.size();
	unsigned bt = get_current_blocktype();

	if (visibleRowsMax < blockLength) {

		if (currentCsrRows[bt] >= visibleFirstRows[bt] + amount) currentCsrRows[bt] -= amount;
		else if (currentCsrRows[bt] >= amount) {
			currentCsrRows[bt] -= amount;
			visibleFirstRows[bt] = currentCsrRows[bt];
		}
		else {
			currentCsrRows[bt] = blockLength - (amount - currentCsrRows[bt]);
			visibleFirstRows[bt] = blockLength - visibleRowsMax;
		}
	}

	else {
		currentCsrRows[bt] = (currentCsrRows[bt] >= amount)
            ? (currentCsrRows[bt] - amount) : (blockLength - (amount - currentCsrRows[bt]));
	}
}

unsigned Display_Status::get_current_reference_row() {

	return currentReferenceRows[get_current_blocktype()];
}

void Display_Status::inc_current_reference_row(unsigned amount) {

	if (currentTab == 1) {

		if (visibleReferenceRowsMax < workTune->sequence.size()) {

            if (amount < workTune->sequence.size()) {

                if (get_current_reference_row() + amount < (get_visible_first_reference_row() + visibleReferenceRowsMax))
                    currentReferenceRows[get_current_blocktype()] += amount;

                else if (get_current_reference_row() + amount < (workTune->sequence.size())) {

                    currentReferenceRows[get_current_blocktype()] += amount;
                    visibleFirstReferenceRows[get_current_blocktype()] = (currentReferenceRows[0] >= visibleReferenceRowsMax)
                    ? (currentReferenceRows[0] + 1 - visibleReferenceRowsMax) : 0;
                }

                else {
                    visibleFirstReferenceRows[get_current_blocktype()] = 0;
                    currentReferenceRows[get_current_blocktype()]
                        = amount - (workTune->sequence.size() - get_current_reference_row());
                }
			}
		}

		else {

            if (amount < workTune->sequence.size()) {
                if (get_current_reference_row() + amount < workTune->sequence.size())
                    currentReferenceRows[get_current_blocktype()] += amount;
                else currentReferenceRows[get_current_blocktype()]
                    = amount - (workTune->sequence.size() - get_current_reference_row());
            }
		}
	}

	else {

		if (visibleReferenceRowsMax < workTune->blockTypes[get_current_blocktype()].blocks.size()) {

            if (amount < workTune->blockTypes[get_current_blocktype()].blocks.size()) {

                if (get_current_reference_row() + amount < (get_visible_first_reference_row() + visibleReferenceRowsMax))
                    currentReferenceRows[get_current_blocktype()] += amount;

                else if (get_current_reference_row() + amount < (workTune->blockTypes[get_current_blocktype()].blocks.size())) {

                    currentReferenceRows[get_current_blocktype()] += amount;
                    visibleFirstReferenceRows[get_current_blocktype()]
                        = (currentReferenceRows[get_current_blocktype()] >= visibleReferenceRowsMax)
                        ? (currentReferenceRows[get_current_blocktype()] + 1 - visibleReferenceRowsMax) : 0;
                }

                else {
                    visibleFirstReferenceRows[get_current_blocktype()] = 0;
                    currentReferenceRows[get_current_blocktype()]
                        = amount - (workTune->blockTypes[get_current_blocktype()].blocks.size() - get_current_reference_row());
                }
			}
		}

		else {

            if (amount < workTune->blockTypes[get_current_blocktype()].blocks.size()) {
                if (get_current_reference_row() + amount < workTune->blockTypes[get_current_blocktype()].blocks.size())
                    currentReferenceRows[get_current_blocktype()] += amount;
                else currentReferenceRows[get_current_blocktype()]
                    = amount - (workTune->blockTypes[get_current_blocktype()].blocks.size() - get_current_reference_row());
            }
		}

		cout << "visibleFirst: " << get_visible_first_reference_row() << ", current: " << get_current_reference_row() << endl;
	}
}

void Display_Status::dec_current_reference_row(unsigned amount) {

	if (currentTab == 1) {

        if (amount <= workTune->sequence.size()) {

            if (visibleReferenceRowsMax <= workTune->sequence.size()) {

                if (get_current_reference_row() >= get_visible_first_reference_row() + amount)
                    currentReferenceRows[get_current_blocktype()] -= amount;

                else if (get_current_reference_row() >= amount) {

                    currentReferenceRows[get_current_blocktype()] -= amount;
                    visibleFirstReferenceRows[get_current_blocktype()] = currentReferenceRows[get_current_blocktype()];
                }

                else {

                    currentReferenceRows[get_current_blocktype()]
                        = workTune->sequence.size() - (amount - get_current_reference_row());
                    visibleFirstReferenceRows[get_current_blocktype()] = workTune->sequence.size() - visibleReferenceRowsMax;
                }
            }

            else {

                if (get_current_reference_row() >= amount) currentReferenceRows[get_current_blocktype()] -= amount;
                else currentReferenceRows[get_current_blocktype()]
                    = workTune->sequence.size() - (amount - get_current_reference_row());
            }
		}
	}

	else {

        if (amount < workTune->blockTypes[get_current_blocktype()].blocks.size()) {

            if (visibleReferenceRowsMax <= workTune->blockTypes[get_current_blocktype()].blocks.size()) {

                if (get_current_reference_row() >= get_visible_first_reference_row() + amount)
                    currentReferenceRows[get_current_blocktype()] -= amount;

                else if (get_current_reference_row() >= amount) {

                    currentReferenceRows[get_current_blocktype()] -= amount;
                    visibleFirstReferenceRows[get_current_blocktype()] = currentReferenceRows[get_current_blocktype()];
                }

                else {

                    currentReferenceRows[get_current_blocktype()]
                        = workTune->blockTypes[get_current_blocktype()].blocks.size() - (amount - get_current_reference_row());
                    visibleFirstReferenceRows[get_current_blocktype()]
                        = workTune->blockTypes[get_current_blocktype()].blocks.size() - visibleReferenceRowsMax;
                }
            }

            else {

                if (get_current_reference_row() >= amount) currentReferenceRows[get_current_blocktype()] -= amount;
                else currentReferenceRows[get_current_blocktype()]
                    = workTune->blockTypes[get_current_blocktype()].blocks.size() - (amount - get_current_reference_row());
            }
		}
	}

//	else {
//
//		if (visibleReferenceRowsMax <= workTune->blockTypes[get_current_blocktype()].blocks.size()) {
//
//			if (get_current_reference_row() > get_visible_first_reference_row())
//				currentReferenceRows[get_current_blocktype()]--;
//
//			else if (get_current_reference_row() > 0) {
//
//				visibleFirstReferenceRows[get_current_blocktype()]--;
//				currentReferenceRows[get_current_blocktype()]--;
//			}
//
//			else {
//
//				currentReferenceRows[get_current_blocktype()]
//					= workTune->blockTypes[get_current_blocktype()].blocks.size() - 1;
//				visibleFirstReferenceRows[get_current_blocktype()]
//					= workTune->blockTypes[get_current_blocktype()].blocks.size() - visibleReferenceRowsMax;
//			}
//		}
//
//		else {
//
//			(get_current_reference_row() == 0) ? (currentReferenceRows[get_current_blocktype()]
//				= workTune->blockTypes[get_current_blocktype()].blocks.size() - 1)
//				: (currentReferenceRows[get_current_blocktype()]--);
//		}
//	}
}

unsigned Display_Status::get_visible_first_reference_row() {

	return visibleFirstReferenceRows[get_current_blocktype()];
}


void Display_Status::set_cursor_to_start() {

    if (focusBlock) {
        if (currentTab == 0) currentGlobal = 0;
        else {
            visibleFirstRows[get_current_blocktype()] = 0;
            currentCsrRows[get_current_blocktype()] = 0;
        }
    }
    else {
        currentReferenceRows[get_current_blocktype()] = 0;
        visibleFirstReferenceRows[get_current_blocktype()] = 0;
    }
}


void Display_Status::set_cursor_to_end() {

    if (focusBlock) {
        if (currentTab == 0) currentGlobal = workTune->globalConstants.size() - 1;
        else {
            currentCsrRows[get_current_blocktype()] = get_current_block_pointer()->columns[0].columnData.size() - 1;
            visibleFirstRows[get_current_blocktype()]
                = (currentCsrRows[get_current_blocktype()] >= visibleRowsMax)
                ? (currentCsrRows[get_current_blocktype()] + 1 - visibleRowsMax) : 0;
        }
    }
    else {
        if (get_current_blocktype() == 0) currentReferenceRows[0] = workTune->sequence.size() - 1;
        else currentReferenceRows[get_current_blocktype()]
            = workTune->blockTypes[get_current_blocktype()].blocks.size() - 1;
        visibleFirstReferenceRows[get_current_blocktype()] = (currentReferenceRows[0] > visibleReferenceRowsMax)
                ? (currentReferenceRows[0] + 1 - visibleReferenceRowsMax) : 0;
    }
}


bool Display_Status::is_selected(const unsigned &row, const unsigned &col) {

    if (selectionType == INACTIVE) return false;
    if (focusBlock) {

        if (selectionType == SEQUENCE) return false;
        if (row >= selectionRowFirst && row <= selectionRowLast && col >= selectionColumnFirst && col <= selectionColumnLast)
            return true;
    }
    else  {

        if (selectionType == BLOCK) return false;
        if (row >= selectionRowFirst && row <= selectionRowLast) return true;
    }
    return false;
}


void Display_Status::start_selection() {

    if (selectionType != INACTIVE || currentTab == 0) return;
    if (focusBlock) {

        selectionType = BLOCK;
        selectionRowFirst = get_current_cursor_row();
        selectionColumnFirst = get_current_cursor_column();
        selectionColumnLast = selectionColumnFirst;
        selectionDirRight = true;
    }
    else {
        if (get_current_blocktype() != 0) return;
        selectionType = SEQUENCE;
        selectionRowFirst = get_current_reference_row();
    }
    selectionRowLast = selectionRowFirst;
    selectionDirDown = true;
}

void Display_Status::select_all() {

    if (currentTab == 0) return;

    selectionRowFirst = 0;
    selectionDirDown = true;

    if (focusBlock) {

        selectionType = BLOCK;
        selectionDirRight = true;
        selectionColumnFirst = 0;
        selectionColumnLast = get_current_block_pointer()->columns.size() - 1;
        selectionRowLast = get_current_block_pointer()->columns[0].columnData.size() - 1;
    }
    else {
        if (get_current_blocktype() != 0) return;
        selectionType = SEQUENCE;
        selectionRowLast = workTune->sequence.size() - 1;
    }
}


void Display_Status::update_selection() {

    if (selectionType == INACTIVE) return;
    if (selectionType == BLOCK) {

        unsigned nextRow = get_current_cursor_row();
        unsigned nextCol = get_current_cursor_column();

        if (selectionDirDown) {
            if (nextRow >= selectionRowFirst) selectionRowLast = nextRow;
            else {
                selectionDirDown = false;
                selectionRowLast = selectionRowFirst;
                selectionRowFirst = nextRow;
            }
        }
        else {
            if (nextRow <= selectionRowLast) selectionRowFirst = nextRow;
            else {
                selectionDirDown = true;
                selectionRowFirst = selectionRowLast;
                selectionRowLast = nextRow;
            }
        }
        if (selectionDirRight) {
            if (nextCol >= selectionColumnFirst) selectionColumnLast = nextCol;
            else {
                selectionDirRight = false;
                selectionColumnLast = selectionColumnFirst;
                selectionColumnFirst = nextCol;
            }
        }
        else {
            if (nextCol <= selectionColumnLast) selectionColumnFirst = nextCol;
            else {
                selectionDirRight = true;
                selectionColumnFirst = selectionColumnLast;
                selectionColumnLast = nextCol;
            }
        }
    }
    else {
        unsigned nextRow = get_current_reference_row();

        if (selectionDirDown) {
            if (nextRow >= selectionRowFirst) selectionRowLast = nextRow;
            else {
                selectionDirDown = false;
                selectionRowLast = selectionRowFirst;
                selectionRowFirst = nextRow;
            }
        }
        else {
            if (nextRow <= selectionRowLast) selectionRowFirst = nextRow;
            else {
                selectionDirDown = true;
                selectionRowFirst = selectionRowLast;
                selectionRowLast = nextRow;
            }
        }
    }
}


void Display_Status::cancel_selection() {

    selectionType = INACTIVE;
    selectionColumnFirst = 0;
    selectionColumnLast = 0;
    selectionRowFirst = 0;
    selectionRowLast = 0;
}

unsigned Display_Status::get_reference_iterator(const string &blockID) {

	unsigned refID = 0;

	for (unsigned i = 0; i < workTune->blockTypes[get_current_blocktype()].blocks.size(); i++) {

		if (workTune->blockTypes[get_current_blocktype()].blocks[i].name == blockID) {

			refID = i;
			break;
		}
	}

	return refID;
}
