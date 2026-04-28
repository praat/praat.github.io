/* TableOfReal.cpp
 *
 * Copyright (C) 1992-2024,2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include "TableOfReal.h"
#include "NUM2.h"
#include "Matrix.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "TableOfReal_def.h"
#include "oo_COPY.h"
#include "TableOfReal_def.h"
#include "oo_EQUAL.h"
#include "TableOfReal_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "TableOfReal_def.h"
#include "oo_WRITE_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_READ_BINARY.h"
#include "TableOfReal_def.h"
#include "oo_DESCRIPTION.h"
#include "TableOfReal_def.h"

Thing_implement (TableOfReal, Daata, 0);
Thing_implement (TableOfRealList, Ordered, 0);

static void fprintquotedstring (MelderFile file, conststring32 s) {
	MelderFile_writeCharacter (file, U'\"');
	if (s) { char32 c; while ((c = *s ++) != U'\0') { MelderFile_writeCharacter (file, c); if (c == U'\"') MelderFile_writeCharacter (file, c); } }
	MelderFile_writeCharacter (file, U'\"');
}

void structTableOfReal :: v1_writeText (MelderFile file) {
	texputi32 (file, our numberOfColumns, U"numberOfColumns");
	MelderFile_write (file, U"\ncolumnLabels []: ");
	if (our numberOfColumns < 1) MelderFile_write (file, U"(empty)");
	MelderFile_write (file, U"\n");
	for (integer i = 1; i <= our numberOfColumns; i ++) {
		fprintquotedstring (file, our columnLabels [i].get());
		MelderFile_writeCharacter (file, U'\t');
	}
	texputi32 (file, our numberOfRows, U"numberOfRows");
	for (integer i = 1; i <= our numberOfRows; i ++) {
		MelderFile_write (file, U"\nrow [", i, U"]: ");
		fprintquotedstring (file, our rowLabels [i].get());
		for (integer j = 1; j <= our numberOfColumns; j ++) {
			double x = our data [i] [j];
			MelderFile_write (file, U"\t", x);
		}
	}
}

void structTableOfReal :: v1_readText (MelderReadText a_text, int /*formatVersion*/) {
	our numberOfColumns = texgeti32 (a_text);
	if (our numberOfColumns >= 1) {
		our columnLabels = autoSTRVEC (our numberOfColumns);
		for (integer i = 1; i <= our numberOfColumns; i ++)
			our columnLabels [i] = texgetw16 (a_text);
	}
	our numberOfRows = texgeti32 (a_text);
	if (our numberOfRows >= 1) {
		our rowLabels = autoSTRVEC (our numberOfRows);
	}
	if (our numberOfRows >= 1 && our numberOfColumns >= 1) {
		our data = zero_MAT (our numberOfRows, our numberOfColumns);
		for (integer i = 1; i <= our numberOfRows; i ++) {
			our rowLabels [i] = texgetw16 (a_text);
			for (integer j = 1; j <= our numberOfColumns; j ++)
				our data [i] [j] = texgetr64 (a_text);
		}
	}
}

void structTableOfReal :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Number of rows: ", our numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", our numberOfColumns);
}

conststring32 structTableOfReal :: v_getRowStr (const integer irow) const {
	if (irow < 1 || irow > our numberOfRows)
		return nullptr;
	return our rowLabels [irow] ? our rowLabels [irow].get() : U"";
}
conststring32 structTableOfReal :: v_getColStr (const integer icol) const {
	if (icol < 1 || icol > our numberOfColumns)
		return nullptr;
	return our columnLabels [icol] ? our columnLabels [icol].get() : U"";
}
double structTableOfReal :: v_getMatrix (const integer irow, const integer icol) const {
	if (irow < 1 || irow > our numberOfRows)
		return undefined;
	if (icol < 1 || icol > our numberOfColumns)
		return undefined;
	return our data [irow] [icol];
}
double structTableOfReal :: v_getRowIndex (const conststring32 rowLabel) const {
	return TableOfReal_rowLabelToIndex (this, rowLabel);
}
double structTableOfReal :: v_getColIndex (const conststring32 columnLabel) const {
	return TableOfReal_columnLabelToIndex (this, columnLabel);
}

void TableOfReal_init (TableOfReal me, integer numberOfRows, integer numberOfColumns) {
	if (numberOfRows < 1 || numberOfColumns < 1)
		Melder_throw (U"Cannot create cell-less table.");
	my numberOfRows = numberOfRows;
	my numberOfColumns = numberOfColumns;
	my rowLabels = autoSTRVEC (numberOfRows);
	Melder_assert (my rowLabels.size == numberOfRows);   // probably captured by test script
	my columnLabels = autoSTRVEC (numberOfColumns);
	my data = zero_MAT (my numberOfRows, my numberOfColumns);
}

