/* MelderReadText.cpp
 *
 * Copyright (C) 2008,2010-2012,2014-2020,2022,2023,2025,2026 Paul Boersma
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
#include "../kar/UnicodeData.h"

char32 MelderReadText_getChar (MelderReadText me) {
	if (my string32) {
		if (* my readPointer32 == U'\0') {
			my previousPointerStep = 0;   // because we're not moving forward
			return U'\0';
		}
		my previousPointerStep = 1;
		return * my readPointer32 ++;
	} else {
		if (* my readPointer8 == '\0') {
			my previousPointerStep = 0;   // because we're not moving forward
			return U'\0';
		}
		if (my input8Encoding == kMelder_textInputEncoding::UTF8) {
			char32 kar1 = (char32) (char8) * my readPointer8 ++;
			if (kar1 <= 0x00'007F) {
				my previousPointerStep = 1;
				return kar1;
			} else if (kar1 <= 0x00'00DF) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				my previousPointerStep = 2;
				return ((kar1 & 0x00'001F) << 6) | (kar2 & 0x00'003F);
			} else if (kar1 <= 0x00'00EF) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				char32 kar3 = (char32) (char8) * my readPointer8 ++;
				my previousPointerStep = 3;
				return ((kar1 & 0x00'000F) << 12) | ((kar2 & 0x00'003F) << 6) | (kar3 & 0x00'003F);
			} else if (kar1 <= 0x00'00F4) {
				char32 kar2 = (char32) (char8) * my readPointer8 ++;
				char32 kar3 = (char32) (char8) * my readPointer8 ++;
				char32 kar4 = (char32) (char8) * my readPointer8 ++;
				my previousPointerStep = 4;
				return ((kar1 & 0x00'0007) << 18) | ((kar2 & 0x00'003F) << 12) | ((kar3 & 0x00'003F) << 6) | (kar4 & 0x00'003F);
			} else {
				my previousPointerStep = 1;
				return UNICODE_REPLACEMENT_CHARACTER;
			}
		} else if (my input8Encoding == kMelder_textInputEncoding::MACROMAN) {
			my previousPointerStep = 1;
			return Melder_decodeMacRoman [(char8) * my readPointer8 ++];
		} else if (my input8Encoding == kMelder_textInputEncoding::WINDOWS_LATIN1) {
			my previousPointerStep = 1;
			return Melder_decodeWindowsLatin1 [(char8) * my readPointer8 ++];
		} else {
			/* Unknown encoding. */
			my previousPointerStep = 1;
			return (char32) (char8) * my readPointer8 ++;
		}
	}
}

void MelderReadText_ungetChar (MelderReadText me) {
	if (my string32)
		my readPointer32 -= my previousPointerStep;
	else
		my readPointer8 -= my previousPointerStep;
	my previousPointerStep = 0;
}

