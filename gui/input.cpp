#include <sstream>
#include <iostream>
#include "gui.h"


void Main_Window::handle_key_down() {

    if (status.editLock) cancel_data_input();

    if (status.focusBlock) {

        status.inc_current_cursor_row();
        status.update_selection();
        if (status.currentTab == 0) print_globals();
        else {
            print_block_data();
            if (status.get_current_blocktype() == 0)
                currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        }
    }
    else {

        status.inc_current_reference_row();
        status.update_selection();
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        print_reference_data();
        print_block_data();
    }
}


void Main_Window::handle_key_up() {

    if (status.editLock) cancel_data_input();

    if (status.focusBlock) {

        status.dec_current_cursor_row();
        status.update_selection();
        if (status.currentTab == 0) print_globals();
        else {
            print_block_data();
            if (status.get_current_blocktype() == 0)
                currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        }
    }
    else {

        status.dec_current_reference_row();
        status.update_selection();
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        print_reference_data();
        print_block_data();
    }
}


void Main_Window::handle_key_left() {

    if (!status.focusBlock || status.currentTab == 0) return;
    if (status.editLock) cancel_data_input();

    status.dec_current_cursor_column();
    status.update_selection();
    print_column_titles();
    print_block_data();
}


void Main_Window::handle_key_right() {

    if (!status.focusBlock || status.currentTab == 0) return;
    if (status.editLock) cancel_data_input();

    status.inc_current_cursor_column();
    status.update_selection();
    print_column_titles();
    print_block_data();
}


void Main_Window::handle_key_pgdown() {

    if (status.currentTab == 0) return;
    if (status.editLock) cancel_data_input();

    if (status.focusBlock) {
        status.inc_current_cursor_row(0x10);
        status.update_selection();
        if (status.get_current_blocktype() == 0)
            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
    }
    else {
        status.inc_current_reference_row(0x10);
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        status.update_selection();
        print_reference_data();
    }

    print_block_data();
}


void Main_Window::handle_key_pgup() {

    if (status.currentTab == 0) return;
    if (status.editLock) cancel_data_input();

    if (status.focusBlock) {
        status.dec_current_cursor_row(0x10);
        status.update_selection();

        if (status.get_current_blocktype() == 0)
            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
    }
    else {
        status.dec_current_reference_row(0x10);
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        status.update_selection();
        print_reference_data();
    }

    print_block_data();
}


void Main_Window::handle_key_home() {

    if (status.editLock) cancel_data_input();
    status.set_cursor_to_start();
    if (status.focusBlock) {
        if (status.get_current_blocktype() == 0)
            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        status.update_selection();
    }
    else {
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        status.update_selection();
        print_reference_data();
    }
    print_block_data();
}


void Main_Window::handle_key_end() {

    if (status.editLock) cancel_data_input();
    status.set_cursor_to_end();
    if (status.focusBlock) {
        if (status.get_current_blocktype() == 0)
            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        status.update_selection();
    }
    else {
        status.set_current_block_from_cursor();
        status.focusBlock = false;
        status.update_selection();
        print_reference_data();
    }
    print_block_data();
}


void Main_Window::scroll_down(const unsigned &amount) {

    if (status.focusBlock) {

        if (status.currentTab == 0) return;

        if (status.get_current_column_pointer()->columnData.size() <= status.visibleRowsMax
            || status.get_current_column_pointer()->columnData.size() - status.get_visible_first_row()
            <= status.visibleRowsMax)
            return;

        int lines = status.get_current_block_pointer()->columns[0].columnData.size()
            - (status.visibleRowsMax + status.visibleFirstRows[status.get_current_blocktype()]);

        if (lines > static_cast<int>(amount)) lines = amount;

        status.visibleFirstRows[status.get_current_blocktype()] += lines;
        if (status.get_current_cursor_row() < status.get_visible_first_row()) {

            cancel_data_input();
            status.inc_current_cursor_row(lines);
            if (status.get_current_blocktype() == 0)
                currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        }
    }
    else {

        unsigned lines = 0;

        if (status.get_current_blocktype() == 0) {

            if (currentTune.sequence.size() <= status.visibleReferenceRowsMax
                || currentTune.sequence.size() - status.get_visible_first_reference_row() <= status.visibleReferenceRowsMax)
            return;

            lines = currentTune.sequence.size() - status.get_visible_first_reference_row() - status.visibleReferenceRowsMax;
            if (lines > amount) lines = amount;

            status.visibleFirstReferenceRows[0] += lines;
        }
        else {

            if (currentTune.blockTypes[status.get_current_blocktype()].blocks.size() <= status.visibleReferenceRowsMax
                || currentTune.blockTypes[status.get_current_blocktype()].blocks.size()
                - status.get_visible_first_reference_row() <= status.visibleReferenceRowsMax)
                return;

            lines = currentTune.blockTypes[status.get_current_blocktype()].blocks.size()
                - status.get_visible_first_reference_row() - status.visibleReferenceRowsMax;
            if (lines > amount) lines = amount;

            status.visibleFirstReferenceRows[status.get_current_blocktype()] += lines;
        }

        if (status.get_current_reference_row() < status.get_visible_first_reference_row()) {

            cancel_data_input();
//            status.inc_current_reference_row(lines);
            status.currentReferenceRows[status.get_current_blocktype()] = status.get_visible_first_reference_row();
            status.set_current_block_from_cursor();
            status.focusBlock = false;
        }
    }

    print_block_data();
    print_reference_data();
}


void Main_Window::scroll_up(const unsigned &amount) {

    if (status.focusBlock) {

        if (status.currentTab == 0 || status.get_visible_first_row() == 0) return;

        unsigned lines = status.get_visible_first_row();
        if (lines > amount) lines = amount;

        status.visibleFirstRows[status.get_current_blocktype()] -= lines;
        if (status.get_current_cursor_row() >= status.get_visible_first_row() + status.visibleRowsMax) {

            cancel_data_input();
            status.dec_current_cursor_row(lines);
            if (status.get_current_blocktype() == 0)
                currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
        }
    }
    else {
        if (status.get_visible_first_reference_row() == 0) return;

        unsigned lines = status.get_visible_first_reference_row();
        if (lines > amount) lines = amount;

        status.visibleFirstReferenceRows[status.get_current_blocktype()] -= lines;

        if (status.get_current_reference_row() >= status.get_visible_first_reference_row() + status.visibleReferenceRowsMax) {

            cancel_data_input();
            status.dec_current_reference_row(lines);
//            status.currentReferenceRows[status.get_current_blocktype()]
//                = status.get_visible_first_reference_row() + status.visibleReferenceRowsMax;
            status.set_current_block_from_cursor();
            status.focusBlock = false;
        }
    }

    print_block_data();
    print_reference_data();
}



void Main_Window::scroll_left() {

    if (!status.focusBlock || status.get_visible_first_column() == 0) return;

    status.visibleFirstColumns[status.get_current_blocktype()]--;
    status.calculate_visible_last_column();
    if (status.get_current_cursor_column() > status.get_visible_last_column())
        status.currentCsrColumns[status.get_current_blocktype()] = status.get_visible_last_column();

    print_block_tabs();
    print_reference_data();
}


void Main_Window::scroll_right() {

    if (!status.focusBlock || status.get_visible_last_column() + 1 == status.get_current_block_pointer()->columns.size())
        return;

    unsigned last = status.get_visible_last_column();
    status.visibleFirstColumns[status.get_current_blocktype()]++;
    //incrementing first column once may not be sufficient, must verify that last column has actually increased
    status.calculate_visible_last_column();
    if (last == status.get_visible_last_column()) status.visibleFirstColumns[status.get_current_blocktype()]++;

    if (status.get_current_cursor_column() < status.get_visible_first_column())
        status.currentCsrColumns[status.get_current_blocktype()] = status.get_visible_first_column();

    print_block_tabs();
    print_reference_data();
}


