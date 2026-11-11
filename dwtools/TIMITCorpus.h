#ifndef _TIMITCorpus_h_
#define _TIMITCorpus_h_
/* TIMITCorpus.h
 *
 * Copyright (C) 2026 David Weenink
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

#include "TextGrid.h"
#include "Sound.h"

#include "Corpus.h"

Thing_define (TIMITCorpus, Corpus) {
	
	/*
		Get the Sound that belongs to item 'number' in the ordered textgrid collection
		TODO: This method should be in the Corpus data type
	*/
	autoSound extractSound (integer number);
	
};

autoTIMITCorpus TIMITCorpus_create (conststring32 folderWithSoundFiles,
	conststring32 soundFileExtension, conststring32 folderWithAnnotationFiles,
	conststring32 annotationFileExtension
);

autoTIMITCorpus TIMITCorpus_import (conststring32 rootFolderPath);

#endif /* _TIMITCorpus_h_ */
