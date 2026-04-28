/* EditDistanceTable.cpp
 *
 * Copyright (C) 2012-2020,2022 David Weenink, 2025,2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "EditDistanceTable.h"

#include "oo_DESTROY.h"
#include "EditDistanceTable_def.h"
#include "oo_COPY.h"
#include "EditDistanceTable_def.h"
#include "oo_EQUAL.h"
#include "EditDistanceTable_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EditDistanceTable_def.h"
#include "oo_WRITE_TEXT.h"
#include "EditDistanceTable_def.h"
#include "oo_WRITE_BINARY.h"
#include "EditDistanceTable_def.h"
#include "oo_READ_TEXT.h"
#include "EditDistanceTable_def.h"
#include "oo_READ_BINARY.h"
#include "EditDistanceTable_def.h"
#include "oo_DESCRIPTION.h"
#include "EditDistanceTable_def.h"

/*
	The insertion, deletion and substitution costs are specified in a TableOfReal
	1..n-2 target symbols/alphabet
	1..m-2 source symbols/alphabet
	row n-1 and col m-1 specify nomatch symbols
	cells [n] [1..m-2] specify insertion costs
	cells [1..n-1] [m] specify deletion costs
	cell [n-1] [m-1] if nomatch target == nomatch source
	cell [n] [m] if nomatch target != nomatch source
*/

Thing_implement (EditCostsTable, TableOfReal, 0);

void structEditCostsTable :: v1_info () {
	EditCostsTable_Parent :: v1_info ();
	MelderInfo_writeLine (U"Target:", numberOfRows - 2, U" symbols.");
	MelderInfo_writeLine (U"Source:", numberOfColumns - 2, U" symbols.");
}

bool structEditCostsTable :: v_matchTargetSymbol (conststring32 targetSymbol, conststring32 symbol) {
	return Melder_equ (targetSymbol, symbol);
}

bool structEditCostsTable :: v_matchSourceSymbol (conststring32 sourceSymbol, conststring32 symbol) {
	return Melder_equ (sourceSymbol, symbol);
}

bool structEditCostsTable :: v_matchTargetWithSourceSymbol (conststring32 targetSymbol, conststring32 sourceSymbol) {
	return Melder_equ (targetSymbol, sourceSymbol);
}

autoEditCostsTable EditCostsTable_create (integer targetAlphabetSize, integer sourceAlphabetSize) {
	try{
		autoEditCostsTable me = Thing_new (EditCostsTable);
		TableOfReal_init (me.get(), targetAlphabetSize + 2, sourceAlphabetSize + 2);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditCostsTable not created.");
	}
}

static autoEditCostsTable EditCostsTable_createDefault () {
	try {
		autoEditCostsTable me = EditCostsTable_create (0, 0);
		my data [1] [1] = 0.0;   // default substitution cost (nomatch == nomatch)
		my data [2] [2] = 2.0;   // default substitution cost (nomatch != nomatch)
		my data [2] [1] = 1.0;   // default insertion cost
		my data [1] [2] = 1.0;   // default deletion cost
		return me;
	} catch (MelderError) {
		Melder_throw (U"Default EditCostsTable not created.");
	}
}

void EditCostsTable_setDefaultCosts (EditCostsTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	my data [my numberOfRows - 1] [my numberOfColumns - 1] = 0.0;
	my data [my numberOfRows] [my numberOfColumns] = substitutionCosts;
	my data [my numberOfRows] [my numberOfColumns - 1] = deletionCosts;
	my data [my numberOfRows - 1] [my numberOfColumns] = insertionCosts;
}

integer EditCostsTable_getTargetIndex (EditCostsTable me, conststring32 symbol) {
	for (integer i = 1; i <= my numberOfRows - 2; i ++)
		if (my v_matchTargetSymbol (my rowLabels [i].get(), symbol))
			return i;
	return 0;
}

integer EditCostsTable_getSourceIndex (EditCostsTable me, conststring32 symbol) {
	for (integer j = 1; j <= my numberOfColumns - 2; j ++)
		if (my v_matchSourceSymbol (my columnLabels [j].get(), symbol))
			return j;
	return 0;
}

void EditCostsTable_setInsertionCosts (EditCostsTable me, conststring32 targets_string, double cost) {
	autoSTRVEC targets = splitByWhitespace_STRVEC (targets_string);
	for (integer itarget = 1; itarget <= targets.size; itarget ++) {
		integer irow = EditCostsTable_getTargetIndex (me, targets [itarget].get());
		irow = ( irow > 0 ? irow : my numberOfRows - 1 );   // nomatch condition to penultimate row
		my data [irow] [my numberOfColumns] = cost;
	}
}

