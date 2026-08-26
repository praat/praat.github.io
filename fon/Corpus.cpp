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

Thing_implement (Corpus, Daata, 0);

autoCorpus Corpus_create (conststring32 folderWithSoundFiles, conststring32 soundFileExtension,
	conststring32 folderWithAnnotationFiles, conststring32 annotationFileExtension)
{
	autoCorpus me = Thing_new (Corpus);
	my folderWithSoundFiles = Melder_dup (folderWithSoundFiles);
	if (folderWithAnnotationFiles [0] == U'\0')
		folderWithAnnotationFiles = folderWithSoundFiles;
	my folderWithAnnotationFiles = Melder_dup (folderWithAnnotationFiles);
	autoSTRVEC fileList = fileNames_STRVEC (Melder_cat (folderWithSoundFiles, U"/*.", soundFileExtension));
	Table_initWithColumnNames (my recordings.get(), fileList.size,
			autoSTRVEC ({ U"Sound", U"Annotation" }).get());
	autoMelderString annotationFileName;
	for (integer ifile = 1; ifile <= fileList.size; ifile ++) {
		conststring32 soundFileName = fileList [ifile].get();
		Table_setStringValue (my recordings.get(), ifile, 1, soundFileName);
		const char32 *dotLocation = str32rchr (soundFileName, U'.');
		Melder_assert (!! dotLocation);
		MelderString_ncopy (& annotationFileName, soundFileName, dotLocation - soundFileName + 1);
		MelderString_append (& annotationFileName, annotationFileExtension);
		structMelderFile annotationFile { };
		Melder_pathToFile (Melder_cat (folderWithAnnotationFiles, U"/", annotationFileName.string), & annotationFile);
		if (MelderFile_exists (& annotationFile))
			Table_setStringValue (my recordings.get(), ifile, 2, annotationFileName.string);
	}
	return me;
}

autoCorpus Corpus_importFromCGN (conststring32 rootFolderPath) {
	autoCorpus me = Thing_new (Corpus);
	const conststring32 columnNames_array [] = { U"component", U"region", U"recording" };
	my recordings = Table_createWithColumnNames (0, ARRAY_TO_STRVEC (columnNames_array));

	structMelderFolder rootFolder { };
	Melder_relativePathToFolder (rootFolderPath, & rootFolder);
	Melder_require (MelderFolder_exists (& rootFolder),
		U"CGN folder ", & rootFolder, U" does not exist.");

	structMelderFolder dataFolder { };
	MelderFolder_getSubfolder (& rootFolder, U"data", & dataFolder);
	Melder_require (MelderFolder_exists (& dataFolder),
		U"CGN folder ", & dataFolder, U" does not exist.");

	structMelderFolder audioFolder { };
	MelderFolder_getSubfolder (& dataFolder, U"audio", & audioFolder);
	Melder_require (MelderFolder_exists (& audioFolder),
		U"CGN folder ", & audioFolder, U" does not exist.");

	structMelderFolder wavFolder { };
	MelderFolder_getSubfolder (& audioFolder, U"wav", & wavFolder);
	Melder_require (MelderFolder_exists (& wavFolder),
		U"CGN folder ", & wavFolder, U" does not exist.");

	my folderWithSoundFiles = Melder_dup (MelderFolder_peekPath (& wavFolder));

	autoSTRVEC compFolderNames = folderNames_STRVEC (Melder_cat (MelderFolder_peekPath (& wavFolder), U"/comp-*"));
	Melder_require (compFolderNames.size > 0,
		U"The folder ", MelderFolder_messageName (& rootFolder), U" contains no folders whose names start with “comp-”.");

	const conststring32 regionNames_array [] = { U"vl", U"nl" };
	const constSTRVEC regionNames = ARRAY_TO_STRVEC (regionNames_array);
	for (integer icomp = 1; icomp <= compFolderNames.size; icomp ++) {
		structMelderFolder compFolder { };
		MelderFolder_getSubfolder (& wavFolder, compFolderNames [icomp].get(), & compFolder);
		for (integer iregion = 1; iregion <= regionNames.size; iregion ++) {
			structMelderFolder regionFolder { };
			MelderFolder_getSubfolder (& compFolder, regionNames [iregion], & regionFolder);
			if (MelderFolder_exists (& regionFolder)) {   // allow partial corpora
				autoSTRVEC soundFileNames = fileNames_STRVEC (Melder_cat (MelderFolder_peekPath (& regionFolder), U"/*.wav"));
				for (integer ifile = 1; ifile <= soundFileNames.size; ifile ++) {
					//TRACE
					trace (U"Reading file ", soundFileNames [ifile].get());
					autoTextGrid textGrid;
					autoSound sound;
					try {
						structMelderFile soundFile { };
						MelderFolder_getFile (& regionFolder, soundFileNames [ifile].get(), & soundFile);
						textGrid = TextGrid_Sound_readFromCorpusGesprokenNederlands (MelderFile_peekPath (& soundFile), nullptr);
						Table_appendRow (my recordings.get());
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 1, compFolderNames [icomp].get());
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 2, regionNames [iregion]);
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 3, soundFileNames [ifile].get());
						my textGrids. addItem_move (textGrid.move());
					} catch (MelderError) {
						Melder_clearError ();
						trace (U"Error handling file ", soundFileNames [ifile].get());
					}
				}
			}
		}
	}

	structMelderFolder metadataFolder { };
	MelderFolder_getSubfolder (& dataFolder, U"meta", & metadataFolder);
	Melder_require (MelderFolder_exists (& metadataFolder),
		U"CGN folder ", & metadataFolder, U" does not exist.");

	structMelderFolder metadataTextFolder { };
	MelderFolder_getSubfolder (& metadataFolder, U"text", & metadataTextFolder);
	Melder_require (MelderFolder_exists (& metadataTextFolder),
		U"CGN folder ", & metadataTextFolder, U" does not exist.");

	structMelderFile speakersFile { };
	MelderFolder_getFile (& metadataTextFolder, U"speakers.txt", & speakersFile);
	Melder_require (MelderFile_exists (& speakersFile),
		U"CGN file ", & speakersFile, U" does not exist.");

	my speakers = Table_readFromCharacterSeparatedTextFile (& speakersFile, U'\t', false);
	return me;
}

