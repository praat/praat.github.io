#ifndef _TextGrid_h_
#define _TextGrid_h_
/* TextGrid.h
 *
 * Copyright (C) 1992-2012,2014-2018,2020,2021,2024,2025 Paul Boersma
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

#include "AnyTier.h"
#include "Label.h"
#include "Graphics.h"
#include "TableOfReal.h"
#include "Table.h"

#include "TextGrid_def.h"

autoTextPoint TextPoint_create (double time, conststring32 mark);

void TextPoint_setText (TextPoint me, conststring32 text);

autoTextInterval TextInterval_create (double tmin, double tmax, conststring32 text);

void TextInterval_setText (TextInterval me, conststring32 text);

autoTextTier TextTier_create (double tmin, double tmax);

void TextTier_addPoint (TextTier me, double time, conststring32 mark);
autoPointProcess TextTier_getPoints (TextTier me, conststring32 text);

autoIntervalTier IntervalTier_create_raw (double tmin, double tmax);

TextInterval /* reference */ IntervalTier_addInterval_raw (
	IntervalTier me,   // the tier to which the new interval shall be added
	double tmin,   // the starting time of the new interval
	double tmax,   // the end time of the new interval
	conststring32 text   // the label of the new interval
);
/*
	This attempts to add a new interval to an existing tier.
	Because the intervals in an IntervalTier form a sorted set,
	the attempt will fail if there is already an interval with the same starting interval.

	This function is called "raw" because no attempt is made to keep the IntervalTier
	in a valid state. Most notably, the caller will have to make sure
	that after a series of calls to this function
	the invariants of the IntervalTier (i.e. adjacent time ranges) will have to be restored.

	The return value is a reference to the new TextInterval,
	or null of the attempt failed.
*/

void IntervalTier_haveAtLeastOneInterval (IntervalTier me);

autoIntervalTier IntervalTier_create (double tmin, double tmax);

integer IntervalTier_timeToLowIndex (IntervalTier me, double t);
integer IntervalTier_timeToIndex (IntervalTier me, double t);   // obsolete
integer IntervalTier_timeToHighIndex (IntervalTier me, double t);
integer IntervalTier_hasTime (IntervalTier me, double t);
integer IntervalTier_hasBoundary (IntervalTier me, double t);
autoPointProcess IntervalTier_getStartingPoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_getEndPoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_getCentrePoints (IntervalTier me, conststring32 text);
autoPointProcess IntervalTier_PointProcess_startToCentre (IntervalTier tier, PointProcess point, double phase);
autoPointProcess IntervalTier_PointProcess_endToCentre (IntervalTier tier, PointProcess point, double phase);
void IntervalTier_removeLeftBoundary (IntervalTier me, integer intervalNumber);
void IntervalTier_moveLeftBoundary (IntervalTier me, integer interval, double newTime);
void IntervalTier_moveRightBoundary (IntervalTier me, integer interval, double newTime);

void TextTier_removePoint (TextTier me, integer pointNumber);

autoTextGrid TextGrid_createWithoutTiers (double tmin, double tmax);
autoTextGrid TextGrid_create (double tmin, double tmax, conststring32 tierNames, conststring32 pointTiers);

integer TextGrid_countLabels (TextGrid me, integer itier, conststring32 text);
integer TextGrid_countIntervalsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
integer TextGrid_countPointsWhere (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getStartingPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getEndPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getCentrePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getPoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);
autoPointProcess TextGrid_getPoints_preceded (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string precededBy, conststring32 criterion_precededBy);
autoPointProcess TextGrid_getPoints_followed (TextGrid me, integer tierNumber,
	kMelder_string which, conststring32 criterion,
	kMelder_string followedBy, conststring32 criterion_followedBy);

Function TextGrid_checkSpecifiedTierNumberWithinRange (TextGrid me, integer tierNumber);
IntervalTier TextGrid_checkSpecifiedTierIsIntervalTier (TextGrid me, integer tierNumber);
TextTier TextGrid_checkSpecifiedTierIsPointTier (TextGrid me, integer tierNumber);
void AnyTextGridTier_identifyClass (Function anyTextGridTier, IntervalTier *intervalTier, TextTier *textTier);

void TextGrid_addTier_copy (TextGrid me, Function tier);
void TextGrid_addTier_move (TextGrid me, autoFunction tier);

autoTextGrid TextGrids_merge (OrderedOf <structTextGrid>* textGrids, bool equalizeDomains);
/*
	Merge two or more textGrids.
	The new domain will run from
		min (textGrids[] -> xmin)
	to
		max (textGrids[] -> xmax)

	If `equalizeDomains`:
		- All tiers will have this same domain.
		- All interval tiers will have intervals that border on the edges of this domain;
		  this implies that a new empty interval can be added at the beginning and/or end of a tier.
*/

autoTextGrid TextGrid_extractPart (TextGrid me, double tmin, double tmax, bool preserveTimes);

autoTextGrid Label_to_TextGrid (Label me, double duration);
autoTextGrid Label_Function_to_TextGrid (Label me, Function function);