mutablestring32 MelderReadText_readLine (MelderReadText me) {
	//TRACE
	my previousPointerStep = 0;
	if (my string32) {
		trace (1);
		Melder_assert (my readPointer32);
		Melder_assert (! my readPointer8);
		if (*my readPointer32 == U'\0')   // tried to read past end of file
			return nullptr;
		char32 *result32 = my readPointer32;
		char32 *newline32 = str32chr (result32, U'\n');
		if (newline32) {
			*newline32 = U'\0';
			my readPointer32 = newline32 + 1;
		} else {
			my readPointer32 += Melder_length (result32);
		}

		integer lineLength = Melder_length (result32);
		integer lineBufferSizeWanted = lineLength + 101;
		if (lineBufferSizeWanted > my lineBufferSize) {
			my lineBuffer = autostring32 (lineBufferSizeWanted);
			my lineBufferSize = lineBufferSizeWanted;
		}
		str32cpy (my lineBuffer.get(), result32);
		if (newline32)
			*newline32 = U'\n';   // restore original newline symbol, so that getNumberOfLines() and getLineNumber() continue to work
	} else {
		trace (2);
		Melder_assert (my string8);
		Melder_assert (! my readPointer32);
		Melder_assert (my readPointer8);
		if (*my readPointer8 == '\0')   // tried to read past end of file
			return nullptr;
		char *result8 = my readPointer8;
		char *newline8 = strchr (result8, '\n');
		if (newline8) {
			*newline8 = '\0';
			my readPointer8 = newline8 + 1;
		} else {
			my readPointer8 += strlen (result8);
		}

		uint64 lineLength_uint64 = (uint64) strlen (result8);
		if (lineLength_uint64 > (uint64) (MAXIMUM_ALLOCATION_SIZE - 101)) {
			if (newline8)
				*newline8 = '\n';
			Melder_throw (U"Line too long: more than ", Melder_bigInteger (MAXIMUM_ALLOCATION_SIZE - 101), U" characters.");
		}
		uint64 lineBufferSizeWanted_uint64 = lineLength_uint64 + 101;   // <= (uint64) INTEGER_MAX
		integer lineBufferSizeWanted = integer (lineBufferSizeWanted_uint64);   // guarded conversion
		if (lineBufferSizeWanted > my lineBufferSize) {
			my lineBuffer = autostring32 (lineBufferSizeWanted);
			my lineBufferSize = lineBufferSizeWanted;
		}
		Melder_8to32_inplace_e (result8, my lineBuffer.get(), my input8Encoding);
		if (newline8)
			*newline8 = '\n';   // restore original newline symbol, so that getNumberOfLines() and getLineNumber() continue to work
	}
	return my lineBuffer.get();
}

int64 MelderReadText_getNumberOfLines (MelderReadText me) {
	int64 n = 0;
	if (my string32) {
		char32 *p = & my string32 [0];
		for (; *p != U'\0'; p ++) {
			if (*p == U'\n')
				n ++;
		}
		if (p - & my string32 [0] > 1 && p [-1] != U'\n')
			n ++;
	} else {
		char *p = & my string8 [0];
		for (; *p != '\0'; p ++) {
			if (*p == '\n')
				n ++;
		}
		if (p - & my string8 [0] > 1 && p [-1] != '\n')
			n ++;
	}
	return n;
}

conststring32 MelderReadText_getLineNumber (MelderReadText me) {
	int64 result = 1;
	if (my string32) {
		char32 *p = & my string32 [0];
		while (my readPointer32 - p > 0) {
			if (*p == U'\0' || *p == U'\n')
				result ++;
			p ++;
		}
	} else {
		char *p = & my string8 [0];
		while (my readPointer8 - p > 0) {
			if (*p == '\0' || *p == '\n')
				result ++;
			p ++;
		}
	}
	return Melder_integer (result);
}

autoMelderReadText MelderReadText_createFromFile (MelderFile file) {
	autoMelderReadText me = std::make_unique <structMelderReadText> ();
	my string32 = MelderFile_readText (file, & my string8);
	if (my string32) {
		my readPointer32 = & my string32 [0];
	} else {
		Melder_assert (my string8);
		my readPointer8 = & my string8 [0];
		my input8Encoding = Melder_getInputEncoding ();
		if (my input8Encoding == kMelder_textInputEncoding::UTF8 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1 ||
			my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN)
		{
			if (Melder_str8IsValidUtf8 (my string8.get())) {
				my input8Encoding = kMelder_textInputEncoding::UTF8;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8) {
				Melder_throw (U"Text is not valid UTF-8; please try a different text input encoding.");
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_ISO_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding::ISO_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_WINDOWS_LATIN1) {
				my input8Encoding = kMelder_textInputEncoding::WINDOWS_LATIN1;
			} else if (my input8Encoding == kMelder_textInputEncoding::UTF8_THEN_MACROMAN) {
				my input8Encoding = kMelder_textInputEncoding::MACROMAN;
			}
		}
	}
	return me;
}

autoMelderReadText MelderReadText_createFromText (autostring32 text) {
	autoMelderReadText me = std::make_unique <structMelderReadText> ();
	my string32 = text.move();
	my readPointer32 = & my string32 [0];
	return me;
}

/* End of file MelderReadText.cpp */
