/* melder_files.cpp
 *
 * Copyright (C) 1992-2008,2010-2026 Paul Boersma, 2013 Tom Naughton
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

/*
 * pb 2002/03/07 GPL
 * rvs&pb 2002/03/07 url support
 * pb 2002/03/10 Mach compatibility
 * pb 2003/09/12 MelderFile_getMacType
 * pb 2003/09/14 MelderFolder_relativePathToFile
 * pb 2004/09/25 use /tmp as temporary directory
 * pb 2004/10/16 C++ compatible structs
 * pb 2005/11/07 Windows: use %USERPROFILE% rather than %HOMESHARE%%HOMEPATH%
 * rvs&pb 2005/11/18 curl support
 * pb 2006/01/21 MelderFile_writeText does not create temporary file
 * pb 2006/08/03 openForWriting
 * rvs 2006/08/12 curl: do not fail on error
 * pb 2006/08/12 check whether unicodeName exists
 * pb 2006/10/28 erased MacOS 9 stuff
 * Erez Volk 2007/05/14 FLAC support
 * pb 2007/05/28 wchar
 * pb 2007/06/09 more wchar
 * pb 2007/08/12 more wchar
 * pb 2007/10/05 FSFindFolder
 * pb 2008/11/01 warn after finding final tabs (not just spaces) in file names
 * pb 2010/12/14 more high Unicode compatibility
 * pb 2011/04/05 C++
 * pb 2024/11/16 rid curl support
 */

#if defined (UNIX)
	#include <unistd.h>
	#include <sys/stat.h>
#endif
#ifdef _WIN32
	#include <windows.h>
#endif
#if defined (macintosh)
	#include "macport_on.h"
	#include <CoreFoundation/CoreFoundation.h>
	#include "macport_off.h"
#endif
#include <errno.h>
#include "melder.h"
#include "../kar/UnicodeData.h"

//#include "flac_FLAC_stream_encoder.h"
extern "C" int  FLAC__stream_encoder_finish (FLAC__StreamEncoder *);
extern "C" void FLAC__stream_encoder_delete (FLAC__StreamEncoder *);

#if defined (macintosh)
	#include <sys/stat.h>
	#define UNIX
	#include <unistd.h>
#endif

static char32 theShellDirectory [kMelder_MAXPATH+1];
void Melder_rememberShellDirectory () {
	structMelderFolder shellFolder { };
	Melder_getCurrentFolder (& shellFolder);
	str32cpy (theShellDirectory, MelderFolder_peekPath (& shellFolder));
}
conststring32 Melder_getShellDirectory () {
	return & theShellDirectory [0];
}

#if defined (UNIX)
void Melder_8bitFileRepresentationToStr32_inplace (const char *path8, char32 *path32) {
	#if defined (macintosh)
		CFStringRef cfpath = CFStringCreateWithCString (nullptr, path8, kCFStringEncodingUTF8);
		if (! cfpath) {
			/*
				Probably something wrong, like a disk was disconnected in the meantime.
			*/
			try {
				Melder_8to32_inplace_e (path8, path32, kMelder_textInputEncoding::UTF8);
			} catch (MelderError) {
				Melder_8to32_inplace_e (path8, path32, kMelder_textInputEncoding::MACROMAN);   // cannot fail
				Melder_throw (U"Unusual error finding or creating file <<", path32, U">> (MacRoman).");
			}
			Melder_throw (U"Unusual error finding or creating file ", path32, U".");
		}
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (nullptr, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormC);   // Praat requires composed characters
		integer n_utf16 = CFStringGetLength (cfpath2);
		integer n_utf32 = 0;
		for (integer i = 0; i < n_utf16; i ++) {
			char32 kar1 = CFStringGetCharacterAtIndex (cfpath2, i);
			if (kar1 >= 0x00'D800 && kar1 <= 0x00'DBFF) {
				char32 kar2 = (char32) CFStringGetCharacterAtIndex (cfpath2, ++ i);   // convert up
				if (kar2 >= 0x00'DC00 && kar2 <= 0x00'DFFF) {
					kar1 = (((kar1 & 0x3FF) << 10) | (kar2 & 0x3FF)) + 0x01'0000;
				} else {
					kar1 = UNICODE_REPLACEMENT_CHARACTER;
				}
			}
			path32 [n_utf32 ++] = kar1;
		}
		path32 [n_utf32] = U'\0';
		CFRelease (cfpath2);
	#else
		Melder_8to32_inplace_e (path8, path32, kMelder_textInputEncoding::UTF8);
	#endif
}
#endif

conststring32 MelderFile_name (MelderFile file) {
	#if defined (UNIX)
		char32 *slash = str32rchr (file -> path, U'/');
		return slash ? slash + 1 : file -> path;
	#elif defined (_WIN32)
		char32 *backslash = str32rchr (file -> path, U'\\');
		return backslash ? backslash + 1 : file -> path;
	#else
		return nullptr;
	#endif
}

conststring32 MelderFolder_name (MelderFolder folder) {
	#if defined (UNIX)
		char32 *slash = str32rchr (folder -> path, U'/');
		return slash ? slash + 1 : folder -> path;
	#elif defined (_WIN32)
		char32 *backslash = str32rchr (folder -> path, U'\\');
		return backslash ? backslash + 1 : folder -> path;
	#else
		return nullptr;
	#endif
}

void Melder_pathToFolder (conststring32 path, MelderFolder folder) {
	/*
		FIXME: document that this function can be used to keep folder names relative and generic
	*/
	Melder_sprint (folder -> path,kMelder_MAXPATH+1, path);
}

void Melder_pathToFile (conststring32 path, MelderFile file) {
	/*
		This handles complete path names only.

		Used if we know for sure that we have a complete path name,
		i.e. if the program determined the name (fileselector, printing, prefs).

		FIXME: document that this function can also be used to keep file names relative and generic
	*/
	Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
}

