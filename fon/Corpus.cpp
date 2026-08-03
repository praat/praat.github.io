/* Corpus.cpp
 *
 * Copyright (C) 2011,2016,2018,2020,2021,2026 Paul Boersma
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

#include "Corpus.h"
#include "TextGrid_Sound.h"

#include "oo_DESTROY.h"
#include "Corpus_def.h"
#include "oo_COPY.h"
#include "Corpus_def.h"
#include "oo_EQUAL.h"
#include "Corpus_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Corpus_def.h"
#include "oo_WRITE_TEXT.h"
#include "Corpus_def.h"
#include "oo_READ_TEXT.h"
#include "Corpus_def.h"
#include "oo_WRITE_BINARY.h"
#include "Corpus_def.h"
#include "oo_READ_BINARY.h"
#include "Corpus_def.h"
#include "oo_DESCRIPTION.h"
#include "Corpus_def.h"

Thing_implement (Corpus, Table, 1);

autoCorpus Corpus_create (conststring32 folderWithSoundFiles, conststring32 soundFileExtension,
	conststring32 folderWithAnnotationFiles, conststring32 annotationFileExtension)
{
	autoCorpus me = Thing_new (Corpus);
	my folderWithSoundFiles = Melder_dup (folderWithSoundFiles);
	if (folderWithAnnotationFiles [0] == U'\0')
		folderWithAnnotationFiles = folderWithSoundFiles;
	my folderWithAnnotationFiles = Melder_dup (folderWithAnnotationFiles);
	autoSTRVEC fileList = fileNames_STRVEC (Melder_cat (folderWithSoundFiles, U"/*.", soundFileExtension));
	Table_initWithColumnNames (me.get(), fileList.size,
			autoSTRVEC ({ U"Sound", U"Annotation" }).get());
	autoMelderString annotationFileName;
	for (integer ifile = 1; ifile <= fileList.size; ifile ++) {
		conststring32 soundFileName = fileList [ifile].get();
		Table_setStringValue (me.get(), ifile, 1, soundFileName);
		const char32 *dotLocation = str32rchr (soundFileName, U'.');
		Melder_assert (!! dotLocation);
		MelderString_ncopy (& annotationFileName, soundFileName, dotLocation - soundFileName + 1);
		MelderString_append (& annotationFileName, annotationFileExtension);
		structMelderFile annotationFile { };
		Melder_pathToFile (Melder_cat (folderWithAnnotationFiles, U"/", annotationFileName.string), & annotationFile);
		if (MelderFile_exists (& annotationFile))
			Table_setStringValue (me.get(), ifile, 2, annotationFileName.string);
	}
	return me;
}

autoCorpus Corpus_readFromCGN (conststring32 rootFolderPath) {
	autoCorpus me = Thing_new (Corpus);
	const conststring32 columnNames_array [] = { U"comp", U"region", U"data" };
	Table_initWithColumnNames (me.get(), 0, ARRAY_TO_STRVEC (columnNames_array));

	structMelderFolder rootFolder { };
	Melder_relativePathToFolder (rootFolderPath, & rootFolder);
	Melder_require (MelderFolder_exists (& rootFolder),
		U"CGN folder ", MelderFolder_messageName (& rootFolder), U" does not exist.");

	autoSTRVEC compFolderNames = folderNames_STRVEC (Melder_cat (MelderFolder_peekPath (& rootFolder), U"/comp-*"));
	Melder_require (compFolderNames.size > 0,
		U"The folder ", MelderFolder_messageName (& rootFolder), U" contains no folders whose names start with “comp-”.");

	const conststring32 regionNames_array [] = { U"vl", U"nl" };
	const constSTRVEC regionNames = ARRAY_TO_STRVEC (regionNames_array);
	for (integer icomp = 1; icomp <= compFolderNames.size; icomp ++) {
		structMelderFolder compFolder { };
		MelderFolder_getSubfolder (& rootFolder, compFolderNames [icomp].get(), & compFolder);
		for (integer iregion = 1; iregion <= regionNames.size; iregion ++) {
			structMelderFolder regionFolder { };
			MelderFolder_getSubfolder (& compFolder, regionNames [iregion], & regionFolder);
			if (MelderFolder_exists (& regionFolder)) {   // allow partial corpora
				autoSTRVEC soundFileNames = fileNames_STRVEC (Melder_cat (MelderFolder_peekPath (& regionFolder), U"/*.wav"));
				const conststring32 brokenFiles [] = {
					U"fn000483.wav",   // zeroes,
					U"fv701103.wav"    // missing ort
				};
				for (integer ifile = 1; ifile <= soundFileNames.size; ifile ++) if (! NUMfindFirst (ARRAY_TO_STRVEC (brokenFiles), soundFileNames [ifile].get())) {
					TRACE
					trace (U"Reading file ", soundFileNames [ifile].get());
					autoTextGrid textGrid;
					autoSound sound;
					try {
						textGrid = TextGrid_Sound_readFromCorpusGesprokenNederlands (Melder_cat (MelderFolder_peekPath (& regionFolder), U"/", soundFileNames [ifile].get()), nullptr);
						Table_appendRow (me.get());
						Table_setStringValue (me.get(), my rows.size, 1, compFolderNames [icomp].get());
						Table_setStringValue (me.get(), my rows.size, 2, regionNames [iregion]);
						Table_setStringValue (me.get(), my rows.size, 3, soundFileNames [ifile].get());
						my textGrids. addItem_move (textGrid.move());
					} catch (MelderError) {
						Melder_clearError ();
						trace (U"Error handling file ", soundFileNames [ifile].get());
					}
				}
			}
		}
	}
	return me;
}

/* End of file Corpus.cpp */