autoTableOfReal TableOfReal_create (integer numberOfRows, integer numberOfColumns) {
	try {
		autoTableOfReal me = Thing_new (TableOfReal);
		TableOfReal_init (me.get(), numberOfRows, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal not created.");
	}
}

/***** QUERY *****/

integer TableOfReal_rowLabelToIndex (const constTableOfReal me, const conststring32 label) {
	for (integer irow = 1; irow <= my numberOfRows; irow ++)
		if (my rowLabels [irow] && str32equ (my rowLabels [irow].get(), label))
			return irow;
	return 0;
}

integer TableOfReal_columnLabelToIndex (const constTableOfReal me, const conststring32 label) {
	for (integer icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnLabels [icol] && str32equ (my columnLabels [icol].get(), label))
			return icol;
	return 0;
}

double TableOfReal_getColumnMean (TableOfReal me, integer columnNumber) {
	if (columnNumber < 1 || columnNumber > my numberOfColumns)
		return undefined;
	return NUMmean (my data.column (columnNumber));
}

double TableOfReal_getColumnStdev (TableOfReal me, integer columnNumber) {
	if (columnNumber < 1 || columnNumber > my numberOfColumns)
		return undefined;
	if (my numberOfRows < 2)
		return undefined;
	return NUMstdev (my data.column (columnNumber));
}

/***** MODIFY *****/

void TableOfReal_removeRow (TableOfReal me, integer rowNumber) {
	try {
		Melder_require (my numberOfRows > 1,
			me, U" has only one row, and a TableOfReal without rows cannot exist.");
		Melder_require (rowNumber > 0 && rowNumber <= my numberOfRows,
			U"No row ", rowNumber, U".");
		autoMAT newData = raw_MAT (my numberOfRows - 1, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			for (integer irow = 1; irow < rowNumber; irow ++)
				newData [irow] [icol] = my data [irow] [icol];
			for (integer irow = rowNumber; irow < my numberOfRows; irow ++)
				newData [irow] [icol] = my data [irow + 1] [icol];
		}
		/*
			Change without error.
		*/
		for (integer irow = rowNumber; irow < my numberOfRows; irow ++)
			my rowLabels [irow] = my rowLabels [irow + 1]. move();
		my rowLabels [my numberOfRows]. reset();
		my rowLabels.size -= 1;
		my data = newData.move();
		my numberOfRows --;
	} catch (MelderError) {
		Melder_throw (me, U": row ", rowNumber, U" not removed.");
	}
}

void TableOfReal_insertRow (TableOfReal me, integer rowNumber) {
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows + 1)
			Melder_throw (U"Cannot create row ", rowNumber, U".");
		/*
			Create without change.
		*/
		autoMAT newData = zero_MAT (my numberOfRows + 1, my numberOfColumns);
		autoSTRVEC newRowLabels (my numberOfRows + 1);
		for (integer irow = 1; irow < rowNumber; irow ++)	{
			newRowLabels [irow] = my rowLabels [irow]. move();
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				newData [irow] [icol] = my data [irow] [icol];
		}
		for (integer irow = my numberOfRows + 1; irow > rowNumber; irow --) {
			newRowLabels [irow] = my rowLabels [irow - 1]. move();
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				newData [irow] [icol] = my data [irow - 1] [icol];
		}
		/*
			Change without error.
		*/
		my rowLabels = std::move (newRowLabels);
		my data = newData.move();
		my numberOfRows ++;
	} catch (MelderError) {
		Melder_throw (me, U": row at position ", rowNumber, U" not inserted.");
	}
}

void TableOfReal_removeColumn (TableOfReal me, integer columnNumber) {
	try {
		if (my numberOfColumns == 1)
			Melder_throw (U"Cannot remove the only column.");
		if (columnNumber < 1 || columnNumber > my numberOfColumns)
			Melder_throw (U"No column ", columnNumber, U".");
		/*
			Create without change.
		*/
		autoMAT newData = raw_MAT (my numberOfRows, my numberOfColumns - 1);
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = 1; icol < columnNumber; icol ++)
				newData [irow] [icol] = my data [irow] [icol];
			for (integer icol = columnNumber; icol < my numberOfColumns; icol ++)
				newData [irow] [icol] = my data [irow] [icol + 1];
		}
		/*
			Change without error.
		*/
		for (integer icol = columnNumber; icol < my numberOfColumns; icol ++)
			my columnLabels [icol] = my columnLabels [icol + 1]. move();
		my columnLabels [my numberOfColumns]. reset();
		my columnLabels.size -= 1;
		my data = newData.move();
		my numberOfColumns --;
	} catch (MelderError) {
		Melder_throw (me, U": column ", columnNumber, U" not removed.");
	}
}

void TableOfReal_insertColumn (TableOfReal me, integer columnNumber) {
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns + 1)
			Melder_throw (U"Cannot create column ", columnNumber, U".");
		/*
			Create without change.
		*/
		autoMAT newData = zero_MAT (my numberOfRows, my numberOfColumns + 1);
		autoSTRVEC newColumnLabels (my numberOfColumns + 1);
		for (integer j = 1; j < columnNumber; j ++) {
			newColumnLabels [j] = my columnLabels [j]. move();
			for (integer i = 1; i <= my numberOfRows; i ++)
				newData [i] [j] = my data [i] [j];
		}
		for (integer j = my numberOfColumns + 1; j > columnNumber; j --) {
			newColumnLabels [j] = my columnLabels [j - 1]. move();
			for (integer i = 1; i <= my numberOfRows; i ++)
				newData [i] [j] = my data [i] [j - 1];
		}
		/*
			Change without error.
		*/
		my columnLabels = std::move (newColumnLabels);
		my data = newData.move();
		my numberOfColumns ++;
	} catch (MelderError) {
		Melder_throw (me, U": column at position ", columnNumber, U" not inserted.");
	}
}