void Melder_relativePathToFile (conststring32 path, MelderFile file) {
	/*
		This handles complete and partial path names,
		and translates slashes to native folder separators.

		Used if we do not know for sure that we have a complete path name,
		i.e. if the user determined the name (scripting).
	*/
	#if defined (UNIX)
		/*
			We assume that Unix complete path names start with a slash.
		*/
		if (path [0] == U'~' && path [1] == U'/') {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, Melder_peek8to32_u (getenv ("HOME")), & path [1]);
		} else if (path [0] == U'/' || str32equ (path, U"<stdout>") || str32str (path, U"://")) {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
		} else {
			structMelderFolder currentFolder { };
			Melder_getCurrentFolder (& currentFolder);   // BUG
			if (currentFolder. path [0] == U'/' && currentFolder. path [1] == U'\0')
				Melder_sprint (file -> path,kMelder_MAXPATH+1, U"/", path);
			else
				Melder_sprint (file -> path,kMelder_MAXPATH+1, currentFolder. path, U"/", path);
		}
	#elif defined (_WIN32)
		/*
			We assume that Win32 complete path names look like:
				C:\WINDOWS\CTRL32.DLL
				LPT1:
				\\host\path
		*/
		structMelderFolder currentFolder { };
		if (path [0] == U'~' && path [1] == U'/') {
			Melder_getHomeDir (& currentFolder);
			Melder_sprint (file -> path,kMelder_MAXPATH+1, currentFolder. path, & path [1]);
			for (;;) {
				char32 *slash = str32chr (file -> path, U'/');
				if (! slash)
					break;
				*slash = U'\\';
			}
			return;
		}
		if (str32chr (path, U'/') && ! str32str (path, U"://")) {
			char32 winPath [kMelder_MAXPATH+1];
			Melder_sprint (winPath,kMelder_MAXPATH+1, path);
			for (;;) {
				char32 *slash = str32chr (winPath, U'/');
				if (! slash)
					break;
				*slash = U'\\';
			}
			Melder_relativePathToFile (winPath, file);
			return;
		}
		if (str32chr (path, U':') || path [0] == U'\\' && path [1] == U'\\' || str32equ (path, U"<stdout>")) {
			Melder_sprint (file -> path,kMelder_MAXPATH+1, path);
		} else {
			Melder_getCurrentFolder (& currentFolder);   // BUG
			Melder_sprint (file -> path,kMelder_MAXPATH+1,
				currentFolder. path,
				currentFolder. path [0] != U'\0' && currentFolder. path [Melder_length (currentFolder. path) - 1] == U'\\' ? U"" : U"\\",
				path
			);
		}
	#endif
}

void Melder_relativePathToFolder (conststring32 path, MelderFolder folder) {
	/*
		This handles complete, partial and home-relative path names,
		and translates slashes to native folder separators.

		Used if we do not know for sure that we have a complete path name,
		i.e. if the user determined the name (scripting).
	*/
	#if defined (UNIX)
		if (const bool pathIsHomeRelative = ( path [0] == U'~' && (path [1] == U'/' || path [1] == U'\0') )) {
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, Melder_peek8to32_u (getenv ("HOME")), & path [1]);
		} else if (const bool pathIsAbsolute = (
				path [0] == U'/'   // path is on local disk
				||
				str32str (path, U"://")   // path is on a service through a URL; is this needed and/or safe?
			))
		{
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, path);
		} else {
			/*
				Remaining case: the path must be current-folder-relative.
			*/
			structMelderFolder currentFolder { };
			Melder_getCurrentFolder (& currentFolder);   // BUG if in library? check in Python, for instance
			const bool weAreInTheRootFolder = ( currentFolder. path [0] == U'/' && currentFolder. path [1] == U'\0' );
			conststring32 folderSeparator = ( weAreInTheRootFolder ? nullptr : U"/" );   // prevent a double slash
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, currentFolder. path, folderSeparator, path);
		}
	#elif defined (_WIN32)
		/*
			We assume that Win32 complete path names look like:
				C:\WINDOWS\CTRL32.DLL
				LPT1:
				\\host\path
		*/
		structMelderFolder currentFolder { };
		if (path [0] == U'~' && path [1] == U'\0') {
			Melder_getHomeDir (& currentFolder);
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, currentFolder. path);
			return;
		}
		if (path [0] == U'~' && path [1] == U'/') {
			Melder_getHomeDir (& currentFolder);
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, currentFolder. path, & path [1]);
			for (;;) {
				char32 *slash = str32chr (folder -> path, U'/');
				if (! slash)
					break;
				*slash = U'\\';
			}
			return;
		}
		if (str32chr (path, U'/') && ! str32str (path, U"://")) {
			char32 winPath [kMelder_MAXPATH+1];
			Melder_sprint (winPath,kMelder_MAXPATH+1, path);
			for (;;) {
				char32 *slash = str32chr (winPath, U'/');
				if (! slash)
					break;
				*slash = U'\\';
			}
			Melder_relativePathToFolder (winPath, folder);
			return;
		}
		if (str32chr (path, U':') || path [0] == U'\\' && path [1] == U'\\') {
			Melder_sprint (folder -> path,kMelder_MAXPATH+1, path);
		} else {
			Melder_getCurrentFolder (& currentFolder);   // BUG
			Melder_sprint (folder -> path,kMelder_MAXPATH+1,
				currentFolder. path,
				currentFolder. path [0] != U'\0' && currentFolder. path [Melder_length (currentFolder. path) - 1] == U'\\' ? U"" : U"\\",
				path
			);
		}
	#endif
}

conststring32 MelderFile_peekPath (MelderFile file) {
	return & file -> path [0];
}
conststring32 MelderFolder_peekPath (MelderFolder folder) {
	return & folder -> path [0];
}

conststring8 MelderFile_peekPath8 (MelderFile file) {
	return Melder_peek32to8_fileSystem (MelderFile_peekPath (file));
}
conststring8 MelderFolder_peekPath8 (MelderFolder folder) {
	return Melder_peek32to8_fileSystem (MelderFolder_peekPath (folder));
}

#ifdef _WIN32
	conststringW MelderFile_peekPathW (MelderFile file) {
		return Melder_peek32toW_fileSystem (MelderFile_peekPath (file));
	}
	conststringW MelderFolder_peekPathW (MelderFolder folder) {
		return Melder_peek32toW_fileSystem (MelderFolder_peekPath (folder));
	}
#endif

#ifdef macintosh
	const void * MelderFile_peekPathCfstring (MelderFile file) {
		return Melder_peek32toCfstring_fileSystem (MelderFile_peekPath (file));
	}
	const void * MelderFolder_peekPathCfstring (MelderFolder folder) {
		return Melder_peek32toCfstring_fileSystem (MelderFolder_peekPath (folder));
	}
#endif