void Main_Window::scroll_drag_v(const int &mouseY) {

    if (status.focusBlock) {

        if (status.vScrollbarBlkSize < 1.0f) return;

        //determine if click is on scrollbar
        if (mouseY >= static_cast<int>(status.vScrollbarBlkStart)
            && mouseY <= static_cast<int>(status.vScrollbarBlkStart + status.vScrollbarBlkSize)) {

                if (static_cast<float>(mouseY) <= status.mouseDragBeginY - status.vScrollbarBlkStep
                    && status.get_visible_first_row() > 0) {

                    unsigned lines = static_cast<unsigned>(0.5f + (status.mouseDragBeginY - static_cast<float>(mouseY))
                        / status.vScrollbarBlkStep);
                    status.mouseDragBeginY = static_cast<float>(mouseY);
                    scroll_up(lines);
                }
                else if (static_cast<float>(mouseY) >= status.mouseDragBeginY + status.vScrollbarBlkStep
                    && status.get_visible_first_row() + status.visibleRowsMax
                    < status.get_current_block_pointer()->columns[0].columnData.size()) {

                    unsigned lines = static_cast<unsigned>(0.5f + (static_cast<float>(mouseY) - status.mouseDragBeginY)
                        / status.vScrollbarBlkStep);
                    status.mouseDragBeginY = static_cast<float>(mouseY);
                    scroll_down(lines);
                }
                //smooth scroll
                else if ((static_cast<float>(mouseY) < status.mouseDragBeginY && status.get_visible_first_row() > 0)
                    || (static_cast<float>(mouseY) > status.mouseDragBeginY
                    && status.get_visible_first_row() + status.visibleRowsMax + 1
                    < status.get_current_block_pointer()->columns[0].columnData.size())) {

                    float yOffset = status.vScrollbarBlkStart + (static_cast<float>(mouseY) - status.mouseDragBeginY);

                    al_draw_filled_rectangle(settings.blockPanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
                        settings.blockPanelArea.topLeft.y + SMALL_BUTTON_SIZE(),
                        settings.blockPanelArea.bottomRight.x - THIN_LINE(),
                        settings.blockPanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);

                    al_draw_filled_rectangle(settings.blockPanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
                        yOffset, settings.blockPanelArea.bottomRight.x - THIN_LINE(),
                        yOffset + status.vScrollbarBlkSize, settings.sysColor);
                }
        }
        else if (mouseY < static_cast<int>(status.vScrollbarBlkStart)) {
            scroll_up(4);
        }
        else {
            scroll_down(4);
        }
    }
    else {

        if (status.vScrollbarRefSize < 1.0f) return;

        if (mouseY >= static_cast<int>(status.vScrollbarRefStart)
            && mouseY <= static_cast<int>(status.vScrollbarRefStart + status.vScrollbarRefSize)) {

            if (static_cast<float>(mouseY) <= status.mouseDragBeginY - status.vScrollbarRefStep
                && status.get_visible_first_reference_row() > 0) {

                unsigned lines = static_cast<unsigned>(0.5f + (status.mouseDragBeginY - static_cast<float>(mouseY))
                    / status.vScrollbarRefStep);
                status.mouseDragBeginY = static_cast<float>(mouseY);
                scroll_up(lines);
            }
            else if (static_cast<float>(mouseY) >= status.mouseDragBeginY + status.vScrollbarRefStep
                && status.vScrollbarRefSize + status.vScrollbarRefStart
                < referencePanel.totalArea.bottomRight.y - SMALL_BUTTON_SIZE()) {

                unsigned lines = static_cast<unsigned>(0.5f + (static_cast<float>(mouseY) - status.mouseDragBeginY)
                    / status.vScrollbarRefStep);
                status.mouseDragBeginY = static_cast<float>(mouseY);
                scroll_down(lines);
            }
            //smooth scroll
            else if ((static_cast<float>(mouseY) < status.mouseDragBeginY && status.get_visible_first_reference_row() > 0)
                || (static_cast<float>(mouseY) > status.mouseDragBeginY
                && status.vScrollbarRefSize + status.vScrollbarRefStart
                < referencePanel.totalArea.bottomRight.y - SMALL_BUTTON_SIZE())) {

                float yOffset = status.vScrollbarRefStart + (static_cast<float>(mouseY) - status.mouseDragBeginY);

                al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
                    settings.referencePanelArea.topLeft.y + SMALL_BUTTON_SIZE(), settings.referencePanelArea.bottomRight.x,
                    settings.referencePanelArea.bottomRight.y - SMALL_BUTTON_SIZE(), settings.bgColor);

                al_draw_filled_rectangle(settings.referencePanelArea.bottomRight.x - SCROLL_BAR_SIZE() - THIN_LINE(),
                    yOffset, settings.referencePanelArea.bottomRight.x,
                    yOffset + status.vScrollbarRefSize, settings.sysColor);
            }
        }
        else if (mouseY < static_cast<int>(status.vScrollbarRefStart)) {
            scroll_up(4);
        }
        else {
            scroll_down(4);
        }
    }
}


void Main_Window::scroll_drag_h(const int &mouseX) {

    if (!status.focusBlock || status.hScrollbarBlkSize < 1.0f) return;

    if (mouseX >= static_cast<int>(status.hScrollbarBlkStart)
        && mouseX <= static_cast<int>(status.hScrollbarBlkStart + status.hScrollbarBlkSize)) {

        //calculate threshold (needs two different thresholds for left|right)
        //threshold = next_col_width / total width * scrollpane_width
        float hTotalSize = 0;
        for (auto&& it: status.get_current_block_pointer()->columns) hTotalSize += (it.width + BT_CHAR_WIDTH());
        float paneSize = settings.blockPanelArea.bottomRight.x - 3 * SMALL_BUTTON_SIZE();
        float thresholdLeft;
        if (status.get_visible_first_column() > 0) {
            if (status.get_visible_first_column() > 1
                && status.get_current_block_pointer()->columns[status.get_visible_last_column()].width
                > status.get_current_block_pointer()->columns[status.get_visible_first_column() - 1].width)
                thresholdLeft = (status.get_current_block_pointer()->columns[status.get_visible_first_column() - 1].width
                     * 2.0f / hTotalSize) * paneSize;
            else thresholdLeft = status.get_current_block_pointer()->columns[status.get_visible_first_column() - 1].width
                / hTotalSize * paneSize;
        }
        else thresholdLeft = settings.xres;

        float thresholdRight;
        if (status.get_visible_last_column() + 1 < status.get_current_block_pointer()->columns.size()) {
            if (status.get_visible_last_column() + 2 < status.get_current_block_pointer()->columns.size()
                && status.get_current_block_pointer()->columns[status.get_visible_first_column()].width
                > status.get_current_block_pointer()->columns[status.get_visible_last_column()].width)
                thresholdRight = (status.get_current_block_pointer()->columns[status.get_visible_last_column() + 1].width
                * 2.0f / hTotalSize) * paneSize;
            else thresholdRight = status.get_current_block_pointer()->columns[status.get_visible_last_column() + 1].width
                / hTotalSize * paneSize;
        }
        else thresholdRight = settings.xres;

        //drag left
        if (status.get_visible_first_column() > 0 && static_cast<float>(mouseX) <= status.mouseDragBeginX - thresholdLeft) {

            status.mouseDragBeginX = static_cast<float>(mouseX);
            scroll_left();
        }
        //drag right
        else if (status.get_visible_last_column() + 1 < status.get_current_block_pointer()->columns.size()
            && static_cast<float>(mouseX) >= status.mouseDragBeginX + thresholdRight) {

            status.mouseDragBeginX = static_cast<float>(mouseX);
            scroll_right();
        }

        else if ((status.get_visible_first_column() > 0 && static_cast<float>(mouseX) < status.mouseDragBeginX)
            || (status.get_visible_last_column() + 1 < status.get_current_block_pointer()->columns.size()
            && static_cast<float>(mouseX) > status.mouseDragBeginX)) {

            al_draw_filled_rectangle(SMALL_BUTTON_SIZE(), settings.blockPanelArea.bottomRight.y - SCROLL_BAR_SIZE(),
                settings.blockPanelArea.bottomRight.x - (2 * SMALL_BUTTON_SIZE()),
                settings.blockPanelArea.bottomRight.y - THIN_LINE(), settings.bgColor);

            float xOffset = static_cast<float>(mouseX) - status.mouseDragBeginX;

            al_draw_filled_rectangle(status.hScrollbarBlkStart + xOffset,
                settings.blockPanelArea.bottomRight.y - SCROLL_BAR_SIZE(),
                status.hScrollbarBlkStart + xOffset + status.hScrollbarBlkSize,
                settings.blockPanelArea.bottomRight.y - THIN_LINE(), settings.sysColor);
        }

    }
    else if (mouseX < static_cast<int>(status.hScrollbarBlkStart)) scroll_left();
    else scroll_right();
}


void Main_Window::select_next_block() {

    if (status.currentTab == 0 || !status.focusBlock) return;
    if (status.editLock) cancel_data_input();

    if (status.get_current_blocktype() == 0) {

         //seek forward from current pos
        for (unsigned pos = status.get_current_reference_row() + 1; pos < currentTune.sequence.size(); pos++) {

            if (currentTune.sequence[pos] != currentTune.sequence[status.get_current_reference_row()]) {

//                unsigned offset = pos - status.get_current_reference_row();
                status.currentBlocks[0] = status.get_reference_iterator(currentTune.sequence[pos]);
                status.inc_current_reference_row(pos - status.get_current_reference_row());
                print_block_data();
                print_reference_data();
                return;
            }
        }

        //seek forward from start
        for (unsigned pos = 0; pos < status.get_current_reference_row(); pos++) {

            if (currentTune.sequence[pos] != currentTune.sequence[status.get_current_reference_row()]) {

                int diff = 0 - (static_cast<int>(pos) - static_cast<int>(status.get_current_reference_row()));

                status.currentBlocks[0] = status.get_reference_iterator(currentTune.sequence[pos]);
                status.dec_current_reference_row(static_cast<unsigned>(diff));
                print_block_data();
                print_reference_data();
                return;
            }
        }
    }
    else {
        unsigned nextBlk = status.get_current_block() + 1;
        if (nextBlk == currentTune.blockTypes[status.get_current_blocktype()].blocks.size()) nextBlk = 0;

        status.inc_current_reference_row();
        status.currentBlocks[status.get_current_blocktype()] = nextBlk;
        print_reference_data();
        print_block_data();
    }
}