void EditCostsTable_setDeletionCosts (EditCostsTable me, conststring32 sources_string, double cost) {
	autoSTRVEC sources = splitByWhitespace_STRVEC (sources_string);
	for (integer isource = 1; isource <= sources.size; isource ++) {
		integer icol = EditCostsTable_getSourceIndex (me, sources [isource].get());
		icol = ( icol > 0 ? icol : my numberOfColumns - 1 );   // nomatch condition to penultimate column
		my data [my numberOfRows] [icol] = cost;
	}
}

void EditCostsTable_setOthersCosts (EditCostsTable me, double insertionCost, double deletionCost, double substitutionCost_equal, double substitutionCost_unequal) {
	my data [my numberOfRows - 1] [my numberOfColumns] = insertionCost;
	my data [my numberOfRows] [my numberOfColumns - 1] = deletionCost;
	my data [my numberOfRows - 1] [my numberOfColumns - 1] = substitutionCost_unequal;
	my data [my numberOfRows] [my numberOfColumns] = substitutionCost_equal;
}

double EditCostsTable_getOthersCost (EditCostsTable me, int costType) {
	return ( costType == 1 ? my data [my numberOfRows - 1] [my numberOfColumns] :   //insertion
		( costType == 2 ? my data [my numberOfRows] [my numberOfColumns - 1] :   // deletion
		( costType == 3 ? my data [my numberOfRows] [my numberOfColumns] :   // equality
		my data [my numberOfRows - 1] [my numberOfColumns - 1] ) ) );   // inequality
}

void EditCostsTable_setSubstitutionCosts (EditCostsTable me, conststring32 targets_string, conststring32 sources_string, double cost) {
	try {
		autoSTRVEC targets = splitByWhitespace_STRVEC (targets_string);
		autoSTRVEC sources = splitByWhitespace_STRVEC (sources_string);
		autoINTVEC targetIndex = zero_INTVEC (my numberOfRows);   // note: this includes zero padding
		autoINTVEC sourceIndex = zero_INTVEC (my numberOfRows);   // note: this includes zero padding
		integer numberOfTargetSymbols = 0;
		for (integer itarget = 1; itarget <= targets.size; itarget ++) {
			const integer index = EditCostsTable_getTargetIndex (me, targets [itarget].get());
			if (index > 0)
				targetIndex [++ numberOfTargetSymbols] = index;
		}
		if (numberOfTargetSymbols == 0)
			targetIndex [++ numberOfTargetSymbols] = my numberOfRows - 1;
		integer numberOfSourceSymbols = 0;
		for (integer isource = 1; isource <= sources.size; isource ++) {
			const integer index = EditCostsTable_getSourceIndex (me, sources [isource].get());
			if (index > 0)
				sourceIndex [++ numberOfSourceSymbols] = index;
		}
		if (numberOfSourceSymbols == 0)
			sourceIndex [++ numberOfSourceSymbols] = my numberOfColumns - 1;
		for (integer i = 1; i <= numberOfTargetSymbols; i ++) {
			const integer irow = targetIndex [i];
			for (integer j = 1; j <= numberOfSourceSymbols; j ++)
				my data [irow] [sourceIndex [j]] = cost;
		}
	} catch (MelderError) {
		Melder_throw (me, U": substitution costs not set.");
	}
}

double EditCostsTable_getInsertionCost (EditCostsTable me, conststring32 symbol) {
	integer irow = EditCostsTable_getTargetIndex (me, symbol);
	irow = ( irow == 0 ? my numberOfRows - 1 : irow );   // others is penultimate row
	return my data [irow] [my numberOfColumns];
}

double EditCostsTable_getDeletionCost (EditCostsTable me, conststring32 sourceSymbol) {
	integer icol = EditCostsTable_getSourceIndex (me, sourceSymbol);
	icol = ( icol == 0 ? my numberOfColumns - 1 : icol );   // others is penultimate column
	return my data [my numberOfRows] [icol];
}

double EditCostsTable_getSubstitutionCost (EditCostsTable me, conststring32 symbol, conststring32 replacement) {
	integer irow = EditCostsTable_getTargetIndex (me, symbol);
	integer icol = EditCostsTable_getSourceIndex (me, replacement);
	if (irow == 0 && icol == 0) { // nomatch
		irow = my numberOfRows;
		icol = my numberOfColumns;
		if (my v_matchTargetWithSourceSymbol (symbol, replacement)) {
			 -- irow;
			 -- icol;
		}
	} else {
		irow = ( irow == 0 ? my numberOfRows - 1 : irow );
		icol = ( icol == 0 ? my numberOfColumns - 1 : icol );
	}
	return my data [irow] [icol];
}