void TableOfReal_setRowLabel (TableOfReal me, integer rowNumber, conststring32 label) {
	try {
		if (rowNumber < 1 || rowNumber > my numberOfRows) return;
		my rowLabels [rowNumber] = Melder_dup (label);
	} catch (MelderError) {
		Melder_throw (me, U": label of row ", rowNumber, U" not set.");
	}
}

void TableOfReal_setColumnLabel (TableOfReal me, integer columnNumber, conststring32 label) {
	try {
		if (columnNumber < 1 || columnNumber > my numberOfColumns)
			return;
		my columnLabels [columnNumber] = Melder_dup (label);
	} catch (MelderError) {
		Melder_throw (me, U": label of column ", columnNumber, U" not set.");
	}
}

void TableOfReal_formula (TableOfReal me, conststring32 expression, Interpreter interpreter, TableOfReal thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! thee)
			thee = me;
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				Formula_run (irow, icol, & result);
				thy data [irow] [icol] = result. numericResult;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

/***** EXTRACT PART *****/

static void copyRowLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	for (integer irow = 1; irow <= my numberOfRows; irow ++)
		thy rowLabels [irow] = Melder_dup (my rowLabels [irow].get());
}

static void copyColumnLabels (TableOfReal me, TableOfReal thee) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	for (integer icol = 1; icol <= my numberOfColumns; icol ++)
		thy columnLabels [icol] = Melder_dup (my columnLabels [icol].get());
}

static void copyRow (TableOfReal me, integer myRow, TableOfReal thee, integer thyRow) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfColumns == thy numberOfColumns);
	thy rowLabels [thyRow] = Melder_dup (my rowLabels [myRow].get());
	for (integer icol = 1; icol <= my numberOfColumns; icol ++)
		thy data [thyRow] [icol] = my data [myRow] [icol];
}

static void copyColumn (TableOfReal me, integer myCol, TableOfReal thee, integer thyCol) {
	Melder_assert (me != thee);
	Melder_assert (my numberOfRows == thy numberOfRows);
	thy columnLabels [thyCol] = Melder_dup (my columnLabels [myCol].get());
	for (integer irow = 1; irow <= my numberOfRows; irow ++)
		thy data [irow] [thyCol] = my data [irow] [myCol];
}

autoTableOfReal TableOfReal_extractRowsWhereColumn (TableOfReal me, integer column, kMelder_number which, double criterion) {
	try {
		if (column < 1 || column > my numberOfColumns)
			Melder_throw (U"No such column: ", column, U".");
		integer n = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_numberMatchesCriterion (my data [irow] [column], which, criterion))
				n ++;
		if (n == 0)
			Melder_throw (U"No row matches this criterion.");
		autoTableOfReal thee = TableOfReal_create (n, my numberOfColumns);
		copyColumnLabels (me, thee.get());
		n = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_numberMatchesCriterion (my data [irow] [column], which, criterion))
				copyRow (me, irow, thee.get(), ++ n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTableOfReal TableOfReal_extractRowsWhoseLabel (TableOfReal me, kMelder_string which, conststring32 criterion) {
	try {
		integer n = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_stringMatchesCriterion (my rowLabels [irow].get(), which, criterion, true))
				n ++;
		if (n == 0)
			Melder_throw (U"No row matches this criterion.");
		autoTableOfReal thee = TableOfReal_create (n, my numberOfColumns);
		copyColumnLabels (me, thee.get());
		n = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++)
			if (Melder_stringMatchesCriterion (my rowLabels [irow].get(), which, criterion, true))
				copyRow (me, irow, thee.get(), ++ n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTableOfReal TableOfReal_extractColumnsWhereRow (TableOfReal me, integer row, kMelder_number which, double criterion) {
	try {
		if (row < 1 || row > my numberOfRows)
			Melder_throw (U"No such row: ", row, U".");
		integer n = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (Melder_numberMatchesCriterion (my data [row] [icol], which, criterion))
				n ++;
		if (n == 0)
			Melder_throw (U"No column matches this criterion.");

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, n);
		copyRowLabels (me, thee.get());
		n = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (Melder_numberMatchesCriterion (my data [row] [icol], which, criterion))
				copyColumn (me, icol, thee.get(), ++ n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": columns not extracted.");
	}
}

autoTableOfReal TableOfReal_extractColumnsWhoseLabel (TableOfReal me, kMelder_string which, conststring32 criterion) {
	try {
		integer n = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (Melder_stringMatchesCriterion (my columnLabels [icol].get(), which, criterion, true))
				n ++;
		if (n == 0)
			Melder_throw (U"No column matches this criterion.");

		autoTableOfReal thee = TableOfReal_create (my numberOfRows, n);
		copyRowLabels (me, thee.get());
		n = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (Melder_stringMatchesCriterion (my columnLabels [icol].get(), which, criterion, true))
				copyColumn (me, icol, thee.get(), ++ n);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": columns not extracted.");
	}
}

