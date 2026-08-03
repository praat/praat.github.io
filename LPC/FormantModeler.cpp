/* FormantModeler.cpp
 *
 * Copyright (C) 2014-2024 David Weenink
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
 * ainteger with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DataModeler.h"
#include "FormantModeler.h"
#include "Formant_extensions.h"
#include "NUM2.h"
#include "NUMmachar.h"
#include "SVD.h"
#include "Strings_extensions.h"
#include "Sound_to_Formant_mt.h"
#include "Table_extensions.h"

#include "oo_DESTROY.h"
#include "FormantModeler_def.h"
#include "oo_COPY.h"
#include "FormantModeler_def.h"
#include "oo_EQUAL.h"
#include "FormantModeler_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantModeler_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantModeler_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantModeler_def.h"
#include "oo_READ_TEXT.h"
#include "FormantModeler_def.h"
#include "oo_READ_BINARY.h"
#include "FormantModeler_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantModeler_def.h"

#include "enums_getText.h"
#include "FormantModeler_enums.h"
#include "enums_getValue.h"
#include "FormantModeler_enums.h"

Thing_implement (FormantModeler, Function, 0);

void structFormantModeler :: v1_info () {
	// skipping parent classes
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	for (integer iformant = 1; iformant <= trackmodelers.size; iformant ++) {
		DataModeler ffi = trackmodelers.at [iformant];
		MelderInfo_writeLine (U"Formant ", iformant);
		ffi -> v1_info();
	}
}

static void checkTrackAutoRange (FormantModeler me, integer *fromTrack, integer *toTrack) {
	if (*fromTrack == 0 && *toTrack == 0) { // auto
		*fromTrack = 1;
		*toTrack = my trackmodelers.size;
		return;
	}
	if (*toTrack == 0)
		*toTrack = my trackmodelers.size; // auto
	Melder_require (*fromTrack <= *toTrack,
		U"\"FromTrack\" should not exceed \"toTrack\".");
	if (*toTrack > my trackmodelers.size) // questionable because 0 is an alternative
		*toTrack = my trackmodelers.size;
	Melder_require (*fromTrack >= 1 && *toTrack <= my trackmodelers.size,
		U"1 \\=< \"fromTrack\" \\=< \"toTrack\" \\=< ", my trackmodelers.size, U".");
}

static autoINTVEC newINTVECasNumbers (integer size, integer number) {
	autoINTVEC target = raw_INTVEC (size);
	for (integer i = 1; i <= size; i++)
		target [i] = number;
	return target;
}

double FormantModeler_getStandardDeviation (FormantModeler me, integer itrack) {
	double sigma = undefined;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		sigma = DataModeler_getDataStandardDeviation (ff);
	}
	return sigma;
}

double FormantModeler_getDataPointValue (FormantModeler me, integer itrack, integer index) {
	double value = undefined;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		value = DataModeler_getDataPointYValue (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointValue (FormantModeler me, integer itrack, integer index, double value) {
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
 		DataModeler_setDataPointYValue (ff, index, value);
	}
}

double FormantModeler_getDataPointSigma (FormantModeler me, integer itrack, integer index) {
	double sigma = undefined;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = (DataModeler) my trackmodelers.at [itrack];
		sigma = DataModeler_getDataPointYSigma (ff, index);
	}
	return sigma;
}

void FormantModeler_setDataPointSigma (FormantModeler me, integer itrack, integer index, double sigma) {
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
 		DataModeler_setDataPointYSigma (ff, index, sigma);
	}
}

kDataModelerData FormantModeler_getDataPointStatus (FormantModeler me, integer itrack, integer index) {
	kDataModelerData value =kDataModelerData::INVALID;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		value = DataModeler_getDataPointStatus (ff, index);
	}
	return value;
}

void FormantModeler_setDataPointStatus (FormantModeler me, integer itrack, integer index, kDataModelerData status) {
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		DataModeler_setDataPointStatus (ff, index, status);
	}
}

static void FormantModeler_setDataPointValueAndStatus (FormantModeler me, integer itrack, integer index, double value, kDataModelerData dataStatus) {
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		DataModeler_setDataPointValueAndStatus (ff, index, value, dataStatus);
	}
}

void FormantModeler_setParameterValueFixed (FormantModeler me, integer itrack, integer index, double value) {
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		DataModeler_setParameterValueFixed (ffi, index, value);
	}
}

void FormantModeler_setParametersFree (FormantModeler me, integer fromTrack, integer toTrack, integer fromIndex, integer toIndex) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);

	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		DataModeler_setParametersFree (ffi, fromIndex, toIndex);
	}
}

void FormantModeler_setDataWeighing (FormantModeler me, integer fromTrack, integer toTrack, kFormantModelerWeights weighFormants) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);

	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		kDataModelerWeights dataWeights = kDataModelerWeights::EQUAL_WEIGHTS;
		if (weighFormants == kFormantModelerWeights::ONE_OVER_BANDWIDTH)
			dataWeights = kDataModelerWeights::ONE_OVER_SIGMA;
		else if (weighFormants == kFormantModelerWeights::ONE_OVER_SQRTBANDWIDTH)
			dataWeights = kDataModelerWeights::ONE_OVER_SQRTSIGMA;
		else
			dataWeights = kDataModelerWeights::RELATIVE_;
		DataModeler_setDataWeighing (ffi, dataWeights);
	}
}

void FormantModeler_fit (FormantModeler me) {
	for (integer itrack = 1; itrack <= my trackmodelers.size; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		DataModeler_fit (ffi);
	}
}

void FormantModeler_drawBasisFunction (FormantModeler me, Graphics g, double tmin, double tmax, double fmin, double fmax,
 	integer itrack, integer iterm, bool scaled, integer numberOfPoints, bool garnish)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	if (itrack < 1 || itrack > my trackmodelers.size)
		return;
	Graphics_setInner (g);
	const DataModeler ffi = my trackmodelers.at [itrack];
	DataModeler_drawBasisFunction_inside (ffi, g, tmin, tmax, fmin, fmax, iterm, scaled, numberOfPoints);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_inqWindow (g, & tmin, & tmax, & fmin, & fmax);
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, ( scaled ? U"Frequency (Hz)" : U"Amplitude" ));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_markLeft (g, fmin, true, true, false, U"");
		Graphics_markLeft (g, fmax, true, true, false, U"");
	}
}

static integer FormantModeler_drawingSpecifiers_x (FormantModeler me, double *xmin, double *xmax, integer *ixmin, integer *ixmax) {
	Melder_assert (my trackmodelers.size > 0);
	const DataModeler fm = my trackmodelers.at [1];
	return DataModeler_getDrawingSpecifiers_x (fm, xmin, xmax, ixmin, ixmax);
}

static void FormantModeler_getCumulativeChiScores (FormantModeler me, VEC chisq) {
	try {
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		const integer numberOfTracks = my trackmodelers.size;
		for (integer itrack = 1; itrack <= numberOfTracks; itrack ++) {
			const DataModeler fm = my trackmodelers.at [itrack];
			autoVEC zscores = DataModeler_getZScores (fm);
			autoVEC chisqif = DataModeler_getChisqScoresFromZScores (fm, zscores.get(), true); // undefined -> average
			for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++)
				chisq [ipoint] += chisqif [ipoint];
		}
	} catch (MelderError) {
		Melder_throw (me, U"cannot determine cumulative chi squares.");
	}
}

static autoVEC FormantModeler_getVariancesBetweenTrackAndEstimatedTrack (FormantModeler me, integer itrack, integer estimatedTrack) {
	try {
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		const integer numberOfTracks = my trackmodelers.size;
		autoVEC var;
		if (itrack < 1 || itrack > numberOfTracks || estimatedTrack < 1 || estimatedTrack > numberOfTracks)
			return var;
		var. resize (numberOfDataPoints);
		const DataModeler fi = my trackmodelers.at [itrack];
		const DataModeler fe = my trackmodelers.at [estimatedTrack];
		for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++) {
			var [ipoint] = undefined;
			if (fi -> data [ipoint] .status != kDataModelerData::INVALID) {
				const double ye = fe -> f_evaluate (fe, fe -> data [ipoint] .x, fe -> parameters.get());
				const double diff = ye - fi -> data [ipoint] .y;
				var [ipoint] = diff * diff;
			}
		}
		return var;
	} catch (MelderError) {
		Melder_throw (me, U"Cannot get variances between track and estimate.");
	}
}

static autoVEC FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (FormantModeler me,
	kFormantModelerTrackShift shiftDirection, integer *fromTrack, integer *toTrack)
{
	try {
		const integer numberOfTracks = my trackmodelers.size;
		if (*fromTrack < 1 || *fromTrack > numberOfTracks || *toTrack < 1 || *toTrack > numberOfTracks || *toTrack < *fromTrack) {
			*toTrack = 1;
			*fromTrack = numberOfTracks;
		}
		integer formantTrack = *fromTrack, estimatedTrack = *fromTrack; // FormantModeler_NOSHIFT_TRACKS
		if (shiftDirection == kFormantModelerTrackShift::DOWN) {
			estimatedTrack = *fromTrack;
			formantTrack = *fromTrack + 1;
			*fromTrack = ( *fromTrack == 1 ? 2 : *fromTrack );
		} else if (shiftDirection == kFormantModelerTrackShift::UP) {
			formantTrack = *fromTrack;
			estimatedTrack = *fromTrack + 1;
			*toTrack = ( *toTrack == numberOfTracks ? numberOfTracks - 1 : *toTrack );
		}
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoVEC sumOfVariances = zero_VEC (numberOfDataPoints);
		for (integer itrack = *fromTrack; itrack <= *toTrack; itrack ++) {
			autoVEC vari = FormantModeler_getVariancesBetweenTrackAndEstimatedTrack (me, formantTrack, estimatedTrack);
			for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++) {
				if (isdefined (vari [ipoint]))
					sumOfVariances [ipoint] += vari [ipoint];
			}
			formantTrack ++;
			estimatedTrack ++;
		}
		return sumOfVariances;
	} catch (MelderError) {
		Melder_throw (me, U" cannot get variances.");
	}
}

void FormantModeler_drawVariancesOfShiftedTracks (FormantModeler me, Graphics g, double xmin, double xmax,
	double ymin, double ymax, kFormantModelerTrackShift shiftDirection, integer fromTrack, integer toTrack, bool garnish)
{
	try {
		integer ixmin, ixmax;
		checkTrackAutoRange (me, & fromTrack, & toTrack);
		Melder_require (FormantModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) > 0,
			U"The are not enough data points in the drawing range.");

		autoVEC varShifted = FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (me, shiftDirection, & fromTrack, & toTrack);
		autoVEC var = FormantModeler_getSumOfVariancesBetweenShiftedAndEstimatedTracks (me, kFormantModelerTrackShift::NO_, & fromTrack, & toTrack);
		for (integer ipoint = ixmin + 1; ipoint <= ixmax; ipoint ++) {
			if (isdefined (varShifted [ipoint]) && isdefined (var [ipoint]))
				var [ipoint] -= varShifted [ipoint];
		}
		if (ymax <= ymin)
			NUMextrema_u (var.part (ixmin, ixmax), & ymin, & ymax);
		if (isundef (ymin) || isundef (ymax))
			return;
		if (ymin == ymax) {
			ymin -= 0.5;
			ymax += 0.5;
		}
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		const DataModeler thee = my trackmodelers.at [1];
		while (isundef (var [ixmin]) && ixmin <= ixmax)
			ixmin ++;
		double xp = thy data [ixmin] .x, yp = var [ixmin];
		for (integer ipoint = ixmin + 1; ipoint <= ixmax; ipoint ++) {
			if (isdefined (var [ipoint])) {
				Graphics_line (g, xp, yp, thy data [ipoint] .x, var [ipoint]);
				xp = thy data [ipoint] .x;
				yp = var [ipoint];
			}
		}
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
		}

	} catch (MelderError) {
		Melder_clearError ();
	}
}

void FormantModeler_drawCumulativeChiScores (FormantModeler me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	try {
		integer ixmin, ixmax;
		Melder_require (FormantModeler_drawingSpecifiers_x (me, & xmin, & xmax, & ixmin, & ixmax) > 0,
			U"Not enough data points in drawing range.");
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoVEC chisq = zero_VEC (numberOfDataPoints);
		FormantModeler_getCumulativeChiScores (me, chisq.get());
		if (ymax <= ymin)
			NUMextrema_u (chisq.part (ixmin, ixmax), & ymin, & ymax);
		if (isundef (ymin) || isundef (ymax))
			return;
		Graphics_setInner (g);
		Graphics_setWindow (g, xmin, xmax, ymin, ymax);
		DataModeler thee = my trackmodelers.at [1];
		for (integer ipoint = ixmin + 1; ipoint <= ixmax; ipoint ++)
			Graphics_line (g, thy data [ipoint - 1] .x, chisq [ipoint - 1], thy data [ipoint] .x, chisq [ipoint]);
		Graphics_unsetInner (g);
		if (garnish) {
			Graphics_drawInnerBox (g);
			Graphics_marksBottom (g, 2, true, true, false);
			Graphics_marksLeft (g, 2, true, true, false);
		}
	} catch (MelderError) {
		Melder_clearError ();
	}
}

void FormantModeler_drawOutliersMarked (FormantModeler me, Graphics g, double tmin, double tmax, double fmax, integer fromTrack, integer toTrack,
	double numberOfSigmas, conststring32 mark, double marksFontSize, MelderColour oddTracks, MelderColour evenTracks, bool garnish)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	Graphics_setInner (g);
	const double currectFontSize = Graphics_inqFontSize (g);
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		Graphics_setColour (g, itrack %2  == 1 ? oddTracks : evenTracks );
		DataModeler_drawOutliersMarked_inside (ffi, g, tmin, tmax, 0.0, fmax, numberOfSigmas, mark, marksFontSize);
	}
	Graphics_setFontSize (g, currectFontSize);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

void FormantModeler_normalProbabilityPlot (FormantModeler me, Graphics g, integer itrack,
	integer numberOfQuantiles, double numberOfSigmas, double labelSize, conststring32 label, bool garnish) {
	if (itrack > 0 || itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		DataModeler_normalProbabilityPlot (ff, g,  numberOfQuantiles, numberOfSigmas, labelSize, label, garnish);
	}
}

void FormantModeler_drawModel_inside (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, MelderColour oddTracks, MelderColour evenTracks, integer numberOfPoints) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		DataModeler ffi = my trackmodelers.at [itrack];
		Graphics_setColour (g, Melder_BLUE); //itrack % 2 == 1 ? oddTracks : evenTracks );
		DataModeler_drawModel_inside (ffi, g, tmin, tmax, 0.0, fmax, numberOfPoints);
	}
}

void FormantModeler_drawTracks_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax, integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, MelderColour oddTracks, MelderColour evenTracks) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		DataModeler ffi = my trackmodelers.at [itrack];
		Graphics_setColour (g, itrack % 2 == 1 ? oddTracks : evenTracks );
		DataModeler_drawTrack_inside (ffi, g, xmin, xmax, 0.0, fmax, useEstimatedTrack);
	}
}

void FormantModeler_drawTracks (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters, MelderColour oddTracks, MelderColour evenTracks, bool garnish)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	Graphics_setInner (g);
	FormantModeler_drawTracks_inside (me, g, tmin, tmax, fmax, fromTrack, toTrack, useEstimatedTrack, numberOfParameters, oddTracks, evenTracks);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

void FormantModeler_speckle_inside (FormantModeler me, Graphics g, double xmin, double xmax, double fmax,
	integer fromTrack, integer toTrack, bool useEstimatedTrack, bool errorBars, MelderColour oddTracks, MelderColour evenTracks) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		Graphics_setColour (g, itrack % 2 == 1 ? oddTracks : evenTracks);
		DataModeler_speckle_inside (ffi, g, xmin, xmax, 0, fmax, useEstimatedTrack, errorBars, 0.0);
	}
}

void FormantModeler_speckle (FormantModeler me, Graphics g, double tmin, double tmax, double fmax,
	integer fromTrack, integer toTrack, bool useEstimatedTrack, integer numberOfParameters,
	bool errorBars, MelderColour oddTracks, MelderColour evenTracks, bool garnish)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	Graphics_setInner (g);
	FormantModeler_speckle_inside (me, g, tmin, tmax, fmax, fromTrack, toTrack, useEstimatedTrack, errorBars, oddTracks, evenTracks);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_textLeft (g, true, U"Formant frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeftEvery (g, 1.0, 1000.0, true, true, true);
	}
}

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfDataPoints, integer numberOfTracks, integer numberOfParameters) {
	autoINTVEC npar = newINTVECasNumbers (numberOfTracks, numberOfParameters);
	return FormantModeler_create (tmin, tmax, numberOfDataPoints, npar.get());
}

autoFormantModeler FormantModeler_create (double tmin, double tmax, integer numberOfDataPoints, constINTVEC const& numberOfParameters) {
	try {
		autoFormantModeler me = Thing_new (FormantModeler);
		my xmin = tmin;
		my xmax = tmax;
		for (integer itrack = 1; itrack <= numberOfParameters.size; itrack ++) {
			autoDataModeler ff = DataModeler_create (tmin, tmax, numberOfDataPoints, numberOfParameters [itrack], kDataModelerFunction::LEGENDRE);
			my trackmodelers. addItem_move (ff.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

double FormantModeler_getModelValueAtTime (FormantModeler me, integer itrack, double time) {
	double f = undefined;
	if (itrack >= 1 && itrack <= my trackmodelers.size) {
		const DataModeler thee = my trackmodelers.at [itrack];
		f = DataModeler_getModelValueAtX (thee, time);
	}
	return f;
}

double FormantModeler_getModelValueAtIndex (FormantModeler me, integer itrack, integer index) {
	double f = undefined;
	if (itrack >= 1 && itrack <= my trackmodelers.size) {
		const DataModeler thee = my trackmodelers.at [itrack];
		f = DataModeler_getModelValueAtIndex (thee, index);
	}
	return f;
}

double FormantModeler_getWeightedMean (FormantModeler me, integer itrack) {
	double f = undefined;
	if (itrack >= 1 && itrack <= my trackmodelers.size) {
		const DataModeler thee = my trackmodelers.at [itrack];
		f = DataModeler_getWeightedMean (thee);
	}
	return f;
}

integer FormantModeler_getNumberOfTracks (FormantModeler me) {
	return my trackmodelers.size;
}

integer FormantModeler_getNumberOfParameters (FormantModeler me, integer itrack) {
	integer numberOfParameters = 0;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		numberOfParameters = ff -> numberOfParameters;
	}
	return numberOfParameters;
}

integer FormantModeler_getNumberOfFixedParameters (FormantModeler me, integer itrack) {
	integer numberOfParameters = 0;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		numberOfParameters = ff -> numberOfParameters;
		numberOfParameters -= DataModeler_getNumberOfFreeParameters (ff);
	}
	return numberOfParameters;
}


integer FormantModeler_getNumberOfInvalidDataPoints (FormantModeler me, integer itrack) {
	integer numberOfInvalidDataPoints = 0;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		numberOfInvalidDataPoints = DataModeler_getNumberOfInvalidDataPoints (ff);
	}
	return numberOfInvalidDataPoints;
}

double FormantModeler_getParameterValue (FormantModeler me, integer itrack, integer iparameter) {
	double value = undefined;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		DataModeler ff = my trackmodelers.at [itrack];
		value = DataModeler_getParameterValue (ff, iparameter);
	}
	return value;
}

kDataModelerParameterStatus FormantModeler_getParameterStatus (FormantModeler me, integer itrack, integer index) {
	kDataModelerParameterStatus status = kDataModelerParameterStatus::NOT_DEFINED;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		status = DataModeler_getParameterStatus (ff, index);
	}
	return status;
}

double FormantModeler_getParameterStandardDeviation ( FormantModeler me, integer itrack, integer index) {
	double stdev = undefined;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		stdev = DataModeler_getParameterStandardDeviation (ff, index);
	}
	return stdev;
}

double FormantModeler_getDegreesOfFreedom (FormantModeler me, integer itrack) {
	double dof = 0.0;
	if (itrack > 0 && itrack <= my trackmodelers.size) {
		const DataModeler ff = my trackmodelers.at [itrack];
		dof = DataModeler_getDegreesOfFreedom (ff);
	}
	return dof;
}

double FormantModeler_getVarianceOfParameters (FormantModeler me, integer fromTrack, integer toTrack, integer fromIndex, integer toIndex, integer *out_numberOfFreeParameters) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	double variance = 0.0;
	integer numberOfFreeParameters = 0;
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ff = my trackmodelers.at [itrack];
		integer free;
		variance += DataModeler_getVarianceOfParameters (ff, fromIndex, toIndex, & free);
		numberOfFreeParameters += free;
	}

	if (out_numberOfFreeParameters)
		*out_numberOfFreeParameters = numberOfFreeParameters;
	return variance;
}

integer FormantModeler_getNumberOfDataPoints (FormantModeler me) {
	Melder_assert (my trackmodelers.size > 0);
	const DataModeler thee = my trackmodelers.at [1];
	// all tracks have the same number of data points
	return thy numberOfDataPoints;
}

autoTable FormantModeler_to_Table_zscores (FormantModeler me) {
	try {
		const integer icolt = 1, numberOfFormants = my trackmodelers.size;
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoTable ztable = Table_createWithoutColumnNames (numberOfDataPoints, numberOfFormants + 1);
		Table_renameColumn_e (ztable.get(), icolt, U"time");
		for (integer itrack = 1; itrack <= numberOfFormants; itrack ++) {
			const integer icolz = itrack + 1;
			Table_renameColumn_e (ztable.get(), icolz, Melder_cat (U"z", itrack));
			DataModeler ffi = my trackmodelers.at [itrack];
			if (itrack == 1) {
				for (integer i = 1; i <= numberOfDataPoints; i ++)   // only once all tracks have same x-values
					Table_setNumericValue (ztable.get(), i, icolt, ffi -> data [i] .x);
			}
			autoVEC zscores = DataModeler_getZScores (ffi);
			for (integer i = 1; i <= numberOfDataPoints; i ++)
				Table_setNumericValue (ztable.get(), i, icolz, zscores [i]);
		}
		return ztable;
	} catch (MelderError) {
		Melder_throw (U"Table not created.");
	}	
}

autoDataModeler FormantModeler_extractDataModeler (FormantModeler me, integer itrack) {
	try {
		Melder_require (itrack > 0 && itrack<= my trackmodelers.size, 
			U"The formant should be greater than zero and smaller than or equal to ", my trackmodelers.size);
		const DataModeler ff = my trackmodelers.at [itrack];
		autoDataModeler thee = Data_copy (ff);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"DataModeler not created.");
	}	
}

autoCovariance FormantModeler_to_Covariance_parameters (FormantModeler me, integer itrack) {
	try {
		Melder_require (itrack > 0 && itrack <= my trackmodelers.size,
			U"The formant should be greater than zero and smaller than or equal to ", my trackmodelers.size);
		const DataModeler dm = my trackmodelers.at [itrack];
		autoCovariance thee = Data_copy (dm -> parameterCovariances.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Covariance not created.");
	}
	
}

void FormantModeler_setTolerance (FormantModeler me, double tolerance) {
	for (integer itrack = 1; itrack <= my trackmodelers.size; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		DataModeler_setTolerance (ffi, tolerance);
	}
}

double FormantModeler_indexToTime (FormantModeler me, integer index) {
	Melder_assert (my trackmodelers.size > 0);
	const DataModeler thee = my trackmodelers.at [1];
	return ( index > 0 && index <= thy numberOfDataPoints ? thy data [index] .x : undefined );
}

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax,
	integer numberOfFormants, integer numberOfParametersPerTrack) {
	autoINTVEC npar = newINTVECasNumbers (numberOfFormants, numberOfParametersPerTrack);
	return Formant_to_FormantModeler (me, tmin, tmax, npar.get());
}

autoFormantModeler Formant_to_FormantModeler (Formant me, double tmin, double tmax, constINTVEC const& numberOfParametersPerTrack) {
	try {
		integer ifmin, ifmax;
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		const integer numberOfDataPoints = Sampled_getWindowSamples (me, tmin, tmax, & ifmin, & ifmax);
		Melder_require (numberOfDataPoints > 0,
			U"There are no formant frames in the selection.");
		autoFormantModeler thee = FormantModeler_create (tmin, tmax, numberOfDataPoints, numberOfParametersPerTrack);
		Thing_setName (thee.get(), my name.get());
		for (integer iformant = 1; iformant <= numberOfParametersPerTrack.size; iformant ++) {
			const DataModeler ffi = thy trackmodelers.at [iformant];
			integer validData = 0;
			for (integer iframe = ifmin, idata = 1; iframe <= ifmax; iframe ++, idata ++) {
				const Formant_Frame curFrame = & my frames [iframe];
				ffi -> data [idata] .x = Sampled_indexToX (me, iframe);
				ffi -> data [idata] .status = kDataModelerData::INVALID;
				if (iformant <= curFrame -> numberOfFormants) {
					const double frequency = curFrame -> formant [iformant]. frequency;
					if (isdefined (frequency)) {
						const double bandwidth = curFrame -> formant [iformant]. bandwidth;
						ffi -> data [idata] .y = curFrame -> formant [iformant]. frequency;
						ffi -> data [idata] .sigmaY = bandwidth;
						ffi -> data [idata] .status = kDataModelerData::VALID;
						validData ++;
					}
				}
			}
			ffi -> weighData = kDataModelerWeights::ONE_OVER_SIGMA;
			ffi -> tolerance = 1e-5;
		}
		FormantModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"FormantModeler not created.");
	}
}

autoFormant FormantModeler_to_Formant (FormantModeler me, bool useEstimates, bool estimateUndefineds) {
	try {
		const integer numberOfFormants = my trackmodelers.size;
		const DataModeler ff = my trackmodelers.at [1];
		const integer numberOfFrames = ff -> numberOfDataPoints;
		const double t1 = ff -> data [1] .x, dt = ff -> data [2] .x - t1;
		autoFormant thee = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		autoVEC sigma = raw_VEC (numberOfFormants);
		if (useEstimates || estimateUndefineds) {
			for (integer itrack = 1; itrack <= numberOfFormants; itrack ++)
				sigma [itrack] = FormantModeler_getStandardDeviation (me, itrack);
		}
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const Formant_Frame thyFrame = & thy frames [iframe];
			thyFrame -> intensity = 1.0; //???
			thyFrame -> formant = newvectorzero <structFormant_Formant> (numberOfFormants);
			
			for (integer itrack = 1; itrack <= numberOfFormants; itrack ++) {
				DataModeler ffi = my trackmodelers.at [itrack];
				double f = undefined, b = f;
				if (ffi -> data [iframe] .status != kDataModelerData::INVALID) {
					f = ( useEstimates ? DataModeler_getModelValueAtX (ffi, ffi -> data [iframe] .x) :
						ffi -> data [iframe] .y);
					b = ff -> data [iframe] .sigmaY; // copy original value
				} else {
					if (estimateUndefineds) {
						f = FormantModeler_getModelValueAtTime (me, itrack, ffi -> data [iframe] .x);
						b = sigma [itrack];
					}
				}
				thyFrame -> formant [itrack]. frequency = f;
				thyFrame -> formant [itrack]. bandwidth = b;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Cannot create Formant from FormantModeler.");
	}
}

double FormantModeler_getChiSquaredQ (FormantModeler me, integer fromTrack, integer toTrack, double *out_probability, double *out_ndf) {
	double ndfTotal = 0.0;
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	double chisq = 0.0;
	integer numberOfDefined = 0;
	for (integer itrack = fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		double p, df;
		const double chisqi = DataModeler_getChiSquaredQ (ffi, & p, & df);
		if (isdefined (chisqi)) {
			chisq += df * chisqi;
			ndfTotal += df;
			numberOfDefined ++;
		}
	}
	if (numberOfDefined == toTrack - fromTrack + 1) {   // chisq of all tracks defined
		chisq /= ndfTotal;
		if (out_ndf)
			*out_ndf = ndfTotal;
		if (out_probability)
			*out_probability = NUMchiSquareQ (chisq, ndfTotal);
	} else
		chisq = undefined;
	return chisq;
}

double FormantModeler_getCoefficientOfDetermination (FormantModeler me, integer fromTrack, integer toTrack) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	double ssreg = 0.0, sstot = 0.0;
	for (integer itrack= fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		double ssregi, sstoti;
		DataModeler_getCoefficientOfDetermination (ffi, & ssregi, & sstoti);
		sstot += sstoti;
		ssreg += ssregi;
	}
	const double rSquared = ( sstot > 0.0 ? ssreg / sstot : 1.0 );
	return rSquared;
}

double FormantModeler_getResidualSumOfSquares (FormantModeler me, integer itrack, integer *out_numberOfDataPoints) {
	double rss = undefined;
	integer numberOfDataPoints = -1;
	if (itrack < 1 || itrack > my trackmodelers.size)
		return undefined;
	const DataModeler ff = my trackmodelers.at [itrack];
	rss = DataModeler_getResidualSumOfSquares (ff, & numberOfDataPoints);
	if (out_numberOfDataPoints)
		*out_numberOfDataPoints = numberOfDataPoints;
	return rss;
}

void FormantModeler_setParameterValuesToZero (FormantModeler me, integer fromTrack, integer toTrack, double numberOfSigmas) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	for (integer itrack= fromTrack; itrack <= toTrack; itrack ++) {
		const DataModeler ffi = my trackmodelers.at [itrack];
		DataModeler_setParameterValuesToZero (ffi, numberOfSigmas);
	}
}


autoFormantModeler FormantModeler_processOutliers (FormantModeler me, double numberOfSigmas) {
	try {
		const integer numberOfFormants = my trackmodelers.size;
		Melder_require (numberOfFormants > 2,
			U"We need at least three formants to process outliers.");
		
		const integer numberOfDataPoints = FormantModeler_getNumberOfDataPoints (me);
		autoVEC x = raw_VEC (numberOfDataPoints); // also store x-values
		autoMAT z = raw_MAT (numberOfFormants, numberOfDataPoints);
		// maybe some of the formants had NUMundefind's.

		// 1. calculate z-scores for each formant and sort them in descending order
		DataModeler ff = my trackmodelers.at [1];
		for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++)
			x [ipoint] = ff -> data [ipoint] .x;
		for (integer itrack = 1; itrack <= numberOfFormants; itrack ++) {
			const DataModeler ffi = my trackmodelers.at [itrack];
			autoVEC zscores = DataModeler_getZScores (ffi);
			z.row (itrack)  <<=  zscores.get();
		}
		// 2. Do the manipulation in a copy
		autoFormantModeler thee = Data_copy (me);
		for (integer ipoint = 1; ipoint <= numberOfDataPoints; ipoint ++) {
			// First the easy one: first formant missing: F1' = F2; F2' = F3
			if (isdefined (z [1] [ipoint]) && isdefined (z [1] [ipoint]) && isdefined (z [3] [ipoint])) {
				if (z [1] [ipoint] > numberOfSigmas && z [2] [ipoint] > numberOfSigmas && z [3] [ipoint] > numberOfSigmas) {
					// all deviations have the same sign:
					// probably F1 is missing
					// try if f2 <- F1 and f3 <- F2 reduces chisq
					const double f2 = FormantModeler_getDataPointValue (me, 1, ipoint); // F1
					const double f3 = FormantModeler_getDataPointValue (me, 2, ipoint); // F2
					FormantModeler_setDataPointStatus (thee.get(), 1, ipoint, kDataModelerData::INVALID);
					FormantModeler_setDataPointValueAndStatus (thee.get(), 2, ipoint, f2, kDataModelerData::VALID);
					FormantModeler_setDataPointValueAndStatus (thee.get(), 3, ipoint, f3, kDataModelerData::VALID);
				}
			}
		}
		FormantModeler_fit (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Cannot calculate track discontinuities");
	}
}

double FormantModeler_getStress (FormantModeler me, integer fromTrack, integer toTrack, integer numberOfParametersPerTrack, double power) {
	checkTrackAutoRange (me, & fromTrack, & toTrack);
	integer numberOfFreeParameters;
	const double var = FormantModeler_getVarianceOfParameters (me, fromTrack, toTrack, 1, numberOfParametersPerTrack, & numberOfFreeParameters);
	double degreesOfFreedom;
	const double chisq = FormantModeler_getChiSquaredQ (me, fromTrack, toTrack, nullptr, & degreesOfFreedom);
	return ( isdefined (var) && isdefined (chisq) && numberOfFreeParameters > 0 && degreesOfFreedom >= 0.0 ? 
		( sqrt (pow (var / numberOfFreeParameters, power) * (chisq / degreesOfFreedom))) :
		undefined );
}

double FormantModeler_getAverageDistanceBetweenTracks (FormantModeler me, integer track1, integer track2, int type) {
	double diff = undefined;
	if (track1 == track2)
		return 0.0;
	if (track1 <= my trackmodelers.size && track2 <= my trackmodelers.size) {
		const DataModeler fi = my trackmodelers.at [track1];
		const DataModeler fj = my trackmodelers.at [track2];
		// fi and fj have equal number of data points
		integer numberOfDataPoints = 0;
		diff = 0.0;
		for (integer ipoint = 1; ipoint <= fi -> numberOfDataPoints; ipoint ++) {
			if (type != 0) {
				const double fie = fi -> f_evaluate (fi, fi -> data [ipoint] .x, fi -> parameters.get());
				const double fje = fj -> f_evaluate (fj, fj -> data [ipoint] .x, fj -> parameters.get());
				diff += fabs (fie - fje);
				numberOfDataPoints ++;
			} else if (fi -> data [ipoint] .status != kDataModelerData::INVALID && fj -> data [ipoint] .status != kDataModelerData::INVALID) {
				diff += fabs (fi -> data [ipoint] .y - fj -> data [ipoint] .y);
				numberOfDataPoints ++;
			}
		}
		diff /= numberOfDataPoints;
	}
	return diff;
}

double FormantModeler_getFormantsConstraintsFactor (FormantModeler me, double minF1, double maxF1, double minF2, double maxF2, double minF3) {
	const double f1 = FormantModeler_getParameterValue (me, 1, 1); // trackmodelers -> item [1] -> parameter [1]
	const double minF1Factor = ( f1 > minF1 ? 1 : sqrt (minF1 - f1 + 1.0) );
	const double maxF1Factor = ( f1 < maxF1 ? 1 : sqrt (f1 - maxF1 + 1.0) );
	const double f2 = FormantModeler_getParameterValue (me, 2, 1); // trackmodelers -> item [2] -> parameter [1]
	const double minF2Factor = ( f2 > minF2 ? 1 : sqrt (minF2 - f2 + 1.0) );
	const double maxF2Factor = ( f2 < maxF2 ? 1 : sqrt (f2 - maxF2 + 1.0) );
	const double f3 = FormantModeler_getParameterValue (me, 3, 1); // trackmodelers -> item [3] -> parameter [1]
	const double minF3Factor = ( f3 > minF3 ? 1 : sqrt (minF3 - f3 + 1.0) );
	return minF1Factor * maxF1Factor * minF2Factor * maxF2Factor * minF3Factor;
}

void FormantModeler_reportChiSquared (FormantModeler me) {
	const integer numberOfTracks = my trackmodelers.size;
	double ndf = 0.0, probability;
	MelderInfo_writeLine (U"Chi squared tests for individual models of each of ", numberOfTracks, U" formant track:");
	MelderInfo_writeLine (( my weighFormants == kFormantModelerWeights::EQUAL_WEIGHTS ? U"Standard deviation is estimated from the data." :
		( my weighFormants == kFormantModelerWeights::ONE_OVER_BANDWIDTH ? U"\tBandwidths are used as estimate for local standard deviations." :
		( my weighFormants == kFormantModelerWeights::Q_FACTOR ? U"\t1/Q's are used as estimate for local standard deviations." :
		U"\tSquare root of bandwidths are used as estimate for local standard deviations." ) ) ));
	for (integer itrack = 1; itrack <= numberOfTracks; itrack ++) {
		const double chisq_f = FormantModeler_getChiSquaredQ (me, itrack, itrack, & probability, & ndf);
		MelderInfo_writeLine (U"Formant track ", itrack, U":");
		MelderInfo_writeLine (U"\tChi squared (F", itrack, U") = ", chisq_f);
		MelderInfo_writeLine (U"\tProbability (F", itrack, U") = ", probability);
		MelderInfo_writeLine (U"\tNumber of degrees of freedom (F", itrack, U") = ", ndf);
	}
	const double chisq = FormantModeler_getChiSquaredQ (me, 1, numberOfTracks, & probability, & ndf);
	MelderInfo_writeLine (U"Chi squared test for the complete model with ", numberOfTracks, U" formants:");
	MelderInfo_writeLine (U"\tChi squared = ", chisq);
	MelderInfo_writeLine (U"\tProbability = ", probability);
	MelderInfo_writeLine (U"\tNumber of degrees of freedom = ", ndf);
}

integer Formants_getSmoothestInInterval (CollectionOf<structFormant>* me, double tmin, double tmax,
	integer numberOfFormantTracks, integer numberOfParametersPerTrack,
	kFormantModelerWeights weighData, bool useConstraints, double numberOfSigmas, double power,
	double minF1, double maxF1, double minF2, double maxF2, double minF3)
{
	try {
		const integer numberOfFormantObjects = my size;
		integer minNumberOfFormants = 1000000;
		if (numberOfFormantObjects == 1)
			return 1;
		autoINTVEC numberOfFormants = raw_INTVEC (numberOfFormantObjects);
		autoINTVEC invalid = zero_INTVEC (numberOfFormantObjects);
		double tminf = 0.0, tmaxf = 0.0;
		for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
			// Check that all Formants have the same domain
			const Formant fi = my at [iobject];
			if (tminf == tmaxf) {
				tminf = fi -> xmin;
				tmaxf = fi -> xmax;
			} else if (fi -> xmin != tminf || fi -> xmax != tmaxf) {
				Melder_throw (U"All Formant objects must have the same starting and finishing times.");
			}
			// Find the one that has least formant tracks
			numberOfFormants [iobject] = Formant_getMaxNumFormants (fi);
			if (numberOfFormants [iobject] < minNumberOfFormants)
				minNumberOfFormants = numberOfFormants [iobject];
		}
		if (numberOfFormantTracks == 0)  // default
			numberOfFormantTracks = minNumberOfFormants;
		if (numberOfFormantTracks > minNumberOfFormants) {
			// make formants with not enough tracks invalid for the competition
			integer numberOfInvalids = 0;
			for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
				if (numberOfFormants [iobject] < numberOfFormantTracks) {
					invalid [iobject] = 1;
					numberOfInvalids ++;
				}
			}
			Melder_require (numberOfInvalids < numberOfFormantObjects, 
				U"None of the Formants has enough formant tracks. Please, lower your upper formant number.");
		}
		if (tmax <= tmin) {
			tmin = tminf;
			tmax = tmaxf;
		}
		Melder_require (tmin >= tminf && tmax <= tmaxf,
			U"The selected interval should be within the Formant object's domain.");
		
		/* The chisq is not meaningfull as a the only test whether one model is better than the other because 
			if we have two models 1 & 2 with the same data points (x1 [i]=x2 [i] and y1 [i]= y2 [i] but if 
			sigma1 [i] < sigma2 [i] than chisq1 > chisq2.
			This is not what we want.
			We test therefore the variances of the parameters because if sigma1 [i] < sigma2 [i] than pvar1 < pvar2.
		 */
		double minChiVar = 1e308;
		integer index = 0;
		for (integer iobject = 1; iobject <= numberOfFormantObjects; iobject ++) {
			if (invalid [iobject] != 1) {
				const Formant fi = my at [iobject];
				autoFormantModeler fs = Formant_to_FormantModeler (fi, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack);
				FormantModeler_setParameterValuesToZero (fs.get(), 1, numberOfFormantTracks, numberOfSigmas);
				const double cf = ( useConstraints ? FormantModeler_getFormantsConstraintsFactor (fs.get(), minF1, maxF1, minF2, maxF2, minF3) : 1.0 );
				const double chiVar = FormantModeler_getStress (fs.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
				if (isdefined (chiVar) && cf * chiVar < minChiVar) {
					minChiVar = cf * chiVar;
					index = iobject;
				}
			}
		}
		return index;
	} catch (MelderError) {
		Melder_throw (U"No Formant object could be selected.");
	}
}

