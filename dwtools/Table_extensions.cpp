/* Table_extensions.cpp
 *
 * Copyright (C) 1997-2023 David Weenink, 2011-2021,2023,2024,2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

/*
  djmw 20020619 GPL header
  djmw 20040113 Added comment header for Peterson& Barney data.
  djmw 20040512 Corrected Peterson& Barney ARPABET-labeling.
  djmw 20041213 Added Table_createFromWeeninkData.
  djmw 20080125 Corrected mislabeling of vowels in the Peterson& Barney dataset according to Watrous
  djmw 20080508 Labeling back to original PB article.
  djmw 20110329 Table_get(Numeric|String)Value is now Table_get(Numeric|String)Value_Assert
  djmw 20131219 Improved Table_scatterPlotWithConfidenceIntervals
*/
/*	speaker type (m|w|c), sex(m|f), id, vowel_number, vowel_label
	F0, F1, F2, F3
*/

#include "Discriminant.h"
#include "Formula.h"
#include "GraphicsP.h"
#include "Graphics_extensions.h"
#include "Index.h"
#include "Matrix_extensions.h"
#include "NUM2.h"
#include "Strings_extensions.h"
#include "Covariance.h"
#include "Table_extensions.h"

autoTable FileInMemorySet_downto_Table (FileInMemorySet me, bool openFilesOnly) {
	try {
		const integer numberOfRows = my size;   // FIXME: try open files only
		const conststring32 columnNames [] = { U"path", U"size", U"position" };
		autoTable thee = Table_createWithColumnNames (numberOfRows, ARRAY_TO_STRVEC (columnNames));
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			const FileInMemory fim = my at [irow];   // FIXME: try open files only
			Table_setStringValue (thee.get(), irow, 1, fim -> string.get());
			Table_setNumericValue (thee.get(), irow, 2, fim -> d_numberOfBytes);
			Table_setNumericValue (thee.get(), irow, 3, fim -> d_position);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table created.");
	}
}

static bool intervalsIntersect (double x1, double x2, double xmin, double xmax, double *out_xc1, double *out_xc2) {
	if (x1 > x2)
		std::swap (x1, x2);
	if (xmin > xmax)
		std::swap (xmin, xmax);
	double xc1 = x1;
	double xc2 = x2;
	bool intersect = true;
	if (x2 <= xmin || x1 >= xmax)
		intersect = false;
	else {
		if (x1 < xmin)
			xc1 = xmin;
		if (x2 > xmax)
			xc2 = xmax;
	}
	if (out_xc1)
		*out_xc1 = xc1;
	if (out_xc2)
		*out_xc2 = xc2;
	return intersect;
}