void MelderFile_copy (constMelderFile file, MelderFile copy) {
	str32cpy (copy -> path, file -> path);
}

void MelderFolder_copy (constMelderFolder folder, MelderFolder copy) {
	str32cpy (copy -> path, folder -> path);
}

bool MelderFile_equal (MelderFile file1, MelderFile file2) {
	return str32equ (file1 -> path, file2 -> path);
}

bool MelderFolder_equal (MelderFolder folder1, MelderFolder folder2) {
	return str32equ (folder1 -> path, folder2 -> path);
}

void MelderFile_setToNull (MelderFile file) {
	file -> path [0] = U'\0';
}

bool MelderFile_isNull (MelderFile file) {
	return ! file || file -> path [0] == U'\0';
}

void MelderFolder_setToNull (MelderFolder folder) {
	folder -> path [0] = U'\0';
}

bool MelderFolder_isNull (MelderFolder folder) {
	return folder -> path [0] == U'\0';
}

void MelderFolder_getFile (MelderFolder parent, conststring32 fileName, MelderFile file) {
	#if defined (UNIX)
		if (parent -> path [0] == U'/' && parent -> path [1] == U'\0')
			Melder_sprint (file -> path,kMelder_MAXPATH+1, U"/", fileName);
		else
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, U"/", fileName);
	#elif defined (_WIN32)
		if (str32rchr (file -> path, U'\\') - file -> path == Melder_length (file -> path) - 1)
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, fileName);
		else
			Melder_sprint (file -> path,kMelder_MAXPATH+1, parent -> path, U"\\", fileName);
	#endif
}

void MelderFolder_relativePathToFile (MelderFolder folder, conststring32 path, MelderFile file) {
	autoMelderSetCurrentFolder saveFolder (folder);
	Melder_relativePathToFile (path, file);
}

#ifndef UNIX
static void Melder_getDesktop (MelderFolder folder) {
	folder -> path [0] = U'\0';
}
#endif

void MelderFile_getParentFolder (MelderFile file, MelderFolder parent) {
	#if defined (UNIX)
		/*
			The parent of /usr/hello.txt is /usr.
			The parent of /hello.txt is /.
		*/
		str32cpy (parent -> path, file -> path);
		char32 *slash = str32rchr (parent -> path, U'/');
		if (slash)
			*slash = U'\0';
		if (parent -> path [0] == U'\0')
			str32cpy (parent -> path, U"/");
	#elif defined (_WIN32)
		/*
			The parent of C:\WINDOWS\CTRL.DLL is C:\WINDOWS.
			The parent of E:\Praat.exe is E:\.
			The parent of \\Swine\Apps\init.txt is \\Swine\Apps.
			The parent of \\Swine\init.txt is \\Swine\.   (BUG ?)
		*/
		str32cpy (parent -> path, file -> path);
		char32 *colon = str32chr (parent -> path, U':');
		if (colon) {
			char32 *backslash = str32rchr (parent -> path, U'\\');
			if (backslash) {   //   C:\WINDOWS\CTRL.DLL or C:\AUTOEXEC.BAT
				if (backslash - colon == 1) {   //   C:\AUTOEXEC.BAT
					* (backslash + 1) = U'\0';   //   C:\   -   !!! aargh this was a bug after converting this line to line comments
				} else {   //   C:\WINDOWS\CTRL.DLL
					*backslash = U'\0';   //   C:\WINDOWS
				}
			} else {   /* ??? */
				Melder_getDesktop (parent);   // empty string
			}
		} else if (parent -> path [0] == U'\\' && parent -> path [1] == U'\\') {
			char32 *backslash = str32rchr (parent -> path + 2, U'\\');
			if (backslash) {   //   \\Swine\Apps\init.txt or \\Swine\init.txt
				char32 *leftBackslash = str32chr (parent -> path + 2, U'\\');
				if (backslash - leftBackslash == 0) {   //   \\Swine\init.txt
					* (backslash + 1) = U'\0';   //   \\Swine\   -   !!! dear developer, don't delete this hyphen, lest the line ends in a backslash
				} else {   //   \\Swine\Apps\init.txt
					*backslash = U'\0';   //   \\Swine\Apps
				}
			} else {   //   \\init.txt   ???
				Melder_getDesktop (parent);   // empty string
			}
		} else {   // unknown path type
			Melder_getDesktop (parent);   // empty string
		}
	#endif
}

void MelderFolder_getParentFolder (MelderFolder subfolder, MelderFolder parentFolder) {
	#if defined (UNIX)
		/*
			The parent of /usr/local is /usr.
			The parent of /usr is /.
			The parent of / is "".
		*/
		str32cpy (parentFolder -> path, subfolder -> path);
		char32 *slash = str32rchr (parentFolder -> path, U'/');
		if (slash) {
			if (slash - parentFolder -> path == 0) {
				if (slash [1] == U'\0') {   // child is "/"
					parentFolder -> path [0] = U'\0';   // parent is ""
				} else {   // child is "/usr"
					slash [1] = '\0';   // parent is "/"
				}
			} else {   // child is "/usr/local"
				*slash = U'\0';   // parent is "/usr"
			}
		} else {
			parentFolder -> path [0] = U'\0';   // some failure; desktop
		}
	#elif defined (_WIN32)
		/*
			The parent of C:\WINDOWS is C:\.
			The parent of E:\ is the desktop.
			The parent of \\Swine\ is the desktop.   (BUG ?)
		*/
		str32cpy (parentFolder -> path, subfolder -> path);
		char32 *colon = str32chr (parentFolder -> path, U':');
		if (colon) {
			const integer length = Melder_length (parentFolder -> path);
			char32 *backslash = str32rchr (parentFolder -> path, U'\\');
			if (backslash) {   //   C:\WINDOWS\FONTS or C:\WINDOWS or C:\   - (cannot add a line comment with a backslash)
				if (backslash - parentFolder -> path == length - 1) {   //   C:\   -
					Melder_getDesktop (parentFolder);   // empty string
				} else if (backslash - colon == 1) {   //   C:\WINDOWS
					* (backslash + 1) = U'\0';   //   C:\   -
				} else {   //   C:\WINDOWS\FONTS
					*backslash = U'\0';   //   C:\WINDOWS
				}
			} else {   //   LPT1:   ???
				Melder_getDesktop (parentFolder);   // empty string
			}
		} else if (parentFolder -> path [0] == U'\\' && parentFolder -> path [1] == U'\\') {
			const integer length = Melder_length (parentFolder -> path);
			char32 *backslash = str32rchr (parentFolder -> path + 2, U'\\');
			if (backslash) {   //   \\Swine\Apps\Praats or \\Swine\Apps or \\Swine\   -
				if (backslash - parentFolder -> path == length - 1) {   //   \\Swine\   -
					Melder_getDesktop (parentFolder);   // empty string
				} else {   //   \\Swine\Apps\Praats or \\Swine\Apps
					char32 *leftBackslash = str32chr (parentFolder -> path + 2, U'\\');
					if (backslash - leftBackslash == 0) {   //   \\Swine\Apps
						* (backslash + 1) = U'\0';   //   \\Swine\   -
					} else {   //   \\Swine\Apps\Praats
						*backslash = U'\0';   //   \\Swine\Apps
					}
				}
			} else {   //   \\Swine   ???
				Melder_getDesktop (parentFolder);   // empty string
			}
		} else {   // unknown path type.
			Melder_getDesktop (parentFolder);   // empty string
		}
	#endif
}

