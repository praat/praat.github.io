/* STR.cpp
 *
 * Copyright (C) 2012-2017 David Weenink, 2008,2018,2020-2022,2024-2026 Paul Boersma
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

static char hexSymbols [] = "0123456789ABCDEF";

static uint64 hexSecret = UINT64_C (5'847'171'831'059'823'557);

autostring8 hex_STR8 (conststring8 string, uint64 key) {
	if (key != 0)
		NUMrandom_initializeWithSeedUnsafelyButPredictably (key ^ hexSecret);
	autostring8 result (Melder8_length (string) * 2);
	char *to = & result [0];
	for (const char8 *from = (char8 *) & string [0]; *from != '\0'; from ++) {
		integer value = *from;
		Melder_assert (value > 0 && value < 256);
		if (key != 0)
			value = (value + NUMrandomInteger (0, 255)) % 256;
		*to ++ = hexSymbols [value / 16];
		*to ++ = hexSymbols [value % 16];
	}
	*to = '\0';
	if (key != 0)
		NUMrandom_initializeSafelyAndUnpredictably ();
	return result;
}

autostring32 hex_STR (conststring32 string, uint64 key) {
	autostring8 string8 = Melder_32to8 (string);
	string8 = hex_STR8 (string8.get(), key);
	return Melder_8to32_e (string8.get());
}

autostring32 left_STR (conststring32 string, integer newLength) {
	const integer length = Melder_length (string);
	Melder_clip (0_integer, & newLength, length);
	autostring32 result (newLength);
	str32ncpy (result.get(), string, newLength);
	return result;
}

autostring32 lowerCase_STR (conststring32 string) {
	const integer length = Melder_length (string);
	autostring32 result (length);
	for (integer i = 0; i < length; i ++)
		result [i] = Melder_toLowerCase (string [i]);
	return result;
}
autostring32 lowerCamelCase_STR (conststring32 string) {
	autostring32 result (Melder_length (string) + 1);
	char32 *q = & result [0];
	bool nextLetterShouldBeUpperCase = false;
	bool nextLetterShouldBeLowerCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*q ++ = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (nextLetterShouldBeLowerCase) {
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}
autostring32 lowerSnakeCase_STR (conststring32 string) {
	autostring32 result (2 * Melder_length (string) + 1);   // make room for extra underscores
	char32 *q = & result [0];
	bool nextLetterShouldBeLowerCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeLowerCase) {
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = false;
		} else if (Melder_isUpperCaseLetter (*p)) {
			*q ++ = U'_';
			*q ++ = Melder_toLowerCase (*p);
			nextLetterShouldBeLowerCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}

autostring32 mid_STR (conststring32 string, integer startingPosition_1, integer numberOfCharacters) {
	const integer length = Melder_length (string);
	integer endPosition_1 = startingPosition_1 + numberOfCharacters - 1;
	if (startingPosition_1 < 1)
		startingPosition_1 = 1;
	if (endPosition_1 > length)
		endPosition_1 = length;
	const integer newLength = endPosition_1 - startingPosition_1 + 1;
	if (newLength <= 0)
		return Melder_dup (U"");
	autostring32 result (newLength);
	str32ncpy (result.get(), & string [startingPosition_1-1], newLength);
	return result;
}

autostring32 padLeft_STR (const conststring32 string, const integer width, const conststring32 pad) {
	return Melder_dup (Melder_padLeft (string, width, pad));
}

autostring32 padOrTruncateLeft_STR (const conststring32 string, const integer width, const conststring32 pad) {
	return Melder_dup (Melder_padOrTruncateLeft (string, width, pad));
}

autostring32 padOrTruncateRight_STR (const conststring32 string, const integer width, const conststring32 pad) {
	return Melder_dup (Melder_padOrTruncateRight (string, width, pad));
}

autostring32 padRight_STR (const conststring32 string, const integer width, const conststring32 pad) {
	return Melder_dup (Melder_padRight (string, width, pad));
}

autostring32 quote_doubleSTR (conststring32 string) {
	/*
		Put the string `str` between quotes, doubling each quote inside `str`.
	*/
	constexpr integer maximumNumberOfOutputCharactersPerInputCharacter = 2;   // namely, the character " has to be converted to ""
	constexpr integer numberOfOutputCharactersNeededForTheQuotes = 2;   // namely, the opening quote and the closing quote
	autostring32 result (maximumNumberOfOutputCharactersPerInputCharacter * Melder_length (string) + numberOfOutputCharactersNeededForTheQuotes);
		// for instance, the string `"""` has to be converted to the string `""""""""`
	const char32 *p = & string [0];
	char32 *q = & result [0];
	* q ++ = U'"';
	while (*p != U'\0') {
		if (*p == U'"')
			* q ++ = U'\"';
		* q ++ = * p ++;
	}
	* q ++ = U'"';
	*q = U'\0';
	return result;
}

