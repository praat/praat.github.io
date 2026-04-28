/* STRVEC.cpp
 *
 * Copyright (C) 2006,2007,2009,2011,2012,2015-2026 Paul Boersma
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

#if defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include "winport_off.h"
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <dirent.h>
#endif

#include "melder.h"

static autoSTRVEC fileOrFolderNames_STRVEC (conststring32 path /* cattable */, bool wantDirectories, bool caseSensitive) {
	/*
		Resolve home-relative paths.
	*/
	if (const bool pathIsHomeRelative = ( path [0] == U'~' &&
		(path [1] == U'/' || path [1] == Melder_DIRECTORY_SEPARATOR || path [1] == U'\0') )
	) {
		char32 absolutePath [kMelder_MAXPATH+1];
		Melder_sprint (absolutePath,kMelder_MAXPATH+1, Melder_peek8to32_u (getenv ("HOME")), & path [1]);
		return fileOrFolderNames_STRVEC (absolutePath, wantDirectories, caseSensitive);
	}
	#if defined (_WIN32)
		try {
			/*
				Parse the path.
				This can be either a directory name such as "/Users/paul/sounds"
				or a wildcarded path such as "/Users/paul/sounds/h*.wav".
				Example: in "/Users/paul/sounds/h*llo.*av",
				the search directory is "/Users/paul/sounds",
				the left environment is "h", the middle environment is "llo.", and the right environment is "av".
			*/
			autoMelderString searchDirectory, left, middle, right, filePath;
			MelderString_copy (& searchDirectory, path);
			char32 *const asterisk1 = str32chr (searchDirectory.string, U'*');
			char32 *const asterisk2 = str32rchr (searchDirectory.string, U'*');
			if (asterisk1) {
				/*
					The path is a wildcarded path.
				*/
				*asterisk1 = U'\0';
				*asterisk2 = U'\0';
				searchDirectory. length = asterisk1 - searchDirectory.string;   // probably superfluous, but correct
				char32 *lastSlash = str32rchr (searchDirectory.string, U'/');
				if (! lastSlash)
					lastSlash = str32rchr (searchDirectory.string, U'\\');
				if (lastSlash) {
					*lastSlash = U'\0';   // this fixes searchDirectory
					searchDirectory. length = lastSlash - searchDirectory.string;   // probably superfluous, but correct
					MelderString_copy (& left, lastSlash + 1);
				} else {
					MelderString_copy (& left, searchDirectory.string);   // quickly save...
					MelderString_empty (& searchDirectory);   // ...before destruction
				}
				if (asterisk1 != asterisk2) {
					MelderString_copy (& middle, asterisk1 + 1);
				}
				MelderString_copy (& right, asterisk2 + 1);
			} else {
				/*
					We're finished. No asterisk, hence the path is a directory name.
				*/
			}
			WIN32_FIND_DATAW findData;
			HANDLE searchHandle = FindFirstFileW (Melder_peek32toW_fileSystem (
					Melder_cat (searchDirectory.string [0] ? searchDirectory.string : U".", U"/*")), & findData);
			if (searchHandle == INVALID_HANDLE_VALUE)
				Melder_throw (U"Cannot open folder ", searchDirectory.string, U".");
			autoSTRVEC strings;
			do {
				if ((! wantDirectories && (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
					|| (wantDirectories && (findData. dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0))
				{
					if (findData. cFileName [0] == L'.')
						continue;
					MelderString_copy (& filePath, searchDirectory.string [0] ? searchDirectory.string : U".");
					MelderString_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
					char32 buffer32 [kMelder_MAXPATH+1];
					Melder_sprint (buffer32,kMelder_MAXPATH+1, Melder_peekWto32 (findData. cFileName));
					MelderString_append (& filePath, buffer32);
					const int64 length = Melder_length (buffer32);
					integer numberOfMatchedCharacters = 0;
					bool doesTheLeftMatch = true;
					if (left. length != 0) {
						doesTheLeftMatch = str32nequ_optionallyCaseSensitive (buffer32, left.string, left. length, caseSensitive);
						if (doesTheLeftMatch)
							numberOfMatchedCharacters = left.length;
					}
					bool doesTheMiddleMatch = true;
					if (middle. length != 0) {
						const char32 *const position = str32str_optionallyCaseSensitive (buffer32 + numberOfMatchedCharacters, middle.string, caseSensitive);
						doesTheMiddleMatch = !! position;
						if (doesTheMiddleMatch)
							numberOfMatchedCharacters = position - buffer32 + middle.length;
					}
					bool doesTheRightMatch = true;
					if (right. length != 0) {
						const int64 startOfRight = length - right. length;
						doesTheRightMatch = startOfRight >= numberOfMatchedCharacters &&
							str32equ_optionallyCaseSensitive (buffer32 + startOfRight, right.string, caseSensitive);
					}
					if (buffer32 [0] != U'.' && doesTheLeftMatch && doesTheMiddleMatch && doesTheRightMatch)
						strings. append (buffer32);
				}
			} while (FindNextFileW (searchHandle, & findData));
			FindClose (searchHandle);
			sort_STRVEC_inout (strings.get());
			return strings;
		} catch (MelderError) {
			throw;
		}
	#else   // use stat
		DIR *d = nullptr;
		try {
			/*
				Parse the path.
				This can be either a directory name such as "/Users/paul/sounds"
				or a wildcarded path such as "/Users/paul/sounds/h*.wav".
				Example: in "/Users/paul/sounds/h*llo.*av",
				the search directory is "/Users/paul/sounds",
				the left environment is "h", the middle environment is "llo.", and the right environment is "av".
			*/
			autoMelderString searchDirectory, left, middle, right, filePath;
			MelderString_copy (& searchDirectory, path);
			char32 *const asterisk1 = str32chr (searchDirectory.string, U'*');
			char32 *const asterisk2 = str32rchr (searchDirectory.string, U'*');
			if (asterisk1) {
				/*
					The path is a wildcarded path.
				*/
				*asterisk1 = U'\0';
				*asterisk2 = U'\0';
				searchDirectory. length = asterisk1 - searchDirectory.string;   // probably superfluous, but correct
				char32 *const lastSlash = str32rchr (searchDirectory.string, Melder_DIRECTORY_SEPARATOR);
				if (lastSlash) {
					*lastSlash = U'\0';   // this fixes searchDirectory
					searchDirectory. length = lastSlash - searchDirectory.string;   // probably superfluous, but correct
					MelderString_copy (& left, lastSlash + 1);
				} else {
					MelderString_copy (& left, searchDirectory.string);   // quickly save...
					MelderString_empty (& searchDirectory);   // ...before destruction
				}
				if (asterisk1 != asterisk2) {
					MelderString_copy (& middle, asterisk1 + 1);
				}
				MelderString_copy (& right, asterisk2 + 1);
			} else {
				/*
					We're finished. No asterisk, hence the path is a directory name.
				*/
			}
			char buffer8 [kMelder_MAXPATH+1];
			Melder_32to8_fileSystem_inplace (searchDirectory.string, buffer8);
			d = opendir (buffer8 [0] ? buffer8 : ".");
			if (! d)
				Melder_throw (U"Cannot open folder ", searchDirectory.string, U".");
			//Melder_casual (U"opened");
			autoSTRVEC strings;
			struct dirent *entry;
			while (!! (entry = readdir (d))) {
				MelderString_copy (& filePath, searchDirectory.string [0] ? searchDirectory.string : U".");
				MelderString_appendCharacter (& filePath, Melder_DIRECTORY_SEPARATOR);
				char32 buffer32 [kMelder_MAXPATH+1];
				Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
				MelderString_append (& filePath, buffer32);
				//Melder_casual (U"read ", filePath.string);
				Melder_32to8_fileSystem_inplace (filePath.string, buffer8);
				struct stat stats;
				if (stat (buffer8, & stats) != 0) {
					//Melder_throw (U"Cannot look at file ", filePath.string, U".");
					//stats. st_mode = -1L;
				}
				//Melder_casual (U"statted ", filePath.string);
				//Melder_casual (U"file ", filePath.string, U" mode ", stats. st_mode / 4096);
				if ((! wantDirectories && S_ISREG (stats. st_mode)) || (wantDirectories && S_ISDIR (stats. st_mode))) {
					Melder_8bitFileRepresentationToStr32_inplace (entry -> d_name, buffer32);
					const int64 length = Melder_length (buffer32);
					integer numberOfMatchedCharacters = 0;
					bool doesTheLeftMatch = true;
					if (left. length != 0) {
						doesTheLeftMatch = str32nequ_optionallyCaseSensitive (buffer32, left.string, left. length, caseSensitive);
						if (doesTheLeftMatch)
							numberOfMatchedCharacters = left.length;
					}
					bool doesTheMiddleMatch = true;
					if (middle. length != 0) {
						const char32 *const position = str32str_optionallyCaseSensitive (buffer32 + numberOfMatchedCharacters, middle.string, caseSensitive);
						doesTheMiddleMatch = !! position;
						if (doesTheMiddleMatch)
							numberOfMatchedCharacters = position - buffer32 + middle.length;
					}
					bool doesTheRightMatch = true;
					if (right. length != 0) {
						const int64 startOfRight = length - right. length;
						doesTheRightMatch = startOfRight >= numberOfMatchedCharacters &&
							str32equ_optionallyCaseSensitive (buffer32 + startOfRight, right.string, caseSensitive);
					}
					if (buffer32 [0] != U'.' && doesTheLeftMatch && doesTheMiddleMatch && doesTheRightMatch)
						strings. append (buffer32);
				}
			}
			closedir (d);
			sort_STRVEC_inout (strings.get());
			return strings;
		} catch (MelderError) {
			if (d)
				closedir (d);   // "finally"
			throw;
		}
	#endif
}
autoSTRVEC fileNames_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, false, true);
}
autoSTRVEC folderNames_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, true, true);
}
autoSTRVEC fileNames_caseInsensitive_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, false, false);
}
autoSTRVEC folderNames_caseInsensitive_STRVEC (conststring32 path /* cattable */) {
	return fileOrFolderNames_STRVEC (path, true, false);
}