void Main_Window::select_prev_block() {

    if (status.currentTab == 0 || !status.focusBlock) return;
    if (status.editLock) cancel_data_input();

    if (status.get_current_blocktype() == 0) {

        //seek backward from pos
        for (int pos = static_cast<int>(status.get_current_reference_row()); pos >= 0; pos--) {

            if (currentTune.sequence[static_cast<unsigned>(pos)]
                != currentTune.sequence[status.get_current_reference_row()]) {

                int diff = 0 - (pos - static_cast<int>(status.get_current_reference_row()));

                status.currentBlocks[0] = status.get_reference_iterator(currentTune.sequence[static_cast<unsigned>(pos)]);
                status.dec_current_reference_row(static_cast<unsigned>(diff));
                print_block_data();
                print_reference_data();
                return;
            }
        }

        //seek backward from end
        for (unsigned pos = currentTune.sequence.size() - 1; pos > status.get_current_reference_row(); pos--) {

            if (currentTune.sequence[pos] != currentTune.sequence[status.get_current_reference_row()]) {

                status.currentBlocks[0] = status.get_reference_iterator(currentTune.sequence[pos]);
                status.inc_current_reference_row(pos - status.get_current_reference_row());
                print_block_data();
                print_reference_data();
                return;
            }
        }
    }
    else {

        unsigned nextBlk = status.get_current_block();
        if (nextBlk == 0) nextBlk = currentTune.blockTypes[status.get_current_blocktype()].blocks.size() - 1;
        else nextBlk -= 1;

        status.dec_current_reference_row();
        status.currentBlocks[status.get_current_blocktype()] = nextBlk;
        print_reference_data();
        print_block_data();
    }
}



void Main_Window::receive_data_input(const char &data) {

//    if (status.editBlockName) {
//        userInputString = userInputString.substr(0, userInputString.size() - 1) + data + "_";
//        if (userInputString.size() == ((status.get_current_blocktype() == 0) ? 13 : 10)) complete_data_input();
//        else print_block_name();
//    }

	if (status.focusBlock) {

		currentField = status.get_current_field_pointer();
		stringstream datastr;

		if (data == '.') {

			if (status.get_current_cursor_row() != 0) {

				if (status.editLock) cancel_data_input();

                push_changelog();
				for (unsigned i = status.get_current_cursor_row(); i > 0; i--) {

					if (status.get_current_column_pointer()->columnData[i - 1].dataString != "") {

						currentField->set(status.get_current_column_pointer()->columnData[i - 1].dataString, settings.hexMode);

						if (!status.editLock && status.autoInc > 0) {

							status.inc_current_cursor_row();
							print_block_data();
							if (status.get_current_blocktype() == 0)
								currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
						}
						else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());

						break;
					}
				}
			}
		}

		else if (currentField->command->mdCmdAuto) {

            push_changelog();
			currentField->set("set", settings.hexMode);
			if (!status.editLock && status.autoInc > 0) {

				status.inc_current_cursor_row();
				print_block_data();
				if (status.get_current_blocktype() == 0)
					currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
			}
			else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
		}

		else if (!status.editLock && data == '=') {

            if (status.selectionType != BLOCK && currentField->command->allowModifiers) {
                push_changelog();
                currentField->remove_modifier();
                print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
            }
            else if (status.selectionType == BLOCK) {
                push_changelog();
                for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

                    if (status.get_current_block_pointer()->columns[col].command->allowModifiers) {

                        for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++)
                            status.get_current_block_pointer()->columns[col].columnData[row].remove_modifier();
                    }
                }
                print_block_data();
            }
		}

		else if (!status.editLock && currentField->command->allowModifiers
			&& (data == '+' || data == '-' || data == '*' || data == '/' || data == '|' || data == '&' || data == '^')) {

            if (!(currentField->dataString == "" || currentField->dataString == "rest")) {

                push_changelog();
                status.editLock = true;
                userInputString = data;
                if (settings.hexMode) userInputString += "$";
                datastr << data << 0;

                currentField->set(datastr.str(), settings.hexMode);
                print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
            }
		}
        //edit global fields with free string input
        //TODO might need a better check
		else if (currentField->command->mdCmdForceString && !currentField->command->mdCmdForceSubstitution
            && !currentField->command->isBlkReference) {

            if (!status.editLock) push_changelog();
//			status.editLock = true;
			userInputString += data;
			string prevString = currentField->dataString;
            userInputString = receive_string_input();

            if (userInputString != "") currentField->set(userInputString, settings.hexMode);
            else {

                currentField->set(prevString, settings.hexMode);
                undoStack.pop_back();
                if (undoStack.empty()) {

                    currentTune.hasUnsavedChanges = false;
                    set_display_title();
                }
            }

            userInputString = "";
			print_global_field(status.currentGlobal);
//			status.keyLock = 2;		//TODO temp solution, input is very unstable for some reason
		}

		else if (!status.editLock && currentField->command->useNoteNames) {

			stringstream noteName;

			if (data == settings.keyC1) noteName << "c" << status.currentOctave;
			else if (data == 's') noteName << "cis" << status.currentOctave;
			else if (data == 'x') noteName << "d" << status.currentOctave;
			else if (data == 'd') noteName << "dis" << status.currentOctave;
			else if (data == 'c') noteName << "e" << status.currentOctave;
			else if (data == 'v') noteName << "f" << status.currentOctave;
			else if (data == 'g') noteName << "fis" << status.currentOctave;
			else if (data == 'b') noteName << "g" << status.currentOctave;
			else if (data == 'h') noteName << "gis" << status.currentOctave;
			else if (data == 'n') noteName << "a" << status.currentOctave;
			else if (data == 'j') noteName << "ais" << status.currentOctave;
			else if (data == settings.keyB1) noteName << "b" << status.currentOctave;
			else if (data == settings.keyC2) noteName << "c" << status.currentOctave + 1;
			else if (data == '2') noteName << "cis" << status.currentOctave + 1;
			else if (data == settings.keyD2) noteName << "d" << status.currentOctave + 1;
			else if (data == '3') noteName << "dis" << status.currentOctave + 1;
			else if (data == 'e') noteName << "e" << status.currentOctave + 1;
			else if (data == 'r') noteName << "f" << status.currentOctave + 1;
			else if (data == '5') noteName << "fis" << status.currentOctave + 1;
			else if (data == 't') noteName << "g" << status.currentOctave + 1;
			else if (data == '6') noteName << "gis" << status.currentOctave + 1;
			else if (data == settings.keyA2) noteName << "a" << status.currentOctave + 1;
			else if (data == '7') noteName << "ais" << status.currentOctave + 1;
			else if (data == 'u') noteName << "b" << status.currentOctave + 1;
			else if (data == 'i') noteName << "c" << status.currentOctave + 2;
			else if (data == '9') noteName << "cis" << status.currentOctave + 2;
			else if (data == 'o') noteName << "d" << status.currentOctave + 2;
			else if (data == '0') noteName << "dis" << status.currentOctave + 2;
			else if (data == 'p') noteName << "e" << status.currentOctave + 2;
			else if (data == '1') noteName << "rest";
			else if (data == 'k') noteName << "noise";

			//verify that the given note name actually exists in the asm equates list, ignore input otherwise
			if (noteName.str() != "" && currentTune.musicdataBinary.equates.count(noteName.str())) {

                push_changelog();

				currentField->set(noteName.str(), settings.hexMode);

				if (status.get_current_blocktype() == 0 && soundEmul.playMode < PM_SONG && noteName.str() != "rest") play_row();

				if (status.autoInc > 0) {
					status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
			}
		}


		else if (!currentField->command->mdCmdForceSubstitution && !currentField->command->isBlkReference) {

			if ((data >= '0' && data <= '9') || (settings.hexMode && (data >= 'a' && data <= 'f'))) {

				//TODO: handle BOOL
				if (!status.editLock) {

                    push_changelog();
					status.editLock = true;
					if (settings.hexMode) userInputString += "$";
				}

				userInputString += data;

				size_t allowedFieldSize = (currentField->command->mdCmdType == BYTE) ? 3 : 5;
				if (userInputString.find_first_of("+-*/|&^") != string::npos) allowedFieldSize++;

				currentField->set(userInputString, settings.hexMode);

				if (userInputString.size() == allowedFieldSize) complete_data_input();

				if (!status.editLock && status.autoInc > 0) {

					status.inc_current_cursor_row();
		 			print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}
				else {
					if (status.currentTab == 0) print_global_field(status.currentGlobal);
					else print_field(status.get_current_cursor_column(), status.get_current_cursor_row());
				}
			}
		}

		else {

			if (!status.editLock) {

                push_changelog();
                status.editLock = true;
			}
			string prevInputString = userInputString;
			userInputString += data;

			vector<string> options;

			if (currentField->command->mdCmdForceSubstitution && !currentField->command->isBlkReference) {

				for (auto&& it: currentField->command->substitutionList) {

					if (it.first.compare(0, userInputString.size(), userInputString) == 0) options.push_back(it.first);
				}

				if (options.size() == 0) {

					userInputString = prevInputString;
					options.clear();

					for (auto&& it: currentField->command->substitutionList) {

						if (userInputString == "" || it.first.compare(0, userInputString.size(), userInputString) == 0)
							options.push_back(it.first);
					}

                    userInputString = auto_complete(options);
					display_options_list(userInputString, options);

				}

				else if (options.size() == 1) {

					currentField->set(options[0], settings.hexMode);
					complete_data_input();
					if (status.autoInc > 0) status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else {	//TODO asserting mdCommand::isBlkReference, may this should be enforced?

                    userInputString = auto_complete(options);
					display_options_list(userInputString, options);
				}
			}
			else {  //command is blkReference

				unsigned bt = 0;
                //determine target blocktype
				for (; currentField->command->referenceBlkID != currentTune.config.blockTypes[bt].blockConfigID; bt++);

				for (unsigned i = 0; i < currentTune.blockTypes[bt].blocks.size(); i++) {

					if (currentTune.blockTypes[bt].blocks[i].name.compare(0, userInputString.size(), userInputString) == 0) {

						options.push_back(currentTune.blockTypes[bt].blocks[i].name);
					}
				}

				if (options.size() == 0) {

					userInputString = prevInputString;
					options.clear();

					for (unsigned i = 0; i < currentTune.blockTypes[bt].blocks.size(); i++) {

						if (userInputString == ""
							|| currentTune.blockTypes[bt].blocks[i].name.compare(0, userInputString.size(), userInputString) == 0)
							options.push_back(currentTune.blockTypes[bt].blocks[i].name);
					}

                    if (options.size() == 1) {
                        currentField->set(options[0], settings.hexMode);
                        complete_data_input();
                        if (status.autoInc > 0) status.inc_current_cursor_row();
                        print_block_data();
                        if (status.get_current_blocktype() == 0)
                            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
                    }
                    else {
                        userInputString = auto_complete(options);
                        display_options_list(userInputString, options);
                    }
				}

				else if (options.size() == 1) {

					currentField->set(options[0], settings.hexMode);
					complete_data_input();
					if (status.autoInc > 0) status.inc_current_cursor_row();
					print_block_data();
					if (status.get_current_blocktype() == 0)
						currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
				}

				else {

                    userInputString = auto_complete(options);
					display_options_list(userInputString, options);
				}
			}
		}
	}

	else if (!status.focusBlock && status.get_current_blocktype() == 0) {

		if (!status.editLock && (data >= 'a' && data <= 'z')) {

			status.editLock = true;
			push_changelog();
		}

		if (status.editLock) {

			string previousInputString = userInputString;
			userInputString += data;

			vector<string> options;
			unsigned bt = status.get_current_blocktype();

			for (unsigned i = 0; i < currentTune.blockTypes[bt].blocks.size(); i++) {

				if (currentTune.blockTypes[bt].blocks[i].name.compare(0, userInputString.size(), userInputString) == 0)
					options.push_back(currentTune.blockTypes[bt].blocks[i].name);
			}

			if (options.size() == 1) {

				currentTune.sequence[status.get_current_reference_row()] = options[0];
				complete_data_input();
                print_reference_data();
			}

			else {

				if (options.size() == 0) {

					userInputString = previousInputString;
					options.clear();

					for (unsigned i = 0; i < currentTune.blockTypes[bt].blocks.size(); i++) {

						if (userInputString == ""
							|| currentTune.blockTypes[bt].blocks[i].name.compare(0, userInputString.size(), userInputString) == 0)
							options.push_back(currentTune.blockTypes[bt].blocks[i].name);
					}
				}

                userInputString = auto_complete(options);
				display_options_list(userInputString, options);
			}
		}
	}
}