autostring32 replace_STR (conststring32 string,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *out_numberOfMatches)
{
	/*
		Sanitize input.
	*/
	if (! string)
		string = U"";
	if (! search)
		search = U"";
	if (! replace)
		replace = U"";
	const integer len_string = Melder_length (string);
	if (len_string == 0)
		maximumNumberOfReplaces = 1;

	const integer len_search = Melder_length (search);
	if (len_search == 0)
		maximumNumberOfReplaces = 1;

	/*
		To allocate memory for 'result' only once, we have to know how many
		matches will occur.
	*/

	const char32 *pos = & string [0];   // current position / start of current match
	integer numberOfMatches = 0;
	if (maximumNumberOfReplaces <= 0)
		maximumNumberOfReplaces = INTEGER_MAX;

	if (len_search == 0) {   /* Search is empty string... */
		if (len_string == 0)
			numberOfMatches = 1;   /* ...only matches empty string */
	} else {
		if (len_string != 0) {   /* Because empty string always matches */
			while (!! (pos = str32str (pos, search)) && numberOfMatches < maximumNumberOfReplaces) {
				pos += len_search;
				numberOfMatches ++;
			}
		}
	}

	const integer len_replace = Melder_length (replace);
	const integer len_result = len_string + numberOfMatches * (len_replace - len_search);
	autostring32 result (len_result);

	const char32 *posp = pos = & string [0];
	integer nchar = 0, result_nchar = 0;
	for (integer i = 1; i <= numberOfMatches; i ++) {
		pos = str32str (pos, search);

		/*
			Copy gap between end of previous match and start of current.
		*/
		nchar = pos - posp;
		if (nchar > 0) {
			str32ncpy (& result [result_nchar], posp, nchar);
			result_nchar += nchar;
		}

		/*
			Insert the replace string in result.
		*/
		str32ncpy (& result [result_nchar], replace, len_replace);
		result_nchar += len_replace;

		/*
			Next search starts after the match.
		*/
		pos += len_search;
		posp = pos;
	}

	/*
		Copy gap between end of match and end of string.
	*/
	pos = string + len_string;
	nchar = pos - posp;
	if (nchar > 0)
		str32ncpy (& result [result_nchar], posp, nchar);
	if (out_numberOfMatches)
		*out_numberOfMatches = numberOfMatches;
	return result;
}

autostring32 replace_regex_STR (conststring32 string,
	regexp *compiledSearchRE, conststring32 replaceRE, integer maximumNumberOfReplaces,
	integer *out_numberOfMatches)
{
	/*
		Sanitize input.
	*/
	if (! string)
		string = U"";
	if (! compiledSearchRE)
		return autostring32();
	if (! replaceRE)
		replaceRE = U"";

	integer buf_nchar = 0;   // number of characters in 'buf'
	integer gap_copied = 0;
	integer nchar;
	bool reverse = false;
	int errorType;
	char32 prev_char = U'\0';
	const char32 *pos;   // current position in 'string' / start of current match
	const char32 *posp;   // end of previous match
	autostring32 buf;

	if (out_numberOfMatches)
		*out_numberOfMatches = 0;

	const integer string_length = Melder_length (string);
	//int replace_length = Melder_length (replaceRE);
	if (string_length == 0)
		maximumNumberOfReplaces = 1;

	integer i = ( maximumNumberOfReplaces > 0 ? 0 : - string_length );

	/*
		We do not know the size of the replaced string in advance,
		therefore we allocate a replace buffer twice the size of the
		original string. After all replaces have taken place we do a
		final realloc to the then exactly known size.
		If during the replace, the size of the buffer happens to be too
		small (this is signalled by the replaceRE function),
		we double its size and restart the replace.
	*/

	integer bufferLength = 2 * string_length;
	bufferLength = bufferLength < 100 ? 100 : bufferLength;
	buf. resize (bufferLength);

	pos = posp = string;
	while (ExecRE (compiledSearchRE, nullptr, pos, nullptr, reverse, prev_char, U'\0', nullptr, nullptr) &&
			i ++ < maximumNumberOfReplaces)
	{
		/*
			Copy gap between the end of the previous match and the start
			of the current match.
			Check buffer overflow. pos == posp ? '\0' : pos [-1],
		*/
		pos = compiledSearchRE -> startp [0];
		nchar = pos - posp;
		if (nchar > 0 && ! gap_copied) {
			if (buf_nchar + nchar > bufferLength) {
				bufferLength *= 2;
				buf. resize (bufferLength);
			}
			str32ncpy (buf.get() + buf_nchar, posp, nchar);
			buf_nchar += nchar;
		}

		gap_copied = 1;

		/*
			Do the substitution. We can check for buffer overflow only afterwards.
			SubstituteRE puts null byte at last replaced position and signals when overflow.
		*/
		if (! SubstituteRE (compiledSearchRE, replaceRE, buf.get() + buf_nchar, bufferLength + 1 - buf_nchar, & errorType)) {
			if (errorType == 1) {   // not enough memory
				bufferLength *= 2;
				buf. resize (bufferLength);
				Melder_clearError ();
				i --;   // retry
				continue;
			}
			Melder_throw (U"Error during substitution.");
		}

		/*
			Buffer is not full; get number of characters added.
		*/
		nchar = Melder_length (buf.get() + buf_nchar);
		buf_nchar += nchar;

		/*
			Update next start position in search string.
		*/
		posp = pos;
		pos = (char32 *) compiledSearchRE -> endp [0];
		if (pos != posp)
			prev_char = pos [-1];
		gap_copied = 0;
		posp = pos; //pb 20080121
		if (out_numberOfMatches)
			(*out_numberOfMatches) ++;
		// at end of string?
		// we need this because .* matches at the end of a string
		if (pos - string == string_length)
			break;
	}

	/*
		Copy last part of string to destination string
	*/
	nchar = (string + string_length) - pos;
	bufferLength = buf_nchar + nchar;
	buf. resize (bufferLength);
	str32ncpy (buf.get() + buf_nchar, pos, nchar);

	return buf;
}

