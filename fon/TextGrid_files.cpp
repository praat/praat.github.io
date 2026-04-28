/* TextGrid_files.cpp
 *
 * Copyright (C) 1992-2026 Paul Boersma
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

#include "TextGrid.h"

static int64 Melder_readInteger (const char32 **p) {
	char32 kar = * (*p) ++;
	Melder_require (kar != U'\0',
		U"Looking for an integer, but found the end of the text.");
	Melder_require (! Melder_isVerticalSpace (kar),
		U"Looking for an integer, but found the end of the line.");
	const bool hasSign = ( kar == U'-' || kar == U'+' );
	Melder_require (hasSign || Melder_isAsciiDecimalNumber (kar),
		U"Looking for an integer, but found “", *p, U"”.");
	constexpr integer MAXIMUM_NUMBER_OF_DIGITS = 40;
	char buffer [1 + MAXIMUM_NUMBER_OF_DIGITS + 1];   // include room for leading sign and trailing null
	/* mutable increment */ integer ipos = 0;
	buffer [ipos ++] = (char) (char8) kar;
	if (hasSign) {
		char32 shouldBeDigit = * (*p) ++;
		Melder_require (Melder_isAsciiDecimalNumber (shouldBeDigit),
			U"Looking for a digit after “", kar, U"”, but found “", *p, U"”.");
		buffer [ipos ++] = (char) (char8) shouldBeDigit;
	}
	for (;;) {
		char32 mayBeDigit = * (*p) ++;
		if (Melder_isAsciiDecimalNumber (mayBeDigit)) {
			Melder_require (ipos < hasSign + MAXIMUM_NUMBER_OF_DIGITS,
				U"Looking for a normal-sized integer, but found more than ", MAXIMUM_NUMBER_OF_DIGITS, U" digits.");
			buffer [ipos ++] = (char) (char8) mayBeDigit;
		} else {
			(*p) --;
			break;
		}
	}
	buffer [ipos] = '\0';
	return strtoll (buffer, nullptr, 10);
}

static double Melder_readReal (const char32 **p) {
	const char32 kar = * (*p) ++;
	Melder_require (kar != U'\0',
		U"Looking for a real number, but found the end of the text.");
	Melder_require (! Melder_isVerticalSpace (kar),
		U"Looking for a real number, but found the end of the line.");
	const bool hasSign = ( kar == U'-' || kar == U'+' );
	Melder_require (hasSign || Melder_isAsciiDecimalNumber (kar),
		U"Looking for a real number, but found “", *p, U"”.");
	constexpr integer MAXIMUM_NUMBER_OF_CHARACTERS = 100;
	char buffer [1 + MAXIMUM_NUMBER_OF_CHARACTERS + 1];   // include room for leading sign and trailing null
	/* mutable increment */ integer ipos = 0;
	buffer [ipos ++] = (char) (char8) kar;
	if (hasSign) {
		char32 shouldBeDigit = * (*p) ++;
		Melder_require (Melder_isAsciiDecimalNumber (shouldBeDigit),
			U"Looking for a digit after “", kar, U"”, but found “", *p, U"”.");
		buffer [ipos ++] = (char) (char8) shouldBeDigit;
	}
	for (;;) {
		char32 numberCharacter = * (*p) ++;
		if (Melder_isAsciiDecimalNumber (numberCharacter) || numberCharacter == U'+' || numberCharacter == U'-' ||
			numberCharacter == U'e' || numberCharacter == U'E' || numberCharacter == U'.'
		) {
			Melder_require (ipos < hasSign + MAXIMUM_NUMBER_OF_CHARACTERS,
				U"Looking for a normal-sized real number, but found more than ", MAXIMUM_NUMBER_OF_CHARACTERS, U" digits.");
			buffer [ipos ++] = (char) (char8) numberCharacter;
		} else {
			(*p) --;
			break;
		}
	}
	buffer [ipos] = '\0';
	return Melder_a8tof (buffer);
}