void TableOfReal_checkRowNumberWithinRange (TableOfReal me, integer rowNumber) {
	Melder_require (rowNumber >= 1 && rowNumber <= my data.nrow,
		me, U": row number (", rowNumber, U") should be between 1 and my number of rows (", my data.nrow, U").");
}
void TableOfReal_checkColumnNumberWithinRange (TableOfReal me, integer columnNumber) {
	Melder_require (columnNumber >= 1 && columnNumber <= my data.ncol,
		me, U": column number (", columnNumber, U") should be between 1 and my number of rows (", my data.ncol, U").");
}

autoTableOfReal TableOfReal_extractRowsByNumber (TableOfReal me, constINTVECVU const& rowNumbers) {
	try {
		autoTableOfReal you = TableOfReal_create (rowNumbers.size, my numberOfColumns);
		copyColumnLabels (me, you.get());
		for (integer ielement = 1; ielement <= rowNumbers.size; ielement ++) {
			TableOfReal_checkRowNumberWithinRange (me, rowNumbers [ielement]);
			copyRow (me, rowNumbers [ielement], you.get(), ielement);
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTableOfReal TableOfReal_extractColumnsByNumber (TableOfReal me, constINTVECVU const& columnNumbers) {
	try {
		autoTableOfReal you = TableOfReal_create (my numberOfRows, columnNumbers.size);
		copyRowLabels (me, you.get());
		for (integer ielement = 1; ielement <= columnNumbers.size; ielement ++) {
			TableOfReal_checkColumnNumberWithinRange (me, columnNumbers [ielement]);
			copyColumn (me, columnNumbers [ielement], you.get(), ielement);
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": columns not extracted.");
	}
}

autoTableOfReal TableOfReal_extractRowsWhere (TableOfReal me, conststring32 condition, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		/*
			Count the new number of rows.
		*/
		integer numberOfElements = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				Formula_run (irow, icol, & result);
				if (result. numericResult != 0.0) {
					numberOfElements ++;
					break;
				}
			}
		}
		if (numberOfElements < 1)
			Melder_throw (U"No rows match this condition.");

		/*
			Create room for the result.
		*/
		autoTableOfReal thee = TableOfReal_create (numberOfElements, my numberOfColumns);
		copyColumnLabels (me, thee.get());
		/*
			Store the result.
		*/
		numberOfElements = 0;
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				Formula_run (irow, icol, & result);
				if (result. numericResult != 0.0) {
					copyRow (me, irow, thee.get(), ++ numberOfElements);
					break;
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": rows not extracted.");
	}
}

autoTableOfReal TableOfReal_extractColumnsWhere (TableOfReal me, conststring32 condition, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, condition, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		/*
			Count the new number of columns.
		*/
		integer numberOfElements = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			for (integer irow = 1; irow <= my numberOfRows; irow ++) {
				Formula_run (irow, icol, & result);
				if (result. numericResult != 0.0) {
					numberOfElements ++;
					break;
				}
			}
		}
		if (numberOfElements < 1)
			Melder_throw (U"No columns match this condition.");

		/*
			Create room for the result.
		*/
		autoTableOfReal thee = TableOfReal_create (my numberOfRows, numberOfElements);
		copyRowLabels (me, thee.get());
		/*
			Store the result.
		*/
		numberOfElements = 0;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			for (integer irow = 1; irow <= my numberOfRows; irow ++) {
				Formula_run (irow, icol, & result);
				if (result. numericResult != 0.0) {
					copyColumn (me, icol, thee.get(), ++ numberOfElements);
					break;
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": columns not extracted.");
	}
}

/***** EXTRACT *****/

autoStrings TableOfReal_extractRowLabelsAsStrings (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my numberOfRows);
		thy numberOfStrings = my numberOfRows;
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			thy strings [irow] = Melder_dup (my rowLabels [irow] ? my rowLabels [irow].get() : U"");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": row labels not extracted.");
	}
}

autoStrings TableOfReal_extractColumnLabelsAsStrings (TableOfReal me) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my numberOfColumns);
		thy numberOfStrings = my numberOfColumns;
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			thy strings [icol] = Melder_dup (my columnLabels [icol] ? my columnLabels [icol].get() : U"");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": column labels not extracted.");
	}
}

/***** DRAW *****/

static void NUMrationalize (double x, integer *numerator, integer *denominator) {
	constexpr double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator;
		integer rounded = Melder_iround (numerator_d);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = rounded;
			return;
		}
	}
	*denominator = 0;   /* Failure. */
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
		snprintf (formatString, 40, "%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		snprintf (buffer, 40, formatString, value);
	}
}

