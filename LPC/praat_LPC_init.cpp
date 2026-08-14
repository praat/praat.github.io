/* praat_LPC_init.cpp
 *
 * Copyright (C) 1994-2026 David Weenink
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

#include "Cepstrumc.h"
#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "DTW.h"
#include "FilterBank.h"
#include "Formant_extensions.h"
#include "FormantPath.h"
#include "FormantPathEditor.h"
#include "LPC.h"
#include "MFCC.h"
#include "LFCC.h"
#include "LPC_and_Cepstrumc.h"
#include "LPC_and_Formant.h"
#include "LPC_and_LFCC.h"
#include "LPC_and_LineSpectralFrequencies.h"
#include "LPC_and_Polynomial.h"
#include "LPC_and_Tube.h"
#include "LPC_to_Spectrogram.h"
#include "LPC_to_Spectrum.h"
#include "NUM2.h"
#include "PowerCepstrum.h"
#include "PowerCepstrogram.h"
#include "Sound_to_CPP.h"
#include "Sound_to_PowerCepstrogram.h"
#include "Sound_and_LPC.h"
#include "Sound_to_Formant_mt.h"
#include "Sound_and_Cepstrum.h"
#include "Sound_to_MFCC.h"
#include "VocalTractTier.h"

#include "praat_TimeFrameSampled.h"
#include "praat_TimeTier.h"
#include "praat_Matrix.h"

#define praat_Quefrency_RANGE(fromQuefrency,toQuefrency) \
	REAL (fromQuefrency, U"left Quefrency range (s)", U"0.0") \
	REAL (toQuefrency, U"right Quefrency range (s)", U"0.0 (= all)")

static const conststring32 DRAW_BUTTON    = U"Draw -";
static const conststring32 QUERY_BUTTON   = U"Query -";
static const conststring32 MODIFY_BUTTON   = U"Modify -";

void praat_CC_init (ClassInfo klas);
void praat_TimeFrameSampled_query_init (ClassInfo klas);

DIRECT (HELP__CPP_help) {
	HELP (U"CPP")
}

FORM (GRAPHICS_EACH__CPP_draw, U"CPP: Draw", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (=all)")
	POSITIVE (cppmax, U"Maximum CPP (dB)", U"100.0")
	BOOLEAN (garnish, U"Garnish", U"yes");
	OK
DO
	GRAPHICS_EACH (CPP)
		CPP_draw (me, GRAPHICS, tmin, tmax, 0.0, cppmax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (HELP__FormantPath_help) {
	HELP (U"FormantPath")
}

static void cb_FormantPathEditor_publication (Editor /* editor */, autoDaata publication) {
	/*
		Keep the gate for error handling.
	*/
	try {
		praat_new (publication.move());
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}

DIRECT (EDITOR_ONE_FormantPath_viewAndEditAlone) {
	EDITOR_ONE (a,FormantPath)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, nullptr, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
	EDITOR_ONE_END
}

DIRECT (HINT__FormantPath_Sound_viewAndEdit) {
	INFO_NONE
		Melder_information (U"To include a Sound in your FormantPath window:\n"
			"select a FormantPath and a Sound, and click \"View & Edit\".");
	INFO_NONE_END
}

FORM (GRAPHICS_EACH__FormantPath_drawAsGrid, U"FormantPath: Draw as grid", nullptr) {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.1")
	POSITIVE (fmax, U"Maximum frequency", U"6200.0")
	NATURAL1 (fromFormant, U"left Formant range", U"1")
	NATURAL1 (toFormant, U"right Formant range", U"5")
	BOOLEAN (showBandwidths, U"Show bandwidths", true)
	COLOUR (odd, U"Colour of F1, F3, F5", U"red")
	COLOUR (even, U"Colour of F2, F4", U"purple")
	NATURAL0 (numberOfRows, U"Number of rows", U"0")
	NATURAL0 (numberOfColumns, U"Number of columns", U"0")
	POSITIVE (xSpaceFraction, U"X space fraction", U"0.1")
	POSITIVE (ySpaceFraction, U"Y space fraction", U"0.2")
	POSITIVE (lineEvery_Hz, U"Horizontal line every (Hz)", U"1000.0")
	REAL (xCursor, U"X cursor line at (s)", U"-0.1 (= no line)")
	REAL (yCursor, U"Y cursor at (Hz)", U"-100.0 (= no line)")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3 3 3")
	BOOLEAN (markCandidatesWithinPath, U"Mark candidates within path", false)
	COLOUR (markColour, U"Mark colour", U"{0.984,0.984, 0.7}")
	BOOLEAN (showStress, U"Show stress", true)
	POSITIVE (powerf, U"Power", U"1.25")
	BOOLEAN (showEstimatedModels, U"Show estimated models", true)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (FormantPath)
		FormantPath_drawAsGrid (me, GRAPHICS, tmin, tmax, fmax, fromFormant, toFormant,
			showBandwidths, odd, even, numberOfRows, numberOfColumns, xSpaceFraction, ySpaceFraction, lineEvery_Hz,
			xCursor, yCursor, markColour, parameters, markCandidatesWithinPath, showStress, powerf, showEstimatedModels, garnish
		);
	GRAPHICS_EACH_END
}

FORM (NEW__FormantPath_downTo_Table_optimalInterval, U"FormantPath: Down to Table (optimal interval)...",
	U"FormantPath: Down to Table (optimal interval)...")
{
	REAL (tmin, U"left Time range (s)", U"0.1")
	REAL (tmax, U"right Time range (s)", U"0.2")
	NATURALVECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	BOOLEAN (includeFrameNumber, U"Include frame number", false)
	BOOLEAN (includeTime, U"Include time", true)
	NATURAL0 (numberOfTimeDecimals, U"Number of time decimals", U"6")
	BOOLEAN (includeIntensity, U"Include intensity", false)
	NATURAL0 (numberOfIntensityDecimals, U"Number of intensity decimals", U"3")
	BOOLEAN (includeNumberOfFormants, U"Include number of formants", true)
	NATURAL0 (numberOfFrequencyDecimals, U"Number of frequency decimals", U"3")
	BOOLEAN (includeBandwidths, U"Include bandwidths", true)
	BOOLEAN (includeOptimumCeiling, U"Include optimal ceiling", true)
	BOOLEAN (includeMinimumStress, U"Include minimum stress", false)
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoTable result = FormantPath_downTo_Table_optimalInterval (me, tmin, tmax, parameters, powerf, 
			includeFrameNumber, includeTime, numberOfTimeDecimals,
			includeIntensity, numberOfIntensityDecimals, includeNumberOfFormants, numberOfFrequencyDecimals,
			includeBandwidths, includeOptimumCeiling, includeMinimumStress
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW__FormantPath_downTo_Table_stresses, U"FormantPath: Down to Table (stresses)", U"FormantPath: Down to Table (stresses)...") {
	REAL (tmin, U"left Time range (s)", U"0.1")
	REAL (tmax, U"right Time range (s)", U"0.2")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	NATURAL0 (numberOfStressDecimals, U"Number of stress decimals", U"2")
	BOOLEAN (includeIntervalTimes, U"Include interval times", true)
	NATURAL0 (numberOfTimeDecimals, U"Number of time decimals", U"6")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoTable result = FormantPath_downTo_Table_stresses (me, tmin, tmax, parameters, 
				powerf, numberOfStressDecimals, includeIntervalTimes, numberOfTimeDecimals);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (QUERY_ONE_FOR_REAL__FormantPath_getNumberOfCandidates) {
	QUERY_ONE_FOR_REAL (FormantPath)
		const integer result = my ceilings.size;
	QUERY_ONE_FOR_REAL_END (U"")
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR__FormantPath_listCeilingFrequencies) {
	QUERY_ONE_FOR_REAL_VECTOR (FormantPath)
		autoVEC result = copy_VEC (my ceilings.get());
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL__FormantPath_getOptimalCeiling, U"FormantPath: Get optimal ceiling", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantPath)
		const double result = FormantPath_getOptimalCeiling (me, tmin, tmax, parameters, powerf);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL__FormantPath_getStressOfCandidate, U"FormantPath: Get stress of candidate", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	NATURAL1 (candidate, U"Candidate number", U"5")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	QUERY_ONE_FOR_REAL (FormantPath)
		const double result = FormantPath_getStressOfCandidate (me, tmin, tmax, 0, 0, parameters, powerf, candidate);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__FormantPath_listStressOfCandidates, U"FormantPath: List stress of candidates", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (FormantPath)
		autoVEC result = FormantPath_getStressOfCandidates (me, tmin, tmax, 0, 0, parameters, powerf);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (MODIFY_EACH__FormantPath_setPath, U"FormantPath: Set path", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	NATURAL1 (candidate, U"Candidate number", U"5")
	OK
DO
	MODIFY_EACH (FormantPath)
		FormantPath_setPath	(me, tmin, tmax, candidate);
	MODIFY_EACH_END	
}

FORM (MODIFY_EACH__FormantPath_setOptimalPath, U"", U"") {
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	MODIFY_EACH (FormantPath)
		FormantPath_setOptimalPath (me, tmin, tmax, parameters, powerf);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_stress, U"FormantPath: To Matrix (stress)", nullptr) {
	POSITIVE (windowLength, U"Window length", U"0.025")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoMatrix result = FormantPath_to_Matrix_stress (me, windowLength, parameters, powerf);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_qsums, U"FormantPath: To Matrix (qsums)", nullptr) {
	NATURAL0 (numberOfTracks, U"Number of tracks", U"4")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoMatrix result = FormantPath_to_Matrix_qSums (me, numberOfTracks);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_transition,  U"FormantPath: To Matrix (transition)", nullptr) {
	NATURAL0 (numberOfTracks, U"Number of tracks", U"4")
	BOOLEAN (maximumCosts, U"Maximum costs", false)
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoMatrix result = FormantPath_to_Matrix_transition (me, numberOfTracks, maximumCosts);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_deltas,  U"FormantPath: To Matrix (deltas)", nullptr) {
	COMMENT (U"Within frame:")
	REAL (qWeight, U"F/B weight (0-1)", U"1.0")
	COMMENT (U"Between frames:")
	REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
	REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
	REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
	POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
	COMMENT (U"Global stress parameters:")
	POSITIVE (windowLength, U"Window length", U"0.035")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	CONVERT_EACH_TO_ONE (FormantPath)
		autoMatrix result;
		Melder_require (qWeight >= 0 && qWeight <= 1.0 &&
			frequencyChangeWeight >= 0 && frequencyChangeWeight <= 1.0 &&
			stressWeight >= 0 && stressWeight <= 1.0 &&
			ceilingChangeWeight >= 0 && ceilingChangeWeight <= 1.0,
			U"A weight should be greater or equal 0.0 and smaller or equal 1.0.");
		autoINTVEC path = FormantPath_getOptimumPath (me, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight,
				windowLength, intensityModulationStepSize, parameters, powerf, & result);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_EACH__FormantPath_pathFinder,  U"FormantPath: Path finder", nullptr) {
	COMMENT (U"Within frame:")
	REAL (qWeight, U"F/B weight (0-1)", U"1.0")
	COMMENT (U"Between frames:")
	REAL (frequencyChangeWeight, U"Frequency change weight (0-1)", U"1.0")
	REAL (stressWeight, U"Stress weight (0-1)", U"1.0")
	REAL (ceilingChangeWeight, U"Ceiling change weight (0-1)", U"1.0")
	POSITIVE (intensityModulationStepSize, U"Intensity modulation step size (dB)", U"5.0")
	COMMENT (U"Global stress parameters:")
	POSITIVE (windowLength, U"Window length", U"0.035")
	NATURAL1VECTOR (parameters, U"Coefficients by track", WHITESPACE_SEPARATED_, U"3 3 3 3")
	POSITIVE (powerf, U"Power", U"1.25")
	OK
DO
	MODIFY_EACH (FormantPath)
		Melder_require (qWeight >= 0 && qWeight <= 1.0 &&
			frequencyChangeWeight >= 0 && frequencyChangeWeight <= 1.0 &&
			stressWeight >= 0 && stressWeight <= 1.0 &&
			ceilingChangeWeight >= 0 && ceilingChangeWeight <= 1.0,
			U"A weight should be greater than or equal to 0.0 and smaller than or equal to 1.0.");
		FormantPath_pathFinder (me, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight,
				intensityModulationStepSize, windowLength, parameters, powerf);
	MODIFY_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__FormantPath_extractFormant) {
	CONVERT_EACH_TO_ONE (FormantPath)
		autoFormant result = FormantPath_extractFormant (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (EDITOR_ONE_WITH_ONE_AND_ONE__Sound_TextGrid_FormantPath_createFormantPathEditor) {
	EDITOR_ONE_WITH_ONE_AND_ONE (a, FormantPath, Sound, TextGrid)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, you, him);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
	EDITOR_ONE_WITH_ONE_AND_ONE_END
}

DIRECT (EDITOR_ONE_WITH_ONE_Sound_FormantPath_createFormantPathEditor) {
	EDITOR_ONE_WITH_ONE (a, FormantPath, Sound)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, you, nullptr);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
	EDITOR_ONE_WITH_ONE_END
}

DIRECT (EDITOR_ONE_WITH_ONE_TextGrid_FormantPath_createFormantPathEditor) {
	EDITOR_ONE_WITH_ONE (a, FormantPath, TextGrid)
		autoFormantPathEditor editor = FormantPathEditor_create (ID_AND_FULL_NAME, me, nullptr, you);
		Editor_setPublicationCallback (editor.get(), cb_FormantPathEditor_publication);
	EDITOR_ONE_WITH_ONE_END
}

/********************** Cepstrum  ****************************************/

DIRECT (CONVERT_EACH_TO_ONE__Cepstrum_downto_PowerCepstrum) {
	CONVERT_EACH_TO_ONE (Cepstrum)
		autoPowerCepstrum result = Cepstrum_downto_PowerCepstrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (HELP__PowerCepstrum_help) {
	HELP (U"PowerCepstrum")
}

FORM (GRAPHICS_EACH__Cepstrum_drawLinear, U"Cepstrum: Draw linear", U"Cepstrum: Draw (linear)...") {
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (ymin, U"Minimum", U"0.0")
	REAL (ymax, U"Maximum", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Cepstrum)
		Cepstrum_drawLinear (me, GRAPHICS, fromQuefrency, toQuefrency, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__PowerCepstrum_draw, U"PowerCepstrum: Draw", U"PowerCepstrum: Draw...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REAL (fromAmplitude_dB, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude_dB, U"right Amplitude range (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (PowerCepstrum)
		PowerCepstrum_draw (me, GRAPHICS, fromQuefrency, toQuefrency, fromAmplitude_dB, toAmplitude_dB, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__PowerCepstrum_drawTrendLine, U"PowerCepstrum: Draw trend line", U"PowerCepstrum: Draw trend line...") {
	praat_Quefrency_RANGE(fromQuefrency,toQuefrency)
	REAL (fromAmplitude_dB, U"left Amplitude range (dB)", U"0.0")
	REAL (toAmplitude_dB, U"right Amplitude range (dB)", U"0.0")
	COMMENT (U"Parameters for the trend line fit")
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	GRAPHICS_EACH (PowerCepstrum)
		PowerCepstrum_drawTrendLine (me, GRAPHICS, fromQuefrency, toQuefrency,
				fromAmplitude_dB, toAmplitude_dB, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	GRAPHICS_EACH_END
}

FORM (MODIFY_EACH__PowerCepstrum_formula, U"PowerCepstrum: Formula...", U"PowerCepstrum: Formula...") {
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula' ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (PowerCepstrum)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getPeak, U"PowerCepstrum: Get peak", U"PowerCepstrum: Get peak...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result, quefrency;
		PowerCepstrum_getMaximumAndQuefrency_pitch (me, fromPitch, toPitch, peakInterpolationType, result, quefrency);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakInQuefrencyInterval, U"PowerCepstrum: Get peak in quefrency interval", nullptr) {
	REAL (fromQuefrency, U"left Quefrency interval (s)", U"0.0033 (= 300 Hz)")
	REAL (toQuefrency, U"right Quefrency interval (s)", U"0.01667 (= 60 Hz)")
	CHOICE_ENUM (kCepstrum_peakInterpolation, peakInterpolationType,
			U"Interpolation", kCepstrum_peakInterpolation :: PARABOLIC)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result, quefrency;
		PowerCepstrum_getMaximumAndQuefrency_q (me, fromQuefrency, toQuefrency, peakInterpolationType, result, quefrency);
	QUERY_ONE_FOR_REAL_END (U" dB (quefrency=", quefrency, U" s; f=", 1.0 / quefrency, U" Hz).")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyOfPeak, U"PowerCepstrum: Get quefrency of peak", U"PowerCepstrum: Get quefrency of peak...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result, peakdB;
		PowerCepstrum_getMaximumAndQuefrency_pitch (me, fromPitch, toPitch, peakInterpolationType, peakdB, result);
		const double f = 1.0 / result;
	QUERY_ONE_FOR_REAL_END (U" seconds (f = ", f, U" Hz)")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getRNR, U"PowerCepstrum: Get rhamonics to noise ration", nullptr) {
	REAL (fromPitch, U"left Pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Pitch range (Hz)", U"333.3")
	POSITIVE (fractionalWidth, U"Fractional width (0-1)", U"0.025")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = PowerCepstrum_getRNR (me, fromPitch, toPitch, fractionalWidth);
	QUERY_ONE_FOR_REAL_END (U" (rnr)")
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrum_getStartQuefrency) {
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrum_getEndQuefrency) {
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_INTEGER__PowerCepstrum_getNumberOfQuefrencyBins) {
	QUERY_ONE_FOR_INTEGER (PowerCepstrum)
		const integer result = my nx;
	QUERY_ONE_FOR_INTEGER_END (U" samples")	
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyStep) {
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyFromIndex, U"PowerCepstrum: Get quefrency from index", nullptr) {
	NATURAL (index, U"Quefrency index", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = Sampled_indexToX (me, index);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getIndexFromQuefrency, U"PowerCepstrum: Get index from quefrency", nullptr) {
	REAL (quefrency, U"Quefrency (s)", U"0.01")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = Sampled_xToIndex (me, quefrency);
	QUERY_ONE_FOR_REAL_END (U" (index at quefrency ", quefrency, U" seconds)")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakProminence_hillenbrand, U"PowerCepstrum: Get peak prominence (hillenbrand)", U"PowerCepstrum: Get peak prominence (hillenbrand)...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double qpeak;
		const double result = PowerCepstrum_getPeakProminence_hillenbrand (me, fromPitch, toPitch, qpeak);
	QUERY_ONE_FOR_REAL_END (U" dB; quefrency=", qpeak, U" s (f=", 1.0 / qpeak, U" Hz).")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineSlope, U"PowerCepstrum: Get trend line slope", U"PowerCepstrum: Get trend line slope...") {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result;
		PowerCepstrum_fitTrendLine (me, fromQuefrency_trendLine, toQuefrency_trendLine, & result, nullptr, lineType, fitMethod);
	QUERY_ONE_FOR_REAL_END (U" dB / ", lineType == kCepstrum_trendType::LINEAR ? U"s" : U"ln (s)");
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineIntercept, U"PowerCepstrum: Get trend line intercept", U"PowerCepstrum: Get trend line intercept...") {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result;
		PowerCepstrum_fitTrendLine (me, fromQuefrency_trendLine, toQuefrency_trendLine, nullptr, & result, lineType, fitMethod);
	QUERY_ONE_FOR_REAL_END (U" dB (intercept)")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineValue, U"PowerCepstrum: Get trend line value", U"PowerCepstrum: Get trend line value...") {
	REAL (quefrency, U"Quefrency (s)", U"0.001")
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double result =	PowerCepstrum_getTrendLineValue (me, quefrency, fromQuefrency_trendLine, toQuefrency_trendLine,
			lineType, fitMethod);
	QUERY_ONE_FOR_REAL_END (U" dB (quefrency = ", quefrency, U" s)")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getValueInBin, U"PowerCepstrum: Get value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		const double result = ( binNumber < 1 || binNumber > my nx ? undefined : my v_getValueAtSample (binNumber, 1, 1) );
	QUERY_ONE_FOR_REAL_END (U" (value in bin ", binNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakProminence, U"PowerCepstrum: Get peak prominence", U"PowerCepstrum: Get peak prominence...") {
	REAL (fromPitch, U"left Search peak in pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Search peak in pitch range (Hz)", U"333.3")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrum)
		double qpeak;
		const double result = PowerCepstrum_getPeakProminence (me, fromPitch, toPitch,
				peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod, qpeak);
	QUERY_ONE_FOR_REAL_END (U" dB; quefrency=", qpeak, U" s (f=", 1.0 / qpeak, U" Hz).");
}

FORM (MODIFY_EACH__PowerCepstrum_subtractTrend_inplace, U"PowerCepstrum: Subtract trend (in-place)", U"PowerCepstrum: Subtract trend...") {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	MODIFY_EACH (PowerCepstrum)
		PowerCepstrum_subtractTrend_inplace (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__PowerCepstrum_smooth_inplace, U"PowerCepstrum: Smooth (in-place)", U"PowerCepstrum: Smooth...") {
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	MODIFY_EACH (PowerCepstrum)
		PowerCepstrum_smooth_inplace (me, quefrencySmoothingWindowDuration, numberOfIterations);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrum_smooth, U"PowerCepstrum: Smooth", U"PowerCepstrum: Smooth...") {
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	NATURAL (numberOfIterations, U"Number of iterations", U"1");
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrum)
		autoPowerCepstrum result = PowerCepstrum_smooth (me, quefrencySmoothingWindowDuration, numberOfIterations);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_smooth")
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrum_subtractTrend, U"PowerCepstrum: Subtract trend", U"PowerCepstrum: Subtract trend...") {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrum)
		autoPowerCepstrum result = PowerCepstrum_subtractTrend (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_minusTrend")
}

DIRECT (CONVERT_EACH_TO_ONE__Cepstrum_to_Spectrum) {
	CONVERT_EACH_TO_ONE (Cepstrum)
		autoSpectrum result = Cepstrum_to_Spectrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrum_to_Spectrum, U"PowerCepstrum: To Spectrum", nullptr) {
	BOOLEAN (randomPhases, U"Random phases", true)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrum)
		autoSpectrum result = PowerCepstrum_to_Spectrum (me, randomPhases);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__PowerCepstrum_to_Matrix) {
	CONVERT_EACH_TO_ONE (PowerCepstrum)
		autoMatrix result = PowerCepstrum_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** Cepstrogram  ****************************************/

DIRECT (HELP__PowerCepstrogram_help) {
	HELP (U"PowerCepstrogram")
}

FORM (GRAPHICS_EACH__old_PowerCepstrogram_paint, U"PowerCepstrogram: Paint", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (minimum_dB, U"Minimum (dB)", U"0.0")
	REAL (maximum_dB, U"Maximum (dB)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (PowerCepstrogram)
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, false, maximum_dB - minimum_dB, 0.0, garnish);
    GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__PowerCepstrogram_paint, U"PowerCepstrogram: Paint", U"PowerCepstrogram: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	praat_Quefrency_RANGE (fromQuefrency, toQuefrency)
	REAL (maximum_dB, U"Maximum (dB)", U"80.0")
	BOOLEAN (autoscaling, U"Autoscaling", false);
	REAL (dynamicRange_dB, U"Dynamic range (dB)", U"30.0");
	REAL (compression, U"Dynamic compression (0-1)", U"0.0");
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO_ALTERNATIVE (GRAPHICS_EACH__old_PowerCepstrogram_paint)
	GRAPHICS_EACH (PowerCepstrogram)
		PowerCepstrogram_paint (me, GRAPHICS, fromTime, toTime, fromQuefrency, toQuefrency, maximum_dB, autoscaling, dynamicRange_dB, compression, garnish);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrogram_smooth, U"PowerCepstrogram: Smooth", U"PowerCepstrogram: Smooth...") {
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoPowerCepstrogram result = PowerCepstrogram_smooth (me, smoothingWindowDuration, quefrencySmoothingWindowDuration);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_smoothed")
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrogram_getStartQuefrency) {
	QUERY_ONE_FOR_REAL (PowerCepstrogram)
		const double result = my ymin;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrogram_getEndQuefrency) {
	QUERY_ONE_FOR_REAL (PowerCepstrogram)
		const double result = my ymax;
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (QUERY_ONE_FOR_INTEGER__PowerCepstrogram_getNumberOfQuefrencyBins) {
	QUERY_ONE_FOR_INTEGER (PowerCepstrogram)
		const integer result = my ny;
	QUERY_ONE_FOR_INTEGER_END (U" quefrency bins")
}

DIRECT (QUERY_ONE_FOR_REAL__PowerCepstrogram_getQuefrencyStep) {
	QUERY_ONE_FOR_REAL (PowerCepstrogram)
		const double result = my dy;
	QUERY_ONE_FOR_REAL_END (U" seconds (quefrency step)")
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrogram_subtractTrend, U"PowerCepstrogram: Subtract trend", nullptr) {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoPowerCepstrogram result = PowerCepstrogram_subtractTrend (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_minusTrend")
}

FORM (MODIFY_EACH__PowerCepstrogram_subtractTrend_inplace, U"PowerCepstrogram: Subtract trend (in-place)", nullptr) {
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	MODIFY_EACH (PowerCepstrogram)
		PowerCepstrogram_subtractTrend_inplace (me, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrogram_getCPPS_hillenbrand, U"PowerCepstrogram: Get CPPS", nullptr) {
	COMMENT (U"Smoothing:")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", true)
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.001")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.00005")
	COMMENT (U"Peak search:")
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrogram)
		const double result = PowerCepstrogram_getCPPS_hillenbrand (me, subtractTrendBeforeSmoothing,
				smoothingWindowDuration, quefrencySmoothingWindowDuration, fromPitch, toPitch);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (QUERY_ONE_FOR_REAL__PowerCepstrogram_getCPPS, U"PowerCepstrogram: Get CPPS", U"PowerCepstrogram: Get CPPS...") {
	COMMENT (U"Smoothing of the Cepstrogram")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", true)
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	COMMENT (U"Peak search in PowerCepstrum:")
	REAL (fromPitch, U"left Pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	COMMENT (U"Trend line fitting in PowerCepstrum:")
	COMMENT (U"")
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (PowerCepstrogram)
		const double result = PowerCepstrogram_getCPPS (me, subtractTrendBeforeSmoothing,
			smoothingWindowDuration, quefrencySmoothingWindowDuration, fromPitch, toPitch, tolerance,
			peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod
		);
	QUERY_ONE_FOR_REAL_END (U" dB");
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrogram_to_CPP, U"PowerCepstrogram: To CPP", nullptr) {
	COMMENT (U"Smoothing of the Cepstrogram:")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", true)
	REAL (smoothingWindowDuration, U"Time averaging window (s)", U"0.02")
	REAL (quefrencySmoothingWindowDuration, U"Quefrency averaging window (s)", U"0.0005")
	COMMENT (U"Peak search in PowerCepstrum:")
	REAL (pitchFloor, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (pitchCeiling, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	COMMENT (U"Trend line fitting in PowerCepstrum:")
	REAL (quefrencyFloor, U"left Quefrency range (s)", U"0.001")
	REAL (quefrencyCeiling, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoCPP result = PowerCepstrogram_to_CPP (me, pitchFloor, pitchCeiling, tolerance,
			peakInterpolationType, quefrencyFloor, quefrencyCeiling, lineType, fitMethod);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY__EACH_WEAK__PowerCepstrogram_formula, U"PowerCepstrogram: Formula", nullptr) {
	COMMENT (U"Do for all times and quefrencies:")
	COMMENT (U"   `x` is the time in seconds")
	COMMENT (U"   `y` is the quefrency in seconds")
	COMMENT (U"   `self` is the current value")
	FORMULA (formula, U"   Replace all values with", U"sqrt(self)")
	OK
DO
	MODIFY_EACH_WEAK (PowerCepstrogram)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrogram_to_PowerCepstrum_slice, U"PowerCepstrogram: To PowerCepstrum (slice)", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoPowerCepstrum result = PowerCepstrogram_to_PowerCepstrum_slice (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", NUMnumber_as_stringWithDotReplacedByUnderscore (time));
}

FORM (LIST__PowerCepstrogram_listCPP, U"PowerCepstrogram: List cepstral peak prominences", U"PowerCepstrogram: To Table (cepstral peak prominences)...") {
	BOOLEAN (includeFrameNumbers, U"Include frame numbers", false)
	BOOLEAN (includeTimes, U"Include times", true)
	NATURAL (numberOfTimeDecimals, U"Number of time decimals", U"6")
	NATURAL (numberOfCPPdecimals, U"Number of CPP decimals", U"3")
	BOOLEAN (includePeakQuefrency, U"Include peak quefrency", false)
	NATURAL (numberOfQuefrencyDecimals, U"Number of quefrency decimals", U"3")
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	INFO_ONE (PowerCepstrogram)
		PowerCepstrogram_listCPP (me, includeFrameNumbers,includeTimes, numberOfTimeDecimals,
			numberOfCPPdecimals, includePeakQuefrency, numberOfQuefrencyDecimals, fromPitch, toPitch, tolerance, 
			peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod
		);
	INFO_ONE_END
}

FORM (NEW__PowerCepstrogram_to_Table_CPP, U"PowerCepstrogram: To Table (cepstral peak prominences)", U"PowerCepstrogram: To Table (cepstral peak prominences)...") {
	BOOLEAN (includeFrameNumbers, U"Include frame numbers", false)
	BOOLEAN (includeTimes, U"Include times", true)
	NATURAL (numberOfTimeDecimals, U"Number of time decimals", U"6")
	NATURAL (numberOfCPPdecimals, U"Number of CPP decimals", U"3")
	BOOLEAN (includePeakQuefrency, U"Include peak quefrency", false)
	NATURAL (numberOfQuefrencyDecimals, U"Number of quefrency decimals", U"3")
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoTable result = PowerCepstrogram_to_Table_CPP (me, includeFrameNumbers,includeTimes, numberOfTimeDecimals,
			numberOfCPPdecimals, includePeakQuefrency, numberOfQuefrencyDecimals, fromPitch, toPitch, tolerance, 
			peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod
		);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cpp");
}

FORM (NEW__PowerCepstrogram_to_Table_CPPvalues, U"PowerCepstrogram: To Table (CPP values)", U"PowerCepstrogram: To Table (CPP values)...") {
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	REAL (fromQuefrency_trendLine, U"left Quefrency range (s)", U"0.001")
	REAL (toQuefrency_trendLine, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
	autoTable result = PowerCepstrogram_to_Table_CPPvalues (me, fromPitch, toPitch, tolerance,
		peakInterpolationType, fromQuefrency_trendLine, toQuefrency_trendLine, lineType, fitMethod
	);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cpp");
}

FORM (CONVERT_EACH_TO_ONE__PowerCepstrogram_to_Table_hillenbrand, U"PowerCepstrogram: To Table (hillenbrand)", U"PowerCepstrogram: To Table (peak prominences)...") {
	REAL (fromPitch, U"left Peak search pitch range (Hz)", U"60.0")
	REAL (toPitch, U"right Peak search pitch range (Hz)", U"330.0")
	OK
DO
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoTable result = PowerCepstrogram_to_Table_hillenbrand (me,fromPitch, toPitch);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_cpp")
}

DIRECT (CONVERT_EACH_TO_ONE__PowerCepstrogram_to_Matrix) {
	CONVERT_EACH_TO_ONE (PowerCepstrogram)
		autoMatrix result = PowerCepstrogram_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/********************** Cepstrumc  ****************************************/

DIRECT (CONVERT_EACH_TO_ONE__Cepstrumc_to_LPC) {
	CONVERT_EACH_TO_ONE (Cepstrumc)
		autoLPC result = Cepstrumc_to_LPC (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_TWO_TO_ONE__Cepstrumc_to_DTW, U"Cepstrumc: To DTW", U"Cepstrumc: To DTW...") {
	COMMENT (U"Distance calculation between Cepstra")
	REAL (cepstralWeight, U"Cepstral weight", U"1.0")
	REAL (logEnergyWeight, U"Log energy weight", U"0.0")
	REAL (regressionWeight, U"Regression weight", U"0.0")
	REAL (regressionLogEnergyWeight, U"Regression weight log energy", U"0.0")
	REAL (windowDuration, U"Window for regression coefficients (seconds)", U"0.056")
	COMMENT (U"Boundary conditions for time warp")
	BOOLEAN (matchBeginPositions, U"Match begin positions", false)
	BOOLEAN (matchEndPositions, U"Match end positions", false)
	CHOICE (slopeConstraintType, U"Slope constraints", 1)
		OPTION (U"no restriction")
		OPTION (U"1/3 < slope < 3")
		OPTION (U"1/2 < slope < 2")
		OPTION (U"2/3 < slope < 3/2")
	OK
DO
	CONVERT_TWO_TO_ONE (Cepstrumc)
		autoDTW result = Cepstrumc_to_DTW (me, you, cepstralWeight, logEnergyWeight, regressionWeight, regressionLogEnergyWeight,
				windowDuration, matchBeginPositions, matchEndPositions, slopeConstraintType);
	CONVERT_TWO_TO_ONE_END (my name.get(), U"_", your name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Cepstrumc_to_Matrix) {
	CONVERT_EACH_TO_ONE (Cepstrumc)
		autoMatrix result = Cepstrumc_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

/******************** Formant ********************************************/

FORM (QUERY_ONE_FOR_REAL_VECTOR__Formant_listFormantSlope, U"Formant: List formant slope", U"Formant: List formant slope...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (tmin, U"left Time range (s)", U"0.0")
	REAL (tmax, U"right Time range (s)", U"0.0 (= all)")
	OPTIONMENU_ENUM (kSlopeCurve, slopeCurve, U"Slope curve", kSlopeCurve::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Formant)
		autoVEC result = Formant_listFormantSlope (me, formantNumber, tmin, tmax, slopeCurve);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (CONVERT_EACH_TO_ONE__Formant_to_LPC, U"Formant: To LPC", nullptr) {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"16000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Formant)
		autoLPC result = Formant_to_LPC (me, 1.0 / samplingFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_EACH_WEAK__Formant_formula, U"Formant: Formula", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (fromFormant, U"left Formant range", U"1")
	NATURAL (toFormant, U"right Formant range", U"5")
	COMMENT (U"Formant frequencies in odd numbered rows")
	COMMENT (U"Formant bandwidths in even numbered rows")
	FORMULA (formula, U"Formula", U"if row mod 2 = 1 and self [row,col] / self [row+1,col] < 5 then 0 else self fi")
	OK
DO
	MODIFY_EACH_WEAK (Formant)
		Formant_formula (me, fromTime, toTime, fromFormant, toFormant, interpreter, formula);
	MODIFY_EACH_WEAK_END
}

/******************** Formant & Spectrogram ************************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Formant_Spectrogram_to_IntensityTier, U"Formant & Spectrogram: To IntensityTier", U"Formant & Spectrogram: To IntensityTier...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Formant, Spectrogram)
		autoIntensityTier result = Formant_Spectrogram_to_IntensityTier (me, you, formantNumber);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", formantNumber)
}

/********************LFCC ********************************************/

DIRECT (HELP__LFCC_help) {
	HELP (U"LFCC")
}

FORM (CONVERT_EACH_TO_ONE__LFCC_to_LPC, U"LFCC: To LPC", U"LFCC: To LPC...") {
	INTEGER (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	Melder_require (numberOfCoefficients >= 0, U"Number of coefficients should not be less than zero.");
	CONVERT_EACH_TO_ONE (LFCC)
		autoLPC result = LFCC_to_LPC (me, numberOfCoefficients);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

/********************LineSpectralFrequencies ********************************************/

DIRECT (HELP__LineSpectralFrequencies_help) {
	HELP (U"LineSpectralFrequencies")
}

FORM (GRAPHICS_EACH__LineSpectralFrequencies_drawFrequencies, U"LineSpectralFrequencies: Draw frequencies", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"5000.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LineSpectralFrequencies)
		LineSpectralFrequencies_drawFrequencies (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (QUERY_ONE_FOR_INTEGER__LineSpectralFrequencies_getNumberOfFrequencies, U"LineSpectralFrequencies: Get number of frequencies", U"LineSpectralFrequencies: Get number of frequencies...") {
	NATURAL (frameNumber, U"Frame number", U"2")
	OK
DO
	QUERY_ONE_FOR_INTEGER (LineSpectralFrequencies)
		my checkIndex (frameNumber);
		const integer result = my d_frames [frameNumber]. numberOfFrequencies;
	QUERY_ONE_FOR_INTEGER_END (U" frequencies")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__LineSpectralFrequencies_listFrequenciesInFrame, U"LineSpectralFrequencies: List all frequencies in frame", U"") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (LineSpectralFrequencies)
		autoVEC result = LineSpectralFrequencies_listFrequenciesInFrame (me, frameNumber);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (QUERY_ONE_FOR_MATRIX__LineSpectralFrequencies_listAllFrequencies) {
	QUERY_ONE_FOR_MATRIX (LineSpectralFrequencies)
		autoMAT result = LineSpectralFrequencies_listAllFrequencies (me);
	QUERY_ONE_FOR_MATRIX_END	
}

DIRECT (CONVERT_EACH_TO_ONE__LineSpectralFrequencies_to_LPC) {
	CONVERT_EACH_TO_ONE (LineSpectralFrequencies)
		autoLPC result = LineSpectralFrequencies_to_LPC (me);
	CONVERT_EACH_TO_ONE_END (my name.get());
}

/********************LPC ********************************************/

DIRECT (HELP__LPC_help) {
	HELP (U"LPC")
}

FORM (GRAPHICS_EACH__LPC_drawGain, U"LPC: Draw gain", U"LPC: Draw gain...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (minimumGain, U"Minimum gain", U"0.0")
	REAL (maximumGain, U"Maximum gain", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LPC)
		LPC_drawGain (me, GRAPHICS, fromTime, toTime, minimumGain, maximumGain, garnish);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_REAL__LPC_getSamplingInterval) {
	QUERY_ONE_FOR_REAL (LPC)
		const double result = my samplingPeriod;
	QUERY_ONE_FOR_REAL_END (U" s");
}

FORM (QUERY_ONE_FOR_INTEGER__LPC_getNumberOfCoefficients, U"LPC: Get number of coefficients", U"LPC: Get number of coefficients...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (LPC)
		my checkIndex (frameNumber);
		const integer result = my d_frames [frameNumber]. nCoefficients;
	QUERY_ONE_FOR_INTEGER_END (U" coefficients")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR_LPC_listAllCoefficientsInFrame, U"", U"") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (LPC)
		autoVEC result = LPC_listCoefficientsInFrame (me, frameNumber);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (QUERY_ONE_FOR_MATRIX__LPC_listAllCoefficients) {
	QUERY_ONE_FOR_MATRIX (LPC)
		autoMAT result = LPC_listAllCoefficients (me);
	QUERY_ONE_FOR_MATRIX_END	
}

FORM (QUERY_ONE_FOR_REAL__LPC_getGainInFrame, U"LPC: Get gain in frame", U"LPC: Get gain in frame...") {
	NATURAL (frameNumber, U"Frame number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL (LPC)
		double result = undefined;
		if (frameNumber > 0 && frameNumber <= my nx)
			result = my d_frames [frameNumber] .gain;
	QUERY_ONE_FOR_REAL_END (U" gain in frame ", frameNumber)
}

DIRECT (QUERY_ONE_FOR_REAL_VECTOR_LPC_listAllGains) {
	QUERY_ONE_FOR_REAL_VECTOR (LPC)
		autoVEC result = LPC_listAllGains (me);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (GRAPHICS_EACH__LPC_drawPoles, U"LPC: Draw poles", U"LPC: Draw poles...") {
	REAL (time, U"Time (seconds)", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (LPC)
		LPC_drawPoles (me, GRAPHICS, time, garnish);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__LPC_to_Formant) {
	CONVERT_EACH_TO_ONE (LPC)
		autoFormant result = LPC_to_Formant (me, 50.0);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__LPC_to_Formant_keep_all) {
	CONVERT_EACH_TO_ONE (LPC)
		autoFormant result = LPC_to_Formant (me, 0.0);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_LFCC, U"LPC: To LFCC", U"LPC: To LFCC...") {
	INTEGER (numberOfCoefficients, U"Number of coefficients", U"0")
	OK
DO
	Melder_require (numberOfCoefficients >= 0, U"The number of coefficients should not be less than zero.");
	CONVERT_EACH_TO_ONE (LPC)
		autoLFCC result = LPC_to_LFCC (me, numberOfCoefficients);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_LineSpectralFrequencies, U"LPC: To LineSpectralFrequencies", nullptr) {
	REAL (gridSize, U"Grid size", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoLineSpectralFrequencies result = LPC_to_LineSpectralFrequencies (me, gridSize);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_Polynomial_slice, U"LPC: To Polynomial", U"LPC: To Polynomial (slice)...") {
	REAL (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoPolynomial result = LPC_to_Polynomial_slice (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", NUMnumber_as_stringWithDotReplacedByUnderscore (time))
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_Spectrum_slice, U"LPC: To Spectrum", U"LPC: To Spectrum (slice)...") {
	REAL (time, U"Time (seconds)", U"0.0")
	REAL (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REAL (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoSpectrum result = LPC_to_Spectrum (me, time, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", NUMnumber_as_stringWithDotReplacedByUnderscore (time))
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_Spectrogram, U"LPC: To Spectrogram", U"LPC: To Spectrogram...") {
	REAL (minimumFrequencyResolution, U"Minimum frequency resolution (Hz)", U"20.0")
	REAL (bandwidthReduction, U"Bandwidth reduction (Hz)", U"0.0")
	REAL (deemphasisFrequency, U"De-emphasis frequency (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoSpectrogram result = LPC_to_Spectrogram (me, minimumFrequencyResolution, bandwidthReduction, deemphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_VocalTract_slice_special, U"LPC: To VocalTract", U"LPC: To VocalTract (slice, special)...") {
	REAL (time, U"Time (s)", U"0.0")
	REAL (glottalDamping, U"Glottal damping", U"0.1")
	BOOLEAN (radiationDamping, U"Radiation damping", true)
	BOOLEAN (internalDamping, U"Internal damping", true)
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoVocalTract result = LPC_to_VocalTract_slice_special (me, time, glottalDamping, radiationDamping, internalDamping);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", NUMnumber_as_stringWithDotReplacedByUnderscore (time))
}

FORM (CONVERT_EACH_TO_ONE__LPC_to_VocalTract_slice, U"LPC: To VocalTract", U"LPC: To VocalTract (slice)...") {
	REAL (time, U"Time (s)", U"0.0")
	POSITIVE (length, U"Length (m)", U"0.17")
	OK
DO
	CONVERT_EACH_TO_ONE (LPC)
		autoVocalTract result = LPC_to_VocalTract_slice (me, time, length);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", NUMnumber_as_stringWithDotReplacedByUnderscore (time))
}

DIRECT (CONVERT_EACH_TO_ONE__LPC_downto_Matrix_lpc) {
	CONVERT_EACH_TO_ONE (LPC)
		autoMatrix result = LPC_downto_Matrix_lpc (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_lpc")
}

DIRECT (CONVERT_EACH_TO_ONE__LPC_downto_Matrix_rc) {
	CONVERT_EACH_TO_ONE (LPC)
		autoMatrix result = LPC_downto_Matrix_rc (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_rc");
}

DIRECT (CONVERT_EACH_TO_ONE__LPC_downto_Matrix_area) {
	CONVERT_EACH_TO_ONE (LPC)
		autoMatrix result = LPC_downto_Matrix_area (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_area");
}

/********************** Sound *******************************************/

FORM (CONVERT_EACH_TO_ONE__Sound_to_CPP, U"Sound: To CPP", U"Sound: To CPP...") {
	HEADING (U"Analysis parameters to get the PowerCepstrogram:")
	POSITIVE (pitchFloor, U"left Pitch range (Hz)", U"60.0")
	POSITIVE (pitchCeiling, U"right Pitch range (Hz)", U"330.0")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	HEADING (U"Smoothing parameters of the PowerCepstrogram:")
	BOOLEAN (subtractTrendBeforeSmoothing, U"Subtract trend before smoothing", U"yes")
	REAL (timeAveragingWindow, U"Time averaging window (s)", U"0.02")
	REAL (quefrencyAveragingWindow, U"Quefrency averaging window (s)", U"0.0005")
	HEADING (U"Peak search in each PowerCepstrum:")
	POSITIVE (tolerance, U"Tolerance (0-1)", U"0.05")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation :: PARABOLIC)
	HEADING (U"Trend line parameters in each PowerCepstrum:")
	REAL (qstartFit, U"left Quefrency range (s)", U"0.001")
	REAL (qendFit, U"right Quefrency range (s)", U"0.05")
	OPTIONMENU_ENUM (kCepstrum_trendType, lineType, U"Line type", kCepstrum_trendType::DEFAULT)
	OPTIONMENU_ENUM (kCepstrum_trendFit, fitMethod, U"Fit method", kCepstrum_trendFit::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoCPP result = Sound_to_CPP (me, pitchFloor, pitchCeiling, timeStep, maximumFrequency, 
			preEmphasisFrequency, subtractTrendBeforeSmoothing, timeAveragingWindow, quefrencyAveragingWindow,
			tolerance, peakInterpolationType, qstartFit, qendFit, lineType, fitMethod
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}
FORM (CONVERT_EACH_TO_ONE__Sound_to_PowerCepstrogram, U"Sound: To PowerCepstrogram", U"Sound: To PowerCepstrogram...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5000.0")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoPowerCepstrogram result = Sound_to_PowerCepstrogram (me, pitchFloor, timeStep, maximumFrequency, preEmphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}


FORM (CONVERT_EACH_TO_ONE__Sound_to_PowerCepstrogram_hillenbrand, U"Sound: To PowerCepstrogram (hillenbrand)", U"Sound: To PowerCepstrogram...") {
	POSITIVE (pitchFloor, U"Pitch floor (Hz)", U"60.0")
	POSITIVE (timeStep, U"Time step (s)", U"0.002")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoPowerCepstrogram result = Sound_to_PowerCepstrogram_hillenbrand (me, pitchFloor, timeStep);
	CONVERT_EACH_TO_ONE_END (my name.get())
}
	
FORM (CONVERT_EACH_TO_ONE__Sound_to_Formant_robust, U"Sound: To Formant (robust)", U"Sound: To Formant (robust)...") {
	REAL (timeStep, U"Time step (s)", U"0.0 (= auto)")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (formantCeiling, U"Formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormant result = Sound_to_Formant_robust (me, timeStep, maximumNumberOfFormants, formantCeiling, windowLength, 
				preEmphasisFrequency, 50.0, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, 0.0, true);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_FormantPath, U"Sound: To FormantPath", nullptr) {
	REAL (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleFormantCeiling, U"Middle formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	OPTIONMENU_ENUM (kLPC_Analysis, lpcModel, U"LPC model", kLPC_Analysis::DEFAULT)
	COMMENT (U"The maximum and minimum ceiling frequencies are determined as:")
	COMMENT (U" middleFormantCeiling * exp(+/- ceilingStepSize * numberOfStepsUpDown).")
	POSITIVE (ceilingStepSize, U"Ceiling step size", U"0.05")
	NATURAL (numberOfStepsUpDown, U"Number of steps up / down", U"4")
	COMMENT (U"For Marple analysis:")
	POSITIVE (marple_tol1, U"Tolerance 1", U"1e-6")
	POSITIVE (marple_tol2, U"Tolerance 2", U"1e-6")
	COMMENT (U"For Robust analysis:")
	POSITIVE (huber_numberOfStdDev, U"Number of std. dev.", U"1.5")
	NATURAL (huber_maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (huber_tolerance, U"Tolerance", U"0.000001")
	BOOLEAN (sourcesAsMultichannel, U"Get sources as multi channel sound", false)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoSound multichannel;
		autoFormantPath result = Sound_to_FormantPath_any (me, lpcModel, timeStep, maximumNumberOfFormants, middleFormantCeiling,
			windowLength, preEmphasisFrequency, ceilingStepSize, numberOfStepsUpDown, marple_tol1, marple_tol2,
			huber_numberOfStdDev, huber_tolerance, huber_maximumNumberOfIterations,
			( sourcesAsMultichannel ? & multichannel : nullptr )
		);
		if (sourcesAsMultichannel)
			praat_new (multichannel.move(), my name.get(), U"_sources");
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_FormantPath_burg, U"Sound: To FormantPath (Burg method)", U"Sound: To FormantPath (burg)...") {
	REAL (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleFormantCeiling, U"Middle formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	COMMENT (U"The maximum and minimum ceiling frequencies are determined as:")
	COMMENT (U" middleCeiling * exp(+/- ceilingStepSize * numberOfStepsUpDown).")
	POSITIVE (ceilingStepSize, U"Ceiling step size", U"0.05")
	NATURAL (numberOfStepsUpDown, U"Number of steps up / down", U"4")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormantPath result = Sound_to_FormantPath_burg (me, timeStep, maximumNumberOfFormants, middleFormantCeiling, windowLength,
				preEmphasisFrequency, ceilingStepSize, numberOfStepsUpDown);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_FormantPath_robust, U"Sound: To FormantPath (robust)", U"Sound: To FormantPath (burg)...") {
	REAL (timeStep, U"Time step (s)", U"0.005")
	POSITIVE (maximumNumberOfFormants, U"Max. number of formants", U"5.0")
	REAL (middleFormantCeiling, U"Middle formant ceiling (Hz)", U"5500.0 (= adult female)")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis from (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	COMMENT (U"The maximum and minimum ceiling frequencies are determined as:")
	COMMENT (U" middleCeiling * exp(+/- ceilingStepSize * numberOfStepsUpDown).")
	POSITIVE (ceilingStepSize, U"Ceiling step size", U"0.05")
	NATURAL (numberOfStepsUpDown, U"Number of steps up / down", U"4")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoFormantPath result = Sound_to_FormantPath_robust (me, timeStep, maximumNumberOfFormants, middleFormantCeiling, windowLength,
				preEmphasisFrequency, numberOfStandardDeviations, maximumNumberOfIterations, tolerance, ceilingStepSize, numberOfStepsUpDown);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

#define Sound_to_LPC_addWarning \
	COMMENT (U"Warning 1:  for formant analysis, use \"To Formant\" instead.") \
	COMMENT (U"Warning 2:  if you do use \"To LPC\", you may want to resample first.") \
	COMMENT (U"Click Help for more details.") \
	COMMENT (U"")

FORM (CONVERT_EACH_TO_ONE__Sound_to_LPC_autocorrelation, U"Sound: To LPC (autocorrelation)", U"Sound: To LPC (autocorrelation)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH_TO_ONE (Sound)
		autoLPC result = Sound_to_LPC_auto (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_LPC_covariance, U"Sound: To LPC (covariance)", U"Sound: To LPC (covariance)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH_TO_ONE (Sound)
		autoLPC result = Sound_to_LPC_covar (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_LPC_burg, U"Sound: To LPC (burg)", U"Sound: To LPC (burg)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH_TO_ONE (Sound)
		autoLPC result = Sound_to_LPC_burg (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_LPC_marple, U"Sound: To LPC (marple)", U"Sound: To LPC (marple)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (tolerance1, U"Tolerance 1", U"1e-6")
	POSITIVE (tolerance2, U"Tolerance 2", U"1e-6")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH_TO_ONE (Sound)
		autoLPC result = Sound_to_LPC_marple (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency, tolerance1, tolerance2);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_LPC_robust, U"Sound: To LPC (robust)", U"Sound: To LPC (robust)...") {
	Sound_to_LPC_addWarning
	NATURAL (predictionOrder, U"Prediction order", U"16")
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	REAL (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	OK
DO
	preEmphasisFrequency = preEmphasisFrequency < 0.0 ? 0.0 : preEmphasisFrequency;
	CONVERT_EACH_TO_ONE (Sound)
		autoLPC result = Sound_to_LPC_robust (me, predictionOrder, windowLength, timeStep, preEmphasisFrequency, 
			 numberOfStandardDeviations, maximumNumberOfIterations, tolerance, true);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_MFCC, U"Sound: To MFCC", U"Sound: To MFCC...") {
	NATURAL (numberOfCoefficients, U"Number of coefficients", U"12")
	POSITIVE (windowLength, U"Window length (s)", U"0.015")
	POSITIVE (timeStep, U"Time step (s)", U"0.005")
	COMMENT (U"Filter bank parameters")
	POSITIVE (firstFilterFrequency, U"First filter frequency (mel)", U"100.0")
	POSITIVE (distanceBetweenFilters, U"Distance between filters (mel)", U"100.0")
	REAL (maximumFrequency, U"Maximum frequency (mel)", U"0.0");
	OK
DO
	Melder_require (numberOfCoefficients < 25, U"The number of coefficients should be less than 25.");
	CONVERT_EACH_TO_ONE (Sound)
		autoMFCC result = Sound_to_MFCC (me, numberOfCoefficients, windowLength, timeStep,
				firstFilterFrequency, maximumFrequency, distanceBetweenFilters);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_EACH__VocalTract_drawSegments, U"VocalTract: Draw segments", nullptr) {
	POSITIVE (maximumLength, U"Maximum length (cm)", U"20.0")
	POSITIVE (maximumArea, U"Maximum area (cm^2)", U"90.0")
	BOOLEAN (glottisClosed, U"Closed at glottis", true)
	OK
DO
	GRAPHICS_EACH (VocalTract)
		VocalTract_drawSegments (me, GRAPHICS, maximumLength, maximumArea, glottisClosed);
	GRAPHICS_EACH_END
}

DIRECT (QUERY_ONE_FOR_REAL__VocalTract_getLength) {
	QUERY_ONE_FOR_REAL (VocalTract)
		const double result = my xmax - my xmin;
	QUERY_ONE_FOR_REAL_END (U" metres")
}

FORM (MODIFY_EACH__VocalTract_setLength, U"", nullptr) {
	POSITIVE (length, U"New length (m)", U"0.17")
	OK
DO
	MODIFY_EACH (VocalTract)
		VocalTract_setLength (me, length);
	MODIFY_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__VocalTract_to_VocalTractTier, U"VocalTract: To VocalTractTier", nullptr) {
	REAL (fromTime, U"Tier start time (s)", U"0.0")
	REAL (toTime, U"Tier end time (s)", U"1.0")
	REAL (time, U"Insert at time (s)", U"0.5")
	OK
DO
	Melder_require (fromTime < toTime, U"Your start time should be before your end time.");
	Melder_require (time >= fromTime && time <= toTime, U"Your insert time should be between your start and end times.");
	CONVERT_EACH_TO_ONE (VocalTract)
		autoVocalTractTier result = VocalTract_to_VocalTractTier (me, fromTime, toTime, time);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (HELP__VocalTractTier_help) {
	HELP (U"VocalTractTier")
}

FORM (CONVERT_EACH_TO_ONE__VocalTractTier_to_LPC, U"VocalTractTier: To LPC", nullptr) {
	POSITIVE (timeStep, U"Time step", U"0.005")
	OK
DO
	CONVERT_EACH_TO_ONE (VocalTractTier)
		autoLPC result = VocalTractTier_to_LPC (me, timeStep);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__VocalTractTier_to_VocalTract, U"", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	CONVERT_EACH_TO_ONE (VocalTractTier)
		autoVocalTract result = VocalTractTier_to_VocalTract (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__VocalTractTier_addVocalTract, U"VocalTractTier: Add VocalTract", nullptr) {
	REAL (time, U"Time (s)", U"0.1")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (VocalTractTier, VocalTract)
		VocalTractTier_addVocalTract (me, time, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

/******************* LPC & Sound *************************************/

FORM (CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filter, U"LPC & Sound: Filter", U"LPC & Sound: Filter...") {
	BOOLEAN (useGain, U"Use LPC gain", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (LPC, Sound)
		autoSound result = LPC_Sound_filter (me, you, useGain);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterWithFilterAtTime, U"LPC & Sound: Filter with one filter at time", U"LPC & Sound: Filter with filter at time...") {
	OPTIONMENU (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REAL (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (LPC, Sound)
		autoSound result = LPC_Sound_filterWithFilterAtTime (me, you, channel - 1, time);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterInverse) {
	CONVERT_ONE_AND_ONE_TO_ONE (LPC, Sound)
		autoSound result = LPC_Sound_filterInverse (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterInverseWithFilterAtTime, U"LPC & Sound: Filter (inverse) with filter at time",
		U"LPC & Sound: Filter (inverse) with filter at time...")
{
	OPTIONMENU (channel, U"Channel", 2)
		OPTION (U"Both")
		OPTION (U"Left")
		OPTION (U"Right")
	REAL (time, U"Use filter at time (s)", U"0.0")
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (LPC, Sound)
		autoSound result = LPC_Sound_filterInverseWithFilterAtTime (me, you, channel - 1, time);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_to_LPC_robust, U"Robust LPC analysis", U"LPC & Sound: To LPC (robust)...") {
	POSITIVE (windowLength, U"Window length (s)", U"0.025")
	POSITIVE (preEmphasisFrequency, U"Pre-emphasis frequency (Hz)", U"50.0")
	POSITIVE (numberOfStandardDeviations, U"Number of std. dev.", U"1.5")
	NATURAL (maximumNumberOfIterations, U"Maximum number of iterations", U"5")
	REAL (tolerance, U"Tolerance", U"0.000001")
	BOOLEAN (locationVariable, U"Variable location", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (LPC, Sound)
		autoLPC result = LPC_and_Sound_to_LPC_robust (me, you, windowLength, preEmphasisFrequency, numberOfStandardDeviations,
			maximumNumberOfIterations, tolerance, locationVariable);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_r");
}

static autoDaata HTKParameterFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 12 ) // HTK header is 12 bytes
		return autoDaata ();
	auto toint32 = [](const char *h0) -> int32 {
		return (int32)
			((uint32) ((uint32) h0 [0] << 24) | (uint32) ((uint32) h0 [1] << 16) |
			 (uint32) ((uint32) h0 [2] <<  8) | (uint32) h0 [3]);
	};
	auto toint16 = [](const char *h0) -> int16 {
		return (int16) ((uint16) ((uint16) h0 [0] << 8) | (uint16) h0 [1]);
	};
	const integer numberOfFrames = toint32 (& header [0]);
	if (numberOfFrames <= 0)
		return autoDaata ();
	const integer samplePeriodTimes100ns = toint32 (& header [4]);
	if (samplePeriodTimes100ns <= 0)
		return autoDaata ();
	const integer frameSize = toint16 (& header [8]);
	if (frameSize <= 0 || frameSize % 2 != 0) // smallest sample size is 2 bytes
		return autoDaata ();
	const integer htkType = toint16 (& header [10]); // end of 12 header (12 bytes)
	const integer fileSize = MelderFile_length (file);
	if (fileSize == numberOfFrames * frameSize + 12) {
		/*
			This could be a HTK parameter file
			Some extra checks on the htkType might be necessary but the chances that
			we have come here by chance alone are very small.
		*/
		conststring32 fileName = MelderFile_name (file);
		if (Melder_endsWith_caseAware (fileName, U".fb") && 
			htkType == 9 && frameSize % 8 == 0) // test user type and if it stores (F + B) as r32
		{
			return Formant_readFromHTKParameterFile (file);
		}
	}
	return autoDaata ();
}

void praat_uvafon_LPC_init ();
void praat_uvafon_LPC_init () {
	
	Data_recognizeFileType (HTKParameterFileRecognizer);
	
	Thing_recognizeClassesByName (
		classCepstrumc, classPowerCepstrum, classCepstrogram, classCPP, classFormantPath, classFormantPathEditor,
		classPowerCepstrogram, classLPC, classLFCC, classLineSpectralFrequencies,classMFCC, classVocalTractTier
	);
	
	structFormantPathArea  :: f_preferences ();
	structFormantPathEditor  :: f_preferences ();

	praat_addAction1 (classCPP, 0, U"CPP help", nullptr, 0,
			HELP__CPP_help);
	praat_addAction1 (classCPP, 0, U"Draw...", nullptr, 0, 
			GRAPHICS_EACH__CPP_draw);
	praat_addAction1 (classCepstrumc, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCepstrumc, 0, U"To LPC", nullptr, 0,
			CONVERT_EACH_TO_ONE__Cepstrumc_to_LPC);
	praat_addAction1 (classCepstrumc, 2, U"To DTW...", nullptr, 0,
			CONVERT_TWO_TO_ONE__Cepstrumc_to_DTW);
	praat_addAction1 (classCepstrumc, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classCepstrumc, 0, U"To Matrix", nullptr, 0,
			CONVERT_EACH_TO_ONE__Cepstrumc_to_Matrix);

	praat_addAction1 (classFormant, 0, U"List formant slope...", U"Get standard deviation...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			QUERY_ONE_FOR_REAL_VECTOR__Formant_listFormantSlope);
	praat_addAction1 (classFormant, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"To LPC...", nullptr, 0,
			CONVERT_EACH_TO_ONE__Formant_to_LPC);
	praat_addAction1 (classFormant, 0, U"Formula...", U"Formula (bandwidths)...", 1,
			MODIFY_EACH_WEAK__Formant_formula);
	praat_addAction2 (classFormant, 1, classSpectrogram, 1, U"To IntensityTier...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Formant_Spectrogram_to_IntensityTier);
	
	praat_addAction1 (classFormantPath, 0, U"FormantPath help", nullptr, 0,
			HELP__FormantPath_help);
	praat_addAction1 (classFormantPath, 1, U"View & Edit alone", nullptr, 0,
			EDITOR_ONE_FormantPath_viewAndEditAlone);
	praat_addAction1 (classFormantPath, 1, U"View & Edit with Sound?", nullptr, 0,
			HINT__FormantPath_Sound_viewAndEdit);
	praat_addAction1 (classFormantPath, 1, U"Draw as grid...", nullptr, 0,
			GRAPHICS_EACH__FormantPath_drawAsGrid);
	praat_addAction1 (classFormantPath, 0, U"Tabulate -" , nullptr, 0, nullptr);
		praat_addAction1 (classFormantPath, 0, U"Down to Table (optimal interval)...", nullptr, 1,
				NEW__FormantPath_downTo_Table_optimalInterval);
		praat_addAction1 (classFormantPath, 0, U"Down to Table (stresses)...", nullptr, 1,
				NEW__FormantPath_downTo_Table_stresses);
	praat_addAction1 (classFormantPath, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classFormantPath);
		praat_addAction1 (classFormantPath, 0, U"Get number of candidates", nullptr, 1,
				QUERY_ONE_FOR_REAL__FormantPath_getNumberOfCandidates);
		praat_addAction1 (classFormantPath, 0, U"List ceiling frequencies", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__FormantPath_listCeilingFrequencies);
		praat_addAction1 (classFormantPath, 0, U"Get stress of candidate...", nullptr, 1,
				QUERY_ONE_FOR_REAL__FormantPath_getStressOfCandidate);
		praat_addAction1 (classFormantPath, 0, U"List stress of candidates...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__FormantPath_listStressOfCandidates);
		praat_addAction1 (classFormantPath, 0, U"Get optimal ceiling...", nullptr, 1,
				QUERY_ONE_FOR_REAL__FormantPath_getOptimalCeiling);
	praat_addAction1 (classFormantPath, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classFormantPath, 0, U"Set path...", nullptr, 1,
			MODIFY_EACH__FormantPath_setPath);
		praat_addAction1 (classFormantPath, 0, U"Set optimal path...", nullptr, 1,
			MODIFY_EACH__FormantPath_setOptimalPath);
	praat_addAction1 (classFormantPath, 0, U"Extract Formant", nullptr, 0,
			CONVERT_EACH_TO_ONE__FormantPath_extractFormant);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (stress)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_stress);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (qsums)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_qsums);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (transition)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_transition);
	praat_addAction1 (classFormantPath, 0, U"To Matrix (deltas)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__FormantPath_to_Matrix_deltas);
	praat_addAction1 (classFormantPath, 0, U"Path finder...", nullptr, 0,
			MODIFY_EACH__FormantPath_pathFinder);

	praat_addAction1 (classLFCC, 0, U"LFCC help", nullptr, 0,
			HELP__LFCC_help);
	praat_CC_init (classLFCC);
	praat_addAction1 (classLFCC, 0, U"To LPC...", nullptr, 0,
			CONVERT_EACH_TO_ONE__LFCC_to_LPC);

	praat_addAction1 (classLineSpectralFrequencies, 0, U"LineSpectralFrequencies help", nullptr, 0,
			HELP__LineSpectralFrequencies_help);
	praat_addAction1 (classLineSpectralFrequencies, 0, U"Draw frequencies...", nullptr, 0,
			GRAPHICS_EACH__LineSpectralFrequencies_drawFrequencies);
	praat_addAction1 (classLineSpectralFrequencies, 0, QUERY_BUTTON, 0, 0, nullptr);
		praat_TimeFrameSampled_query_init (classLineSpectralFrequencies);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"Get number of frequencies...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__LineSpectralFrequencies_getNumberOfFrequencies);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"List frequencies in frame...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__LineSpectralFrequencies_listFrequenciesInFrame);
		praat_addAction1 (classLineSpectralFrequencies, 1, U"List all frequencies", nullptr, 1,
				QUERY_ONE_FOR_MATRIX__LineSpectralFrequencies_listAllFrequencies);

	praat_addAction1 (classLineSpectralFrequencies, 0, U"To LPC", nullptr, 0,
			CONVERT_EACH_TO_ONE__LineSpectralFrequencies_to_LPC);

	praat_addAction1 (classLPC, 0, U"LPC help", nullptr, 0, 
			HELP__LPC_help);
	praat_addAction1 (classLPC, 0, DRAW_BUTTON, nullptr, 0, nullptr);
	praat_addAction1 (classLPC, 0, U"Draw gain...", nullptr, 1, 
			GRAPHICS_EACH__LPC_drawGain);
	praat_addAction1 (classLPC, 0, U"Draw poles...", nullptr, 1, 
			GRAPHICS_EACH__LPC_drawPoles);
	praat_addAction1 (classLPC, 0, QUERY_BUTTON, nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classLPC);
		praat_addAction1 (classLPC, 1, U"Get sampling interval", nullptr, 1, 
				QUERY_ONE_FOR_REAL__LPC_getSamplingInterval);
		praat_addAction1 (classLPC, 1, U"-- get coefficients --", nullptr, 1, nullptr);
		praat_addAction1 (classLPC, 1, U"Get number of coefficients...", nullptr, 1, 
				QUERY_ONE_FOR_INTEGER__LPC_getNumberOfCoefficients);
		praat_addAction1 (classLPC, 1, U"Get coefficients in frame...", nullptr, 1, 
				QUERY_ONE_FOR_REAL_VECTOR_LPC_listAllCoefficientsInFrame);
		praat_addAction1 (classLPC, 1, U"List all coefficients", nullptr, 1, 
				QUERY_ONE_FOR_MATRIX__LPC_listAllCoefficients);
		praat_addAction1 (classLPC, 1, U"-- get gain --", nullptr, 1, 
				nullptr);
		praat_addAction1 (classLPC, 1, U"Get gain in frame...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__LPC_getGainInFrame);
		praat_addAction1 (classLPC, 1, U"List all gains", nullptr, 1, 
				QUERY_ONE_FOR_REAL_VECTOR_LPC_listAllGains);
	praat_addAction1 (classLPC, 0, MODIFY_BUTTON, nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classLPC);
	praat_addAction1 (classLPC, 0, U"Extract", nullptr, 0, nullptr);

	praat_addAction1 (classLPC, 0, U"To Spectrum (slice)...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_Spectrum_slice);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice)...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_VocalTract_slice);
	praat_addAction1 (classLPC, 0, U"To VocalTract (slice, special)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__LPC_to_VocalTract_slice_special);
	praat_addAction1 (classLPC, 0, U"To Polynomial (slice)...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_Polynomial_slice);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (lpc)", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_downto_Matrix_lpc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (rc)", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__LPC_downto_Matrix_rc);
	praat_addAction1 (classLPC, 0, U"Down to Matrix (area)", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__LPC_downto_Matrix_area);
	praat_addAction1 (classLPC, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classLPC, 0, U"To Formant", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_Formant);
	praat_addAction1 (classLPC, 0, U"To Formant (keep all)", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_Formant_keep_all);
	praat_addAction1 (classLPC, 0, U"To LFCC...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_LFCC);
	praat_addAction1 (classLPC, 0, U"To Spectrogram...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_Spectrogram);
	praat_addAction1 (classLPC, 0, U"To LineSpectralFrequencies...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__LPC_to_LineSpectralFrequencies);
	
	praat_addAction2 (classLPC, 1, classSound, 1, U"Analyse", nullptr, 0, nullptr);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filter);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse)", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterInverse);
	praat_addAction2 (classLPC, 1, classSound, 1, U"To LPC (robust)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_to_LPC_robust);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter with filter at time...", nullptr, 0, 
			CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterWithFilterAtTime);
	praat_addAction2 (classLPC, 1, classSound, 1, U"Filter (inverse) with filter at time...", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__LPC_Sound_filterInverseWithFilterAtTime);
	
	praat_addAction1 (classPowerCepstrum, 0, U"PowerCepstrum help", nullptr, 0, 
			HELP__PowerCepstrum_help);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw...", nullptr, 0, 
			GRAPHICS_EACH__PowerCepstrum_draw);
	praat_addAction1 (classPowerCepstrum, 0, U"Draw trend line... || Draw tilt line...",
			nullptr, 0, GRAPHICS_EACH__PowerCepstrum_drawTrendLine);   // alternative GuiMenu_DEPRECATED_2019
	praat_addAction1 (classCepstrum, 0, U"Draw (linear)...", nullptr, GuiMenu_HIDDEN,
			GRAPHICS_EACH__Cepstrum_drawLinear);
	praat_addAction1 (classCepstrum, 0, U"Down to PowerCepstrum", nullptr, 0, 
			CONVERT_EACH_TO_ONE__Cepstrum_downto_PowerCepstrum);
	praat_addAction1 (classPowerCepstrum, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classPowerCepstrum, 1, U"Query quefrency domain", nullptr, 1, nullptr);
			praat_addAction1 (classPowerCepstrum, 1, U"Get start quefrency", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrum_getStartQuefrency);
			praat_addAction1 (classPowerCepstrum, 1, U"Get end quefrency", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrum_getEndQuefrency);
		praat_addAction1 (classPowerCepstrum, 1, U"Query quefrency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classPowerCepstrum, 1, U"Get number of quefrency bins", nullptr, 2,
					QUERY_ONE_FOR_INTEGER__PowerCepstrum_getNumberOfQuefrencyBins);
			praat_addAction1 (classPowerCepstrum, 1, U"Get quefrency step", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyStep);
			praat_addAction1 (classPowerCepstrum, 1, U"Get quefrency from index...", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyFromIndex);
			praat_addAction1 (classPowerCepstrum, 1, U"Get index from quefrency...", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrum_getIndexFromQuefrency);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__PowerCepstrum_getPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak in quefrency interval...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakInQuefrencyInterval);
		praat_addAction1 (classPowerCepstrum, 0, U"Get quefrency of peak...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__PowerCepstrum_getQuefrencyOfPeak);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence (hillenbrand)...",
				nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakProminence_hillenbrand);
		praat_addAction1 (classPowerCepstrum, 0, U"Get peak prominence...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__PowerCepstrum_getPeakProminence);
		praat_addAction1 (classPowerCepstrum, 0, U"Get trend line slope... || Get tilt line slope...",
				nullptr, 1, QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineSlope);
		praat_addAction1 (classPowerCepstrum, 0, U"Get trend line intercept... || Get tilt line intercept...", nullptr, 1,
				QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineIntercept);
		praat_addAction1 (classPowerCepstrum, 0, U"Get trend line value...", nullptr, 1,
				QUERY_ONE_FOR_REAL__PowerCepstrum_getTrendLineValue);
		praat_addAction1 (classPowerCepstrum, 0, U"Get value in bin...", nullptr, 1,
				QUERY_ONE_FOR_REAL__PowerCepstrum_getValueInBin);
		praat_addAction1 (classPowerCepstrum, 0, U"Get rhamonics to noise ratio...", nullptr, 1, 
				QUERY_ONE_FOR_REAL__PowerCepstrum_getRNR);
	praat_addAction1 (classPowerCepstrum, 1, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPowerCepstrum, 0, U"Formula...", nullptr, 1, 
				MODIFY_EACH__PowerCepstrum_formula);
		praat_addAction1 (classPowerCepstrum, 0, U"Subtract trend (in-place)... || Subtract tilt (in-line)...",
				nullptr, 1, MODIFY_EACH__PowerCepstrum_subtractTrend_inplace);   // alternative GuiMenu_DEPRECATED_2017
		praat_addAction1 (classPowerCepstrum, 0, U"Smooth (in-place)... || Smooth (in-line)...",
				nullptr, 1, MODIFY_EACH__PowerCepstrum_smooth_inplace);   // alternative GuiMenu_DEPRECATED_2017

	praat_addAction1 (classPowerCepstrum, 0, U"Subtract trend... || Subtract tilt...",
			nullptr, 0, CONVERT_EACH_TO_ONE__PowerCepstrum_subtractTrend);   // alternative GuiMenu_DEPRECATED_2019
	praat_addAction1 (classPowerCepstrum, 0, U"Smooth...", nullptr, 0,
			CONVERT_EACH_TO_ONE__PowerCepstrum_smooth);
	praat_addAction1 (classCepstrum, 0, U"To Spectrum", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Cepstrum_to_Spectrum);
	praat_addAction1 (classPowerCepstrum, 0, U"To Spectrum...", nullptr, GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__PowerCepstrum_to_Spectrum);
	praat_addAction1 (classPowerCepstrum, 0, U"To Matrix", nullptr, 0, 
			CONVERT_EACH_TO_ONE__PowerCepstrum_to_Matrix);

	praat_addAction1 (classPowerCepstrogram, 0, U"PowerCepstrogram help", nullptr, 0,
			HELP__PowerCepstrogram_help);
	praat_addAction1 (classPowerCepstrogram, 0, U"Paint...", nullptr, 0, 
			GRAPHICS_EACH__PowerCepstrogram_paint);
	praat_addAction1 (classPowerCepstrogram, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classPowerCepstrogram, 1, U"List cepstral peak prominences...",
				nullptr, 1, LIST__PowerCepstrogram_listCPP);
		praat_addAction1 (classPowerCepstrogram, 0, U"To Table (cepstral peak prominences)...",
				nullptr, 1, NEW__PowerCepstrogram_to_Table_CPP);
		praat_addAction1 (classPowerCepstrogram, 0, U"To Table (CPP values)...",
				nullptr, 1, NEW__PowerCepstrogram_to_Table_CPPvalues);
	praat_addAction1 (classPowerCepstrogram, 0, U"To Table (hillenbrand)...", nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__PowerCepstrogram_to_Table_hillenbrand);
	praat_addAction1 (classPowerCepstrogram, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency domain", nullptr, 1, nullptr);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get start quefrency", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrogram_getStartQuefrency);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get end quefrency", nullptr, 2, 
					QUERY_ONE_FOR_REAL__PowerCepstrogram_getEndQuefrency);
		praat_addAction1 (classPowerCepstrogram, 1, U"Query quefrency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get number of quefrency bins", nullptr, 2,
					QUERY_ONE_FOR_INTEGER__PowerCepstrogram_getNumberOfQuefrencyBins);
			praat_addAction1 (classPowerCepstrogram, 1, U"Get quefrency step", nullptr, 2,
					QUERY_ONE_FOR_REAL__PowerCepstrogram_getQuefrencyStep);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS (hillenbrand)...",
				nullptr, GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, QUERY_ONE_FOR_REAL__PowerCepstrogram_getCPPS_hillenbrand);
		praat_addAction1 (classPowerCepstrogram, 0, U"Get CPPS...", nullptr, 1,
				QUERY_ONE_FOR_REAL__PowerCepstrogram_getCPPS);
	praat_addAction1 (classPowerCepstrogram, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPowerCepstrogram);
		praat_addAction1 (classPowerCepstrogram, 0, U"Formula...", nullptr, 1, 
				MODIFY__EACH_WEAK__PowerCepstrogram_formula);
		praat_addAction1 (classPowerCepstrogram, 0,
			U"Subtract trend (in-place)... || Subtract tilt (in-place)... || Subtract tilt (in-line)...",
			nullptr, 1, MODIFY_EACH__PowerCepstrogram_subtractTrend_inplace
		);   // GuiMenu_DEPRECATED_2017/2019
	praat_addAction1 (classPowerCepstrogram, 0, U"To PowerCepstrum (slice)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__PowerCepstrogram_to_PowerCepstrum_slice);
	praat_addAction1 (classPowerCepstrogram, 0, U"Smooth...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__PowerCepstrogram_smooth);
	praat_addAction1 (classPowerCepstrogram, 0, U"Subtract trend... || Subtract tilt...",
			nullptr, 0, CONVERT_EACH_TO_ONE__PowerCepstrogram_subtractTrend);   // alternative GuiMenu_DEPRECATED_2019
	praat_addAction1 (classPowerCepstrogram, 0, U"To CPP...", nullptr, 0,
			CONVERT_EACH_TO_ONE__PowerCepstrogram_to_CPP);
praat_addAction1 (classPowerCepstrogram, 0, U"To Matrix", nullptr, 0,
			CONVERT_EACH_TO_ONE__PowerCepstrogram_to_Matrix);

	praat_addAction1 (classSound, 0, U"To PowerCepstrogram...", U"To Harmonicity (gne)...", 1, 
			CONVERT_EACH_TO_ONE__Sound_to_PowerCepstrogram);
	praat_addAction1 (classSound, 0, U"To CPP...", U"To Harmonicity (gne)...", 1, 
			CONVERT_EACH_TO_ONE__Sound_to_CPP);
	
	praat_addAction1 (classSound, 0, U"To PowerCepstrogram (hillenbrand)...", U"To Harmonicity (gne)...", 
			GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, CONVERT_EACH_TO_ONE__Sound_to_PowerCepstrogram_hillenbrand);
	
	praat_addAction1 (classSound, 0, U"To Formant (robust)...", U"To Formant (sl)...", 2,
			CONVERT_EACH_TO_ONE__Sound_to_Formant_robust);
	praat_addAction1 (classSound, 0, U"To FormantPath...", U"To Formant (robust)...", 2, 
			CONVERT_EACH_TO_ONE__Sound_to_FormantPath);
	praat_addAction1 (classSound, 0, U"To FormantPath (burg)...", U"To FormantPath...", 1, 
			CONVERT_EACH_TO_ONE__Sound_to_FormantPath_burg);
	praat_addAction1 (classSound, 0, U"To FormantPath (robust)...", U"To FormantPath (burg)...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN, 
			CONVERT_EACH_TO_ONE__Sound_to_FormantPath_robust);
	praat_addAction1 (classSound, 0, U"To LPC", U"To FormantPath...", 1, nullptr);
	praat_addAction1 (classSound, 0, U"To LPC (autocorrelation)...", U"To LPC", 2,
			CONVERT_EACH_TO_ONE__Sound_to_LPC_autocorrelation);
	praat_addAction1 (classSound, 0, U"To LPC (covariance)...", U"To LPC (autocorrelation)...", 2, 
			CONVERT_EACH_TO_ONE__Sound_to_LPC_covariance);
	praat_addAction1 (classSound, 0, U"To LPC (burg)...", U"To LPC (covariance)...", 2,
			CONVERT_EACH_TO_ONE__Sound_to_LPC_burg);
	praat_addAction1 (classSound, 0, U"To LPC (marple)...", U"To LPC (burg)...", 2,
			CONVERT_EACH_TO_ONE__Sound_to_LPC_marple);
	praat_addAction1 (classSound, 0, U"To LPC (robust)...", U"To LPC (marple)...", 2,
			CONVERT_EACH_TO_ONE__Sound_to_LPC_robust);
	praat_addAction1 (classSound, 0, U"To MFCC...", U"To LPC (robust)...", 1,
			CONVERT_EACH_TO_ONE__Sound_to_MFCC);
	praat_addAction2 (classSound, 1, classFormantPath, 1, U"View & Edit", nullptr,0,
			EDITOR_ONE_WITH_ONE_Sound_FormantPath_createFormantPathEditor);
	praat_addAction2 (classTextGrid, 1, classFormantPath, 1, U"View & Edit", nullptr,0,
			EDITOR_ONE_WITH_ONE_TextGrid_FormantPath_createFormantPathEditor);
	praat_addAction3 (classSound, 1, classTextGrid, 1, classFormantPath, 1, U"View & Edit", nullptr, 0,
			EDITOR_ONE_WITH_ONE_AND_ONE__Sound_TextGrid_FormantPath_createFormantPathEditor);
	
	praat_addAction1 (classVocalTract, 0, U"Draw segments...", U"Draw", 0,
			GRAPHICS_EACH__VocalTract_drawSegments);
	praat_addAction1 (classVocalTract, 1, U"Get length", U"Draw segments...", 0,
			QUERY_ONE_FOR_REAL__VocalTract_getLength);
	praat_addAction1 (classVocalTract, 1, U"Set length", U"Formula...", 0,
			MODIFY_EACH__VocalTract_setLength);
	praat_addAction1 (classVocalTract, 0, U"To VocalTractTier...", U"To Spectrum...", 0, 
			CONVERT_EACH_TO_ONE__VocalTract_to_VocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"VocalTractTier help", nullptr, 0, 
			HELP__VocalTractTier_help);
	praat_addAction1 (classVocalTractTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeTier_modify_init (classVocalTractTier);
	praat_addAction1 (classVocalTractTier, 0, U"To LPC...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__VocalTractTier_to_LPC);
	praat_addAction1 (classVocalTractTier, 0, U"To VocalTract...", nullptr, 0, 
			CONVERT_EACH_TO_ONE__VocalTractTier_to_VocalTract);
	praat_addAction2 (classVocalTractTier, 1, classVocalTract, 1, U"Add VocalTract...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__VocalTractTier_addVocalTract);

	INCLUDE_MANPAGES (manual_LPC)
	INCLUDE_MANPAGES (manual_DataModeler)

	INCLUDE_LIBRARY (praat_DataModeler_init)
}

/* End of file praat_LPC_init.c */
