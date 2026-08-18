#ifndef _TableEditor_h_
#define _TableEditor_h_
/* TableEditor.h
 *
 * Copyright (C) 2006,2007,2010-2012,2015-2019,2022,2023,2026 Paul Boersma
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

#include "ScriptEditor.h"
#include "Table.h"

#define kTableEditor_MAXNUM_VISIBLE_COLUMNS  100

Thing_define (TableEditor, Editor) {
	Table table() { return static_cast <Table> (our data()); }

	integer topRow, leftColumn, selectedRow, selectedColumn;
	GuiText text;
	GuiDrawingArea drawingArea;
	GuiScrollBar horizontalScrollBar, verticalScrollBar;
	double columnLeft [kTableEditor_MAXNUM_VISIBLE_COLUMNS], columnRight [kTableEditor_MAXNUM_VISIBLE_COLUMNS];
	autoGraphics graphics;

	void v1_info ()
		override;
	void v_createChildren ()
		override;
	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v1_dataChanged (Editor sender)
		override;

	virtual void v_draw ();
	virtual bool v_clickCell (integer row, integer column, bool shiftKeyPressed);

	#include "TableEditor_prefs.h"
};

void TableEditor_init (TableEditor me, conststring32 title, Table table);

autoTableEditor TableEditor_create (conststring32 title, Table table);

/* End of file TableEditor.h */
#endif