static void fixRows (TableOfReal me, integer *rowmin, integer *rowmax) {
	if (*rowmax < *rowmin) { *rowmin = 1; *rowmax = my numberOfRows; }
	else if (*rowmin < 1) *rowmin = 1;
	else if (*rowmax > my numberOfRows) *rowmax = my numberOfRows;
}
static void fixColumns (TableOfReal me, integer *colmin, integer *colmax) {
	if (*colmax < *colmin) { *colmin = 1; *colmax = my numberOfColumns; }
	else if (*colmin < 1) *colmin = 1;
	else if (*colmax > my numberOfColumns) *colmax = my numberOfColumns;
}
static double getMaxRowLabelWidth (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	double maxWidth = 0.0;
	if (! my rowLabels) return 0.0;
	fixRows (me, & rowmin, & rowmax);
	for (integer irow = rowmin; irow <= rowmax; irow ++) if (my rowLabels [irow] && my rowLabels [irow] [0]) {
		double textWidth = Graphics_textWidth_ps (graphics, my rowLabels [irow].get(), true);   // SILIPA is bigger than XIPA
		if (textWidth > maxWidth) maxWidth = textWidth;
	}
	return maxWidth;
}
static double getLeftMargin (Graphics graphics) {
	return Graphics_dxMMtoWC (graphics, 1.0);
}
static double getLineSpacing (Graphics graphics) {
	return Graphics_dyMMtoWC (graphics, 1.5 * Graphics_inqFontSize (graphics) * 25.4 / 72.0);
}
static double getMaxColumnLabelHeight (TableOfReal me, Graphics graphics, integer colmin, integer colmax) {
	double maxHeight = 0.0, lineSpacing = getLineSpacing (graphics);
	if (! my columnLabels) return 0.0;
	fixRows (me, & colmin, & colmax);
	for (integer icol = colmin; icol <= colmax; icol ++) if (my columnLabels [icol] && my columnLabels [icol] [0]) {
		if (maxHeight == 0.0) maxHeight = lineSpacing;
	}
	return maxHeight;
}

void TableOfReal_drawAsNumbers (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax, int iformat, int precision) {
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0.0, 1.0);
	const double leftMargin = getLeftMargin (graphics);   // not earlier!
	const double lineSpacing = getLineSpacing (graphics);   // not earlier!
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
	for (integer icol = 1; icol <= my numberOfColumns; icol ++)
		if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
			Graphics_text (graphics, icol, 1, my columnLabels [icol].get());
	for (integer irow = rowmin; irow <= rowmax; irow ++) {
		const double y = 1.0 - lineSpacing * (irow - rowmin + 0.6);
		Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
		if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
			Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow].get());
		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			char text [40];
			print4 (text, my data [irow] [icol], iformat, 0, precision);
			Graphics_text (graphics, icol, y, Melder_peek8to32_u (text));
		}
	}
	if (maxTextHeight != 0.0) {
		double left = 0.5;
		if (maxTextWidth > 0.0)
			left -= maxTextWidth + 2 * leftMargin;
		Graphics_line (graphics, left, 1.0, my numberOfColumns + 0.5, 1.0);
	}
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawAsNumbers_if (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax, int iformat, int precision,
	conststring32 conditionFormula, Interpreter interpreter)
{
	try {
		autoMatrix original = TableOfReal_to_Matrix (me);
		autoMatrix conditions = Data_copy (original.get());
		fixRows (me, & rowmin, & rowmax);
		Graphics_setInner (graphics);
		Graphics_setWindow (graphics, 0.5, my numberOfColumns + 0.5, 0.0, 1.0);
		const double leftMargin = getLeftMargin (graphics);   // not earlier!
		const double lineSpacing = getLineSpacing (graphics);   // not earlier!
		const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
		const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);
		Matrix_formula (original.get(), conditionFormula, interpreter, conditions.get());

		Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_BOTTOM);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (my columnLabels && my columnLabels [icol] && my columnLabels [icol] [0])
				Graphics_text (graphics, icol, 1, my columnLabels [icol].get());
		for (integer irow = rowmin; irow <= rowmax; irow ++) {
			const double y = 1.0 - lineSpacing * (irow - rowmin + 0.6);
			Graphics_setTextAlignment (graphics, Graphics_RIGHT, Graphics_HALF);
			if (my rowLabels && my rowLabels [irow] && my rowLabels [irow] [0])
				Graphics_text (graphics, 0.5 - leftMargin, y, my rowLabels [irow].get());
			Graphics_setTextAlignment (graphics, Graphics_CENTRE, Graphics_HALF);
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) if (conditions -> z [irow] [icol] != 0.0) {
				char text [40];
				print4 (text, my data [irow] [icol], iformat, 0, precision);
				Graphics_text (graphics, icol, y, Melder_peek8to32_u (text));
			}
		}
		if (maxTextHeight != 0.0) {
			double left = 0.5;
			if (maxTextWidth > 0.0)
				left -= maxTextWidth + 2 * leftMargin;
			Graphics_line (graphics, left, 1.0, my numberOfColumns + 0.5, 1.0);
		}
		Graphics_unsetInner (graphics);
	} catch (MelderError) {
		Melder_throw (me, U": numbers not drawn.");
	}
}