autoFormant Formants_extractSmoothestPart (CollectionOf<structFormant>* me, double tmin, double tmax,
	integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power) {
	try {
		const integer index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,
			weighFormants, false, numberOfSigmas, power, 1.0, 1.0, 1.0, 1.0, 1.0); // last five are just fillers
		const Formant bestfit = my at [index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}

autoFormant Formants_extractSmoothestPart_withFormantsConstraints (CollectionOf<structFormant>* me, double tmin, double tmax,
	integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power, double minF1, double maxF1, double minF2, double maxF2, double minF3)
{
	try {
		const integer index = Formants_getSmoothestInInterval (me, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack, weighFormants, 1, numberOfSigmas, power, minF1, maxF1, minF2, maxF2, minF3);
		const Formant bestfit = my at [index];
		autoFormant thee = Formant_extractPart (bestfit, tmin, tmax);
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Smoothest Formant part could not be extracted.");
	}
}

double Sound_getOptimalFormantCeiling (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power)
{
	double optimalCeiling;
	autoFormant thee = Sound_to_Formant_interval (me, startTime, endTime, windowLength, timeStep, minFreq, maxFreq,  numberOfFrequencySteps, preemphasisFrequency, numberOfFormantTracks, numberOfParametersPerTrack, weighFormants,  numberOfSigmas, power, false, 0.0, 5000.0, 0.0, 5000.0, 0.0, & optimalCeiling);
	return optimalCeiling;
}

autoFormant Sound_to_Formant_interval (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3,
	double *out_optimalCeiling)
{
	try {
		// parameter check
		Function_unidirectionalAutowindow (me, & startTime, & endTime);
		const double nyquistFrequency = 0.5 / my dx;
		Melder_require (maxFreq <= nyquistFrequency,
			U"The upper value of the maximum frequency range should not exceed the Nyquist frequency of the sound.");
		autoINTVEC noPararametersPerTrack = newINTVECasNumbers (numberOfFormantTracks, numberOfParametersPerTrack);
		double df = 0.0, mincriterium = 1e28;
		if (minFreq >= maxFreq)
			numberOfFrequencySteps = 1;
		else
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);

		double optimalCeiling = minFreq;
		integer istep_best = 0;
		
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2.0, endTime + windowLength + timeStep / 2.0, kSound_windowShape::RECTANGULAR, 1, 1);

		// Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant
		autoSound resampled = Sound_resample (part.get(), 2.0 * maxFreq, 50);
		OrderedOf<structFormant> formants;
		Melder_progressOff ();
		for (integer istep = 1; istep <= numberOfFrequencySteps; istep ++) {
			const double currentCeiling = minFreq + (istep - 1) * df;
			autoFormant formant = Sound_to_Formant_burg_mt (resampled.get(), timeStep, 5.0, currentCeiling, windowLength, preemphasisFrequency, 50.0);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.get(), startTime, endTime, noPararametersPerTrack.get());
			//TODO FormantModeler_setFormantWeighting (me, weighFormants);
			FormantModeler_setParameterValuesToZero (fm.get(), 1, numberOfFormantTracks, numberOfSigmas);
			formants. addItem_move (formant.move());
			const double cf = ( useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.get(), minF1, maxF1, minF2, maxF2, minF3) : 1.0 );
			const double chiVar = FormantModeler_getStress (fm.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			const double criterium = chiVar * cf;
			if (isdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				optimalCeiling = currentCeiling;
				istep_best = istep;
			}
		}
		Melder_require (istep_best > 0,
			U"No optimal ceiling found.");
		autoFormant thee = Formant_extractPart (formants.at [istep_best], startTime, endTime);
		Melder_progressOn ();
		if (out_optimalCeiling)
			*out_optimalCeiling = optimalCeiling;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