autoTextGrid Corpus_extractTextGrid_number (Corpus me, integer recordingNumber) {
	Melder_require (recordingNumber >= 1,
		U"The recording number (", recordingNumber, U") should be at least 1.");
	Melder_require (recordingNumber <= my recordings -> rows.size,
		U"The recording number (", recordingNumber, U") should be at most the number of recordings (", my recordings -> rows.size, U").");
	autoTextGrid thee = Data_copy (my textGrids.at [recordingNumber]);
	Thing_setName (thee.get(), Table_getStringValue_a (my recordings.get(), recordingNumber, 3));
	return thee;
}

autoSound Corpus_extractSound_number (Corpus me, integer recordingNumber) {
	Melder_require (recordingNumber >= 1,
		U"The recording number (", recordingNumber, U") should be at least 1.");
	Melder_require (recordingNumber <= my recordings -> rows.size,
		U"The recording number (", recordingNumber, U") should be at most the number of recordings (", my recordings -> rows.size, U").");
	structMelderFolder wavFolder { };
	Melder_relativePathToFolder (my folderWithSoundFiles.get(), & wavFolder);
	structMelderFolder compFolder { };
	MelderFolder_getSubfolder (& wavFolder, Table_getStringValue_a (my recordings.get(), recordingNumber, 1), & compFolder);
	structMelderFolder countryFolder { };
	MelderFolder_getSubfolder (& compFolder, Table_getStringValue_a (my recordings.get(), recordingNumber, 2), & countryFolder);
	structMelderFile soundFile { };
	MelderFolder_getFile (& countryFolder, Table_getStringValue_a (my recordings.get(), recordingNumber, 3), & soundFile);
	autoSound thee = Sound_readFromSoundFile (& soundFile);
	Thing_setName (thee.get(), Table_getStringValue_a (my recordings.get(), recordingNumber, 3));
	return thee;
}