autoTableOfReal EditCostsTable_to_TableOfReal (EditCostsTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		thy columnLabels.all()  <<=  my columnLabels.all();
		thy rowLabels.all()  <<=  my rowLabels.all();
		thy data.all()  <<=  my data.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

Thing_implement (EditDistanceTable, TableOfReal, 0);

void structEditDistanceTable :: v1_info () {
	EditDistanceTable_Parent :: v1_info ();
	MelderInfo_writeLine (U"Target:", numberOfRows, U" symbols.");
	MelderInfo_writeLine (U"Source:", numberOfColumns, U" symbols.");
}

autoEditDistanceTable EditDistanceTable_create (Strings target, Strings source) {
	try {
		autoEditDistanceTable me = Thing_new (EditDistanceTable);
		const integer numberOfSourceSymbols = source -> numberOfStrings, numberOfTargetSymbols = target -> numberOfStrings;
		TableOfReal_init (me.get(), numberOfTargetSymbols + 1, numberOfSourceSymbols + 1);
		TableOfReal_setColumnLabel (me.get(), 1, U"");
		for (integer j = 1; j <= numberOfSourceSymbols; j ++)
			my columnLabels [j + 1] = Melder_dup (source -> strings [j].get());
		TableOfReal_setRowLabel (me.get(), 1, U"");
		for (integer i = 1; i <= numberOfTargetSymbols; i ++)
			my rowLabels [i + 1] = Melder_dup (target -> strings [i].get());
		my path = newvectorzero <structPairOfInteger> (numberOfTargetSymbols + numberOfSourceSymbols + 1);
		my pathLength = my path.size;   // maintain invariant
		my editCostsTable = EditCostsTable_createDefault ();
		EditDistanceTable_findPath (me.get(), nullptr);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditDistanceTable not created.");
	}
}

void EditDistanceTable_setEditCosts (EditDistanceTable me, EditCostsTable thee) {
	try {
		my editCostsTable = Data_copy (thee);
	} catch (MelderError) {
		Melder_throw (me, U": edit costs not set.");
	}
}

autoEditDistanceTable EditDistanceTable_createFromCharacterStrings (const char32 chars1 [], const char32 chars2 []) {
	try {
		autoStrings s1 = Strings_createAsCharacters (chars1);
		autoStrings s2 = Strings_createAsCharacters (chars2);
		autoEditDistanceTable me = EditDistanceTable_create (s1.get(), s2.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"EditDistanceTable not created from character strings.");
	}
}

static void NUMrationalize (double x, integer *numerator, integer *denominator) {
	constexpr double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		const double numerator_d = x * *denominator, rounded = round (numerator_d);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = (integer) rounded;
			return;
		}
	}
	*denominator = 0;   /* Failure. */
}

static void fixRows (TableOfReal me, integer *rowmin, integer *rowmax) {
	if (*rowmax < *rowmin) {
		*rowmin = 1; 
		*rowmax = my numberOfRows;
	} else if (*rowmin < 1) {
		*rowmin = 1;
	} else if (*rowmax > my numberOfRows) {
		*rowmax = my numberOfRows;
	}
}

static void print4 (char *buffer, double value, int iformat, int width, int precision) {
	char formatString [40];
	if (iformat == 4) {
		integer numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			snprintf (buffer, 40, "0");
		else if (denominator > 1)
			snprintf (buffer, 40, "%s/%s", Melder8_integer (numerator), Melder8_integer (denominator));
		else
			snprintf (buffer, 40, "%.7g", value);
	} else {
		snprintf (formatString, 40, "%%%d.%d%c", width, precision, ( iformat == 1 ? 'f' : ( iformat == 2 ? 'e' : 'g' ) ));
		snprintf (buffer, 40, formatString, value);
	}
}

static double getMaxRowLabelWidth (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	double maxWidth = 0.0;
	if (! my rowLabels)
		return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (integer irow = rowmin; irow <= rowmax; irow ++) {
		if (my rowLabels [irow] && my rowLabels [irow] [0]) {
			const double textWidth = Graphics_textWidth_ps (graphics, my rowLabels [irow].get(), true);   /* SILIPA is bigger than XIPA */
			if (textWidth > maxWidth) {
				maxWidth = textWidth;
			}
		}
	}
	return maxWidth;
}

static double getLeftMargin (Graphics graphics) {
	return Graphics_dxMMtoWC (graphics, 1.0);
}

static double getLineSpacing (Graphics graphics) {
	return Graphics_dyMMtoWC (graphics, 1.5 * Graphics_inqFontSize (graphics) * 25.4 / 72.0);
}

