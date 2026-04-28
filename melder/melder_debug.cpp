/* melder_debug.cpp
 *
 * Copyright (C) 2000-2026 Paul Boersma
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

#include "melder.h"
#ifdef linux
	#include "../sys/GuiP.h"
#endif
#include <time.h>
#ifdef _WIN32
	#include "../kar/UnicodeData.h"
	#include <windows.h>
#endif

/* global */ std::mutex theMelder_trace_mutex;

/*
Melder_debug will always be set to 0 when Praat starts up.
If Melder_debug is temporarily set to the following values
(preferably with the "Debug..." command under Praat->Technical,
 which you can use from a script),
the behaviour of Praat will temporarily change in the following ways:

1: Windows: use C-clock instead of multimedia-clock in melder_audio.cpp.
2: Windows: always reset waveOut, even when played to end, in melder_audio.cpp.
3: Windows: reset waveOut if unprepareHeader fails, and retry, in melder_audio.cpp. STAY: 20010214
4: Windows: fewer callbacks during sound play, in melder_audio.cpp. STAY: 20010214
6: Windows: info on metafile properties in Picture.cpp.
8: Windows: don't reset waveIn, in SoundRecorder.cpp.
9: flush Error in FunctionEditor_Sound_draw
10: geometric pens
11: clicked item in option menu in Ui.cpp
14: switches off the progress window in melder.cpp
15: don't use TrueType IPA fonts, but always bitmaps instead
16: Linux: open /dev/dsp without O_NDELAY
17: debugging on in Formula.cpp
18: no endian assumptions in abcio.cpp
19: show path name in UiOutfile_do
20: trace PortAudio
21: Mac: list supported document formats when printing
22: UTF-8 tests in logo
23: recognize special chunks in WAV files
24: measure buttons in drawing area
25: read crooked Manipulation files (January 2008)
26: force OT-GLA
27: force HG-GLA
28: don't use GSL in NUMfisherQ
29: use GSL in NUMinvFisherQ
30: pitch path finder: use octave jump cost across voiceless parts
31: Pitch analysis: formant pulling on
32: show info on file names in ExperimentMFC
33: trace the Pitch path finder
34: trace memory allocation and deallocation
35: debugging on for QuickTime movie file opening
37: debug autoThing
39: debug autostring
40: debug Thing_new
41: OTGrammar_getWinner: always first choice rather than random choice
42: OTGrammar_getWinner: always last choice rather than random choice
43: trace class table initialization
44: trace Collection
45: tracing structMatrix :: read ()
46: trace GTK parent sizes in _GuiObject_position ()
47: force resampling in OTGrammar RIP
48: compute sum, mean, stdev with naive implementation in double (64 bits)
49: compute sum, mean, stdev with naive implementation in longdouble (80 bits on Intel64)
50: compute sum, mean, stdev with first-element offset (80 bits on Intel64)
51: compute sum, mean, stdev with two cycles, as in R (80 bits on Intel64)
(other numbers than 48-51: compute sum, mean, stdev with simple pairwise algorithm, base case 64 [80 bits on Intel64])
52: debug Discriminant_TableOfReal_to_ClassificationTable
53: trace running cursor
54: ignore gdk_cairo_reset_clip
55: trace Gui init, draw, destroy
56: trace text styles
57: no parabolic interpolation in Sound_Pitch_to_PointProcess_cc (March 2024)
58: list font replacements at start-up (December 2025)
181: read and write native-endian real64
900: use DG Meta Serif Science instead of Palatino
1264: Mac: Sound_record_fixedTime uses microphone "FW Solo (1264)"
2001: trace timing for whisper.cpp
2002: use greedy sampling strategy for whisper.cpp (instead of the beam search)

(negative values are for David)

*/

/*
	In order to make sure that Melder_casual() and trace() can be called from anywhere,
	including e.g. from Melder_realloc() or Melder_free(),
	they cannot use any Melder_xxx() functions.
*/