bool MelderFolder_isDesktop (MelderFolder folder) {
	return folder -> path [0] == U'\0';
}

void MelderFolder_getSubfolder (MelderFolder parentFolder, conststring32 subfolderName, MelderFolder subfolder) {
	#if defined (UNIX)
		if (parentFolder -> path [0] == U'/' && parentFolder -> path [1] == U'\0') {
			Melder_sprint (subfolder -> path,kMelder_MAXPATH+1, U"/", subfolderName);
		} else {
			Melder_sprint (subfolder -> path,kMelder_MAXPATH+1, parentFolder -> path, U"/", subfolderName);
		}
	#elif defined (_WIN32)
		const integer length = Melder_length (parentFolder -> path);
		char32 *backslash = str32rchr (parentFolder -> path, U'\\');
		if (backslash && backslash - parentFolder -> path == length - 1) {   //   C:\ or \\Swine\   -
			Melder_sprint (subfolder -> path, kMelder_MAXPATH+1, parentFolder -> path, subfolderName);
		} else {   //   C:\WINDOWS or \\Swine\Apps or even C:
			Melder_sprint (subfolder -> path,kMelder_MAXPATH+1, parentFolder -> path, U"\\", subfolderName);
		}
	#endif
}

void Melder_getHomeDir (MelderFolder homeDir) {
	#if defined (UNIX)
		char *home = getenv ("HOME");
		Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, home ? Melder_peek8to32_u (home) : U"/");
	#elif defined (_WIN32)
		WCHAR driveW [kMelder_MAXPATH+1], pathW [kMelder_MAXPATH+1];
		DWORD n = GetEnvironmentVariableW (L"USERPROFILE", pathW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH)
			Melder_throw (U"Home folder name too long.");
		if (n > 0) {
			Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (pathW));
			return;
		}
		n = GetEnvironmentVariableW (L"HOMEDRIVE", driveW, kMelder_MAXPATH+1);
		if (n > kMelder_MAXPATH)
			Melder_throw (U"Home drive name too long.");
		if (n > 0) {
			GetEnvironmentVariable (L"HOMEPATH", pathW, kMelder_MAXPATH+1);
			Melder_sprint (homeDir -> path,kMelder_MAXPATH+1, Melder_peekWto32 (driveW), Melder_peekWto32 (pathW));
			return;
		}
		MelderFolder_setToNull (homeDir);   // Windows 95 and 98: alas
	#endif
}

static structMelderFolder thePreferencesFolder;
void Melder_setPreferencesFolder (conststring32 path) {
	Melder_pathToFolder (path, & thePreferencesFolder);
}
MelderFolder Melder_preferencesFolder() {
	return & thePreferencesFolder;
}
MelderFolder Melder_preferencesFolder5() {
	static structMelderFolder thePreferencesFolder5;
	if (MelderFolder_isNull (& thePreferencesFolder5)) {
		structMelderFolder homeFolder { };
		Melder_getHomeDir (& homeFolder);
		#if defined (macintosh)
			Melder_sprint (thePreferencesFolder5. path,kMelder_MAXPATH+1, homeFolder. path,
					U"/Library/Preferences/", Melder_upperCaseAppName(), U" Prefs");
		#elif defined (UNIX)
			Melder_sprint (thePreferencesFolder5. path,kMelder_MAXPATH+1, homeFolder. path,
					U"/.", Melder_lowerCaseAppName(), U"-dir");
		#elif defined (_WIN32)
			Melder_sprint (thePreferencesFolder5. path,kMelder_MAXPATH+1, homeFolder. path,
					U"\\", Melder_upperCaseAppName());
		#endif
	}
	return & thePreferencesFolder5;
}
MelderFolder Melder_preferencesFolder7() {
	static structMelderFolder thePreferencesFolder7;
	if (MelderFolder_isNull (& thePreferencesFolder7)) {
		structMelderFolder homeFolder { };
		Melder_getHomeDir (& homeFolder);
		#if defined (macintosh)
			Melder_sprint (thePreferencesFolder7. path,kMelder_MAXPATH+1, homeFolder. path,
					U"/Library/Application Support/", Melder_upperCaseAppName());
		#elif defined (UNIX)
			Melder_sprint (thePreferencesFolder7. path,kMelder_MAXPATH+1, homeFolder. path,
					U"/.config/", Melder_lowerCaseAppName());
		#elif defined (_WIN32)
			Melder_sprint (thePreferencesFolder7. path,kMelder_MAXPATH+1, homeFolder. path,
					U"\\AppData\\Roaming\\", Melder_upperCaseAppName());
		#endif
	}
	return & thePreferencesFolder7;
}

void Melder_getTempDir (MelderFolder temporaryFolder) {
	#if defined (macintosh)
		Melder_sprint (temporaryFolder -> path,kMelder_MAXPATH+1, Melder_peek8to32_u (getenv ("TMPDIR")));   // or append /TemporaryItems
		// confstr with _CS_DARWIN_USER_TEMP_DIR
	#else
		(void) temporaryFolder;
	#endif
}