autoTextTier PointProcess_upto_TextTier (PointProcess me, conststring32 text);
autoTableOfReal IntervalTier_downto_TableOfReal (IntervalTier me, conststring32 label);
autoTableOfReal IntervalTier_downto_TableOfReal_any (IntervalTier me);
autoTableOfReal TextTier_downto_TableOfReal (TextTier me, conststring32 label);
autoTableOfReal TextTier_downto_TableOfReal_any (TextTier me);

autoTextGrid PointProcess_to_TextGrid_vuv (PointProcess me, double maxT, double meanT);

integer TextInterval_labelLength (TextInterval me);
integer TextPoint_labelLength (TextPoint me);
integer IntervalTier_maximumLabelLength (IntervalTier me);
integer TextTier_maximumLabelLength (TextTier me);
integer TextGrid_maximumLabelLength (TextGrid me);
void TextGrid_convertToBackslashTrigraphs (TextGrid me);
void TextGrid_convertToUnicode (TextGrid me);

void TextInterval_removeText (TextInterval me);
void TextPoint_removeText (TextPoint me);
void IntervalTier_removeText (IntervalTier me);
void TextTier_removeText (TextTier me);

void TextTier_removePoints (TextTier me, kMelder_string which, conststring32 criterion);
void TextGrid_removePoints (TextGrid me, integer tierNumber, kMelder_string which, conststring32 criterion);

void TextGrid_insertBoundary (TextGrid me, integer tierNumber, double t);
void TextGrid_removeBoundaryAtTime (TextGrid me, integer tierNumber, double t);
void TextGrid_setIntervalText (TextGrid me, integer tierNumber, integer intervalNumber, conststring32 text);
void TextGrid_insertPoint (TextGrid me, integer tierNumber, double t, conststring32 mark);
void TextGrid_setPointText (TextGrid me, integer tierNumber, integer pointNumber, conststring32 text);

autoTable TextGrid_downto_Table (TextGrid me, bool includeLineNumbers, integer timeDecimals, bool includeTierNames, bool includeEmptyIntervals);
autoTable TextGrid_tabulateOccurrences (TextGrid me, constVEC searchTiers,
	kMelder_string which, conststring32 criterion, bool caseSensitive);
void TextGrid_list (TextGrid me, bool includeLineNumbers, integer timeDecimals, bool includeTierNames,
	bool includeEmptyIntervals);

void TextGrid_correctRoundingErrors (TextGrid me);
autoTextGrid TextGrids_concatenate (OrderedOf<structTextGrid>* me);

/*
	Defined in TextGrid_files.cpp
*/

autoTextGrid TextGrid_readFromEspsLabelFile (
	MelderFile file,
	bool tiersArePointTiers,
	integer overrideNumberOfTiers   // 0 means don't override
);
void IntervalTier_writeToXwaves (IntervalTier me, MelderFile file);

void TextGrid_writeToChronologicalTextFile (TextGrid me, MelderFile file);
autoTextGrid TextGrid_readFromChronologicalTextFile (MelderFile file);
autoTextGrid TextGrid_readFromCgnSyntaxFile (MelderFile file);

autoIntervalTier IntervalTier_readFromTimitLabelFile (MelderFile file, bool hasPhones);
autoTextGrid TextGrid_readFromTimitLabelFile (MelderFile file, bool hasPhones);
/*
	Read TIMIT label file with the following structure:
		samplenumber1 samplenumber2 label1
		samplenumber3 samplenumber4 label2
		...
		samplenumber2n-1 samplenumber2n labeln

	The first tier of TextGrid will contain the TIMIT labels.
	If phnFile == true, the second tier will contain the translation of the
	TIMIT labels into IPA labels.
	For the translation from sample number to time a default sampling
	frequency of 16000 Hz is assumed.
*/

autoDaata TextGrid_TIMITLabelFileRecognizer (integer nread, const char *header, MelderFile file);
/*
	There are two types of TIMIT label files. One with phonetic labels, these
	files have '.phn' as file extension. The other contains word labels and has
	'.wrd' as extension. Since these extensions are only valid on the CDROM we can
	not use them for filetype recognition. Both TIMIT label files do not have a
	self-describing format. For filetype recognition we make use of the fact that
	both files are text files and always have three items on each line: two numbers
	followed by a string. The numbers increase in a monotone way.
	The recognizer only checks the first two lines and it tests whether
		0 <= number 1] < number [2] <= number [3] < number [4]
	(A number of .wrd files do not obey the monotonocity constraint for
	 number [4] and number [5] !)
	The decision whether it is a .phn or .wrd file is:
		.phn if string [1] == 'h#' AND string [2] is a TIMIT phonetic label
		.wrd if (string [1] == 'h#' AND string [2] is a valid word) OR
			string [1] and string [2] are both valid words.
		A valid word is a string with contains the lowercase characters [a-z] and ['].
*/

autoDaata TextGrid_KielLabelFileRecognizer (integer nread, const char *header, MelderFile file);
autoIntervalTier IntervalTier_readFromKielLabelFile (MelderFile file);
autoTextGrid TextGrid_readFromKielLabelFile (MelderFile file);

void TextGrid_checkInvariants_e (const constTextGrid me, const bool includeWeakInvariants);

void TextGrid_scaleTimes_e (mutableTextGrid me, double xminfrom, double xmaxfrom, double xminto, double xmaxto);

/* End of file TextGrid.h */
#endif
