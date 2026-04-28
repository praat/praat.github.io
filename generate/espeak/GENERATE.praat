"How to integrate eSpeak into Praat"
© 2012,2015,2017,2021,2023 David Weenink, 2024-2026 Paul Boersma

This script is `generate/espeak/GENERATE.praat` in the Praat source distribution.
It looks like a Praat %notebook and can indeed be dry-run as such,
but it is actually meant as a Praat %script from which you copy–paste
`bash` lines into a terminal window and run Praat chunks with "Run selection".

All paths in this script are relative to where this script is.

1. Getting the eSpeak sources
=============================

The eSpeak NG program and its library are the successor of eSpeak 
(eSpeak was maintained by Jonathan Duddington); eSpeak NG is a fork 
maintained by Reece H. Dunn.

If you download the eSpeak NG code for the first time, you can do this by downloading or cloning the sources from upstream,
namely from `https://github.com/espeak-ng/espeak-ng`, in one of two ways.

If you just need the sources and don’t plan to send bug fixes to the eSpeak team,
you can just download the ZIP file and unpack it,
in e.g. `../../../support/external/eSpeak`, giving a folder `../../../support/external/eSpeak/espeak-ng`,
to whose name you could then append the appropriate version number,
(which you can find in the second line of `../../../support/external/eSpeak/espeak-ng/configure.ac`)
or the download date (this is better, because versions changes much more often than the version number).
If the eSpeak version number is `1.52-dev` and the download date is 2024-08-24,
the resulting folder `../../../support/external/eSpeak/espeak-ng-20240824` takes up 66.5 MB.

Alternatively, you %clone the Git repository:
{;
	cd ../../../support/external/eSpeak
	git clone https://github.com/espeak-ng/espeak-ng.git
	mv espeak-ng espeak-ng-20240824
}
This way, the folder `../../../support/external/eSpeak/espeak-ng-20240824` will be 152.1 MB large.
The difference between the two ways of creating the folder is due to the existence of the `.git` file,
which takes up 85.7 MB.

2. Building eSpeak language dictionaries
========================================

The eSpeak NG sources as downloaded above are not complete. We will have to build the dictionaries.
If you can build Praat itself, you have already installed a compiler, and compiler tools (`make`, `pkg-config`).
Also install `automake`, `autoconf` and `libtool`.
{;
	cd ../../../support/external/eSpeak/espeak-ng-20240824
	./autogen.sh
	./configure
	# or: CC=gcc CFLAGS="-Wprototype..." ./configure prefix=/usr/local (or /usr)
	make
}
This creates in the subfolder `espeak-ng-data` the 117 files `XXX_dict` (the voices were already there),
as well as `phontab`, `phondata`, `phondata-manifest`, `phonindex`, and `intonations`.
Next to these 122 files in `espeak-ng-data`, there are 104 voices in its subfolder `voices/!v`,
and 131 files in its subfolder `lang`, for a total of 360 files (version downloaded 2024-08-24).

If you have the Git version, you can establish updates via
{;
	cd ../../../support/external/eSpeak/espeak-ng-20240824
	git pull https://github.com/espeak-ng/espeak-ng.git
	make clean
	make
}

3. Copying eSpeak data to the Praat sources
===========================================

Copy the files in `../../../support/external/eSpeak/espeak-ng-20240824/espeak-ng-data`
to `./data` and its subfolders `lang` and `voices`,
for instance by just copying the whole folder and deleting the subfolders `mbrola_ph` and `voices/mb`.

In the eSpeak NG app, the `espeak-ng-data` folder is used to supply the data the synthesizer needs.
The synthesizer needs to know the location of this folder to work correctly;
also, the synthesizer’s versions and the `espeak-ng-data` versions of the data files have to be synchronized.
This scheme is not acceptable in Praat, since we cannot expect users to maintain
a separate folder for the eSpeak NG data.
Therefore we convert the 360 data files into source code files that contain FileInMemory objects,
inside the folder of this script. The eSpeak source code will have to be converted to accomplish this.

