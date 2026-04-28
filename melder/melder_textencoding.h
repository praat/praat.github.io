#ifndef _melder_textencoding_h_
#define _melder_textencoding_h_
/* melder_textencoding.h
 *
 * Copyright (C) 1992-2020,2022,2024-2026 Paul Boersma
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

/**
 * Text encodings.
 */
void Melder_textEncoding_prefs ();
void Melder_setInputEncoding (kMelder_textInputEncoding encoding);
kMelder_textInputEncoding Melder_getInputEncoding ();
void Melder_setOutputEncoding (kMelder_textOutputEncoding encoding);
kMelder_textOutputEncoding Melder_getOutputEncoding ();

/*
 * Some other encodings. Although not used in the above set/get functions,
 * these constants should stay separate from the above encoding constants
 * because they occur in the same fields of struct MelderFile.
 */
constexpr uint32 kMelder_textOutputEncoding_ASCII = 0x4153'4349;
constexpr uint32 kMelder_textOutputEncoding_ISO_LATIN1 = 0x4C41'5401;
constexpr uint32 kMelder_textOutputEncoding_FLAC = 0x464C'4143;

bool Melder_isValidAscii (conststring32 string);
bool Melder_str8IsValidAscii (conststring8 string);
bool Melder_str8IsValidUtf8 (conststring8 string);
bool Melder_isEncodable (conststring32 string, int outputEncoding);
extern char32 Melder_decodeMacRoman [256];
extern char32 Melder_decodeWindowsLatin1 [256];

/**
	 Replace all bare returns (old Mac) or return-plus-linefeed sequences (Win) with bare linefeeds
	 (generic: Unix and modern Mac).
	 Return new length of string (equal to or less than old length).
*/
integer Melder_killReturns_inplace (mutablestring32 text);
integer Melder_killReturns_inplace (mutablestring8 text);

int64 Melder_length_utf8  (conststring32 string, bool nativizeNewlines);
int64 Melder_length_utf16 (conststring32 string, bool nativizeNewlines);

void MelderString_8to32 (MelderString *me, conststring8 textA);
extern "C" conststring32 Melder_peek8to32_u (conststring8 string);
void Melder_8to32_inplace_e (conststring8 source, mutablestring32 target, kMelder_textInputEncoding inputEncoding);
	// errors: Text is not valid UTF-8.
autostring32 Melder_8to32_e (conststring8 string, kMelder_textInputEncoding inputEncoding);
	// errors: Out of memory; Text is not valid UTF-8.
autostring32 Melder_8to32_e (conststring8 string);
	// errors: Out of memory; Text is not valid UTF-8.

conststring32 Melder_peek16to32 (conststring16 text);
autostring32 Melder_16to32 (conststring16 text);

extern "C" conststring8 Melder_peek32to8 (conststring32 string);
void Melder_32to8_inplace (conststring32 string, mutablestring8 utf8);
autostring8 Melder_32to8 (conststring32 string);
autostring16 Melder_32to16 (conststring32 string);
	// errors: Out of memory.

conststring16 Melder_peek32to16 (conststring32 text, bool nativizeNewlines);
extern "C" conststring16 Melder_peek32to16 (conststring32 string);

#ifdef _WIN32
	inline conststringW Melder_peek32toW (conststring32 string) { return (conststringW) Melder_peek32to16 (string); }
	conststringW Melder_peek32toW_fileSystem (conststring32 string);
	autostringW Melder_32toW (conststring32 string);
	autostringW Melder_32toW_fileSystem (conststring32 string);
	inline conststring32 Melder_peekWto32 (conststringW string) { return Melder_peek16to32 ((conststring16) string); }
	inline autostring32 Melder_Wto32 (conststringW string) { return Melder_16to32 ((conststring16) string); }
#endif

void Melder_32to8_fileSystem_inplace (conststring32 string, mutablestring8 utf8);
conststring8 Melder_peek32to8_fileSystem (conststring32 string);

void Melder_8bitFileRepresentationToStr32_inplace (conststring8 utf8, mutablestring32 string);
const void * Melder_peek32toCfstring (conststring32 string);
const void * Melder_peek32toCfstring_fileSystem (conststring32 text);

/* End of file melder_textencoding.h */
#endif