void TableOfReal_drawVerticalLines (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	integer colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	const double lineSpacing = getLineSpacing (graphics);   // not earlier!
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	if (maxTextWidth > 0.0)
		colmin -= 1;
	for (integer col = colmin + 1; col <= colmax; col ++)
		Graphics_line (graphics, col - 0.5, 1.0 + maxTextHeight, col - 0.5, 1.0 - lineSpacing * (rowmax - rowmin + 1));
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawLeftAndRightLines (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	integer colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0.0, 1.0);
	const double lineSpacing = getLineSpacing (graphics);
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	if (maxTextWidth > 0.0)
		left -= maxTextWidth + 2.0 * lineSpacing;
	const double right = colmax + 0.5;
	const double top = 1.0 + maxTextHeight;
	const double bottom = 1.0 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (graphics, left, top, left, bottom);
	Graphics_line (graphics, right, top, right, bottom);
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawHorizontalLines (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	integer colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0, 1);
	const double lineSpacing = getLineSpacing (graphics);
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	const double top = rowmin;
	if (maxTextWidth > 0.0)
		left -= maxTextWidth + 2.0 * lineSpacing;
	if (maxTextHeight > 0.0)
		rowmin -= 1;
	const double right = colmax + 0.5;
	for (integer irow = rowmin; irow < rowmax; irow ++) {
		const double y = 1.0 - lineSpacing * (irow - top + 1);
		Graphics_line (graphics, left, y, right, y);
	}
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawTopAndBottomLines (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax) {
	integer colmin = 1, colmax = my numberOfColumns;
	fixRows (me, & rowmin, & rowmax);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, 0.0, 1.0);
	const double lineSpacing = getLineSpacing (graphics);
	const double maxTextWidth = getMaxRowLabelWidth (me, graphics, rowmin, rowmax);
	const double maxTextHeight = getMaxColumnLabelHeight (me, graphics, 1, my numberOfColumns);

	double left = 0.5;
	if (maxTextWidth > 0.0)
		left -= maxTextWidth + 2 * lineSpacing;
	const double right = colmax + 0.5;
	const double top = 1.0 + maxTextHeight;
	const double bottom = 1.0 - lineSpacing * (rowmax - rowmin + 1);
	Graphics_line (graphics, left, top, right, top);
	Graphics_line (graphics, left, bottom, right, bottom);
	Graphics_unsetInner (graphics);
}

void TableOfReal_drawAsSquares (TableOfReal me, Graphics graphics, integer rowmin, integer rowmax,
	integer colmin, integer colmax, bool garnish)
{
	const double dx = 1.0, dy = 1.0;
	MelderColour colour = Graphics_inqColour (graphics);
	fixRows (me, & rowmin, & rowmax);
	fixColumns (me, & colmin, & colmax);

	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, colmin - 0.5, colmax + 0.5, rowmin - 0.5, rowmax + 0.5);
	//const double datamax = NUMabsoluteExtremum (my data); TODO
	double datamax = my data [rowmin] [colmin];
	for (integer irow = 1; irow <= my numberOfRows; irow ++)
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			if (fabs (my data [irow] [icol]) > datamax)
				datamax = fabs (my data [irow] [icol]);

	for (integer irow = rowmin; irow <= rowmax; irow ++) {
		double y = rowmax + rowmin - irow;
		for (integer icol = colmin; icol <= colmax; icol ++) {
			const double x = icol;
			/* two neighbouring squares should not touch -> 0.95 */
			const double d = 0.95 * sqrt (fabs (my data [irow] [icol]) / datamax);
			const double x1WC = x - d * dx / 2.0, x2WC = x + d * dx / 2.0;
			const double y1WC = y - d * dy / 2.0, y2WC = y + d * dy / 2.0;
			if (my data [irow] [icol] > 0.0)
				Graphics_setColour (graphics, Melder_WHITE);
			Graphics_fillRectangle (graphics, x1WC, x2WC, y1WC, y2WC);
			Graphics_setColour (graphics, colour);
			Graphics_rectangle (graphics, x1WC, x2WC , y1WC, y2WC);
		}
	}
	Graphics_setGrey (graphics, 0.0);
	Graphics_unsetInner (graphics);
	if (garnish) {
		for (integer irow = rowmin; irow <= rowmax; irow ++) if (my rowLabels [irow])
			Graphics_markLeft (graphics, rowmax + rowmin - irow, false, false, false, my rowLabels [irow].get());
		for (integer icol = colmin; icol <= colmax; icol ++) if (my columnLabels [icol])
			Graphics_markTop (graphics, icol, false, false, false, my columnLabels [icol].get());
	}
}

autoTableOfReal TablesOfReal_append (TableOfReal me, TableOfReal thee) {
	try {
		if (thy numberOfColumns != my numberOfColumns)
			Melder_throw (U"Numbers of columns are ", my numberOfColumns, U" and ", thy numberOfColumns, U" but should be equal.");
		autoTableOfReal him = Thing_newFromClass (my classInfo).static_cast_move <structTableOfReal> ();
		TableOfReal_init (him.get(), my numberOfRows + thy numberOfRows, my numberOfColumns);
		/* Unsafe: new attributes not initialized. */
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			TableOfReal_setColumnLabel (him.get(), icol, my columnLabels [icol].get());
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			TableOfReal_setRowLabel (him.get(), irow, my rowLabels [irow].get());
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				his data [irow] [icol] = my data [irow] [icol];
		}
		for (integer irow = 1; irow <= thy numberOfRows; irow ++) {
			integer hisRow = irow + my numberOfRows;
			TableOfReal_setRowLabel (him.get(), hisRow, thy rowLabels [irow].get());
			for (integer icol = 1; icol <= my numberOfColumns; icol ++)
				his data [hisRow] [icol] = thy data [irow] [icol];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal objects not appended.");
	}
}

