/* melder_textencoding.cpp
 *
 * Copyright (C) 2007-2026 Paul Boersma
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
#include "../sys/Preferences.h"
#include "../kar/UnicodeData.h"

#ifdef _WIN32
	#include <windows.h>
#endif
#if defined (macintosh)
	#include "macport_on.h"
	#include <CoreFoundation/CoreFoundation.h>
	#include "macport_off.h"
#endif

static struct {
	kMelder_textInputEncoding inputEncoding;
	kMelder_textOutputEncoding outputEncoding;
} preferences;

void Melder_setInputEncoding (kMelder_textInputEncoding encoding) { preferences. inputEncoding = encoding; }
kMelder_textInputEncoding Melder_getInputEncoding () { return preferences. inputEncoding; }

void Melder_setOutputEncoding (kMelder_textOutputEncoding encoding) { preferences. outputEncoding = encoding; }
kMelder_textOutputEncoding Melder_getOutputEncoding () { return preferences. outputEncoding; }

void Melder_textEncoding_prefs () {
	Preferences_addEnum (U"TextEncoding.inputEncoding", & preferences. inputEncoding, kMelder_textInputEncoding, kMelder_textInputEncoding::DEFAULT);
	Preferences_addEnum (U"TextEncoding.outputEncoding", & preferences. outputEncoding, kMelder_textOutputEncoding, kMelder_textOutputEncoding::DEFAULT);
}

bool Melder_isValidAscii (conststring32 text) {
	for (; *text != U'\0'; text ++) {
		if (*text > 127)
			return false;
	}
	return true;
}

bool Melder_str8IsValidAscii (conststring8 text) {
	for (; *text != U'\0'; text ++) {
		if (* (unsigned char *) text > 127)
			return false;
	}
	return true;
}

bool Melder_isEncodable (conststring32 text, int outputEncoding) {
	switch (outputEncoding) {
		case kMelder_textOutputEncoding_ASCII: {
			for (; *text != U'\0'; text ++) {
				if (*text > 127)
					return false;
			}
			return true;
		} break;
		case kMelder_textOutputEncoding_ISO_LATIN1: {
			for (; *text != U'\0'; text ++) {
				if (*text > 255)
					return false;
			}
			return true;
		} break;
		case (int) kMelder_textOutputEncoding::UTF8:
		case (int) kMelder_textOutputEncoding::UTF16:
		case (int) kMelder_textOutputEncoding::ASCII_THEN_UTF16:
		case (int) kMelder_textOutputEncoding::ISO_LATIN1_THEN_UTF16: {
			return true;
		}
	}
	return false;
}

bool Melder_str8IsValidUtf8 (const char *string) {
	for (const char8 *p = (const char8 *) & string [0]; *p != '\0'; p ++) {
		char32 kar = (char32) *p;
		if (kar <= 0x7F) {
			;
		} else if (kar <= 0xC1) {
			return false;
		} else if (kar <= 0xDF) {
			if ((* ++ p & 0xC0) != 0x80)
				return false;
		} else if (kar <= 0xEF) {
			if ((* ++ p & 0xC0) != 0x80)
				return false;
			if ((* ++ p & 0xC0) != 0x80)
				return false;
		} else if (kar <= 0xF4) {
			if ((* ++ p & 0xC0) != 0x80)
				return false;
			if ((* ++ p & 0xC0) != 0x80)
				return false;
			if ((* ++ p & 0xC0) != 0x80)
				return false;
		} else {
			return false;
		}
	}
	return true;
}

integer Melder_killReturns_inplace (char *text) {
	const char *from;
	char *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   // carriage return?
			if (from [1] == '\n') {   // followed by linefeed? Must be a Windows text
				from ++;   // ignore carriage return
				*to = '\n';   // copy linefeed
			} else {   // bare carriage return? Must be a Macintosh text
				*to = '\n';   // change to linefeed
			}
		} else {
			*to = *from;
		}
	}
	*to = '\0';   // closing null byte
	return to - text;
}

template <class CHAR>
integer Melder_killReturns_inplaceCHAR (CHAR *text) {
	const CHAR *from;
	CHAR *to;
	for (from = text, to = text; *from != '\0'; from ++, to ++) {
		if (*from == 13) {   // carriage return?
			if (from [1] == '\n' || from [1] == 0x0085 /* NextLine */) {   // followed by linefeed? Must be a Windows text
				from ++;   // ignore carriage return
				*to = '\n';   // copy linefeed
			} else {   // bare carriage return? Must be a Macintosh text
				*to = '\n';   // change to linefeed (10)
			}
		} else if (*from == 0x0085 /* NextLine */ || *from == 0x000C /* FormFeed */ ||
		    *from == UNICODE_LINE_SEPARATOR || *from == UNICODE_PARAGRAPH_SEPARATOR)
		{
			*to = '\n';
		} else {
			*to = *from;
		}
	}
	*to = '\0';   // closing null character
	return to - text;
}
integer Melder_killReturns_inplace (char32 *text) {
	return Melder_killReturns_inplaceCHAR <char32> (text);
}