As the license of eSpeak is GPL 3 or later, our derived FileInMemory objects are distributed under GPL 3 or later as well:
{
procedure saveFileInMemorySetAsCppFile: .name$
	.fileName$ = .name$ + ".cpp"
	.head$ =
	... "/* " + .fileName$ + newline$ +
	... " *" + newline$ +
	... " * This file was automatically created from files in the folder `generate/espeak/data`" + newline$ +
	... " * by the script `generate/espeak/GENERATE.praat` in the Praat source distribution." + newline$ +
	... " *" + newline$ +
	... " * eSpeak NG version: 1.52-dev, downloaded 2024-08-24T19:38Z from https://github.com/espeak-ng/espeak-ng" + newline$ +
	... " * File creation date: " + date$ () + newline$ +
	... " *" + newline$ +
	... " * Copyright (C) 2005-2014 Jonathan Duddington (for eSpeak)" + newline$ +
	... " * Copyright (C) 2015-2023 Reese Dunn (for eSpeak-NG)" + newline$ +
	... " * Copyright (C) 2012-2024 David Weenink, 2024,2026 Paul Boersma (for Praat)" + newline$ +
	... " *" + newline$ +
	... " * This program is free software; you can redistribute it and/or modify " + newline$ +
	... " * it under the terms of the GNU General Public License as published by" + newline$ +
	... " * the Free Software Foundation; either version 3 of the License, or" + newline$ +
	... " * (at your option) any later version." + newline$ +
	... " *" + newline$ +
	... " * This program is distributed in the hope that it will be useful," + newline$ +
	... " * but WITHOUT ANY WARRANTY; without even the implied warranty of" + newline$ +
	... " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the" + newline$ +
	... " * GNU General Public License for more details." + newline$ +
	... " *" + newline$ +
	... " * You should have received a copy of the GNU General Public License" + newline$ +
	... " * along with this program; if not, see: <http://www.gnu.org/licenses/>." + newline$ +
	... " */" + newline$ + newline$ +
	... “#include "espeak_praat.h"” + newline$
	.code$ = Show as code: .name$, 100
	.tail$ = "/* End of file " + .fileName$ + " */" + newline$
	writeFile: .fileName$, .head$, .code$, .tail$
endproc
}

Create FileInMemory objects for the five `phon` files.

%Hack: we have to make five sets of one, because `Create FileInMemory...` turns the relative path
into an absolute path, while `Create FileInMemorySet from directory contents...`
retains the relative path, which is what we want.

{
procedure defineDataFolders
	generationFolder$ = "."   ; the folder of this script, i.e. `generate/espeak` in the Praat sources
	generationDataFolder$ = generationFolder$ + "/data"
	generationLanguageFolder$ = generationDataFolder$ + "/lang"
	generationVoicesFolder$ = generationDataFolder$ + "/voices/!v"
	generationSourceFolder$ = generationFolder$ + "/src"
endproc
}