void Table_horizontalErrorBarsPlot (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax,
	double ymin, double ymax, integer xci_min, integer xci_max, double bar_mm, bool garnish)
{
	try {
		const integer nrows = my rows.size;
		if (xcolumn < 1 || xcolumn > nrows || ycolumn < 1 || ycolumn > nrows ||
			(xci_min != 0 && xci_min > nrows) || (xci_max != 0 && xci_max > nrows))
				return;
		if (ymin >= ymax) {
			Table_getExtrema (me, ycolumn, & ymin, & ymax);
			if (ymin >= ymax) {
				ymin -= 1.0;
				ymax += 1.0;
			}
		}
		double x1min, x1max;
		if (xmin >= xmax) {
			Table_getExtrema (me, xcolumn, & xmin, & xmax);
			if (xci_min > 0) {
				Table_getExtrema (me, xci_min, & x1min, & x1max);
				xmin -= x1max;
			}
			if (xci_max > 0) {
				Table_getExtrema (me, xci_max, & x1min, & x1max);
				xmax += x1max;
			}
			if (xmin >= xmax) {
				xmin -= 1.0;
				xmax += 1.0;
			}
		}
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		double dy = Graphics_dyMMtoWC (g, bar_mm);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const double x = Table_getNumericValue_a (me, irow, xcolumn);
			const double y = Table_getNumericValue_a (me, irow, ycolumn);
			const double dx1 =
				xci_min > 0 ? Table_getNumericValue_a (me, irow, xci_min) : 0.0;
			const double dx2 =
				xci_max > 0 ? Table_getNumericValue_a (me, irow, xci_max) : 0.0;
			const double x1 = x - dx1, x2 = x + dx2;

			if (x <= xmax && x >= xmin && y <= ymax && y >= ymin) {
				double xc1, yc1, xc2, yc2;
				if (intervalsIntersect (x1, x2, xmin, xmax, & xc1, & xc2)) {
					Graphics_line (g, xc1, y, xc2, y);
					if (dy > 0 && intervalsIntersect (y - dy / 2.0, y + dy / 2.0, ymin, ymax, & yc1, & yc2)) {
						if (xc1 >= xmin && dx1 > 0.0)
							Graphics_line (g, xc1, yc1, xc1, yc2);
						if (xc2 <= xmax && dx2 > 0.0)
							Graphics_line (g, xc2, yc1, xc2, yc2);
					}
				}
			}
		}
		Graphics_unsetInner (g);

		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksLeft (g, 2, true, true, false);
			Graphics_marksBottom (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void Table_verticalErrorBarsPlot (Table me, Graphics g,
	integer xcolumn, integer ycolumn, double xmin, double xmax,
	double ymin, double ymax, integer yci_min, integer yci_max,
	double bar_mm, bool garnish)
{
	try {
		const integer nrows = my rows.size;
		if (xcolumn < 1 || xcolumn > nrows || ycolumn < 1 || ycolumn > nrows ||
			(yci_min != 0 && yci_min > nrows) || (yci_max != 0 && yci_max > nrows))
				return;
		if (xmin >= xmax) {
			Table_getExtrema (me, ycolumn, & ymin, & ymax);
			if (xmin >= xmax) {
				xmin -= 1.0;
				xmax += 1.0;
			}
		}
		if (ymin >= ymax) {
			double y1min, y1max;
			Table_getExtrema (me, ycolumn, & ymin, & ymax);
			if (yci_min > 0.0) {
				Table_getExtrema (me, yci_min, & y1min, & y1max);
				ymin -= y1max;
			}
			if (yci_max > 0.0) {
				Table_getExtrema (me, yci_max, & y1min, & y1max);
				ymax += y1max;
			}
			if (ymin >= ymax) {
				ymin -= 1.0;
				ymax += 1.0;
			}
		}
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		const double dx = Graphics_dxMMtoWC (g, bar_mm);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const double x = Table_getNumericValue_a (me, irow, xcolumn);
			const double y = Table_getNumericValue_a (me, irow, ycolumn);
			const double dy1 =
				yci_min > 0 ? Table_getNumericValue_a (me, irow, yci_min) : 0.0;
			const double dy2 =
				yci_max > 0 ? Table_getNumericValue_a (me, irow, yci_max) : 0.0;
			const double y1 = y - dy1, y2 = y + dy2;

			if (x <= xmax && x >= xmin && y <= ymax && y >= ymin) {
				double xc1, yc1, xc2, yc2;
				if (intervalsIntersect (y1, y2, ymin, ymax, & yc1, & yc2)) {
					Graphics_line (g, x, yc1, x, yc2);
					if (dx > 0 && intervalsIntersect (x - dx / 2.0, x + dx / 2.0, xmin, xmax, & xc1, & xc2)) {
						if (yc1 >= ymin && dy1 > 0.0)
							Graphics_line (g, xc1, yc1, xc2, yc1);
						if (yc2 <= ymax && dy2 > 0.0)
							Graphics_line (g, xc1, yc2, xc2, yc2);
					}
				}
			}
		}
		Graphics_unsetInner (g);

		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksLeft (g, 2, true, true, false);
			Graphics_marksBottom (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

double Table_getMedianAbsoluteDeviation (Table me, integer columnNumber) {
	try {
		autoVEC data = Table_getColumnVector (me, columnNumber);
		double mad, location;
		autoVEC workSpace = raw_VEC (data.size);
		NUMmad (data.get(), & location, true, & mad, workSpace.get());
		return mad;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute median absolute deviation of column ", columnNumber, U".");
	}
}

autoVEC Table_getColumnVector (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		Table_numericize_a (me, columnNumber);
		Melder_require (my rows.size > 0,
			U"The table is empty.");
		autoVEC result = raw_VEC (my rows.size);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const TableRow row = my rows.at [irow];
			result [irow] = row -> cells [columnNumber].number;
			Melder_require (isdefined (result [irow]),
				U"The cell in row ", irow, U" of column ", Table_messageColumn (me, columnNumber), U" is undefined.");
		}
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": cannot get column vector.");
	}
}

void Table_reportHuberMStatistics (Table me, integer columnNumber, double k_std, double tol, double *out_location, double *out_scale, integer maximumNumberOfIterations) {
	try {
		autoVEC data = Table_getColumnVector (me, columnNumber);
		double location, scale;
		autoVEC workSpace = raw_VEC (data.size);
		NUMstatistics_huber (data.get(), & location, true, & scale, true, k_std, tol, maximumNumberOfIterations, workSpace.get());
		if (out_location)
			*out_location = location;
		if (out_scale)
			*out_scale = scale;
	} catch (MelderError) {
		Melder_throw (me, U": cannot compute median absolute deviation of column ", columnNumber, U".");
	}
}

autoTable Table_getOneWayKruskalWallis (Table me, integer column, integer factorColumn, double *out_prob, double *out_kruskalWallis, double *out_df) {
	try {
		Melder_require (column > 0 && column <= my numberOfColumns,
			U"Invalid column number.");
		Melder_require (factorColumn > 0 && factorColumn <= my numberOfColumns && factorColumn != column,
			U"Invalid group column number.");

		const integer numberOfData = my rows.size;
		Table_numericize_a (me, column);
		autoVEC data = raw_VEC (numberOfData);
		autoStringsIndex levels = Table_to_StringsIndex_column (me, factorColumn, kStrings_sorting::NUMBER_AWARE);
		const integer numberOfLevels = levels -> classes->size;
		
		Melder_require (numberOfLevels > 1, 
			U"There should be at least two levels.");

		for (integer irow = 1; irow <= numberOfData; irow ++)
			data [irow] = my rows.at [irow] -> cells [column]. number;

		NUMsortTogether (data.get(), levels -> classIndex.get());
		VECrankSorted (data.get());
		/*
			Get correctionfactor for ties (Hays pg. 831)
			W. Hays (1988), Statistics, Fourth edition, Sauders college publishing.
		*/
		longdouble c = 0.0;
		integer jt, j = 1;
		while (j < numberOfData) {
        	for (jt = j + 1; jt <= numberOfData && data [jt] == data [j]; jt ++);
        	const double multiplicity = jt - j;
			if (multiplicity > 1)
				c += multiplicity * (multiplicity *multiplicity - 1.0);
        	j = jt;
		}
		const double tiesCorrection = 1.0 - (double) c / (numberOfData * (numberOfData * numberOfData - 1.0));

		autoINTVEC factorLevelSizes = zero_INTVEC (numberOfLevels);
		autoVEC factorLevelSums = zero_VEC (numberOfLevels);
		autoINTVEC ties = zero_INTVEC (numberOfLevels);
		for (integer i = 1; i <= numberOfData; i ++) {
			const integer index = levels -> classIndex [i];
			factorLevelSizes [index] ++;
			factorLevelSums [index] += data [i];
		}

		longdouble kruskalWallis = 0.0;
		for (integer level = 1; level <= numberOfLevels; level ++) {
			if (factorLevelSizes [level] < 2) {
				SimpleString ss = (SimpleString) levels -> classes->at [level];   // FIXME cast
				Melder_throw (U"Group ", ss -> string.get(), U" has fewer than two cases.");
			}
			/*
				factorLevelMeans * groupMean * factorLevelSizes
			*/
			kruskalWallis += factorLevelSums [level] * factorLevelSums [level] / factorLevelSizes [level];
		}
		kruskalWallis = (12.0 / (numberOfData * (numberOfData + 1.0))) * kruskalWallis - 3.0 * (numberOfData + 1);
		kruskalWallis /= tiesCorrection;
		const double df = numberOfLevels - 1.0;
		if (out_df)
			*out_df = df;
		if (out_kruskalWallis)
			*out_kruskalWallis = (double) kruskalWallis;
		if (out_prob)
			*out_prob = NUMchiSquareQ ((double) kruskalWallis, df);

		const conststring32 columnNames [] = { U"Group(R)", U"Sums(R)", U"Cases" };
		autoTable him = Table_createWithColumnNames (numberOfLevels, ARRAY_TO_STRVEC (columnNames));
		for (integer irow = 1; irow <= numberOfLevels; irow ++) {
			const SimpleString ss = (SimpleString) levels -> classes->at [irow];
			Table_setStringValue  (him.get(), irow, 1, ss -> string.get());
			Table_setNumericValue (him.get(), irow, 2, factorLevelSums [irow]);
			Table_setNumericValue (him.get(), irow, 3, factorLevelSizes [irow]);
		}
		Table_numericize_a (him.get(), 2);
		Table_numericize_a (him.get(), 3);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no one-way Kruskal-Wallis performed.");
	}
}

// Table with Group Means Cases
static void _Table_postHocTukeyHSD (Table me, double sumOfSquaresWithin, double degreesOfFreedomWithin, autoTable *out_meansDiff, autoTable *out_meansDiffProbabilities) {
	try {
		Table_numericize_a (me, 2);
		Table_numericize_a (me, 3);
		const integer numberOfMeans = my rows.size;
		autoVEC means = raw_VEC (numberOfMeans);
		autoVEC cases = raw_VEC (numberOfMeans);
		autoTable meansD = Table_create (numberOfMeans - 1, numberOfMeans);
		for (integer i = 1; i <= numberOfMeans; i ++) {
			const TableRow row = my rows.at [i];
			means [i] = row -> cells [2]. number;
			cases [i] = row -> cells [3]. number;
		}
		for (integer i = 1; i <= numberOfMeans - 1; i ++) {
			Table_setStringValue (meansD.get(), i, 1, my rows.at [i] -> cells [1]. string.get());
			Table_renameColumn_e (meansD.get(), i + 1, my rows.at [i + 1] -> cells [1]. string.get());
		}

		for (integer irow = 1; irow <= numberOfMeans - 1; irow ++) {
			for (integer icol = irow + 1; icol <= numberOfMeans; icol ++) {
				const double dif = fabs (means [irow] - means [icol]);
				Table_setNumericValue (meansD.get(), irow, icol, dif);
			}
		}
		autoTable meansP = Data_copy (meansD.get());
		for (integer irow = 1; irow <= numberOfMeans - 1; irow ++) {
			for (integer icol = irow + 1; icol <= numberOfMeans; icol ++) {
				/*
					Tukey-Kramer correction for unequal sample sizes
				*/
				const double oneOverNstar =  0.5 * (1.0 / cases [icol] + 1.0 / cases [irow]);
				const double s = sqrt (sumOfSquaresWithin * oneOverNstar);
				const double q = fabs (means [irow] - means [icol]) / s;
				const double p = NUMtukeyQ (q, numberOfMeans, degreesOfFreedomWithin, 1);
				Table_setNumericValue (meansP.get(), irow, icol, p);
			}
		}
		if (out_meansDiff)
			*out_meansDiff = meansD.move();
		if (out_meansDiffProbabilities)
			*out_meansDiffProbabilities = meansP.move();
	} catch (MelderError) {
		Melder_throw (me, U": no post-hoc performed.");
	}
}

// expect 6 columns, first text others numeric
void Table_printAsAnovaTable (Table me) {
	autoMelderString s;
	const integer width [7] = { 0, 25, 15, 10, 15, 10, 10 };
	if (my numberOfColumns < 6)
		return;
	MelderInfo_writeLine (
		Melder_padLeft (U"Source", width [1]), U"\t",
		Melder_padLeft (U"SS", width [2]), U"\t",
		Melder_padLeft (U"Df", width [3]), U"\t",
		Melder_padLeft (U"MS", width [4]), U"\t",
		Melder_padLeft (U"F", width [5]), U"\t",
		Melder_padLeft (U"P", width [6])
	);
	for (integer icol = 2; icol <= 6; icol ++)
		Table_numericize_a (me, icol);

	for (integer i = 1; i <= my rows.size; i ++) {
		const TableRow row = my rows.at [i];
		MelderString_copy (& s, Melder_padOrTruncateLeft (row -> cells [1]. string.get(), width [1]), U"\t");
		for (integer j = 2; j <= 6; j ++) {
			const double value = row -> cells [j]. number;
			if (isdefined (value))
				MelderString_append (& s, Melder_padLeft (Melder_single (value), width [j]), j == 6 ? U"" : U"\t");
			else
				MelderString_append (& s, Melder_padLeft (U"", width [j]), j == 6 ? U"" : U"\t");
		}
		MelderInfo_writeLine (s.string);
	}
}

void Table_printAsMeansTable (Table me) {
	autoMelderString s;
	for (integer icol = 2; icol <= my numberOfColumns; icol ++)
		Table_numericize_a (me, icol);

	for (integer j = 1; j <= my numberOfColumns; j ++)
		MelderString_append (& s,
			Melder_padOrTruncateLeft (( my columnHeaders [j]. label ? my columnHeaders [j]. label.get() : U"" ), 10),
			( j == my numberOfColumns ? U"" : U"\t" ));

	MelderInfo_writeLine (s.string);
	for (integer i = 1; i <= my rows.size; i ++) {
		const TableRow row = my rows.at [i];
		MelderString_copy (& s, Melder_padOrTruncateLeft (row -> cells [1]. string.get(), 10), U"\t");
		for (integer j = 2; j <= my numberOfColumns; j ++) {
			const double value = row -> cells [j].number;
			if (isdefined (value))
				MelderString_append (& s, Melder_padLeft (Melder_half (value), 10),
					( j == my numberOfColumns ? U"" : U"\t" ));
			else
				MelderString_append (& s, Melder_padLeft (U"", 10), ( j == my numberOfColumns ? U"" : U"\t" ));
		}
		MelderInfo_writeLine (s.string);
	}
}

/*
	W. Hays (1988), Statistics, Fourth edition, Sauders college publishing.
	Chapter 10.14 outlines 12 steps to calculate the one-way analysis of variance.
*/
autoTable Table_getOneWayAnalysisOfVarianceF (Table me, integer column, integer factorColumn, autoTable *means, autoTable *meansDiff, autoTable *meansDiffProbabilities) {
	try {
		Melder_require (column > 0 && column <= my numberOfColumns,
			U"Invalid column number.");
		Melder_require (factorColumn > 0 && factorColumn <= my numberOfColumns && factorColumn != column,
			U"Invalid group column number.");
		const integer numberOfData = my rows.size;
		Table_numericize_a (me, column);
		autoStringsIndex levels = Table_to_StringsIndex_column (me, factorColumn, kStrings_sorting::NUMBER_AWARE);
		// copy data from Table
		autoVEC data = raw_VEC (numberOfData);
		for (integer irow = 1; irow <= numberOfData; irow ++)
			data [irow] = my rows.at [irow] -> cells [column]. number;
		const integer numberOfLevels = levels -> classes->size;
		Melder_require (numberOfLevels > 1,
			U"There should be at least two levels.");
		autoINTVEC factorLevelSizes = zero_INTVEC (numberOfLevels);
		autoVEC factorLevelMeans = zero_VEC (numberOfLevels);

		const longdouble sumOfSquares = NUMsum2 (data.get());  // step 2
		const longdouble sumOfRawScores = NUMsum (data.get()); // step 3
		for (integer i = 1; i <= numberOfData; i ++) {
			const integer index = levels -> classIndex [i];
			factorLevelSizes [index] ++;
			factorLevelMeans [index] += data [i];
		}

		longdouble c = 0.0; // step 5.
		for (integer j = 1; j <= numberOfLevels; j ++) {
			if (factorLevelSizes [j] < 2) {
				const SimpleString ss = (SimpleString) levels -> classes->at [j];
				Melder_throw (U"Level \"", ss -> string.get(), U"\" has less then two members.");
			}
			c += factorLevelMeans [j] * factorLevelMeans [j] / factorLevelSizes [j]; // order of these two is important!
			factorLevelMeans [j] /= factorLevelSizes [j];
		}
		
		const double sumOfSquares_total = double (sumOfSquares - sumOfRawScores * sumOfRawScores / numberOfData); // step 6
		const double sumOfSquares_between = double (c - sumOfRawScores * sumOfRawScores / numberOfData); // step 7
		const double sumOfSquares_within = double (sumOfSquares - c); // step 8
		const double degreesOfFreedom_within = numberOfData - numberOfLevels;
		const double degreesOfFreedom_between = numberOfLevels - 1;

		const conststring32 anovaColumnNames [] = { U"Source", U"SS", U"Df", U"MS", U"F", U"P" };
		autoTable anova = Table_createWithColumnNames (3, ARRAY_TO_STRVEC (anovaColumnNames));
		const integer col_s = 1, col_ss = 2, col_df = 3, col_ms = 4, col_f = 5, col_p = 6;
		const integer row_b = 1, row_w = 2, row_t = 3;
		Table_setStringValue (anova.get(), row_b, col_s, U"Between");
		Table_setStringValue (anova.get(), row_w, col_s, U"Within");
		Table_setStringValue (anova.get(), row_t, col_s, U"Total");

		Table_setNumericValue (anova.get(), row_b, col_ss, sumOfSquares_between);
		Table_setNumericValue (anova.get(), row_b, col_df, degreesOfFreedom_between);
		const double ms_between = sumOfSquares_between / degreesOfFreedom_between; // step 9
		Table_setNumericValue (anova.get(), row_b, col_ms, ms_between);

		Table_setNumericValue (anova.get(), row_w, col_ss, sumOfSquares_within);
		Table_setNumericValue (anova.get(), row_w, col_df, degreesOfFreedom_within);
		const double ms_within = sumOfSquares_within / degreesOfFreedom_within; // step 10
		Table_setNumericValue (anova.get(), row_w, col_ms, ms_within);
		const double fisherF = ms_between / ms_within; // step 11
		const double probability = NUMfisherQ (fisherF, degreesOfFreedom_between, degreesOfFreedom_within); // step 12

		Table_setNumericValue (anova.get(), row_b, col_f, fisherF);
		Table_setNumericValue (anova.get(), row_b, col_p, probability);

		Table_setNumericValue (anova.get(), row_t, col_ss, sumOfSquares_total);
		Table_setNumericValue (anova.get(), row_t, col_df, degreesOfFreedom_within + degreesOfFreedom_between);

		const conststring32 ameansColumnNames [] = { U"Group", U"Mean", U"Cases" };
		autoTable ameans = Table_createWithColumnNames (numberOfLevels, ARRAY_TO_STRVEC (ameansColumnNames));
		for (integer irow = 1; irow <= numberOfLevels; irow ++) {
			const SimpleString name = (SimpleString) levels -> classes->at [irow];
			Table_setStringValue (ameans.get(), irow, 1, name -> string.get());
			Table_setNumericValue (ameans.get(), irow, 2, factorLevelMeans [irow]);
			Table_setNumericValue (ameans.get(), irow, 3, factorLevelSizes [irow]);
		}
		const integer sortingColumns [] = { 2 };   // sort by column 2
		Table_sortRows_a (ameans.get(), ARRAY_TO_INTVEC (sortingColumns));
		_Table_postHocTukeyHSD (ameans.get(), ms_within, degreesOfFreedom_within, meansDiff, meansDiffProbabilities);
		if (means)
			*means = ameans.move();
		return anova;
	} catch (MelderError) {
		Melder_throw (me, U": no one-way anova performed.");
	}
}

autoTable Table_getTwoWayAnalysisOfVarianceF (Table me, integer column, integer factorColumnA, integer factorColumnB, autoTable *out_means, autoTable *out_levelSizes) {
	try {
		Melder_require (column > 0 && column <= my numberOfColumns,
			U"Invalid column number.");
		Melder_require (factorColumnA > 0 && factorColumnA <= my numberOfColumns && factorColumnA != column,
			U"Invalid A group column number.");
		Melder_require (factorColumnB > 0 && factorColumnB <= my numberOfColumns && factorColumnB != column && factorColumnA != factorColumnB,
			U"Invalid B group column number.");

		const integer numberOfData = my rows.size;
		Table_numericize_a (me, column);
		autoStringsIndex levelsA = Table_to_StringsIndex_column (me, factorColumnA, kStrings_sorting::NUMBER_AWARE);
		autoStringsIndex levelsB = Table_to_StringsIndex_column (me, factorColumnB, kStrings_sorting::NUMBER_AWARE);
		/*
			Copy data from Table
		*/
		autoVEC data = raw_VEC (numberOfData);
		for (integer irow = 1; irow <= numberOfData; irow ++)
			data [irow] = my rows.at [irow] -> cells [column]. number;
		const integer numberOfLevelsA = levelsA -> classes->size;
		const integer numberOfLevelsB = levelsB -> classes->size;
		

		conststring32 label_A = my columnHeaders [factorColumnA]. label.get();
		conststring32 label_B = my columnHeaders [factorColumnB]. label.get();
		Melder_require (numberOfLevelsA > 1,
			U"There should be at least two levels in \"", label_A, U"\".");
		Melder_require (numberOfLevelsB > 1,
			U"There should be at least two levels in \"", label_B, U"\".");

		/*
			Formula's according to A. Khuri (1998), Unweighted sums of squares
			in unbalanced analysis of variance, Journal of Statistical Planning
			and Inference (74): 135--147.

			Model:

			y [i,j,k] = mu + alpha [i] + beta [j] + gamma [i,j] + eps [i,j,k]
			i=1..r, j = 1..s, k=1..n [i,j]
		
			ss(alpha) = nh * s * sum(i=1..r, (ystar [i.]-ystar [..])^2)
			ss(beta)  = nh * r * sum(j=1..s, (ystar [.j]-ystar [..])^2)
			ss(alpha,beta) = nh * sum (i=1..r, sum (j=1..s, (ymean [ij.]-ystar [i.] - ystar [.j] + ystar [..])^2)),

			where

			nh = r * s / sum (i=1..r, sum (j=1..s, 1/n [ij])),
			ymean [ij.] = sum (k=1..n [ij], y [ijk]/n [ij])
			ystar [i.] = sum (j=1..s, ymean [ij.]) / s,
			ystar [.j] = sum (i=1..r, ymean [ij.]) / r,
			ystar [..] = sum (i=1..r, sum (j=1..s, ymean [ij.])) / (r * s)
		 */

		autoMAT factorLevelSizes = zero_MAT (numberOfLevelsA + 1, numberOfLevelsB + 1); // sum + weighted sum
		/*
			Extra column for ystar [i.], extra row for ystar [.j]
		*/
		autoMAT factorLevelMeans = zero_MAT (numberOfLevelsA + 1, numberOfLevelsB + 1); // weighted mean + mean

		for (integer k = 1; k <= numberOfData; k ++) {
			const integer indexA = levelsA -> classIndex [k];
			const integer indexB = levelsB -> classIndex [k];
			factorLevelSizes [indexA] [indexB] ++;
			factorLevelMeans [indexA] [indexB] += data [k];
		}
		/*
			Check for unfilled cells and calculate cell means
		*/
		longdouble nh = 0.0;
		for (integer i = 1; i <= numberOfLevelsA; i ++) {
			for (integer j = 1; j <= numberOfLevelsB; j ++) {
				SimpleString ai = static_cast <SimpleString> (levelsA -> classes -> at [i]);
				SimpleString aj = static_cast <SimpleString> (levelsA -> classes -> at [j]);
				Melder_require (factorLevelSizes [i] [j] > 0,
					U"Level ", ai -> string.get(), U" of ", aj -> string.get(),	U" has no data.");
				factorLevelMeans [i] [j] /= factorLevelSizes [i] [j];
				nh += 1.0 / factorLevelSizes [i] [j];
			}
		}
		nh = numberOfLevelsA * numberOfLevelsB / nh;
		/*
			Row marginals (ystar [i.])
		*/
		longdouble sum = 0.0;
		for (integer i = 1; i <= numberOfLevelsA; i ++) {
			for (integer j = 1; j <= numberOfLevelsB; j ++) {
				factorLevelMeans [i] [numberOfLevelsB + 1] += factorLevelMeans [i] [j];
				sum += factorLevelMeans [i] [j];
				factorLevelSizes [i] [numberOfLevelsB + 1] += factorLevelSizes [i] [j];
			}
			factorLevelMeans [i] [numberOfLevelsB + 1] /= numberOfLevelsB;
		}
		const double mean = double (sum) / (numberOfLevelsA * numberOfLevelsB);
		factorLevelMeans [numberOfLevelsA + 1] [numberOfLevelsB + 1] = mean;
		factorLevelSizes [numberOfLevelsA + 1] [numberOfLevelsB + 1] = numberOfData;
		/*
			Column marginals (ystar [.j])
		*/
		for (integer j = 1; j <= numberOfLevelsB; j ++) {
			for (integer i = 1; i <= numberOfLevelsA; i ++) {
				factorLevelMeans [numberOfLevelsA + 1] [j] += factorLevelMeans [i] [j];
				factorLevelSizes [numberOfLevelsA + 1] [j] += factorLevelSizes [i] [j];
			}
			factorLevelMeans [numberOfLevelsA + 1] [j] /= numberOfLevelsA;
		}
		/*
			The sums of squares
		*/
		longdouble ss_T = 0.0;
		for (integer k = 1; k <= numberOfData; k ++) {
			const double dif = data [k] - mean;
			ss_T += dif * dif;
		}

		longdouble ss_A = 0.0;
		for (integer i = 1; i <= numberOfLevelsA; i ++) {
			const double dif = factorLevelMeans [i] [numberOfLevelsB + 1] - mean;
			ss_A += dif * dif;
		}
		ss_A *= nh * numberOfLevelsB;

		longdouble ss_B = 0.0;
		for (integer j = 1; j <= numberOfLevelsB; j ++) {
			const double dif = factorLevelMeans [numberOfLevelsA + 1] [j] - mean;
			ss_B += dif * dif;
		}
		ss_B *= nh * numberOfLevelsA;

		longdouble ss_AB = 0.0;
		for (integer i = 1; i <= numberOfLevelsA; i ++) {
			for (integer j = 1; j <= numberOfLevelsB; j ++) {
				const double dif = factorLevelMeans [i] [j] - factorLevelMeans [i] [numberOfLevelsB + 1] - factorLevelMeans [numberOfLevelsA + 1] [j] + mean;
				ss_AB += dif * dif;
			}
		}
		ss_AB *= nh;

		double ss_E = double (ss_T - ss_A - ss_B - ss_AB);
		/*
			Are there any replications? If not then the error term is the AB interaction.
		*/
		bool replications = true;
		if (factorLevelSizes [numberOfLevelsA + 1] [1] == numberOfLevelsA)
			replications = false;
		/*
			Construct the means Table (numberOfLevelsA+1)x(numberOfLevelsB + 1 + 1)
		*/
		autoTable ameans = Table_createWithoutColumnNames (numberOfLevelsA + 1, numberOfLevelsB + 1 + 1);
		for (integer k = 2; k <= numberOfLevelsB + 1; k ++) {
			const SimpleString name = (SimpleString) levelsB -> classes->at [k - 1];
			Table_renameColumn_e (ameans.get(), k, name -> string.get());
		}
		Table_renameColumn_e (ameans.get(), numberOfLevelsB + 1 + 1, U"Mean");
		for (integer j = 1; j <= numberOfLevelsA; j ++) {
			const SimpleString name = (SimpleString) levelsA -> classes->at [j];
			Table_setStringValue (ameans.get(), j, 1, name -> string.get());
		}
		Table_setStringValue (ameans.get(), numberOfLevelsA + 1, 1, U"Mean");

		for (integer i = 1; i <= numberOfLevelsA + 1; i ++)
			for (integer j = 1; j <= numberOfLevelsB + 1; j ++)
				Table_setNumericValue (ameans.get(), i, j + 1, factorLevelMeans [i] [j]);


		if (out_levelSizes) {
			autoTable asizes = Data_copy (ameans.get());
			Table_renameColumn_e (asizes.get(), numberOfLevelsB + 1 + 1, U"Total");
			Table_setStringValue (asizes.get(), numberOfLevelsA + 1, 1, U"Total");
			for (integer i = 1; i <= numberOfLevelsA + 1; i ++)
				for (integer j = 1; j <= numberOfLevelsB + 1; j ++)
					Table_setNumericValue (asizes.get(), i, j + 1, factorLevelSizes [i] [j]);
			*out_levelSizes = asizes.move();
		}

		const conststring32 anovaColumnNames [] = { U"Source", U"SS", U"Df", U"MS", U"F", U"P" };
		autoTable anova = Table_createWithColumnNames (replications ? 5 : 4, ARRAY_TO_STRVEC (anovaColumnNames));
		const integer col_s = 1, col_ss = 2, col_df = 3, col_ms = 4, col_f = 5, col_p = 6;
		const integer row_A = 1, row_B = 2, row_AB = 3, row_E = replications ? 4 : 3, row_t = replications ? 5 : 4;
		Table_setStringValue (anova.get(), row_A, col_s, label_A);
		Table_setStringValue (anova.get(), row_B, col_s, label_B);
		Table_setStringValue (anova.get(), row_AB, col_s, Melder_cat (label_A, U" x ", label_B));
		if (replications)
			Table_setStringValue (anova.get(), row_E, col_s, U"Error");

		Table_setStringValue (anova.get(), row_t, col_s, U"Total");

		double dof_A = numberOfLevelsA - 1, ms_A = ss_A / dof_A;
		Table_setNumericValue (anova.get(), row_A, col_ss, ss_A);
		Table_setNumericValue (anova.get(), row_A, col_df, dof_A);
		Table_setNumericValue (anova.get(), row_A, col_ms, ms_A);

		double dof_B = numberOfLevelsB - 1, ms_B = ss_B / dof_B;
		Table_setNumericValue (anova.get(), row_B, col_ss, ss_B);
		Table_setNumericValue (anova.get(), row_B, col_df, dof_B);
		Table_setNumericValue (anova.get(), row_B, col_ms, ms_B);

		const double dof_AB = dof_A * dof_B;
		double dof_E, ms_E, ms_AB = 0.0;
		if (replications) {
			ms_AB = ss_AB / dof_AB;
			dof_E = numberOfData - dof_A - dof_B - dof_AB - 1;
			ms_E = ss_E / dof_E;
			Table_setNumericValue (anova.get(), row_AB, col_ss, ss_AB);
			Table_setNumericValue (anova.get(), row_AB, col_df, dof_AB);
			Table_setNumericValue (anova.get(), row_AB, col_ms, ms_AB);
		} else {
			ss_E = ss_AB;
			dof_E = numberOfData - dof_A - dof_B - 1;
			ms_E = ss_AB / dof_E;
		}
		Table_setNumericValue (anova.get(), row_E, col_ss, ss_E);
		Table_setNumericValue (anova.get(), row_E, col_df, dof_E);
		Table_setNumericValue (anova.get(), row_E, col_ms, ms_E);
		Table_setNumericValue (anova.get(), row_t, col_ss, ss_T);
		Table_setNumericValue (anova.get(), row_t, col_df, numberOfData - 1);
		/*
			Get f and p values wrt ms_E
		*/
		const double f_A = ms_A / ms_E;
		const double f_B = ms_B / ms_E;
		const double p_A = NUMfisherQ (f_A, dof_A, dof_E);
		const double p_B = NUMfisherQ (f_B, dof_B, dof_E);
		Table_setNumericValue (anova.get(), row_A, col_f, f_A);
		Table_setNumericValue (anova.get(), row_B, col_f, f_B);
		Table_setNumericValue (anova.get(), row_A, col_p, p_A);
		Table_setNumericValue (anova.get(), row_B, col_p, p_B);
		if (replications) {
			const double f_AB = ms_AB / ms_E;
			const double p_AB = NUMfisherQ (f_AB, dof_AB, dof_E);
			Table_setNumericValue (anova.get(), row_AB, col_f, f_AB);
			Table_setNumericValue (anova.get(), row_AB, col_p, p_AB);
		}
		if (out_means)
			*out_means = ameans.move();
		return anova;
	} catch (MelderError) {
		Melder_throw (me, U": two-way anova not created.");
	}
}

void Table_normalProbabilityPlot (Table me, Graphics g, integer column, integer numberOfQuantiles,
	double numberOfSigmas, double labelSize, conststring32 label, bool garnish)
{
	try {
		if (column < 1 || column > my numberOfColumns)
			return;
		Table_numericize_a (me, column);
		const integer numberOfData = my rows.size;
		autoVEC data = raw_VEC (numberOfData);
		for (integer irow = 1; irow <= numberOfData; irow ++)
			data [irow] = my rows.at [irow] -> cells [column]. number;

		MelderGaussianStats stats = NUMmeanStdev (data.all());
		double xmin = 100.0, xmax = -xmin, ymin = 1e308, ymax = -ymin;
		if (numberOfSigmas != 0) {
			xmin = -numberOfSigmas;
			xmax =  numberOfSigmas;
			ymin = stats.mean - numberOfSigmas * stats.stdev;
			ymax = stats.mean + numberOfSigmas * stats.stdev;
		}
		sort_e_VEC_inout (data.get());
		numberOfQuantiles = numberOfData < numberOfQuantiles ? numberOfData : numberOfQuantiles;
		autoTableOfReal thee = TableOfReal_create (numberOfQuantiles, 2);
		TableOfReal_setColumnLabel (thee.get(), 1, U"Normal distribution quantiles");
		TableOfReal_setColumnLabel (thee.get(), 2, my columnHeaders [column]. label.get());
		const double un = pow (0.5, 1.0 / numberOfQuantiles);
		for (integer irow = 1; irow <= numberOfQuantiles; irow ++) {
			const double ui = irow == 1 ? 1.0 - un : (irow == numberOfQuantiles ? un : (irow - 0.3175) / (numberOfQuantiles + 0.365));
			const double q = NUMquantile (data.all(), ui);
			const double zq = - NUMinvGaussQ (ui);
			thy data [irow] [1] = zq; // along x
			thy data [irow] [2] = q;  // along y
			if (numberOfSigmas == 0.0) {
				xmin = zq < xmin ? zq : xmin;
				xmax = zq > xmax ? zq : xmax;
				ymin = q < ymin ? q : ymin;
				ymax = q > ymax ? q : ymax;
			}
		}

		TableOfReal_drawScatterPlot (thee.get(), g, 1, 2, 1, numberOfQuantiles, xmin, xmax, ymin, ymax, labelSize, false, label, garnish);

		Graphics_setInner (g);
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, xmin, ymin, xmax, ymax);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_unsetInner (g);

	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g,
	integer dataColumn, integer factorColumn, conststring32 xlevel, conststring32 ylevel, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 plotLabel, bool garnish)
{
	try {
		if (dataColumn < 1 || dataColumn > my numberOfColumns || factorColumn < 1 || factorColumn > my numberOfColumns)
			return;
		Table_numericize_a (me, dataColumn);
		const integer numberOfData = my rows.size;
		autoVEC xdata = raw_VEC (numberOfData);
		autoVEC ydata = raw_VEC (numberOfData);
		integer xnumberOfData = 0, ynumberOfData = 0;
		for (integer irow = 1; irow <= numberOfData; irow ++) {
			char32 *label = my rows.at [irow] -> cells [factorColumn]. string.get();
			const double val = my rows.at [irow] -> cells [dataColumn]. number;
			if (Melder_equ (label, xlevel))
				xdata [ ++ xnumberOfData] = val;
			else if (Melder_equ (label, ylevel))
				ydata [ ++ ynumberOfData] = val;
		}
		if (xnumberOfData == 0 || ynumberOfData == 0)
			return;
		xdata. resize (xnumberOfData);
		ydata. resize (ynumberOfData);
		if (xmin == xmax) {
			NUMextrema_u (xdata.part (1, xnumberOfData), & xmin, & xmax);
			if (xmin == xmax) {
				xmin -= 1.0;
				xmax += 1.0;
			}
		}
		if (ymin == ymax) {
			NUMextrema_u (ydata.part (1, ynumberOfData), & ymin, & ymax);
			if (ymin == ymax) {
				ymin -= 1.0;
				ymax += 1.0;
			}
		}
		if (isundef (xmin) || isundef (xmax) || isundef (ymin) || isundef (ymax))
			return;
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		Graphics_quantileQuantilePlot (g, numberOfQuantiles, xdata.get(), ydata.get(),
			xmin, xmax, ymin, ymax, labelSize, plotLabel);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);

			Graphics_textBottom (g, true, Melder_cat (my columnHeaders [dataColumn]. label.get(), U" (", xlevel, U")"));
			Graphics_marksBottom (g, 2, true, true, false);

			Graphics_textLeft (g, true, Melder_cat (my columnHeaders [dataColumn]. label.get(), U" (", ylevel, U")"));
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_quantileQuantilePlot (Table me, Graphics g, integer xcolumn, integer ycolumn, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 plotLabel, bool garnish)
{
	try {
		if (xcolumn < 1 || xcolumn > my numberOfColumns || ycolumn < 1 || ycolumn > my numberOfColumns)
			return;
		Table_numericize_a (me, xcolumn);
		Table_numericize_a (me, ycolumn);
		const integer numberOfData = my rows.size;
		autoVEC xdata = raw_VEC (numberOfData);
		autoVEC ydata = raw_VEC (numberOfData);
		for (integer irow = 1; irow <= numberOfData; irow ++) {
			xdata [irow] = my rows.at [irow] -> cells [xcolumn]. number;
			ydata [irow] = my rows.at [irow] -> cells [ycolumn]. number;
		}
		if (xmin == xmax) {
			NUMextrema_u (xdata.get(), & xmin, & xmax);
			if (xmin == xmax) {
				xmin -= 1.0;
				xmax += 1.0;
			}
		}
		if (ymin == ymax) {
			NUMextrema_u (ydata.get(), & ymin, & ymax);
			if (ymin == ymax) {
				ymin -= 1.0;
				ymax += 1.0;
			}
		}
		if (isundef (xmin) || isundef (xmax) || isundef (ymin) || isundef (ymax))
			return;
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		Graphics_quantileQuantilePlot (g, numberOfQuantiles, xdata.get(), ydata.get(),
				xmin, xmax, ymin, ymax, labelSize, plotLabel);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			if (my columnHeaders [xcolumn].label)
				Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label.get());

			Graphics_marksBottom (g, 2, true, true, false);
			if (my columnHeaders [ycolumn].label)
				Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label.get());

			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_boxPlots (Table me, Graphics g, integer dataColumn, integer factorColumn, double ymin, double ymax, bool garnish) {
	try {
		if (dataColumn < 1 || dataColumn > my numberOfColumns || factorColumn < 1 || factorColumn > my numberOfColumns)
			return;
		Table_numericize_a (me, dataColumn);
		const integer numberOfData = my rows.size;
		autoStringsIndex si = Table_to_StringsIndex_column (me, factorColumn, kStrings_sorting::NUMBER_AWARE);
		const integer numberOfLevels = si -> classes->size;
		if (ymin == ymax) {
			ymax = Table_getMaximum (me, dataColumn);
			ymin = Table_getMinimum (me, dataColumn);
			if (ymax == ymin) {
				ymax += 1.0;
				ymin -= 1.0;
			}
		}
		Graphics_setWindow (g, 1.0 - 0.5, numberOfLevels + 0.5, ymin, ymax);
		Graphics_setInner (g);
		autoVEC data = raw_VEC (numberOfData);
		for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
			integer numberOfDataInLevel = 0;
			for (integer k = 1; k <= numberOfData; k ++)
				if (si -> classIndex [k] == ilevel)
					data [++ numberOfDataInLevel] = Table_getNumericValue_a (me, k, dataColumn);
			Graphics_boxAndWhiskerPlot (g, data.part (1, numberOfDataInLevel), ilevel, 0.2, 0.35, ymin, ymax);
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				const SimpleString ss = (SimpleString) si -> classes->at [ilevel];
				Graphics_markBottom (g, ilevel, false, true, false, ss -> string.get());
			}
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_boxPlots (Table me, Graphics g, constINTVEC const& dataColumnNumbers, integer factorColumnNumber, double ymin, double ymax,
	bool garnish)
{
	try {
		const integer numberOfSelectedColumns = dataColumnNumbers.size;
		if (numberOfSelectedColumns == 0)
			return;
		const integer numberOfData = my rows.size;
		autoStringsIndex si = Table_to_StringsIndex_column (me, factorColumnNumber, kStrings_sorting::NUMBER_AWARE);
		const integer numberOfLevels = si -> classes->size;
		if (ymin == ymax) {
			ymin = 1e308, ymax = - ymin;
			for (integer icol = 1; icol <= numberOfSelectedColumns; icol ++) {
				double ymini = 1e30, ymaxi = - ymini;
				Table_getExtrema (me, dataColumnNumbers [icol], & ymini, & ymax);
				if (ymaxi > ymax)
					ymax = ymaxi;
				if (ymini < ymin)
					ymin = ymini;
			}
			if (ymax == ymin) {
				ymax += 1.0;
				ymin -= 1.0;
			}
		}
		Graphics_setWindow (g, 1.0 - 0.5, numberOfLevels + 0.5, ymin, ymax);
		Graphics_setInner (g);
		const double boxWidth = 4.0, spaceBetweenBoxesInGroup = 1.0, barWidth = boxWidth / 3.0;
		const double spaceBetweenGroupsdiv2 = 3.0 / 2.0;
		const double widthUnit = 1.0 / (numberOfSelectedColumns * boxWidth + (numberOfSelectedColumns - 1) * spaceBetweenBoxesInGroup + spaceBetweenGroupsdiv2 + spaceBetweenGroupsdiv2);
		autoVEC data = raw_VEC (numberOfData);
		for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
			const double xlevel = ilevel;
			for (integer icol = 1; icol <= numberOfSelectedColumns; icol ++) {
				integer numberOfDataInLevelColumn = 0;
				for (integer irow = 1; irow <= numberOfData; irow ++)
					if (si -> classIndex [irow] == ilevel)
						data [++ numberOfDataInLevelColumn] = Table_getNumericValue_a (me, irow, dataColumnNumbers [icol]);
				if (numberOfDataInLevelColumn > 0) {
					/*
						Determine position
					*/
					const double xc = xlevel - 0.5 + (spaceBetweenGroupsdiv2 + (icol - 1) * (boxWidth + spaceBetweenBoxesInGroup) + boxWidth / 2) * widthUnit;
					Graphics_boxAndWhiskerPlot (g, data.part (1, numberOfDataInLevelColumn), xc, 0.5 * barWidth * widthUnit , 0.5 * boxWidth * widthUnit, ymin, ymax);
				}
			}
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				const SimpleString ss = (SimpleString) si -> classes->at [ilevel];
				Graphics_markBottom (g, ilevel, false, true, false, ss -> string.get());
			}
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_boxPlotsWhere (Table me, Graphics g, constINTVEC const& dataColumnNumbers, integer factorColumnNumber, double ymin, double ymax,
	bool garnish, conststring32 formula, Interpreter interpreter)
{
	try {
		const integer numberOfSelectedColumns = dataColumnNumbers.size;
		if (numberOfSelectedColumns == 0)
			return;
		Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		const integer numberOfData = my rows.size;
		autoStringsIndex si = Table_to_StringsIndex_column (me, factorColumnNumber, kStrings_sorting::NUMBER_AWARE);
		const integer numberOfLevels = si -> classes->size;
		if (ymin == ymax) {
			ymin = 1e308, ymax = - ymin;
			for (integer icol = 1; icol <= numberOfSelectedColumns; icol ++) {
				double ymini = 1e308, ymaxi = - ymini;
				Table_getExtrema (me, dataColumnNumbers [icol], & ymini, & ymaxi);
				if (ymaxi > ymax)
					ymax = ymaxi;
				if (ymini < ymin)
					ymin = ymini;
			}
			if (ymax == ymin) {
				ymax += 1.0;
				ymin -= 1.0;
			}
		}
		Graphics_setWindow (g, 1.0 - 0.5, numberOfLevels + 0.5, ymin, ymax);
		Graphics_setInner (g);
		const double boxWidth = 4.0, spaceBetweenBoxesInGroup = 1.0, barWidth = boxWidth / 3.0;
		const double spaceBetweenGroupsdiv2 = 3.0 / 2.0;
		const double widthUnit = 1.0 / (numberOfSelectedColumns * boxWidth + (numberOfSelectedColumns - 1) * spaceBetweenBoxesInGroup + spaceBetweenGroupsdiv2 + spaceBetweenGroupsdiv2);
		autoVEC data = raw_VEC (numberOfData);
		for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
			const double xlevel = ilevel;
			for (integer icol = 1; icol <= numberOfSelectedColumns; icol ++) {
				integer numberOfDataInLevelColumn = 0;
				for (integer irow = 1; irow <= numberOfData; irow ++) {
					if (si -> classIndex [irow] == ilevel) {
						Formula_run (irow, dataColumnNumbers [icol], & result);
						if (result. numericResult != 0.0)
							data [++ numberOfDataInLevelColumn] = Table_getNumericValue_a (me, irow, dataColumnNumbers [icol]);
					}
				}
				if (numberOfDataInLevelColumn > 0) {
					/*
						Determine position
					*/
					const double xc = xlevel - 0.5 + (spaceBetweenGroupsdiv2 + (icol - 1) * (boxWidth + spaceBetweenBoxesInGroup) + boxWidth / 2) * widthUnit;
					Graphics_boxAndWhiskerPlot (g, data.part (1, numberOfDataInLevelColumn), xc, 0.5 * barWidth * widthUnit , 0.5 * boxWidth * widthUnit, ymin, ymax);
				}
			}
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			for (integer ilevel = 1; ilevel <= numberOfLevels; ilevel ++) {
				const SimpleString ss = (SimpleString) si -> classes->at [ilevel];
				Graphics_markBottom (g, ilevel, false, true, false, ss -> string.get());
			}
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_distributionPlot (Table me, Graphics g, integer dataColumn, double minimum, double maximum, integer nBins, double freqMin, double freqMax, bool garnish) {
	try {
		if (dataColumn < 1 || dataColumn > my numberOfColumns)
			return;
		Table_numericize_a (me, dataColumn);
		integer mrow = 0;
		autoMatrix thee = Matrix_create (1.0, 1.0, 1, 1.0, 1.0, 0.0, my rows.size + 1.0, my rows.size, 1.0, 1.0);
		for (integer irow = 1; irow <= my rows.size; irow ++)
				thy z [1] [++ mrow] = Table_getNumericValue_a (me, irow, dataColumn);
		Matrix_drawDistribution (thee.get(), g, 0, 1, 0.5, mrow + 0.5, minimum, maximum, nBins, freqMin, freqMax, false, false);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			if (my columnHeaders [dataColumn]. label)
				Graphics_textBottom (g, true, my columnHeaders [dataColumn]. label.get());
			Graphics_marksLeft (g, 2, true, true, false);
			Graphics_textLeft (g, true, U"Number / bin");
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

integer Table_getNumberOfRowsWhere (Table me, conststring32 formula, Interpreter interpreter) {
	integer numberOfRows = 0;
	Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
	Formula_Result result;
	for (integer irow = 1; irow <= my rows.size; irow ++) {
		Formula_run (irow, 1, & result);
		if (result. numericResult != 0.0)
			numberOfRows ++;
	}
	return numberOfRows;
}

autoINTVEC Table_listRowNumbersWhere (Table me, conststring32 formula, Interpreter interpreter) {
	try {
		const integer numberOfMatches = Table_getNumberOfRowsWhere (me, formula, interpreter);
		if (numberOfMatches == 0)
			return autoINTVEC();
		Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		autoINTVEC selectedRows = zero_INTVEC (numberOfMatches);
		integer n = 0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			Formula_run (irow, 1, & result);
			if (result. numericResult != 0.0)
				selectedRows [++ n] = irow;
		}
		Melder_assert (n == numberOfMatches);
		return selectedRows;
	} catch (MelderError) {
		Melder_throw (me, U": cannot find matches.");
	}
}

void Table_barPlot (Table me, Graphics g,
	constINTVEC columnNumbers, double ymin, double ymax, integer labelColumnNumber,
	double xoffsetFraction, double interbarFraction, double interbarsFraction, constSTRVEC colours,
	double angle, bool garnish)
{
	try {
		if (ymax <= ymin) {   // autoscaling
			ymin = 1e308;
			ymax = - ymin;
			for (integer icol = 1; icol <= columnNumbers.size; icol ++) {
				double cmin, cmax;
				Table_getExtrema (me, columnNumbers [icol], & cmin, & cmax);
				if (cmin < ymin)
					ymin = cmin;
				if (cmax > ymax)
					ymax = cmax;
			}
			Melder_clipRight (& ymin, 0.0);
			Melder_clipLeft (0.0, & ymax);
		}
		if (ymin == ymax)
			return; // Table still could have equal or zero entries
		Graphics_setInner (g);
		Graphics_setWindow (g, 0, 1, ymin, ymax);

		const integer numberOfGroups = my rows.size;
		const integer groupSize = columnNumbers.size;
		const double bar_width = 1.0 / (numberOfGroups * groupSize + 2.0 * xoffsetFraction + (numberOfGroups - 1) * interbarsFraction + numberOfGroups * (groupSize - 1) * interbarFraction);
		const double dx = (interbarsFraction + groupSize + (groupSize - 1) * interbarFraction) * bar_width;

		for (integer icol = 1; icol <= groupSize; icol ++) {
			const double xb = xoffsetFraction * bar_width + (icol - 1) * (1 + interbarFraction) * bar_width;
			double x1 = xb;
			const integer index = std::min (icol, colours.size);
			MelderColour colour = MelderColour_fromColourNameOrNumberStringOrRGBString (colours [index]);
			if (! colour.valid())
				colour = Melder_GREY;
			for (integer irow = 1; irow <= my rows.size; irow ++) {
				const double x2 = x1 + bar_width;
				const double y2 = Melder_clipped (ymin, Table_getNumericValue_a (me, irow, columnNumbers [icol]), ymax);
				const double y1 = Melder_clippedLeft (0.0, ymin);
				
				Graphics_setColour (g, colour);
				Graphics_fillRectangle (g, x1, x2, y1, y2);
				Graphics_setGrey (g, 0.0);   // black
				Graphics_rectangle (g, x1, x2, y1, y2);

				x1 += dx;
			}
		}

		if (garnish) {
			if (labelColumnNumber > 0) {
				double y = ymin, xb = (xoffsetFraction + 0.5 * (groupSize + (groupSize - 1) * interbarFraction)) * bar_width;
				const double lineSpacing = Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72.0);
				const double currentFontSize = Graphics_inqFontSize (g);
				Graphics_setTextRotation (g, angle);
				if (angle < 0.0) {
					y -= 0.3 * lineSpacing;
					xb -= 0.5 * bar_width;
					Graphics_setFontSize (g, currentFontSize - ( currentFontSize > 12.0 ? 2.0 : 1.0 ));
					Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_TOP);
				} else if (angle > 0.0) {
					y -= 0.3*lineSpacing;
					xb += 0.5 * bar_width;
					Graphics_setFontSize (g, currentFontSize - ( currentFontSize > 12.0 ? 2.0 : 1.0 ));
					Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_TOP);
				} else {
					Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
				}
				for (integer irow = 1; irow <= numberOfGroups; irow ++) {
					conststring32 label = Table_getStringValue_a (me, irow, labelColumnNumber);
					if (label)
						Graphics_text (g, xb, ymin - g -> vertTick, label); // was y
					xb += dx;
				}
				Graphics_setFontSize (g, currentFontSize);
				Graphics_setTextRotation (g, 0);
			}
		}
		Graphics_unsetInner (g);
		if (garnish) {
			if (ymin * ymax < 0.0)
				Graphics_markLeft (g, 0.0, true, true, true, nullptr);
			Graphics_drawInnerBox (g);
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError (); // drawing errors shall be ignored
	}
}

// TODO extra parameter font size
static bool Graphics_getConnectingLine (Graphics g, conststring32 text1, double fontSize, double x1, double y1, conststring32 text2, double x2, double y2, double *x3, double *y3, double *x4, double *y4) {
	bool drawLine = false;
	const double width1 = Graphics_textWidth (g, text1);
	const double width2 = Graphics_textWidth (g, text2);
	const double h = Graphics_dyMMtoWC (g, 1.5 * fontSize * 25.4 / 72.0) / 1.5;
	const double xleft = x1 < x2 ? x1 : x2, xright = x2 > x1 ? x2 : x1;
	double xi [3], yi [3];
	int numberOfIntersections = NUMgetIntersectionsWithRectangle (x1, y1, x2, y2, xleft - width1 / 2.0, y1 - h/2, xleft + width1 / 2.0, y1 + h/2, xi, yi);
	if (numberOfIntersections == 1) {
		*x3 = xi [1];
		*y3 = yi [1];
		numberOfIntersections = NUMgetIntersectionsWithRectangle (x1, y1, x2, y2, xright - width2 / 2.0, y2 - h/2, xright + width2 / 2.0, y2 + h/2, xi, yi);
		if (numberOfIntersections == 1) {
			*x4 = xi [1];
			*y4 = yi [1];
			drawLine = true;
		}
	}
	return drawLine;
}

/*
	Take the xcolumn as labels if non-numeric column else as numbers and arrange distances accordingly.
	TODO We need an extra argument Label size!
*/
void Table_lineGraph (Table me, Graphics g, integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax, conststring32 symbol, double symbolFontSize, double angle, bool garnish) {
	try {
		if (ycolumn < 1 || ycolumn > my numberOfColumns || xcolumn < 0 || xcolumn > my numberOfColumns)
			return;
		if (ymax <= ymin)
			Table_getExtrema (me, ycolumn, & ymin, & ymax);

		bool xIsNumeric = true;
		for (integer irow = 1; irow <= my rows.size; irow ++)
			if (! Table_isCellNumeric_ErrorFalse (me, irow, xcolumn)) {
				xIsNumeric = false;
				break;
			}
		if (xmin >= xmax) {
			if (xIsNumeric)
				Table_getExtrema (me, xcolumn, & xmin, & xmax);
			else {
				xmin = 0.0;
				xmax = my rows.size + 1;
			}
		}
		double currentFontSize = Graphics_inqFontSize (g);
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		const double lineSpacing = Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72.0);
		//double symbolHeight = lineSpacing / 1.5;
		double x1, y1;
		Graphics_setFontSize (g, symbolFontSize);
		autoLineSegmentClipper clipper = LineSegmentClipper_create (xmin, xmax, ymin, ymax);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const double y2 = Table_getNumericValue_a (me, irow, ycolumn);
			double x2 = xIsNumeric ? Table_getNumericValue_a (me, irow, xcolumn) : irow;
			//double symbolWidth = 0;
			if (x2 >= xmin && (x2 <= xmax || x1 < xmax)) {
				if (symbol && y2 >= ymin && y2 <= ymax && x2 <= xmax)
					Graphics_text (g, x2, y2, symbol);
				if (irow > 1) {
					double x3, y3, x4, y4;
					if (Graphics_getConnectingLine (g, symbol, symbolFontSize, x1, y1, symbol, x2, y2, & x3, & y3, & x4, & y4) && 
						clipper -> clip (x3, y3, x4, y4))
						Graphics_line (g, x3, y3, x4, y4);
				}
			} else {
				x2 = x2 < xmin ? xmin : xmax;
			}
			x1 = x2;
			y1 = y2;
		}
		Graphics_setFontSize (g, currentFontSize);
		
		if (garnish && ! xIsNumeric && xcolumn > 0) {
			double y = ymin, dx = 0.0;
			Graphics_setTextRotation (g, angle);
			if (angle < 0.0) {
				y -= 0.3 * lineSpacing;
				dx = - 0.5;
				Graphics_setFontSize (g, currentFontSize - (currentFontSize > 12 ? 2 : 1));
				Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_TOP);
			} else if (angle > 0.0) {
				y -= 0.3 * lineSpacing;
				dx = 0.5;
				Graphics_setFontSize (g, currentFontSize - (currentFontSize > 12 ? 2 : 1));
				Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_TOP);
			} else {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
			}
			for (integer irow = 1; irow <= my rows.size; irow ++) {
				const double x2 = double (irow);
				if (x2 >= xmin && x2 <= xmax) {
					conststring32 label = Table_getStringValue_a (me, irow, xcolumn);
					if (label)
						Graphics_text (g, x2 + dx, ymin - g -> vertTick, label); // was y
				}
			}
			Graphics_setFontSize (g, currentFontSize);
			Graphics_setTextRotation (g, 0);
		}
		Graphics_unsetInner (g);

		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksLeft (g, 2, true, true, false);
			if (xIsNumeric)
				Graphics_marksBottom (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

void Table_lineGraph_old (Table me, Graphics g, integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax, conststring32 symbol, double angle, bool garnish) {
	double fontSize = Graphics_inqFontSize (g);
	Table_lineGraph (me, g, xcolumn, xmin, xmax, ycolumn, ymin, ymax, symbol, fontSize, angle, garnish);
}

void Table_lagPlot (Table me, Graphics g, integer column, integer lag, double xmin, double xmax,
	conststring32 symbol, double labelSize, bool garnish)
{
	try {
		if (column < 1 || column > my rows.size)
			return;
		if (xmax <= xmin)   // autoscaling
			Table_getExtrema (me, column, & xmin, & xmax);
		autoVEC x = raw_VEC (my rows.size);
		for (integer irow = 1; irow <= my rows.size; irow ++)
			x [irow] = Table_getNumericValue_a (me, irow, column);
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, xmin, xmax);
		Graphics_lagPlot (g, x.get(), xmin, xmax, lag, labelSize, symbol);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
			if (my columnHeaders [column].label) {
				Graphics_textLeft (g, true, my columnHeaders [column].label.get());
				Graphics_textBottom (g, true, Melder_cat (my columnHeaders [column]. label.get(), U" (lag = ", lag, U")"));
			}
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

autoTable Table_extractRowsWhere (Table me, conststring32 formula, Interpreter interpreter) {
	try {
		Formula_compile (interpreter, me, formula, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		autoTable thee = Table_create (0, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			thy columnHeaders [icol].label = Melder_dup (my columnHeaders [icol].label.get());
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			Formula_run (irow, 1, & result);
			if (result. numericResult != 0.0) {
				const TableRow row = my rows.at [irow];
				autoTableRow newRow = Data_copy (row);
				thy rows. addItem_move (newRow.move());
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table could be extracted.");
	}
}


autoTable Table_extractRowsWhere_e (Table me, conststring32 formula, Interpreter interpreter) {
	try {
		autoTable result = Table_extractRowsWhere (me, formula, interpreter);
		Melder_require (result -> rows.size > 0,
			U"No row matches criterion.");
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no Table could be extracted.");
	}
}

static autoTableOfReal Table_to_TableOfReal_selectedColumns (Table me, INTVEC const& columnNumbers, integer labelColumnNumber) {
	try {
		Melder_require (columnNumbers.size > 0,
			U"No columns selected.");
		if (labelColumnNumber != 0)
			Melder_require (labelColumnNumber > 0 && labelColumnNumber <= my numberOfColumns,
				U"The label column number (", labelColumnNumber, U") in not valid.");
		autoTableOfReal thee = TableOfReal_create (my rows.size, columnNumbers.size);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			for (integer icol = 1; icol <= columnNumbers.size; icol ++)
				thy data [irow] [icol] = Table_getNumericValue_a (me, irow, columnNumbers [icol]);
			if (labelColumnNumber > 0)
				TableOfReal_setRowLabel (thee.get(), irow, Table_getStringValue_a (me, irow, labelColumnNumber));
		}
		for (integer icol = 1; icol <= columnNumbers.size; icol ++)
			TableOfReal_setColumnLabel (thee.get(), icol, my columnHeaders [columnNumbers [icol]].label.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not create TableOfReal for selected columns.");
	}
}

static integer SSCPList_findIndexOfGroupLabel (SSCPList me, conststring32 label) {
	for (integer i = 1; i <= my size; i ++)
		if (Melder_equ (Thing_getName (my at [i]), label))
			return i;
	return 0;
}

static autoTable Table_SSCPList_extractMahalanobis (Table me, SSCPList thee, kMelder_number which, double numberOfSigmas, integer factorColumnNumber) {
	try {
		const integer numberOfGroups = thy size;
		Melder_assert (numberOfGroups > 0);

		const SSCP sscp = thy at [1];
		const integer numberOfColumns = sscp -> numberOfColumns;  // can be absent
		autoINTVEC columnIndex = raw_INTVEC (numberOfColumns);
		autoVEC vector = raw_VEC (numberOfColumns);
		for (integer icol = 1; icol <= numberOfColumns; icol ++)
			columnIndex [icol] = Table_columnNameToNumber_e (me, sscp -> columnLabels [icol].get()); // throw if not present
		autoTable him = Table_create (0, my numberOfColumns);
		for (integer icol = 1; icol <= my numberOfColumns; icol ++)
			his columnHeaders [icol].label = Melder_dup (my columnHeaders [icol]. label.get());
		OrderedOf<structCovariance> covs;
		for (integer igroup = 1; igroup <= numberOfGroups; igroup ++) {
			autoCovariance cov = SSCP_to_Covariance (thy at [igroup], 1);
			SSCP_expandWithLowerCholeskyInverse (cov.get());
			covs. addItem_move (cov.move());
		}
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			integer igroup = 1;
			if (factorColumnNumber > 0) {// if factorColumnNumber == 0 we don't need labels
				conststring32 label = Table_getStringValue_a (me, irow, factorColumnNumber);
				igroup = SSCPList_findIndexOfGroupLabel (thee, label);
				Melder_require (igroup > 0,
					U"The label \"", label, U"\" in row ", irow, U" is not valid in this context.");
			}
			const Covariance covi = covs.at [igroup];
			for (integer icol = 1; icol <= numberOfColumns; icol ++)
				vector [icol] = Table_getNumericValue_a (me, irow, columnIndex [icol]);
			const double dm2 = NUMmahalanobisDistanceSquared (covi -> lowerCholeskyInverse.get(), vector.get(), covi -> centroid.get());
			if (Melder_numberMatchesCriterion (sqrt (dm2), which, numberOfSigmas)) {
				const TableRow row = my rows.at [irow];
				autoTableRow newRow = Data_copy (row);
				his rows. addItem_move (newRow.move());
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"Table (mahalanobis) not extracted.");
	}
}

autoTable Table_extractMahalanobis (Table me, INTVEC const& columnNumbers,
	kMelder_number which, double numberOfSigmas, integer factorColumnNumber)
{
	try {
		autoTableOfReal thee = Table_to_TableOfReal_selectedColumns (me, columnNumbers, factorColumnNumber);
		autoSSCPList him = TableOfReal_to_SSCPList_byLabel (thee.get());
		autoTable result = Table_SSCPList_extractMahalanobis (me, him.get(), which, numberOfSigmas, factorColumnNumber);
		return result;
	} catch (MelderError) {
		Melder_throw (me, U"Table not extracted.");
	}
}

void Table_drawEllipses (Table me, Graphics g, integer xcolumn, integer ycolumn, integer factorColumn,
	double xmin, double xmax, double ymin, double ymax, double numberOfSigmas, double labelSize, bool garnish)
{
	try {
		autoTableOfReal thee = TableOfReal_create (my rows.size, 2);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			const conststring32 label = Table_getStringValue_a (me, irow, factorColumn);
			TableOfReal_setRowLabel (thee.get(), irow, label);
			thy data [irow] [1] = Table_getNumericValue_a (me, irow, xcolumn);
			thy data [irow] [2] = Table_getNumericValue_a (me, irow, ycolumn);
		}
		autoSSCPList him = TableOfReal_to_SSCPList_byLabel (thee.get());
		constexpr bool confidence = false;
		if (xmax == xmin || ymax == ymin) {  // autoscaling
			double xmin_as, xmax_as, ymin_as, ymax_as;
			SSCPList_getEllipsesBoundingBoxCoordinates (him.get(), numberOfSigmas, confidence, & xmin_as, & xmax_as, & ymin_as, & ymax_as);
			if (xmax == xmin) {
				xmax = xmax_as;
				xmin = xmin_as;
			}
			if (ymax == ymin) {
				ymax = ymax_as;
				ymin = ymin_as;
			}
		}
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		Graphics_setInner (g);
		for (integer i = 1; i <= his size; i ++) {
			const SSCP sscpi = his at [i];
			const double scalei = SSCP_getEllipseScalefactor (sscpi, numberOfSigmas, confidence);
			if (scalei > 0.0)
				SSCP_drawTwoDimensionalEllipse_inside (sscpi, g, scalei, Thing_getName (sscpi), labelSize);
		}
		Graphics_unsetInner (g);

		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
			if (my columnHeaders [xcolumn]. label)
				Graphics_textBottom (g, true, my columnHeaders [xcolumn]. label.get());
			if (my columnHeaders [ycolumn]. label)
				Graphics_textLeft (g, true, my columnHeaders [ycolumn]. label.get());
		}
	} catch (MelderError) {
		Melder_clearError ();   // drawing errors shall be ignored
	}
}

autoTable Table_extractColumnsByNumber (Table me, constINTVECVU const& columnNumbers) {
	try {
		Table_checkSpecifiedColumnNumbersWithinRange (me, columnNumbers);
		const integer numberOfRows = my rows.size;
		autoTable thee = Table_createWithoutColumnNames (numberOfRows, columnNumbers.size);
		for (integer icol = 1; icol <= columnNumbers.size; icol ++)
			Table_renameColumn_e (thee.get(), icol, my v_getColStr (columnNumbers [icol]));
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			for (integer icol = 1; icol <= columnNumbers.size; icol ++) {
				const conststring32 value = Table_getStringValue_a (me, irow, columnNumbers [icol]);
				Table_setStringValue (thee.get(), irow, icol, value);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no column range extracted.");
	}
}

/* End of file Table_extensions.cpp */
