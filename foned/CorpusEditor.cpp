/* CorpusEditor.cpp
 *
 * Copyright (C) 2026 Paul Boersma
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

#include "CorpusEditor.h"
#include "TextGridEditor.h"
#include "praat.h"

Thing_implement (CorpusEditor, TableEditor, 0);

bool structCorpusEditor :: v_clickCell (integer row, integer column, bool shiftKeyPressed) {
	CorpusEditor_Parent :: v_clickCell (row, column, shiftKeyPressed);
	if (our selectedColumn == 3) {
		conststring32 rowTitle = Table_getStringValue_a (our table(), our selectedRow, our selectedColumn);
		structMelderFolder wavFolder { };
		Melder_relativePathToFolder (our corpus() -> folderWithSoundFiles.get(), & wavFolder);
		structMelderFolder compFolder { };
		MelderFolder_getSubfolder (& wavFolder, Table_getStringValue_a (our table(), our selectedRow, 1), & compFolder);
		structMelderFolder regionFolder { };
		MelderFolder_getSubfolder (& compFolder, Table_getStringValue_a (our table(), our selectedRow, 2), & regionFolder);
		structMelderFile soundFile { };
		MelderFolder_getFile (& regionFolder, Table_getStringValue_a (our table(), our selectedRow, 3), & soundFile);

		our sound = Data_readFromFile (& soundFile).static_cast_move <structSound>();
		Melder_require (our sound -> classInfo == classSound,
			U"File ", & soundFile, U" contains a ", Thing_className (our sound.get()), U" instead of a sound.");
		autoTextGridEditor textgridEditor = TextGridEditor_create (rowTitle, our corpus() -> textGrids.at [our selectedRow], our sound.get(), nullptr, nullptr);
		/*
			TODO: the following five lines probably don't belong here.
		*/
		int IOBJECT;
		WHERE (theCurrentPraatObjects -> list [IOBJECT]. object == our corpus())
			break;
		praat_installEditor (textgridEditor.get(), IOBJECT);
		textgridEditor.releaseToUser();
	}
	return true;
}

autoCorpusEditor CorpusEditor_create (conststring32 title, Corpus corpus) {
	try {
		autoCorpusEditor me = Thing_new (CorpusEditor);
		TableEditor_init (me.get(), title, corpus);
		return me;
	} catch (MelderError) {
		Melder_throw (U"CorpusEditor not created.");
	}
}

/* End of file CorpusEditor.cpp */