autoCorpus Corpus_extractComponents (Corpus me,
	bool comp_a, bool comp_b, bool comp_c, bool comp_d, bool comp_e,
	bool comp_f, bool comp_g, bool comp_h, bool comp_i, bool comp_j,
	bool comp_k, bool comp_l, bool comp_m, bool comp_n, bool comp_o
) {
	bool components [15] {
		comp_a, comp_b, comp_c, comp_d, comp_e,
		comp_f, comp_g, comp_h, comp_i, comp_j,
		comp_k, comp_l, comp_m, comp_n, comp_o
	};
	autoCorpus thee = Data_copy (me);
	for (integer row = thy recordings -> rows.size; row >= 1; row --) {
		conststring32 component = Table_getStringValue_a (thy recordings.get(), row, 1);
		Melder_require (Melder_stringMatchesCriterion (component, kMelder_string::STARTS_WITH, U"comp-", true),
			U"Component ", component, U" does't start with “comp-”.");
		const char32 componentLetter = component [5];
		Melder_require (componentLetter >= U'a' && componentLetter <= U'o',
			U"The sixth character of component ", component, U" should have been between “a” and “o”.");
		if (! components [componentLetter - U'a']) {
			Table_removeRow (thy recordings.get(), row);
			thy textGrids. removeItem (row);
		}
	}
	return thee;
}

autoCorpus Corpus_extractEducationRegions (Corpus me,
	bool sHolland, bool nHolland, bool wUtrecht, bool zeeland, bool eUtrecht,
	bool guelders, bool veluwe, bool wFriesland, bool polders, bool achterhoek,
	bool overijssel, bool drenthe, bool groningen, bool frisia, bool nBrabant,
	bool eLimburg, bool netherlands, bool sBrabant, bool eFlanders, bool wFlanders,
	bool wLimburg, bool nBelgium, bool sBelgium, bool reg_other, bool reg_unknown
) {
	bool regions [25] {
		sHolland, nHolland, wUtrecht, zeeland, eUtrecht,
		guelders, veluwe, wFriesland, polders, achterhoek,
		overijssel, drenthe, groningen, frisia, nBrabant,
		eLimburg, netherlands, sBrabant, eFlanders, wFlanders,
		wLimburg, nBelgium, sBelgium, reg_other, reg_unknown
	};
	autoCorpus thee = Data_copy (me);
	for (integer row = thy recordings -> rows.size; row >= 1; row --) {
		TextGrid textgrid = thy textGrids.at [row];
		bool foundMatch = false;
		for (integer itier = 1; itier <= textgrid -> tiers->size; itier ++) {
			IntervalTier tier = (IntervalTier) textgrid -> tiers->at [itier];   // TODO: check cast
			conststring32 tierName = tier -> name.get();
			if (Melder_stringMatchesCriterion (tierName, kMelder_string::STARTS_WITH, U"ort/", true)) {
				const conststring32 speakerName = & tier -> name [4];
				integer speakerRow = Table_searchColumn (thy speakers.get(), 5, speakerName);
				if (speakerRow == 0) {
					if (! str32equ (speakerName, U"BACKGROUND") &&
						! str32equ (speakerName, U"COMMENT") &&
						! str32equ (speakerName, U"UNKNOWN") &&
						! str32equ (speakerName, U"N00383"))
						Melder_crash (U"<<", speakerName, U">> <<", tierName, U">>");
				} else {
					conststring32 speakerRegionCode = Table_getStringValue_a (thy speakers.get(), speakerRow, 17);
					integer icode = 0;
					for (; icode < 25; icode ++)
						if (str32equ (speakerRegionCode, Corpus_CGN_regionCodes [icode]))
							break;
					if (icode >= 25) {
						if (str32equ (speakerRegionCode, U"regB"))
							icode = 24;
					}
					if (icode >= 25)
						Melder_crash (icode, U" <<", speakerRegionCode, U">>");
					if (regions [icode]) {
						foundMatch = true;
						break;
					}
				}
			}
		}
		if (! foundMatch) {
			/*
				Remove this TextGrid and Sound.
			*/
			thy textGrids. removeItem (row);
			Table_removeRow (thy recordings.get(), row);
		}
	}
	return thee;
}

/* End of file Corpus.cpp */
