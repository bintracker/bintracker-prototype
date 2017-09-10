#include <iostream>
#include "gui.h"

void Main_Window::handle_mouse_input() {

    //if left mouse button isn't pressed, reset some status vars and do hover effects
    if (!mouseState.buttons & 1) {

        status.leftMouseButtonDown = false;
        status.mouseDragLockH = false;
        status.mouseDragLockV = false;
        status.mouseDragLockRefV = false;

        messagePanel.totalArea.clear(settings.bgColor);

        //clear menu button hover effects
        if (status.menuButtonHighlighted >= 0) {

            float xpos = status.menuButtonHighlighted * BUTTON_SIZE()
                + ((status.menuButtonHighlighted * 2) + 1) * DEFAULT_MARGIN();
            al_draw_filled_rectangle(xpos, 6.0f, xpos + BUTTON_SIZE(), 6.0f + BUTTON_SIZE(), settings.bgColor);
            al_draw_tinted_bitmap(menuButtons[status.menuButtonHighlighted], settings.sysColor, xpos, 6.0f, 0);
            status.menuButtonHighlighted = -1;
        }

        //clear block/reference panel button hover effects
        if (status.panelButtonHighlighted >= 0) {

            switch (status.panelButtonHighlighted) {

                case 0:
                    al_draw_tinted_bitmap(arrowLeft, settings.sysColor, blockPanel.printArea.topLeft.x,
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 1:
                    al_draw_tinted_bitmap(arrowRight, settings.sysColor,
                        blockPanel.printArea.topLeft.x + SMALL_BUTTON_SIZE() + 12 * CHAR_WIDTH() + 3 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 2:
                    al_draw_tinted_bitmap(buttonPlus, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 5 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 3:
                    al_draw_tinted_bitmap(buttonMinus, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 3 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 6 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 4:
                    al_draw_tinted_bitmap(buttonInsert, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 4 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 7 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 5:
                    al_draw_tinted_bitmap(buttonRemove, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 5 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 8 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 6:
                    al_draw_tinted_bitmap(buttonExpand, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 6 * SMALL_BUTTON_SIZE() + 24 * CHAR_WIDTH() + 10 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 7:
                    al_draw_tinted_bitmap(buttonShrink, settings.sysColor,
                        blockPanel.printArea.topLeft.x + 7 * SMALL_BUTTON_SIZE() + 24 * CHAR_WIDTH() + 11 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 10:
                    al_draw_tinted_bitmap(buttonPlus, settings.sysColor, referencePanel.printArea.topLeft.x,
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 11: {
                    if (status.get_current_blocktype() == 0) al_draw_tinted_bitmap(buttonInsert, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 1 * SMALL_BUTTON_SIZE() + 1 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    else al_draw_tinted_bitmap(buttonRemove, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 1 * SMALL_BUTTON_SIZE() + 1 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                }
                case 12: {
                    if (status.get_current_blocktype() == 0) al_draw_tinted_bitmap(buttonRemove, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 2 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    else al_draw_tinted_bitmap(buttonClone, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 2 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                }
//                case 13:
//                    al_draw_tinted_bitmap(buttonRemove, settings.sysColor,
//                        referencePanel.printArea.topLeft.x + 3 * SMALL_BUTTON_SIZE() + 3 * DEFAULT_MARGIN(),
//                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
//                    break;
                case 13:
                    al_draw_tinted_bitmap(buttonClone, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 3 * SMALL_BUTTON_SIZE() + 3 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 14:
                    al_draw_tinted_bitmap(buttonLooppoint, settings.sysColor,
                        referencePanel.printArea.topLeft.x + 4 * SMALL_BUTTON_SIZE() + 4 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    break;
                case 20:
                    al_draw_tinted_bitmap(arrowDown, settings.sysColor,
                        2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()),
                        buttonPanel.printArea.topLeft.y - 1.0f, 0);
                    break;
                case 21:
                    al_draw_tinted_bitmap(arrowUp, settings.sysColor, SMALL_BUTTON_SIZE() + 2.0f
                        + 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()),
                        buttonPanel.printArea.topLeft.y - 1.0f, 0);
                    break;
                case 22:
                    al_draw_tinted_bitmap(arrowDown, settings.sysColor,
                        2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()),
                        buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
                    break;
                case 23:
                    al_draw_tinted_bitmap(arrowUp, settings.sysColor, SMALL_BUTTON_SIZE() + 2.0f
                        + 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()),
                        buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
                    break;

                default:
                    break;
            }

            status.panelButtonHighlighted = -1;
        }

        //button panel
        if (mouseState.y < static_cast<int>(BUTTON_PANEL_HEIGHT())) {

            if (mouseState.y <= 0) {}   //workaround for buttons lighting up if mouse is initially out of display bounds
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "New...");
                al_draw_tinted_bitmap(menuButtons[0], settings.rowActColor, DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 0;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 2) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Open... (Ctrl+O)");
                al_draw_tinted_bitmap(menuButtons[1], settings.rowActColor, BUTTON_SIZE() + 3.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 1;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 3) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Save (Ctrl+S)");
                al_draw_tinted_bitmap(menuButtons[2], settings.rowActColor, 2.0f * BUTTON_SIZE() + 5.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 2;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 4) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Save As... (Ctrl+Shift+S)");
                al_draw_tinted_bitmap(menuButtons[3], settings.rowActColor, 3.0f * BUTTON_SIZE() + 7.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 3;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 5) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Play from Start (F5)");
                al_draw_tinted_bitmap(menuButtons[4], settings.rowActColor, 4.0f * BUTTON_SIZE() + 9.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 4;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 6) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Play from Current Position (F6)");
                al_draw_tinted_bitmap(menuButtons[5], settings.rowActColor, 5.0f * BUTTON_SIZE() + 11.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 5;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 7) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Play Pattern (F7)");
                al_draw_tinted_bitmap(menuButtons[6], settings.rowActColor, 6.0f * BUTTON_SIZE() + 13.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 6;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 8) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Stop (F8)");
                al_draw_tinted_bitmap(menuButtons[7], settings.rowActColor, 7.0f * BUTTON_SIZE() + 15.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 7;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 9) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Undo (Ctrl+Z)");
                al_draw_tinted_bitmap(menuButtons[8], settings.rowActColor, 8.0f * BUTTON_SIZE() + 17.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 8;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 10) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Redo (Ctrl+Y)");
                al_draw_tinted_bitmap(menuButtons[9], settings.rowActColor, 9.0f * BUTTON_SIZE() + 19.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 9;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 11) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Copy (Ctrl+C)");
                al_draw_tinted_bitmap(menuButtons[10], settings.rowActColor, 10.0f * BUTTON_SIZE() + 21.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 10;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 12) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Cut (Ctrl+X)");
                al_draw_tinted_bitmap(menuButtons[11], settings.rowActColor, 11.0f * BUTTON_SIZE() + 23.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 11;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 13) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Delete (Cut & Shift)(Ctrl+Del)");
                al_draw_tinted_bitmap(menuButtons[12], settings.rowActColor, 12.0f * BUTTON_SIZE() + 25.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 12;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 14) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Paste (Ctrl+P)");
                al_draw_tinted_bitmap(menuButtons[13], settings.rowActColor, 13.0f * BUTTON_SIZE() + 27.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 13;
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 15) {
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                ALLEGRO_ALIGN_LEFT, "Insert (Ctrl+V)");
                al_draw_tinted_bitmap(menuButtons[14], settings.rowActColor, 14.0f * BUTTON_SIZE() + 29.0f * DEFAULT_MARGIN(), 6.0f, 0);
                status.menuButtonHighlighted = 14;
            }
            else if (mouseState.x > static_cast<int>(2 * DEFAULT_MARGIN()
                + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()))
                && mouseState.x < static_cast<int>(3 * DEFAULT_MARGIN()
                + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH())
                + 2 * SMALL_BUTTON_SIZE())) {

                float xpos = 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size())
                     + (10 * CHAR_WIDTH());

                if (mouseState.y < static_cast<int>(buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f)) {
                    if (mouseState.x < 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size())
                        + (10 * CHAR_WIDTH()) + SMALL_BUTTON_SIZE() + 2.0f) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                            ALLEGRO_ALIGN_LEFT, "Decrease Base Octave (Alt+7)");
                        al_draw_tinted_bitmap(arrowDown, settings.rowActColor, xpos, buttonPanel.printArea.topLeft.y - 1.0f, 0);
                        status.panelButtonHighlighted = 20;
                    }
                    else {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                            ALLEGRO_ALIGN_LEFT, "Increase Base Octave (Alt+8)");
                        al_draw_tinted_bitmap(arrowUp, settings.rowActColor, xpos + SMALL_BUTTON_SIZE() + 2.0f,
                            buttonPanel.printArea.topLeft.y - 1.0f, 0);
                        status.panelButtonHighlighted = 21;
                    }
                }
                else {
                    if (mouseState.x < 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size())
                        + (10 * CHAR_WIDTH()) + SMALL_BUTTON_SIZE() + 2.0f) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                            ALLEGRO_ALIGN_LEFT, "Decrease Auto-Increment Step (Alt+PageDown)");
                        al_draw_tinted_bitmap(arrowDown, settings.rowActColor, xpos,
                            buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
                        status.panelButtonHighlighted = 22;
                    }
                    else {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                            ALLEGRO_ALIGN_LEFT, "Increase Auto-Increment Step (Alt+PageUp)");
                        al_draw_tinted_bitmap(arrowUp, settings.rowActColor, xpos + SMALL_BUTTON_SIZE() + 2.0f,
                            buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f, 0);
                        status.panelButtonHighlighted = 23;
                    }
                }
            }
            else status.menuButtonHighlighted = -1;
        }

        //block/reflist panel
        else if (mouseState.y > static_cast<int>(settings.blockHeaderArea.topLeft.y)
            && mouseState.y < static_cast<int>(settings.blockHeaderArea.bottomRight.y)
            && status.currentTab != 0) {

            status.panelButtonHighlighted = -1;

            if (mouseState.x < static_cast<int>(settings.blockNameArea.topLeft.x)) {

                if (status.get_current_blocktype() == 0)
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Display Previous Unique Pattern in Sequence (Alt+Left)");
                else al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Display Previous Block (Alt+Left)");

                al_draw_tinted_bitmap(arrowLeft, settings.rowActColor, blockPanel.printArea.topLeft.x,
                    settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                status.panelButtonHighlighted = 0;
            }
            else if (mouseState.x >= static_cast<int>(settings.blockNameArea.topLeft.x)
                && mouseState.x <= static_cast<int>(settings.blockNameArea.bottomRight.x + DEFAULT_MARGIN()))
                al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                    ALLEGRO_ALIGN_LEFT, "Rename Block (Alt+R)");
            else if (mouseState.x <= static_cast<int>(settings.blockNameArea.bottomRight.x + SMALL_BUTTON_SIZE()
                + 2 * DEFAULT_MARGIN())) {

                if (status.get_current_blocktype() == 0)
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Display Next Unique Pattern in Sequence (Alt+Right)");
                else al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Display Next Block (Alt+Right)");

                al_draw_tinted_bitmap(arrowRight, settings.rowActColor,
                    blockPanel.printArea.topLeft.x + SMALL_BUTTON_SIZE() + 12 * CHAR_WIDTH() + 3 * DEFAULT_MARGIN(),
                    settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                status.panelButtonHighlighted = 1;
            }
            else if (mouseState.x > static_cast<int>(settings.blockLengthArea.bottomRight.x)
                && mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x)) {

                if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Add Row (Alt++)");
                    al_draw_tinted_bitmap(buttonPlus, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 5 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 2;
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 2) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                    ALLEGRO_ALIGN_LEFT, "Remove Last Row (Alt+-)");
                    al_draw_tinted_bitmap(buttonMinus, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 3 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 6 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 3;
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 3) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Insert Row (Alt+V)");
                    al_draw_tinted_bitmap(buttonInsert, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 4 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 7 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 4;
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 4) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Delete Row (Alt+Del)");
                    al_draw_tinted_bitmap(buttonRemove, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 5 * SMALL_BUTTON_SIZE() + 22 * CHAR_WIDTH() + 8 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 5;
                }
                //TODO temporary hard-coded offset for expand factor area
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 5 + 24) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Expand (Alt+X)");
                    al_draw_tinted_bitmap(buttonExpand, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 6 * SMALL_BUTTON_SIZE() + 24 * CHAR_WIDTH() + 10 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 6;
                }

                //TODO temporary hard-coded offset for expand factor area
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 6 + 24) {
                    al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x, messagePanel.printArea.topLeft.y,
                        ALLEGRO_ALIGN_LEFT, "Shrink (Alt+Shift+X)");
                    al_draw_tinted_bitmap(buttonShrink, settings.rowActColor,
                        blockPanel.printArea.topLeft.x + 7 * SMALL_BUTTON_SIZE() + 24 * CHAR_WIDTH() + 11 * DEFAULT_MARGIN(),
                        settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                    status.panelButtonHighlighted = 7;
                }
            }

            else if (mouseState.x > static_cast<int>(settings.referencePanelArea.topLeft.x)) {

                if (status.get_current_blocktype() == 0) {
                    //sequence buttons
                    if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Add Step (Alt++)");
                        al_draw_tinted_bitmap(buttonPlus, settings.rowActColor, referencePanel.printArea.topLeft.x,
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 10;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 2) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Insert Step (Ins)");
                        al_draw_tinted_bitmap(buttonInsert, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 1 * SMALL_BUTTON_SIZE() + 1 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 11;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 3) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Delete Step (Del)");
                        al_draw_tinted_bitmap(buttonRemove, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 2 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 12;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 4) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Clone Pattern (Alt+C)");
                        al_draw_tinted_bitmap(buttonClone, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 3 * SMALL_BUTTON_SIZE() + 3 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 13;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 5) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Set Loop Point (Alt+L)");
                        al_draw_tinted_bitmap(buttonLooppoint, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 4 * SMALL_BUTTON_SIZE() + 4 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 14;
                    }
//                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
//                        + SMALL_BUTTON_WIDTH * 6) {
//                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
//                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Set Loop Point (Alt+L)");
//                        al_draw_tinted_bitmap(buttonLooppoint, settings.rowActColor,
//                            referencePanel.printArea.topLeft.x + 5 * SMALL_BUTTON_SIZE() + 5 * DEFAULT_MARGIN(),
//                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
//                        status.panelButtonHighlighted = 15;
//                    }
                }
                else {
                    //reference list buttons
                    if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Add Block (Alt++)");
                        al_draw_tinted_bitmap(buttonPlus, settings.rowActColor, referencePanel.printArea.topLeft.x,
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 10;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 2) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Delete Block (Del)");
                        al_draw_tinted_bitmap(buttonRemove, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 1 * SMALL_BUTTON_SIZE() + 1 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 11;
                    }
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 3) {
                        al_draw_text(font, settings.sysColor, messagePanel.printArea.topLeft.x,
                            messagePanel.printArea.topLeft.y, ALLEGRO_ALIGN_LEFT, "Clone Block (Alt+C)");
                        al_draw_tinted_bitmap(buttonClone, settings.rowActColor,
                            referencePanel.printArea.topLeft.x + 2 * SMALL_BUTTON_SIZE() + 2 * DEFAULT_MARGIN(),
                            settings.tabArea.bottomRight.y + DEFAULT_MARGIN(), 0);
                        status.panelButtonHighlighted = 12;
                    }
                }
            }
        }

        return;
    }

    if (mouseState.y < static_cast<int>(BUTTON_PANEL_HEIGHT())) {

        if (!status.leftMouseButtonDown) {

            if (mouseState.x < 2 + MENU_BUTTON_SIZE) {

                //dirty workaround for Allegro popup menu GTK bug
                ALLEGRO_EVENT_QUEUE *tQ;
                ALLEGRO_EVENT tE;
                tQ = al_create_event_queue();
                al_register_event_source(tQ, al_get_mouse_event_source());
                al_wait_for_event(tQ, &tE);
                while (tE.type != ALLEGRO_EVENT_MOUSE_BUTTON_UP) al_wait_for_event(tQ, &tE);
                if (tE.mouse.button == 1) al_popup_menu(newFilePopupMenu, mainDisplay);
                al_destroy_event_queue(tQ);
            }
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 2) open_file();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 3) save_file();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 4) save_file_as();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 5) play_from_start();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 6) play_from_current_pos();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 7) play_pattern();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 8) soundEmul.stop();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 9) undo();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 10) redo();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 11) copy_selection();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 12) cut_selection();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 13) delete_selection();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 14) paste_selection();
            else if (mouseState.x < 2 + MENU_BUTTON_SIZE * 15) insert_selection();
            else if (mouseState.x > static_cast<int>(2 * DEFAULT_MARGIN()
                + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH()))
                && mouseState.x < static_cast<int>(3 * DEFAULT_MARGIN()
                + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size()) + (10 * CHAR_WIDTH())
                + 2 * SMALL_BUTTON_SIZE())) {

                if (mouseState.y < static_cast<int>(buttonPanel.printArea.topLeft.y + SMALL_BUTTON_SIZE() + 1.0f)) {
                    if (mouseState.x < 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size())
                        + (10 * CHAR_WIDTH()) + SMALL_BUTTON_SIZE() + 2.0f) {
                        status.currentOctave = (status.currentOctave == 0) ? 0 : status.currentOctave - 1;
                        print_base_octave();
                    }
                    else {
                        status.currentOctave = (status.currentOctave == 6) ? 6 : status.currentOctave + 1;
                        print_base_octave();
                    }
                }
                else {
                    if (mouseState.x < 2 * DEFAULT_MARGIN() + ((BUTTON_SIZE() + 2 * DEFAULT_MARGIN()) * menuButtons.size())
                        + (10 * CHAR_WIDTH()) + SMALL_BUTTON_SIZE() + 2.0f) {
                        status.autoInc = (status.autoInc - 1) & 0x3f;
                        print_autoinc_setting();
                    }
                    else {
                        status.autoInc = (status.autoInc + 1) & 0x3f;
                        print_autoinc_setting();
                    }
                }
            }
            status.leftMouseButtonDown = true;
        }
    }

    //block panel vertical scrollbar
    else if (status.mouseDragLockV || (mouseState.y < static_cast<int>(blockPanel.totalArea.bottomRight.y)
        && mouseState.x >= static_cast<int>(blockPanel.totalArea.bottomRight.x - SCROLL_PANE_SIZE())
        && mouseState.x <= static_cast<int>(blockPanel.totalArea.bottomRight.x))) {

        if (!status.focusBlock) {
            cancel_data_input();
            status.cancel_selection();
            status.focusBlock = true;
        }

        //top arrow button
        if (mouseState.y <= static_cast<int>(BUTTON_PANEL_HEIGHT() + SMALL_BUTTON_SIZE())) scroll_up();

        //scroll pane
        else if (mouseState.y < static_cast<int>(blockPanel.totalArea.bottomRight.y - SMALL_BUTTON_SIZE())) {

//            cout << "scrollpane clicked\n";
            if (status.leftMouseButtonDown) scroll_drag_v(mouseState.y);
            else {
                status.mouseDragBeginY = static_cast<float>(mouseState.y);
                status.leftMouseButtonDown = true;
                status.mouseDragLockV = true;
            }
        }
        //bottom arrow button
        else scroll_down();
    }
    //reference panel vertical scrollbar
    else if (status.mouseDragLockRefV || (mouseState.y < static_cast<int>(blockPanel.totalArea.bottomRight.y)
        && mouseState.x >= static_cast<int>(referencePanel.totalArea.bottomRight.x - SCROLL_PANE_SIZE()))) {

        if (status.focusBlock) {
            cancel_data_input();
            status.cancel_selection();
            status.focusBlock = false;
        }

        //top arrow button
        if (mouseState.y <= static_cast<int>(BUTTON_PANEL_HEIGHT() + SMALL_BUTTON_SIZE())) scroll_up();
        //scroll pane
        else if (mouseState.y < static_cast<int>(referencePanel.totalArea.bottomRight.y - SMALL_BUTTON_SIZE())) {

            if (status.leftMouseButtonDown) scroll_drag_v(mouseState.y);
            else {
                status.mouseDragBeginY = static_cast<float>(mouseState.y);
                status.leftMouseButtonDown = true;
                status.mouseDragLockRefV = true;
            }
        }
        //bottom arrow button
        else scroll_down();
    }
    //block panel horizontal scrollbar
    else if (status.mouseDragLockH || (mouseState.y >= static_cast<int>(blockPanel.totalArea.bottomRight.y - SCROLL_PANE_SIZE())
        && mouseState.y <= static_cast<int>(blockPanel.totalArea.bottomRight.y)
        && mouseState.x < static_cast<int>(blockPanel.totalArea.bottomRight.x - SCROLL_PANE_SIZE()))) {

        if (!status.focusBlock) {
            cancel_data_input();
            status.cancel_selection();
            status.focusBlock = true;
        }

        //left button
        if (mouseState.x <= static_cast<int>(SMALL_BUTTON_SIZE())) scroll_left();
        //scroll pane
        else if (mouseState.x < static_cast<int>(blockPanel.totalArea.bottomRight.x - 2 * SMALL_BUTTON_SIZE())) {

            if (status.leftMouseButtonDown) scroll_drag_h(mouseState.x);
            else {
                status.mouseDragBeginX = static_cast<float>(mouseState.x);
                status.leftMouseButtonDown = true;
                status.mouseDragLockH = true;
            }
        }
        //right button
        else scroll_right();
    }

    //tab panel
    else if (mouseState.y < static_cast<int>(settings.blockHeaderArea.topLeft.y)) {

        if (!status.leftMouseButtonDown) {

            if (status.editLock) cancel_data_input();
            status.cancel_selection();

            if (mouseState.x >= static_cast<int>(settings.referencePanelArea.topLeft.x)) {

                if (status.currentTab == 0) return;
                status.focusBlock = false;
                print_block_data();
                print_reference_data();
            }
            else {
                status.focusBlock = true;
                unsigned tab;
                for (tab = 0; tab < tabPanel.size()
                    && mouseState.x > static_cast<int>(tabPanel[tab].xpos + tabPanel[tab].xsize); tab++) {}

                if (tab < tabPanel.size()) status.currentTab = tab;

                print_block_tabs();
                if (status.currentTab != 0) print_reference_data();
            }

            status.leftMouseButtonDown = true;
        }
    }

    //block header/references header
    else if (mouseState.y <= static_cast<int>(settings.blockHeaderArea.bottomRight.y)) {

        if (status.currentTab == 0) return;

        if (!status.leftMouseButtonDown) {

            if (mouseState.x < static_cast<int>(settings.blockNameArea.topLeft.x)) select_prev_block();
            else if (mouseState.x >= static_cast<int>(settings.blockNameArea.topLeft.x)
                && mouseState.x <= static_cast<int>(settings.blockNameArea.bottomRight.x + DEFAULT_MARGIN()))
                rename_block();
            else if (mouseState.x <= static_cast<int>(settings.blockNameArea.bottomRight.x + SMALL_BUTTON_SIZE()
                + 2 * DEFAULT_MARGIN()))
                select_next_block();
            else if (mouseState.x > static_cast<int>(settings.blockLengthArea.bottomRight.x)
                && mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x)) {

                if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH) {
                    if (!status.focusBlock) {
                        cancel_data_input();
                        status.cancel_selection();
                        status.focusBlock = true;
                        print_reference_data();
                    }
                    add_row();
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 2) {
                    if (!status.focusBlock) {
                        cancel_data_input();
                        status.cancel_selection();
                        status.focusBlock = true;
                        print_reference_data();
                    }
                    remove_row();
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 3) {
                    if (!status.focusBlock) {
                        cancel_data_input();
                        status.cancel_selection();
                        status.focusBlock = true;
                        print_reference_data();
                    }
                    insert_row();
                }
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 4) {
                    if (!status.focusBlock) {
                        cancel_data_input();
                        status.cancel_selection();
                        status.focusBlock = true;
                        print_reference_data();
                    }
                    delete_row();
                }
                //TODO temporary hard-coded offset for expand factor area
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 5 + 24) {
                    expand_block();
                }
                //TODO temporary hard-coded offset for expand factor area
                else if (mouseState.x < static_cast<int>(settings.blockLengthArea.bottomRight.x
                    + DEFAULT_MARGIN()) + SMALL_BUTTON_WIDTH * 6 + 24) {
                    shrink_block();
                }


            }

            else if (mouseState.x > static_cast<int>(settings.referencePanelArea.topLeft.x)) {

                if (status.focusBlock) {

                    cancel_data_input();
                    status.cancel_selection();
                    status.focusBlock = false;
                    print_block_data();
                }

                if (status.get_current_blocktype() == 0) {
                    //sequence buttons
                    if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH)
                        add_row();
//                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
//                        + SMALL_BUTTON_WIDTH * 2)
//                        remove_row();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 2)
                        insert_row();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 3)
                        delete_row();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 4)
                        clone_block();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 5)
                        set_loop_point();
                }
                else {
                    //reference list buttons
                    if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH)
                        add_row();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 2)
                        delete_row();
                    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x + DEFAULT_MARGIN())
                        + SMALL_BUTTON_WIDTH * 3)
                        clone_block();
                }
            }
        }

        status.leftMouseButtonDown = true;
    }

    //block data/reference data
    else if (mouseState.x < static_cast<int>(settings.referencePanelArea.topLeft.x)) {

        if (!status.leftMouseButtonDown) {

            cancel_data_input();
            status.cancel_selection();

            if (!status.focusBlock) {

                status.focusBlock = true;
                print_block_data();
                print_reference_data();
            }

            if (status.currentTab == 0) {

                if (status.leftMouseButtonDown || currentTune.globalConstants.size() == 1) return;

                unsigned row = (mouseState.y - static_cast<int>(settings.blockDataArea.topLeft.y - 0.5f))
                    / static_cast<int>(CHAR_HEIGHT());

                if (row >= currentTune.globalConstants.size()) return;

                status.currentGlobal = row;
                print_globals();
            }
            else {
                unsigned row = (mouseState.y - static_cast<int>(settings.blockDataArea.topLeft.y - 0.5f))
                    / static_cast<int>(CHAR_HEIGHT()) + status.get_visible_first_row();

                unsigned col = status.get_visible_first_column();
                float colpos = settings.columnHeaderArea.topLeft.x;
                //TODO first check is redundant?
                while (col < status.get_current_block_pointer()->columns.size() && col < status.get_visible_last_column()
                    && mouseState.x > static_cast<int>(colpos
                    + status.get_current_block_pointer()->columns[col].width + CHAR_WIDTH())) {

                    colpos += (status.get_current_block_pointer()->columns[col].width + CHAR_WIDTH());
                    col++;
                }

//                if (row < status.get_current_block_pointer()->columns[0].columnData.size() - status.get_visible_first_row()) {
                if (row < status.get_current_block_pointer()->columns[0].columnData.size()) {
                    status.currentCsrRows[status.get_current_blocktype()] = row;
                    status.currentCsrColumns[status.get_current_blocktype()] = col;
                    print_block_data();
                }
            }

            status.leftMouseButtonDown = true;
        }

        else {

            unsigned row = (mouseState.y - static_cast<int>(settings.blockDataArea.topLeft.y - 0.5f))
                / static_cast<int>(CHAR_HEIGHT()) + status.get_visible_first_row();

            unsigned col = status.get_visible_first_column();
            float colpos = settings.columnHeaderArea.topLeft.x;
            while (col < status.get_current_block_pointer()->columns.size() && col < status.get_visible_last_column()
                && mouseState.x > static_cast<int>(colpos
                + status.get_current_block_pointer()->columns[col].width + CHAR_WIDTH())) {

                colpos += (status.get_current_block_pointer()->columns[col].width + CHAR_WIDTH());
                col++;
            }

//            if (row < status.get_current_block_pointer()->columns[0].columnData.size() - status.get_visible_first_row()) {
            if (row < status.get_current_block_pointer()->columns[0].columnData.size()) {
                if (row != status.currentCsrRows[status.get_current_blocktype()]
                    || col != status.currentCsrColumns[status.get_current_blocktype()]) {

                    status.start_selection();
                    status.currentCsrRows[status.get_current_blocktype()] = row;
                    status.currentCsrColumns[status.get_current_blocktype()] = col;
                    status.update_selection();
                    print_block_data();
                }
            }
        }

        if (status.get_current_blocktype() == 0)
            currentTune.update_last_used(status.get_current_block(), status.get_current_cursor_row());
    }

    else {
        //click in reference area
        if (status.currentTab == 0) return;

        if (!status.leftMouseButtonDown) {

            cancel_data_input();
            status.cancel_selection();

            if (status.focusBlock) {

                status.focusBlock = false;
                print_block_data();
                print_reference_data();
            }

            //mouse y-pos to reference_row
            unsigned row = (mouseState.y - static_cast<int>(settings.referenceDataArea.topLeft.y - 0.5f))
                / static_cast<int>(CHAR_HEIGHT()) + status.get_visible_first_reference_row();

            if (row < ((status.get_current_blocktype() == 0)
                ? currentTune.sequence.size() : currentTune.blockTypes[status.get_current_blocktype()].blocks.size())) {

                status.currentReferenceRows[status.get_current_blocktype()] = row;

                status.set_current_block_from_cursor();
                status.focusBlock = false;
                print_reference_data();
                print_block_data();
            }
            status.leftMouseButtonDown = true;
        }

        else {
            //TODO scroll block area when status.focusBlock
            //TODO scroll down if mouseState.y > referenceDataArea.bottomRight.y
            if (status.focusBlock || status.get_current_blocktype() != 0) return;

            unsigned row = (mouseState.y - static_cast<int>(settings.referenceDataArea.topLeft.y - 0.5f))
                / static_cast<int>(CHAR_HEIGHT()) + status.get_visible_first_reference_row();

            if (row < ((status.get_current_blocktype() == 0)
                ? currentTune.sequence.size() : currentTune.blockTypes[status.get_current_blocktype()].blocks.size())) {

                if (row != status.get_current_reference_row()) {

                    status.start_selection();
                    status.currentReferenceRows[status.get_current_blocktype()] = row;
                    status.update_selection();
                    status.set_current_block_from_cursor();
                    status.focusBlock = false;
                    print_reference_data();
                    print_block_data();
                }
            }
        }
    }
}
