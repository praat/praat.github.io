/* TableEditor.cpp
 *
 * Copyright (C) 2006-2013,2015-2023,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TableEditor.h"
#include "machine.h"
#include "EditorM.h"

Thing_implement (TableEditor, Editor, 0);

#include "Prefs_define.h"
#include "TableEditor_prefs.h"
#include "Prefs_install.h"
#include "TableEditor_prefs.h"
#include "Prefs_copyToInstance.h"
#include "TableEditor_prefs.h"

#define SIZE_INCHES  80

/********** EDITOR METHODS **********/

void structTableEditor :: v1_info () {
	TableEditor_Parent :: v1_info ();
	MelderInfo_writeLine (U"Table uses text styles: ", our instancePref_useTextStyles());
	//MelderInfo_writeLine (U"Table font size: ", our p_fontSize);
}

static void updateVerticalScrollBar (TableEditor me) {
	GuiScrollBar_set (my verticalScrollBar, undefined, my table() -> rows.size + 1, my topRow, undefined, undefined, undefined);
}

static void updateHorizontalScrollBar (TableEditor me) {
	GuiScrollBar_set (my horizontalScrollBar, undefined, my table() -> numberOfColumns + 1, my leftColumn, undefined, undefined, undefined);
}

void structTableEditor :: v1_dataChanged (Editor /* sender */) {
	Melder_clipRight (& our topRow, our table() -> rows.size);
	Melder_clipRight (& our leftColumn, our table() -> numberOfColumns);
	updateVerticalScrollBar (this);
	updateHorizontalScrollBar (this);
	Graphics_updateWs (our graphics.get());
}

/********** FILE MENU **********/

static void menu_cb_preferences (TableEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"TableEditor settings", nullptr);
		OPTIONMENU (useTextStyles, U"The symbols %#_^ in labels", my default_useTextStyles() + 1)
			OPTION (U"are shown as typed")
			OPTION (U"mean italic/bold/sub/super")
	EDITOR_OK
		SET_OPTION (useTextStyles, my instancePref_useTextStyles() + 1)
	EDITOR_DO
		my setInstancePref_useTextStyles (useTextStyles - 1);
		Graphics_updateWs (my graphics.get());
	EDITOR_END
}

/********** EDIT MENU **********/

#ifndef macintosh
/*
	On macOS, Cut/Copy/Paste are already available in the Praat:Edit menu.
*/
static void menu_cb_CutText (TableEditor me, EDITOR_ARGS) {
	GuiText_cut (my text);
}
static void menu_cb_CopyText (TableEditor me, EDITOR_ARGS) {
	GuiText_copy (my text);
}
static void menu_cb_PasteText (TableEditor me, EDITOR_ARGS) {
	GuiText_paste (my text);
}
static void menu_cb_EraseText (TableEditor me, EDITOR_ARGS) {
	GuiText_remove (my text);
}
#endif

/********** VIEW MENU **********/

/********** HELP MENU **********/

static void menu_cb_TableEditorHelp (TableEditor, EDITOR_ARGS) {
	Melder_help (U"TableEditor");
}

/********** DRAWING AREA **********/