autoFormant Sound_to_Formant_interval_robust (Sound me, double startTime, double endTime,
	double windowLength, double timeStep, double minFreq, double maxFreq, integer numberOfFrequencySteps,
	double preemphasisFrequency, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants,
	double numberOfSigmas, double power, bool useConstraints, double minF1, double maxF1, double minF2, double maxF2, double minF3,
	double *out_optimalCeiling)
{
	try {
		if (endTime <= startTime) {
			startTime = my xmin;
			endTime = my xmax;
		}
		const double nyquistFrequency = 0.5 / my dx;
		Melder_require (maxFreq <= nyquistFrequency,
			U"The upper value of the maximum frequency range should not exceed the Nyquist frequency of the sound.");
		double df = 0.0, mincriterium = 1e28;
		if (minFreq >= maxFreq)
			numberOfFrequencySteps = 1;
		else
			df = (maxFreq - minFreq) / (numberOfFrequencySteps - 1);
		
		autoINTVEC noPararametersPerTrack = newINTVECasNumbers (numberOfFormantTracks, numberOfParametersPerTrack);

		integer istep_best = 0;
		double optimalCeiling = minFreq;
		// extract part +- windowLength because of Gaussian windowing in the formant analysis
		// +timeStep/2 to have the analysis points maximally spread in the new domain.
		
		autoSound part = Sound_extractPart (me, startTime - windowLength + timeStep / 2.0, endTime + windowLength + timeStep / 2.0,
				kSound_windowShape::RECTANGULAR, 1, true);

		/*
			Resample to 2*maxFreq to reduce resampling load in Sound_to_Formant.
		*/
		autoSound resampled = Sound_resample (part.get(), 2.0 * maxFreq, 50);

		OrderedOf<structFormant> formants;
		Melder_progressOff ();
		for (integer istep = 1; istep <= numberOfFrequencySteps; istep ++) {
			const double currentCeiling = minFreq + (istep - 1) * df;			
			autoFormant formant = Sound_to_Formant_robust_mt (resampled.get(), timeStep, 5.0,
					currentCeiling, windowLength, preemphasisFrequency, 50.0, 1.5, 3, 0.0000001, 0.0, true);
			autoFormantModeler fm = Formant_to_FormantModeler (formant.get(), startTime, endTime, noPararametersPerTrack.get());
			// TODO set weighing
			FormantModeler_setParameterValuesToZero (fm.get(), 1, numberOfFormantTracks, numberOfSigmas);
			formants. addItem_move (formant.move());
			const double cf = ( useConstraints ? FormantModeler_getFormantsConstraintsFactor (fm.get(), minF1, maxF1, minF2, maxF2, minF3) : 1.0 );
			const double chiVar = FormantModeler_getStress (fm.get(), 1, numberOfFormantTracks, numberOfParametersPerTrack, power);
			const double criterium = chiVar * cf;
			if (isdefined (chiVar) && criterium < mincriterium) {
				mincriterium = criterium;
				optimalCeiling = currentCeiling;
				istep_best = istep;
			}
		}
		Melder_require (istep_best > 0,
			U"No optimal ceiling found.");
		autoFormant thee = Formant_extractPart (formants.at [istep_best], startTime, endTime);
		Melder_progressOn ();
		if (out_optimalCeiling)
			*out_optimalCeiling = optimalCeiling;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No Formant object created.");
	}
}

