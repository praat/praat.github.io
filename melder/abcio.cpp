/* abcio.cpp
 *
 * Copyright (C) 1992-2011,2015,2017-2020,2022,2024,2025 Paul Boersma
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
#ifdef macintosh
	#include <TargetConditionals.h>
#endif

/********** text I/O **********/

static int64 getInteger (MelderReadText me) {
	char buffer [41];
	char32 c;
	/*
	 * Look for the first numeric character.
	 */
	for (c = MelderReadText_getChar (me); c != U'-' && ! Melder_isAsciiDecimalNumber (c) && c != U'+'; c = MelderReadText_getChar (me)) {
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while looking for an integer (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != U'\n' && c != U'\r') {
				if (c == 0)
					Melder_throw (U"Early end of text detected in comment while looking for an integer (line ", MelderReadText_getLineNumber (me), U").");
			}
		}
		if (c == U'\"')
			Melder_throw (U"Found a string while looking for an integer in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'<')
			Melder_throw (U"Found an enumerated value while looking for an integer in text (line ", MelderReadText_getLineNumber (me), U").");
		while (! Melder_isHorizontalOrVerticalSpace (c)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected in comment (line ", MelderReadText_getLineNumber (me), U").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (; i < 40; i ++) {
		if (c > 127)
			Melder_throw (U"Found strange text while looking for an integer in text (line ", MelderReadText_getLineNumber (me), U").");
		buffer [i] = (char) (char8) c;   // guarded conversion down
		c = MelderReadText_getChar (me);
		if (c == U'\0') { break; }   // this may well be OK here
		if (Melder_isHorizontalOrVerticalSpace (c)) break;
	}
	if (i >= 40)
		Melder_throw (U"Found long text while looking for an integer in text (line ", MelderReadText_getLineNumber (me), U").");
	buffer [i + 1] = '\0';
	return strtoll (buffer, nullptr, 10);
}

static uint64 getUnsigned (MelderReadText me) {
	char buffer [41];
	char32 c;
	for (c = MelderReadText_getChar (me); ! Melder_isAsciiDecimalNumber (c) && c != U'+'; c = MelderReadText_getChar (me)) {
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while looking for an unsigned integer (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == U'\0')
					Melder_throw (U"Early end of text detected in comment while looking for an unsigned integer (line ", MelderReadText_getLineNumber (me), U").");
			}
		}
		if (c == U'\"')
			Melder_throw (U"Found a string while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'<')
			Melder_throw (U"Found an enumerated value while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'-')
			Melder_throw (U"Found a negative value while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), U").");
		while (! Melder_isHorizontalOrVerticalSpace (c)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected in comment (line ", MelderReadText_getLineNumber (me), U").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (i = 0; i < 40; i ++) {
		if (c > 127)
			Melder_throw (U"Found strange text while looking for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), U").");
		buffer [i] = (char) (char8) c;   // guarded conversion down
		c = MelderReadText_getChar (me);
		if (c == U'\0') { break; }   // this may well be OK here
		if (Melder_isHorizontalOrVerticalSpace (c)) break;
	}
	if (i >= 40)
		Melder_throw (U"Found long text while searching for an unsigned integer in text (line ", MelderReadText_getLineNumber (me), U").");
	buffer [i + 1] = '\0';
	return strtoull (buffer, nullptr, 10);
}

static double getReal (MelderReadText me) {
	int i;
	char buffer [41], *slash;
	char32 c;
	do {
		for (c = MelderReadText_getChar (me); c != U'-' && ! Melder_isAsciiDecimalNumber (c) && c != U'+'; c = MelderReadText_getChar (me)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected while looking for a real number (line ", MelderReadText_getLineNumber (me), U").");
			if (c == U'!') {   // end-of-line comment?
				while ((c = MelderReadText_getChar (me)) != U'\n' && c != U'\r') {
					if (c == U'\0')
						Melder_throw (U"Early end of text detected in comment while looking for a real number (line ", MelderReadText_getLineNumber (me), U").");
				}
			}
			if (c == U'\"')
				Melder_throw (U"Found a string while looking for a real number in text (line ", MelderReadText_getLineNumber (me), U").");
			if (c == U'<')
				Melder_throw (U"Found an enumerated value while looking for a real number in text (line ", MelderReadText_getLineNumber (me), U").");
			while (! Melder_isHorizontalOrVerticalSpace (c)) {
				if (c == U'\0')
					Melder_throw (U"Early end of text detected in comment while looking for a real number (line ", MelderReadText_getLineNumber (me), U").");
				c = MelderReadText_getChar (me);
			}
		}
		for (i = 0; i < 40; i ++) {
			if (c > 127)
				Melder_throw (U"Found strange text while looking for a real number in text (line ", MelderReadText_getLineNumber (me), U").");
			buffer [i] = (char) (char8) c;   // guarded conversion down
			c = MelderReadText_getChar (me);
			if (c == U'\0') { break; }   // this may well be OK here
			if (Melder_isHorizontalOrVerticalSpace (c)) break;
		}
		if (i >= 40)
			Melder_throw (U"Found long text while searching for a real number in text (line ", MelderReadText_getLineNumber (me), U").");
	} while (i == 0 && buffer [0] == '+');   // guard against single '+' symbols, which occur in complex numbers
	buffer [i + 1] = '\0';
	slash = strchr (buffer, '/');
	if (slash) {
		*slash = '\0';
		double numerator = Melder_a8tof (buffer), denominator = Melder_a8tof (slash + 1);
		if (isundef (numerator) || isundef (denominator) || denominator == 0.0)
			return undefined;
		return numerator / denominator;
	}
	return Melder_a8tof (buffer);
}

static dcomplex getComplex (MelderReadText me) {
	dcomplex result;
	char realBuffer [41], imaginaryBuffer [41];
	integer ireal = 0, iimag = 0;
	char32 c;
	bool inExponent = false, inExponentNumber = false, separatorIsMinus = false;
	for (c = MelderReadText_getChar (me); c != U'-' && ! Melder_isAsciiDecimalNumber (c) && c != U'+'; c = MelderReadText_getChar (me)) {
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while looking for a complex number (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != U'\n' && c != U'\r') {
				if (c == U'\0')
					Melder_throw (U"Early end of text detected in comment while looking for a complex number (line ", MelderReadText_getLineNumber (me), U").");
			}
		}
		if (c == U'\"')
			Melder_throw (U"Found a string while looking for a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'<')
			Melder_throw (U"Found an enumerated value while looking for a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		while (! Melder_isHorizontalOrVerticalSpace (c)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected in comment while looking for a complex number (line ", MelderReadText_getLineNumber (me), U").");
			c = MelderReadText_getChar (me);
		}
	}
	for (; ireal < 40; ireal ++) {
		if (c > 127)
			Melder_throw (U"Found strange text while looking for a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		if (inExponent) {
			if (c == U'+' || c == U'-')  {
				if (inExponentNumber) {
					/*
						This  must be the beginning of the imaginary part.
					*/
					separatorIsMinus = ( c == U'-' );
					realBuffer [ireal] = U'\0';
					break;
				} else {
					inExponentNumber = true;
				}
			} else if (Melder_isAsciiDecimalNumber (c)) {
				inExponentNumber = true;
			} else if (inExponentNumber) {   // typically a space
				realBuffer [ireal] = U'\0';
				break;
			} else {
				Melder_throw (U"Found unexpected symbol in the exponent of a complex number (line ", MelderReadText_getLineNumber (me), U").");
			}
		} else if (ireal > 0 && (c == U'+' || c == U'-')) {   // note: initial signs are not separators
			separatorIsMinus = ( c == U'-' );
			realBuffer [ireal] = U'\0';
			break;
		}
		if (c == 'e' || c == 'E')
			inExponent = true;
		realBuffer [ireal] = (char) (char8) c;   // guarded conversion down
		c = MelderReadText_getChar (me);
		if (c == U'\0')
			Melder_throw (U"Missing imaginary part in complex number (line ", MelderReadText_getLineNumber (me), U").");
		if (Melder_isHorizontalOrVerticalSpace (c))
			Melder_throw (U"Found a space within a complex number (line ", MelderReadText_getLineNumber (me), U").");
	}
	if (ireal >= 40)
		Melder_throw (U"Found long text while searching for a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
	realBuffer [ireal + 1] = '\0';
	result. real (Melder_a8tof (realBuffer));
	c = MelderReadText_getChar (me);
	if (c != U'-' && ! Melder_isAsciiDecimalNumber (c) && c != U'+')
		Melder_throw (U"Found strange text while looking for the imaginary part of a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
	if (c == U'\0')
		Melder_throw (U"Early end of text detected while looking for the imaginary part of a complex number (line ", MelderReadText_getLineNumber (me), U").");
	if (Melder_isHorizontalOrVerticalSpace (c))
		Melder_throw (U"Found a space within a complex number (line ", MelderReadText_getLineNumber (me), U").");
	if (c == U'!') {   // end-of-line comment?
		while ((c = MelderReadText_getChar (me)) != U'\n' && c != U'\r') {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected in comment while looking for the imaginary part of a complex number (line ", MelderReadText_getLineNumber (me), U").");
		}
	}
	if (c == U'\"')
		Melder_throw (U"Found a string while looking for the imaginary part of a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
	if (c == U'<')
		Melder_throw (U"Found an enumerated value while looking for the imaginary part of a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
	for (; iimag < 40; iimag ++) {
		if (c > 127)
			Melder_throw (U"Found strange text while looking for the imaginary part of a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		imaginaryBuffer [iimag] = (char) (char8) c;   // guarded conversion down
		c = MelderReadText_getChar (me);
		if (c == U'\0')
			Melder_throw (U"Missing i in a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		if (Melder_isHorizontalOrVerticalSpace (c))
			Melder_throw (U"Missing i in a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'i')
			break;
	}
	if (iimag >= 40)
		Melder_throw (U"Found long text while searching for the imaginary part of a complex number in text (line ", MelderReadText_getLineNumber (me), U").");
	imaginaryBuffer [iimag + 1] = '\0';
	result. imag (Melder_a8tof (imaginaryBuffer) * ( separatorIsMinus ? -1.0 : 1.0 ));
	return result;
}

static int getEnum (MelderReadText me, int (*getValue) (conststring32)) {
	char32 buffer [41], c;
	for (c = MelderReadText_getChar (me); c != U'<'; c = MelderReadText_getChar (me)) {
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'!') {   /* End-of-line comment? */
			while ((c = MelderReadText_getChar (me)) != U'\n' && c != U'\r') {
				if (c == U'\0')
					Melder_throw (U"Early end of text detected in comment while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), U").");
			}
		}
		if (c == U'-' || Melder_isAsciiDecimalNumber (c) || c == U'+')
			Melder_throw (U"Found a number while looking for an enumerated value in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'\"')
			Melder_throw (U"Found a string while looking for an enumerated value in text (line ", MelderReadText_getLineNumber (me), U").");
		while (! Melder_isHorizontalOrVerticalSpace (c)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected in comment while looking for an enumerated value (line ", MelderReadText_getLineNumber (me), U").");
			c = MelderReadText_getChar (me);
		}
	}
	int i = 0;
	for (; i < 40; i ++) {
		c = MelderReadText_getChar (me);   // read past first '<'
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while reading an enumerated value (line ", MelderReadText_getLineNumber (me), U").");
		constexpr char32 theOnlySpaceAllowedInAnEnum = U' ';
		if (Melder_isHorizontalOrVerticalSpace (c) && c != theOnlySpaceAllowedInAnEnum)
			Melder_throw (U"No matching '>' while reading an enumerated value (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'>')
			break;   // the expected closing bracket; not added to the buffer
		buffer [i] = c;
	}
	if (i >= 40)
		Melder_throw (U"Found strange text while reading an enumerated value in text (line ", MelderReadText_getLineNumber (me), U").");
	buffer [i] = U'\0';
	int value = getValue (buffer);
	if (value < 0)
		Melder_throw (U"\"", buffer, U"\" is not a value of the enumerated type.");
	return value;
}

static char32 * peekString (MelderReadText me) {
	static MelderString buffer;
	MelderString_empty (& buffer);
	for (char32 c = MelderReadText_getChar (me); c != U'\"'; c = MelderReadText_getChar (me)) {
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while looking for a string (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'!') {   // end-of-line comment?
			while ((c = MelderReadText_getChar (me)) != '\n' && c != '\r') {
				if (c == U'\0')
					Melder_throw (U"Early end of text detected in comment while looking for a string (line ", MelderReadText_getLineNumber (me), U").");
			}
		}
		if (c == U'-' || Melder_isAsciiDecimalNumber (c) || c == U'+')
			Melder_throw (U"Found a number while looking for a string in text (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'<')
			Melder_throw (U"Found an enumerated value while looking for a string in text (line ", MelderReadText_getLineNumber (me), U").");
		while (! Melder_isHorizontalOrVerticalSpace (c)) {
			if (c == U'\0')
				Melder_throw (U"Early end of text detected while looking for a string (line ", MelderReadText_getLineNumber (me), U").");
			c = MelderReadText_getChar (me);
		}
	}
	for (int i = 0; 1; i ++) {
		char32 c = MelderReadText_getChar (me);   // read past first '"'
		if (c == U'\0')
			Melder_throw (U"Early end of text detected while reading a string (line ", MelderReadText_getLineNumber (me), U").");
		if (c == U'\"') {
			char32 next = MelderReadText_getChar (me);
			if (next == U'\0') { break; }   // closing quote is last character in file: OK
			if (next != U'\"') {
				if (Melder_isHorizontalOrVerticalSpace (next)) {
					// closing quote is followed by whitespace: it is OK to skip this whitespace (no need to "ungetChar")
				} else {
					char32 kar2 [2] = { next, U'\0' };
					Melder_throw (U"Character ", kar2, U" following quote (line ", MelderReadText_getLineNumber (me), U"). End of string or undoubled quote?");
				}
				break;   // the expected closing double quote; not added to the buffer
			}   // else: add only one of the two quotes to the buffer
		}
		MelderString_appendCharacter (& buffer, c);
	}
	return buffer.string;
}

#include "enums_getText.h"
#include "abcio_enums.h"
#include "enums_getValue.h"
#include "abcio_enums.h"

int texgeti8 (MelderReadText text) {
	try {
		int64 externalValue = getInteger (text);
		if (externalValue < INT8_MIN || externalValue > INT8_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (-128 .. +127).");
		return (int) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Signed small integer not read from text file.");
	}
}

int16 texgeti16 (MelderReadText text) {
	try {
		int64 externalValue = getInteger (text);
		if (externalValue < INT16_MIN || externalValue > INT16_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (-32768 .. +32767).");
		return (int16) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Signed short integer not read from text file.");
	}
}

int32 texgeti32 (MelderReadText text) {
	try {
		int64 externalValue = getInteger (text);
		if (externalValue < INT32_MIN || externalValue > INT32_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (-2147483648 .. +2147483647).");
		return (int32) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from text file.");
	}
}

integer texgetinteger (MelderReadText text) {
	try {
		int64 externalValue = getInteger (text);
		if (externalValue < INT32_MIN || externalValue > INT32_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (-2147483648 .. +2147483647).");   // this will change
		return (integer) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from text file.");
	}
}

unsigned int texgetu8 (MelderReadText text) {
	try {
		uint64 externalValue = getUnsigned (text);
		if (externalValue > UINT8_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (0 .. 255).");
		return (unsigned int) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Unsigned small integer not read from text file.");
	}
}

uint16 texgetu16 (MelderReadText text) {
	try {
		uint64 externalValue = getUnsigned (text);
		if (externalValue > UINT16_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (0 .. 65535).");
		return (uint16) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Unsigned short integer not read from text file.");
	}
}

uint32 texgetu32 (MelderReadText text) {
	try {
		uint64 externalValue = getUnsigned (text);
		if (externalValue > UINT32_MAX)
			Melder_throw (U"Value (", externalValue, U") out of range (0 .. 4294967295).");
		return (uint32) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from text file.");
	}
}

double texgetr32 (MelderReadText text) { return getReal (text); }
double texgetr64 (MelderReadText text) { return getReal (text); }
double texgetr80 (MelderReadText text) { return getReal (text); }
dcomplex texgetc64  (MelderReadText text) { return getComplex (text); }
dcomplex texgetc128 (MelderReadText text) { return getComplex (text); }

int texgete8 (MelderReadText text, enum_generic_getValue getValue) { return getEnum (text, getValue); }
int texgete16 (MelderReadText text, enum_generic_getValue getValue) { return getEnum (text, getValue); }
bool texgeteb (MelderReadText text) { return getEnum (text, (enum_generic_getValue) kBoolean_getValue); }
bool texgeteq (MelderReadText text) { return getEnum (text, (enum_generic_getValue) kQuestion_getValue); }
bool texgetex (MelderReadText text) { return getEnum (text, (enum_generic_getValue) kExistence_getValue); }
autostring32 texgetw16 (MelderReadText text) { return Melder_dup (peekString (text)); }
autostring32 texgetw32 (MelderReadText text) { return Melder_dup (peekString (text)); }

void texindent (MelderFile file) { file -> indent += 4; }
void texexdent (MelderFile file) { file -> indent -= 4; }
void texresetindent (MelderFile file) { file -> indent = 0; }

#define texput_UP_TO_NINE_NULLABLE_STRINGS  \
	conststring32 s1, conststring32 s2, conststring32 s3, \
	conststring32 s4, conststring32 s5, conststring32 s6, \
	conststring32 s7, conststring32 s8, conststring32 s9

void texputintro (MelderFile file, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	if (file -> verbose) {
		MelderFile_write (file, U"\n");
		for (int iindent = 1; iindent <= file -> indent; iindent ++) {
			MelderFile_write (file, U" ");
		}
		MelderFile_write (file,
			s1 && s1 [0] == U'd' && s1 [1] == U'_' ? & s1 [2] : & s1 [0],
			s2 && s2 [0] == U'd' && s2 [1] == U'_' ? & s2 [2] : & s2 [0],
			s3 && s3 [0] == U'd' && s3 [1] == U'_' ? & s3 [2] : & s3 [0],
			s4 && s4 [0] == U'd' && s4 [1] == U'_' ? & s4 [2] : & s4 [0],
			s5 && s5 [0] == U'd' && s5 [1] == U'_' ? & s5 [2] : & s5 [0],
			s6 && s6 [0] == U'd' && s6 [1] == U'_' ? & s6 [2] : & s6 [0],
			s7 && s7 [0] == U'd' && s7 [1] == U'_' ? & s7 [2] : & s7 [0],
			s8 && s8 [0] == U'd' && s8 [1] == U'_' ? & s8 [2] : & s8 [0],
			s9 && s9 [0] == U'd' && s9 [1] == U'_' ? & s9 [2] : & s9 [0]);
	}
	file -> indent += 4;
}

#define PUTLEADER  \
	MelderFile_write (file, U"\n"); \
	if (file -> verbose) { \
		for (int iindent = 1; iindent <= file -> indent; iindent ++) { \
			MelderFile_write (file, U" "); \
		} \
		MelderFile_write (file, \
			s1 && s1 [0] == U'd' && s1 [1] == U'_' ? & s1 [2] : & s1 [0], \
			s2 && s2 [0] == U'd' && s2 [1] == U'_' ? & s2 [2] : & s2 [0], \
			s3 && s3 [0] == U'd' && s3 [1] == U'_' ? & s3 [2] : & s3 [0], \
			s4 && s4 [0] == U'd' && s4 [1] == U'_' ? & s4 [2] : & s4 [0], \
			s5 && s5 [0] == U'd' && s5 [1] == U'_' ? & s5 [2] : & s5 [0], \
			s6 && s6 [0] == U'd' && s6 [1] == U'_' ? & s6 [2] : & s6 [0], \
			s7 && s7 [0] == U'd' && s7 [1] == U'_' ? & s7 [2] : & s7 [0], \
			s8 && s8 [0] == U'd' && s8 [1] == U'_' ? & s8 [2] : & s8 [0], \
			s9 && s9 [0] == U'd' && s9 [1] == U'_' ? & s9 [2] : & s9 [0]); \
	}

void texputi8 (MelderFile file, int i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, i, file -> verbose ? U" " : nullptr);
}
void texputi16 (MelderFile file, int i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, i, file -> verbose ? U" " : nullptr);
}
void texputi32 (MelderFile file, long i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, i, file -> verbose ? U" " : nullptr);
}
void texputinteger (MelderFile file, integer number, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, number, file -> verbose ? U" " : nullptr);
}
void texputu8 (MelderFile file, unsigned int u, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, u, file -> verbose ? U" " : nullptr);
}
void texputu16 (MelderFile file, unsigned int u, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, u, file -> verbose ? U" " : nullptr);
}
void texputu32 (MelderFile file, unsigned long u, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, u, file -> verbose ? U" " : nullptr);
}
void texputr32 (MelderFile file, double x, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, Melder_single (x), file -> verbose ? U" " : nullptr);
}
void texputr64 (MelderFile file, double x, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, x, file -> verbose ? U" " : nullptr);
}
void texputc64 (MelderFile file, dcomplex z, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, z, file -> verbose ? U" " : nullptr);
}
void texputc128 (MelderFile file, dcomplex z, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, z, file -> verbose ? U" " : nullptr);
}
void texpute8 (MelderFile file, int i, conststring32 (*getText) (int), texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = <" : U"<", getText (i), file -> verbose ? U"> " : U">");
}
void texpute16 (MelderFile file, int i, conststring32 (*getText) (int), texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = <" : U"<", getText (i), file -> verbose ? U"> " : U">");
}
void texputeb (MelderFile file, bool i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = " : nullptr, i ? U"<true>" : U"<false>", file -> verbose ? U" " : nullptr);
}
void texputeq (MelderFile file, bool i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U"? " : nullptr, i ? U"<yes>" : U"<no>", file -> verbose ? U" " : nullptr);
}
void texputex (MelderFile file, bool i, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U"? " : nullptr, i ? U"<exists>" : U"<absent>", file -> verbose ? U" " : nullptr);
}
void texputs8 (MelderFile file, const char *s, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = \"" : U"\"");
	if (s) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, (char32) (char8) c);
			if (c == U'\"') MelderFile_writeCharacter (file, (char32) (char8) c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? U"\" " : U"\"");
}
void texputs16 (MelderFile file, const char *s, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = \"" : U"\"");
	if (s) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, (char32) (char8) c);
			if (c == '\"') MelderFile_writeCharacter (file, (char32) (char8) c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? U"\" " : U"\"");
}
void texputs32 (MelderFile file, const char *s, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = \"" : U"\"");
	if (s) {
		char c;
		while ((c = *s ++) != '\0') {
			MelderFile_writeCharacter (file, (char32) (char8) c);
			if (c == '\"') MelderFile_writeCharacter (file, (char32) (char8) c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? U"\" " : U"\"");
}
void texputw16 (MelderFile file, conststring32 s, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = \"" : U"\"");
	if (s) {
		char32 c;
		while ((c = *s ++) != U'\0') {
			MelderFile_writeCharacter (file, c);
			if (c == U'\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? U"\" " : U"\"");
}
void texputw32 (MelderFile file, conststring32 s, texput_UP_TO_NINE_NULLABLE_STRINGS) {
	PUTLEADER
	MelderFile_write (file, file -> verbose ? U" = \"" : U"\"");
	if (s) {
		char32 c;
		while ((c = *s ++) != U'\0') {
			MelderFile_writeCharacter (file, c);
			if (c == U'\"') MelderFile_writeCharacter (file, c);   // double any internal quotes
		}
	}
	MelderFile_write (file, file -> verbose ? U"\" " : U"\"");
}

/********** machine-independent binary I/O **********/

/* Optimizations for machines for which some of the formats are native. */

/* On which machines is "short" a two's complement Big-Endian (MSB-first) 2-byte word? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_16bitBE 1
	#define binario_16bitLE 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_16bitBE 0
	#define binario_16bitLE 1
#else
	#define binario_16bitBE 0
	#define binario_16bitLE 0
#endif

/* On which machines is "long" a two's complement Big-Endian (MSB-first) 4-byte word? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_32bitBE 1
	#define binario_32bitLE 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_32bitBE 0
	#define binario_32bitLE 1
#else
	#define binario_32bitBE 0
	#define binario_32bitLE 0
#endif

/* On which machines is "float" IEEE, four bytes, Most Significant Bit first? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_floatIEEE4msb (sizeof (float) == 4)
	#define binario_floatIEEE4lsb 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_floatIEEE4msb 0
	#define binario_floatIEEE4lsb (sizeof (float) == 4)
#else
	#define binario_floatIEEE4msb 0
	#define binario_floatIEEE4lsb 0
#endif

/* On which machines is "double" IEEE, eight bytes, Most Significant Bit first? */

#if defined (macintosh) && TARGET_RT_BIG_ENDIAN == 1
	#define binario_doubleIEEE8msb (sizeof (double) == 8)
	#define binario_doubleIEEE8lsb 0
#elif defined (_WIN32) || defined (macintosh) && TARGET_RT_LITTLE_ENDIAN == 1
	#define binario_doubleIEEE8msb 0
	#define binario_doubleIEEE8lsb (sizeof (double) == 8)
#else
	#define binario_doubleIEEE8msb 0
	#define binario_doubleIEEE8lsb 0
#endif

/*
	The routines bingetr32, bingetr64, binputr32, and binputr64,
	were implemented by Paul Boersma from the descriptions of the IEEE floating-point formats,
	as found in the MC68881/MC68882 User's Manual by Motorola (second edition, 1989).
	The following copyright notice only refers to the code of bingetr10 and binputr10.
*/

/* Copyright (C) 1988-1991 Apple Computer, Inc.
 * All rights reserved.
 *
 * Warranty Information
 *  Even though Apple has reviewed this software, Apple makes no warranty
 *  or representation, either express or implied, with respect to this
 *  software, its quality, accuracy, merchantability, or fitness for a
 *  particular purpose.  As a result, this software is provided "as is,"
 *  and you, its user, are assuming the entire risk as to its quality
 *  and accuracy.
 *
 * This code may be used and freely distributed as long as it includes
 * this copyright notice and the above warranty information.
 *
 * Machine-independent I/O routines for IEEE floating-point numbers.
 *
 * NaN's and infinities are converted to HUGE_VAL or HUGE, which
 * happens to be infinity on IEEE machines.  Unfortunately, it is
 * impossible to preserve NaN's in a machine-independent way.
 * Infinities are, however, preserved on IEEE machines.
 *
 * These routines have been tested on the following machines:
 *    Apple Macintosh, MPW 3.1 C compiler
 *    Apple Macintosh, THINK C compiler
 *    Silicon Graphics IRIS, MIPS compiler
 *    Cray X/MP and Y/MP
 *    Digital Equipment VAX
 *
 * Implemented by Malcolm Slaney and Ken Turkowski.
 *
 * Malcolm Slaney contributions during 1988-1990 include big- and little-
 * endian file I/O, conversion to and from Motorola's extended 80-bit
 * floating-point format, and conversions to and from IEEE single-
 * precision floating-point format.
 *
 * In 1991, Ken Turkowski implemented the conversions to and from
 * IEEE double-precision format, added more precision to the extended
 * conversions, and accommodated conversions involving +/- infinity,
 * NaN's, and denormalized numbers.
 */

/****************************************************************
 * Extended precision IEEE floating-point conversion routines.
 ****************************************************************/

/*
 * C O N V E R T   T O   I E E E   E X T E N D E D
 */

/*
 * C O N V E R T   F R O M   I E E E   E X T E N D E D  
 */

/*************** End of Apple Computer intermezzo. ***************/

static void readError (FILE *f, conststring32 text) {
	Melder_throw (feof (f) ? U"Reached end of file" : U"Error in file", U" while trying to read ", text);
}

static void writeError (conststring32 text) {
	Melder_throw (U"Error in file while trying to write ", text);
}

uint32 bingetu8 (FILE *f) {
	try {
		int32 externalValue = getc (f);   // either -1 (EOF) or in the range 0..255
		if (externalValue < 0)
			readError (f, U"a byte.");
		return (uint32) externalValue;
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from 1 byte in binary file.");
	}
}

void binputu8 (uint32 u, FILE *f) {
	try {
		if (putc ((int32) u, f) < 0)
			writeError (U"a byte.");
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not written to 1 byte in binary file.");
	}
}

int32 bingeti8 (FILE *f) {
	try {
		int32 externalValue = getc (f);
		if (externalValue < 0)
			readError (f, U"a byte.");
		return (signed char) externalValue;   // this converts e.g. 200 to -56
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 1 byte in binary file.");
	}
}

void binputi8 (int32 u, FILE *f) {
	try {
		if (putc (u, f) < 0)
			writeError (U"a byte.");
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 1 byte in binary file.");
	}
}

bool bingetbool8 (FILE *f) {
	try {
		int32 externalValue = getc (f);
		if (externalValue < 0)
			readError (f, U"a byte.");
		return (bool) externalValue;   // this converts e.g. 200 to true
	} catch (MelderError) {
		Melder_throw (U"Boolean not read from 1 byte in binary file.");
	}
}

void binputbool8 (bool value, FILE *f) {
	try {
		if (putc (value, f) < 0)
			writeError (U"a byte.");
	} catch (MelderError) {
		Melder_throw (U"Boolean not written to 1 byte in binary file.");
	}
}

int32 bingete8 (FILE *f, int min, int max, conststring32 type) {
	try {
		int32 externalValue = getc (f);
		if (externalValue < 0)
			readError (f, U"a byte.");
		int32 result = (signed char) externalValue;   // this converts e.g. 200 to -56, so the enumerated type is signed
		if (result < min || result > max)
			Melder_throw (result, U" is not a value of enumerated type <", type, U">.");
		return result;
	} catch (MelderError) {
		Melder_throw (U"Enumerated type not read from 1 byte in binary file.");
	}
}

void binpute8 (int32 value, FILE *f) {
	try {
		if (putc (value, f) < 0)
			writeError (U"a byte.");
	} catch (MelderError) {
		Melder_throw (U"Enumerated type not written to 1 byte in binary file.");
	}
}

int16 bingeti16 (FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			int16 s;
			if (fread (& s, sizeof (int16), 1, f) != 1) readError (f, U"a signed 16-bit integer.");
			return s;
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			return (int16)   // reinterpret sign bit
				((uint16) ((uint16) bytes [0] << 8) |
						   (uint16) bytes [1]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 2 bytes in binary file.");
	}
}

int16 bingeti16LE (FILE *f) {
	try {
		if (binario_16bitLE && ! (Melder_debug == 18)) {
			int16 s;
			if (fread (& s, sizeof (int16), 1, f) != 1) readError (f, U"a signed 16-bit integer.");
			return s;
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			return (int16)   // reinterpret sign bit
				((uint16) ((uint16) bytes [1] << 8) |
						   (uint16) bytes [0]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 2 bytes in binary file.");
	}
}

integer bingetinteger16BE (FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			int16 s;
			if (fread (& s, sizeof (int16), 1, f) != 1) readError (f, U"a signed 16-bit integer.");
			return s;
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			return (int16)   // reinterpret sign bit
				((uint16) ((uint16) bytes [0] << 8) |
						   (uint16) bytes [1]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 2 bytes in binary file.");
	}
}

uint16 bingetu16 (FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			uint16 s;
			if (fread (& s, sizeof (uint16), 1, f) != 1) readError (f, U"an unsigned 16-bit integer.");
			return s;   // without sign extension
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			return
				(uint16) ((uint16) bytes [0] << 8) |
						  (uint16) bytes [1];
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from 2 bytes in binary file.");
	}
}

uint16 bingetu16LE (FILE *f) {
	try {
		if (binario_16bitLE && ! (Melder_debug == 18)) {
			uint16 s;
			if (fread (& s, sizeof (uint16), 1, f) != 1) readError (f, U"an unsigned 16-bit integer.");
			return s;   // without sign extension
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			return
				(uint16) ((uint16) bytes [1] << 8) |
						  (uint16) bytes [0];
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from 2 bytes in binary file.");
	}
}

int32 bingete16 (FILE *f, int min, int max, conststring32 type) {
	try {
		int16 result;
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			if (fread (& result, sizeof (int16), 1, f) != 1) readError (f, U"a signed 16-bit integer.");
		} else {
			uint8 bytes [2];
			if (fread (bytes, sizeof (uint8), 2, f) != 2) readError (f, U"two bytes.");
			uint16 externalValue =
				(uint16) ((uint16) bytes [0] << 8) |
						  (uint16) bytes [1];
			result = (int16) externalValue;
		}
		if (result < min || result > max)
			Melder_throw (result, U" is not a value of enumerated type \"", type, U"\".");
		return (int32) result;
	} catch (MelderError) {
		Melder_throw (U"Enumerated value not read from 2 bytes in binary file.");
	}
}

int32 bingeti24 (FILE *f) {
	try {
		uint8 bytes [3];
		if (fread (bytes, sizeof (uint8), 3, f) != 3) readError (f, U"three bytes.");
		uint32 externalValue =
			(uint32) ((uint32) bytes [0] << 16) |
			(uint32) ((uint32) bytes [1] << 8) |
					  (uint32) bytes [2];
		if ((bytes [0] & 128) != 0)   // is the 24-bit sign bit on?
			externalValue |= 0xFF00'0000;   // extend negative sign to 32 bits
		return (int32) externalValue;   // reinterpret sign bit
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 3 bytes in binary file.");
	}
}

int32 bingeti24LE (FILE *f) {
	try {
		uint8 bytes [3];
		if (fread (bytes, sizeof (uint8), 3, f) != 3) readError (f, U"three bytes.");
		uint32 externalValue =
			(uint32) ((uint32) bytes [2] << 16) |
			(uint32) ((uint32) bytes [1] << 8) |
					  (uint32) bytes [0];
		if ((bytes [2] & 128) != 0)   // is the 24-bit sign bit on?
			externalValue |= 0xFF00'0000;   // extend negative sign to 32 bits
		return (int32) externalValue;   // reinterpret sign bit
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 3 bytes in binary file.");
	}
}

int32 bingeti32 (FILE *f) {
	try {
		if (binario_32bitBE && ! (Melder_debug == 18)) {
			int32 l;
			if (fread (& l, sizeof (int32), 1, f) != 1) readError (f, U"a signed 32-bit integer.");
			return l;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			return (int32)
				((uint32) ((uint32) bytes [0] << 24) |
				 (uint32) ((uint32) bytes [1] << 16) |
				 (uint32) ((uint32) bytes [2] << 8) |
						   (uint32) bytes [3]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 4 bytes in binary file.");
	}
}

int32 bingeti32LE (FILE *f) {
	try {
		if (binario_32bitLE && ! (Melder_debug == 18)) {
			int32 l;
			if (fread (& l, sizeof (int32), 1, f) != 1) readError (f, U"a signed 32-bit integer.");
			return l;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			return (int32)   // reinterpret sign bit
				((uint32) ((uint32) bytes [3] << 24) |
				 (uint32) ((uint32) bytes [2] << 16) |
				 (uint32) ((uint32) bytes [1] << 8) |
						   (uint32) bytes [0]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 4 bytes in binary file.");
	}
}

integer bingetinteger32BE (FILE *f) {
	try {
		if (binario_32bitBE && ! (Melder_debug == 18)) {
			int32 l;
			if (fread (& l, sizeof (int32), 1, f) != 1) readError (f, U"a signed 32-bit integer.");
			return (integer) l;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			return (integer) (int32)
				((uint32) ((uint32) bytes [0] << 24) |
				 (uint32) ((uint32) bytes [1] << 16) |
				 (uint32) ((uint32) bytes [2] << 8) |
						   (uint32) bytes [3]);
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not read from 4 bytes in binary file.");
	}
}

uint32 bingetu32 (FILE *f) {
	try {
		if (binario_32bitBE && ! (Melder_debug == 18)) {
			uint32 l;
			if (fread (& l, sizeof (uint32), 1, f) != 1) readError (f, U"an unsigned 32-bit integer.");
			return l;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			return
				(uint32) ((uint32) bytes [0] << 24) |
				(uint32) ((uint32) bytes [1] << 16) |
				(uint32) ((uint32) bytes [2] << 8) |
						  (uint32) bytes [3];
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from 4 bytes in binary file.");
	}
}

uint32 bingetu32LE (FILE *f) {
	try {
		if (binario_32bitLE && ! (Melder_debug == 18)) {
			uint32 l;
			if (fread (& l, sizeof (uint32), 1, f) != 1) readError (f, U"an unsigned 32-bit integer.");
			return l;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			return
				(uint32) ((uint32) bytes [3] << 24) |
				(uint32) ((uint32) bytes [2] << 16) |
				(uint32) ((uint32) bytes [1] << 8) |
						  (uint32) bytes [0];
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not read from 4 bytes in binary file.");
	}
}

double bingetr32 (FILE *f) {
	try {
		if (binario_floatIEEE4msb && ! (Melder_debug == 18)) {
			float x;
			if (fread (& x, sizeof (float), 1, f) != 1) readError (f, U"a 32-bit floating-point number.");
			return x;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			int32 exponent = (int32)
				((uint32) ((uint32) ((uint32) bytes [0] & 0x0000'007F) << 1) |
				 (uint32) ((uint32) ((uint32) bytes [1] & 0x0000'0080) >> 7));   // between 0 and 255 (it's signed because we're going to subtract something)
			uint32 mantissa =
				(uint32) ((uint32) ((uint32) bytes [1] & 0x0000'007F) << 16) |
						  (uint32) ((uint32) bytes [2] << 8) |
									(uint32) bytes [3];
			double x;
			if (exponent == 0)
				if (mantissa == 0) x = 0.0;
				else x = ldexp ((double) mantissa, exponent - 149);   // denormalized
			else if (exponent == 0x0000'00FF)   // Infinity or Not-a-Number
				return undefined;
			else   // finite
				x = ldexp ((double) (mantissa | 0x0080'0000), exponent - 150);
			return bytes [0] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not read from 4 bytes in binary file.");
	}
}

double bingetr32LE (FILE *f) {
	try {
		if (binario_floatIEEE4lsb && ! (Melder_debug == 18)) {
			float x;
			if (fread (& x, sizeof (float), 1, f) != 1) readError (f, U"a 32-bit floating-point number.");
			return x;
		} else {
			uint8 bytes [4];
			if (fread (bytes, sizeof (uint8), 4, f) != 4) readError (f, U"four bytes.");
			int32 exponent = (int32)
				((uint32) ((uint32) ((uint32) bytes [3] & 0x0000'007F) << 1) |
				 (uint32) ((uint32) ((uint32) bytes [2] & 0x0000'0080) >> 7));
			uint32 mantissa =
				(uint32) ((uint32) ((uint32) bytes [2] & 0x0000'007F) << 16) |
						  (uint32) ((uint32) bytes [1] << 8) |
									(uint32) bytes [0];
			double x;
			if (exponent == 0)
				if (mantissa == 0) x = 0.0;
				else x = ldexp ((double) mantissa, exponent - 149);   // denormalized
			else if (exponent == 0x0000'00FF)   // Infinity or Not-a-Number
				return undefined;
			else   // finite
				x = ldexp ((double) (mantissa | 0x0080'0000), exponent - 150);
			return bytes [3] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not read from 4 bytes in binary file.");
	}
}

double bingetr64 (FILE *f) {
	try {
		if (binario_doubleIEEE8msb && ! (Melder_debug == 18) || Melder_debug == 181) {
			double x;
			if (fread (& x, sizeof (double), 1, f) != 1) readError (f, U"a 64-bit floating-point number.");
			return x;
		} else {
			uint8 bytes [8];
			if (fread (bytes, sizeof (uint8), 8, f) != 8) readError (f, U"eight bytes.");
			int32 exponent = (int32)
				((uint32) ((uint32) ((uint32) bytes [0] & 0x0000'007F) << 4) |
				 (uint32) ((uint32) ((uint32) bytes [1] & 0x0000'00F0) >> 4));
			uint32 highMantissa =
				(uint32) ((uint32) ((uint32) bytes [1] & 0x0000'000F) << 16) |
						  (uint32) ((uint32) bytes [2] << 8) |
									(uint32) bytes [3];
			uint32 lowMantissa =
				(uint32) ((uint32) bytes [4] << 24) |
				(uint32) ((uint32) bytes [5] << 16) |
				(uint32) ((uint32) bytes [6] << 8) |
						  (uint32) bytes [7];
			double x;
			if (exponent == 0)
				if (highMantissa == 0 && lowMantissa == 0) x = 0.0;
				else x = ldexp ((double) highMantissa, exponent - 1042) +
					ldexp ((double) lowMantissa, exponent - 1074);   // denormalized
			else if (exponent == 0x0000'07FF)   // Infinity or Not-a-Number
				return undefined;
			else
				x = ldexp ((double) (highMantissa | 0x0010'0000), exponent - 1043) +
					ldexp ((double) lowMantissa, exponent - 1075);
			return bytes [0] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not read from 8 bytes in binary file.");
	}
}

double bingetr64LE (FILE *f) {
	try {
		if (binario_doubleIEEE8lsb && ! (Melder_debug == 18) || Melder_debug == 181) {
			double x;
			if (fread (& x, sizeof (double), 1, f) != 1) readError (f, U"a 64-bit floating-point number.");
			return x;
		} else {
			uint8 bytes [8];
			if (fread (bytes, sizeof (uint8), 8, f) != 8) readError (f, U"eight bytes.");
			int32 exponent = (int32)
				((uint32) ((uint32) ((uint32) bytes [7] & 0x0000'007F) << 4) |
				 (uint32) ((uint32) ((uint32) bytes [6] & 0x0000'00F0) >> 4));
			uint32 highMantissa =
				(uint32) ((uint32) ((uint32) bytes [6] & 0x0000'000F) << 16) |
						  (uint32) ((uint32) bytes [5] << 8) |
									(uint32) bytes [4];
			uint32 lowMantissa =
				(uint32) ((uint32) bytes [3] << 24) |
				(uint32) ((uint32) bytes [2] << 16) |
				(uint32) ((uint32) bytes [1] << 8) |
						  (uint32) bytes [0];
			double x;
			if (exponent == 0)
				if (highMantissa == 0 && lowMantissa == 0) x = 0.0;
				else x = ldexp ((double) highMantissa, exponent - 1042) +
					ldexp ((double) lowMantissa, exponent - 1074);   // denormalized
			else if (exponent == 0x0000'07FF)   // Infinity or Not-a-Number
				return undefined;
			else
				x = ldexp ((double) (highMantissa | 0x0010'0000), exponent - 1043) +
					ldexp ((double) lowMantissa, exponent - 1075);
			return bytes [7] & 0x80 ? - x : x;
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not read from 8 bytes in binary file.");
	}
}

double bingetr80 (FILE *f) {
	try {
		uint8 bytes [10];
		if (fread (bytes, sizeof (uint8), 10, f) != 10) readError (f, U"ten bytes.");
		int32 exponent = (int32)
			((uint32) ((uint32) ((uint32) bytes [0] & 0x0000'007F) << 8) |
								 (uint32) bytes [1]);   // between 0 and 32767
		uint32 highMantissa =
			(uint32) ((uint32) bytes [2] << 24) |
			(uint32) ((uint32) bytes [3] << 16) |
			(uint32) ((uint32) bytes [4] << 8) |
					  (uint32) bytes [5];
		uint32 lowMantissa =
			(uint32) ((uint32) bytes [6] << 24) |
			(uint32) ((uint32) bytes [7] << 16) |
			(uint32) ((uint32) bytes [8] << 8) |
					  (uint32) bytes [9];
		double x;
		if (exponent == 0 && highMantissa == 0 && lowMantissa == 0) x = 0.0;
		else if (exponent == 0x0000'7FFF) return undefined;   // Infinity or NaN
		else {
			exponent -= 16'383;   // between -16'382 and +16'383
			x = ldexp ((double) highMantissa, exponent - 31);
			x += ldexp ((double) lowMantissa, exponent - 63);
		}
		return bytes [0] & 0x80 ? - x : x;
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not read from 10 bytes in binary file.");
	}
}

void binputi16 (int16 i, FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			if (fwrite (& i, sizeof (short), 1, f) != 1)
				writeError (U"a signed 16-bit integer.");
		} else {
			uint8 bytes [2];
			bytes [0] = (uint8) (i >> 8);   // truncate
			bytes [1] = (uint8) i;   // truncate
			if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
				writeError (U"two bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 2 bytes in binary file.");
	}
}

void binputi16LE (int16 i, FILE *f) {
	try {
		if (binario_16bitLE && ! (Melder_debug == 18)) {
			if (fwrite (& i, sizeof (short), 1, f) != 1)
				writeError (U"a signed 16-bit integer.");
		} else {
			uint8 bytes [2];
			bytes [1] = (uint8) (i >> 8);   // truncate
			bytes [0] = (uint8) i;   // truncate
			if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
				writeError (U"two bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 2 bytes in binary file.");
	}
}

void binputinteger16BE (integer i, FILE *f) {
	try {
		if (i < INT16_MIN || i > INT16_MAX)
			Melder_throw (U"The number ", i, U" is too big to fit into 16 bits.");   // this will change in the future
		uint8 bytes [2];
		bytes [0] = (uint8) (i >> 8);   // truncate
		bytes [1] = (uint8) i;   // truncate
		if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
			writeError (U"two bytes.");
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 2 bytes in binary file.");
	}
}

void binputu16 (uint16 u, FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			if (fwrite (& u, sizeof (uint16), 1, f) != 1)
				writeError (U"an unsigned 16-bit integer.");
		} else {
			uint8 bytes [2];
			bytes [0] = (uint8) (u >> 8);   // truncate
			bytes [1] = (uint8) u;   // truncate
			if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
				writeError (U"two bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not written to 2 bytes in binary file.");
	}
}

void binputu16LE (uint16 u, FILE *f) {
	try {
		if (binario_16bitLE && ! (Melder_debug == 18)) {
			if (fwrite (& u, sizeof (uint16), 1, f) != 1)
				writeError (U"an unsigned 16-bit integer.");
		} else {
			uint8 bytes [2];
			bytes [1] = (uint8) (u >> 8);   // truncate
			bytes [0] = (uint8) u;   // truncate
			if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
				writeError (U"two bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not written to 2 bytes in binary file.");
	}
}

void binpute16 (int32 value, FILE *f) {
	try {
		if (binario_16bitBE && ! (Melder_debug == 18)) {
			short s = value;
			if (fwrite (& s, sizeof (short), 1, f) != 1)
				writeError (U"a signed 16-bit integer");
		} else {
			uint8 bytes [2];
			bytes [0] = (uint8) (value >> 8);   // truncate
			bytes [1] = (uint8) value;   // truncate
			if (fwrite (bytes, sizeof (uint8), 2, f) != 2)
				writeError (U"two bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Enumerated value not written to 2 bytes in binary file.");
	}
}

void binputi24 (int32 i, FILE *f) {
	try {
		uint8 bytes [3];
		bytes [0] = (uint8) (i >> 16);   // truncate
		bytes [1] = (uint8) (i >> 8);   // truncate
		bytes [2] = (uint8) i;   // truncate
		if (fwrite (bytes, sizeof (uint8), 3, f) != 3)
			writeError (U"three bytes");
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 3 bytes in binary file.");
	}
}

void binputi24LE (int32 i, FILE *f) {
	try {
		uint8 bytes [3];
		bytes [2] = (uint8) (i >> 16);   // truncate
		bytes [1] = (uint8) (i >> 8);   // truncate
		bytes [0] = (uint8) i;   // truncate
		if (fwrite (bytes, sizeof (uint8), 3, f) != 3)
			writeError (U"three bytes");
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 3 bytes in binary file.");
	}
}

void binputi32 (int32 i, FILE *f) {
	try {
		if (binario_32bitBE && ! (Melder_debug == 18)) {
			if (fwrite (& i, sizeof (int32), 1, f) != 1)
				writeError (U"a signed 32-bit integer.");
		} else {
			uint8 bytes [4];
			bytes [0] = (uint8) (i >> 24);   // truncate
			bytes [1] = (uint8) (i >> 16);   // truncate
			bytes [2] = (uint8) (i >> 8);   // truncate
			bytes [3] = (uint8) i;   // truncate
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 4 bytes in binary file.");
	}
}

void binputi32LE (int32 i, FILE *f) {
	try {
		if (binario_32bitLE && ! (Melder_debug == 18)) {
			if (fwrite (& i, sizeof (int32), 1, f) != 1)
				writeError (U"a signed 32-bit integer.");
		} else {
			uint8 bytes [4];
			bytes [3] = (uint8) (i >> 24);   // truncate
			bytes [2] = (uint8) (i >> 16);   // truncate
			bytes [1] = (uint8) (i >> 8);   // truncate
			bytes [0] = (uint8) i;   // truncate
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 4 bytes in binary file.");
	}
}

void binputinteger32BE (integer i, FILE *f) {
	try {
		if (i < INT32_MIN || i > INT32_MAX)
			Melder_throw (U"The number ", i, U" is too big to fit into 32 bits.");   // this will change in the future
		uint8 bytes [4];
		bytes [0] = (uint8) (i >> 24);   // truncate
		bytes [1] = (uint8) (i >> 16);   // truncate
		bytes [2] = (uint8) (i >> 8);   // truncate
		bytes [3] = (uint8) i;   // truncate
		if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
			writeError (U"a signed 32-bit integer.");
	} catch (MelderError) {
		Melder_throw (U"Signed integer not written to 4 bytes in binary file.");
	}
}

void binputu32 (uint32 u, FILE *f) {
	try {
		if (binario_32bitBE && ! (Melder_debug == 18)) {
			if (fwrite (& u, sizeof (uint32), 1, f) != 1)
				writeError (U"an unsigned 32-bit integer.");
		} else {
			uint8 bytes [4];
			bytes [0] = (uint8) (u >> 24);   // truncate
			bytes [1] = (uint8) (u >> 16);   // truncate
			bytes [2] = (uint8) (u >> 8);   // truncate
			bytes [3] = (uint8) u;   // truncate
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not written to 4 bytes in binary file.");
	}
}

void binputu32LE (uint32 u, FILE *f) {
	try {
		if (binario_32bitLE && ! (Melder_debug == 18)) {
			if (fwrite (& u, sizeof (uint32), 1, f) != 1)
				writeError (U"an unsigned 32-bit integer.");
		} else {
			uint8 bytes [4];
			bytes [3] = (uint8) (u >> 24);   // truncate
			bytes [2] = (uint8) (u >> 16);  // truncate
			bytes [1] = (uint8) (u >> 8);  // truncate
			bytes [0] = (uint8) u;  // truncate
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Unsigned integer not written to 4 bytes in binary file.");
	}
}

void binputr32 (double x, FILE *f) {
	try {
		if (binario_floatIEEE4msb && ! (Melder_debug == 18)) {
			float x32 = (float) x;   // convert down, with loss of precision
			if (fwrite (& x32, sizeof (float), 1, f) != 1)
				writeError (U"a 32-bit floating-point number.");
		} else {
			uint8 bytes [4];
			int sign, exponent;
			double fMantissa, fsMantissa;
			uint32 mantissa;
			if (x < 0.0) {
				sign = 0x0100;
				x *= -1.0;
			} else
				sign = 0;
			if (x == 0.0) {
				exponent = 0;
				mantissa = 0;
			} else {
				fMantissa = frexp (x, & exponent);
				if ((exponent > 128) || ! (fMantissa < 1.0))   // Infinity or Not-a-Number
					{ exponent = sign | 0x00FF; mantissa = 0; }   // Infinity
				else {   // finite
					exponent += 126;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 24);
					fsMantissa = floor (fMantissa); 
					mantissa = (uint32) fsMantissa & 0x007FFFFF;
				}
			}
			bytes [0] = (uint8) (exponent >> 1);   // truncate: bits 2 through 9 (bit 9 is the sign bit)
			bytes [1] = (uint8) ((exponent << 7) | (mantissa >> 16));   // truncate
			bytes [2] = (uint8) (mantissa >> 8);   // truncate
			bytes [3] = (uint8) mantissa;   // truncate
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not written to 4 bytes in binary file.");
	}
}

void binputr32LE (double x, FILE *f) {
	try {
		if (binario_floatIEEE4lsb && ! (Melder_debug == 18)) {
			float x32 = (float) x;   // convert down, with loss of precision
			if (fwrite (& x32, sizeof (float), 1, f) != 1)
				writeError (U"a 32-bit floating-point number.");
		} else {
			uint8 bytes [4];
			int sign, exponent;
			double fMantissa, fsMantissa;
			uint32 mantissa;
			if (x < 0.0) {
				sign = 0x0100;
				x *= -1.0;
			} else
				sign = 0;
			if (x == 0.0) {
				exponent = 0;
				mantissa = 0;
			} else {
				fMantissa = frexp (x, & exponent);
				if ((exponent > 128) || ! (fMantissa < 1.0))   // Infinity or Not-a-Number
					{ exponent = sign | 0x00FF; mantissa = 0; }   // Infinity
				else {   // finite
					exponent += 126;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 24);
					fsMantissa = floor (fMantissa);
					mantissa = (uint32) fsMantissa & 0x007F'FFFF;
				}
			}
			bytes [3] = (uint8) (exponent >> 1);
			bytes [2] = (uint8) ((exponent << 7) | (mantissa >> 16));
			bytes [1] = (uint8) (mantissa >> 8);
			bytes [0] = (uint8) mantissa;
			if (fwrite (bytes, sizeof (uint8), 4, f) != 4)
				writeError (U"four bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not written to 4 bytes in binary file.");
	}
}

void binputr64 (double x, FILE *f) {
	try {
		if (binario_doubleIEEE8msb && ! (Melder_debug == 18) || Melder_debug == 181) {
			if (fwrite (& x, sizeof (double), 1, f) != 1)
				writeError (U"a 64-bit floating-point number.");
		} else if (binario_doubleIEEE8lsb && ! (Melder_debug == 18)) {
			union { double xx; uint8 bytes [8]; };
			xx = x;
			std::swap (bytes [0], bytes [7]);
			std::swap (bytes [1], bytes [6]);
			std::swap (bytes [2], bytes [5]);
			std::swap (bytes [3], bytes [4]);
			if (fwrite (& xx, sizeof (double), 1, f) != 1)
				writeError (U"a 64-bit floating-point number.");
		} else {
			uint8 bytes [8];
			int sign, exponent;
			double fMantissa, fsMantissa;
			uint32 highMantissa, lowMantissa;
			if (x < 0.0) {
				sign = 0x0800;
				x *= -1.0;
			} else
				sign = 0;
			if (x == 0.0) {
				exponent = 0;
				highMantissa = 0;
				lowMantissa = 0;
			} else {
				fMantissa = frexp (x, & exponent);
				if (/*(exponent > 1024) ||*/ ! (fMantissa < 1.0))   // Infinity or Not-a-Number
					{ exponent = sign | 0x07FF; highMantissa = 0; lowMantissa = 0; }   // Infinity
				else { // finite
					exponent += 1022;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 21);
					fsMantissa = floor (fMantissa);
					highMantissa = (uint32) fsMantissa & 0x000F'FFFF;
					fMantissa = ldexp (fMantissa - fsMantissa, 32);
					fsMantissa = floor (fMantissa);
					lowMantissa = (uint32) fsMantissa;
				}
			}
			bytes [0] = (uint8) (exponent >> 4);
			bytes [1] = (uint8) ((exponent << 4) | (highMantissa >> 16));
			bytes [2] = (uint8) (highMantissa >> 8);
			bytes [3] = (uint8) highMantissa;
			bytes [4] = (uint8) (lowMantissa >> 24);
			bytes [5] = (uint8) (lowMantissa >> 16);
			bytes [6] = (uint8) (lowMantissa >> 8);
			bytes [7] = (uint8) lowMantissa;
			if (fwrite (bytes, sizeof (uint8), 8, f) != 8)
				writeError (U"eight bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not written to 8 bytes in binary file.");
	}
}

void binputr64LE (double x, FILE *f) {
	try {
		if (binario_doubleIEEE8lsb && ! (Melder_debug == 18) || Melder_debug == 181) {
			if (fwrite (& x, sizeof (double), 1, f) != 1)
				writeError (U"a 64-bit floating-point number.");
		} else if (binario_doubleIEEE8msb && ! (Melder_debug == 18)) {
			union { double xx; uint8 bytes [8]; };
			xx = x;
			std::swap (bytes [0], bytes [7]);
			std::swap (bytes [1], bytes [6]);
			std::swap (bytes [2], bytes [5]);
			std::swap (bytes [3], bytes [4]);
			if (fwrite (& xx, sizeof (double), 1, f) != 1)
				writeError (U"a 64-bit floating-point number.");
		} else {
			uint8 bytes [8];
			int sign, exponent;
			double fMantissa, fsMantissa;
			uint32 highMantissa, lowMantissa;
			if (x < 0.0) {
				sign = 0x0800;
				x *= -1.0;
			} else
				sign = 0;
			if (x == 0.0) {
				exponent = 0;
				highMantissa = 0;
				lowMantissa = 0;
			} else {
				fMantissa = frexp (x, & exponent);
				if (/*(exponent > 1024) ||*/ ! (fMantissa < 1.0))   // Infinity or Not-a-Number
					{ exponent = sign | 0x07FF; highMantissa = 0; lowMantissa = 0; }   // Infinity
				else { // finite
					exponent += 1022;   // add bias
					if (exponent <= 0) {   // denormalized
						fMantissa = ldexp (fMantissa, exponent - 1);
						exponent = 0;
					}
					exponent |= sign;
					fMantissa = ldexp (fMantissa, 21);
					fsMantissa = floor (fMantissa);
					highMantissa = (uint32) fsMantissa & 0x000F'FFFF;
					fMantissa = ldexp (fMantissa - fsMantissa, 32);
					fsMantissa = floor (fMantissa);
					lowMantissa = (uint32) fsMantissa;
				}
			}
			bytes [7] = (uint8) (exponent >> 4);
			bytes [6] = (uint8) ((exponent << 4) | (highMantissa >> 16));
			bytes [5] = (uint8) (highMantissa >> 8);
			bytes [4] = (uint8) highMantissa;
			bytes [3] = (uint8) (lowMantissa >> 24);
			bytes [2] = (uint8) (lowMantissa >> 16);
			bytes [1] = (uint8) (lowMantissa >> 8);
			bytes [0] = (uint8) lowMantissa;
			if (fwrite (bytes, sizeof (uint8), 8, f) != 8)
				writeError (U"eight bytes.");
		}
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not written to 8 bytes in binary file.");
	}
}

void binputr80 (double x, FILE *f) {
	try {
		uint8 bytes [10];
		Melder_assert (sizeof (int) > 2);
		int sign, exponent;   // these should be uint16, but frexp() expects an int
		double fMantissa, fsMantissa;
		uint32 highMantissa, lowMantissa;
		if (x < 0.0) {
			sign = 0x8000;
			x *= -1.0;
		} else
			sign = 0;
		if (x == 0.0) {
			exponent = 0;
			highMantissa = 0;
			lowMantissa = 0;
		} else {
			fMantissa = frexp (x, & exponent);
			if ((exponent > 16384) || ! (fMantissa < 1.0))   // Infinity or Not-a-Number
				{ exponent = sign | 0x7FFF; highMantissa = 0; lowMantissa = 0; }   // Infinity
			else {   // finite
				exponent += 16382;   // add bias
				if (exponent < 0) {   // denormalized
					fMantissa = ldexp (fMantissa, exponent);
					exponent = 0;
				}
				exponent |= sign;
				fMantissa = ldexp (fMantissa, 32);
				fsMantissa = floor (fMantissa);
				highMantissa = (uint32) fsMantissa;
				fMantissa = ldexp (fMantissa - fsMantissa, 32); 
				fsMantissa = floor (fMantissa); 
				lowMantissa = (uint32) fsMantissa;
			}
		}
		bytes [0] = (uint8) (exponent >> 8);
		bytes [1] = (uint8) exponent;
		bytes [2] = (uint8) (highMantissa >> 24);
		bytes [3] = (uint8) (highMantissa >> 16);
		bytes [4] = (uint8) (highMantissa >> 8);
		bytes [5] = (uint8) highMantissa;
		bytes [6] = (uint8) (lowMantissa >> 24);
		bytes [7] = (uint8) (lowMantissa >> 16);
		bytes [8] = (uint8) (lowMantissa >> 8);
		bytes [9] = (uint8) lowMantissa;
		if (fwrite (bytes, sizeof (uint8), 10, f) != 10)
			writeError (U"ten bytes.");
	} catch (MelderError) {
		Melder_throw (U"Floating-point number not written to 10 bytes in binary file.");
	}
}

dcomplex bingetc64 (FILE *f) {
	try {
		dcomplex result;
		result. real (bingetr32 (f));
		result. imag (bingetr32 (f));
		return result;
	} catch (MelderError) {
		Melder_throw (U"Complex number not read from 8 bytes in binary file.");
		dcomplex result { };
		return result;
	}
}

dcomplex bingetc128 (FILE *f) {
	try {
		dcomplex result;
		result. real (bingetr64 (f));
		result. imag (bingetr64 (f));
		return result;
	} catch (MelderError) {
		Melder_throw (U"Complex number not read from 16 bytes in binary file.");
		dcomplex result { };
		return result;
	}
}

void binputc64 (dcomplex z, FILE *f) {
	try {
		binputr32 (z.real(), f);
		binputr32 (z.imag(), f);
	} catch (MelderError) {
		Melder_throw (U"Complex number not written to 8 bytes in binary file.");
	}
}

void binputc128 (dcomplex z, FILE *f) {
	try {
		binputr64 (z.real(), f);
		binputr64 (z.imag(), f);
	} catch (MelderError) {
		Melder_throw (U"Complex number not written to 16 bytes in binary file.");
	}
}

autostring8 bingets8 (FILE *f) {
	try {
		uint32 length = bingetu8 (f);
		autostring8 result (length);
		if (fread (result.get(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? U"Reached end of file" : U"Error in file", U" while trying to read ", length, U" one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

autostring8 bingets16 (FILE *f) {
	try {
		uint16 length = bingetu16 (f);
		autostring8 result (length);
		if (fread (result.get(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? U"Reached end of file" : U"Error in file", U" while trying to read ", length, U" one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

autostring8 bingets32 (FILE *f) {
	try {
		uint32 length = bingetu32 (f);
		autostring8 result (length);
		if (fread (result.get(), sizeof (char), length, f) != length)
			Melder_throw (feof (f) ? U"Reached end of file" : U"Error in file", U" while trying to read ", length, U" one-byte characters.");
		result [length] = 0;   // trailing null byte
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

autostring32 bingetw8 (FILE *f) {
	try {
		autostring32 result;
		uint32 length = bingetu8 (f);
		if (length == 0xFF) {   // an escape for encoding
			/*
				UTF-16
			*/
			length = bingetu8 (f);
			result = autostring32 (length);
			for (unsigned int i = 0; i < length; i ++) {
				char32 kar = bingetu16 (f);
				if ((kar & 0x00'F800) == 0x00'D800) {
					if (kar > 0x00'DBFF)
						Melder_throw (U"Incorrect Unicode value (first surrogate member ", kar, U").");
					char32 kar2 = bingetu16 (f);
					if (kar2 < 0x00'DC'00 || kar2 > 0x00'DF'FF)
						Melder_throw (U"Incorrect Unicode value (second surrogate member ", kar2, U").");
					result [i] = (((kar & 0x00'03FF) << 10) | (kar2 & 0x00'03FF)) + 0x01'0000;
				} else {
					result [i] = kar;
				}
			}
		} else {
			/*
				ASCII
			*/
			result = autostring32 (length);
			for (uint32 i = 0; i < length; i ++)
				result [i] = bingetu8 (f);
		}
		result [length] = U'\0';
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

autostring32 bingetw16 (FILE *f) {
	try {
		autostring32 result;
		uint16 length = bingetu16 (f);
		if (length == 0xFFFF) {   // an escape for encoding
			/*
				UTF-16
			*/
			length = bingetu16 (f);
			result = autostring32 (length);
			for (uint16 i = 0; i < length; i ++) {
				char32 kar = (char32) (char16) bingetu16 (f);
				if ((kar & 0x00'F800) == 0x00'D800) {
					if (kar > 0x00'DBFF)
						Melder_throw (U"Incorrect Unicode value (first surrogate member ", kar, U").");
					char32 kar2 = (char32) (char16) bingetu16 (f);
					if (kar2 < 0x00'DC00 || kar2 > 0x00'DFFF)
						Melder_throw (U"Incorrect Unicode value (second surrogate member ", kar2, U").");
					result [i] = (((kar & 0x00'03FF) << 10) | (kar2 & 0x00'03FF)) + 0x01'0000;
				} else {
					result [i] = kar;
				}
			}
		} else {
			/*
				ASCII
			*/
			result = autostring32 (length);
			for (uint16 i = 0; i < length; i ++)
				result [i] = (char32) (char8) bingetu8 (f);
		}
		result [length] = U'\0';
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

autostring32 bingetw32 (FILE *f) {
	try {
		autostring32 result;
		uint32 length = bingetu32 (f);
		if (length == 0xFFFF'FFFF) {   // an escape for encoding
			/*
				UTF-16
			*/
			length = bingetu32 (f);
			result = autostring32 (length);
			for (uint32 i = 0; i < length; i ++) {
				char32 kar = bingetu16 (f);
				if ((kar & 0x00'F800) == 0x00'D800) {
					if (kar > 0x00'DBFF)
						Melder_throw (U"Incorrect Unicode value (first surrogate member ", kar, U").");
					char32 kar2 = bingetu16 (f);
					if (kar2 < 0x00'DC00 || kar2 > 0x00'DFFF)
						Melder_throw (U"Incorrect Unicode value (second surrogate member ", kar2, U").");
					result [i] = (((kar & 0x00'03FF) << 10) | (kar2 & 0x00'03FF)) + 0x01'0000;
				} else {
					result [i] = kar;
				}
			}
		} else {
			/*
				ASCII
			*/
			result = autostring32 (length);
			for (uint32 i = 0; i < length; i ++)
				result [i] = bingetu8 (f);
		}
		result [length] = U'\0';
		return result;
	} catch (MelderError) {
		Melder_throw (U"Text not read from a binary file.");
	}
}

void binputs8 (const char *s, FILE *f) {
	try {
		if (! s) {
			binputu8 (0, f);
		} else {
			size_t length = strlen (s);
			if (length > UINT8_MAX) {
				Melder_warning (U"Text of ", length, U" characters truncated to 255 characters.");
				length = UINT8_MAX;
			}
			binputu8 ((uint32) length, f);   // guarded conversion down (or not: `length` may be uint32 or uint64)
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw (U"Error in file while trying to write ", length, U" one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

void binputs16 (const char *s, FILE *f) {
	try {
		if (! s) {
			binputu16 (0, f);
		} else {
			size_t length = strlen (s);
			if (length > UINT16_MAX) {
				Melder_warning (U"Text of ", length, U" characters truncated to 65535 characters.");
				length = UINT16_MAX;
			}
			binputu16 ((uint16) length, f);   // guarded conversion down
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw (U"Error in file while trying to write ", length, U" one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

void binputs32 (const char *s, FILE *f) {
	try {
		if (! s) {
			binputu32 (0, f);
		} else {
			size_t length = strlen (s);
			if (length > UINT32_MAX) {
				Melder_warning (U"Text of ", length, U" characters truncated to 4,294,967,295 characters.");
				length = UINT32_MAX;
			}
			binputu32 ((uint32) length, f);   // guarded conversion down (unless size_t is already uint32)
			if (fwrite (s, sizeof (char), length, f) != length)
				Melder_throw (U"Error in file while trying to write ", length, U" one-byte characters.");
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

static inline void binpututf16 (char32 kar, FILE *f) {
	if (kar <= 0x00'FFFF) {
		binputu16 ((uint16) kar, f);   // truncate to lower 16 bits
	} else if (kar <= 0x10'FFFF) {
		kar -= 0x01'0000;
		binputu16 ((uint16) (0x00'D800 | (kar >> 10)), f);
		binputu16 ((uint16) (0x00'DC00 | (kar & 0x00'03FF)), f);
	} else {
		Melder_crash (U"Impossible Unicode value.");
	}
}

void binputw8 (conststring32 s, FILE *f) {
	try {
		if (! s) {
			binputu8 (0, f);
		} else {
			integer length = Melder_length (s);
			if (length > UINT8_MAX - 1) {
				Melder_warning (U"Text of ", length, U" characters truncated to 254 characters.");
				length = UINT8_MAX - 1;
			}
			if (Melder_isValidAscii (s)) {
				/*
					ASCII
				*/
				binputu8 ((uint32) length, f);   // guarded conversion down
				for (integer i = 0; i < length; i ++)
					binputu8 ((uint32) (uint8) s [i], f);   // guarded conversion down from char32 to uint8
			} else {
				/*
					UTF-16
				*/
				binputu8 (0xFF, f);   // an escape for multibyte encoding
				binputu8 ((uint32) length, f);
				for (integer i = 0; i < length; i ++)
					binpututf16 (s [i], f);
			}
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

void binputw16 (conststring32 s, FILE *f) {
	try {
		if (! s) {
			binputu16 (0, f);
		} else {
			integer length = Melder_length (s);
			if (length > UINT16_MAX - 1) {
				Melder_warning (U"Text of ", length, U" characters truncated to 65534 characters.");
				length = UINT16_MAX - 1;
			}
			if (Melder_isValidAscii (s)) {
				/*
					ASCII
				*/
				binputu16 ((uint16) length, f);   // guarded conversion down
				for (int64 i = 0; i < length; i ++)
					binputu8 ((uint32) (uint8) s [i], f);   // guarded conversion down from char32 to uint8
			} else {
				/*
					UTF-16
				*/
				binputu16 (0xFFFF, f);   // an escape for multibyte encoding
				binputu16 ((uint16) length, f);   // guarded conversion down
				for (int64 i = 0; i < length; i ++)
					binpututf16 (s [i], f);
			}
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

void binputw32 (conststring32 s, FILE *f) {
	try {
		if (! s) {
			binputu32 (0, f);
		} else {
			integer length = Melder_length (s);   // on 32-bit systems the length cannot exceed 2^30 char32s, i.e. `length` must be positive
			if (length > UINT32_MAX - 1) {
				Melder_warning (U"Text of ", length, U" characters truncated to 4,294,967,294 characters.");
				length = UINT32_MAX - 1;
			}
			if (Melder_isValidAscii (s)) {
				/*
					ASCII
				*/
				binputu32 ((uint32) length, f);   // guarded conversion down on 64-bit systems, and valid even on 32-bit systems
				for (integer i = 0; i < length; i ++)
					binputu8 ((uint32) (uint8) s [i], f);   // guarded conversion down from char32 to uint8
			} else {
				/*
					UTF-16
				*/
				binputu32 (0xFFFF'FFFF, f);   // an escape for multibyte encoding
				binputu32 ((uint32) length, f);   // guarded conversion down
				for (integer i = 0; i < length; i ++)
					binpututf16 (s [i], f);
			}
		}
	} catch (MelderError) {
		Melder_throw (U"Text not written to a binary file.");
	}
}

/* End of file abcio.cpp */