/*
	peek32to8 substitutes for Melder_peek32to8(),
	which can call Melder_realloc() and Melder_free();
	also, we need no newline nativization, as Melder_32to8_inplace() does.
*/
conststring8 MelderTrace::_peek32to8 (conststring32 string) {
	if (! string)
		return "";
	static char *buffer { nullptr };
	static int64 bufferSize { 0 };
	const int64 n = Melder_length (string);
	int64 sizeNeeded = n * 4 + 1;
	if ((bufferSize - sizeNeeded) * (int64) sizeof (char) >= 10'000) {
		free (buffer);
		buffer = nullptr;   // undangle
		bufferSize = 0;
	}
	if (sizeNeeded > bufferSize) {
		sizeNeeded = (int64) floor (sizeNeeded * 1.61803) + 100;
		buffer = (char *) realloc (buffer, (size_t) sizeNeeded * sizeof (char));
		if (buffer == nullptr) {
			bufferSize = 0;
			return "(out of memory during tracing)";
		}
		bufferSize = sizeNeeded;
	}
	int64 i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00'007F) {   // 7 bits
			buffer [j ++] = (char) (char8) kar;   // guarded truncation
		} else if (kar <= 0x00'07FF) {   // 11 bits
			buffer [j ++] = (char) (char8) (0x00'00C0 | (kar >> 6));   // the upper 5 bits yield a number between 0xC4 and 0xDF
			buffer [j ++] = (char) (char8) (0x00'0080 | (kar & 0x00'003F));   // the lower 6 bits yield a number between 0x80 and 0xBF
		} else if (kar <= 0x00'FFFF) {   // 16 bits
			buffer [j ++] = (char) (char8) (0x00'00E0 | (kar >> 12));   // the upper 4 bits yield a number between 0xE0 and 0xEF
			buffer [j ++] = (char) (char8) (0x00'0080 | ((kar >> 6) & 0x00'003F));
			buffer [j ++] = (char) (char8) (0x00'0080 | (kar & 0x00'003F));
		} else {   // 21 bits
			buffer [j ++] = (char) (char8) (0x00'00F0 | (kar >> 18));   // the upper 3 bits yield a number between 0xF0 and 0xF4 (0x10FFFF >> 18 == 4)
			buffer [j ++] = (char) (char8) (0x00'0080 | ((kar >> 12) & 0x00'003F));   // the next 6 bits
			buffer [j ++] = (char) (char8) (0x00'0080 | ((kar >> 6) & 0x00'003F));   // the third 6 bits
			buffer [j ++] = (char) (char8) (0x00'0080 | (kar & 0x00'003F));   // the lower 6 bits
		}
	}
	buffer [j] = '\0';
	return buffer;
}
#ifdef _WIN32
conststring16 MelderTrace::_peek32to16 (conststring32 string) {
	if (! string)
		return u"";
	static char16 *buffer { nullptr };
	static int64 bufferSize { 0 };
	const int64 n = Melder_length (string);
	int64 sizeNeeded = n * 2 + 1;
	if ((bufferSize - sizeNeeded) * (int64) sizeof (char16) >= 10'000) {
		free (buffer);
		bufferSize = 0;
	}
	if (sizeNeeded > bufferSize) {
		sizeNeeded = (int64) floor (sizeNeeded * 1.61803) + 100;
		buffer = (char16 *) realloc (buffer, (size_t) sizeNeeded * sizeof (char16));
		if (! buffer) {
			bufferSize = 0;
			return u"(out of memory during tracing)";
		}
		bufferSize = sizeNeeded;
	}
	int64 i, j;
	for (i = 0, j = 0; i < n; i ++) {
		char32 kar = string [i];
		if (kar <= 0x00'D7FF) {   // 16 bits
			buffer [j ++] = (char16) kar;   // guarded truncation
		} else if (kar <= 0x00'DFFF) {   // 16 bits
			buffer [j ++] = (char16) UNICODE_REPLACEMENT_CHARACTER;   // forbidden for UTF-32
		} else if (kar <= 0x00'FFFF) {   // 16 bits
			buffer [j ++] = (char16) kar;   // guarded truncation
		} else if (kar <= 0x10'FFFF) {   // 21 bits
			kar -= 0x01'0000;
			buffer [j ++] = (char16) (0x00'D800 | (kar >> 10));   // guarded truncation
			buffer [j ++] = (char16) (0x00'DC00 | (kar & 0x00'03FF));   // guarded truncation
		} else {   // 21 bits
			buffer [j ++] = (char16) UNICODE_REPLACEMENT_CHARACTER;
		}
	}
	buffer [j] = u'\0';
	return buffer;
}
#endif