#if 0
// If e.g. first formant is obviously "missing" then assign F1 as 
static void FormantModeler_correctFormantsProbablyIndexedFalsely (FormantModeler /* me */) {
	/* if shift down F1 ("correct" F1 missed)
	 * elsif shift down F2  ("correct" F2 missed)
	 * else if spurious formant before F1
	 * else if spurious formant between F1 and F2
	 * endif
	 * */
}
#endif

autoOptimalCeilingTier Sound_to_OptimalCeilingTier (Sound me,
	double windowLength, double timeStep, double minCeiling, double maxCeiling, integer numberOfFrequencySteps,
	double preemphasisFrequency, double smoothingWindow, integer numberOfFormantTracks, integer numberOfParametersPerTrack, kFormantModelerWeights weighFormants, double numberOfSigmas, double power) {
	try {
		OrderedOf<structFormant> formants;
		const double frequencyStep = ( numberOfFrequencySteps > 1 ? (maxCeiling - minCeiling) / (numberOfFrequencySteps - 1) : 0.0 );
		for (integer i = 1; i <= numberOfFrequencySteps; i ++) {
			const double ceiling = minCeiling + (i - 1) * frequencyStep;
			autoFormant formant = Sound_to_Formant_burg_mt (me, timeStep, 5.0, ceiling, windowLength, preemphasisFrequency, 50.0);
			formants. addItem_move (formant.move());
		}
		integer numberOfFrames;
		double firstTime;
		const double modellingTimeStep = timeStep;
		autoOptimalCeilingTier octier = OptimalCeilingTier_create (my xmin, my xmax);
		Sampled_shortTermAnalysis (me, smoothingWindow, modellingTimeStep, & numberOfFrames, & firstTime);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double time = firstTime + (iframe - 1) * modellingTimeStep;
			const double tmin = time - smoothingWindow / 2.0;
			const double tmax = tmin + smoothingWindow;
			const integer index = Formants_getSmoothestInInterval (& formants, tmin, tmax, numberOfFormantTracks, numberOfParametersPerTrack,	weighFormants,
					false, numberOfSigmas, power, 200.0, 1500.0, 300.0, 3000.0, 1000.0);   // min/max values are not used
			const double ceiling = minCeiling + (index - 1) * frequencyStep;
			RealTier_addPoint (octier.get(), time, ceiling);
		}
		return octier;
	} catch (MelderError) {
		Melder_throw (me, U" no OptimalCeilingTier calculated.");
	}
}

/* End of file FormantModeler.cpp */