FILE * Melder_fopen (MelderFile file, const char *type) {
	if (MelderFile_isNull (file))
		Melder_throw (U"Cannot open null file.");
	if (! Melder_isTracingGlobally)
		Melder_assert (str32equ (Melder_double (1.5), U"1.5"));   // check locale settings; because of the required file portability Praat cannot stand "1,5"
	/*
		On the Unix-like systems (including MacOS), the path has to be converted to 8-bit characters in UTF-8 encoding.
		On MacOS, the characters also have to be decomposed.
		On Windows, the characters have to be precomposed.
	*/
	bool isFolder = false;
	char utf8path [kMelder_MAXPATH+1];
	Melder_32to8_fileSystem_inplace (file -> path, utf8path);
	FILE *f;
	file -> openForWriting = ( type [0] == 'w' || type [0] == 'a' || strchr (type, '+') );
	if (str32equ (file -> path, U"<stdout>") && file -> openForWriting) {
		f = Melder_stdout;
	} else {
		//TRACE
		#if defined (_WIN32) && ! defined (__CYGWIN__)
			f = _wfopen (MelderFile_peekPathW (file), Melder_peek32toW (Melder_peek8to32_u (type)));
		#else
			struct stat statbuf;
			int status = stat ((char *) utf8path, & statbuf);
			if (status == -1) {
				if (type [0] == 'r')
					f = nullptr;   // and wait for errno to tell us why
				else
					f = fopen ((char *) utf8path, type);
			} else if (S_ISDIR (statbuf. st_mode)) {
				isFolder = true;
				trace (U"A folder is not a file!");
				f = nullptr;
			} else {
				f = fopen ((char *) utf8path, type);
			}
		#endif
	}
	if (! f) {
		char32 *path = file -> path;
		Melder_appendError (
			( errno == EPERM || errno == EACCES ? U"No permission to " : U"Cannot " ),
			( type [0] == 'r' ? U"open" : type [0] == 'a' ? U"append to" : U"create" ),
			U" file ", file, U"."
		);
		if (errno == EIO)
			Melder_appendError (U"Not-so-useful hint: an error occurred while reading from the file system.");
		else if (errno == ELOOP)
			Melder_appendError (U"Hint: the file path contains a loop of symbolic links.");
		else if (errno == ENAMETOOLONG)
			Melder_appendError (U"Not-so-useful hint: the file path may be too long. This should not occur.");
		else if (errno == ENOENT)
			if (file -> openForWriting)
				Melder_appendError (U"Hint: one of the folders in this file path does not exist.");
			else
				Melder_appendError (U"Hint: one of the folders or files in this file path does not exist.");
		else if (errno == ENOTDIR)
			Melder_appendError (U"Hint: a component of the file path is not a folder.");
		else if (errno == EOVERFLOW)
			Melder_appendError (U"Not-so-useful hint: the file size is too big.");
		else if (errno != 0)
			Melder_appendError (U"Not-so-useful hint: unexpected error ", errno, U".");
		if (isFolder)
			Melder_appendError (U"Hint: this is a folder, not a file.");
		else if (path [0] == U'\0')
			Melder_appendError (U"Hint: empty file name.");
		else if (path [0] == U' ' || path [0] == U'\t')
			Melder_appendError (U"Hint: file name starts with a space or tab.");
		else if (path [Melder_length (path) - 1] == U' ' || path [Melder_length (path) - 1] == U'\t')
			Melder_appendError (U"Hint: file name ends in a space or tab.");
		else if (str32chr (path, U'\n'))
			Melder_appendError (U"Hint: file name contains a newline symbol.");
		throw MelderError ();
		return nullptr;
	}
	return f;
}

void Melder_fclose (MelderFile file, FILE *f) {
	if (! f)
		return;
	if (f != Melder_stdout && fclose (f) == EOF)
		Melder_throw (U"Error closing file ", file, U".");
}

bool MelderFile_exists (MelderFile file) {
	#if defined (UNIX)
		struct stat fileOrFolderStatus;
		const bool exists = ( stat (MelderFile_peekPath8 (file), & fileOrFolderStatus) == 0 );
		if (! exists)
			return false;
		return ! S_ISDIR (fileOrFolderStatus. st_mode);
	#else
		DWORD fileOrFolderAttributes = GetFileAttributesW (MelderFile_peekPathW (file));
		if (fileOrFolderAttributes == INVALID_FILE_ATTRIBUTES)
			return false;
		return (fileOrFolderAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
	#endif
}

bool MelderFolder_exists (MelderFolder folder) {
	#if defined (UNIX)
		struct stat fileOrFolderStatus;
		const bool exists = ( stat (MelderFolder_peekPath8 (folder), & fileOrFolderStatus) == 0 );
		if (! exists)
			return false;
		return S_ISDIR (fileOrFolderStatus. st_mode);
	#else
		DWORD fileOrFolderAttributes = GetFileAttributesW (MelderFolder_peekPathW (folder));
		if (fileOrFolderAttributes == INVALID_FILE_ATTRIBUTES)
			return false;
		return (fileOrFolderAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	#endif
}

bool MelderFile_readable (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

bool Melder_tryToWriteFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "wb");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

bool Melder_tryToAppendFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "ab");
		f.close (file);
		return true;
	} catch (MelderError) {
		Melder_clearError ();
		return false;
	}
}

integer MelderFile_length (MelderFile file) {
	#if defined (UNIX)
		struct stat statistics;
		if (stat (MelderFile_peekPath8 (file), & statistics) != 0)
			return -1;
		return statistics. st_size;
	#else
		try {
			autofile f = Melder_fopen (file, "r");
			fseek (f, 0, SEEK_END);
			integer length = ftell (f);
			f.close (file);
			return length;
		} catch (MelderError) {
			Melder_clearError ();
			return -1;
		}
	#endif
}

void MelderFile_delete (MelderFile file) {
	if (! file)
		return;
	#if defined (UNIX)
		remove (MelderFile_peekPath8 (file));
	#elif defined (_WIN32)
		DeleteFile (MelderFile_peekPathW (file));
	#endif
}