string Main_Window::receive_string_input() {

    string outputStr = "";
    if (userInputString != "_") outputStr = userInputString;
//    soundEmul.stop();
    al_pause_event_queue(eventQueue, true);

    ALLEGRO_EVENT_QUEUE *kbdQueue = al_create_event_queue();
    if (!kbdQueue) throw (string("Could not initialize event queue"));

    al_register_event_source(kbdQueue, al_get_keyboard_event_source());
    if (soundEmul.playMode > PM_STOPPED)
        al_register_event_source(kbdQueue, al_get_audio_stream_event_source(soundEmul.audioStream));
    ALLEGRO_EVENT keyEv;

    if (status.editBlockName) print_block_name();
    else if (status.currentTab == 0) {

        currentField->set(userInputString, settings.hexMode);
        print_global_field(status.currentGlobal);
    }
    al_flip_display();

    bool running = true;

    while (running) {

        al_wait_for_event(kbdQueue, &keyEv);

        if (keyEv.type == ALLEGRO_EVENT_KEY_CHAR) {

            if (keyEv.keyboard.keycode >= ALLEGRO_KEY_A && keyEv.keyboard.keycode <= ALLEGRO_KEY_9) {

                outputStr.append(al_keycode_to_name(keyEv.keyboard.keycode));
                userInputString = outputStr + "_";

                if (status.editBlockName) {

                    if (status.get_current_blocktype() == 0) {
                        if (outputStr.size() >= 12) running = false;
                    }
                    else if (outputStr.size() >= 9) running = false;

                    if (running) print_block_name();
                }
                else if (status.currentTab == 0) {

                    currentField->set(userInputString, settings.hexMode);
                    print_global_field(status.currentGlobal);
                    if (outputStr.size() > 48) running = false;
                }
            }
            else if (keyEv.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {

                if (outputStr.size() > 0) outputStr = outputStr.substr(0, outputStr.size() - 1);
                userInputString = outputStr + "_";

                if (status.editBlockName) print_block_name();
                else if (status.currentTab == 0) {

                    currentField->set(userInputString, settings.hexMode);
                    print_global_field(status.currentGlobal);
                }

            }
            else if (keyEv.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
                outputStr = "";
            }
            else if (keyEv.keyboard.keycode == ALLEGRO_KEY_ENTER) running = false;

            al_flip_display();
        }

        else if (keyEv.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT) soundEmul.update_audio();

    }

    al_destroy_event_queue(kbdQueue);
    al_pause_event_queue(eventQueue, false);
    return outputStr;
}


void Main_Window::clone_block() {

    if (status.focusBlock || status.currentTab == 0) return;

    push_changelog();
    string newBlockName = currentTune.generate_unique_block_name();
    Datablock newBlock(newBlockName);
    newBlock = *status.get_current_block_pointer();
    newBlock.name = newBlockName;
    currentTune.blockTypes[status.get_current_blocktype()].blocks.push_back(newBlock);
    if (status.get_current_blocktype() == 0) currentTune.sequence[status.get_current_reference_row()] = newBlockName;
    print_reference_data();
}

void Main_Window::rename_block() {

    if (status.currentTab == 0) return;

    push_changelog();
    string currentBlkName = status.get_current_block_pointer()->name;
    status.editBlockName = true;
    status.editLock = true;
    userInputString = "_";

    if (receive_string_input() != "") {

        complete_data_input();
        if (status.get_current_block_pointer()->name == currentBlkName) {

            undoStack.pop_back();
            if (undoStack.empty()) {

                currentTune.hasUnsavedChanges = false;
                set_display_title();
            }
        }
    }
    else {

        cancel_data_input();
        print_block_name();
    }
}

void Main_Window::erase_data_input() {

    if (status.editBlockName) {
        if (userInputString.size() < 2) return;
        userInputString = userInputString.substr(0, userInputString.size() - 2) + "_";
        print_block_name();
        return;
    }

	else if (!status.focusBlock || status.currentTab != 0 || !status.get_current_field_pointer()->command->mdCmdForceString
		|| status.get_current_field_pointer()->command->mdCmdForceSubstitution
		|| status.get_current_field_pointer()->dataString == "")
		return;

	status.editLock = true;
	userInputString = status.get_current_field_pointer()->dataString.substr(0, status.get_current_field_pointer()->dataString.size() - 1);
	status.get_current_field_pointer()->set(userInputString, settings.hexMode);
	print_global_field(status.currentGlobal);
}

void Main_Window::complete_data_input() {

	if (status.editLock) {

        if (status.editBlockName) {

            //TODO: check uniqueness
            userInputString = userInputString.substr(0, userInputString.size() - 1);
            bool validName = true;
            for (auto&& it: currentTune.blockTypes[status.get_current_blocktype()].blocks) {
                if (it.name == userInputString) {
                    validName = false;
                    break;
                }
            }

            if (userInputString != "" && validName) {

                if (status.get_current_blocktype() == 0) {
                    for (auto&& it: currentTune.sequence) {
                        if (it == status.get_current_block_pointer()->name) it = userInputString;
                    }
                }
                //TODO: blockTypeID is currently ignored in replace_reference();
                currentTune.replace_reference(status.get_current_block_pointer()->name, userInputString, "");
                status.get_current_block_pointer()->name = userInputString;
                print_reference_data();
            }
            status.editBlockName = false;
            print_block_name();
        }

		else if (status.focusBlock) {

            Datablock_Field *field = status.get_current_field_pointer();

            if (field->command->isBlkReference) {

                unsigned targetBt;
                for (targetBt = 0; field->command->referenceBlkID != currentTune.config.blockTypes[targetBt].blockConfigID;
                    targetBt++);

                for (auto&& it: currentTune.blockTypes[targetBt].blocks) {

                    if (it.name == userInputString) {

                        field->set(userInputString, settings.hexMode);
                        break;
                    }
                }
            }

            else if (field->command->mdCmdForceSubstitution) {

                if (field->command->substitutionList.count(userInputString))
                    field->set(userInputString, settings.hexMode);
            }
		}

		else if (!status.focusBlock && status.get_current_blocktype() == 0) {

            for (auto&& it: currentTune.blockTypes[0].blocks) {
                if (it.name == userInputString) {

                    currentTune.sequence[status.get_current_reference_row()] = userInputString;
                    break;
                }
            }
		}

		userInputString = "";
		status.editLock = false;
	}
	if (status.currentTab == 0) status.keyLock = 3;
}

void Main_Window::cancel_data_input() {

    if (status.editLock && !undoStack.empty()) {

        undoStack.pop_back();
        if (undoStack.empty()) {
            currentTune.hasUnsavedChanges = false;
            set_display_title();
        }
    }
	userInputString = "";
	status.editLock = false;
	status.renameLock = false;
	status.editBlockName = false;
	if (status.currentTab != 0) print_block_name();
}

void Main_Window::display_options_list(const string &currentVal, const vector<string> &options) {

	//TODO: limit list output if ypos out of blockDataArea bounds
//	cout << "display_options_list\n";
	if (status.focusBlock) {

		if (status.currentTab > 0) {

			print_block_data();

			float xpos = settings.columnHeaderArea.topLeft.x;
			for (unsigned vcol = status.get_visible_first_column(); vcol < status.get_current_cursor_column(); vcol++)
				xpos += (status.get_current_block_pointer()->columns[vcol].width + BT_CHAR_WIDTH());

			float ypos = settings.blockDataArea.topLeft.y + (static_cast<float>(status.get_current_cursor_row()
				- status.get_visible_first_row()) * CHAR_HEIGHT());

			if (status.get_current_cursor_row() - status.get_visible_first_row() >= status.visibleRowsMax / 2) {

				//print upwards
				//TODO: make window size dependant on command type, see downwards print
				ypos -= static_cast<float>(options.size() * CHAR_HEIGHT());

				al_draw_filled_rectangle(xpos, (ypos >= settings.blockDataArea.topLeft.y) ? ypos : settings.blockDataArea.topLeft.y,
					xpos + ((currentField->command->mdCmdType == WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					(ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())), settings.sysColor);
				al_draw_filled_rectangle(xpos + 1.0f, (ypos >= settings.blockDataArea.topLeft.y)
					? ypos + 1.0f : settings.blockDataArea.topLeft.y + 1.0f,
					xpos + ((currentField->command->mdCmdType == WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())) - 1.0f,
					(ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())) - 1.0f, settings.bgColor);

				for (auto&& it: options) {

					if (ypos >= settings.blockDataArea.topLeft.y)
						al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, it.data());
					ypos += CHAR_HEIGHT();
				}

				al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, (currentVal == "") ? "_" : currentVal.data());
			}
			else {

				//print downwards
				al_draw_filled_rectangle(xpos, ypos, xpos + ((currentField->command->mdCmdType == WORD)
					? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					((ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) < settings.blockDataArea.bottomRight.y)
					? (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())) : settings.blockDataArea.bottomRight.y),
					settings.sysColor);
				al_draw_filled_rectangle(xpos + 1.0f, ypos + 1.0f, xpos - 1.0f
					+ ((currentField->command->mdCmdType == WORD) ? (9.0 * BT_CHAR_WIDTH()) : (5.0 * BT_CHAR_WIDTH())),
					(ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) < settings.blockDataArea.bottomRight.y)
					? (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) - 1.0f)
					: (settings.blockDataArea.bottomRight.y - 1.0f), settings.bgColor);

				al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, (currentVal == "") ? "_" : currentVal.data());

				for (auto&& it: options) {

					ypos += CHAR_HEIGHT();

					if ((ypos + CHAR_HEIGHT()) > settings.blockDataArea.bottomRight.y) break;

					al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, it.data());
				}
			}
		}
		else {

			//TODO: options list for global input
		}
	}

	else {

        print_reference_data();

		float xpos = settings.referenceDataArea.topLeft.x;
		if (status.get_current_blocktype() == 0) xpos += (4 * BT_CHAR_WIDTH());

		float ypos = settings.referenceDataArea.topLeft.y + (static_cast<float>(status.get_current_reference_row()
				- status.get_visible_first_reference_row()) * CHAR_HEIGHT());


		if (status.get_current_reference_row() - status.get_visible_first_reference_row() >= status.visibleReferenceRowsMax / 2) {

			//print upwards
			ypos -= static_cast<float>(options.size() * CHAR_HEIGHT());

			al_draw_filled_rectangle(xpos, (ypos >= settings.referenceDataArea.topLeft.y) ? ypos : settings.referenceDataArea.topLeft.y,
				xpos + (12.0 * BT_CHAR_WIDTH()), (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())), settings.sysColor);
			al_draw_filled_rectangle(xpos + 1.0f, (ypos >= settings.referenceDataArea.topLeft.y)
				? ypos + 1.0f : settings.referenceDataArea.topLeft.y + 1.0f,
				xpos + (12.0 * BT_CHAR_WIDTH()) - 1.0f, (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())) - 1.0f,
				settings.bgColor);

			for (auto&& it: options) {

				if (ypos >= settings.referenceDataArea.topLeft.y)
					al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, it.data());
				ypos += CHAR_HEIGHT();
			}

			al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, (currentVal == "") ? "_" : currentVal.data());

		}
		else {

			//print downwards
			al_draw_filled_rectangle(xpos, ypos, xpos + (12.0 * BT_CHAR_WIDTH()),
				((ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) < settings.referenceDataArea.bottomRight.y)
				? (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT())) : settings.referenceDataArea.bottomRight.y),
				settings.sysColor);
			al_draw_filled_rectangle(xpos + 1.0f, ypos + 1.0f, xpos - 1.0f + (12.0 * BT_CHAR_WIDTH()),
				(ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) < settings.referenceDataArea.bottomRight.y)
				? (ypos + static_cast<float>((options.size() + 1) * CHAR_HEIGHT()) - 1.0f)
				: (settings.referenceDataArea.bottomRight.y - 1.0f), settings.bgColor);

			al_draw_text(font, settings.rowActColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, (currentVal == "") ? "_" : currentVal.data());

			for (auto&& it: options) {

				ypos += CHAR_HEIGHT();

				if ((ypos + CHAR_HEIGHT()) > settings.referenceDataArea.bottomRight.y) break;

				al_draw_text(font, settings.rowHlColor, xpos, ypos, ALLEGRO_ALIGN_LEFT, it.data());
			}
		}
	}
}