{;
	@defineDataFolders
	fim1 = Create FileInMemorySet from directory contents: "fim1", generationDataFolder$, "*phondata"
	fim2 = Create FileInMemorySet from directory contents: "fim2", generationDataFolder$, "*phonindex"
	fim3 = Create FileInMemorySet from directory contents: "fim3", generationDataFolder$, "*phontab"
	fim4 = Create FileInMemorySet from directory contents: "fim4", generationDataFolder$, "*intonations"
	fim5 = Create FileInMemorySet from directory contents: "fim5", generationDataFolder$, "*phondata-manifest"
	selectObject: fim1, fim2, fim3, fim4, fim5
	Merge
	Rename: "phon"
	removeObject: fim1, fim2, fim3, fim4, fim5
}
Create FileInMemory objects for the 117 dictionary files, with 23,013,656 bytes in total.
{;
	@defineDataFolders
	Create FileInMemorySet from directory contents: "dicts", generationDataFolder$, "*_dict"
	numberOfDicts = Get number of files
	assert numberOfDicts = 117
}
Create FileInMemory objects for the 134 language files. Three language files (eu, ko, qu)
are at the top level of the `lang` folder:
{;
	@defineDataFolders
	Create FileInMemorySet from directory contents: "languages", generationLanguageFolder$, "*"
	numberOfLanguages = Get number of files
	assert numberOfLanguages = 3
}
The remaining 134 language files are in subfolders:
{;
	@defineDataFolders
	languageFolders$# = folderNames$# (generationLanguageFolder$ + "/*")
	for languageFolder to size (languageFolders$#)
		languageFolder$ = languageFolders$# [languageFolder]
		languageFiles = Create FileInMemorySet from directory contents: "language", generationLanguageFolder$ + "/" + languageFolder$ , "*"
		plusObject: "FileInMemorySet languages"
		merged = Merge
		# should be: Merge second into first
		removeObject: languageFiles, "FileInMemorySet languages"
		selectObject: merged
		Rename: "languages"
	endfor
	numberOfLanguages = Get number of files
	assert numberOfLanguages = 137
}
Create FileInMemory objects for the 104 voice files.
{;
	@defineDataFolders
	Create FileInMemorySet from directory contents: "voices", generationVoicesFolder$, "*"
	numberOfVoices = Get number of files
	assert numberOfVoices = 104
}
We safely save these into the generation folder:
{;
	@defineDataFolders
	selectObject: "FileInMemorySet phon"
	@saveFileInMemorySetAsCppFile: "espeak_praat_FileInMemorySet_addPhon"
	selectObject: "FileInMemorySet dicts"
	@saveFileInMemorySetAsCppFile: "espeak_praat_FileInMemorySet_addDicts"
	selectObject: "FileInMemorySet languages"
	@saveFileInMemorySetAsCppFile: "espeak_praat_FileInMemorySet_addLanguages"
	selectObject: "FileInMemorySet voices"
	@saveFileInMemorySetAsCppFile: "espeak_praat_FileInMemorySet_addVoices"
}
After some inspection for correctness, %move (not %copy, in order to save space)
these files into `src/external/espeak`.

4. Copying eSpeak source code to the Praat sources
==================================================

The following files are in `support/external/eSpeak/espeak-ng-20240824/src`:
{
procedure defineSourceFolders
	srcRootFolder$ = "../../../support/external/eSpeak/espeak-ng-20240824/src"

	sourceFolder$ = srcRootFolder$ + "/libespeak-ng"
	sourceFiles$# = { "common.c", "compiledict.c", "compilembrola.c", "dictionary.c",
	... "encoding.c", "error.c", "espeak_api.c", "espeak_command.c",
	... "event.c", "fifo.c", "ieee80.c", "intonation.c",
	... "klatt.c", "langopts.c", "mnemonics.c", "numbers.c", "phoneme.c", "phonemelist.c",
	... "readclause.c", "setlengths.c", "soundicon.c", "spect.c", "speech.c", "ssml.c",
	... "synthdata.c", "synthesize.c", "synth_mbrola.c", "translate.c", "translateword.c", "tr_languages.c",
	... "voices.c", "wavegen.c" }

	privateHeaderFolder$ = sourceFolder$
	privateHeaderFiles$# = { "common.h", "compiledict.h", "dictionary.h",
	... "error.h", "espeak_command.h", "event.h", "fifo.h", "intonation.h",
	... "klatt.h", "langopts.h", "mnemonics.h", "mbrola.h", "numbers.h", "phoneme.h", "phonemelist.h",
	... "readclause.h", "setlengths.h", "sintab.h", "soundicon.h", "speech.h", "ssml.h",
	... "synthdata.h", "synthesize.h", "translate.h", "translateword.h",
	... "voice.h", "wavegen.h" }

	publicHeaderFolder$ = srcRootFolder$ + "/include/espeak-ng"
	publicHeaderFiles$# = { "encoding.h", "espeak_ng.h", "speak_lib.h" }

	ucdSourceFolder$ = srcRootFolder$ + "/ucd-tools/src"
	ucdSourceFiles$# = { "case.c", "categories.c", "proplist.c" }

	ucdHeaderFolder$ = srcRootFolder$ + "/ucd-tools/src/include/ucd"
	ucdHeaderFiles$# = { "ucd.h" }