void EditDistanceTable_draw (EditDistanceTable me, Graphics graphics, int iformat, int precision, double angle) {
	const integer rowmin = 1, rowmax = my numberOfRows;
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0.0, 1.0);
	const double leftMargin = getLeftMargin (graphics);   // not earlier!
	const double lineSpacing = getLineSpacing (graphics);   // not earlier!
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	double y = 1.0 + 0.1 * lineSpacing;
	autoBOOLMAT onPath = zero_BOOLMAT (my numberOfRows, my numberOfColumns);
	for (integer i = 1; i <= my pathLength; i ++) {
		const structPairOfInteger poi = my path [i];
		onPath [poi.y] [poi.x] = true;
	}
	for (integer irow = my numberOfRows; irow > 0; irow --) {
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
			Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow].get());
		Graphics_setTextAlignment (graphics, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			char text [40];
			print4 (text, my data [irow] [icol], iformat, 0, precision);
			Graphics_setBold (graphics, onPath [irow] [icol]);
			Graphics_text (graphics, icol, y, Melder_peek8to32_u (text));
			if (onPath [irow] [icol]) {
				Graphics_rectangle (graphics, icol - 0.5, icol + 0.5, y - 0.5 * lineSpacing, y + 0.5 * lineSpacing);
			}
		}
		y -= lineSpacing;
		Graphics_setBold (graphics, false);
	}

	double left = 0.5;
	if (maxTextWidth > 0.0) left -= maxTextWidth + 2.0 * leftMargin;
	Graphics_line (graphics, left, y, my numberOfColumns + 0.5, y);

	Graphics_setTextRotation (graphics, angle);
	if (angle < 0.0) {
		y -= 0.3 * lineSpacing;
		Graphics_setTextAlignment (graphics, Graphics_LEFT, Graphics_HALF);
	} else if (angle > 0.0) {
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		y -= 0.3 * lineSpacing;
	} else {
		Graphics_setTextAlignment (graphics, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
	}
	for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
		if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
			Graphics_text (graphics, icol, y, my columnLabels [icol].get());
	}
	Graphics_setTextRotation (graphics, 0.0);
	y -= lineSpacing;
	Graphics_line (graphics, 0.5, y, 0.5, 1 + 0.5 * lineSpacing);
	Graphics_unsetInner (graphics);
}