string Main_Window::auto_complete(const vector<string> &options) {

    string testString = userInputString;
    string autoCompleted;
    bool valid = true;

    while (valid) {

        autoCompleted = testString;

        if (options[0].size() > testString.size()) {

            testString = options[0].substr(0, testString.size() + 1);

            for (auto&& it: options) {

                if (it.compare(0, testString.size(), testString) != 0) {

                    valid = false;
                    break;
                }
            }
        }
        else valid = false;

    }

    return autoCompleted;
}

void Main_Window::add_row() {

	if (status.focusBlock) {

		if  (status.get_current_column_pointer()->columnData.size() >= DISPLAY_MAX_ROWS) return;
		if (status.currentTab == 0) return;

        push_changelog();
		for (auto&& it: status.get_current_block_pointer()->columns) it.columnData.push_back(Datablock_Field(it.command));
		print_block_data();
	}

// 	//TODO must find a better way of detecting wether sequence is available or not
	else {

        push_changelog();

		string newBlockName = currentTune.generate_unique_block_name();
		Datablock newBlock(newBlockName);

		for (auto&& it: currentTune.blockTypes[status.get_current_blocktype()].commands) {

			Datablock_Column newColumn(it);
			for (unsigned i = 0; i < 16; i++) newColumn.columnData.push_back(Datablock_Field(it));
			newBlock.columns.push_back(newColumn);
		}

		currentTune.blockTypes[status.get_current_blocktype()].blocks.push_back(newBlock);
        if (status.get_current_blocktype() == 0) currentTune.sequence.push_back(newBlockName);
        print_reference_data();
	}
}


void Main_Window::remove_row() {

	if (status.focusBlock) {

        push_changelog();
		if (status.get_current_column_pointer()->columnData.size() <= 1) return;
		if (status.currentTab == 0) return;

		for (auto&& it: status.get_current_block_pointer()->columns) it.columnData.pop_back();

		if (status.get_current_cursor_row() == status.get_current_block_pointer()->columns[0].columnData.size())
			status.dec_current_cursor_row();

		print_block_data();
	}
	//REDUNDANT since Alt+Del on the last row does the same thing
//	else if (status.get_current_blocktype() == 0 && currentTune.sequence.size() > 1) {
//
//        push_changelog();
//		currentTune.sequence.pop_back();
//		print_reference_data();
//	}
}