int64 Melder_length_utf8 (conststring32 string, bool nativizeNewlines) {
	int64 length = 0;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00'007F) {
			#ifdef _WIN32
				if (nativizeNewlines && kar == U'\n')
					length ++;
			#else
				(void) nativizeNewlines;
			#endif
			length ++;
		} else if (kar <= 0x00'07FF) {
			length += 2;
		} else if (kar <= 0x00'FFFF) {
			length += 3;
		} else {
			Melder_assert (kar <= 0x10'FFFF);
			length += 4;
		}
	}
	return length;
}

int64 Melder_length_utf16 (conststring32 string, bool nativizeNewlines) {
	int64 length = 0;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		char32 kar = *p;
		if (kar <= 0x00'007F) {
			#ifdef _WIN32
				if (nativizeNewlines && kar == U'\n')
					length ++;
			#else
				(void) nativizeNewlines;
			#endif
			length ++;
		} else if (kar >= 0x01'0000) {
			length += 2;
		} else {
			length += 1;
		}
	}
	return length;
}

void MelderString_8to32 (MelderString *me, conststring8 textA) {
	if (! textA)
		textA = "";
	MelderString_empty (me);
	uinteger n = strlen (textA);
	for (uinteger i = 0; i <= n; i ++) {
		char8 kar1 = (char8) textA [i];   // convert sign
		if (kar1 <= 0x7F) {
			MelderString_appendCharacter (me,
				(char32) kar1
			);
		} else if (kar1 <= 0xC1) {
			MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
		} else if (kar1 <= 0xDF) {
			char8 kar2 = textA [++ i];
			if ((kar2 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (me,
				(char32) ((char32) ((char32) kar1 & 0x00'001F) << 6) |
						  (char32) ((char32) kar2 & 0x00'003F)
			);
		} else if (kar1 <= 0xEF) {
			char8 kar2 = textA [++ i];
			if ((kar2 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			char8 kar3 = textA [++ i];
			if ((kar3 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			MelderString_appendCharacter (me,
				(char32) ((char32) ((char32) kar1 & 0x00'000F) << 12) |
				(char32) ((char32) ((char32) kar2 & 0x00'003F) << 6) |
						  (char32) ((char32) kar3 & 0x00'003F)
			);
		} else if (kar1 <= 0xF4) {
			char8 kar2 = (char8) textA [++ i];
			if ((kar2 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			char8 kar3 = (char8) textA [++ i];
			if ((kar3 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			char8 kar4 = (char8) textA [++ i];
			if ((kar4 & 0xC0) != 0x80)
				MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
			char32 character =
				(char32) ((char32) ((char32) kar1 & 0x00'0007) << 18) |
				(char32) ((char32) ((char32) kar2 & 0x00'003F) << 12) |
				(char32) ((char32) ((char32) kar3 & 0x00'003F) << 6) |
						  (char32) ((char32) kar4 & 0x00'003F);
			MelderString_appendCharacter (me, character);
		} else {
			MelderString_appendCharacter (me, UNICODE_REPLACEMENT_CHARACTER);
		}
	}
}

conststring32 Melder_peek8to32_u (conststring8 textA) {
	static MelderString buffers [19];
	static int ibuffer = 0;
	if (++ ibuffer == 19)
		ibuffer = 0;
	MelderString_8to32 (& buffers [ibuffer], textA);
	return buffers [ibuffer].string;
}

char32 Melder_decodeMacRoman [256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
	 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
	 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
	 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 124, 125, 126, 127,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE,
	UNICODE_LATIN_CAPITAL_LETTER_C_WITH_CEDILLA, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_N_WITH_TILDE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_A_WITH_ACUTE,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_GRAVE, UNICODE_LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_A_WITH_TILDE,
	UNICODE_LATIN_SMALL_LETTER_A_WITH_RING_ABOVE, UNICODE_LATIN_SMALL_LETTER_C_WITH_CEDILLA,
	UNICODE_LATIN_SMALL_LETTER_E_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_E_WITH_GRAVE,

	UNICODE_LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_E_WITH_DIAERESIS,
	UNICODE_LATIN_SMALL_LETTER_I_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_I_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_I_WITH_DIAERESIS,
	UNICODE_LATIN_SMALL_LETTER_N_WITH_TILDE, UNICODE_LATIN_SMALL_LETTER_O_WITH_ACUTE,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_GRAVE, UNICODE_LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_DIAERESIS, UNICODE_LATIN_SMALL_LETTER_O_WITH_TILDE,
	UNICODE_LATIN_SMALL_LETTER_U_WITH_ACUTE, UNICODE_LATIN_SMALL_LETTER_U_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX, UNICODE_LATIN_SMALL_LETTER_U_WITH_DIAERESIS,

	UNICODE_DAGGER, UNICODE_DEGREE_SIGN, UNICODE_CENT_SIGN, UNICODE_POUND_SIGN,
	UNICODE_SECTION_SIGN, UNICODE_BULLET, UNICODE_PILCROW_SIGN, UNICODE_LATIN_SMALL_LETTER_SHARP_S,
	UNICODE_REGISTERED_SIGN, UNICODE_COPYRIGHT_SIGN, UNICODE_TRADE_MARK_SIGN, UNICODE_ACUTE_ACCENT,
	UNICODE_DIAERESIS, UNICODE_NOT_EQUAL_TO,
	UNICODE_LATIN_CAPITAL_LETTER_AE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_STROKE,

	UNICODE_INFINITY, UNICODE_PLUS_MINUS_SIGN, UNICODE_LESS_THAN_OR_EQUAL_TO, UNICODE_GREATER_THAN_OR_EQUAL_TO,
	UNICODE_YEN_SIGN, UNICODE_MICRO_SIGN, UNICODE_PARTIAL_DIFFERENTIAL, UNICODE_N_ARY_SUMMATION,
	UNICODE_N_ARY_PRODUCT, UNICODE_GREEK_SMALL_LETTER_PI, UNICODE_INTEGRAL,
	UNICODE_FEMININE_ORDINAL_INDICATOR, UNICODE_MASCULINE_ORDINAL_INDICATOR,
	UNICODE_GREEK_CAPITAL_LETTER_OMEGA, UNICODE_LATIN_SMALL_LETTER_AE,
	UNICODE_LATIN_SMALL_LETTER_O_WITH_STROKE,

	UNICODE_INVERTED_QUESTION_MARK, UNICODE_INVERTED_EXCLAMATION_MARK, UNICODE_NOT_SIGN, UNICODE_SQUARE_ROOT,
	UNICODE_LATIN_SMALL_LETTER_F_WITH_HOOK, UNICODE_ALMOST_EQUAL_TO, UNICODE_INCREMENT,
	UNICODE_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK, UNICODE_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK,
	UNICODE_HORIZONTAL_ELLIPSIS, UNICODE_NO_BREAK_SPACE, UNICODE_LATIN_CAPITAL_LETTER_A_WITH_GRAVE,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_TILDE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_TILDE,
	UNICODE_LATIN_CAPITAL_LIGATURE_OE, UNICODE_LATIN_SMALL_LIGATURE_OE,

	UNICODE_EN_DASH, UNICODE_EM_DASH, UNICODE_LEFT_DOUBLE_QUOTATION_MARK, UNICODE_RIGHT_DOUBLE_QUOTATION_MARK,
	UNICODE_LEFT_SINGLE_QUOTATION_MARK, UNICODE_RIGHT_SINGLE_QUOTATION_MARK, UNICODE_DIVISION_SIGN, UNICODE_LOZENGE,
	UNICODE_LATIN_SMALL_LETTER_Y_WITH_DIAERESIS, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS,
	UNICODE_FRACTION_SLASH, UNICODE_EURO_SIGN,
	UNICODE_SINGLE_LEFT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_SINGLE_RIGHT_POINTING_ANGLE_QUOTATION_MARK,
	UNICODE_LATIN_SMALL_LIGATURE_FI, UNICODE_LATIN_SMALL_LIGATURE_FL,

	UNICODE_DOUBLE_DAGGER, UNICODE_MIDDLE_DOT,
	UNICODE_SINGLE_LOW_9_QUOTATION_MARK, UNICODE_DOUBLE_LOW_9_QUOTATION_MARK, UNICODE_PER_MILLE_SIGN,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX,
	UNICODE_LATIN_CAPITAL_LETTER_A_WITH_ACUTE, UNICODE_LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_E_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS,
	UNICODE_LATIN_CAPITAL_LETTER_I_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX,

	0xf8ff /* Apple logo */, UNICODE_LATIN_CAPITAL_LETTER_O_WITH_GRAVE, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_ACUTE,
	UNICODE_LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX, UNICODE_LATIN_CAPITAL_LETTER_U_WITH_GRAVE,
	UNICODE_LATIN_SMALL_LETTER_DOTLESS_I, UNICODE_MODIFIER_LETTER_CIRCUMFLEX_ACCENT, UNICODE_SMALL_TILDE,
	UNICODE_MACRON, UNICODE_BREVE, UNICODE_DOT_ABOVE, UNICODE_RING_ABOVE, UNICODE_CEDILLA,
	UNICODE_DOUBLE_ACUTE_ACCENT, UNICODE_OGONEK, UNICODE_CARON };

char32 Melder_decodeWindowsLatin1 [256] = {
	  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
	 20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
	 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
	 60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99,
	100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	120, 121, 122, 123, 124, 125, 126, 127,
	UNICODE_EURO_SIGN, 129, UNICODE_SINGLE_LOW_9_QUOTATION_MARK, UNICODE_LATIN_SMALL_LETTER_F_WITH_HOOK,
	UNICODE_DOUBLE_LOW_9_QUOTATION_MARK, UNICODE_HORIZONTAL_ELLIPSIS, UNICODE_DAGGER, UNICODE_DOUBLE_DAGGER,
	UNICODE_MODIFIER_LETTER_CIRCUMFLEX_ACCENT, UNICODE_PER_MILLE_SIGN, UNICODE_LATIN_CAPITAL_LETTER_S_WITH_CARON,
	UNICODE_SINGLE_LEFT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_LATIN_CAPITAL_LIGATURE_OE, 141,
	UNICODE_LATIN_CAPITAL_LETTER_Z_WITH_CARON, 143,
	144, UNICODE_LEFT_SINGLE_QUOTATION_MARK, UNICODE_RIGHT_SINGLE_QUOTATION_MARK, UNICODE_LEFT_DOUBLE_QUOTATION_MARK,
	UNICODE_RIGHT_DOUBLE_QUOTATION_MARK, UNICODE_BULLET, UNICODE_EN_DASH, UNICODE_EM_DASH,
	UNICODE_SMALL_TILDE, UNICODE_TRADE_MARK_SIGN, UNICODE_LATIN_SMALL_LETTER_S_WITH_CARON,
	UNICODE_SINGLE_RIGHT_POINTING_ANGLE_QUOTATION_MARK, UNICODE_LATIN_SMALL_LIGATURE_OE, 157,
	UNICODE_LATIN_SMALL_LETTER_Z_WITH_CARON, UNICODE_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179,
	180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199,
	200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
	220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255 };

void Melder_8to32_inplace_e (conststring8 string8, mutablestring32 string32, kMelder_textInputEncoding inputEncoding) {
	const char8 *p = (const char8 *) & string8 [0];
	char32 *q = & string32 [0];
	if (inputEncoding == kMelder_textInputEncoding::UNDEFINED) {
		inputEncoding = preferences. inputEncoding;
		/*
			In case the preferences weren't initialized yet, use the platform defaults:
		*/
		if (inputEncoding == kMelder_textInputEncoding::UNDEFINED) {
			#if defined (macintosh)
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_MACROMAN;
			#elif defined (_WIN32)
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1;
			#else
				inputEncoding = kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1;
			#endif
		}
	}
	if (inputEncoding == kMelder_textInputEncoding::UTF8 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1 ||
		inputEncoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN)
	{
		if (Melder_str8IsValidUtf8 (string8)) {
			inputEncoding = kMelder_textInputEncoding::UTF8;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1) {
			inputEncoding = kMelder_textInputEncoding::ISO_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1) {
			inputEncoding = kMelder_textInputEncoding::WINDOWS_LATIN1;
		} else if (inputEncoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN) {
			inputEncoding = kMelder_textInputEncoding::MACROMAN;
		} else {
			Melder_assert (inputEncoding == kMelder_textInputEncoding::UTF8);
			Melder_throw (U"Text is not valid UTF-8; please try a different text input encoding.");
		}
	}
	if (inputEncoding == kMelder_textInputEncoding::UTF8) {
		while (*p != '\0') {
			char32 kar1 = *p ++;   // convert up without sign extension
			if (kar1 <= 0x00'007F) {
				*q ++ = kar1;
			} else if (kar1 <= 0x00'00DF) {
				char32 kar2 = *p ++;   // convert up without sign extension
				*q ++ = ((kar1 & 0x00'001F) << 6) | (kar2 & 0x00'003F);
			} else if (kar1 <= 0x00'00EF) {
				char32 kar2 = *p ++, kar3 = *p ++;   // convert up without sign extension
				*q ++ = ((kar1 & 0x00'000F) << 12) | ((kar2 & 0x00'003F) << 6) | (kar3 & 0x00'003F);
			} else if (kar1 <= 0x00'00F4) {
				char32 kar2 = *p ++, kar3 = *p ++, kar4 = *p ++;   // convert up without sign extension
				char32 kar = ((kar1 & 0x00'0007) << 18) | ((kar2 & 0x00'003F) << 12) | ((kar3 & 0x00'003F) << 6) | (kar4 & 0x00'003F);
				*q ++ = kar;
			}
		}
	} else if (inputEncoding == kMelder_textInputEncoding::ISO_LATIN1) {
		while (*p != '\0')
			*q ++ = *p ++;
	} else if (inputEncoding == kMelder_textInputEncoding::WINDOWS_LATIN1) {
		while (*p != '\0')
			*q ++ = Melder_decodeWindowsLatin1 [*p ++];
	} else if (inputEncoding == kMelder_textInputEncoding::MACROMAN) {
		while (*p != '\0')
			*q ++ = Melder_decodeMacRoman [*p ++];
	} else if (inputEncoding != kMelder_textInputEncoding::UTF8) {
		Melder_crash (U"Unknown text input encoding ", (int) inputEncoding, U".");
	}
	*q = U'\0';   // closing null character
	(void) Melder_killReturns_inplaceCHAR <char32> (string32);
}

autostring32 Melder_8to32_e (const char *string, kMelder_textInputEncoding inputEncoding) {
	if (! string)
		return autostring32();
	autostring32 result (Melder8_length (string));
	Melder_8to32_inplace_e (string, result.get(), inputEncoding);
	return result;
}

autostring32 Melder_8to32_e (const char *string) {
	if (! string)
		return autostring32();
	autostring32 result (Melder8_length (string));
	Melder_8to32_inplace_e (string, result.get(), kMelder_textInputEncoding::UTF8);
	return result;
}

conststring32 Melder_peek16to32 (conststring16 text) {
	if (! text)
		return nullptr;
	static MelderString buffers [19];
	static int bufferNumber = 0;
	if (++ bufferNumber == 19)
		bufferNumber = 0;
	MelderString_empty (& buffers [bufferNumber]);
	for (;;) {
		char16 kar1 = *text ++;
		if (kar1 == u'\0')
			return buffers [bufferNumber].string;
		if (kar1 < 0xD800) {
			MelderString_appendCharacter (& buffers [bufferNumber], (char32) kar1);   // convert up without sign extension
		} else if (kar1 < 0xDC00) {
			char16 kar2 = *text ++;
			if (kar2 >= 0xDC00 && kar2 <= 0xDFFF) {
				MelderString_appendCharacter (& buffers [bufferNumber],
					(char32) (0x01'0000 +
						(char32) (((char32) kar1 & 0x00'03FF) << 10) +
						(char32)  ((char32) kar2 & 0x00'03FF)));
			} else {
				MelderString_appendCharacter (& buffers [bufferNumber], UNICODE_REPLACEMENT_CHARACTER);
			}
		} else if (kar1 < 0xE000) {
			MelderString_appendCharacter (& buffers [bufferNumber], UNICODE_REPLACEMENT_CHARACTER);
		} else {
			MelderString_appendCharacter (& buffers [bufferNumber], (char32) kar1);   // convert up without sign extension
		}
	}
}

autostring32 Melder_16to32 (conststring16 text) {
	return Melder_dup (Melder_peek16to32 (text));
}

void Melder_32to8_inplace (conststring32 string, mutablestring8 utf8) {;
	Melder_assert (utf8);   // and unassertable: utf8 should be a long enough buffer
	char8 *to = (char8 *) & utf8 [0];
	if (string) {
		const char32 *p = & string [0];
		while (*p != U'\0') {
			const char32 kar = *p ++;
			if (kar <= 0x00'007F) {   // 7 bits
				#ifdef _WIN32
					if (kar == U'\n')
						*to ++ = 13;
				#endif
				*to ++ = (char8) kar;   // guarded truncation
			} else if (kar <= 0x00'07FF) {   // 11 bits
				*to ++ = (char8) (0x00'00C0 | (kar >> 6));   // the upper 5 bits yield a number between 0xC4 and 0xDF
				*to ++ = (char8) (0x00'0080 | (kar & 0x00'003F));   // the lower 6 bits yield a number between 0x80 and 0xBF
			} else if (kar <= 0x00'FFFF) {   // 16 bits
				*to ++ = (char8) (0x00'00E0 | (kar >> 12));   // the upper 4 bits yield a number between 0xE0 and 0xEF
				*to ++ = (char8) (0x00'0080 | ((kar >> 6) & 0x00'003F));
				*to ++ = (char8) (0x00'0080 | (kar & 0x00'003F));
			} else {   // unguarded truncation to 21 bits
				*to ++ = (char8) (0x00'00F0 | (kar >> 18));   // the upper 3 bits yield a number between 0xF0 and 0xF4 (0x10FFFF >> 18 == 4)
				*to ++ = (char8) (0x00'0080 | ((kar >> 12) & 0x00'003F));   // the next 6 bits
				*to ++ = (char8) (0x00'0080 | ((kar >> 6) & 0x00'003F));   // the third 6 bits
				*to ++ = (char8) (0x00'0080 | (kar & 0x00'003F));   // the lower 6 bits
			}
		}
	}
	*to = '\0';
}

conststring8 Melder_peek32to8 (conststring32 text) {
	if (! text)
		return nullptr;
	static mutablestring8 buffers [19] { nullptr };
	static int64 bufferSizes [19] { 0 };
	static int bufferNumber = 0;
	if (++ bufferNumber == 19)
		bufferNumber = 0;
	constexpr int64 maximumNumberOfUTF8bytesPerUTF32point = 4;   // because we use only the lower 21 bits
	const int64 numberOfUTF32points = Melder_length (text);
	const int64 maximumNumberOfBytesNeeded = numberOfUTF32points * maximumNumberOfUTF8bytesPerUTF32point + 1;
	if ((bufferSizes [bufferNumber] - maximumNumberOfBytesNeeded) * (integer) sizeof (char) >= 10'000) {
		Melder_free (buffers [bufferNumber]);
		bufferSizes [bufferNumber] = 0;
	}
	if (maximumNumberOfBytesNeeded > bufferSizes [bufferNumber]) {
		const int64 newBufferSize = (int64) floor (maximumNumberOfBytesNeeded * 1.61803) + 100;
		buffers [bufferNumber] = (char *) Melder_realloc_f (buffers [bufferNumber], newBufferSize * (int64) sizeof (char));
		bufferSizes [bufferNumber] = newBufferSize;
	}
	Melder_32to8_inplace (text, buffers [bufferNumber]);
	return buffers [bufferNumber];
}

autostring8 Melder_32to8 (conststring32 string) {
	if (! string)
		return autostring8();
	autostring8 result (Melder_length_utf8 (string, true));
	Melder_32to8_inplace (string, result.get());
	return result;
}

conststring16 Melder_peek32to16 (conststring32 text, bool nativizeNewlines) {
	if (! text)
		return nullptr;
	static MelderString16 buffers [19] { };
	static int bufferNumber = 0;
	if (++ bufferNumber == 19)
		bufferNumber = 0;
	MelderString16_empty (& buffers [bufferNumber]);
	const int64 n = Melder_length (text);
	if (nativizeNewlines) {
		for (int64 i = 0; i <= n; i ++) {
			#ifdef _WIN32
				if (text [i] == U'\n')
					MelderString16_appendCharacter (& buffers [bufferNumber], (char32) 13);
			#endif
			MelderString16_appendCharacter (& buffers [bufferNumber], text [i]);
		}
	} else {
		for (int64 i = 0; i <= n; i ++)
			MelderString16_appendCharacter (& buffers [bufferNumber], text [i]);
	}
	return buffers [bufferNumber].string;
}
conststring16 Melder_peek32to16 (conststring32 text) {
	return Melder_peek32to16 (text, false);
}

autostring16 Melder_32to16 (conststring32 text) {
	conststring16 text16 = Melder_peek32to16 (text);
	const integer length = Melder16_length (text16);
	autostring16 result (length);
	str16cpy (result.get(), text16);
	return result;
}

#if defined (_WIN32)
autostringW Melder_32toW (conststring32 text) {
	conststringW textW = Melder_peek32toW (text);
	const integer length = Melder16_length ((conststring16) textW);
	autostringW result (length);
	str16cpy ((mutablestring16) result.get(), (conststring16) textW);
	return result;
}
conststringW Melder_peek32toW_fileSystem (conststring32 string) {
	static wchar_t buffer [1 + kMelder_MAXPATH];
	NormalizeString (NormalizationC, Melder_peek32toW (string), -1, buffer, 1 + kMelder_MAXPATH);
	//FoldStringW (MAP_PRECOMPOSED, Melder_peek32toW (string), -1, buffer, 1 + kMelder_MAXPATH);   // this works even on Windows XP
	return buffer;
}
autostringW Melder_32toW_fileSystem (conststring32 text) {
	conststringW textW = Melder_peek32toW_fileSystem (text);
	const integer length = Melder16_length ((conststring16) textW);
	autostringW result (length);
	str16cpy ((mutablestring16) result.get(), (conststring16) textW);
	return result;
}
#endif

void Melder_32to8_fileSystem_inplace (conststring32 string, char *utf8) {
	#if defined (macintosh)
		/*
			On the Mac, the POSIX path name is stored in canonically decomposed UTF-8 encoding.
			The path is probably in precomposed UTF-32.
			So we first convert to UTF-16, then turn into CFString, then decompose, then convert to UTF-8.
			TODO: why not CFStringGetFileSystemRepresentation? perhaps because it's available from 10.4, whereas CFStringNormalize from 10.2
		*/
		UniChar unipath [kMelder_MAXPATH+1];
		const int64 n = Melder_length (string);
		int n_utf16 = 0;
		for (int64 i = 0; i < n; i ++) {
			char32 kar = (char32) string [i];   // change sign (bit 32 is never used)
			if (kar <= 0x00'FFFF) {
				unipath [n_utf16 ++] = (UniChar) kar;   // including null byte; guarded truncation
			} else if (kar <= 0x10'FFFF) {
				kar -= 0x01'0000;
				unipath [n_utf16 ++] = (UniChar) (0x00'D800 | (kar >> 10));   // correct truncation, because UTF-32 has fewer than 27 bits (in fact it has 21 bits)
				unipath [n_utf16 ++] = (UniChar) (0x00'DC00 | (kar & 0x00'03FF));
			} else {
				unipath [n_utf16 ++] = UNICODE_REPLACEMENT_CHARACTER;
			}
		}
		unipath [n_utf16] = u'\0';
		CFStringRef cfpath = CFStringCreateWithCharacters (nullptr, unipath, n_utf16);
		CFMutableStringRef cfpath2 = CFStringCreateMutableCopy (nullptr, 0, cfpath);
		CFRelease (cfpath);
		CFStringNormalize (cfpath2, kCFStringNormalizationFormD);   // Mac requires decomposed characters
		CFStringGetCString (cfpath2, (char *) utf8, kMelder_MAXPATH+1, kCFStringEncodingUTF8);   // Mac POSIX requires UTF-8
		CFRelease (cfpath2);
	#else
		Melder_32to8_inplace (string, utf8);
				// fallback; should work even on Windows if converted to W later (as in op_open_file 2021-12-16 and flac_fopen 2024-08-11)
	#endif
}
conststring8 Melder_peek32to8_fileSystem (conststring32 string) {
	static char buffer [1 + kMelder_MAXPATH];
	Melder_32to8_fileSystem_inplace (string, buffer);
	return buffer;
}

#if defined (macintosh)
const void * Melder_peek32toCfstring (conststring32 text) {
	if (! text)
		return nullptr;
	static CFStringRef cfString [11];
	static int icfString = 0;
	if (++ icfString == 11)
		icfString = 0;
	if (cfString [icfString])
		CFRelease (cfString [icfString]);
	cfString [icfString] = CFStringCreateWithCString (nullptr, (const char *) Melder_peek32to8 (text), kCFStringEncodingUTF8);
	return cfString [icfString];
}
const void * Melder_peek32toCfstring_fileSystem (conststring32 text) {
	if (! text)
		return nullptr;
	static CFStringRef cfString [11];
	static int icfString = 0;
	if (++ icfString == 11)
		icfString = 0;
	if (cfString [icfString])
		CFRelease (cfString [icfString]);
	cfString [icfString] = CFStringCreateWithCString (nullptr, (const char *) Melder_peek32to8_fileSystem (text), kCFStringEncodingUTF8);
	return cfString [icfString];
}
#endif

/* End of file melder_textencoding.cpp */