void MelderFile_moveAndOrRename (MelderFile fromFile, MelderFile toFile) {
	if (! fromFile || ! toFile)
		return;
	try {
		#if defined (UNIX)
			static char fromPath8 [1 + kMelder_MAXPATH], toPath8 [1 + kMelder_MAXPATH];
			Melder_32to8_fileSystem_inplace (MelderFile_peekPath (fromFile), fromPath8);   // not MelderFile_peekPath8, because of static
			Melder_32to8_fileSystem_inplace (MelderFile_peekPath (toFile), toPath8);
			const int result = rename (fromPath8, toPath8);
		#elif defined (_WIN32)
			autostringW fromPathW = Melder_32toW_fileSystem (MelderFile_peekPath (fromFile));
			autostringW toPathW = Melder_32toW_fileSystem (MelderFile_peekPath (toFile));
			const int result = _wrename (fromPathW.get(), toPathW.get());
		#endif
		if (result == 0)
			return;   // success
		switch (errno) {
			case EACCES:
				Melder_throw (U"A folder in one of the paths doesn’t allow reading and/or writing access.");
			case EBUSY:
				Melder_throw (U"A folder in one of the paths is busy.");
			case EINVAL:
				Melder_throw (U"Invalid path (recursive? dots?).");
			case EIO:
				Melder_throw (U"Physical I/O error.");
			case EISDIR:
				Melder_throw (U"Cannot move an/or rename a file to a folder name.");
			#if ! defined (UNIX) && ! defined (_WIN32)
				case EMLOOP:
					Melder_throw (U"Loop in symbolic links.");
			#endif
			case EMLINK:
				Melder_throw (U"Too many links.");
			case ENAMETOOLONG:
				Melder_throw (U"Name too long.");
			case ENOENT:
				Melder_throw (U"A folder or file in the path does not exist.");
			case ENOSPC:
				Melder_throw (U"Folder cannot be extended.");
			case ENOTDIR:
				Melder_throw (U"A part of the path is not a folder (hint: look at the slahes).");
			case EPERM:
				Melder_throw (U"No permission to read and/or write in one of the paths.");
			case EROFS:
				Melder_throw (U"Cannot do links between file systems.");
			#if ! defined (UNIX) && ! defined (_WIN32)
				case ETXTBUSY:
					Melder_throw (U"Busy pure procedure file.");
			#endif
			default:
				Melder_throw (U"Unknown error ", errno, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"Could not rename ", fromFile, U" to ", toFile, U".");
	}
}

char32 * Melder_peekExpandBackslashes (conststring32 message) {
	static char32 names [11] [kMelder_MAXPATH+1];
	static int index = 0;
	if (++ index == 11)
		index = 0;
	char32 *to = & names [index] [0];
	for (const char32 *from = & message [0]; *from != '\0'; from ++, to ++) {
		*to = *from;
		if (*from == U'\\') {
			* ++ to = U'b';
			* ++ to = U's';
		}
	}
	*to = U'\0';
	return & names [index] [0];
}

conststring32 MelderFile_messageName (MelderFile file) {
	return Melder_cat (U"“", file -> path, U"”");   // BUG: is cat allowed here?
}

conststring32 MelderFolder_messageName (MelderFolder folder) {
	return Melder_cat (U"“", folder -> path, U"”");   // BUG: is cat allowed here?
}

#if defined (UNIX)
	/*
		From macOS 10.15 Catalina on, getcwd() has failed if a part of the path
		is inaccessible, such as when you open a script that is attached to an email message.
	*/
	static structMelderFolder theDefaultDir;
#endif

void Melder_getCurrentFolder (MelderFolder folder) {
	#if defined (UNIX)
		char path [kMelder_MAXPATH+1];
		char *pathResult = getcwd (path, kMelder_MAXPATH+1);
		if (pathResult)
			Melder_8bitFileRepresentationToStr32_inplace (path, folder -> path);
		else if (errno == EPERM)
			str32cpy (folder -> path, theDefaultDir. path);
		else
			Melder_throw (Melder_peek8to32_u (strerror (errno)));
		Melder_assert (Melder_length (folder -> path) <= kMelder_MAXPATH);
	#elif defined (_WIN32)
		static WCHAR folderPathW [kMelder_MAXPATH+1];
		GetCurrentDirectory (kMelder_MAXPATH+1, folderPathW);
		Melder_sprint (folder -> path,kMelder_MAXPATH+1, Melder_peekWto32 (folderPathW));
	#endif
}

void Melder_setCurrentFolder (MelderFolder folder) {
	#if defined (UNIX)
		chdir (MelderFolder_peekPath8 (folder));
		str32cpy (theDefaultDir. path, folder -> path);
	#elif defined (_WIN32)
		SetCurrentDirectory (MelderFolder_peekPathW (folder));
	#endif
}

void MelderFile_setDefaultDir (MelderFile file) {
	structMelderFolder folder { };
	MelderFile_getParentFolder (file, & folder);
	Melder_setCurrentFolder (& folder);
}

MelderFolder Melder_peekWorkingDirectory () {
	static structMelderFolder s_workingDirectory;
	Melder_getCurrentFolder (& s_workingDirectory);
	return & s_workingDirectory;
}

void Melder_createDirectory (MelderFolder parent, conststring32 folderName, int mode) {
#if defined (UNIX)
	structMelderFile file { };
	if (folderName [0] == U'/') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, folderName);   // absolute path
	} else if (parent -> path [0] == U'/' && parent -> path [1] == U'\0') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, U"/", folderName);   // relative path in root folder
	} else {
		Melder_sprint (file. path,kMelder_MAXPATH+1, parent -> path, U"/", folderName);   // relative path
	}
	if (mkdir (MelderFile_peekPath8 (& file), mode) == -1 && errno != EEXIST)   // ignore if folder already exists
		Melder_throw (U"Cannot create directory ", & file, U".");
#elif defined (_WIN32)
	structMelderFile file { };
	SECURITY_ATTRIBUTES sa;
	(void) mode;
	sa. nLength = sizeof (SECURITY_ATTRIBUTES);
	sa. lpSecurityDescriptor = nullptr;
	sa. bInheritHandle = false;
	if (str32chr (folderName, U':') || folderName [0] == U'/' && folderName [1] == U'/') {
		Melder_sprint (file. path,kMelder_MAXPATH+1, folderName);   // absolute path
	} else {
		Melder_sprint (file. path,kMelder_MAXPATH+1, parent -> path, U"/", folderName);   // relative path
	}
	if (! CreateDirectoryW (MelderFile_peekPathW (& file), & sa) && GetLastError () != ERROR_ALREADY_EXISTS)   // ignore if folder already exists
		Melder_throw (U"Cannot create directory ", & file, U".");