autoTableOfReal TablesOfReal_appendMany (OrderedOf<structTableOfReal>* me) {
	try {
		if (my size == 0)
			Melder_throw (U"Cannot add zero tables.");
		TableOfReal thee = my at [1];
		integer totalNumberOfRows = thy numberOfRows;
		integer numberOfColumns = thy numberOfColumns;
		for (integer itab = 2; itab <= my size; itab ++) {
			thee = my at [itab];
			totalNumberOfRows += thy numberOfRows;
			if (thy numberOfColumns != numberOfColumns)
				Melder_throw (U"Numbers of columns do not match.");
		}
		autoTableOfReal him = Thing_newFromClass (thy classInfo).static_cast_move <structTableOfReal>();
		TableOfReal_init (him.get(), totalNumberOfRows, numberOfColumns);
		/* Unsafe: new attributes not initialized. */
		for (integer icol = 1; icol <= numberOfColumns; icol ++)
			TableOfReal_setColumnLabel (him.get(), icol, thy columnLabels [icol].get());
		totalNumberOfRows = 0;
		for (integer itab = 1; itab <= my size; itab ++) {
			thee = my at [itab];
			for (integer irow = 1; irow <= thy numberOfRows; irow ++) {
				totalNumberOfRows ++;
				TableOfReal_setRowLabel (him.get(), totalNumberOfRows, thy rowLabels [irow].get());
				for (integer icol = 1; icol <= numberOfColumns; icol ++)
					his data [totalNumberOfRows] [icol] = thy data [irow] [icol];
			}
		}
		Melder_assert (totalNumberOfRows == his numberOfRows);
		return him;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal objects not appended.");
	}
}

static void TableOfReal_sort (TableOfReal me, bool useLabels, integer column1, integer column2) {
	for (integer irow = 1; irow < my numberOfRows; irow ++) for (integer jrow = irow + 1; jrow <= my numberOfRows; jrow ++) {
		if (useLabels) {
			if (my rowLabels [irow]) {
				if (my rowLabels [jrow]) {
					int compare = str32cmp (my rowLabels [irow].get(), my rowLabels [jrow].get());
					if (compare < 0)
						continue;
					if (compare > 0)
						goto swap;
				} else {
					goto swap;
				}
			} else if (my rowLabels [jrow]) continue;
		}
		/*
		 * If we arrive here, the two labels are equal or both null (or useLabels is `false`).
		 */
		if (column1 > 0 && column1 <= my numberOfColumns) {
			if (my data [irow] [column1] < my data [jrow] [column1])
				continue;
			if (my data [irow] [column1] > my data [jrow] [column1])
				goto swap;
		}
		if (column2 > 0 && column2 <= my numberOfColumns) {
			if (my data [irow] [column2] < my data [jrow] [column2])
				continue;
			if (my data [irow] [column2] > my data [jrow] [column2])
				goto swap;
		}
		/*
		 * If we arrive here, everything is equal.
		 */
		continue;
	swap:
		std::swap (my rowLabels [irow], my rowLabels [jrow]);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			double tmpValue = my data [irow] [icol];
			my data [irow] [icol] = my data [jrow] [icol];
			my data [jrow] [icol] = tmpValue;
		}
	}
}

void TableOfReal_sortByLabel (TableOfReal me, integer column1, integer column2) {
	TableOfReal_sort (me, true, column1, column2);
}

void TableOfReal_sortByColumn (TableOfReal me, integer column1, integer column2) {
	TableOfReal_sort (me, false, column1, column2);
}