void structTableEditor :: v_draw () {
	constexpr double spacing = 2.0;   // millimetres at both edges
	/*
		We fit 400 rows in 80 inches, which is 14.4 points per row.
	*/
	integer rowmin = our topRow, rowmax = rowmin + 397;
	integer colmin = our leftColumn, colmax = colmin + (kTableEditor_MAXNUM_VISIBLE_COLUMNS - 1);
	Melder_clipLeft (1_integer, & rowmin);
	Melder_clipRight (& rowmax, our table() -> rows.size);
	Melder_clipRight (& colmax, our table() -> numberOfColumns);
	Graphics_clearWs (graphics.get());
	Graphics_setTextAlignment (graphics.get(), Graphics_CENTRE, Graphics_HALF);
	Graphics_setWindow (graphics.get(), 0.0, 1.0, rowmin + 397.5, rowmin - 2.5);
	Graphics_setColour (graphics.get(), Melder_SILVER);
	Graphics_fillRectangle (graphics.get(), 0.0, 1.0, rowmin - 2.5, rowmin - 0.5);
	Graphics_setColour (graphics.get(), Melder_BLACK);
	Graphics_line (graphics.get(), 0.0, rowmin - 0.5, 1.0, rowmin - 0.5);
	Graphics_setWindow (graphics.get(), 0.0, Graphics_dxWCtoMM (graphics.get(), 1.0), rowmin + 397.5, rowmin - 2.5);
	/*
		Determine the width of the column with the row numbers.
	*/
	{// scope
		double columnWidth = Graphics_textWidth (graphics.get(), U"row");
		for (integer irow = rowmin; irow <= rowmax; irow ++) {
			const double cellWidth = Graphics_textWidth (graphics.get(), Melder_integer (irow));
			if (cellWidth > columnWidth)
				columnWidth = cellWidth;
		}
		columnLeft [0] = columnWidth + 2 * spacing;
	}
	Graphics_setColour (graphics.get(), Melder_SILVER);
	Graphics_fillRectangle (graphics.get(), 0.0, columnLeft [0], rowmin - 0.5, rowmin + 397.5);
	Graphics_setColour (graphics.get(), Melder_BLACK);
	Graphics_line (graphics.get(), columnLeft [0], rowmin - 0.5, columnLeft [0], rowmin + 397.5);
	/*
		Determine the widths of the columns.
	*/
	for (integer icol = colmin; icol <= colmax; icol ++) {
		conststring32 columnLabel = our table() -> columnHeaders [icol]. label.get();
		double columnWidth = Graphics_textWidth (graphics.get(), Melder_integer (icol));
		{// scope
			if (! columnLabel)
				columnLabel = U"";
			const double cellWidth = Graphics_textWidth (graphics.get(), columnLabel);
			if (cellWidth > columnWidth)
				columnWidth = cellWidth;
		}
		for (integer irow = rowmin; irow <= rowmax; irow ++) {
			conststring32 cell = Table_getStringValue_a (our table(), irow, icol);
			Melder_assert (cell);
			if (cell [0] == U'\0')
				cell = U"?";
			const double cellWidth = Graphics_textWidth (graphics.get(), cell);
			if (cellWidth > columnWidth)
				columnWidth = cellWidth;
		}
		our columnRight [icol - colmin] = columnLeft [icol - colmin] + columnWidth + 2 * spacing;
		if (icol < colmax)
			our columnLeft [icol - colmin + 1] = our columnRight [icol - colmin];
	}
	/*
		Text can be "graphic" or not.
	*/
	Graphics_setPercentSignIsItalic (our graphics.get(), our instancePref_useTextStyles());
	Graphics_setNumberSignIsBold (our graphics.get(), our instancePref_useTextStyles());
	Graphics_setCircumflexIsSuperscript (our graphics.get(), our instancePref_useTextStyles());
	Graphics_setUnderscoreIsSubscript (our graphics.get(), our instancePref_useTextStyles());
	/*
		Show the row numbers.
	*/
	Graphics_text (graphics.get(), our columnLeft [0] / 2, rowmin - 1, U"row");
	for (integer irow = rowmin; irow <= rowmax; irow ++)
		Graphics_text (graphics.get(), our columnLeft [0] / 2, irow, irow);
	/*
		Show the column labels.
	*/
	for (integer icol = colmin; icol <= colmax; icol ++) {
		const double mid = (our columnLeft [icol - colmin] + our columnRight [icol - colmin]) / 2;
		conststring32 columnLabel = our table() -> columnHeaders [icol]. label.get();
		if (! columnLabel || columnLabel [0] == U'\0')
			columnLabel = U"?";
		Graphics_text (graphics.get(), mid, rowmin - 2, icol);
		Graphics_text (graphics.get(), mid, rowmin - 1, columnLabel);
	}
	/*
		Show the cell contents.
	*/
	for (integer irow = rowmin; irow <= rowmax; irow ++) {
		for (integer icol = colmin; icol <= colmax; icol ++) {
			if (irow == selectedRow && icol == selectedColumn) {
				Graphics_setColour (graphics.get(), Melder_YELLOW);
				const double dx = Graphics_dxMMtoWC (graphics.get(), 0.3);
				Graphics_fillRectangle (graphics.get(),
						our columnLeft [icol - colmin] + dx, columnRight [icol - colmin] - dx, irow - 0.45, irow + 0.55);
				Graphics_setColour (graphics.get(), Melder_BLACK);
			}
			const double mid = (our columnLeft [icol - colmin] + our columnRight [icol - colmin]) / 2.0;
			conststring32 cell = Table_getStringValue_a (our table(), irow, icol);
			Melder_assert (cell);
			if (cell [0] == U'\0')
				cell = U"?";
			Graphics_text (graphics.get(), mid, irow, cell);
		}
	}
}

bool structTableEditor :: v_clickCell (integer row, integer column, bool /* shiftKeyPressed */) {
	our selectedRow = row;
	our selectedColumn = column;
	return true;
}

static void gui_text_cb_changed (TableEditor me, GuiTextEvent /* event */) {
	Editor_broadcastDataChanged (me);
}

static void gui_drawingarea_cb_expose (TableEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics)
		return;
	my v_draw ();
}

