#ifndef _Corpus_h_
#define _Corpus_h_
/* Corpus.h
 *
 * Copyright (C) 2011,2015,2016,2018,2026 Paul Boersma
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

#include "Table.h"

#include "Corpus_def.h"

autoCorpus Corpus_create (conststring32 folderWithSoundFiles, conststring32 soundFileExtension,
	conststring32 folderWithAnnotationFiles, conststring32 annotationFileExtension);

autoCorpus Corpus_readFromCGN (conststring32 rootFolderPath);

#endif
/* End of file Corpus.h */