autostring32 right_STR (conststring32 string, integer newLength) {
	const integer length = Melder_length (string);
	Melder_clip (0_integer, & newLength, length);
	return Melder_dup (string + length - newLength);
}

autostring32 truncateLeft_STR (const conststring32 string, const integer width) {
	return Melder_dup (Melder_truncateLeft (string, width));
}
autostring32 truncateRight_STR (const conststring32 string, const integer width) {
	return Melder_dup (Melder_truncateRight (string, width));
}

autostring8 unhex_STR8 (conststring8 string, uint64 key) {
	if (key != 0)
		NUMrandom_initializeWithSeedUnsafelyButPredictably (key ^ hexSecret);
	autostring8 result (Melder8_length (string) / 2);
	char *to = & result [0];
	for (const char8 *from = (char8 *) & string [0];;) {
		char8 code1 = *from ++;
		while (Melder_isHorizontalOrVerticalSpace (code1))
			code1 = *from ++;
		if (code1 == '\0')
			break;
		char8 code2 = *from ++;
		while (Melder_isHorizontalOrVerticalSpace (code2))
			code2 = *from ++;
		if (code2 == '\0')
			Melder_throw (U"(unhex$:) incomplete hexadecimal string.");
		const char *index1 = strchr (hexSymbols, code1), *index2 = strchr (hexSymbols, code2);
		if (! index1 || ! index2)
			Melder_throw (U"(unhex$:) not a hexadecimal string: ", Melder_peek8to32_u (string));
		integer value = (index1 - hexSymbols) * 16 + (index2 - hexSymbols);
		if (key != 0)
			value = (value + 256 - NUMrandomInteger (0, 255)) % 256;
		*to ++ = char (value);
	}
	*to = '\0';
	if (key != 0)
		NUMrandom_initializeSafelyAndUnpredictably ();
	return result;
}

autostring32 unhex_STR (conststring32 string, uint64 key) {
	autostring8 string8 = Melder_32to8 (string);
	string8 = unhex_STR8 (string8.get(), key);
	return Melder_8to32_e (string8.get());
}

autostring32 upperCase_STR (conststring32 string) {
	const integer length = Melder_length (string);
	autostring32 result (length);
	for (integer i = 0; i < length; i ++)
		result [i] = Melder_toUpperCase (string [i]);
	return result;
}
autostring32 upperCamelCase_STR (conststring32 string) {
	autostring32 result (Melder_length (string) + 1);
	char32 *q = & result [0];
	bool nextLetterShouldBeUpperCase = true;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*q ++ = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		} else
			*q ++ = *p;
	}
	*q = U'\0';   // closing null byte
	return result;
}
autostring32 upperSnakeCase_STR (conststring32 string) {
	autostring32 result = lowerSnakeCase_STR (string);
	bool nextLetterShouldBeUpperCase = true;
	for (char32 *p = & result [0]; *p != U'\0'; p ++) {
		if (nextLetterShouldBeUpperCase) {
			*p = Melder_toUpperCase (*p);
			nextLetterShouldBeUpperCase = false;
		} else if (*p == U'_') {
			nextLetterShouldBeUpperCase = true;
		}
	}
	return result;
}

/* End of file STR.cpp */