static void gui_drawingarea_cb_mouse (TableEditor me, GuiDrawingArea_MouseEvent event) {
	if (! my graphics)
		return;   // could be the case in the very beginning
	if (! event -> isClick())
		return;
	integer rowmin = my topRow, rowmax = rowmin + 397;
	integer colmin = my leftColumn, colmax = colmin + (kTableEditor_MAXNUM_VISIBLE_COLUMNS - 1);
	Melder_clipLeft (1_integer, & rowmin);
	Melder_clipRight (& rowmax, my table() -> rows.size);
	Melder_clipRight (& colmax, my table() -> numberOfColumns);
	double xWC, yWC;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);
	if (yWC < rowmin - 0.45 || yWC > rowmax + 0.55)
		return;
	for (integer icol = colmin; icol <= colmax; icol ++) {
		if (xWC > my columnLeft [icol - colmin] && xWC < my columnRight [icol - colmin]) {
			const integer selectedRow = Melder_iround (yWC);
			const integer selectedColumn = icol;
			if (my v_clickCell (selectedRow, selectedColumn, event -> shiftKeyPressed))
				Graphics_updateWs (my graphics.get());
			return;
		}
	}
}

static void gui_drawingarea_cb_resize (TableEditor me, GuiDrawingArea_ResizeEvent /* event */) {
	if (! my graphics)
		return;
	Graphics_updateWs (my graphics.get());
}

static void gui_cb_scrollHorizontal (TableEditor me, GuiScrollBarEvent event) {
	const integer value = GuiScrollBar_getValue (event -> scrollBar);
	if (value != my leftColumn) {
		my leftColumn = value;
		Graphics_updateWs (my graphics.get());   // wait for expose event
	}
}

static void gui_cb_scrollVertical (TableEditor me, GuiScrollBarEvent event) {
	const integer value = GuiScrollBar_getValue (event -> scrollBar);
	if (value != my topRow) {
		my topRow = value;
		Graphics_updateWs (my graphics.get());   // wait for expose event
	}
}

void structTableEditor :: v_createChildren () {
	int y = Machine_getMenuBarBottom () + 4, scrollWidth = Machine_getScrollBarWidth ();

	our text = GuiText_createShown (our windowForm, 0, 0, y, y + Machine_getTextHeight (), 0);
	GuiText_setChangedCallback (our text, gui_text_cb_changed, this);
	y += Machine_getTextHeight () + 4;

	our drawingArea = GuiDrawingArea_createShown (our windowForm, 0, - scrollWidth, y, - scrollWidth,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse,
		nullptr, gui_drawingarea_cb_resize, nullptr, this, 0
	);

	our verticalScrollBar = GuiScrollBar_createShown (our windowForm, - scrollWidth, 0, y, - scrollWidth,
			1, our table() -> rows.size + 1, 1, 1, 1, 10, gui_cb_scrollVertical, this, 0);

	our horizontalScrollBar = GuiScrollBar_createShown (our windowForm, 0, - scrollWidth, - scrollWidth, 0,
			1, our table() -> numberOfColumns + 1, 1, 1, 1, 3, gui_cb_scrollHorizontal, this, GuiScrollBar_HORIZONTAL);

	GuiDrawingArea_setSwipable (our drawingArea, our horizontalScrollBar, our verticalScrollBar);
}

void structTableEditor :: v_createMenus () {
	TableEditor_Parent :: v_createMenus ();

	Editor_addCommand (this, U"File", U"Preferences...", 0, menu_cb_preferences);
	Editor_addCommand (this, U"File", U"-- before scripting --", 0, nullptr);

	#ifndef macintosh
	Editor_addCommand (this, U"Edit", U"-- cut copy paste text --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Cut text", 'X', menu_cb_CutText);
	Editor_addCommand (this, U"Edit", U"Copy text", 'C', menu_cb_CopyText);
	Editor_addCommand (this, U"Edit", U"Paste text", 'V', menu_cb_PasteText);
	Editor_addCommand (this, U"Edit", U"Erase text", 0, menu_cb_EraseText);
	#endif
}

void structTableEditor :: v_createMenuItems_help (EditorMenu menu) {
	TableEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"TableEditor help", U'?', menu_cb_TableEditorHelp);
}

void TableEditor_init (TableEditor me, conststring32 title, Table table) {
	Editor_init (me, 0, 0, 700, 500, title, table);
	#if motif
		Melder_assert (XtWindow (my drawingArea -> d_widget));
	#endif
	my topRow = 1;
	my leftColumn = 1;
	my selectedColumn = 1;
	my selectedRow = 1;
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	const double size_pixels = SIZE_INCHES * Graphics_getResolution (my graphics.get());
	Graphics_setWsViewport (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
	Graphics_setWsWindow (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
	Graphics_setViewport (my graphics.get(), 0.0, size_pixels, 0.0, size_pixels);
	Graphics_setFont (my graphics.get(), kGraphics_font::COURIER);
	Graphics_setFontSize (my graphics.get(), 12);
	Graphics_setUnderscoreIsSubscript (my graphics.get(), false);
	Graphics_setAtSignIsLink (my graphics.get(), true);
}

autoTableEditor TableEditor_create (conststring32 title, Table table) {
	try {
		autoTableEditor me = Thing_new (TableEditor);
		TableEditor_init (me.get(), title, table);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableEditor not created.");
	}
}

/* End of file TableEditor.cpp */
