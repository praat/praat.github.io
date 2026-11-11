/* TIMITCorpus.cpp
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

#include "TIMITCorpus.h"
#include "TextGrid_Sound.h"

static conststring32 TIMITCorpus_regions [8] {
	U"New England", U"Northern", U"North Midland", U"South Midland",
	U"Southern", U"New York City", U"Western", U"Army Brat (moved around)"
};

static conststring32 TIMITCorpus_race_short [6] {
	U"WHT", U"BLK", U"AMR", U"SPN", U"ORN", U"?" 
};

static conststring32 TIMITCorpus_race_long [6] {
	U"White", U"Black", U"American Indian", U"Spanish-American", U"Oriental", U"Unknown" 
};

static conststring32 TIMITCorpus_education_short [6] {
	U"HS", U"AS", U"BS", U"MS", U"PHD", U"?" 
};

static conststring32 TIMITCorpus_education_long [6] {
	U"High School", U"Associate Degree", U"Bachelor's Degree (BS or BA)",
	U"Master's Degree (MS or MA)", U"Doctorate Degree (PhD, JD, or MD)", U"Unknown" 
};

autoSound structTIMITCorpus :: extractSound (integer number) {
	autoMelderString fileName;
	MelderString_append (& fileName, folderWithSoundFiles.get());
	/* 2: use, dr, speaker, sound */
	MelderString_append (& fileName, U"/", Table_getStringValue_a (recordings.get(), number, 2_integer));
	MelderString_append (& fileName, U"/", U"dr", Table_getStringValue_a (recordings.get(), number, 3_integer));
	MelderString_append (& fileName, U"/", Table_getStringValue_a (recordings.get(), number, 4_integer));
	MelderString_append (& fileName, U"/", Table_getStringValue_a (recordings.get(), number, 5_integer));
	structMelderFile file;
	Melder_pathToFile (fileName.string, & file);
	autoSound me = Data_readFromFile (& file).static_cast_move <structSound>();
	return me;
}