conststring32 MelderTrace::_peek8to32 (conststring8 string8) {
	static MelderString buffers [19];
	static int ibuffer = 0;
	if (++ ibuffer == 19)
		ibuffer = 0;
	MelderString_8to32 (& buffers [ibuffer], string8);
	return buffers [ibuffer].string;
}

/********** TRACE **********/

void Melder_tracingToFile (MelderFile file) {
	MelderFile_copy (file, & MelderTrace::_file);
	MelderFile_delete (& MelderTrace::_file);
}

FILE * MelderTrace::_open (conststring8 sourceCodeFileName, int lineNumber, conststring8 functionName) {
	FILE *f;
	#if defined (_WIN32) && ! defined (__CYGWIN__)
		f = _wfopen ((const wchar_t *) MelderTrace::_peek32to16 (MelderTrace::_file. path), L"a");
	#else
		char utf8path [kMelder_MAXPATH+1];
		Melder_32to8_fileSystem_inplace (MelderTrace::_file. path, utf8path);   // this Melder_xxx() function is OK to call
		f = fopen ((char *) utf8path, "a");
	#endif
	if (! f)
		f = Melder_stderr;   // if the file cannot be opened, we can still trace to stderr!
	if (sourceCodeFileName) {
		const char *slashLocation = strrchr (sourceCodeFileName, Melder_DIRECTORY_SEPARATOR);
		fprintf (f, "%s (%s:%d): ", functionName, slashLocation ? slashLocation + 1 : sourceCodeFileName, lineNumber);
	} else {
		fprintf (f, "%s: ", functionName);
	}
	return f;
}

void MelderTrace::_close (FILE *f) {
	fprintf (f, "\n");
	if (f != Melder_stderr)
		fclose (f);
}

#if defined (linux) && ! defined (NO_GUI)
static void theGtkLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = MelderTrace::_open (nullptr, 0, "GTK");
	fprintf (f, "%s", message);
	MelderTrace::_close (f);
}
static void theGlibLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = MelderTrace::_open (nullptr, 0, "GLib");
	fprintf (f, "%s", message);
	MelderTrace::_close (f);
}
static void theGlibGobjectLogHandler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer unused_data) {
	FILE *f = MelderTrace::_open (nullptr, 0, "GLib-GObject");
	fprintf (f, "%s", message);
	MelderTrace::_close (f);
}
#endif

void Melder_setTracing (bool tracing) {
	time_t today = time (nullptr);
	if (! tracing)
		trace (U"switch tracing off"
			U" in Praat version ", Melder_appVersionSTR(),
			U" at ", Melder_peek8to32_u (ctime (& today))
		);
	Melder_isTracingGlobally = tracing;
	#if defined (linux) && ! defined (NO_GUI)
		static guint handler_id1, handler_id2, handler_id3;
		if (tracing) {
			handler_id1 = g_log_set_handler ("Gtk",          (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGtkLogHandler,         nullptr);
			handler_id2 = g_log_set_handler ("GLib",         (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibLogHandler,        nullptr);
			handler_id3 = g_log_set_handler ("GLib-GObject", (GLogLevelFlags) (G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION), theGlibGobjectLogHandler, nullptr);
		} else {
			if (handler_id1) g_log_remove_handler ("Gtk",          handler_id1);
			if (handler_id2) g_log_remove_handler ("GLib",         handler_id2);
			if (handler_id3) g_log_remove_handler ("GLib-GObject", handler_id3);
			handler_id1 = handler_id2 = handler_id3 = 0;
		}
	#endif
	if (tracing)
		trace (U"switch tracing on"
			U" in Praat version ", Melder_appVersionSTR(),
			U" at ", Melder_peek8to32_u (ctime (& today))
		);
}

/* End of file melder_debug.cpp */