autoTextGrid TextGrid_readFromEspsLabelFile (
	const MelderFile file,
	const bool tiersArePointTiers,
	const integer overrideNumberOfTiers
) {
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);   // going to be UTF-8-compatible

		/*
			Cycle through all lines until encountering a line that starts with '#'.
		*/
		/* mutable count */ integer numberOfTiers = 1;
		char32 separator = U';';
		for (;;) {
			const conststring32 line = MelderReadText_readLine (text.get());
			if (! line)
				Melder_throw (U"Missing '#' line.");
			if (line [0] == '#')
				break;
			if (Melder_startsWith (line, U"nfields "))
				numberOfTiers = Melder_atoi (line + 8);
			if (Melder_startsWith (line, U"separator "))
				separator = line [10];
			else if (Melder_startsWith (line, U"separator"))
				separator = line [9];
		}
		Melder_require (numberOfTiers >= 1,
			U"The number of tiers has to be at least 1, but the file states that it should be ", numberOfTiers, U".");
		Melder_require (numberOfTiers <= 1'000'000'000,
			U"The number of tiers has to be at most 1,000,000,000, but the file states that it should be ", numberOfTiers, U".");
		if (overrideNumberOfTiers >= 1)
			numberOfTiers = overrideNumberOfTiers;

		/* mutable preliminary */ double globalTmin = 0.0, globalTmax = 1e-6;
		autoTextGrid me = TextGrid_createWithoutTiers (globalTmin, globalTmax);
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			autoFunction tier = tiersArePointTiers ? static_cast <autoFunction> (TextTier_create (globalTmin, globalTmax)) :
													 static_cast <autoFunction> (IntervalTier_create_raw (globalTmin, globalTmax));
			Thing_setName (tier.get(), Melder_integer (itier));
			my tiers -> addItem_move (tier.move());
		}
		Melder_assert (my tiers->size > 0);

		autoMelderString label;
		/* mutable step */ double startingTime = 0.0, endTime = 1e-6;
		for (;;) {
			/* mutable scan */ conststring32 line = MelderReadText_readLine (text.get());
			if (! line)
				break;
			if (line [0] == U'\0')
				continue;   // moderately normal stray empty line
			if (line [0] == U';') {
				/*
					This is a continuation from the previous line; it occurs in Buckeye/s35/s3504a.words.
					The interpretation is that this is the last tier.
				*/
				line ++;   // step over semicolon
				Melder_skipHorizontalSpace (& line);
				Function lastTier = my tiers->at [my tiers->size];
				if (tiersArePointTiers) {
					TextTier tier = static_cast <TextTier> (lastTier);
					Melder_require (tier -> points.size > 0,
						U"Stray semicolon in line ", MelderReadText_getLineNumber (text.get()), U".");
					TextPoint point = tier -> points.at [tier -> points.size];
					TextPoint_setText (point, line);
				} else {
					IntervalTier tier = static_cast <IntervalTier> (lastTier);
					if (tier -> intervals.size > 0) {
						TextInterval interval = tier -> intervals.at [tier -> intervals.size];
						TextInterval_setText (interval, line);
					}
				}
				/*
					The quirk in Buckeye/s35/s3504a.words also means that the second and third tiers haven't been filled.
					Repair this very specific situation, after checking that it indeed occurs.
				*/
				if (my tiers->size == 4) {
					auto getAbbreviation = [] (conststring32 fullText) -> conststring32 {
						if (Melder_equ (fullText, U"<VOCNOISE>"))
							return U"U";
						if (Melder_startsWith (fullText, U"<SIL"))
							return U"S";
						if (Melder_startsWith (fullText, U"<IVER"))
							return U"S";
						if (Melder_startsWith (fullText, U"<IVER"))
							return U"S";
						if (Melder_equ (fullText, U"{B_TRANS}"))
							return U"B";
						if (Melder_equ (fullText, U"{E_TRANS}"))
							return U"E";
						if (Melder_startsWith (fullText, U"<UNKNOWN"))
							return U"U";
						if (Melder_startsWith (fullText, U"<CUTOFF"))
							return U"U";
						if (Melder_startsWith (fullText, U"<LAUGH"))
							return U"U";
						if (Melder_startsWith (fullText, U"<EXT"))
							return U"U";
						if (Melder_startsWith (fullText, U"<NOISE"))
							return U"U";
						if (Melder_startsWith (fullText, U"<ERROR"))
							return U"U";
						if (Melder_startsWith (fullText, U"<HES"))
							return U"U";
						return U"";
					};
					/*
						Get the texts from the first three tiers.
					*/
					Function firstAnyTier  = my tiers->at [1];
					Function secondAnyTier = my tiers->at [2];
					Function thirdAnyTier  = my tiers->at [3];
					if (tiersArePointTiers) {
						TextTier firstTier  = static_cast <TextTier> (firstAnyTier);
						TextTier secondTier = static_cast <TextTier> (secondAnyTier);
						TextTier thirdTier  = static_cast <TextTier> (thirdAnyTier);
						if (firstTier->points.size > 0 && secondTier->points.size > 0 && thirdTier->points.size > 0) {
							TextTier tier = static_cast <TextTier> (lastTier);
							TextPoint point = tier -> points.at [tier -> points.size];
							const double localTime = point -> number;
							TextPoint firstPoint  = firstTier  -> points.at [firstTier->points.size];
							TextPoint secondPoint = secondTier -> points.at [secondTier->points.size];
							TextPoint thirdPoint  = thirdTier  -> points.at [thirdTier->points.size];
							if (
								firstPoint  -> number == localTime &&
								secondPoint -> number == localTime &&
								thirdPoint  -> number == localTime &&
								! Melder_equ (firstPoint -> mark.get(), U"") &&   // The first tier should have a text,
								Melder_equ (secondPoint -> mark.get(), U"") &&   // but the second tier should be empty
								Melder_equ (thirdPoint  -> mark.get(), U"")   // and the third tier should also be empty.
							) {
								conststring32 abbreviation = getAbbreviation (firstPoint -> mark.get());
								TextPoint_setText (secondPoint, abbreviation);
								TextPoint_setText (thirdPoint,  abbreviation);
							}
						}
					} else {
						IntervalTier firstTier  = static_cast <IntervalTier> (firstAnyTier);
						IntervalTier secondTier = static_cast <IntervalTier> (secondAnyTier);
						IntervalTier thirdTier  = static_cast <IntervalTier> (thirdAnyTier);
						if (firstTier->intervals.size > 0 && secondTier->intervals.size > 0 && thirdTier->intervals.size > 0) {
							IntervalTier tier = static_cast <IntervalTier> (lastTier);
							TextInterval interval = tier -> intervals.at [tier -> intervals.size];
							const double localTmin = interval -> xmin;
							const double localTmax = interval -> xmax;
							TextInterval firstInterval  = firstTier  -> intervals.at [firstTier->intervals.size];
							TextInterval secondInterval = secondTier -> intervals.at [secondTier->intervals.size];
							TextInterval thirdInterval  = thirdTier  -> intervals.at [thirdTier->intervals.size];
							if (
								firstInterval  -> xmin == localTmin &&
								secondInterval -> xmin == localTmin &&
								thirdInterval  -> xmin == localTmin &&
								firstInterval  -> xmax == localTmax &&
								secondInterval -> xmax == localTmax &&
								thirdInterval  -> xmax == localTmax &&
								! Melder_equ (firstInterval -> text.get(), U"") &&   // The first tier should have a text,
								Melder_equ (secondInterval -> text.get(), U"") &&   // but the second tier should be empty
								Melder_equ (thirdInterval  -> text.get(), U"")   // and the third tier should also be empty.
							) {
								conststring32 abbreviation = getAbbreviation (firstInterval -> text.get());
								TextInterval_setText (secondInterval, abbreviation);
								TextInterval_setText (thirdInterval,  abbreviation);
							}
						}
					}
				}
				continue;
			}
			Melder_skipHorizontalSpace (& line);
			endTime = Melder_readReal (& line);
			if (endTime < 0.0) {
				Melder_casual (U"A negative time in file ", file, U" in line ", MelderReadText_getLineNumber (text.get()), U".");
				continue;   // don't insert any interval
			}
			if (endTime < startingTime) {
				/*
					This could be the error in Buckeye/s28/s2801a/s2301a.words.
				*/
				if (my tiers->size == 4 && fabs (endTime - 421.986875) < 1e-9 && fabs (startingTime - 431.847408) < 1e-9)
					endTime += 10.0;
			}
			{// scope
				const char32 shouldBeHorizontalSpace = * line ++;
				Melder_require (Melder_isHorizontalSpace (shouldBeHorizontalSpace),
					U"There should be a space after the time in line ", MelderReadText_getLineNumber (text.get()), U".");
			}
			Melder_skipHorizontalSpace (& line);
			const integer colour = Melder_readInteger (& line);
			(void) colour;   // ignore
			{// scope
				const char32 shouldBeHorizontalSpace = * line ++;
				Melder_require (Melder_isHorizontalSpace (shouldBeHorizontalSpace),
					U"There should be a space after the colour number in line ", MelderReadText_getLineNumber (text.get()), U".");
			}
			for (integer itier = 1; itier <= numberOfTiers; itier ++) {
				Melder_skipHorizontalSpace (& line);
				MelderString_empty (& label);
				for (;;) {
					char32 kar = * line;
					if (kar == U'\0')
						break;
					if (kar == separator) {
						line ++;   // consume separator
						break;
					}
					MelderString_appendCharacter (& label, kar);
					line ++;   // consume kar
				}
				while (label.length > 0 && Melder_isHorizontalSpace (label.string [label.length - 1]))   // FIXME should encapsulate
					label.string [-- label.length] = U'\0';   // remove the space before ";" in e.g. "ih ; *"
				if (tiersArePointTiers) {
					TextTier tier = static_cast <TextTier> (my tiers->at [itier]);
					TextTier_addPoint (tier, endTime, label.string);
				} else {
					IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
					if (startingTime == endTime) {
						/*
							A zero-duration interval: combine with the previous interval.
						*/
						if (tier -> intervals.size > 1) {
							const TextInterval previousInterval = tier -> intervals.at [tier -> intervals.size];
							TextInterval_setText (previousInterval, Melder_cat (previousInterval -> text.get(), U"//", label.string));
						} else
							Melder_casual (U"A zero-duration interval at the start of file ", file, U" in tier ", itier,
									U", containing the text “", label.string, U"”.");
					} else {
						(void) IntervalTier_addInterval_raw (tier, startingTime, endTime, label.string);
					}
				}
			}
			startingTime = endTime;
		}
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			Function tier = my tiers->at [itier];
			tier -> xmax = endTime;
			if (! tiersArePointTiers)
				IntervalTier_haveAtLeastOneInterval (static_cast <IntervalTier> (tier));
		}
		my xmax = endTime;

		TextGrid_checkInvariants_e (me.get(), true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from file ", file, U".");
	}
}

