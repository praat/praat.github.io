#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
 *
 * Copyright (C) 1993-2026 David Weenink
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
 djmw 20020411 initial GPL
 djmw 20131220 Latest modification.
*/

/*
	TODO : 15/8/23 harmonize all arguments columnnames + columnnumbers
 */
#include "TableOfReal.h"
#include "Collection.h"
#include "Categories.h"
#include "FileInMemory.h"
#include "Strings_.h"
#include "SSCP.h"
#include "Table.h"

autoTable Table_extractRowsWhere (Table me, conststring32 formula, Interpreter interpreter);
autoTable Table_extractRowsWhere_e (Table me, conststring32 formula, Interpreter interpreter);

autoTable Table_extractColumnsByNumber (Table me, constINTVECVU const& columnNumbers);

integer Table_getNumberOfRowsWhere (Table me, conststring32 formula, Interpreter interpreter);

autoINTVEC Table_listRowNumbersWhere (Table me, conststring32 formula, Interpreter interpreter);

autoVEC Table_getColumnVector (Table me, integer columnNumber);

autoTable Table_create_petersonBarney1952 ();

autoTable Table_create_hillenbrandEtAl1995 ();

autoTable Table_create_polsVanNierop1973 ();

autoTable Table_create_weenink1983 ();

autoTable Table_create_keatingEsposito2006 ();

autoTable Table_create_ganong1980 ();

autoTable FileInMemorySet_downto_Table (FileInMemorySet me, bool openFilesOnly);

double Table_getMedianAbsoluteDeviation (Table me, integer columnNumber);

void Table_reportHuberMStatistics (Table me, integer columnNumber, double k_std, double tol, double *out_location, double *out_scale, integer maximumNumberOfiterations);

// Two one-way tests for normal and non-normally distributed data, respectively.
autoTable Table_getOneWayAnalysisOfVarianceF (Table me,
	integer column, integer groupColumn, autoTable *means, autoTable *meansDiff, autoTable *meansDiffProbabilities
);

autoTable Table_getOneWayKruskalWallis (Table me,
	integer column, integer factorColumn, double *p_prob, double *p_kruskalWallis, double *p_df
);

autoTable Table_getTwoWayAnalysisOfVarianceF (Table me,
	integer column, integer groupColumnA, integer groupColumnB, autoTable *means, autoTable *factorLevelSizes
);

void Table_verticalErrorBarsPlot (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer yci_min, integer yci_max, double bar_mm, bool garnish
);

void Table_horizontalErrorBarsPlot (Table me, Graphics g, integer xcolumn, integer ycolumn, double xmin, double xmax, 
	double ymin, double ymax, integer xci_min, integer xci_max, double bar_mm, bool garnish
);

void Table_normalProbabilityPlot (Table me, Graphics g,
	integer column, integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish
);

void Table_quantileQuantilePlot (Table me, Graphics g,
	integer xcolumn, integer ycolumn, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 label, bool garnish
);

void Table_quantileQuantilePlot_betweenLevels (Table me, Graphics g,
	integer dataColumn, integer factorColumn, conststring32 xlevel, conststring32 ylevel, integer numberOfQuantiles,
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 label, bool garnish
);

void Table_boxPlots (Table me, Graphics g,
	integer dataColumn, integer factorColumn, double ymin, double ymax, bool garnish);

void Table_boxPlots (Table me, Graphics g,
	constINTVEC const& dataColumnNumbers, integer factorColumn,
	double ymin, double ymax, bool garnish
);

void Table_distributionPlot (Table me, Graphics g,
	integer dataColumn, double minimum, double maximum, integer nBins,
	double freqMin, double freqMax, bool garnish
);

void Table_barPlot (Table me, Graphics g,
	constINTVEC columnNumbers, double ymin, double ymax, integer labelColumnNumber,
	double xoffsetFraction, double interbarFraction, double interbarsFraction, constSTRVEC colours,
	double angle, bool garnish
);

void Table_lineGraph (Table me, Graphics g,
	integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax,
	conststring32 symbol, double symbolFontSize, double angle, bool garnish
);

void Table_lineGraph_old (Table me, Graphics g,
	integer xcolumn, double xmin, double xmax, integer ycolumn, double ymin, double ymax,
	conststring32 symbol, double angle, bool garnish
);

void Table_lagPlot (Table me, Graphics g,
	integer column, integer lag, double xmin, double xmax,
	conststring32 symbol, double labelSize, bool garnish
);

void Table_drawEllipses (Table me, Graphics g,
	integer xcolumn, integer ycolumn, integer labelcolumn, double xmin, double xmax, double ymin, double ymax,
	double numberOfSigmas, double labelSize, bool garnish
);

autoTable Table_extractMahalanobis (Table me, INTVEC const& columnNumbers,
	kMelder_number which, double numberOfSigmas, integer factorColumn);

void Table_printAsAnovaTable (Table me);

void Table_printAsMeansTable (Table me);

autoTable Table_readFromXLSXFile (MelderFile file);

#endif // _Table_extensions_h_