void Main_Window::insert_row() {

	if (status.focusBlock) {

        push_changelog();
		if (status.get_current_column_pointer()->columnData.size() >= DISPLAY_MAX_ROWS) return;
		if (status.currentTab == 0) return;

 		for (auto&& it: status.get_current_block_pointer()->columns)
  			it.columnData.insert(it.columnData.begin() + status.get_current_cursor_row(), Datablock_Field(it.command));

		print_block_data();
	}

	else if (status.get_current_blocktype() == 0) {

        push_changelog();
		if (currentTune.sequenceLoopPoint >= status.get_current_reference_row()) currentTune.sequenceLoopPoint++;

		currentTune.sequence.insert(currentTune.sequence.begin() + status.get_current_reference_row(),
			currentTune.sequence[status.get_current_reference_row()]);

        print_reference_data();
	}
}


void Main_Window::delete_row() {

	if (status.focusBlock) {
        push_changelog();
		if (status.get_current_column_pointer()->columnData.size() <= 1) return;
		if (status.currentTab == 0) return;

 		for (auto&& it: status.get_current_block_pointer()->columns)
  			it.columnData.erase(it.columnData.begin() + status.get_current_cursor_row());

		if (status.get_current_cursor_row() == status.get_current_block_pointer()->columns[0].columnData.size())
			status.dec_current_cursor_row();
		print_block_data();
	}

	else if (status.get_current_blocktype() == 0 && currentTune.sequence.size() > 1) {
        push_changelog();
		if (currentTune.sequenceLoopPoint > status.get_current_reference_row()) currentTune.sequenceLoopPoint--;

		currentTune.sequence.erase(currentTune.sequence.begin() + status.get_current_reference_row());

		if (status.get_current_reference_row() >= currentTune.sequence.size()) status.dec_current_reference_row();

		status.set_current_block_from_cursor();
        status.focusBlock = false;
        print_block_data();
        print_reference_data();
	}
	//TODO: find better way of detecting sequence/no sequence
	//	(currently using status.get_current_blocktype())
	//TODO: need a better way of detecting if a block can be deleted or is required as default reference somewhere
	//	(currently using status.get_current_reference_row())
	else if (status.get_current_blocktype() && status.get_current_reference_row()
		&& currentTune.blockTypes[status.get_current_blocktype()].blocks.size() > 1) {
		unsigned oldRow = status.get_current_reference_row();

        push_changelog();
		currentTune.replace_reference(currentTune.blockTypes[status.get_current_blocktype()].blocks[oldRow].name,
			currentTune.blockTypes[status.get_current_blocktype()].blocks[0].name, "");

		currentTune.blockTypes[status.get_current_blocktype()].blocks.erase(
			currentTune.blockTypes[status.get_current_blocktype()].blocks.begin() + status.get_current_reference_row());

		if (oldRow == currentTune.blockTypes[status.get_current_blocktype()].blocks.size())
			status.dec_current_reference_row();

		if (oldRow == status.get_current_block()) status.set_current_block_from_cursor();

        status.focusBlock = false;
		print_block_tabs();
        print_reference_data();
	}
}


void Main_Window::insert_field() {

	if (status.focusBlock) {

        push_changelog();
		status.get_current_column_pointer()->columnData.pop_back();
		status.get_current_column_pointer()->columnData.insert(status.get_current_column_pointer()->columnData.begin()
			+ status.get_current_cursor_row(), Datablock_Field(status.get_current_column_pointer()->command));

		print_block_data();
	}
	else insert_row();
}


void Main_Window::delete_field() {

	if (status.focusBlock) {

        push_changelog();
		status.get_current_column_pointer()->columnData.erase(
			status.get_current_column_pointer()->columnData.begin() + status.get_current_cursor_row());
		status.get_current_column_pointer()->columnData.push_back(Datablock_Field(status.get_current_column_pointer()->command));

		print_block_data();
	}
	else delete_row();
}

void Main_Window::set_loop_point() {

	if (status.get_current_blocktype() != 0 || status.focusBlock) return;
	push_changelog();
	currentTune.sequenceLoopPoint = status.get_current_reference_row();
    print_reference_data();
}

void Main_Window::copy_selection() {

    if (status.selectionType == INACTIVE) return;
    if (currentTune.clipboardStatus != INACTIVE) {
        currentTune.clipboard.clear();
        currentTune.clipboardSequence.clear();
    }

    if (status.focusBlock) {

        currentTune.clipboardStatus = BLOCK;
        unsigned targetCol = 0;

        for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

            currentTune.clipboard.push_back(Datablock_Column(status.get_current_block_pointer()->columns[col].command));

            for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++)
                currentTune.clipboard[targetCol].columnData
                    .push_back(status.get_current_block_pointer()->columns[col].columnData[row]);

            targetCol++;
        }
    }
    else {

        currentTune.clipboardStatus = SEQUENCE;
        for (unsigned pos = status.selectionRowFirst; pos <= status.selectionRowLast; pos++)
            currentTune.clipboardSequence.push_back(currentTune.sequence[pos]);
    }
}


void Main_Window::cut_selection() {

    if (status.selectionType == INACTIVE) return;
    push_changelog();

    if (!status.focusBlock) {

        delete_selection();
        return;
    }

    copy_selection();

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++)
            status.get_current_block_pointer()->columns[col].columnData[row].set("", settings.hexMode);
    }

    print_block_data();
}


void Main_Window::delete_selection() {

    if (status.selectionType == INACTIVE) return;

    push_changelog();
    copy_selection();

    if (status.selectionType == SEQUENCE) {

        currentTune.sequence.erase(currentTune.sequence.begin() + status.selectionRowFirst,
            currentTune.sequence.begin() + status.selectionRowLast + 1);

        if (currentTune.sequenceLoopPoint >= currentTune.sequence.size())
            currentTune.sequenceLoopPoint = currentTune.sequence.size() - 1;

        print_reference_data();
    }
    else {

        for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

            status.get_current_block_pointer()->columns[col].columnData
                .erase(status.get_current_block_pointer()->columns[col].columnData.begin() + status.selectionRowFirst,
                status.get_current_block_pointer()->columns[col].columnData.begin() + status.selectionRowLast + 1);

            for (unsigned i = 0; i < status.selectionRowLast + 1 - status.selectionRowFirst; i++)
                status.get_current_block_pointer()->columns[col].columnData
                    .push_back(Datablock_Field(status.get_current_block_pointer()->columns[col].command));
        }

        print_block_data();
    }
}

void Main_Window::insert_selection() {

    if (currentTune.clipboardStatus == INACTIVE) return;

    if (status.focusBlock) {

        if (currentTune.clipboardStatus != BLOCK) return;

        push_changelog();
        unsigned targetCol = status.get_current_cursor_column();

        for (auto&& ccol: currentTune.clipboard) {

            if (currentTune.config.isCompatible(*ccol.command, *status.get_current_block_pointer()->columns[targetCol].command)) {

                unsigned sourceRow = 0;
                unsigned maxInsert = status.get_current_block_pointer()->columns[0].columnData.size();

                for (unsigned targetRow = status.get_current_cursor_row();
                    targetRow < maxInsert && sourceRow < ccol.columnData.size(); targetRow++) {
                        status.get_current_block_pointer()->columns[targetCol].columnData
                            .insert(status.get_current_block_pointer()->columns[targetCol].columnData.begin() + targetRow,
                            ccol.columnData[sourceRow]);
                        sourceRow++;
                    }

                while (status.get_current_block_pointer()->columns[targetCol].columnData.size() > maxInsert)
                    status.get_current_block_pointer()->columns[targetCol].columnData.pop_back();
            }

            targetCol++;
            if (targetCol >= status.get_current_block_pointer()->columns.size()) break;
        }

        print_block_data();
    }
    else {

        if (currentTune.clipboardStatus != SEQUENCE || status.get_current_blocktype() != 0) return;

        push_changelog();
        unsigned pos = status.get_current_reference_row();

        for (auto&& it: currentTune.clipboardSequence) {

            if (currentTune.config.seqMaxLength && pos >= currentTune.config.seqMaxLength) break;
            currentTune.sequence.insert(currentTune.sequence.begin() + pos, it);
            pos++;
        }
        print_reference_data();
    }
}


void Main_Window::paste_selection() {

    if (currentTune.clipboardStatus == INACTIVE) return;
    if (status.focusBlock) {

        if (currentTune.clipboardStatus != BLOCK) return;

        push_changelog();
        unsigned targetCol = status.get_current_cursor_column();

        for (auto&& ccol: currentTune.clipboard) {

            if (currentTune.config.isCompatible(*ccol.command, *status.get_current_block_pointer()->columns[targetCol].command)) {

                unsigned sourceRow = 0;

                for (unsigned targetRow = status.get_current_cursor_row();
                    targetRow < status.get_current_block_pointer()->columns[0].columnData.size()
                    && sourceRow < ccol.columnData.size(); targetRow++) {
                        status.get_current_block_pointer()->columns[targetCol].columnData[targetRow] = ccol.columnData[sourceRow];
                        sourceRow++;
                    }
            }

            targetCol++;
            if (targetCol >= status.get_current_block_pointer()->columns.size()) break;
        }

        //TODO move cursor to end of pasted selection
        print_block_data();
    }
    else {
        if (currentTune.clipboardStatus != SEQUENCE || status.get_current_blocktype() != 0) return;
        push_changelog();
        unsigned pos = status.get_current_reference_row();

        for (auto&& it: currentTune.clipboardSequence) {

            if (currentTune.config.seqMaxLength && pos >= currentTune.config.seqMaxLength) break;
            if (pos < currentTune.sequence.size()) currentTune.sequence[pos] = it;
            else currentTune.sequence.insert(currentTune.sequence.begin() + pos, it);
            pos++;
        }

        print_reference_data();
    }
}