void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "w");
		fprintf (f, "separator ;\nnfields 1\n#\n");
		for (integer iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
			TextInterval interval = my intervals.at [iinterval];
			fprintf (f, "\t%.6f 26\t%s\n", interval -> xmax, Melder_peek32to8 (interval -> text.get()));
		}
		f.close (file);
	} catch (MelderError) {
		Melder_throw (me, U": not written to Xwaves file ", file, U".");
	}
}

static void sgmlToPraat (char *text) {
	char *sgml = text, *praat = text;
	for (;;) {
		if (*sgml == '\0')
			break;
		if (*sgml == '&') {
			static struct { const char *sgml, *praat; } translations [] = {
				{ "auml", "\\a\"" }, { "euml", "\\e\"" }, { "iuml", "\\i\"" },
				{ "ouml", "\\o\"" }, { "ouml", "\\o\"" },
				{ "Auml", "\\A\"" }, { "Euml", "\\E\"" }, { "Iuml", "\\I\"" },
				{ "Ouml", "\\O\"" }, { "Uuml", "\\U\"" },
				{ "aacute", "\\a'" }, { "eacute", "\\e'" }, { "iacute", "\\i'" },
				{ "oacute", "\\o'" }, { "oacute", "\\o'" },
				{ "Aacute", "\\A'" }, { "Eacute", "\\E'" }, { "Iacute", "\\I'" },
				{ "Oacute", "\\O'" }, { "Uacute", "\\U'" },
				{ "agrave", "\\a`" }, { "egrave", "\\e`" }, { "igrave", "\\i`" },
				{ "ograve", "\\o`" }, { "ograve", "\\o`" },
				{ "Agrave", "\\A`" }, { "Egrave", "\\E`" }, { "Igrave", "\\I`" },
				{ "Ograve", "\\O`" }, { "Ugrave", "\\U`" },
				{ "acirc", "\\a^" }, { "ecirc", "\\e^" }, { "icirc", "\\i^" },
				{ "ocirc", "\\o^" }, { "ocirc", "\\o^" },
				{ "Acirc", "\\A^" }, { "Ecirc", "\\E^" }, { "Icirc", "\\I^" },
				{ "Ocirc", "\\O^" }, { "Ucirc", "\\U^" },
				{ nullptr, nullptr } };
			char sgmlCode [201];
			int i = 0;
			++ sgml;
			for (i = 0; i < 200; i ++) {
				char sgmlChar = sgml [i];
				if (sgmlChar == ';') {
					if (i == 0)
						Melder_throw (U"Empty SGML code.");
					sgml += i + 1;
					break;
				}
				sgmlCode [i] = sgmlChar;
			}
			if (i >= 200)
				Melder_throw (U"Unfinished SGML code.");
			sgmlCode [i] = '\0';
			for (i = 0; translations [i]. sgml; i ++) {
				if (strequ (sgmlCode, translations [i]. sgml)) {
					memcpy (praat, translations [i]. praat, strlen (translations [i]. praat));
					praat += strlen (translations [i]. praat);
					break;
				}
			}
			if (! translations [i]. sgml)
				Melder_throw (U"Unknown SGML code &", Melder_peek8to32_u (sgmlCode), U";.");
		} else {
			* praat ++ = * sgml ++;
		}
	}
	*praat = '\0';
}

autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file) {
	try {
		int formatVersion = 0;
		autoMelderReadText text = MelderReadText_createFromFile (file);
		autostring32 tag = texgetw16 (text.get());
		if (! str32equ (tag.get(), U"Praat chronological TextGrid text file"))
			Melder_throw (U"This is not a chronological TextGrid text file.");
		autoTextGrid me = Thing_new (TextGrid);
		my structFunction :: v1_readText (text.get(), formatVersion);
		my tiers = FunctionList_create ();
		integer numberOfTiers = texgeti32 (text.get());
		for (integer itier = 1; itier <= numberOfTiers; itier ++) {
			autostring32 klas = texgetw16 (text.get());
			if (str32equ (klas.get(), U"IntervalTier")) {
				autoIntervalTier tier = Thing_new (IntervalTier);
				tier -> name = texgetw16 (text.get());
				tier -> structFunction :: v1_readText (text.get(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else if (str32equ (klas.get(), U"TextTier")) {
				autoTextTier tier = Thing_new (TextTier);
				tier -> name = texgetw16 (text.get());
				tier -> structFunction :: v1_readText (text.get(), formatVersion);
				my tiers -> addItem_move (tier.move());
			} else {
				Melder_throw (U"Unknown tier class \"", klas.get(), U"\".");
			}
		}
		for (;;) {
			integer tierNumber;
			try {
				tierNumber = texgeti32 (text.get());
			} catch (MelderError) {
				if (str32str (Melder_getError (), U"Early end of text")) {
					Melder_clearError ();
					break;
				} else {
					throw;
				}
			}
			Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (me.get(), tierNumber);
			if (anyTier -> classInfo == classIntervalTier) {
				IntervalTier tier = static_cast <IntervalTier> (anyTier);
				autoTextInterval interval = Thing_new (TextInterval);
				interval -> v1_readText (text.get(), formatVersion);
				tier -> intervals. addItem_move (interval.move());   // not earlier: sorting depends on contents of interval
			} else {
				TextTier tier = static_cast <TextTier> (anyTier);
				autoTextPoint point = Thing_new (TextPoint);
				point -> v1_readText (text.get(), formatVersion);
				tier -> points. addItem_move (point.move());   // not earlier: sorting depends on contents of point
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from chronological text file ", file, U".");
	}
}

static void writeQuotedString (MelderFile file, conststring32 string) {
	MelderFile_writeCharacter (file, U'\"');
	if (string) {
		char32 kar;
		while ((kar = *string ++) != U'\0') {
			MelderFile_writeCharacter (file, kar);
			if (kar == '\"')
				MelderFile_writeCharacter (file, kar);
		}
	}   // BUG
	MelderFile_writeCharacter (file, U'\"');
}

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file) {
	try {
		Data_createTextFile (me, file, false);
		autoMelderFile mfile = file;
		/*
			The "elements" (intervals and points) are sorted primarily by time and secondarily by tier.
		*/
		double sortingTime = -1e308;
		integer sortingTier = 0;
		file -> verbose = false;
		texindent (file);
		MelderFile_write (file, U"\"Praat chronological TextGrid text file\"\n", my xmin, U" ", my xmax,
			U"   ! Time domain.\n", my tiers->size, U"   ! Number of tiers.");
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			Function anyTier = my tiers->at [itier];
			MelderFile_write (file, U"\n");
			writeQuotedString (file, Thing_className (anyTier));
			MelderFile_write (file, U" ");
			writeQuotedString (file, anyTier -> name.get());
			MelderFile_write (file, U" ", anyTier -> xmin, U" ", anyTier -> xmax);
		}
		for (;;) {
			double firstRemainingTime = +1e308;
			integer firstRemainingTier = 2000000000, firstRemainingElement = 0;
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function anyTier = my tiers->at [itier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					for (integer ielement = 1; ielement <= tier -> intervals.size; ielement ++) {
						TextInterval interval = tier -> intervals.at [ielement];
						if ((interval -> xmin > sortingTime ||   // sort primarily by time
							 (interval -> xmin == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(interval -> xmin < firstRemainingTime ||   // sort primarily by time
							 (interval -> xmin == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = interval -> xmin;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				} else {
					TextTier tier = static_cast <TextTier> (anyTier);
					for (integer ielement = 1; ielement <= tier -> points.size; ielement ++) {
						TextPoint point = tier -> points.at [ielement];
						if ((point -> number > sortingTime ||   // sort primarily by time
							 (point -> number == sortingTime && itier > sortingTier)) &&   // sort secondarily by tier number
							(point -> number < firstRemainingTime ||   // sort primarily by time
							 (point -> number == firstRemainingTime && itier < firstRemainingTier)))   // sort secondarily by tier number
						{
							firstRemainingTime = point -> number;
							firstRemainingTier = itier;
							firstRemainingElement = ielement;
						}
					}
				}
			}
			if (firstRemainingElement == 0) {
				break;
			} else {
				Function anyTier = my tiers->at [firstRemainingTier];
				if (anyTier -> classInfo == classIntervalTier) {
					IntervalTier tier = static_cast <IntervalTier> (anyTier);
					TextInterval interval = tier -> intervals.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name.get(), U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", interval -> xmin, U" ", interval -> xmax);
					texputw32 (file, interval -> text.get(), U"", 0,0,0,0,0);
				} else {
					TextTier tier = static_cast <TextTier> (anyTier);
					TextPoint point = tier -> points.at [firstRemainingElement];
					if (tier -> name) MelderFile_write (file, U"\n\n! ", tier -> name.get(), U":");
					MelderFile_write (file, U"\n", firstRemainingTier, U" ", point -> number, U" ");
					texputw32 (file, point -> mark.get(), U"", 0,0,0,0,0);
				}
				sortingTime = firstRemainingTime;
				sortingTier = firstRemainingTier;
			}
		}
		texexdent (file);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (me, U": not written to chronological text file ", file, U".");
	}
}

autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file) {
	try {
		autoTextGrid me = Thing_new (TextGrid);
		integer sentenceNumber = 0;
		double phraseBegin = 0.0, phraseEnd = 0.0;
		IntervalTier sentenceTier = nullptr, phraseTier = nullptr;
		TextInterval lastInterval = nullptr;
		static char phrase [1000];
		my tiers = FunctionList_create ();
		autoMelderFile mfile = MelderFile_open (file);
		char *const line1 = MelderFile_readLine8 (file);
		Melder_require (strequ (line1, "<?xml version=\"1.0\"?>"),
			U"This is not a CGN syntax file.");
		char *const line2 = MelderFile_readLine8 (file);
		Melder_require (strequ (line2, "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">"),
			U"This is not a CGN syntax file.");
		(void) MelderFile_readLine8 (file);   // ignore third line
		const integer startOfData = MelderFile_tell (file);
		/*
			Get duration.
		*/
		my xmin = 0.0;
		char arg1 [41], arg2 [41], arg3 [41], arg4 [41], arg5 [41], arg6 [41], arg7 [201];
		for (;;) {
			char *const line = MelderFile_readLine8 (file);
			if (! line)
				break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tau line.");
				my xmax = atof (arg5 + 4);
			}
		}
		Melder_require (my xmax > 0.0,
			U"Duration (", my xmax, U" seconds) should be greater than zero.");
		/*
			Get number and names of tiers.
		*/
		MelderFile_seek (file, startOfData, SEEK_SET);
		for (;;) {
			char *const line = MelderFile_readLine8 (file);
			if (! line)
				break;
			if (strnequ (line, "  <tau ref=\"", 12)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tau line.");
				const integer length_s = Melder8_length (arg3);
				Melder_require (length_s >= 5 && strnequ (arg3, "s=\"", 3),
					U"Missing speaker name.");
				arg3 [length_s - 1] = '\0';   // truncate at double quote
				char *const speakerName = arg3 + 3;   // truncate leading s="
				/*
					Does this speaker name occur in the tiers?
				*/
				/* mutable search */ integer speakerTier = 0;
				for (integer itier = 1; itier <= my tiers->size; itier ++) {
					IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
					if (str32equ (tier -> name.get(), Melder_peek8to32_u (speakerName))) {
						speakerTier = itier;
						break;
					}
				}
				if (speakerTier == 0) {
					/*
						Create two new tiers.
					*/
					autoIntervalTier newSentenceTier = Thing_new (IntervalTier);
					newSentenceTier -> xmin = 0.0;
					newSentenceTier -> xmax = my xmax;
					Thing_setName (newSentenceTier.get(), Melder_peek8to32_u (speakerName));
					sentenceTier = (IntervalTier) my tiers -> addItem_move (newSentenceTier.move());
					autoIntervalTier newPhraseTier = Thing_new (IntervalTier);
					newPhraseTier -> xmin = 0.0;
					newPhraseTier -> xmax = my xmax;
					Thing_setName (newPhraseTier.get(), Melder_peek8to32_u (speakerName));
					phraseTier = (IntervalTier) my tiers -> addItem_move (newPhraseTier.move());
				} else {
					sentenceTier = (IntervalTier) my tiers->at [speakerTier];
					phraseTier = (IntervalTier) my tiers->at [speakerTier + 1];
				}
				const double tb = atof (arg4 + 4), te = atof (arg5 + 4);
				Melder_require (te > tb,
					U"Zero duration for sentence.");
				/*
					We are going to add one or two intervals to the sentence tier.
				*/
				if (sentenceTier -> intervals.size > 0) {
					TextInterval latestInterval = sentenceTier -> intervals.at [sentenceTier -> intervals.size];
					if (tb > latestInterval -> xmax) {
						autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
						sentenceTier -> intervals. addItem_move (interval.move());
					} else if (tb < latestInterval -> xmax) {
						Melder_throw (U"Overlap on tier not allowed.");
					}
				} else {
					if (tb > 0.0) {
						autoTextInterval interval = TextInterval_create (0.0, tb, U"");
						sentenceTier -> intervals. addItem_move (interval.move());
					} else if (tb < 0.0) {
						Melder_throw (U"Negative times not allowed.");
					}
				}
				autoTextInterval interval = TextInterval_create (tb, te, Melder_integer (++ sentenceNumber));
				sentenceTier -> intervals. addItem_move (interval.move());
			} else if (strnequ (line, "    <tw ref=\"", 13)) {
				Melder_require (sscanf (line, "%40s%40s%40s%40s%40s%40s%200s", arg1, arg2, arg3, arg4, arg5, arg6, arg7) == 7,
					U"Too few strings in tw line.");
				const integer length_tb = Melder8_length (arg3);
				Melder_require (length_tb >= 6 && strnequ (arg3, "tb=\"", 4),
					U"Missing tb.");
				const double tb = atof (arg3 + 4);
				const integer length_te = Melder8_length (arg4);
				Melder_require (length_te >= 6 && strnequ (arg4, "te=\"", 4),
					U"Missing te.");
				const double te = atof (arg4 + 4);
				Melder_require (te > tb,
					U"Zero duration for phrase.");
				if (tb == phraseBegin && te == phraseEnd) {
					/*
						Append a word.
					*/
					strcat (phrase, " ");
					const integer length_w = Melder8_length (arg7);
					Melder_require (length_w >= 6 && strnequ (arg7, "w=\"", 3),   // BUG? no words of length 1 allowed
						U"Missing word.");
					arg7 [length_w - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
				} else {
					/*
						Begin a phrase.
					*/
					if (lastInterval) {
						sgmlToPraat (phrase);
						TextInterval_setText (lastInterval, Melder_peek8to32_u (phrase));
					}
					phrase [0] = '\0';
					const integer length_w = Melder8_length (arg7);
					Melder_require (length_w >= 6 && strnequ (arg7, "w=\"", 3),   // BUG? no words of length 1 allowed
						U"Missing word.");
					arg7 [length_w - 3] = '\0';   // truncate "/>
					strcat (phrase, arg7 + 3);
					if (phraseTier -> intervals.size > 0) {
						TextInterval latestInterval = phraseTier -> intervals.at [phraseTier -> intervals.size];
						if (tb > latestInterval -> xmax) {
							autoTextInterval interval = TextInterval_create (latestInterval -> xmax, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < latestInterval -> xmax) {
							Melder_throw (U"Overlap on tier not allowed.");
						}
					} else {
						if (tb > 0.0) {
							autoTextInterval interval = TextInterval_create (0.0, tb, U"");
							phraseTier -> intervals. addItem_move (interval.move());
						} else if (tb < 0.0) {
							Melder_throw (U"Negative times not allowed.");
						}
					}
					if (! phraseTier)
						Melder_throw (U"Phrase outside sentence.");
					autoTextInterval newLastInterval = TextInterval_create (tb, te, U"");
					lastInterval = newLastInterval.get();
					phraseTier -> intervals. addItem_move (newLastInterval.move());
					phraseBegin = tb;
					phraseEnd = te;
				}
			}
		}
		if (lastInterval) {
			sgmlToPraat (phrase);
			TextInterval_setText (lastInterval, Melder_peek8to32_u (phrase));
		}
		for (integer itier = 1; itier <= my tiers->size; itier ++) {
			IntervalTier tier = static_cast <IntervalTier> (my tiers->at [itier]);
			if (tier -> intervals.size > 0) {
				TextInterval latestInterval = tier -> intervals.at [tier -> intervals.size];
				if (my xmax > latestInterval -> xmax) {
					autoTextInterval interval = TextInterval_create (latestInterval -> xmax, my xmax, U"");
					tier -> intervals. addItem_move (interval.move());
				}
			} else {
				autoTextInterval interval = TextInterval_create (my xmin, my xmax, U"");
				tier -> intervals. addItem_move (interval.move());
			}
		}
		mfile.close ();
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from CGN syntax file ", file, U".");
	}
}

const struct TIMIT_key {
	const char *timitLabel, *ipaLabel;
} TIMIT_toIpaTable [] = {
	{"", ""},
	/* Vowels */
	{"iy", "i"},			/* beet: bcl b IY tcl t */
	{"ih", "\\ic"}, 		/* bit: bcl b IH tcl t */
	/* 20190704 wgmichener "\\ep" -> "\\ef" */
	{"eh", "\\ef"}, 		/* bet: bcl b EH tcl t */
	{"ey", "e"},  			/* bait: bcl b EY tcl t */
	{"ae", "\\ae"},  		/* bat: bcl b AE tcl t */
	{"aa", "\\as"}, 		/* bott: bcl b AA tcl t */
	{"aw", "a\\hs"},  		/* bout: bcl b AW tcl t */
	{"ay", "a\\ic"},  		/* bite: bcl b AY tcl t */
	{"ah", "\\vt"}, 		/* but: bcl b AH tcl t */
	{"ao", "\\ct"},  		/* bought: bcl b AO tcl t */
	{"oy", "\\ct\\ic"},		/* boy: bcl b OY */
	{"ow", "o"}, 			/* boat: bcl b OW tcl t */
	{"uh", "\\hs"}, 		/* book: bcl b UH tcl t */
	{"uw", "u"},  			/* boot: bcl b UW tcl t */
	/* fronted allophone of uw (alveolar contexts) */
	/* 20190704 wgmichener "\\u\"" -> "\\u\\:^" */
	/* 20250422 ppgb       "\\u\\:^" -> "u\\:^"  (as in Zue & Seneff 1988: "Transcription and alignment of the Timit database") */
	/* 20250426 ppgb       "u\\:^" -> "\\u-"     (turn Americanist into IPA (could also have been [y]) */
	{"ux", "\\u-"}, 		/* toot: tcl t UX tcl t */
	{"er", "\\er\\hr"},		/* bird: bcl b ER dcl d */
	{"ax", "\\sw"}, 		/* about: AX bcl b aw tcl t */
	{"ix", "\\i-"}, 		/* debit: dcl d eh bcl b IX tcl t */
	{"axr", "\\sr"}, 		/* butter: bcl ah dx AXR */
	/* devoiced schwa, very short */
	/* 20190704 wgmichener "\\sw\\ov"" -> "\\sw\\0v" */
	{"ax-h", "\\sw\\0v"}, 	/* suspect: s AX-H s pcl p eh kcl k tcl t */
	/* Semivowels and glides */
	{"l", "l"},				/* lay:	L ey */
	/* 20190704 wgmichener "r" -> "\\rt" */
	{"r", "\\rt"},			/* ray:	R ey */
	{"w", "w"},				/* way:	w ey */
	{"y", "j"},				/* yacht: Y aa tcl t */
	{"hh", "h" },		/* hay: HH ey*/
	/* voiced allophone of h */
	/* 20190704 wgmichener "\\hv" -> "\\h^" */
	{"hv", "\\h^"},			/* ahead: ax HV eh dcl d */
	{"el", "l\\|v"},		/* bottle: bcl b aa tcl t EL */
	/* Nasals */
	{"m", "m"},				/* mom:	M aa M */
	{"n", "n"},				/* noon: N uw N*/
	{"ng", "\\ng"},			/* sing: s ih NG */
	{"em", "m\\|v"},		/* bottom: b aa tcl t EM */
	{"en", "n\\|v"},		/* button:	b ah q EN */
	{"eng", "\\ng\\|v"},	/* washington: w aa sh ENG tcl t ax n */
	/* nasal flap */
	/* 20190704 wgmichener "n^\\fh" -> "\\fh\\~^" */
	{"nx", "\\fh\\~^"},		/* winner: wih NX axr */
	/* Fricatives */
	{"s", "s"},				/* sea: S iy */
	{"sh", "\\sh"},			/* she: SH iy */
	{"z", "z"},				/* zone: Z ow n */
	{"zh", "\\zh"},			/* azure: ae ZH er */
	{"f", "f"},				/* fin: F ih n */
	/* 20190704 wgmichener "\\te" -> "\\tf" */
	{"th", "\\tf"},			/* thin: TH ih n */
	{"v", "v"},				/* van: v ae n */
	{"dh", "\\dh"},			/* then: DH en */
	/* Affricates */
	{"jh", "d\\zh"},		/* joke: DCL JH ow kcl k */
	{"ch", "t\\sh"},		/* choke TCL CH ow kcl k */
	/* Stops */
	{"b", "b"},				/* bee: BCL B iy */
	{"d", "d"},				/* day: DCL D ey */
	{"g", "g"},				/* gay: GCL G ey */
	{"p", "p"},				/* pea: PCL P iy */
	{"t", "t"},				/* tea: TCL T iy */
	{"k", "k"},				/* key: KCL K iy */
	/* 20140315: Added silences before the burst */
	/* 20190704 wgmichener "" -> "x\\cn" */
	{"bcl", "b\\cn"},
	{"dcl", "d\\cn"},
	{"gcl", "g\\cn"},
	{"pcl", "p\\cn"},
	{"tcl", "t\\cn"},
	{"kcl", "k\\cn"},
	/* flap */
	{"dx", "\\fh"},			/* muddy: m ah DX iy & dirty: dcl d er DX iy */
	/* glottal stop */
	/* 20190704 wgmichener "?" -> "\\?g" */
	{"q", "\\?g"},
	/* Others */
	{"pau", "(pau)"},	/* pause */
	{"epi", "(epi)"},	/* epenthetic silence */
	{"h#", "(h#)"}, 	/* marks start and end piece of sentence */
	/* the following markers occur only in the dictionary */
	/* 20190704 wgmichener "1" -> "\\'1" */
	{"1", "\\'1"},		/* primary stress marker */
	/* 20190704 wgmichener "2" -> "\\'2" */
	{"2", "\\'2"}		/* secondary stress marker */
};

constexpr integer TIMIT_NLABELS = ((integer) sizeof TIMIT_toIpaTable / (integer) sizeof TIMIT_toIpaTable [1] - 1);
static const char *TIMIT_DELIMITER = "h#";

static const char *timitLabelToIpaLabel (const char timitLabel []) {
	for (integer i = 1; i <= TIMIT_NLABELS; i ++)
		if (! strcmp (TIMIT_toIpaTable [i].timitLabel, timitLabel))
			return TIMIT_toIpaTable [i].ipaLabel;
	return timitLabel;
}

static bool isTimitPhoneticLabel (const char label []) {
	for (integer i = 1; i <= TIMIT_NLABELS; i ++)
		if (! strcmp (TIMIT_toIpaTable [i].timitLabel, label))
			return true;
	return false;
}

static bool isTimitWord (const char label []) {
	const char *p = label;
	for (; *p; p++)
		if (Melder_isUpperCaseLetter (*p))
			return false;
	return true;
}

autoDaata TextGrid_TIMITLabelFileRecognizer (integer nread, const char *header, MelderFile file) {
	char hkruis [3] = "h#", label1 [512], label2 [512];
	int length;
	bool phnFile = false;
	integer it [5];
	if (nread < 12 || sscanf (header, "%td%td%511s%n\n", & it [1], & it [2], label1, & length) != 3 ||
		it [1] < 0 || it [2] <= it [1] || sscanf (& header [length], "%td%td%511s\n", & it [3], & it [4], label2) != 3 ||
		it [4] <= it [3]) {
		/*
			20120512 djmw removed the extra "it [3] < it [2]" check, because otherwise train/dr7/mdlm0/si1864.wrd cannot be read
		*/
		return autoDaata ();
	}
	if (! strcmp (label1, hkruis)) {
		if (isTimitPhoneticLabel (label2))
			phnFile = true;
		else if (! isTimitWord (label2))
			return autoDaata ();
	} else if (! isTimitWord (label1) || ! isTimitWord (label2)) {
		return autoDaata ();
	}
	autoTextGrid thee = TextGrid_readFromTimitLabelFile (file, phnFile);
	return thee.move();
}

autoIntervalTier IntervalTier_readFromTimitLabelFile (
	const MelderFile file,
	const bool hasPhones
) {
	try {
		autoMelderReadText text = MelderReadText_createFromFile (file);   // going to be UTF-8-compatible
		constexpr double fixedTimitSamplingFrequency = 16000.0;   // Hz
		constexpr double fixedSamplingPeriod = 1.0 / fixedTimitSamplingFrequency;
		autoMelderString label, swallowedWord;
		autoIntervalTier me = IntervalTier_create_raw (+1e308, -1e308);
		Melder_assert (my intervals.size == 0);

		/* mutable step */ integer previousStartingSample = -2, previousEndSample = -1;
		for (;;) {
			/* mutable scan */ conststring32 line = MelderReadText_readLine (text.get());
			if (! line)
				break;

			/*
				Read and check starting sample.
			*/
			const integer startingSample = Melder_readInteger (& line);
			if (my intervals.size == 0 && startingSample != 0) {
				Melder_require (startingSample > 0,
					U"The annotation should not start at a negative sample number.");
				Melder_assert (startingSample > 0);
				(void) IntervalTier_addInterval_raw (me.get(), 0.0, startingSample * fixedSamplingPeriod, U"");
				previousStartingSample = 0;
				previousEndSample = startingSample;
			}
			Melder_require (startingSample >= previousStartingSample,
				U"The starting sample in line ", MelderReadText_getLineNumber (text.get()),
				U" should be at least the starting sample of the previous line, but ",
				startingSample, U" is less than ", previousStartingSample, U"."
			);
			{// scope
				const char32 shouldBeHorizontalSpace = * line ++;
				Melder_require (Melder_isHorizontalSpace (shouldBeHorizontalSpace),
					U"There should be a space after the starting sample in line ", MelderReadText_getLineNumber (text.get()), U".");
			}

			/*
				Read and check end sample.
			*/
			Melder_skipHorizontalSpace (& line);
			const integer endSample = Melder_readInteger (& line);
			Melder_require (endSample >= startingSample,
				U"End sample should be at least starting sample in line ", MelderReadText_getLineNumber (text.get()),
				U" but ", endSample, U" is less than ", startingSample, U"."
			);
			Melder_require (endSample >= previousEndSample,
				U"The end sample in line ", MelderReadText_getLineNumber (text.get()),
				U" should be at least the end sample of the previous line, but ",
				endSample, U" is less than ", previousEndSample, U"."
			);
			{// scope
				const char32 shouldBeHorizontalSpace = * line ++;
				Melder_require (Melder_isHorizontalSpace (shouldBeHorizontalSpace),
					U"There should be a space after the end sample in line ", MelderReadText_getLineNumber (text.get()), U".");
			}

			/*
				Read and check label text.
			*/
			Melder_skipHorizontalSpace (& line);
			MelderString_copy (& label, line);
			if (endSample == startingSample) {
				if (hasPhones) {
					Melder_throw (U"Zero-duration phone found at line ", MelderReadText_getLineNumber (text.get()), U".");
				} else {
					/*
						For the following .wrd files this occurs once per file:
						train/dr3/makr0/si1982
						train/dr3/mhjb0/sa2
						train/dr5/mmcc0/sx348
						train/dr6/meal0/sa2
						train/dr6/mesj0/si2039
						train/dr7/fmkc0/sx352
						train/dr7/mrem0/sx61
						train/dr7/mvrw0/sx315
						test/dr6/mjfc0/sx138
					*/
					MelderString_copy (& swallowedWord, line);
					continue;
				}
			} else {
				/* mutable fix */ double startingTime = startingSample * fixedSamplingPeriod;
				const double endTime = endSample * fixedSamplingPeriod;
				if (my intervals.size > 0 && startingSample != previousEndSample) {
					/*
						Overlapping intervals.
					*/
					if (hasPhones) {
						/*
							Overlapping phones. Should not occur.
						*/
						Melder_throw (U"Overlapping phones around line ", MelderReadText_getLineNumber (text.get()),
								U": previous end sample ", previousEndSample, U", current starting sample ", startingSample, U".");
					} else if (startingSample < previousEndSample){
						/*
							Overlapping words. Put the boundary in the middle (the boundary will fall within a phone).
						*/
						const double previousEndTime = previousEndSample * fixedSamplingPeriod;
						startingTime = 0.5 * (startingTime + previousEndTime);
						TextInterval previousInterval = my intervals.at [my intervals.size];
						previousInterval -> xmax = startingTime;
						Melder_assert (previousInterval -> xmin < previousInterval -> xmax);
					} else {
						/*
							A gap between words. Fill with an empty interval.
						*/
						(void) IntervalTier_addInterval_raw (me.get(), previousEndSample * fixedSamplingPeriod, startingTime, U"");
					}
				}
				(void) IntervalTier_addInterval_raw (me.get(), startingTime, endTime,
						swallowedWord.length > 0 ? Melder_cat (swallowedWord.string, U"+", line) : line);
				MelderString_empty (& swallowedWord);
			}
			previousStartingSample = startingSample;
			previousEndSample = endSample;
		}
		Melder_require (my intervals.size >= 1,
			U"The file should contain at least one interval, but none were found.");
		return me;
	} catch (MelderError) {
		Melder_throw (U"Interval tier not read from file ", file, U".");
	}
}

autoTextGrid TextGrid_readFromTimitLabelFile (
	const MelderFile file,
	const bool hasPhones
) {
	try {
		autoTextGrid result = TextGrid_createWithoutTiers (+1e308, -1e308);
		autoIntervalTier tier = IntervalTier_readFromTimitLabelFile (file, hasPhones);
		if (hasPhones) {
			autoIntervalTier ipa = Data_copy (tier.get());
			for (integer iinterval = 1; iinterval <= ipa -> intervals.size; iinterval ++) {
				const mutableTextInterval interval = ipa -> intervals.at [iinterval];
				TextInterval_setText (ipa -> intervals.at [iinterval],
						Melder_peek8to32_u (timitLabelToIpaLabel (Melder_peek32to8 (interval -> text.get()))));
			}
			TextGrid_addTier_move (result.get(), ipa.move());
		}
		TextGrid_addTier_move (result.get(), tier.move());
		return result;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not read from file ", file, U".");
	}
}

/* End of file TextGrid_files.cpp */