autoTableOfReal Table_to_TableOfReal (Table me, integer labelColumn) {
	try {
		if (labelColumn < 1 || labelColumn > my numberOfColumns)
			labelColumn = 0;
		autoTableOfReal thee = TableOfReal_create (my rows.size, labelColumn ? my numberOfColumns - 1 : my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			Table_numericize_a (me, icol);
		if (labelColumn) {
			for (integer icol = 1; icol < labelColumn; icol ++)
				TableOfReal_setColumnLabel (thee.get(), icol, my columnHeaders [icol]. label.get());
			for (integer icol = labelColumn + 1; icol <= my numberOfColumns; icol ++)
				TableOfReal_setColumnLabel (thee.get(), icol - 1, my columnHeaders [icol]. label.get());
			for (integer irow = 1; irow <= my rows.size; irow ++) {
				TableRow row = my rows.at [irow];
				char32 *string = row -> cells [labelColumn]. string.get();
				TableOfReal_setRowLabel (thee.get(), irow, string ? string : U"");
				for (integer icol = 1; icol < labelColumn; icol ++)
					thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
							//thy data [irow] [icol] = Table_getNumericValue_a (me, irow, icol);
				for (integer icol = labelColumn + 1; icol <= my numberOfColumns; icol ++)
					thy data [irow] [icol - 1] = row -> cells [icol]. number;   // Optimization.
							//thy data [irow] [icol - 1] = Table_getNumericValue_a (me, irow, icol);
			}
		} else {
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				TableOfReal_setColumnLabel (thee.get(), icol, my columnHeaders [icol]. label.get());
			}
			for (integer irow = 1; irow <= my rows.size; irow ++) {
				TableRow row = my rows.at [irow];
				for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
					thy data [irow] [icol] = row -> cells [icol]. number;   // Optimization.
					//thy data [irow] [icol] = Table_getNumericValue_a (me, irow, icol);
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

autoTable TableOfReal_to_Table (TableOfReal me, conststring32 nameOfFirstColumn) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my numberOfRows, my numberOfColumns + 1);
		Table_renameColumn_e (thee.get(), 1, nameOfFirstColumn);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			conststring32 columnLabel = my columnLabels [icol].get();
			thy columnHeaders [icol + 1]. label = Melder_dup (columnLabel && columnLabel [0] ? columnLabel : U"?");
		}
		for (integer irow = 1; irow <= thy rows.size; irow ++) {
			conststring32 stringValue = my rowLabels [irow].get();
			const TableRow row = thy rows.at [irow];
			row -> cells [1]. string = Melder_dup (stringValue && stringValue [0] ? stringValue : U"?");
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				const double numericValue = my data [irow] [icol];
				row -> cells [icol + 1]. string = Melder_dup (Melder_double (numericValue));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

void TableOfReal_writeToHeaderlessSpreadsheetFile (TableOfReal me, MelderFile file) {
	try {
		autoMelderString buffer;
		MelderString_copy (& buffer, U"rowLabel");
		for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
			MelderString_appendCharacter (& buffer, U'\t');
			conststring32 s = my columnLabels [icol].get();
			MelderString_append (& buffer, ( s && s [0] != U'\0' ? s : U"?" ));
		}
		MelderString_appendCharacter (& buffer, U'\n');
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			conststring32 s = my rowLabels [irow].get();
			MelderString_append (& buffer, ( s && s [0] != U'\0' ? s : U"?" ));
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				MelderString_appendCharacter (& buffer, U'\t');
				double x = my data [irow] [icol];
				MelderString_append (& buffer, x);
			}
			MelderString_appendCharacter (& buffer, U'\n');
		}
		MelderFile_writeText (file, buffer.string, Melder_getOutputEncoding ());
	} catch (MelderError) {
		Melder_throw (me, U": not saved to tab-separated file.");
	}
}

autoTableOfReal TableOfReal_readFromHeaderlessSpreadsheetFile (MelderFile file) {
	try {
		autostring32 string = MelderFile_readText (file);

		/*
			Count columns.
		*/
		integer ncol = 0;
		char32 *p = & string [0];
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\n' || kar == U'\0')   // TODO: Unicode
				break;
			if (kar == U' ' || kar == U'\t')
				continue;
			ncol ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\n' || kar == U'\0')
				break;
		}
		ncol --;
		if (ncol < 1)
			Melder_throw (U"No columns.");

		/*
			Count elements.
		*/
		p = & string [0];
		integer nelements = 0;
		for (;;) {
			char32 kar = *p++;
			if (kar == U'\0')
				break;
			if (kar == U' ' || kar == U'\t' || kar == U'\n')
				continue;
			nelements ++;
			do { kar = *p++; } while (kar != U' ' && kar != U'\t' && kar != U'\n' && kar != U'\0');
			if (kar == U'\0')
				break;
		}

		/*
			Check if all columns are complete.
		*/
		if (nelements == 0 || nelements % (ncol + 1) != 0)
			Melder_throw (U"The number of elements (", nelements, U") is not a multiple of the number of columns plus 1 (", ncol + 1, U").");

		/*
			Create empty table.
		*/
		integer nrow = nelements / (ncol + 1) - 1;
		autoTableOfReal me = TableOfReal_create (nrow, ncol);

		/*
			Read elements.
		*/
		autoMelderString buffer;
		p = & string [0];
		while (*p == U' ' || *p == U'\t') { Melder_assert (*p != U'\0'); p ++; }
		while (*p != U' ' && *p != U'\t') { Melder_assert (*p != U'\0'); p ++; }   // ignore the header of the zeroth column ("rowLabel" perhaps)
		for (integer icol = 1; icol <= ncol; icol ++) {
			while (*p == U' ' || *p == U'\t') { Melder_assert (*p != U'\0'); p ++; }
			MelderString_empty (& buffer);
			while (*p != U' ' && *p != U'\t' && *p != U'\n') {
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			TableOfReal_setColumnLabel (me.get(), icol, buffer.string);
		}
		for (integer irow = 1; irow <= nrow; irow ++) {
			while (*p == U' ' || *p == U'\t' || *p == U'\n') { Melder_assert (*p != U'\0'); p ++; }
			MelderString_empty (& buffer);
			while (*p != U' ' && *p != U'\t') {
				MelderString_appendCharacter (& buffer, *p);
				p ++;
			}
			TableOfReal_setRowLabel (me.get(), irow, buffer.string);
			for (integer icol = 1; icol <= ncol; icol ++) {
				while (*p == U' ' || *p == U'\t' || *p == U'\n') { Melder_assert (*p != U'\0'); p ++; }
				MelderString_empty (& buffer);
				while (*p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\0') {
					MelderString_appendCharacter (& buffer, *p);
					p ++;
				}
				my data [irow] [icol] = Melder_atof (buffer.string);   // if cell contains a string, this will be 0
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"TableOfReal: tab-separated file ", file, U" not read.");
	}
}

/* End of file TableOfReal.cpp */