#else
	//#error Unsupported operating system.
#endif
}

void MelderFolder_create (MelderFolder folder) {
	#if defined (UNIX)
		const int status = mkdir (MelderFolder_peekPath8 (folder), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (status == 0)
			return;   // successfully created a new folder
		if (errno == EEXIST)
			return;   // it is no failure if the folder already existed
		Melder_throw (U"Cannot create folder ", folder, U".");
	#elif defined (_WIN32)
		SECURITY_ATTRIBUTES securityAttributes;
		securityAttributes. nLength = sizeof (SECURITY_ATTRIBUTES);
		securityAttributes. lpSecurityDescriptor = nullptr;
		securityAttributes. bInheritHandle = false;
		const int status = CreateDirectoryW (MelderFolder_peekPathW (folder), & securityAttributes);
		if (status != 0)
			return;   // successfully created a new folder
		if (GetLastError () == ERROR_ALREADY_EXISTS)
			return;   // it is no failure if the folder already existed
		Melder_throw (U"Cannot create folder ", folder, U".");
	#else
		#error Unsupported operating system.
	#endif
}
void Melder_createFolder (conststring32 path) {
	structMelderFolder folder { };
	Melder_relativePathToFolder (path, & folder);
	MelderFolder_create (& folder);
}

static size_t fread_multi (char *buffer, size_t numberOfBytes, FILE *f) {
	off_t offset = 0;
	size_t numberOfBytesRead = 0;
	const size_t chunkSize = 1'000'000'000;
	while (numberOfBytes > chunkSize) {
		size_t numberOfBytesReadInChunk = fread (buffer + offset, sizeof (char), chunkSize, f);
		numberOfBytesRead += numberOfBytesReadInChunk;
		if (numberOfBytesReadInChunk < chunkSize)
			return numberOfBytesRead;
		numberOfBytes -= chunkSize;
		offset += chunkSize;
	}
	size_t numberOfBytesReadInLastChunk = fread (buffer + offset, sizeof (char), numberOfBytes, f);
	numberOfBytesRead += numberOfBytesReadInLastChunk;
	return numberOfBytesRead;
}

autostring32 MelderFile_readText (MelderFile file, autostring8 *string8) {
	try {
		int type = 0;   // 8-bit
		autostring32 text;
		autofile f = Melder_fopen (file, "rb");
		if (fseeko (f, 0, SEEK_END) < 0)
			Melder_throw (U"Cannot count the bytes in the file.");
		Melder_assert (sizeof (off_t) >= 8);
		int64 length = ftello (f);
		rewind (f);
		if (length >= 2) {
			int firstByte = fgetc (f), secondByte = fgetc (f);
			if (firstByte == 0xFE && secondByte == 0xFF) {
				type = 1;   // big-endian 16-bit
			} else if (firstByte == 0xFF && secondByte == 0xFE) {
				type = 2;   // little-endian 16-bit
			} else if (firstByte == 0xEF && secondByte == 0xBB && length >= 3) {
				int thirdByte = fgetc (f);
				if (thirdByte == 0xBF)
					type = -1;   // UTF-8 with BOM
			}
		}
		if (type <= 0) {
			if (type == -1) {
				length -= 3;
				fseeko (f, 3, SEEK_SET);
			} else {
				rewind (f);   // length and type already set correctly
			}
			autostring8 text8bit (length);
			Melder_assert (text8bit);
			size_t numberOfBytesRead = fread_multi (text8bit.get(), (size_t) length, f);
			Melder_require ((int64) numberOfBytesRead == length,
				U"The file contains ", length, U" bytes",
				type == -1 ? U" after the byte-order mark" : U"",
				U", but we could read only ", numberOfBytesRead, U" of them."
			);
			text8bit [length] = '\0';
			/*
				Count and repair null bytes.
			*/
			if (length > 0) {
				int64 numberOfNullBytes = 0;
				char *q = & text8bit [0];
				for (integer i = 0; i < length; i ++)
					if (text8bit [i] != '\0')
						* (q ++) = text8bit [i];
					else
						numberOfNullBytes ++;
				*q = '\0';
				if (numberOfNullBytes > 0)
					Melder_warning (U"Ignored ", numberOfNullBytes, U" null bytes in text file ", file, U".");
			}
			if (string8) {
				*string8 = text8bit.move();
				(void) Melder_killReturns_inplace (string8->get());
				return autostring32();   // OK
			} else {
				text = Melder_8to32_e (text8bit.get(), kMelder_textInputEncoding::UNDEFINED);
			}
		} else {
			length = length / 2 - 1;   // Byte Order Mark subtracted. Length = number of UTF-16 codes
			text = autostring32 (length + 1);
			if (type == 1) {
				for (int64 i = 0; i < length; i ++) {
					const char16 kar1 = bingetu16 (f);
					if (kar1 < 0xD800) {
						text [i] = (const char32) kar1;   // convert up without sign extension
					} else if (kar1 < 0xDC00) {
						length --;
						const char16 kar2 = bingetu16 (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = (const char32) (0x010000 +
									(const char32) (((const char32) kar1 & 0x0003FF) << 10) +
									(const char32)  ((const char32) kar2 & 0x0003FF));
						} else {
							text [i] = UNICODE_REPLACEMENT_CHARACTER;
						}
					} else if (kar1 < 0xE000) {
						text [i] = UNICODE_REPLACEMENT_CHARACTER;
					} else {
						text [i] = (const char32) kar1;   // convert up without sign extension
					}
				}
			} else {
				for (int64 i = 0; i < length; i ++) {
					const char16 kar1 = bingetu16LE (f);
					if (kar1 < 0xD800) {
						text [i] = (const char32) kar1;   // convert up without sign extension
					} else if (kar1 < 0xDC00) {
						length --;
						const char16 kar2 = bingetu16LE (f);
						if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
							text [i] = (const char32) (0x01'0000 +
								(const char32) (((const char32) kar1 & 0x00'03FF) << 10) +
								(const char32)  ((const char32) kar2 & 0x00'03FF));
						} else {
							text [i] = UNICODE_REPLACEMENT_CHARACTER;
						}
					} else if (kar1 < 0xE000) {
						text [i] = UNICODE_REPLACEMENT_CHARACTER;
					} else if (kar1 <= 0xFFFF) {
						text [i] = (const char32) kar1;   // convert up without sign extension
					} else {
						Melder_crash (U"MelderFile_readText: unsigned short greater than 0xFFFF: should not occur.");
					}
				}
			}
			text [length] = U'\0';
			(void) Melder_killReturns_inplace (text.get());
		}
		f.close (file);
		return text;
	} catch (MelderError) {
		Melder_throw (U"Error reading file ", file, U".");
	}
}

void Melder_fwrite32to8 (conststring32 string, FILE *f) {
	/*
	 * Precondition:
	 *    the string's encoding is UTF-32.
	 * Failure:
	 *    if the precondition does not hold, we don't crash,
	 *    but the characters that are written may be incorrect.
	 */
	for (const char32* p = string; *p != U'\0'; p ++) {
		const char32 kar = *p;
		if (kar <= 0x00'007F) {
			#ifdef _WIN32
				if (kar == U'\n')
					fputc (13, f);
			#endif
			fputc ((int) kar, f);   // because fputc wants an int instead of an uint8 (guarded conversion)
		} else if (kar <= 0x00'07FF) {
			fputc (0xC0 | (kar >> 6), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		} else if (kar <= 0x00FFFF) {
			fputc (0xE0 | (kar >> 12), f);
			fputc (0x80 | ((kar >> 6) & 0x00'003F), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		} else {
			fputc (0xF0 | (kar >> 18), f);
			fputc (0x80 | ((kar >> 12) & 0x00'003F), f);
			fputc (0x80 | ((kar >> 6) & 0x00'003F), f);
			fputc (0x80 | (kar & 0x00'003F), f);
		}
	}
}

void MelderFile_writeText (MelderFile file, conststring32 text, kMelder_textOutputEncoding outputEncoding) {
	if (! text)
		text = U"";
	autofile f = Melder_fopen (file, "wb");
	if (outputEncoding == kMelder_textOutputEncoding::UTF8) {
		Melder_fwrite32to8 (text, f);
	} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isValidAscii (text)) ||
		(outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
	{
		#ifdef _WIN32
			#define flockfile(f)  (void) 0
			#define funlockfile(f)  (void) 0
			#define putc_unlocked  putc
		#endif
		flockfile (f);
		const integer n = Melder_length (text);
		for (integer i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n')
					putc_unlocked (13, f);
			#endif
			putc_unlocked (kar, f);
		}
		funlockfile (f);
	} else {
		binputu16 (0xFEFF, f);   // Byte Order Mark
		const integer n = Melder_length (text);
		for (integer i = 0; i < n; i ++) {
			char32 kar = text [i];
			#ifdef _WIN32
				if (kar == U'\n')
					binputu16 (13, f);
			#endif
			if (kar <= 0x00'FFFF) {
				binputu16 ((char16) kar, f);   // guarded conversion down
			} else if (kar <= 0x10'FFFF) {
				kar -= 0x010000;
				binputu16 (0xD800 | (uint16) (kar >> 10), f);
				binputu16 (0xDC00 | (uint16) ((char16) kar & 0x3ff), f);
			} else {
				binputu16 (UNICODE_REPLACEMENT_CHARACTER, f);
			}
		}
	}
	f.close (file);
}

void MelderFile_appendText (MelderFile file, conststring32 text) {
	if (! text) text = U"";
	autofile f1;
	try {
		f1 = Melder_fopen (file, "rb");
	} catch (MelderError) {
		Melder_clearError ();   // it's OK if the file didn't exist yet...
		MelderFile_writeText (file, text, Melder_getOutputEncoding ());   // because then we just "write"
		return;
	}
	/*
		The file already exists and is open. Determine its type.
	*/
	int firstByte = fgetc (f1), secondByte = fgetc (f1);
	f1.close (file);
	int type = 0;
	if (firstByte == 0xfe && secondByte == 0xff) {
		type = 1;   // big-endian 16-bit
	} else if (firstByte == 0xff && secondByte == 0xfe) {
		type = 2;   // little-endian 16-bit
	}
	if (type == 0) {
		kMelder_textOutputEncoding outputEncoding = Melder_getOutputEncoding ();
		if (outputEncoding == kMelder_textOutputEncoding::UTF8) {   // TODO: read as file's encoding
			autofile f2 = Melder_fopen (file, "ab");
			Melder_fwrite32to8 (text, f2);
			f2.close (file);
		} else if ((outputEncoding == kMelder_textOutputEncoding::ASCII_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ASCII))
		    || (outputEncoding == kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16 && Melder_isEncodable (text, kMelder_textOutputEncoding_ISO_LATIN1)))
		{
			/*
				Append ASCII or ISOLatin1 text to ASCII or ISOLatin1 file.
			*/
			autofile f2 = Melder_fopen (file, "ab");
			const integer n = Melder_length (text);
			for (integer i = 0; i < n; i ++) {
				const char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						fputc (13, f2);
				#endif
				fputc ((char8) kar, f2);
			}
			f2.close (file);
		} else {
			/*
				Convert to wide character file.
			*/
			autostring32 oldText = MelderFile_readText (file);
			autofile f2 = Melder_fopen (file, "wb");
			binputu16 (0xfeff, f2);
			integer n = Melder_length (oldText.get());
			for (int64 i = 0; i < n; i ++) {
				char32 kar = oldText [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			n = Melder_length (text);
			for (integer i = 0; i < n; i ++) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
			f2.close (file);
		}
	} else {
		autofile f2 = Melder_fopen (file, "ab");
		const integer n = Melder_length (text);
		for (integer i = 0; i < n; i ++) {
			if (type == 1) {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16 (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16 ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10'FFFF) {
					kar -= 0x01'0000;
					binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16 (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			} else {
				char32 kar = text [i];
				#ifdef _WIN32
					if (kar == U'\n')
						binputu16LE (13, f2);
				#endif
				if (kar <= 0x00'FFFF) {
					binputu16LE ((uint16) kar, f2);   // guarded conversion down
				} else if (kar <= 0x10FFFF) {
					kar -= 0x01'0000;
					binputu16LE ((uint16) (0x00'D800 | (kar >> 10)), f2);
					binputu16LE ((uint16) (0x00'DC00 | (kar & 0x00'03ff)), f2);
				} else {
					binputu16LE (UNICODE_REPLACEMENT_CHARACTER, f2);
				}
			}
		}
		f2.close (file);
	}
}

/* End of file melder_files.cpp */