void Main_Window::porous_paste_selection() {

    if (currentTune.clipboardStatus != BLOCK || !status.focusBlock) return;

    push_changelog();
    unsigned targetCol = status.get_current_cursor_column();

    for (auto&& ccol: currentTune.clipboard) {

        if (currentTune.config.isCompatible(*ccol.command, *status.get_current_block_pointer()->columns[targetCol].command)) {

            unsigned sourceRow = 0;

            for (unsigned targetRow = status.get_current_cursor_row();
                targetRow < status.get_current_block_pointer()->columns[0].columnData.size()
                && sourceRow < ccol.columnData.size(); targetRow++) {
                    if (status.get_current_block_pointer()->columns[targetCol].columnData[targetRow].dataString == "")
                        status.get_current_block_pointer()->columns[targetCol].columnData[targetRow] = ccol.columnData[sourceRow];
                    sourceRow++;
                }
        }

        targetCol++;
        if (targetCol >= status.get_current_block_pointer()->columns.size()) break;
    }

    print_block_data();
}


void Main_Window::inverse_porous_paste_selection() {

    if (currentTune.clipboardStatus != BLOCK || !status.focusBlock) return;

    push_changelog();
    unsigned targetCol = status.get_current_cursor_column();

    for (auto&& ccol: currentTune.clipboard) {

        if (currentTune.config.isCompatible(*ccol.command, *status.get_current_block_pointer()->columns[targetCol].command)) {

            unsigned sourceRow = 0;

            for (unsigned targetRow = status.get_current_cursor_row();
                targetRow < status.get_current_block_pointer()->columns[0].columnData.size()
                && sourceRow < ccol.columnData.size(); targetRow++) {
                    if (ccol.columnData[sourceRow].dataString != "")
                        status.get_current_block_pointer()->columns[targetCol].columnData[targetRow] = ccol.columnData[sourceRow];
                    sourceRow++;
                }
        }

        targetCol++;
        if (targetCol >= status.get_current_block_pointer()->columns.size()) break;
    }

    print_block_data();
}

//TODO must handle selections with multiple values as well!
void Main_Window::interpolate_selection(const unsigned &interpolationType) {

    if (status.selectionType != BLOCK || !status.focusBlock) return;

    push_changelog();

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        if (!(status.get_current_block_pointer()->columns[col].command->mdCmdType == BOOL)
            && !(status.get_current_block_pointer()->columns[col].command->mdCmdForceString)
            && status.get_current_block_pointer()->columns[col].command->substitutionList.empty()) {

            unsigned searchBegin = status.selectionRowFirst;
            unsigned nextField = find_next_used_field(col, searchBegin);

            if (status.get_current_block_pointer()->columns[col].command->useNoteNames) {
                //TODO
                while (nextField <= status.selectionRowLast) {

                    float increment;
                    //TODO evaluate modifiers
                    //TODO safeguard against fields that aren't note names
//                    long startValue = currentTune.get_frequency_divider(status.get_current_block_pointer()
//                        ->columns[col].columnData[searchBegin].arg1);
//                    long endValue = currentTune.get_frequency_divider(status.get_current_block_pointer()
//                        ->columns[col].columnData[nextField].arg1);
                    long startValue = currentTune.musicdataBinary.evalExpr(status.get_current_block_pointer()
                        ->columns[col].columnData[searchBegin].dataString);
                    long endValue = currentTune.musicdataBinary.evalExpr(status.get_current_block_pointer()
                        ->columns[col].columnData[nextField].dataString);

                    float nextValue = static_cast<float>(startValue);

                    if (interpolationType == LINEAR) {

                        increment = static_cast<float>(endValue - startValue) / static_cast<float>(nextField - searchBegin);

                        for (unsigned row = searchBegin + 1; row < nextField; row++) {
                            nextValue += increment;
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(currentTune.get_note_data_string(static_cast<unsigned>(nextValue + 0.5f)), settings.hexMode);
                        }
                    }

                    else if (interpolationType == LOGARITHMIC) {

                        increment = static_cast<float>(endValue - startValue) / 2.0f;

                        for (unsigned row = searchBegin + 1; row < nextField; row++) {

                            nextValue += increment;
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(currentTune.get_note_data_string(static_cast<unsigned>(nextValue + 0.5f)), settings.hexMode);
                            increment /= 2.0f;
                        }
                    }

                    else {

                        increment = static_cast<float>(endValue - startValue) / 2.0f;
                        vector<float> steps;

                        for (unsigned i = 0; i < (status.selectionRowLast - status.selectionRowFirst); i++) {

                            steps.push_back(increment);
                            increment /= 2.0f;
                        }

                        unsigned step = steps.size() - 1;
                        for (unsigned row = searchBegin + 1; row < nextField; row++) {

                            nextValue += steps[step];
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(currentTune.get_note_data_string(static_cast<unsigned>(nextValue + 0.5f)), settings.hexMode);
                            step--;
                        }

                    }

                    searchBegin = nextField;
                    nextField = find_next_used_field(col, searchBegin);
                }
            }
            else {

//                unsigned searchBegin = status.selectionRowFirst;
//                unsigned nextField = find_next_used_field(col, searchBegin);

                while (nextField <= status.selectionRowLast) {

                    float increment;
                    long startValue;
                    long endValue;
                    if (status.get_current_block_pointer()->columns[col].command->allowModifiers) {
                        startValue = currentTune.musicdataBinary.evalExpr(
                            status.get_current_block_pointer()->columns[col].columnData[searchBegin].dataString);
                        endValue = currentTune.musicdataBinary.evalExpr(
                            status.get_current_block_pointer()->columns[col].columnData[nextField].dataString);
                    }
                    else {
                        startValue = strToNum(status.get_current_block_pointer()->columns[col].columnData[searchBegin].arg1);
                        endValue = strToNum(status.get_current_block_pointer()->columns[col].columnData[nextField].arg1);
                    }


                    float nextValue = static_cast<float>(startValue);

                    if (interpolationType == LINEAR) {

                        increment = static_cast<float>(endValue - startValue) / static_cast<float>(nextField - searchBegin);

                        for (unsigned row = searchBegin + 1; row < nextField; row++) {
                            nextValue += increment;
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(numToStr(static_cast<unsigned>(nextValue + 0.5f), 0, false), settings.hexMode);
                        }
                    }
                    else if (interpolationType == LOGARITHMIC) {

                        increment = static_cast<float>(endValue - startValue) / 2.0f;

                        for (unsigned row = searchBegin + 1; row < nextField; row++) {

                            nextValue += increment;
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(numToStr(static_cast<unsigned>(nextValue + 0.5f), 0, false), settings.hexMode);
                            increment /= 2.0f;
                        }
                    }
                    else {

                        increment = static_cast<float>(endValue - startValue) / 2.0f;
                        vector<float> steps;

                        for (unsigned i = 0; i < (status.selectionRowLast - status.selectionRowFirst); i++) {

                            steps.push_back(increment);
                            increment /= 2.0f;
                            cout << steps.back() << endl;
                        }

                        unsigned step = steps.size() - 1;
                        for (unsigned row = searchBegin + 1; row < nextField; row++) {

                            nextValue += steps[step];
                            status.get_current_block_pointer()->columns[col].columnData[row]
                                .set(numToStr(static_cast<unsigned>(nextValue + 0.5f), 0, false), settings.hexMode);
                            step--;
                        }
                    }

                    searchBegin = nextField;
                    nextField = find_next_used_field(col, searchBegin);
                }
            }
        }
    }

    print_block_data();
}

unsigned Main_Window::find_next_used_field(const unsigned &col, const unsigned &startPosition) {

    unsigned nextField;

    for (nextField = startPosition + 1; nextField <= status.selectionRowLast
        && status.get_current_block_pointer()->columns[col].columnData[nextField].dataString == ""; nextField++) {}

    return nextField;
}


void Main_Window::reverse_selection() {

    if (status.selectionType != BLOCK || !status.focusBlock) return;

    push_changelog();

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        vector<Datablock_Field> tempData;
        tempData.insert(tempData.begin(),
            status.get_current_block_pointer()->columns[col].columnData.begin() + status.selectionRowFirst,
            status.get_current_block_pointer()->columns[col].columnData.begin() + status.selectionRowLast + 1);

        unsigned row = status.selectionRowLast;
        for (auto&& it: tempData) {
            status.get_current_block_pointer()->columns[col].columnData[row] = it;
            row--;
        }
    }
}