void EditDistanceTable_drawEditOperations (EditDistanceTable me, Graphics graphics) {
	const conststring32 oinsertion = U"i", insertion = U"*", odeletion = U"d", deletion = U"*", osubstitution = U"s", oequal = U"";
	Graphics_setWindow (graphics, 0.5, my pathLength - 0.5, 0.0, 1.0);   // pathLength-1 symbols
	const double lineSpacing = getLineSpacing (graphics);
	const double ytarget = 1 - lineSpacing, ysource = ytarget - 2 * lineSpacing, yoper = ysource - lineSpacing;
	Graphics_setTextAlignment (graphics, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
	for (integer i = 2; i <= my pathLength; i ++) {
		const structPairOfInteger p = my path [i], p1 = my path [i - 1];
		const double x = i - 1;
		if (p.x == p1.x) { // insertion
			Graphics_text (graphics, x, ytarget, my rowLabels [p.y].get());
			Graphics_text (graphics, x, ysource, deletion);
			Graphics_text (graphics, x, yoper, oinsertion);
		} else if (p.y == p1.y) { // deletion
			Graphics_text (graphics, x, ytarget, insertion);
			Graphics_text (graphics, x, ysource, my columnLabels [p.x].get());
			Graphics_text (graphics, x, yoper, odeletion);
		} else { // substitution ?
			Graphics_text (graphics, x, ytarget, my rowLabels [p.y].get());
			Graphics_text (graphics, x, ysource, my columnLabels [p.x].get());
			Graphics_text (graphics, x, yoper, ( Melder_equ (my rowLabels [p.y].get(), my columnLabels [p.x].get()) ? oequal : osubstitution ));
		}
		Graphics_line (graphics, x, ysource + lineSpacing, x, ytarget - 0.1 * lineSpacing);
	}
}

void EditDistanceTable_setDefaultCosts (EditDistanceTable me, double insertionCosts, double deletionCosts, double substitutionCosts) {
	EditCostsTable_setDefaultCosts (my editCostsTable.get(), insertionCosts, deletionCosts, substitutionCosts);
	EditDistanceTable_findPath (me, nullptr);
}

autoTableOfReal EditDistanceTable_to_TableOfReal_directions (EditDistanceTable me) {
	autoTableOfReal tor;
	EditDistanceTable_findPath (me, & tor);
	return tor;
}

void EditDistanceTable_findPath (EditDistanceTable me, autoTableOfReal *out_directions) {
	try {
		/*
			What do we have to do to source to get target?
			The origin [1] [1] is in the bottom-left corner.
			Target vertical, source horizontal.
			Going in the vertical direction is a deletion, horizontal is insertion, diagonal is substitution.
		*/
		const integer numberOfSources = my numberOfColumns - 1, numberOfTargets = my numberOfRows - 1;
		autoINTMAT psi = zero_INTMAT (my numberOfRows, my numberOfColumns);
		autoMAT delta = zero_MAT (my numberOfRows, my numberOfColumns);

		for (integer icol = 2; icol <= my numberOfColumns; icol ++) {
			delta [1] [icol] = delta [1] [icol - 1] + EditCostsTable_getDeletionCost (my editCostsTable.get(), my columnLabels [icol].get());
			psi [1] [icol] = WARPING_fromLeft;
		}
		for (integer irow = 2; irow <= my numberOfRows; irow ++) {
			delta [irow] [1] = delta [irow - 1] [1] + EditCostsTable_getInsertionCost (my editCostsTable.get(), my rowLabels [irow].get());
			psi [irow] [1] = WARPING_fromBelow;
		}
		for (integer icol = 2; icol <= my numberOfColumns; icol ++) {
			for (integer irow = 2; irow <= my numberOfRows; irow ++) {
				// the substitution, deletion and insertion costs.
				const double left = delta [irow] [icol - 1] +
						EditCostsTable_getInsertionCost (my editCostsTable.get(), my rowLabels [irow].get());
				const double bottom = delta [irow - 1] [icol] +
						EditCostsTable_getDeletionCost (my editCostsTable.get(), my columnLabels [icol].get());
				double mindist = delta [irow - 1] [icol - 1] +
						EditCostsTable_getSubstitutionCost (my editCostsTable.get(), my rowLabels [irow].get(), my columnLabels [icol].get()); // diag
				psi [irow] [icol] = WARPING_fromDiag;
				if (bottom < mindist) {
					mindist = bottom;
					psi [irow] [icol] = WARPING_fromBelow;
				}
				if (left < mindist) {
					mindist = left;
					psi [irow] [icol] = WARPING_fromLeft;
				}
				delta [irow] [icol] = mindist;
			}
		}

		/*
			Reset the path to its maximum length, clearing it to zero.
		*/
		my path. resize (my numberOfRows + my numberOfColumns - 1);
		for (integer i = 1; i <= my path.size; i ++)
			my path [i]. x = my path [i]. y = 0;
		my pathLength = my path.size;   // maintain invariant

		/*
			Compute the shortest path.
		*/
		integer iy = numberOfTargets, ix = numberOfSources;
		integer index = my pathLength;
		my path [index]. x = ix;
		my path [index]. y = iy;
		while (! (ix == 0 && iy == 0)) {
			Melder_assert (ix >= 0 && iy >= 0);
			if (psi [1 + iy] [1 + ix] == WARPING_fromLeft) {
				ix --;
			} else if (psi [1 + iy] [1 + ix] == WARPING_fromBelow) {
				iy --;
			} else {   // WARPING_fromDiag
				ix --;
				iy --;
			}
			my path [-- index]. x = ix;
			my path [index]. y = iy;
		}
		if (index > 1) {
			integer k = 1;
			for (integer i = index; i <= my pathLength; i ++) {
				my path [k ++] = my path [i];
				my path [i]. x = my path [i]. y = 0;
			}
			my path. resize (k - 1);
			my pathLength = my path.size;   // maintain invariant
		}

		/*
			Shift by 1.
		*/
		for (integer i = 1; i <= my pathLength; i ++) {
			my path [i]. x ++;
			my path [i]. y ++;
		}

		my data.all()  <<=  delta.all();

		if (out_directions) {
			autoTableOfReal him = EditDistanceTable_to_TableOfReal (me);
			for (integer irow = 1; irow <= my numberOfRows; irow ++)
				for (integer icol = 1; icol <= my numberOfColumns; icol ++)
					his data [irow] [icol] = double (psi [irow] [icol]);
			*out_directions = him.move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": minimum path not found.");
	}
}

autoTableOfReal EditDistanceTable_to_TableOfReal (EditDistanceTable me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, my numberOfColumns);
		thy columnLabels.all()  <<=  my columnLabels.all();
		thy rowLabels.all()  <<=  my rowLabels.all();
		thy data.all()  <<=  my data.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no TableOfReal created.");
	}
}

/* End of file EditDistanceTable.cpp */
