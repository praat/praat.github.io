#ifndef _praat_TextGrid_init_h_
#define _praat_TextGrid_init_h_
/* praat_TextGrid_init.h
 *
 * Copyright (C) 2016,2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

/*
	Interface functions for subclasses of TextGrid and Corpus.
*/

#include "TextGrid.h"
#include "praatM.h"

extern "C" Graphics Movie_create (conststring32 title, int width, int height);

/*
	Action buttons.
*/
void praat_TextGrid_init ();

void praat_init_Corpus (ClassInfo klas);

/* End of file praat_TextGrid_init.h */
#endif