static autoTable Table_readSpeakerInfoFromTIMITTextFile (MelderFile speakersFile) {
	try {
		const conststring32 colNames [] = { U"ID", U"Sex", U"DR", U"Use", U"RecDate", U"BirthDate", U"Ht", U"Race", U"Edu", U"Comments" };
		autoTable me = Table_createWithColumnNames (0, ARRAY_TO_STRVEC (colNames));
		autoSTRVEC lines = readLinesFromFile_STRVEC (speakersFile);
		const integer numberOfLines = lines.size;
		Melder_require (numberOfLines > 1,
			U"Not enough lines.");
		autoMelderString comment, dateString;
		for (integer iline = 1; iline <= lines.size; iline ++) {
			if (Melder_startsWith (lines [iline].get(), U";"))
				continue;
			autostring32 lcLine = lowerCase_STR (lines [iline].get()); // kill the ugly uppercase
			autoSTRVEC items = splitByWhitespace_STRVEC(lcLine.get());
			Melder_require (items.size >= my numberOfColumns - 1,
				U"There should be at least ", my numberOfColumns - 1, U" items in line ", iline, U".");
			/*
				First put data into sensible formats:
					From 'mm/dd/yy' to the '19yymmdd'
					Lengths to mks units x'y" to (x*30.48+y*2.54)/100 m
			*/
			auto convertDate = [&](conststring32 date) -> conststring32  {
				Melder_assert (Melder_length (date) == 8);
				MelderString_empty (& dateString);
				MelderString_append (& dateString, U"19");
				MelderString_appendCharacter (& dateString, date [6]);	//y
				MelderString_appendCharacter (& dateString, date [7]);	//y
				MelderString_appendCharacter (& dateString, date [0]);	//m
				MelderString_appendCharacter (& dateString, date [1]);	//m
				MelderString_appendCharacter (& dateString, date [3]);	//d
				MelderString_appendCharacter (& dateString, date [4]);	//d
				return dateString.string;
			};
			
			items [5] = Melder_dup (convertDate (items[5].get()));
			items [6] = Melder_dup (convertDate (items[6].get()));
			conststring32 size = items [7].get();
			const integer feet = size [0] - U'0';
			integer inches = size [2] - U'0';
			if (size[3] != U'"') {
				inches = inches * 10 + (size[3] - U'0');
			}
			const double height = (feet * 30.38 + inches * 2.54) / 100.0; // m
			items [7] = Melder_dup (Melder_fixed (height, 2));
			/*
				End of conversions
			*/
			Table_appendRow (me.get());
			const integer irow = my rows.size;
			TableRow row = my rows.at [irow];
			for (integer icol = 1; icol <= my numberOfColumns - 1; icol ++)
				row -> cells [icol]. string = items[icol].move();
			/*
				The comment has been split up in separate items; join the with a space
			*/
			if (items.size > my numberOfColumns - 1) {
				MelderString_empty (& comment);
				MelderString_append (& comment, items [my numberOfColumns].get());
				for (integer i = my numberOfColumns + 1; i <= items.size; i ++)
					MelderString_append (& comment, U" ", items [i].get());
				row -> cells [my numberOfColumns]. string = Melder_dup (comment.string);
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cannot read speaker information from file ", speakersFile);
	}
}

autoTIMITCorpus TIMITCorpus_import (conststring32 rootFolderPath) {
	autoTIMITCorpus me = Thing_new (TIMITCorpus);

	structMelderFolder rootFolder { };
	Melder_relativePathToFolder (rootFolderPath, & rootFolder);
	Melder_require (MelderFolder_exists (& rootFolder),
		U"TIMIT folder ", & rootFolder, U" does not exist.");
	const integer rootPathLength = Melder_length (rootFolder.path);
	
	my folderWithSoundFiles = Melder_dup (MelderFolder_peekPath (& rootFolder));
	my folderWithAnnotationFiles = Melder_dup (MelderFolder_peekPath (& rootFolder));
	/*
		First check whether the main folders "train", "test" and "doc" exist
	*/
	structMelderFolder trainFolder { };
	MelderFolder_getSubfolder (& rootFolder, U"train", & trainFolder);
	Melder_require (MelderFolder_exists (& trainFolder),
		U"TIMIT folder ", & trainFolder, U" does not exist.");
	structMelderFolder testFolder { };
	MelderFolder_getSubfolder (& rootFolder, U"test", & testFolder);
	Melder_require (MelderFolder_exists (& testFolder),
		U"TIMIT folder ", & testFolder, U" does not exist.");
	
	structMelderFolder metadataFolder { };
	MelderFolder_getSubfolder (& rootFolder, U"doc", & metadataFolder);
	Melder_require (MelderFolder_exists (& metadataFolder),
		U"TIMIT folder ", & metadataFolder, U" does not exist.");
	/*
		The first column 'number' refers to the corresponding textgrid.
		If we sort the recordings the association with the corresponding textgrids keeps intact!
	*/
	const conststring32 columnNames_array [] = { U"number", U"use", U"dr", U"speaker", U"sound" };
	my recordings = Table_createWithColumnNames (0, ARRAY_TO_STRVEC (columnNames_array));

	/*
		In TIMIT for speaker <xxxx> their sound and label files are in the folder m<xxxx> or f<xxxx>,
		depending on whether speaker <xxxx> is male or female, respectively.
	*/
	integer fileNumber = 0;
	auto getData = [&] (MelderFolder useFolder) {
		autoSTRVEC regionFolderNames = folderNames_STRVEC (Melder_cat (MelderFolder_peekPath (useFolder), U"/dr*"));
		for (integer iregion = 1; iregion <= regionFolderNames.size; iregion ++) {
			structMelderFolder regionFolder { };
			MelderFolder_getSubfolder (useFolder, regionFolderNames [iregion].get(), & regionFolder);
			autoSTRVEC speakerFolderNames = folderNames_STRVEC (Melder_cat (MelderFolder_peekPath (& regionFolder), U"/*"));
			for (integer ispeaker = 1; ispeaker <= speakerFolderNames.size; ispeaker ++) {
				structMelderFolder speakerFolder { };
				MelderFolder_getSubfolder (& regionFolder, speakerFolderNames [ispeaker].get(), & speakerFolder);
				autoSTRVEC soundFileNames = fileNames_STRVEC (Melder_cat (MelderFolder_peekPath (& speakerFolder), U"/*wav"));
				for (integer ifile = 1; ifile <= soundFileNames.size; ifile ++) {
					structMelderFile soundFile {};
					MelderFolder_getFile (& speakerFolder, soundFileNames [ifile].get(), & soundFile);
					trace (U"Reading file ", soundFileNames [ifile].get());
					fileNumber ++;
					try {
						autoTextGrid textGrid;
						autoSound sound = Sound_readWithAdjacentAnnotationFiles_timit (soundFile.path, & textGrid);
						Table_appendRow (my recordings.get());
						Table_setNumericValue (my recordings.get(), my recordings -> rows.size, 1, fileNumber);
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 2, MelderFolder_name (useFolder));
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 3, regionFolderNames [iregion].get());
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 4, speakerFolderNames [ispeaker].get());
						const integer soundFileLength = Melder_length (soundFile.path);
						char32 *lastPeriod = str32rchr (soundFile.path, U'.');
						lastPeriod [0] = U'\0';
						Table_setStringValue (my recordings.get(), my recordings -> rows.size, 5, MelderFile_name (& soundFile));
						my textGrids. addItem_move (textGrid.move());
					} catch (MelderError) {
						Melder_clearError ();
						trace (U"Error handling file ", soundFileNames [ifile].get());
					}
				}
			}
		}
	};
	
	getData (& trainFolder);
	getData (& testFolder);
	

	structMelderFile speakersFile { };
	MelderFolder_getFile (& metadataFolder, U"spkrinfo.txt", & speakersFile);
	Melder_require (MelderFile_exists (& speakersFile),
		U"TIMIT file ", & speakersFile, U" does not exist.");
	my speakers = Table_readSpeakerInfoFromTIMITTextFile (& speakersFile);
	return me;
}

/* End of file TIMITCorpus.cpp */