autoSTRVEC readLinesFromFile_STRVEC (MelderFile file) {
	autoMelderReadText text = MelderReadText_createFromFile (file);
	int64 numberOfLines = MelderReadText_getNumberOfLines (text.get());
	if (numberOfLines == 0)
		return autoSTRVEC ();
	autoSTRVEC result = autoSTRVEC (numberOfLines);
	for (integer iline = 1; iline <= numberOfLines; iline ++) {
		const mutablestring32 line = MelderReadText_readLine (text.get());
		result [iline] = Melder_dup (line);
	}
	return result;
}

autoSTRVEC splitByWhitespace_STRVEC (conststring32 string) {
	if (! string)
		return autoSTRVEC();   // accept null pointer input
	integer n = NUMnumberOfTokens (string);
	if (n == 0)
		return autoSTRVEC();
	autoSTRVEC result (n);

	integer itoken = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		const char32 *beginOfInk = p;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
		const integer numberOfCharacters = p - beginOfInk;
		autostring32 token (numberOfCharacters);
		str32ncpy (token.get(), beginOfInk, numberOfCharacters);
		result [++ itoken] = token.move();
	}
	return result;
}

autoSTRVEC splitBy_STRVEC (conststring32 string, conststring32 separator) {
	if (! string)
		return autoSTRVEC();   // accept null pointer input
	const integer separatorLength = Melder_length (separator);
	const char32 *p = & string [0];
	const char32 *locationOfSeparator = str32str (p, separator);
	if (! locationOfSeparator) {
		if (string [0] == U'\0') {
			/*
				This is ambiguous: either a single empty string, or no strings at all.
				We decide that, as with space separation, empty strings are somewhat hard to find.
				So we decide that this is a single empty string.
			*/
			return autoSTRVEC();
		}
		return autoSTRVEC ({ string });
	}
	integer n = 1;
	do {
		n += 1;
		p = locationOfSeparator + separatorLength;
		locationOfSeparator = str32str (p, separator);
	} while (locationOfSeparator);
	autoSTRVEC result (n);
	integer itoken = 0;
	p = & string [0];
	do {
		locationOfSeparator = str32str (p, separator);
		if (locationOfSeparator) {
			const integer numberOfCharacters = locationOfSeparator - p;
			autostring32 token (numberOfCharacters);
			str32ncpy (token.get(), p, numberOfCharacters);
			result [++ itoken] = token.move();
			p = locationOfSeparator + separatorLength;
		} else {
			result [++ itoken] = Melder_dup (p);
		}
	} while (locationOfSeparator);
	return result;
}

/* End of file STRVEC.cpp */
