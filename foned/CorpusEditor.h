#ifndef _CorpusEditor_h_
#define _CorpusEditor_h_
/* CorpusEditor.h
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

#include "TableEditor.h"
#include "Corpus.h"
#include "Sound.h"

Thing_define (CorpusEditor, TableEditor) {
	Corpus corpus() { return static_cast <Corpus> (our data()); }
	autoSound sound;
	bool v_clickCell (integer row, integer column, bool shiftKeyPressed)
		override;
};

autoCorpusEditor CorpusEditor_create (conststring32 title, Corpus corpus);

/* End of file CorpusEditor.h */
#endif