endproc
}
All of these are moved to `generate/espeak/src`, without any recursion, while renaming `c` to `cpp` files:
{;
	@defineSourceFolders
	for i to size (sourceFiles$#)
		text$ = readFile$: sourceFolder$ + "/" + sourceFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + sourceFiles$# [i] + "pp", text$
	endfor
	for i to size (privateHeaderFiles$#)
		text$ = readFile$: privateHeaderFolder$ + "/" + privateHeaderFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + privateHeaderFiles$# [i], text$
	endfor
	for i to size (publicHeaderFiles$#)
		text$ = readFile$: publicHeaderFolder$ + "/" + publicHeaderFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + publicHeaderFiles$# [i], text$
	endfor
	for i to size (ucdSourceFiles$#)
		text$ = readFile$: ucdSourceFolder$ + "/" + ucdSourceFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + ucdSourceFiles$# [i] + "pp", text$
	endfor
	for i to size (ucdHeaderFiles$#)
		text$ = readFile$: ucdHeaderFolder$ + "/" + ucdHeaderFiles$# [i]
		writeFile: generationSourceFolder$ + "/" + ucdHeaderFiles$# [i], text$
	endfor
}

5. Adapting eSpeak source code to Praat
=======================================

Many changes to the sources have to be made. If the original eSpeak sources don't change too much,
this can be done by a visual `diff` between for instance `./src/speech.cpp` (in the present generation folder)
and `../../external/espeak/speech.cpp` (in the actual Praat sources).
The former is the one to be changed (in the way described below).
Once all the source and header files are fine, they can be moved over the ones in `../../external/espeak`.
After this, the folder `./src` will be empty again.

- Simplify all includes, e.g. "espeak-ng/something.h" to "something.h"
- Rename `config.h` to `espeak__config.h`, and simplify it strongly,
  reducing it to
{;
	/*
		The config file typically contains lots of stuff that is specific to the machine on which eSpeak is compiled.
		However, in Praat we relegate all of this to `melder.h`.
	
		This file should be included before any other include file, in order to prevent definition clashes.
	
		In the Praat sources, this file should be included only in `.cpp` files inside `src/external/espeak`.
		Inclusion elsewhere in Praat can lead to clashes (e.g., `USE_ASYNC` undergoes an `#ifdef` in `mad`).
	*/
	
	#define PACKAGE_VERSION  "1.52-dev"
	
	#define USE_ASYNC  0
	#define USE_LIBPCAUDIO  0
	
	/*
		Integration into Praat
	*/
	#define DATA_FROM_SOURCECODE_FILES  1
	#define PATH_ESPEAK_DATA  "./data"   /* a relative path */
}
- Adapt the source files that include `config.h` to including `espeak__config.h`.
- Cast many return values and error values.

- Adapt `GetVoices()` in `voices.cpp`:
{;
	#if DATA_FROM_SOURCECODE_FILES   /* ppgb: whole function adapted to Praat */
		FileInMemorySet me = theEspeakPraatFileInMemorySet();
		static MelderString criterion;
		MelderString_copy (& criterion, Melder_peek8to32_u (path));
		MelderString_appendCharacter (& criterion, PATHSEP);
		Melder_assert (criterion.length == len_path_voices);   // sanity check
		for (long ifile = 1; ifile <= my size; ifile ++) {
			FileInMemory fim = my at [ifile];
			if (Melder_stringMatchesCriterion (fim -> string.get(), kMelder_string :: STARTS_WITH, criterion.string, true)) {
				FileInMemory f_voice = FileInMemory_fopen (fim, "r");
				conststring8 fname = Melder_peek32to8_fileSystem (fim -> string.get());
				espeak_VOICE *voice_data = ReadVoiceFile (f_voice, fname + len_path_voices, is_language_file);
				FileInMemory_fclose (f_voice);
				if (voice_data)
					voices_list [n_voices_list ++] = voice_data;
			}
		}
	#else   /* ppgb: the original code, which uses opendir: */
		char fname[sizeof(path_home)+100];
		DIR *dir;
		struct dirent *ent;
		if ((dir = opendir((char *)path)) == NULL) // note: (char *) is needed for WINCE
			return;
		while ((ent = readdir(dir)) != NULL) {
			if (n_voices_list >= (N_VOICES_LIST-2)) {
				fprintf(stderr, "Warning: maximum number %d of (N_VOICES_LIST = %d - 1) reached\n", n_voices_list + 1, N_VOICES_LIST);
				break; // voices list is full
			}
			if (ent->d_name[0] == '.')
				continue;
			sprintf(fname, "%s%c%s", path, PATHSEP, ent->d_name);
			if (AddToVoicesList(fname, len_path_voices, is_language_file) != 0)
				continue;
		}
		closedir(dir);
	#endif   /* DATA_FROM_SOURCECODE_FILES */
}

# 	7. in error.cpp replace writing to stderr with calls to Melder_throw()

In `dictionary.cpp`, `soundicon.cpp`, `speech.cpp`, `synthdata.cpp`, and `voices.cpp`:
- replace `FILE *` with `FileInMemory` (also in `common.cpp`);
- `#include "espeak_praat.h"`;
- prepend "FileInMemorySet_" to all calls of stat and fopen, adding a first argument `theEspeakPraatFileInMemorySet()`;
- prepend "FileInMemory_" to all calls of fclose, feof, fseek, ftell, fgets, fread, fgetc, fprintf and ungetc;

Here we list all the (remaining) occurrences of those functions in those five files
(`fprintf` should stay if to `stderr` or `f_trans`):
{;
	if 0
		@defineSourceFolders
		sourceFolder$ = generationSourceFolder$
	else
		sourceFolder$ = "../../external/espeak"
	endif
	sourceFiles$# = { "dictionary.cpp", "soundicon.cpp", "speech.cpp", "synthdata.cpp", "voices.cpp", "common.cpp" }
	targets$# = { "stat", "fopen", "fclose", "feof", "fseek", "ftell", "fgets", "fread", "fgetc", "fprintf", "ungetc" }
	writeInfo()
	for file to size (sourceFiles$#)
		lines$# = readLinesFromFile$#: sourceFolder$ + "/" + sourceFiles$# [file]
		for line to size (lines$#)
			line$ = lines$# [line]
			for target to size (targets$#)
				target$ = targets$# [target]
				if index_regex (line$, "[^_]" + target$)
					appendInfoLine: sourceFiles$# [file], " ", line, " ", line$
				endif
			endfor
		endfor
	endfor
}
Replace `FILE *` with `FileInMemory` in the relevant files.
{;
	if 0
		@defineSourceFolders
		sourceFolder$ = generationSourceFolder$
	else
		sourceFolder$ = "../../external/espeak"
	endif
	sourceFiles$# = { "dictionary.cpp", "soundicon.cpp", "speech.cpp", "synthdata.cpp", "voices.cpp", "common.cpp", "common.h" }
	writeInfo()
	for file to size (sourceFiles$#)
		lines$# = readLinesFromFile$#: sourceFolder$ + "/" + sourceFiles$# [file]
		for line to size (lines$#)
			line$ = lines$# [line]
			if index_regex (line$, "FILE\W")
				appendInfoLine: sourceFiles$# [file], " ", line, " ", line$
			endif
		endfor
	endfor
}
In `speech.cpp`:
- make `sync_espeak_Key`, `sync_espeak_Char`, `sync_espeak_SetPunctuationList` static, moving `sync_espeak_Char` first
- uninclude "espeak_command.h" (because `delete_espeak_command` falls under `USE_ASYNC`,
  `SetParameter` is also in `setlengths.h`, and `sync_espeak_Char` has become static)
- include "setlengths.h" (for `SetParameter`)

- `GetFileLength()` in `common.cpp` will simply end up like this (remove the Windows-specific stuff):
{;
	int GetFileLength(const char *filename)
	{
		struct stat statbuf;
		if (FileInMemorySet_stat(theEspeakPraatFileInMemorySet(), filename, &statbuf) != 0)
			return -errno;
		if (S_ISDIR(statbuf.st_mode))
			return -EISDIR;
		return statbuf.st_size;
	}
}

#	9. delete all "#pragma GCC visibility" lines

We inserted a number of explicit casts:
static_cast<espeak_ng_STATUS> (errno)

The overaching header file is `espeak_ng.h`,
so any #defines that should be global to espeak should be defined in `espeak_ng.h`.

In `espeak_ng.h`, we should ignore all the `dllexport` and `dllimport` labels:
	//ppgb #if defined(_WIN32) || defined(_WIN64)
	//ppgb #ifdef LIBESPEAK_NG_EXPORT
	//ppgb #define ESPEAK_NG_API __declspec(dllexport)
	//ppgb #else
	//ppgb #define ESPEAK_NG_API __declspec(dllimport)
	//ppgb #endif
	//ppgb #else
	#define ESPEAK_NG_API
	//ppgb #endif

In `speak_ng.h`, we replace the Windows-specific part
	#define PLATFORM_WINDOWS  1
	#define PATHSEP '\\'
with
	#if DATA_FROM_SOURCECODE_FILES
		#define PLATFORM_WINDOWS  0
		#define PATHSEP '/'
	#else
		#define PLATFORM_WINDOWS  1
		#define PATHSEP '\\'
	#endif
#endif
This is because we hard-coded the relative paths to the data files with forward slashes.

In `speech.cpp`, in the function `espeak_ng_Initialize`, we remove `setlocale`,
because the locale of the entire program shouldn't be overwritten by a library:
	/*
		(Paul Boersma 20240426:)
		When using this library in an app, e.g. Praat,
		we should not set the locale, because it will interfere with the locale
		that has been set in praat_init().
		To nevertheless experiment with setting the locale here,
		remove the space from "set locale" in the definition of SET_LOCALE
		and set USE_SET_LOCALE_IN_THIS_LIBRARY to 1 instead of 0.
		(The space is needed to be able to automatically determine that the name
		 of the function does not appear in the present source file `speech.cpp`.)
	*/
	#define SET_LOCALE  set locale
	#define USE_SET_LOCALE_IN_THIS_LIBRARY  0
	#if USE_SET_LOCALE_IN_THIS_LIBRARY
		if (SET_LOCALE(LC_CTYPE, "C.UTF-8") == NULL) {
			if (SET_LOCALE(LC_CTYPE, "UTF-8") == NULL) {
				if (SET_LOCALE(LC_CTYPE, "en_US.UTF-8") == NULL)
					SET_LOCALE(LC_CTYPE, "");
			}
		}
	#endif
However, eSpeak does need Unicode knowledge to work correctly,
so that iswalpha and all other isw* functions should be replaced
with iswalpha_portable and so on.

There is a bug in wavegen.cpp, where it reads
	MarkerEvent(marker_type, q[1], q[2], q[3], out_ptr);
The problem here is that q[2] and q[3] are of type intptr_t,
whereas MarkerEvent expects int arguments. This is no problem
on 32-bit systems, but on 64-bit systems the higher 4 bytes of q[2] are discarded.
On little-endian systems, these higher 4 bytes are the last 4 bytes,
and these rarely contain any relevant phoneme codes, so the problem rarely surfaces;
however, on big-endian systems the higher 4 bytes are the first 4 bytes,
so that all phoneme codes are in effect lost. A correct version is:
	MarkerEvent(marker_type, q[1], * (int *) & q[2], * ((int *) & q[2] + 1), out_ptr);
On 64-bit systems this splits up q[2] into two ints (in correct order),
and on 32-bit systems * ((int *) & q[2] + 1) is actually q[3], which is also correct.
MarkerEvent() itself pastes the two ints together again, in correct order.

#include "speak_lib.h"
#include "encoding.h"
#include "ucd.h"

#undef INCLUDE_MBROLA
#undef PLATFORM_POSIX
#undef PLATFORM_WINDOWS
#undef USE_NANOSLEEP