void Main_Window::randomize_selection() {

    if (status.selectionType != BLOCK || !status.focusBlock) return;

    push_changelog();

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        mdCommand *cmd = status.get_current_block_pointer()->columns[col].command;

        if (!cmd->isBlkReference && !cmd->mdCmdAuto && !cmd->mdCmdForceString) {

            vector<unsigned> randomData = generate_random_data(status.selectionRowLast - status.selectionRowFirst + 1);

            unsigned row = status.selectionRowFirst;

            for (auto&& it: randomData) {

                if (cmd->limitRange) it = cmd->lowerRangeLimit + (it % (cmd->upperRangeLimit - cmd->lowerRangeLimit));

                if (cmd->mdCmdForceSubstitution) {

                    unsigned pos = it % cmd->substitutionList.size();
                    auto el = cmd->substitutionList.begin();
                    for (unsigned i = 0; i < pos; i++) el++;

                    status.get_current_block_pointer()->columns[col].columnData[row].set(el->first, settings.hexMode);
                }
                else if (cmd->useNoteNames) {

                    unsigned pos = it % currentTune.freqDividers.size();

                    status.get_current_block_pointer()->columns[col].columnData[row]
                        .set(currentTune.get_note_data_string(currentTune.freqDividers[pos]), settings.hexMode);
                }
                else if (cmd->mdCmdType == BOOL) {

                    if (it & 1) status.get_current_block_pointer()->columns[col].columnData[row].set("true", settings.hexMode);
                    else status.get_current_block_pointer()->columns[col].columnData[row].set("false", settings.hexMode);
                }
                else if (cmd->mdCmdType == BYTE) {

                    status.get_current_block_pointer()->columns[col].columnData[row]
                        .set(numToStr(it & 0xff, (settings.hexMode) ? 2 : 3, settings.hexMode), settings.hexMode);
                }
                else if (cmd->mdCmdType == WORD) {

                    status.get_current_block_pointer()->columns[col].columnData[row]
                        .set(numToStr(it & 0xffff, (settings.hexMode) ? 4 : 5, settings.hexMode), settings.hexMode);
                }

                row++;
            }
        }
    }

    print_block_data();
}


void Main_Window::fill_data(const unsigned &fillType) {

    if (currentTune.clipboardStatus != BLOCK || !status.focusBlock) return;

    if (status.selectionType == INACTIVE) {

        //TODO should it do all columns on clipboard?
        if (!currentTune.config.isCompatible(*currentTune.clipboard[0].command, *status.get_current_column_pointer()->command))
            return;

        push_changelog();

        unsigned sourcePos = 0;

        for (unsigned pos = status.get_current_cursor_row();
            pos < status.get_current_column_pointer()->columnData.size(); pos++) {

            if (fillType == REPLACE || status.get_current_column_pointer()->columnData[pos].dataString == "")
                status.get_current_column_pointer()->columnData[pos] = currentTune.clipboard[0].columnData[sourcePos];
            sourcePos++;
            if (sourcePos >= currentTune.clipboard[0].columnData.size()) sourcePos = 0;
        }
    }
    else {

        push_changelog();
        unsigned sourceCol = 0;
        for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

            if (currentTune.config.isCompatible(*currentTune.clipboard[sourceCol].command,
                *status.get_current_block_pointer()->columns[col].command)) {

                unsigned sourceRow = 0;

                for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++) {

                    if (fillType == REPLACE || status.get_current_block_pointer()->columns[col].columnData[row].dataString == "")
                        status.get_current_block_pointer()->columns[col].columnData[row]
                        = currentTune.clipboard[sourceCol].columnData[sourceRow];

                    sourceRow++;
                    if (sourceRow >= currentTune.clipboard[0].columnData.size()) sourceRow = 0;
                }
            }

            sourceCol++;
            if (sourceCol >= currentTune.clipboard.size()) break;
        }
    }

    print_block_data();
}


void Main_Window::transpose(const int &amount) {

    if (status.selectionType != BLOCK || !status.focusBlock) return;

//    bool confirmOutOfRange = true;
    unsigned outOfRangeCount = 0;

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        if (status.get_current_block_pointer()->columns[col].command->useNoteNames) {

            //check and display message
            for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++) {

                if (status.get_current_block_pointer()->columns[col].columnData[row].arg1 != ""
                    && status.get_current_block_pointer()->columns[col].columnData[row].arg1 != "rest"
                    && status.get_current_block_pointer()->columns[col].columnData[row].arg1 != "noise") {

                    int index = static_cast<signed>(currentTune.get_note_index(status.get_current_block_pointer()
                        ->columns[col].columnData[row].arg1));
//                    cout << "arg1: " << status.get_current_block_pointer()->columns[col].columnData[row].arg1 << ", index: " << index << " + amount: " << amount << ", divider: " << currentTune.freqDividers[(index + amount)] << endl;
                    if ((index + amount) < 0 || (index + amount) >= static_cast<int>(currentTune.freqDividers.size())
                        || currentTune.freqDividers[(index + amount)] == 0) {

                            outOfRangeCount++;
//                            cout << "out of range" << endl;
                    }
                }
            }
        }
    }


    if (outOfRangeCount) {
        if (!display_msg_confirm_out_of_range_transpose(outOfRangeCount)) {
            return;
        }
    }


    push_changelog();

    for (unsigned col = status.selectionColumnFirst; col <= status.selectionColumnLast; col++) {

        if (status.get_current_block_pointer()->columns[col].command->useNoteNames) {

            for (unsigned row = status.selectionRowFirst; row <= status.selectionRowLast; row++) {

                if (status.get_current_block_pointer()->columns[col].columnData[row].arg1 != ""
                    && status.get_current_block_pointer()->columns[col].columnData[row].arg1 != "rest"
                    && status.get_current_block_pointer()->columns[col].columnData[row].arg1 != "noise") {

                    int index = static_cast<signed>(currentTune.get_note_index(status.get_current_block_pointer()
                        ->columns[col].columnData[row].arg1)) + amount;
                    string dataString = "";
                    if (index < 0 || index >= static_cast<int>(currentTune.freqDividers.size())
                        || currentTune.freqDividers[index] == 0)
                        dataString = "rest";
                    else dataString = currentTune.noteNames[index % 12] + numToStr((index / 12), 0, false);

                    status.get_current_block_pointer()->columns[col].columnData[row].set(dataString, settings.hexMode);
                }
            }
        }
    }

    print_block_data();
}


void Main_Window::expand_block() {

    if (status.currentTab == 0) return;

    unsigned len = status.get_current_block_pointer()->columns[0].columnData.size() * status.expandFactor;
    unsigned maxLen = static_cast<unsigned>(currentTune.config.blockTypes[status.get_current_blocktype()].blkMaxLength);
    if (maxLen != 0 && len > maxLen) return;

    cancel_data_input();
    status.cancel_selection();
    push_changelog();

    for (unsigned col = 0; col < status.get_current_block_pointer()->columns.size(); col++) {

        unsigned row = 0;
        Datablock_Field field(status.get_current_block_pointer()->columns[col].command);

        while (row < len) {

            row++;
            for (unsigned i = 0; i < status.expandFactor - 1; i++)
                status.get_current_block_pointer()->columns[col].columnData
                .insert(status.get_current_block_pointer()->columns[col].columnData.begin() + row, field);
            row += status.expandFactor - 1;
        }
    }

    print_block_data();
}


void Main_Window::shrink_block() {

    if (status.currentTab == 0) return;
    if (status.get_current_block_pointer()->columns[0].columnData.size() < status.expandFactor) return;

    cancel_data_input();
    status.cancel_selection();
    push_changelog();

    for (unsigned col = 0; col < status.get_current_block_pointer()->columns.size(); col++) {

        for (unsigned row = 0; status.get_current_block_pointer()->columns[col].columnData.size() >= row + status.expandFactor;
            row++) {

            status.get_current_block_pointer()->columns[col].columnData
                .erase(status.get_current_block_pointer()->columns[col].columnData.begin() + row + 1,
                status.get_current_block_pointer()->columns[col].columnData.begin() + row + status.expandFactor);
        }
    }

    print_block_data();
}


void Main_Window::undo() {

    if (undoStack.empty()) return;
    if (status.editLock) cancel_data_input();
    redoStack.push_back(Changelog_Entry(&currentTune, &status));
    undoStack.back().retrieve(&currentTune, &status);
    undoStack.pop_back();

    print_block_data();
    print_reference_data();

    if (undoStack.empty()) {

        currentTune.hasUnsavedChanges = false;
        set_display_title();
    }
}


void Main_Window::redo() {

    if (redoStack.empty()) return;
    if (status.editLock) cancel_data_input();
    undoStack.push_back(Changelog_Entry(&currentTune, &status));
    redoStack.back().retrieve(&currentTune, &status);
    redoStack.pop_back();

    if (!currentTune.hasUnsavedChanges) {

        currentTune.hasUnsavedChanges = true;
        set_display_title();
    }

    print_block_data();
    print_reference_data();
}


void Main_Window::push_changelog() {

    redoStack.clear();
    //prevent undoStack from growing indefinately
    if (undoStack.size() >= 120) undoStack.erase(undoStack.begin(), undoStack.begin() + 20);
    undoStack.push_back(Changelog_Entry(&currentTune, &status));

    if (!currentTune.hasUnsavedChanges) {

        currentTune.hasUnsavedChanges = true;
        set_display_title();
    }
}
