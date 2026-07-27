/* praat_uvafon_init.cpp
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

#include "Cochleagram_and_Excitation.h"
#include "Corpus.h"
#include "Distributions_and_Strings.h"
#include "Distributions_and_Transition.h"
#include "Excitation_to_Formant.h"
#include "Harmonicity.h"
#include "LongSound.h"
#include "Ltas_to_SpectrumTier.h"
#include "ManipulationEditor.h"
#include "Matrix_and_Pitch.h"
#include "Matrix_and_PointProcess.h"
#include "Matrix_and_Polygon.h"
#include "Matrix_extensions.h"
#include "Movie.h"
#include "ParamCurve.h"
#include "Pitch_Intensity.h"
#include "Pitch_to_PitchTier.h"
#include "Pitch_to_PointProcess.h"
#include "Pitch_to_Sound.h"
#include "PitchEditor.h"
#include "PitchTier_to_PointProcess.h"
#include "PitchTier_to_Sound.h"
#include "PointEditor.h"
#include "PointProcess_and_Sound.h"
#include "Praat_tests.h"
#include "Sound_and_Spectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_PointProcess.h"
#include "SpectrogramEditor.h"
#include "Spectrum_and_Spectrogram.h"
#include "Spectrum_to_Excitation.h"
#include "Spectrum_to_Formant.h"
#include "SpectrumEditor.h"
#include "SpellingChecker.h"
#include "Strings_extensions.h"
#include "StringsEditor.h"
#include "TableEditor.h"
#include "TextGrid_Sound.h"
#include "VocalTract.h"
#include "VoiceAnalysis.h"
#include "WordList.h"

#include "praat_TimeTier.h"
#include "praat_TimeFrameSampled.h"
#include "praat_TimeVector.h"
#include "praat_Sound.h"
#include "praat_Matrix.h"
#include "praat_Tiers.h"
#include "praat_ExperimentMFC.h"
#include "praat_uvafon_init.h"

static const conststring32 STRING_FROM_FREQUENCY_HZ = U"left Frequency range (Hz)";
static const conststring32 STRING_TO_FREQUENCY_HZ = U"right Frequency range (Hz)";

// MARK: - COCHLEAGRAM

// MARK: Help

DIRECT (HELP_Cochleagram_help) {
	HELP (U"Cochleagram")
}

// MARK: Movie

DIRECT (MOVIE_Cochleagram_playMovie) {
	MOVIE_ONE (Cochleagram, U"Cochleagram movie", 300, 300)
		Matrix_playMovie (me, graphics);
	MOVIE_ONE_END
}

// MARK: Info

FORM (REAL_Cochleagram_difference, U"Cochleagram difference", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	QUERY_TWO_FOR_REAL (Cochleagram)
		const double result = Cochleagram_difference (me, you, fromTime, toTime);
	QUERY_TWO_FOR_REAL_END (U" hertz (root-mean-square)")
}

// MARK: Draw

FORM (GRAPHICS_Cochleagram_paint, U"Paint Cochleagram", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Cochleagram)
		Cochleagram_paint (me, GRAPHICS, fromTime, toTime, garnish);
	GRAPHICS_EACH_END
}

// MARK: Modify

FORM (MODIFY_Cochleagram_formula, U"Cochleagram Formula", U"Cochleagram: Formula...") {
	COMMENT (U"`x` is time in seconds, `y` is place in Bark")
	COMMENT (U"y := y1; for row := 1 to nrow do { x := x1; "
		"for col := 1 to ncol do { self [row, col] := `formula` ; x := x + dx } y := y + dy }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Cochleagram)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Analyse

FORM (NEW_Cochleagram_to_Excitation, U"From Cochleagram to Excitation", nullptr) {
	REAL (time, U"Time (s)", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Cochleagram)
		autoExcitation result = Cochleagram_to_Excitation (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Hack

DIRECT (NEW_Cochleagram_to_Matrix) {
	CONVERT_EACH_TO_ONE (Cochleagram)
		autoMatrix result = Cochleagram_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - CORPUS

// MARK: New

FORM (NEW1_Corpus_create, U"Create Corpus", U"Create Corpus...") {
	WORD (name, U"Name", U"myCorpus")
	FOLDER (folderWithSoundFiles, U"Folder with sound files", U"")
	WORD (soundFileExtension, U"Sound file extension", U"wav")
	FOLDER (folderWithAnnotationFiles, U"Folder with annotation files", U"")
	WORD (annotationFileExtension, U"Annotation file extension", U"TextGrid")
	OK
DO
	CREATE_ONE
		autoCorpus result = Corpus_create (folderWithSoundFiles, soundFileExtension, folderWithAnnotationFiles, annotationFileExtension);
	CREATE_ONE_END (name)
}

// MARK: View & Edit

DIRECT (EDITOR_ONE_Corpus_edit) {
	EDITOR_ONE (a,Corpus)
		autoTableEditor editor = TableEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: - DISTRIBUTIONS

FORM (NEW_Distributions_to_Transition, U"To Transition", nullptr) {
	NATURAL (environment, U"Environment", U"1")
	BOOLEAN (greedy, U"Greedy", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Distributions)
		autoTransition result = Distributions_to_Transition (me, nullptr, environment, nullptr, greedy);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW1_Distributions_to_Transition_adj, U"To Transition", nullptr) {
	NATURAL (environment, U"Environment", U"1")
	BOOLEAN (greedy, U"Greedy", true)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Distributions, Transition)
		autoTransition result = Distributions_to_Transition (me, nullptr, environment, you, greedy);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

FORM (NEW1_Distributions_to_Transition_noise, U"To Transition (noise)", nullptr) {
	NATURAL (environment, U"Environment", U"1")
	BOOLEAN (greedy, U"Greedy", true)
	OK
DO
	CONVERT_TWO_TO_ONE (Distributions)
		autoTransition result = Distributions_to_Transition (me, you, environment, nullptr, greedy);
	CONVERT_TWO_TO_ONE_END (my name.get())
}

FORM (NEW1_Distributions_to_Transition_noise_adj, U"To Transition (noise)", nullptr) {
	NATURAL (environment, U"Environment", U"1")
	BOOLEAN (greedy, U"Greedy", true)
	OK
DO
	CONVERT_TWO_AND_ONE_TO_ONE (Distributions, Transition)
		autoTransition result = Distributions_to_Transition (me, you, environment, him, greedy);
	CONVERT_TWO_AND_ONE_TO_ONE_END (my name.get())
}

// MARK: - DISTRIBUTIONS & TRANSITION

DIRECT (NEW1_Distributions_Transition_map) {
	CONVERT_ONE_AND_ONE_TO_ONE (Distributions, Transition)
		autoDistributions result = Distributions_Transition_map (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (U"surface")
}

// MARK: - EXCITATION

// MARK: Help

DIRECT (HELP_Excitation_help) {
	HELP (U"Excitation")
}

// MARK: Draw

FORM (GRAPHICS_Excitation_draw, U"Draw Excitation", nullptr) {
	REAL (fromFrequency, U"From frequency (Bark)", U"0.0")
	REAL (toFrequency, U"To frequency (Bark)", U"25.6")
	REAL (minimum, U"Minimum (phon)", U"0.0")
	REAL (maximum, U"Maximum (phon)", U"100.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Excitation)
		Excitation_draw (me, GRAPHICS, fromFrequency, toFrequency, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

DIRECT (REAL_Excitation_getLoudness) {
	QUERY_ONE_FOR_REAL (Excitation)
		const double result = Excitation_getLoudness (me);
	QUERY_ONE_FOR_REAL_END (U" sones")
}

// MARK: Modify

FORM (MODIFY_Excitation_formula, U"Excitation Formula", U"Excitation: Formula...") {
	COMMENT (U"`x` is the place in Bark, `col` is the bin number")
	COMMENT (U"x := 0;   for col := 1 to ncol do { self [1, col] := `formula` ; x := x + dx }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Excitation)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Convert

FORM (NEW_Excitation_to_Formant, U"From Excitation to Formant", 0) {
	NATURAL (maximumNumberOfFormants, U"Maximum number of formants", U"20")
	OK
DO
	CONVERT_EACH_TO_ONE (Excitation)
		autoFormant result = Excitation_to_Formant (me, maximumNumberOfFormants);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Excitation_to_Matrix) {
	CONVERT_EACH_TO_ONE (Excitation)
		autoMatrix result = Excitation_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - FORMANT

// MARK: Help

DIRECT (HELP_Formant_help) {
	HELP (U"Formant")
}

// MARK: Draw

FORM (GRAPHICS_Formant_drawSpeckles, U"Draw Formant", U"Formant: Draw speckles...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	REAL (dynamicRange, U"Dynamic range (dB)", U"30.0")
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_drawSpeckles (me, GRAPHICS, fromTime, toTime, maximumFrequency, dynamicRange, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Formant_drawTracks, U"Draw formant tracks", U"Formant: Draw tracks...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (maximumFrequency, U"Maximum frequency (Hz)", U"5500.0")
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_drawTracks (me, GRAPHICS, fromTime, toTime, maximumFrequency, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Formant_scatterPlot, U"Formant: Scatter plot", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	NATURAL (horizontalFormantNumber, U"Horizontal formant number", U"2")
	REAL (left, U"left Horizontal range (Hz)", U"3000.0")
	REAL (right, U"right Horizontal range (Hz)", U"400.0")
	NATURAL (verticalFormantNumber, U"Vertical formant number", U"1")
	REAL (bottom, U"left Vertical range (Hz)", U"1500.0")
	REAL (top, U"right Vertical range (Hz)", U"100.0")
	POSITIVE (markSize, U"Mark size (mm)", U"1.0")
	BOOLEAN (garnish, U"Garnish", true)
	SENTENCE (markString, U"Mark string (+xo.)", U"+")
	OK
DO
	GRAPHICS_EACH (Formant)
		Formant_scatterPlot (me, GRAPHICS, fromTime, toTime,
			horizontalFormantNumber, left, right, verticalFormantNumber, bottom, top,
			markSize, markString, garnish
		);
	GRAPHICS_EACH_END
}

// MARK: Tabulate

FORM (LIST_Formant_list, U"Formant: List", nullptr) {
	BOOLEAN (includeFrameNumber, U"Include frame number", false)
	BOOLEAN (includeTime, U"Include time", true)
	NATURAL (numberOfTimeDecimals, U"Number of time decimals", U"6")
	BOOLEAN (includeIntensity, U"Include intensity", false)
	NATURAL (numberOfIntensityDecimals, U"Number of intensity decimals", U"3")
	BOOLEAN (includeNumberOfFormants, U"Include number of formants", true)
	NATURAL (numberOfFrequencyDecimals, U"Number of frequency decimals", U"3")
	BOOLEAN (includeBandwidths, U"Include bandwidths", true)
	OK
DO
	INFO_ONE (Formant)
		Formant_list (me, includeFrameNumber,
			includeTime, numberOfTimeDecimals,
			includeIntensity, numberOfIntensityDecimals,
			includeNumberOfFormants, numberOfFrequencyDecimals, includeBandwidths
		);
	INFO_ONE_END
}

FORM (NEW_Formant_downto_Table, U"Formant: Down to Table", nullptr) {
	BOOLEAN (includeFrameNumber, U"Include frame number", false)
	BOOLEAN (includeTime, U"Include time", true)
	NATURAL (numberOfTimeDecimals, U"Number of time decimals", U"6")
	BOOLEAN (includeIntensity, U"Include intensity", false)
	NATURAL (numberOfIntensityDecimals, U"Number of intensity decimals", U"3")
	BOOLEAN (includeNumberOfFormants, U"Include number of formants", true)
	NATURAL (numberOfFrequencyDecimals, U"Number of frequency decimals", U"3")
	BOOLEAN (includeBandwidths, U"Include bandwidths", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Formant)
		autoTable result = Formant_downto_Table (me, includeFrameNumber, includeTime, numberOfTimeDecimals,
			includeIntensity, numberOfIntensityDecimals, includeNumberOfFormants, numberOfFrequencyDecimals,
			includeBandwidths
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Query

FORM (REAL_Formant_getValueAtTime, U"Formant: Get value", U"Formant: Get value at time...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.5")
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICE (interpolation, U"Interpolation", 1)   // ignored
		OPTION (U"linear")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getValueAtTime (me, formantNumber, time, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getBandwidthAtTime, U"Formant: Get bandwidth", U"Formant: Get bandwidth at time...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	REAL (time, U"Time (s)", U"0.5")
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICE (interpolation, U"Interpolation", 1)   // ignored
		OPTION (U"linear")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getBandwidthAtTime (me, formantNumber, time, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getMinimum, U"Formant: Get minimum", U"Formant: Get minimum...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getMinimum (me, formantNumber, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getMaximum, U"Formant: Get maximum", U"Formant: Get maximum...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getMaximum (me, formantNumber, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getTimeOfMinimum, U"Formant: Get time of minimum", U"Formant: Get time of minimum...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getTimeOfMinimum (me, formantNumber, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Formant_getTimeOfMaximum, U"Formant: Get time of maximum", U"Formant: Get time of maximum...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getTimeOfMaximum (me, formantNumber, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

DIRECT (INTEGER_Formant_getMaximumNumberOfFormants) {
	QUERY_ONE_FOR_REAL (Formant)
		const integer result = Formant_getMaxNumFormants (me);
	QUERY_ONE_FOR_REAL_END (U" (there are at most this many formants in every frame)")
}

FORM (REAL_Formant_getMean, U"Formant: Get mean", U"Formant: Get mean...") {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getMean (me, formantNumber, fromTime, toTime, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

DIRECT (INTEGER_Formant_getMinimumNumberOfFormants) {
	QUERY_ONE_FOR_REAL (Formant)
		const integer result = Formant_getMinNumFormants (me);
	QUERY_ONE_FOR_REAL_END (U" (there are at least this many formants in every frame)");
}

FORM (INTEGER_Formant_getNumberOfFormants, U"Formant: Get number of formants", U"Formant: Get number of formants...") {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		if (frameNumber > my nx)
			Melder_throw (U"There is no frame ", frameNumber, U" in a Formant with only ", my nx, U" frames.");
		const integer result = my frames [frameNumber]. numberOfFormants;
	QUERY_ONE_FOR_REAL_END (U" formants")
}

FORM (REAL_Formant_getQuantile, U"Formant: Get quantile", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	REAL (quantile, U"Quantile", U"0.50 (= median)")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getQuantile (me, formantNumber, quantile, fromTime, toTime, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getQuantileOfBandwidth, U"Formant: Get quantile of bandwidth", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	REAL (quantile, U"Quantile", U"0.50 (= median)")
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getQuantileOfBandwidth (me, formantNumber, quantile, fromTime, toTime, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

FORM (REAL_Formant_getStandardDeviation, U"Formant: Get standard deviation", nullptr) {
	NATURAL (formantNumber, U"Formant number", U"1")
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE_ENUM (kFormant_unit, unit, U"Unit", kFormant_unit::HERTZ)
	OK
DO
	QUERY_ONE_FOR_REAL (Formant)
		const double result = Formant_getStandardDeviation (me, formantNumber, fromTime, toTime, unit);
	QUERY_ONE_FOR_REAL_END (U" ", kFormant_unit_getText (unit))
}

// MARK: Modify

DIRECT (MODIFY_Formant_sort) {
	MODIFY_EACH (Formant)
		Formant_sort (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Formant_formula_frequencies, U"Formant: Formula (frequencies)", U"Formant: Formula (frequencies)...") {
	COMMENT (U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do F (row, col) :=")
	FORMULA (formula, U"Formula", U"if row = 2 then self + 200 else self fi")
	OK
DO
	MODIFY_EACH_WEAK (Formant)
		Formant_formula_frequencies (me, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Formant_formula_bandwidths, U"Formant: Formula (bandwidths)", U"Formant: Formula (bandwidths)...") {
	COMMENT (U"row is formant number, col is frame number: for row from 1 to nrow do for col from 1 to ncol do B (row, col) :=")
	FORMULA (formula, U"Formula", U"self / 2 ; sharpen all peaks")
	OK
DO
	MODIFY_EACH_WEAK (Formant)
		Formant_formula_bandwidths (me, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

// MARK: Convert

FORM (NEW_Formant_tracker, U"Formant tracker", U"Formant: Track...") {
	NATURAL (numberOfTracks, U"Number of tracks (1-5)", U"3")
	REAL (referenceF1, U"Reference F1 (Hz)", U"550")
	REAL (referenceF2, U"Reference F2 (Hz)", U"1650")
	REAL (referenceF3, U"Reference F3 (Hz)", U"2750")
	REAL (referenceF4, U"Reference F4 (Hz)", U"3850")
	REAL (referenceF5, U"Reference F5 (Hz)", U"4950")
	REAL (frequencyCost, U"Frequency cost (/kHz)", U"1.0")
	REAL (bandwidthCost, U"Bandwidth cost", U"1.0")
	REAL (transitionCost, U"Transition cost (/octave)", U"1.0")
	OK
DO
	if (numberOfTracks > 5) Melder_throw (U"Your number of tracks should not be more than 5.");
	CONVERT_EACH_TO_ONE (Formant)
		autoFormant result = Formant_tracker (me, numberOfTracks,
			referenceF1, referenceF2, referenceF3, referenceF4, referenceF5,
			frequencyCost, bandwidthCost, transitionCost
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Formant_downto_FormantGrid) {
	CONVERT_EACH_TO_ONE (Formant)
		autoFormantGrid result = Formant_downto_FormantGrid (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Formant_downto_FormantTier) {
	CONVERT_EACH_TO_ONE (Formant)
		autoFormantTier result = Formant_downto_FormantTier (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Formant_to_Matrix, U"From Formant to Matrix", nullptr) {
	INTEGER (formantNumber, U"Formant number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Formant)
		autoMatrix result = Formant_to_Matrix (me, formantNumber);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - FORMANT & POINTPROCESS

DIRECT (NEW1_Formant_PointProcess_to_FormantTier) {
	CONVERT_ONE_AND_ONE_TO_ONE (Formant, PointProcess)
		autoFormantTier result = Formant_PointProcess_to_FormantTier (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

// MARK: - FORMANT & SOUND

DIRECT (NEW1_Sound_Formant_filter) {
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Formant)
		autoSound result = Sound_Formant_filter (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_filt")
}

DIRECT (NEW1_Sound_Formant_filter_noscale) {
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Formant)
		autoSound result = Sound_Formant_filter_noscale (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_filt")
}

// MARK: - HARMONICITY

// MARK: Help

DIRECT (HELP_Harmonicity_help) {
	HELP (U"Harmonicity")
}

// MARK: Draw

FORM (GRAPHICS_Harmonicity_draw, U"Draw harmonicity", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (minimum, U"Minimum", U"0.0")
	REAL (maximum, U"Maximum", U"0.0 (= auto)")
	OK
DO
	GRAPHICS_EACH (Harmonicity);
		Matrix_drawRows (me, GRAPHICS, fromTime, toTime, 0.0, 0.0, minimum, maximum);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (REAL_Harmonicity_getMaximum, U"Harmonicity: Get maximum", U"Harmonicity: Get maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Vector_getMaximum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Harmonicity_getMean, U"Harmonicity: Get mean", U"Harmonicity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Harmonicity_getMean (me, fromTime, toTime);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Harmonicity_getMinimum, U"Harmonicity: Get minimum", U"Harmonicity: Get minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Vector_getMinimum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Harmonicity_getStandardDeviation, U"Harmonicity: Get standard deviation", U"Harmonicity: Get standard deviation...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Harmonicity_getStandardDeviation (me, fromTime, toTime);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Harmonicity_getTimeOfMaximum, U"Harmonicity: Get time of maximum", U"Harmonicity: Get time of maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Vector_getXOfMaximum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Harmonicity_getTimeOfMinimum, U"Harmonicity: Get time of minimum", U"Harmonicity: Get time of minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Vector_getXOfMinimum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Harmonicity_getValueAtTime, U"Harmonicity: Get value", U"Harmonicity: Get value at time...") {
	praat_TimeVector_INTERPOLATED_VALUE (time, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = Vector_getValueAtX (me, time, 1, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Harmonicity_getValueInFrame, U"Get value in frame", U"Harmonicity: Get value in frame...") {
	INTEGER (frameNumber, U"Frame number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL (Harmonicity)
		const double result = ( frameNumber < 1 || frameNumber > my nx ? undefined : my z [1] [frameNumber] );
	QUERY_ONE_FOR_REAL_END (U" dB")
}

// MARK: Modify

FORM (MODIFY_Harmonicity_formula, U"Harmonicity Formula", U"Harmonicity: Formula...") {
	COMMENT (U"`x` is time")
	COMMENT (U"for col := 1 to ncol do { self [col] := `formula` ; x := x + dx }")
	FORMULA (formula, U"Formula", U"self")
	OK
DO
	MODIFY_EACH_WEAK (Harmonicity)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Convert

DIRECT (NEW_Harmonicity_to_Matrix) {
	CONVERT_EACH_TO_ONE (Harmonicity)
		autoMatrix result = Harmonicity_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - INTENSITY

// MARK: Help

DIRECT (HELP_Intensity_help) {
	HELP (U"Intensity")
}

// MARK: Draw

FORM (GRAPHICS_Intensity_draw, U"Draw Intensity", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (minimum, U"Minimum (dB)", U"0.0")
	REAL (maximum, U"Maximum (dB)", U"0.0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Intensity)
		Intensity_draw (me, GRAPHICS, fromTime, toTime, minimum, maximum, garnish);
	GRAPHICS_EACH_END
}

// MARK: Query

FORM (REAL_Intensity_getValueAtTime, U"Intensity: Get value", U"Intensity: Get value at time...") {
	praat_TimeVector_INTERPOLATED_VALUE (time, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getValueAtX (me, time, 1, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getValueInFrame, U"Get value in frame", U"Intensity: Get value in frame...") {
	INTEGER (frameNumber, U"Frame number", U"10")
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = ( frameNumber < 1 || frameNumber > my nx ? undefined : my z [1] [frameNumber] );
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getMinimum, U"Intensity: Get minimum", U"Intensity: Get minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getMinimum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getTimeOfMinimum, U"Intensity: Get time of minimum", U"Intensity: Get time of minimum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getXOfMinimum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Intensity_getMaximum, U"Intensity: Get maximum", U"Intensity: Get maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getMaximum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getTimeOfMaximum, U"Intensity: Get time of maximum", U"Intensity: Get time of maximum...") {
	praat_TimeVector_INTERPOLATED_EXTREMUM (fromTime, toTime, interpolation)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getXOfMaximum (me, fromTime, toTime, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Intensity_getQuantile, U"Intensity: Get quantile", 0) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (quantile, U"Quantile (0-1)", U"0.50")
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Intensity_getQuantile (me, fromTime, toTime, quantile);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_old_Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Sampled_getMean_standardUnit (me, fromTime, toTime, 0, 0, true);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getMean, U"Intensity: Get mean", U"Intensity: Get mean...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	CHOICE (averagingMethod, U"Averaging method", 1)
		OPTION (U"energy")
		OPTION (U"sones")
		OPTION (U"dB")
	OK
DO_ALTERNATIVE (REAL_old_Intensity_getMean)
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Sampled_getMean_standardUnit (me, fromTime, toTime, 0, averagingMethod, true);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Intensity_getStandardDeviation, U"Intensity: Get standard deviation", U"Intensity: Get standard deviation...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OK
DO
	QUERY_ONE_FOR_REAL (Intensity)
		const double result = Vector_getStandardDeviation (me, fromTime, toTime, 1);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

// MARK: Modify

FORM (MODIFY_Intensity_formula, U"Intensity Formula", nullptr) {
	COMMENT (U"`x` is the time in seconds, `col` is the frame number, `self` is in dB")
	COMMENT (U"x := x1;   for col := 1 to ncol do { self [col] := `formula` ; x := x + dx }")
	FORMULA (formula, U"Formula", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Intensity)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Analyse

DIRECT (NEW_Intensity_to_IntensityTier_peaks) {
	CONVERT_EACH_TO_ONE (Intensity)
		autoIntensityTier result = Intensity_to_IntensityTier_peaks (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Intensity_to_IntensityTier_valleys) {
	CONVERT_EACH_TO_ONE (Intensity)
		autoIntensityTier result = Intensity_to_IntensityTier_valleys (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Convert

DIRECT (NEW_Intensity_downto_IntensityTier) {
	CONVERT_EACH_TO_ONE (Intensity)
		autoIntensityTier result = Intensity_downto_IntensityTier (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Intensity_downto_Matrix) {
	CONVERT_EACH_TO_ONE (Intensity)
		autoMatrix result = Intensity_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - INTENSITY & PITCH

FORM (GRAPHICS_Pitch_Intensity_draw, U"Plot intensity by pitch", nullptr) {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= auto)")
	REAL (fromIntensity, U"From intensity (dB)", U"0.0")
	REAL (toIntensity, U"To intensity (dB)", U"100.0")
	BOOLEAN (garnish, U"Garnish", true)
	CHOICE (drawingMethod, U"Drawing method", 1)
		OPTION (U"speckles")
		OPTION (U"curve")
		OPTION (U"speckles and curve")
	OK
DO
	GRAPHICS_ONE_AND_ONE (Pitch, Intensity)
		Pitch_Intensity_draw (me, you, GRAPHICS,
				fromFrequency, toFrequency, fromIntensity, toIntensity, garnish, drawingMethod);
	GRAPHICS_ONE_AND_ONE_END
}

DIRECT (REAL_Pitch_Intensity_getMean) {
	QUERY_ONE_AND_ONE_FOR_REAL (Pitch, Intensity)
		const double result = Pitch_Intensity_getMean (me, you);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" dB")
}

DIRECT (REAL_Pitch_Intensity_getMeanAbsoluteSlope) {
	QUERY_ONE_AND_ONE_FOR_REAL (Pitch, Intensity)
		const double result = Pitch_Intensity_getMeanAbsoluteSlope (me, you);
	QUERY_ONE_AND_ONE_FOR_REAL_END (U" dB/second")
}

// MARK: - INTENSITY & POINTPROCESS

DIRECT (NEW1_Intensity_PointProcess_to_IntensityTier) {
	CONVERT_ONE_AND_ONE_TO_ONE (Intensity, PointProcess)
		autoIntensityTier result = Intensity_PointProcess_to_IntensityTier (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

// MARK: - LTAS

DIRECT (NEW1_Ltases_average) {
	COMBINE_ALL_LISTED_TO_ONE (Ltas, LtasBag)
		autoLtas result = Ltases_average (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"averaged")
}

FORM (NEW_Ltas_computeTrendLine, U"Ltas: Compute trend line", U"Ltas: Compute trend line...") {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"600.0")
	POSITIVE (toFrequency, U"right Frequency range (Hz)", U"4000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Ltas)
		autoLtas result = Ltas_computeTrendLine (me, fromFrequency, toFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_trend")
}

FORM (GRAPHICS_old_Ltas_draw, U"Ltas: Draw", nullptr) {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (fromPower, U"left Power range (dB/Hz)", U"-20.0")
	REAL (toPower, U"right Power range (dB/Hz)", U"80.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Ltas)
		Ltas_draw (me, GRAPHICS, fromFrequency, toFrequency, fromPower, toPower, garnish, U"Bars");
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Ltas_draw, U"Ltas: Draw", nullptr) {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (fromPower, U"left Power range (dB/Hz)", U"-20.0")
	REAL (toPower, U"right Power range (dB/Hz)", U"80.0")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 2)
		OPTION (U"curve")
		OPTION (U"bars")
		OPTION (U"poles")
		OPTION (U"speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_Ltas_draw)
	GRAPHICS_EACH (Ltas)
		Ltas_draw (me, GRAPHICS, fromFrequency, toFrequency, fromPower, toPower, garnish, drawingMethod);
	GRAPHICS_EACH_END
}

FORM (MODIFY_Ltas_formula, U"Ltas Formula", nullptr) {
	COMMENT (U"`x` is the frequency in hertz, `col` is the bin number")
	COMMENT (U"x := x1;   for col := 1 to ncol do { self [1, col] := `formula` ; x := x + dx }")
	FORMULA (formula, U"Formula", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Ltas)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

DIRECT (REAL_Ltas_getLowestFrequency) {
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

DIRECT (REAL_Ltas_getHighestFrequency) {
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

DIRECT (REAL_Ltas_getBinWidth) {
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

FORM (REAL_Ltas_getFrequencyFromBinNumber, U"Ltas: Get frequency from bin number", U"Ltas: Get frequency from bin number...") {
	NATURAL (binNumber, U"Bin number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Sampled_indexToX (me, binNumber);
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

FORM (REAL_Ltas_getBinNumberFromFrequency, U"Ltas: Get band from frequency", U"Ltas: Get band from frequency...") {
	REAL (frequency, U"Frequency (Hz)", U"2000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Sampled_xToIndex (me, frequency);
	QUERY_ONE_FOR_REAL_END (U"")
}

FORM (REAL_Ltas_getFrequencyOfMinimum, U"Ltas: Get frequency of minimum", U"Ltas: Get frequency of minimum...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::NONE)
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Vector_getXOfMinimum (me, fromFrequency, toFrequency, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" hertz");
}

FORM (REAL_Ltas_getFrequencyOfMaximum, U"Ltas: Get frequency of maximum", U"Ltas: Get frequency of maximum...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::NONE)
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Vector_getXOfMaximum (me, fromFrequency, toFrequency, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" hertz");
}

FORM (REAL_Ltas_getLocalPeakHeight, U"Ltas: Get local peak height", nullptr) {
	REAL (environmentMin, U"left Environment (Hz)", U"1700.0")
	REAL (environmentMax, U"right Environment (Hz)", U"4200.0")
	REAL (peakMin, U"left Peak (Hz)", U"2400.0")
	REAL (peakMax, U"right Peak (Hz)", U"3200.0")
	CHOICE (averagingMethod, U"Averaging method", 1)
		OPTION (U"energy")
		OPTION (U"sones")
		OPTION (U"dB")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		if (environmentMin >= peakMin)
			Melder_throw (U"The beginning of the environment must lie before the peak.");
		if (peakMin >= peakMax)
			Melder_throw (U"The end of the peak must lie after its beginning.");
		if (environmentMax <= peakMax)
			Melder_throw (U"The end of the environment must lie after the peak.");
		double result = Ltas_getLocalPeakHeight (me, environmentMin, environmentMax, peakMin, peakMax, averagingMethod);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getMaximum, U"Ltas: Get maximum", U"Ltas: Get maximum...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::NONE)
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Vector_getMaximum (me, fromFrequency, toFrequency, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getMean, U"Ltas: Get mean", U"Ltas: Get mean...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE (averagingMethod, U"Averaging method", 1)
		OPTION (U"energy")
		OPTION (U"sones")
		OPTION (U"dB")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Sampled_getMean_standardUnit (me, fromFrequency, toFrequency,
				0, averagingMethod, false);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getMinimum, U"Ltas: Get minimum", U"Ltas: Get minimum...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE_ENUM (kVector_peakInterpolation, peakInterpolationType,
			U"Interpolation", kVector_peakInterpolation::NONE)
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Vector_getMinimum (me, fromFrequency, toFrequency, peakInterpolationType);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

DIRECT (INTEGER_Ltas_getNumberOfBins) {
	QUERY_ONE_FOR_REAL (Ltas)
		const integer result = my nx;
	QUERY_ONE_FOR_REAL_END (U" bins")
}

FORM (REAL_Ltas_getSlope, U"Ltas: Get slope", 0) {
	REAL (lowBandFrom, U"left Low band (Hz)", U"0.0")
	REAL (lowBandTo, U"right Low band (Hz)", U"1000.0")
	REAL (highBandFrom, U"left High band (Hz)", U"1000.0")
	REAL (highBandTo, U"right High band (Hz)", U"4000.0")
	CHOICE (averagingMethod, U"Averaging method", 1)
		OPTION (U"energy")
		OPTION (U"sones")
		OPTION (U"dB")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Ltas_getSlope (me, lowBandFrom, lowBandTo, highBandFrom, highBandTo, averagingMethod);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getStandardDeviation, U"Ltas: Get standard deviation", U"Ltas: Get standard deviation...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"0.0 (= all)")
	CHOICE (averagingMethod, U"Averaging method", 1)
		OPTION (U"energy")
		OPTION (U"sones")
		OPTION (U"dB")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Sampled_getStandardDeviation_standardUnit (me, fromFrequency, toFrequency,
			0,   // level (irrelevant)
			averagingMethod,
			false   // interpolate (don't)
		);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getValueAtFrequency, U"Ltas: Get value", U"Ltas: Get value at frequency...") {
	REAL (frequency, U"Frequency (Hz)", U"1500.0")
	CHOICE_ENUM (kVector_valueInterpolation, valueInterpolationType,
			U"Interpolation", kVector_valueInterpolation :: NEAREST)
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = Vector_getValueAtX (me, frequency,
			1,   // level
			valueInterpolationType
		);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Ltas_getValueInBin, U"Get value in bin", U"Ltas: Get value in bin...") {
	INTEGER (binNumber, U"Bin number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (Ltas)
		const double result = binNumber < 1 || binNumber > my nx ? undefined : my z [1] [binNumber];
	QUERY_ONE_FOR_REAL_END (U" dB")
}

DIRECT (HELP_Ltas_help) {
	HELP (U"Ltas")
}

DIRECT (NEW1_Ltases_merge) {
	COMBINE_ALL_LISTED_TO_ONE (Ltas, LtasBag)
		autoLtas result = Ltases_merge (list.get());
	COMBINE_ALL_LISTED_TO_ONE_END (U"merged")
}

FORM (NEW_Ltas_subtractTrendLine, U"Ltas: Subtract trend line", U"Ltas: Subtract trend line...") {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"600.0")
	POSITIVE (toFrequency, U"right Frequency range (Hz)", U"4000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Ltas)
		autoLtas result = Ltas_subtractTrendLine (me, fromFrequency, toFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_fit")
}

DIRECT (NEW_Ltas_to_Matrix) {
	CONVERT_EACH_TO_ONE (Ltas)
		autoMatrix result = Ltas_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Ltas_to_SpectrumTier_peaks) {
	CONVERT_EACH_TO_ONE (Ltas)
		autoSpectrumTier result = Ltas_to_SpectrumTier_peaks (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - MANIPULATION

static void cb_ManipulationEditor_publication (Editor /* editor */, autoDaata publication) {
	try {
		praat_new (publication.move(), U"fromManipulationEditor");
		praat_updateSelection ();
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (EDITOR_ONE_Manipulation_viewAndEdit) {
	EDITOR_ONE (a,Manipulation)
		autoManipulationEditor editor = ManipulationEditor_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_ManipulationEditor_publication);
	EDITOR_ONE_END
}

DIRECT (NEW_Manipulation_extractDurationTier) {
	CONVERT_EACH_TO_ONE (Manipulation)
		if (! my duration)
			Melder_throw (me, U": I don't contain a DurationTier.");
		autoDurationTier result = Data_copy (my duration.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Manipulation_extractOriginalSound) {
	CONVERT_EACH_TO_ONE (Manipulation)
		if (! my sound)
			Melder_throw (me, U": I don't contain a Sound.");
		autoSound result = Data_copy (my sound.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Manipulation_extractPitchTier) {
	CONVERT_EACH_TO_ONE (Manipulation)
		if (! my pitch)
			Melder_throw (me, U": I don't contain a PitchTier.");
		autoPitchTier result = Data_copy (my pitch.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Manipulation_extractPulses) {
	CONVERT_EACH_TO_ONE (Manipulation)
		if (! my pulses)
			Melder_throw (me, U": I don't contain a PointProcess.");
		autoPointProcess result = Data_copy (my pulses.get());
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Manipulation_getResynthesis_lpc) {
	CONVERT_EACH_TO_ONE (Manipulation)
		autoSound result = Manipulation_to_Sound (me, Manipulation_PITCH_LPC);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Manipulation_getResynthesis_overlapAdd) {
	CONVERT_EACH_TO_ONE (Manipulation)
		autoSound result = Manipulation_to_Sound (me, Manipulation_OVERLAPADD);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (HELP_Manipulation_help) {
	HELP (U"Manipulation")
}

DIRECT (PLAY_Manipulation_play_lpc) {
	PLAY_EACH (Manipulation)
		Manipulation_play (me, Manipulation_PITCH_LPC);
	PLAY_EACH_END
}

DIRECT (PLAY_Manipulation_play_overlapAdd) {
	PLAY_EACH (Manipulation)
		Manipulation_play (me, Manipulation_OVERLAPADD);
	PLAY_EACH_END
}

DIRECT (MODIFY_Manipulation_removeDuration) {
	MODIFY_EACH (Manipulation)
		my duration = autoDurationTier();
	MODIFY_EACH_END
}

DIRECT (MODIFY_Manipulation_removeOriginalSound) {
	MODIFY_EACH (Manipulation)
		my sound = autoSound();
	MODIFY_EACH_END
}

FORM_SAVE (SAVE_Manipulation_writeToBinaryFileWithoutSound, U"Binary file without Sound", nullptr, nullptr) {
	SAVE_ONE (Manipulation, U"save the selected Manipulation object to the sound-less binary file")
		Manipulation_writeToBinaryFileWithoutSound (me, file);
	SAVE_ONE_END
}

FORM_SAVE (SAVE_Manipulation_writeToTextFileWithoutSound, U"Text file without Sound", nullptr, nullptr) {
	SAVE_ONE (Manipulation, U"save the selected Manipulation object to the sound-less text file")
		Manipulation_writeToTextFileWithoutSound (me, file);
	SAVE_ONE_END
}

// MARK: - MANIPULATION & DURATIONTIER

DIRECT (MODIFY_Manipulation_replaceDurationTier) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Manipulation, DurationTier)
		Manipulation_replaceDurationTier (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (HELP_Manipulation_replaceDurationTier_help) {
	HELP (U"Manipulation: Replace duration tier")
}

// MARK: - MANIPULATION & PITCHTIER

DIRECT (MODIFY_Manipulation_replacePitchTier) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Manipulation, PitchTier)
		Manipulation_replacePitchTier (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

DIRECT (HELP_Manipulation_replacePitchTier_help) {
	HELP (U"Manipulation: Replace pitch tier")
}

// MARK: - MANIPULATION & POINTPROCESS

DIRECT (MODIFY_Manipulation_replacePulses) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Manipulation, PointProcess)
		Manipulation_replacePulses (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

// MARK: - MANIPULATION & SOUND

DIRECT (MODIFY_Manipulation_replaceOriginalSound) {
	MODIFY_FIRST_OF_ONE_AND_ONE (Manipulation, Sound)
		Manipulation_replaceOriginalSound (me, you);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

// MARK: - MANIPULATION & TEXTTIER

DIRECT (NEW1_Manipulation_TextTier_to_Manipulation) {
	CONVERT_ONE_AND_ONE_TO_ONE (Manipulation, TextTier)
		autoManipulation result = Manipulation_AnyTier_to_Manipulation (me, reinterpret_cast <AnyTier> (you));
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get())
}

// MARK: - PARAMCURVE

FORM (GRAPHICS_ParamCurve_draw, U"Draw parametrized curve", nullptr) {
	REAL (tmin, U"Tmin", U"0.0")
	REAL (tmax, U"Tmax", U"0.0")
	REAL (step, U"Step", U"0.0")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ParamCurve)
		ParamCurve_draw (me, GRAPHICS, tmin, tmax, step, xmin, xmax, ymin, ymax, garnish);
	GRAPHICS_EACH_END
}

DIRECT (HELP_ParamCurve_help) {
	HELP (U"ParamCurve")
}

// MARK: - PITCH

DIRECT (INTEGER_Pitch_getNumberOfVoicedFrames) {
	QUERY_ONE_FOR_REAL (Pitch)
		integer result = Pitch_countVoicedFrames (me);
	QUERY_ONE_FOR_REAL_END (U" voiced frames")
}

DIRECT (INFO_Pitch_difference) {
	INFO_TWO (Pitch)
		Pitch_difference (me, you);
	INFO_TWO_END
}

FORM (GRAPHICS_Pitch_draw, U"Pitch: Draw", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::HERTZ);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawErb, U"Pitch: Draw erb", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::ERB);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawLogarithmic, U"Pitch: Draw logarithmic", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawMel, U"Pitch: Draw mel", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::MEL);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones100, U"Pitch: Draw semitones (re 100 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 100 Hz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::SEMITONES_100);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones200, U"Pitch: Draw semitones (re 200 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 200 Hz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-24.0")
	REAL (toFrequency, U"right Frequency range (st)", U"18.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::SEMITONES_200);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_drawSemitones440, U"Pitch: Draw semitones (re 440 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 440 Hz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-36.0")
	REAL (toFrequency, U"right Frequency range (st)", U"6.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				garnish, Pitch_speckle_NO, kPitch_unit::SEMITONES_440);
	GRAPHICS_EACH_END
}

DIRECT (EDITOR_ONE_Pitch_viewAndEdit) {
	EDITOR_ONE (a,Pitch)
		autoPitchEditor editor = PitchEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

FORM (MODIFY_Pitch_formula, U"Pitch: Formula", U"Formula...") {
	COMMENT (U"`x` is time; `col` is frame number; `row` = candidate (1 = current path); frequency (time, candidate) :=")
	FORMULA (formula, U"Formula", U"self*2 ; example: octave jump up")
	OK
DO
	MODIFY_EACH_WEAK (Pitch)
		Pitch_formula (me, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (NUMMAT_Pitch_getAllCandidatesInFrame, U"Pitch: Get all candidates in frame", nullptr) {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	QUERY_ONE_FOR_MATRIX (Pitch)
		autoMAT result = Pitch_getAllCandidatesInFrame (me, frameNumber);
	QUERY_ONE_FOR_MATRIX_END
}

FORM (NEW_Pitch_tabulateCandidatesInFrame, U"Pitch: Tabulate candidates in frame", nullptr) {
	NATURAL (frameNumber, U"Frame number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Pitch)
		autoTable result = Pitch_tabulateCandidatesInFrame (me, frameNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_", frameNumber)
}

DIRECT (NEW_Pitch_tabulateCandidates) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoTable result = Pitch_tabulateCandidates (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (REAL_Pitch_getMinimum, U"Pitch: Get minimum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Pitch_getMinimum (me, fromTime, toTime, unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0))
}

FORM (REAL_Pitch_getMaximum, U"Pitch: Get maximum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Pitch_getMaximum (me, fromTime, toTime, unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0))
}

FORM (REAL_Pitch_getMean, U"Pitch: Get mean", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Pitch_getMean (me, fromTime, toTime, unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0));
}

FORM (REAL_Pitch_getMeanAbsoluteSlope, U"Pitch: Get mean absolute slope", 0) {
	CHOICE (unit, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double slope;
		const integer nVoiced = (unit == 1 ? Pitch_getMeanAbsSlope_hertz : unit == 2 ? Pitch_getMeanAbsSlope_mel : unit == 3 ? Pitch_getMeanAbsSlope_semitones : Pitch_getMeanAbsSlope_erb)
			(me, & slope);
		const double result = ( nVoiced < 2 ? undefined : slope );
	QUERY_ONE_FOR_REAL_END (U" ", ( unit == 1 ? U"Hz" : unit == 2 ? U"mel" : unit == 3 ? U"semitones" : U"ERB" ), U"/s");
}

DIRECT (REAL_Pitch_getMeanAbsSlope_noOctave) {
	QUERY_ONE_FOR_REAL (Pitch)
		double result;
		(void) Pitch_getMeanAbsSlope_noOctave (me, & result);
	QUERY_ONE_FOR_REAL_END (U" semitones/s")
}

FORM (REAL_Pitch_getQuantile, U"Pitch: Get quantile", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (quantile, U"Quantile", U"0.50 (= median)")
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Sampled_getQuantile (me, fromTime, toTime, quantile, Pitch_LEVEL_FREQUENCY, (int) unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0))
}

FORM (REAL_Pitch_getStandardDeviation, U"Pitch: Get standard deviation", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU (unit_i, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK
DO
	const kPitch_unit unit =
		unit_i == 1 ? kPitch_unit::HERTZ :
		unit_i == 2 ? kPitch_unit::MEL :
		unit_i == 3 ? kPitch_unit::LOG_HERTZ :
		unit_i == 4 ? kPitch_unit::SEMITONES_1 :
		kPitch_unit::ERB;
	QUERY_ONE_FOR_REAL (Pitch)
		const double result = Pitch_getStandardDeviation (me, fromTime, toTime, unit);
		conststring32 unitText =
			unit == kPitch_unit::HERTZ ? U"Hz" :
			unit == kPitch_unit::MEL ? U"mel" :
			unit == kPitch_unit::LOG_HERTZ ? U"logHz" :
			unit == kPitch_unit::SEMITONES_1 ? U"semitones" :
			U"ERB";
	QUERY_ONE_FOR_REAL_END (U" ", unitText)
}

FORM (REAL_Pitch_getTimeOfMaximum, U"Pitch: Get time of maximum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		const double result = Pitch_getTimeOfMaximum (me, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Pitch_getTimeOfMinimum, U"Pitch: Get time of minimum", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"none")
		OPTION (U"parabolic")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		const double result = Pitch_getTimeOfMinimum (me, fromTime, toTime, unit, interpolation);
	QUERY_ONE_FOR_REAL_END (U" seconds")
}

FORM (REAL_Pitch_getValueAtTime, U"Pitch: Get value at time", U"Pitch: Get value at time...") {
	REAL (time, U"Time (s)", U"0.5")
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"nearest")
		OPTION (U"linear")
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Sampled_getValueAtX (me, time, Pitch_LEVEL_FREQUENCY, (int) unit, interpolation);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0))
}
	
FORM (REAL_Pitch_getValueInFrame, U"Pitch: Get value in frame", U"Pitch: Get value in frame...") {
	INTEGER (frameNumber, U"Frame number", U"10")
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL (Pitch)
		double result = Sampled_getValueAtSample (me, frameNumber, Pitch_LEVEL_FREQUENCY, (int) unit);
		result = Function_convertToNonlogarithmic (me, result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_END (U" ", Function_getUnitText (me, Pitch_LEVEL_FREQUENCY, (int) unit, 0));
}

DIRECT (HELP_Pitch_help) {
	HELP (U"Pitch")
}

DIRECT (PLAY_Pitch_hum) {
	PLAY_EACH (Pitch)
		Pitch_hum (me, 0.0, 0.0, nullptr, nullptr);
	PLAY_EACH_END
}

DIRECT (NEW_Pitch_interpolate) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitch result = Pitch_interpolate (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_killOctaveJumps) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitch result = Pitch_killOctaveJumps (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NUMVEC_Pitch_listValuesAtTimes, U"Pitch: List values at times", U"Pitch: List values at times...") {
	REALVECTOR (times, U"Times (s)", WHITESPACE_SEPARATED_, U"0.5 0.7 2.0")
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	CHOICEx (interpolation, U"Interpolation", 2, 0)
		OPTION (U"nearest")
		OPTION (U"linear")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Pitch)
		autoVEC result = Sampled_listValuesAtXes (me, times, Pitch_LEVEL_FREQUENCY, (int) unit, interpolation);
		for (integer iframe = 1; iframe <= result.size; iframe ++)
			result [iframe] = Function_convertToNonlogarithmic (me, result [iframe], Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (NUMVEC_Pitch_listValuesInAllFrames, U"Pitch: List values in all frames", U"Pitch: List values in all frames...") {
	OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", kPitch_unit::DEFAULT)
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Pitch)
		autoVEC result = Sampled_listValuesOfAllSamples (me, Pitch_LEVEL_FREQUENCY, (int) unit);
		for (integer iframe = 1; iframe <= result.size; iframe ++)
			result [iframe] = Function_convertToNonlogarithmic (me, result [iframe], Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

DIRECT (PLAY_Pitch_play) {
	PLAY_EACH (Pitch)
		Pitch_play (me, 0.0, 0.0);
	PLAY_EACH_END
}

FORM (NEW_Pitch_smooth, U"Pitch: Smooth", U"Pitch: Smooth...") {
	REAL (bandwidth, U"Bandwidth (Hz)", U"10.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitch result = Pitch_smooth (me, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_Pitch_speckle, U"Pitch: Speckle", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"0.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Maximum frequency should be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleErb, U"Pitch: Speckle erb", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (ERB)", U"0.0")
	REAL (toFrequency, U"right Frequency range (ERB)", U"10.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::ERB);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleLogarithmic, U"Pitch: Speckle logarithmic", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromFrequency, STRING_FROM_FREQUENCY_HZ, U"50.0")
	POSITIVE (toFrequency, STRING_TO_FREQUENCY_HZ, U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	if (toFrequency <= fromFrequency)
		Melder_throw (U"Maximum frequency must be greater than minimum frequency.");
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::HERTZ_LOGARITHMIC);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleMel, U"Pitch: Speckle mel", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (mel)", U"0.0")
	REAL (toFrequency, U"right Frequency range (mel)", U"500.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::MEL);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones100, U"Pitch: Speckle semitones (re 100 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 100 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-12.0")
	REAL (toFrequency, U"right Frequency range (st)", U"30.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::SEMITONES_100);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones200, U"Pitch: Speckle semitones (re 200 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 200 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-24.0")
	REAL (toFrequency, U"right Frequency range (st)", U"18.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::SEMITONES_200);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Pitch_speckleSemitones440, U"Pitch: Speckle semitones (re 440 Hz)", U"Pitch: Draw...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	COMMENT (U"Range in semitones re 440 hertz:")
	REAL (fromFrequency, U"left Frequency range (st)", U"-36.0")
	REAL (toFrequency, U"right Frequency range (st)", U"6.0")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Pitch)
		Pitch_draw (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency, garnish, Pitch_speckle_YES, kPitch_unit::SEMITONES_440);
	GRAPHICS_EACH_END
}

FORM (NEW_Pitch_subtractLinearFit, U"Pitch: subtract linear fit", nullptr) {
	OPTIONMENU (unit_i, U"Unit", 1)
		OPTION (U"Hertz")
		OPTION (U"mel")
		OPTION (U"logHertz")
		OPTION (U"semitones")
		OPTION (U"ERB")
	OK
DO
	const kPitch_unit unit =
		unit_i == 1 ? kPitch_unit::HERTZ :
		unit_i == 2 ? kPitch_unit::MEL :
		unit_i == 3 ? kPitch_unit::LOG_HERTZ :
		unit_i == 4 ? kPitch_unit::SEMITONES_1 :
		kPitch_unit::ERB;
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitch result = Pitch_subtractLinearFit (me, unit);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_IntervalTier) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoIntervalTier result = IntervalTier_create (my xmin, my xmax);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_Matrix) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoMatrix result = Pitch_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_downto_PitchTier) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoPitchTier result = Pitch_to_PitchTier (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_PointProcess) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoPointProcess result = Pitch_to_PointProcess (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_Sound_pulses) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoSound result = Pitch_to_Sound (me, 0.0, 0.0, false);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_Sound_hum) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoSound result = Pitch_to_Sound (me, 0.0, 0.0, true);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Pitch_to_Sound_sine, U"Pitch: To Sound (sine)", nullptr) {
	POSITIVE (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	CHOICEx (cutVoicelessStretches, U"Cut voiceless stretches", 2, 0)
		OPTION (U"exactly")
		OPTION (U"at nearest zero crossings")
	OK
DO
	CONVERT_EACH_TO_ONE (Pitch)
		autoSound result = Pitch_to_Sound_sine (me, 0.0, 0.0, samplingFrequency, cutVoicelessStretches);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Pitch_to_TextGrid, U"To TextGrid...", U"Pitch: To TextGrid...") {
	SENTENCE (tierNames, U"Tier names", U"Mary John bell")
	SENTENCE (pointTiers, U"Point tiers", U"bell")
	OK
DO
	CONVERT_EACH_TO_ONE (Pitch)
		autoTextGrid result = TextGrid_create (my xmin, my xmax, tierNames, pointTiers);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Pitch_to_TextTier) {
	CONVERT_EACH_TO_ONE (Pitch)
		autoTextTier result = TextTier_create (my xmin, my xmax);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - PITCH & PITCHTIER

FORM (GRAPHICS_old_PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"500.0")
	CHOICEx (lineTypeForNonperiodicIntervals, U"Line type for non-periodic intervals", 2, 0)
		OPTION (U"normal")
		OPTION (U"dotted")
		OPTION (U"blank")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_ONE_AND_ONE (PitchTier, Pitch)
		PitchTier_Pitch_draw (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				lineTypeForNonperiodicIntervals, garnish, U"lines and speckles");
	GRAPHICS_ONE_AND_ONE_END
}

FORM (GRAPHICS_PitchTier_Pitch_draw, U"PitchTier & Pitch: Draw", nullptr) {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"From frequency (Hz)", U"0.0")
	REAL (toFrequency, U"To frequency (Hz)", U"500.0")
	CHOICEx (lineTypeForNonperiodicIntervals, U"Line type for non-periodic intervals", 2, 0)
		OPTION (U"normal")
		OPTION (U"dotted")
		OPTION (U"blank")
	BOOLEAN (garnish, U"Garnish", true)
	COMMENT (U"")
	OPTIONMENUSTR (drawingMethod, U"Drawing method", 1)
		OPTION (U"lines")
		OPTION (U"speckles")
		OPTION (U"lines and speckles")
	OK
DO_ALTERNATIVE (GRAPHICS_old_PitchTier_Pitch_draw)
	GRAPHICS_ONE_AND_ONE (PitchTier, Pitch)
		PitchTier_Pitch_draw (me, you, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
				lineTypeForNonperiodicIntervals, garnish, drawingMethod);
	GRAPHICS_ONE_AND_ONE_END
}

DIRECT (NEW1_Pitch_PitchTier_to_Pitch) {
	CONVERT_ONE_AND_ONE_TO_ONE (Pitch, PitchTier)
		autoPitch result = Pitch_PitchTier_to_Pitch (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_stylized");
}

// MARK: - PITCH & POINTPROCESS

DIRECT (NEW1_Pitch_PointProcess_to_PitchTier) {
	CONVERT_ONE_AND_ONE_TO_ONE (Pitch, PointProcess)
		autoPitchTier result = Pitch_PointProcess_to_PitchTier (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get());
}

// MARK: - PITCH & SOUND

DIRECT (NEW1_Sound_Pitch_to_Manipulation) {
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoManipulation result = Sound_Pitch_to_Manipulation (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (your name.get());
}

DIRECT (NEW1_Sound_Pitch_to_PointProcess_cc) {
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoPointProcess result = Sound_Pitch_to_PointProcess_cc (me, you);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get());
}

FORM (NEW1_Sound_Pitch_to_PointProcess_peaks, U"Sound & Pitch: To PointProcess (peaks)", 0) {
	BOOLEAN (includeMaxima, U"Include maxima", true)
	BOOLEAN (includeMinima, U"Include minima", false)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Sound, Pitch)
		autoPointProcess result = Sound_Pitch_to_PointProcess_peaks (me, you, includeMaxima, includeMinima);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

// MARK: - POLYGON

FORM (NEW1_Polygon_createFromValues, U"Create Polygon from values", U"Create Polygon from values...") {
	WORD (name, U"Name", U"triangle")
	REALVECTOR (xValues, U"X values", WHITESPACE_SEPARATED_, U"0 50 20")
	REALVECTOR (yValues, U"Y values", WHITESPACE_SEPARATED_, U"0 10 80")
	OK
DO
	CREATE_ONE
		Melder_require (xValues.size == yValues.size,
			U"The number of x values should equal the number of y values.");
		autoPolygon result = Polygon_create (xValues.size);
		result -> x.all()  <<=  xValues;
		result -> y.all()  <<=  yValues;
	CREATE_ONE_END (name);
}

FORM (GRAPHICS_Polygon_draw, U"Polygon: Draw", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_draw (me, GRAPHICS, xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_drawCircles, U"Polygon: Draw circles", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	POSITIVE (diameter, U"Diameter (mm)", U"3.0")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_drawCircles (me, GRAPHICS, xmin, xmax, ymin, ymax, diameter);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_drawClosed, U"Polygon: Draw", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_drawClosed (me, GRAPHICS, xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygons_drawConnection, U"Polygons: Draw connection", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	BOOLEAN (arrow, U"Arrow", false)
	POSITIVE (relativeLength, U"Relative length", U"0.9")
	OK
DO
	GRAPHICS_TWO (Polygon)
		Polygons_drawConnection (me, you, GRAPHICS, xmin, xmax, ymin, ymax, arrow, relativeLength);
	GRAPHICS_TWO_END
}

DIRECT (HELP_Polygon_help) {
	HELP (U"Polygon")
}

FORM (GRAPHICS_Polygon_paint, U"Polygon: Paint", nullptr) {
	COLOUR (colour, U"Colour (0-1, name, or {r,g,b})", U"0.5")
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_paint (me, GRAPHICS, colour, xmin, xmax, ymin, ymax);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Polygon_paintCircles, U"Polygon: Paint circles", nullptr) {
	REAL (xmin, U"Xmin", U"0.0")
	REAL (xmax, U"Xmax", U"0.0 (= all)")
	REAL (ymin, U"Ymin", U"0.0")
	REAL (ymax, U"Ymax", U"0.0 (= all)")
	POSITIVE (diameter, U"Diameter (mm)", U"3.0")
	OK
DO
	GRAPHICS_EACH (Polygon)
		Polygon_paintCircles (me, GRAPHICS, xmin, xmax, ymin, ymax, diameter);
	GRAPHICS_EACH_END
}

DIRECT (MODIFY_Polygon_randomize) {
	MODIFY_EACH (Polygon)
		Polygon_randomize (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Polygon_salesperson, U"Polygon: Find shortest path", nullptr) {
	NATURAL (numberOfIterations, U"Number of iterations", U"1")
	OK
DO
	MODIFY_EACH (Polygon)
		Polygon_salesperson (me, numberOfIterations);
	MODIFY_EACH_END
}

DIRECT (NEW_Polygon_to_Matrix) {
	CONVERT_EACH_TO_ONE (Polygon)
		autoMatrix result = Polygon_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - SOUND & PITCH & POINTPROCESS

FORM (INFO_Sound_Pitch_PointProcess_voiceReport, U"Voice report", U"Voice") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	POSITIVE (fromPitch, U"left Pitch range (Hz)", U"75.0")
	POSITIVE (toPitch, U"right Pitch range (Hz)", U"600.0")
	POSITIVE (maximumPeriodFactor, U"Maximum period factor", U"1.3")
	POSITIVE (maximumAmplitudeFactor, U"Maximum amplitude factor", U"1.6")
	REAL (silenceThreshold, U"Silence threshold", U"0.03")
	REAL (voicingThreshold, U"Voicing threshold", U"0.45")
	OK
DO
	INFO_ONE_AND_ONE_AND_ONE (Sound, Pitch, PointProcess)
		MelderInfo_open ();
		Sound_Pitch_PointProcess_voiceReport (me, you, him, fromTime, toTime, fromPitch, toPitch,
				maximumPeriodFactor, maximumAmplitudeFactor, silenceThreshold, voicingThreshold);
		MelderInfo_close ();
	INFO_ONE_AND_ONE_AND_ONE_END
}

// MARK: - SOUND & POINTPROCESS & PITCHTIER & DURATIONTIER

FORM (NEW1_Sound_Point_Pitch_Duration_to_Sound, U"To Sound", nullptr) {
	POSITIVE (longestPeriod, U"Longest period (s)", U"0.02")
	OK
DO
	CONVERT_ONE_AND_ONE_AND_ONE_AND_ONE_TO_ONE (Sound, PointProcess, PitchTier, DurationTier)
		autoSound result = Sound_Point_Pitch_Duration_to_Sound (me, you, him, she, longestPeriod);
	CONVERT_ONE_AND_ONE_AND_ONE_AND_ONE_TO_ONE_END (U"manip");
}

// MARK: - SPECTROGRAM

FORM (GRAPHICS_Spectrogram_paint, U"Spectrogram: Paint", U"Spectrogram: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (maximum, U"Maximum (dB/Hz)", U"100.0")
	BOOLEAN (autoscaling, U"Autoscaling", 1)
	POSITIVE (dynamicRange, U"Dynamic range (dB)", U"50.0")
	REAL (preEmphasis, U"Pre-emphasis (dB/oct)", U"6.0")
	REAL (dynamicCompression, U"Dynamic compression (0-1)", U"0.0")
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (Spectrogram)
		Spectrogram_paint (me, GRAPHICS, fromTime, toTime, fromFrequency, toFrequency,
			maximum, autoscaling, dynamicRange, preEmphasis, dynamicCompression,
			kSpectrogram_colourMap::GREY, false,
			garnish
		);
	GRAPHICS_EACH_END
}

FORM (MODIFY_Spectrogram_formula, U"Spectrogram: Formula", U"Spectrogram: Formula...") {
	COMMENT (U"Do for all times and frequencies:")
	COMMENT (U"   `x` is the time in seconds")
	COMMENT (U"   `y` is the frequency in hertz")
	COMMENT (U"   `self` is the current value in Pa\u00B2/Hz")
	FORMULA (formula, U"   Replace all values with", U"self * exp (- x / 0.1)")
	OK
DO
	MODIFY_EACH_WEAK (Spectrogram)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (REAL_Spectrogram_getPowerAt, U"Spectrogram: Get power at (time, frequency)", nullptr) {
	REAL (time, U"Time (s)", U"0.5")
	REAL (frequency, U"Frequency (Hz)", U"1000")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrogram)
		const double result = Matrix_getValueAtXY (me, time, frequency);
	QUERY_ONE_FOR_REAL_END (U" Pa2/Hz (at time = ", time, U" seconds and frequency = ", frequency, U" Hz)")
}

DIRECT (HELP_Spectrogram_help) {
	HELP (U"Spectrogram")
}

DIRECT (MOVIE_Spectrogram_playMovie) {
	MOVIE_ONE (Spectrogram, U"Spectrogram movie", 300, 300)
		Matrix_playMovie (me, graphics);
	MOVIE_ONE_END
}

DIRECT (NEW_Spectrogram_to_Matrix) {
	CONVERT_EACH_TO_ONE (Spectrogram)
		autoMatrix result = Spectrogram_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrogram_to_Sound, U"Spectrogram: To Sound", nullptr) {
	REAL (samplingFrequency, U"Sampling frequency (Hz)", U"44100.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrogram)
		autoSound result = Spectrogram_to_Sound (me, samplingFrequency);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrogram_to_Spectrum, U"Spectrogram: To Spectrum (slice)", nullptr) {
	REAL (time, U"Time (seconds)", U"0.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrogram)
		autoSpectrum result = Spectrogram_to_Spectrum (me, time);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (EDITOR_ONE_Spectrogram_view) {
	EDITOR_ONE (a,Spectrogram)
		autoSpectrogramEditor editor = SpectrogramEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: - SPECTRUM

// MARK: - Help

DIRECT (HELP_Spectrum_help) {
	HELP (U"Spectrum")
}

// MARK: View & Edit

DIRECT (EDITOR_ONE_Spectrum_viewAndEdit) {
	EDITOR_ONE (a,Spectrum)
		autoSpectrumEditor editor = SpectrumEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_Spectrum_draw, U"Spectrum: Draw", nullptr) {
	REAL (fromFrequency, U"left Frequency range (Hz)", U"0.0")
	REAL (toFrequency, U"right Frequency range (Hz)", U"0.0 (= all)")
	REAL (minimumPower, U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL (maximumPower, U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_draw (me, GRAPHICS, fromFrequency, toFrequency, minimumPower, maximumPower, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_Spectrum_drawLogFreq, U"Spectrum: Draw (log freq)", nullptr) {
	POSITIVE (fromFrequency, U"left Frequency range (Hz)", U"10.0")
	POSITIVE (toFrequency, U"right Frequency range (Hz)", U"10000.0")
	REAL (minimumPower, U"Minimum power (dB/Hz)", U"0 (= auto)")
	REAL (maximumPower, U"Maximum power (dB/Hz)", U"0 (= auto)")
	BOOLEAN (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (Spectrum)
		Spectrum_drawLogFreq (me, GRAPHICS, fromFrequency, toFrequency, minimumPower, maximumPower, garnish);
	GRAPHICS_EACH_END
}

// MARK: Tabulate

FORM (NEW_Spectrum_tabulate, U"Spectrum: Tabulate", 0) {
	BOOLEAN (includeBinNumber, U"Include bin number", false)
	BOOLEAN (includeFrequency, U"Include frequency", true)
	BOOLEAN (includeRealPart, U"Include real part", false)
	BOOLEAN (includeImaginaryPart, U"Include imaginary part", false)
	BOOLEAN (includeEnergyDensity, U"Include energy density", false)
	BOOLEAN (includePowerDensity, U"Include power density", true)
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoTable result = Spectrum_tabulate (me, includeBinNumber, includeFrequency, includeRealPart, includeImaginaryPart,
				includeEnergyDensity, includePowerDensity);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_tabulate_verbose) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoTable result = Spectrum_tabulate_verbose (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Query

FORM (REAL_Spectrum_getBandDensity, U"Spectrum: Get band density", nullptr) {
	REAL (bandFloor, U"Band floor (Hz)", U"200.0")
	REAL (bandCeiling, U"Band ceiling (Hz)", U"1000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getBandDensity (me, bandFloor, bandCeiling);
	QUERY_ONE_FOR_REAL_END (U" Pa2 / Hz2")
}

FORM (REAL_Spectrum_getBandDensityDifference, U"Spectrum: Get band density difference", nullptr) {
	REAL (lowBandFloor, U"Low band floor (Hz)", U"0.0")
	REAL (lowBandCeiling, U"Low band ceiling (Hz)", U"500.0")
	REAL (highBandFloor, U"High band floor (Hz)", U"500.0")
	REAL (highBandCeiling, U"High band ceiling (Hz)", U"4000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getBandDensityDifference (me,
				lowBandFloor, lowBandCeiling, highBandFloor, highBandCeiling);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Spectrum_getBandEnergy, U"Spectrum: Get band energy", nullptr) {
	REAL (bandFloor, U"Band floor (Hz)", U"200.0")
	REAL (bandCeiling, U"Band ceiling (Hz)", U"1000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getBandEnergy (me, bandFloor, bandCeiling);
	QUERY_ONE_FOR_REAL_END (U" Pa2 sec")
}

FORM (REAL_Spectrum_getBandEnergyDifference, U"Spectrum: Get band energy difference", nullptr) {
	REAL (lowBandFloor, U"Low band floor (Hz)", U"0.0")
	REAL (lowBandCeiling, U"Low band ceiling (Hz)", U"500.0")
	REAL (highBandFloor, U"High band floor (Hz)", U"500.0")
	REAL (highBandCeiling, U"High band ceiling (Hz)", U"4000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getBandEnergyDifference (me,
				lowBandFloor, lowBandCeiling, highBandFloor, highBandCeiling);
	QUERY_ONE_FOR_REAL_END (U" dB")
}

FORM (REAL_Spectrum_getBinNumberFromFrequency, U"Spectrum: Get bin number from frequency", nullptr) {
	REAL (frequency, U"Frequency (Hz)", U"2000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Sampled_xToIndex (me, frequency);
	QUERY_ONE_FOR_REAL_END (U" (bin number as a real value)")
}

DIRECT (REAL_Spectrum_getBinWidth) {
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = my dx;
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

FORM (REAL_Spectrum_getCentralMoment, U"Spectrum: Get central moment", U"Spectrum: Get central moment...") {
	POSITIVE (moment, U"Moment", U"3.0")
	POSITIVE (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getCentralMoment (me, moment, power);
	QUERY_ONE_FOR_REAL_END (U" hertz to the power ", moment)
}

FORM (REAL_Spectrum_getCentreOfGravity, U"Spectrum: Get centre of gravity", U"Spectrum: Get centre of gravity...") {
	POSITIVE (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getCentreOfGravity (me, power);
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

FORM (REAL_Spectrum_getFrequencyFromBin, U"Spectrum: Get frequency from bin", nullptr) {
	NATURAL (bandNumber, U"Band number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Sampled_indexToX (me, bandNumber);
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

DIRECT (REAL_Spectrum_getLowestFrequency) {
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = my xmin;
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

DIRECT (REAL_Spectrum_getHighestFrequency) {
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = my xmax;
	QUERY_ONE_FOR_REAL_END (U" hertz");
}

FORM (REAL_Spectrum_getRealValueInBin, U"Spectrum: Get real value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		if (binNumber > my nx)
			Melder_throw (U"Bin number should not exceed number of bins.");
		const double result = my z [1] [binNumber];
	QUERY_ONE_FOR_REAL_END (U" (real value in bin ", binNumber, U")")
}

FORM (REAL_Spectrum_getImaginaryValueInBin, U"Spectrum: Get imaginary value in bin", nullptr) {
	NATURAL (binNumber, U"Bin number", U"100")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		if (binNumber > my nx)
			Melder_throw (U"The bin number should not exceed the number of bins.");
		const double result = my z [2] [binNumber];
	QUERY_ONE_FOR_REAL_END (U" (imaginary value in bin ", binNumber, U")")
}

FORM (REAL_Spectrum_getKurtosis, U"Spectrum: Get kurtosis", U"Spectrum: Get kurtosis...") {
	POSITIVE (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getKurtosis (me, power);
	QUERY_ONE_FOR_REAL_END (U" (kurtosis)")
}

FORM (REAL_Spectrum_getSoundPressureLevelOfNearestMaximum, U"Spectrum: Get sound pressure level of nearest maximum", U"Spectrum: Get sound pressure level of nearest maximum...") {
	POSITIVE (frequency, U"Frequency (Hz)", U"1000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const MelderPoint maximum = Spectrum_getNearestMaximum (me, frequency);
		const double result = maximum. y;
	QUERY_ONE_FOR_REAL_END (U" \"dB/Hz\"")
}

FORM (REAL_Spectrum_getFrequencyOfNearestMaximum, U"Spectrum: Get frequency of nearest maximum", U"Spectrum: Get frequency of nearest maximum...") {
	POSITIVE (frequency, U"Frequency (Hz)", U"1000.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const MelderPoint maximum = Spectrum_getNearestMaximum (me, frequency);
		const double result = maximum. x;
	QUERY_ONE_FOR_REAL_END (U" Hz")
}

DIRECT (INTEGER_Spectrum_getNumberOfBins) {
	QUERY_ONE_FOR_REAL (Spectrum)
		const integer result = my nx;
	QUERY_ONE_FOR_REAL_END (U" bins")
}

FORM (REAL_Spectrum_getSkewness, U"Spectrum: Get skewness", U"Spectrum: Get skewness...") {
	POSITIVE (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getSkewness (me, power);
	QUERY_ONE_FOR_REAL_END (U" (skewness)")
}

FORM (REAL_Spectrum_getStandardDeviation, U"Spectrum: Get standard deviation", U"Spectrum: Get standard deviation...") {
	POSITIVE (power, U"Power", U"2.0")
	OK
DO
	QUERY_ONE_FOR_REAL (Spectrum)
		const double result = Spectrum_getStandardDeviation (me, power);
	QUERY_ONE_FOR_REAL_END (U" hertz")
}

// MARK: Modify

FORM (MODIFY_Spectrum_formula, U"Spectrum: Formula", U"Spectrum: Formula...") {
	COMMENT (U"`x` is the frequency in hertz, `col` is the bin number;   "
		"`y' = `row' is 1 (real part) or 2 (imaginary part)")
	COMMENT (U"y := 1;   row := 1;   "
		"x := 0;   for col := 1 to ncol do { self [1, col] := `formula` ; x := x + dx }")
	COMMENT (U"y := 2;   row := 2;   "
		"x := 0;   for col := 1 to ncol do { self [2, col] := `formula` ; x := x + dx }")
	FORMULA (formula, U"Formula", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Spectrum)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Spectrum_passHannBand, U"Spectrum: Filter (pass Hann band)", U"Spectrum: Filter (pass Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Spectrum_passHannBand (me, fromFrequency, toFrequency, smoothing);
	MODIFY_EACH_END
}

FORM (MODIFY_Spectrum_stopHannBand, U"Spectrum: Filter (stop Hann band)", U"Spectrum: Filter (stop Hann band)...") {
	REAL (fromFrequency, U"From frequency (Hz)", U"500.0")
	REAL (toFrequency, U"To frequency (Hz)", U"1000.0")
	POSITIVE (smoothing, U"Smoothing (Hz)", U"100.0")
	OK
DO
	MODIFY_EACH (Spectrum)
		Spectrum_stopHannBand (me, fromFrequency, toFrequency, smoothing);
	MODIFY_EACH_END
}

// MARK: Convert

FORM (NEW_Spectrum_cepstralSmoothing, U"Spectrum: Cepstral smoothing", nullptr) {
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"500.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrum result = Spectrum_cepstralSmoothing (me, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrum_lpcSmoothing, U"Spectrum: LPC smoothing", 0) {
	NATURAL (numberOfPeaks, U"Number of peaks", U"5")
	POSITIVE (preEmphasisFrom, U"Pre-emphasis from (Hz)", U"50.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrum result = Spectrum_lpcSmoothing (me, numberOfPeaks, preEmphasisFrom);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrum_to_Excitation, U"Spectrum: To Excitation", nullptr) {
	POSITIVE (frequencyResolution, U"Frequency resolution (Bark)", U"0.1")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoExcitation result = Spectrum_to_Excitation (me, frequencyResolution);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrum_to_Formant_peaks, U"Spectrum: To Formant (peaks)", nullptr) {
	COMMENT (U"Warning: this simply picks peaks from 0 Hz up!")
	NATURAL (maximumNumberOfFormants, U"Maximum number of formants", U"1000")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoFormant result = Spectrum_to_Formant (me, maximumNumberOfFormants);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (NEW_Spectrum_to_Ltas, U"Spectrum: To Long-term average spectrum", nullptr) {
	POSITIVE (bandwidth, U"Bandwidth (Hz)", U"1000.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Spectrum)
		autoLtas result = Spectrum_to_Ltas (me, bandwidth);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_to_Ltas_1to1) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoLtas result = Spectrum_to_Ltas_1to1 (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_to_Matrix) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoMatrix result = Spectrum_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_to_Sound) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSound result = Spectrum_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_to_Spectrogram) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrogram result = Spectrum_to_Spectrogram (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Spectrum_to_SpectrumTier_peaks) {
	CONVERT_EACH_TO_ONE (Spectrum)
		autoSpectrumTier result = Spectrum_to_SpectrumTier_peaks (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - STRINGS

// MARK: New

FORM (NEW1_Strings_createFromTexts, U"Create Strings from texts", nullptr) {
	WORD (name, U"Name", U"texts")
	STRINGARRAY_LINES (10, texts, U"Texts", { U"one line", U"another line", U"yet another line" })
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createFromTexts (texts);
	CREATE_ONE_END (name);
}

FORM (NEW1_Strings_createAsFileList, U"Create Strings as file list", U"Create Strings as file list...") {
	SENTENCE (name, U"Name", U"fileList")
	static structMelderFolder homeFolder { };
	Melder_getHomeDir (& homeFolder);
	static conststring32 homeFolderPath = MelderFolder_peekPath (& homeFolder);
	static char32 defaultGlob [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath, U"/*.wav");
	#elif defined (_WIN32)
	{
		static integer len = Melder_length (homeFolderPath);
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath,
				len == 0 || homeFolderPath [len - 1] != U'\\' ? U"\\" : U"", U"*.wav");
	}
	#else
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath, U"/*.wav");
	#endif
	FOLDER (path, U"File path", defaultGlob)
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsFileList (path);
	CREATE_ONE_END (name)
}

FORM (NEW1_Strings_createAsFolderList, U"Create Strings as folder list", U"Create Strings as folder list...") {
	SENTENCE (name, U"Name", U"folderList")
	static structMelderFolder homeFolder { };
	Melder_getHomeDir (& homeFolder);
	static conststring32 homeFolderPath = MelderFolder_peekPath (& homeFolder);
	static char32 defaultGlob [kMelder_MAXPATH+1];
	#if defined (UNIX)
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath, U"/*");
	#elif defined (_WIN32)
	{
		const integer len = Melder_length (homeFolderPath);
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath,
				len == 0 || homeFolderPath [len - 1] != U'\\' ? U"\\" : U"");
	}
	#else
		Melder_sprint (defaultGlob,kMelder_MAXPATH+1, homeFolderPath, U"/*");
	#endif
	FOLDER (path, U"Path", defaultGlob)
	OK
DO
	CREATE_ONE
		autoStrings result = Strings_createAsFolderList (path);
	CREATE_ONE_END (name)
}

// MARK: Open

FORM_READ (READ_ONE__Strings_readFromRawTextFile, U"Read Strings from raw text file", nullptr, true) {
	READ_ONE
		autoStrings result = Strings_readFromRawTextFile (file);
	READ_ONE_END
}

// MARK: Save

FORM_SAVE (SAVE_Strings_writeToRawTextFile, U"Save Strings as raw text file", nullptr, U"txt") {
	SAVE_ONE (Strings, U"save the selected Strings object to the raw text file")
		Strings_writeToRawTextFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_Strings_help) {
	HELP (U"Strings")
}

// MARK: View & Edit

DIRECT (EDITOR_ONE_Strings_viewAndEdit) {
	EDITOR_ONE (a,Strings)
		autoStringsEditor editor = StringsEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: Query

DIRECT (BOOLEAN_Strings_equal) {
	QUERY_TWO_FOR_REAL (Strings)
		const integer result = (integer) Data_equal (me, you);   // cast bool to 0 or 1
	QUERY_TWO_FOR_REAL_END (result ? U" (equal)" : U" (unequal)")
}

DIRECT (INTEGER_Strings_getNumberOfStrings) {
	QUERY_ONE_FOR_REAL (Strings)
		const integer result = my numberOfStrings;
	QUERY_ONE_FOR_REAL_END (U" strings")
}

FORM (STRING_Strings_getString, U"Get string", nullptr) {
	NATURAL (position, U"Position", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (Strings)
		const conststring32 result = position > my numberOfStrings ? U"" : my strings [position].get();   // TODO
	QUERY_ONE_FOR_STRING_END
}

DIRECT (STRVEC_Strings_listAllStrings) {
	QUERY_ONE_FOR_STRING_ARRAY (Strings)
		autoSTRVEC result = copy_STRVEC (my strings.get());
	QUERY_ONE_FOR_STRING_ARRAY_END
}

// MARK: Modify

FORM (MODIFY_Strings_insertString, U"Strings: Insert string", nullptr) {
	INTEGER (atPosition, U"At position", U"0 (= at end)")
	TEXTFIELD (string, U"String", U"", 10)
	OK
DO
	MODIFY_EACH (Strings)
		Strings_insert (me, atPosition, string);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Strings_nativize) {
	MODIFY_EACH_WEAK (Strings)
		Strings_nativize (me);
	MODIFY_EACH_WEAK_END
}

DIRECT (MODIFY_Strings_genericize) {
	MODIFY_EACH_WEAK (Strings)
		Strings_genericize (me);
	MODIFY_EACH_WEAK_END
}

DIRECT (MODIFY_Strings_randomize) {
	MODIFY_EACH (Strings)
		Strings_randomize (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Strings_removeString, U"Strings: Remove string", nullptr) {
	NATURAL (position, U"Position", U"1")
	OK
DO
	MODIFY_EACH (Strings)
		Strings_remove (me, position);
	MODIFY_EACH_END
}

FORM (MODIFY_Strings_setString, U"Strings: Set string", nullptr) {
	NATURAL (position, U"Position", U"1")
	TEXTFIELD (newString, U"New string", U"", 10)
	OK
DO
	MODIFY_EACH (Strings)
		Strings_replace (me, position, newString);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Strings_sort) {
	MODIFY_EACH (Strings)
		Strings_sort (me);
	MODIFY_EACH_END
}

// MARK: Convert

FORM (NEW_Strings_replaceAll, U"Strings: Replace all", nullptr) {
	SENTENCE (find, U"Find", U"a")
	SENTENCE (replaceWith, U"Replace with", U"b")
	INTEGER (replaceLimitPerString, U"Replace limit per string", U"0 (= unlimited)")
	CHOICEx (findAndReplaceStringsAre, U"Find and replace strings are", 1, 0)
		OPTION (U"literals")
		OPTION (U"regular expressions")
	OK
DO
	CONVERT_EACH_TO_ONE (Strings)
		integer numberOfMatches, numberOfStringMatches;
		autoStrings result = Strings_change (me, find, replaceWith,
				replaceLimitPerString, & numberOfMatches, & numberOfStringMatches, findAndReplaceStringsAre);   // FIXME: boolean inappropriate
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_replaced")
}

DIRECT (NEW_Strings_to_Distributions) {
	CONVERT_EACH_TO_ONE (Strings)
		autoDistributions result = Strings_to_Distributions (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Strings_to_WordList) {
	CONVERT_EACH_TO_ONE (Strings)
		autoWordList result = Strings_to_WordList (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - TEXTGRID; the remainder is in praat_TextGrid_init.cpp

FORM (NEW1_TextGrid_create, U"Create TextGrid", U"Create TextGrid...") {
	COMMENT (U"Hint: to label or segment an existing Sound,")
	COMMENT (U"select that Sound and choose \"To TextGrid...\".")
	REAL (startTime, U"Start time (s)", U"0.0")
	REAL (endTime, U"End time (s)", U"1.0")
	SENTENCE (allTierNames, U"All tier names", U"Mary John bell")
	SENTENCE (whichOfTheseArePointTiers, U"Which of these are point tiers?", U"bell")
	OK
DO
	if (endTime <= startTime)
		Melder_throw (U"The end time should be greater than the start time");
	CREATE_ONE
		autoTextGrid result = TextGrid_create (startTime, endTime, allTierNames, whichOfTheseArePointTiers);
	CREATE_ONE_END (allTierNames)
}

FORM (READ_ONE__TextGrid_readFromEspsLabelFile, U"Read TextGrid from ESPS label file", U"Read TextGrid from ESPS label file...") {
	INFILE (soundFilePath, U"Sound file path", U"")
	BOOLEAN (tiersArePointTiers, U"Tiers are point tiers", false)
	INTEGER (overrideNumberOfTiers, U"Override number of tiers", U"0 (= don't override)")
	OK
DO
	CREATE_ONE
		structMelderFile file { };
		Melder_relativePathToFile (soundFilePath, & file);
		autoTextGrid result = TextGrid_readFromEspsLabelFile (& file, tiersArePointTiers, overrideNumberOfTiers);
	CREATE_ONE_END (U"")
}

FORM (NEW_Sound_readWithAdjacentAnnotationFiles_buckeye, U"Read with adjacent annotations (Buckeye)", U"Read with adjacent annotation files (Buckeye)...") {
	INFILE (soundFileName, U"Sound file name", U"/Volumes/Buckeye/s01/s0101a/s0101a.wav")
	OK
DO
	CREATE_MULTIPLE
		autoTextGrid textgrid;
		autoSound sound = Sound_readWithAdjacentAnnotationFiles_buckeye (soundFileName, & textgrid);
		praat_new (sound.move());
		praat_new (textgrid.move());
	CREATE_MULTIPLE_END
}

FORM (NEW_Sound_readWithAdjacentAnnotationFiles_timit, U"Read with adjacent annotations (TIMIT)", U"Read with adjacent annotation files (TIMIT)...") {
	INFILE (soundFileName, U"Sound file name", U"/Volumes/TIMIT/train/dr1/fcjf0/sa1.wav")
	OK
DO
	CREATE_MULTIPLE
		autoTextGrid textgrid;
		autoSound sound = Sound_readWithAdjacentAnnotationFiles_timit (soundFileName, & textgrid);
		praat_new (sound.move());
		praat_new (textgrid.move());
	CREATE_MULTIPLE_END
}

FORM (NEW_Sound_readWithAdjacentAnnotationFiles_cgn, U"Read with adjacent annotations (CGN)", U"Read with adjacent annotation files (CGN)...") {
	INFILE (soundFileName, U"Sound file name", U"/Volumes/CGN/comp-a/vl/fv400061.wav")
	OK
DO
	CREATE_MULTIPLE
		autoTextGrid textgrid;
		autoSound sound = Sound_readWithAdjacentAnnotationFiles_cgn (soundFileName, & textgrid);
		praat_new (sound.move());
		praat_new (textgrid.move());
	CREATE_MULTIPLE_END
}

FORM (NEW_Corpus_readFromCGN, U"Read Corpus from CGN", U"Read Corpus from CGN...") {
	FOLDER (folderName, U"CGN folder name", U"/Volumes/CGN")
	OK
DO
	CREATE_ONE
		autoCorpus result = Corpus_readFromCGN (folderName);
	CREATE_ONE_END (U"CGN")
}

// MARK: - TRANSITION

DIRECT (NEW_Transition_conflate) {
	CONVERT_EACH_TO_ONE (Transition)
		autoDistributions result = Transition_to_Distributions_conflate (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_Transition_drawAsNumbers, U"Draw as numbers", nullptr) {
	CHOICE (format, U"Format", 1)
		OPTION (U"decimal")
		OPTION (U"exponential")
		OPTION (U"free")
		OPTION (U"rational")
	NATURAL (precision, U"Precision", U"2")
	OK
DO
	GRAPHICS_EACH (Transition)
		Transition_drawAsNumbers (me, GRAPHICS, format, precision);
	GRAPHICS_EACH_END
}

DIRECT (NEWTIMES2_Transition_eigen) {
	CONVERT_EACH_TO_MULTIPLE (Transition)
		autoMatrix vectors, values;
		Transition_eigen (me, & vectors, & values);
		praat_new (vectors.move(), U"eigenvectors");
		praat_new (values.move(), U"eigenvalues");
	CONVERT_EACH_TO_MULTIPLE_END
}

DIRECT (HELP_Transition_help) {
	HELP (U"Transition")
}

FORM (NEW_Transition_power, U"Transition: Power...", nullptr) {
	NATURAL (power, U"Power", U"2")
	OK
DO
	CONVERT_EACH_TO_ONE (Transition)
		autoTransition result = Transition_power (me, power);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (NEW_Transition_to_Matrix) {
	CONVERT_EACH_TO_ONE (Transition)
		autoMatrix result = Transition_to_Matrix (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: - Praat menu

FORM (INFO_Praat_test, U"Praat test", 0) {
	OPTIONMENU_ENUM (kPraatTests, test, U"Test", kPraatTests::DEFAULT)
	SENTENCE (arg1, U"arg1", U"1000000")
	SENTENCE (arg2, U"arg2", U"")
	SENTENCE (arg3, U"arg3", U"")
	SENTENCE (arg4, U"arg4", U"")
	OK
DO
	INFO_NONE
		Praat_tests ((kPraatTests) test, arg1, arg2, arg3, arg4);
	INFO_NONE_END
}

// MARK: - Help menu

DIRECT (HELP_PraatIntro) { HELP (U"Intro") }
DIRECT (HELP_ObjectWindow) { HELP (U"Objects window") }
DIRECT (HELP_FrequentlyAskedQuestions) { HELP (U"FAQ (Frequently Asked Questions)") }
DIRECT (HELP_TypesOfObjects) { HELP (U"Types of objects") }
DIRECT (HELP_Editors) { HELP (U"Editors") }
DIRECT (HELP_WhatsNew) { HELP (U"What's new?") }
DIRECT (HELP_Acknowledgments) { HELP (U"Acknowledgments") }
DIRECT (HELP_License) { HELP (U"License") }
DIRECT (HELP_PrivacyAndSecurity) { HELP (U"Privacy and security") }
DIRECT (HELP_CheckingForUpdates) { HELP (U"Checking for updates") }
DIRECT (HELP_ReportingAProblem) { HELP (U"Reporting a problem") }
DIRECT (HELP_FormulasTutorial) { HELP (U"Formulas") }
DIRECT (HELP_ScriptingTutorial) { HELP (U"Scripting") }
DIRECT (HELP_Functions) { HELP (U"Functions") }
DIRECT (HELP_DemoWindow) { HELP (U"Demo window") }
DIRECT (HELP_Interoperability) { HELP (U"Interoperability") }
DIRECT (HELP_Programming) { HELP (U"Programming with Praat") }
DIRECT (HELP_SearchManual_Fon) { Melder_search (); END_NO_NEW_DATA }

// MARK: - file recognizers

static autoDaata cgnSyntaxFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 57)
		return autoDaata ();
	if (! strnequ (& header [0], "<?xml version=\"1.0\"?>", 21) ||
	    (! strnequ (& header [22], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35) &&
	     ! strnequ (& header [23], "<!DOCTYPE ttext SYSTEM \"ttext.dtd\">", 35))
	)
		return autoDaata ();
	return TextGrid_readFromCgnSyntaxFile (file);
}

static autoDaata chronologicalTextGridTextFileRecognizer (integer nread, const char *header, MelderFile file) {
	if (nread < 100)
		return autoDaata ();
	if (strnequ (& header [0], "\"Praat chronological TextGrid text file\"", 40))
		return TextGrid_readFromChronologicalTextFile (file);
	char headerCopy [101];
	memcpy (headerCopy, header, 100);
	headerCopy [100] = '\0';
	for (int i = 0; i < 100; i ++)
		if (headerCopy [i] == '\0')
			headerCopy [i] = '\001';
	//if (strstr (headerCopy, "\"\001P\001r\001a\001a\001t\001 \001c\001h\001r\001o\001n\001o\001l\001o\001g\001i\001c\001a\001l\001"
	//	" \001T\001e\001x\001t\001G\001r\001i\001d\001 t\001"))
	if (strstr (headerCopy, "\"\001P\001r\001a\001a\001t\001 \001c\001h\001r\001o\001n\001o\001l\001o\001g\001i\001c\001a\001l\001"
		" \001T\001e\001x\001t\001G\001r\001i\001d\001 \001t\001e\001x\001t\001 \001f\001i\001l\001e\001\""))
	{
		return TextGrid_readFromChronologicalTextFile (file);
	}
	return autoDaata ();
}

// MARK: - extension to praat_stat_init

static bool isTabSeparated_8bit (integer nread, const char *header) {
	for (integer i = 0; i < nread; i ++) {
		if (header [i] == '\t')
			return true;
		if (header [i] == '\n' || header [i] == '\r')
			return false;
	}
	return false;
}

static bool isTabSeparated_utf16be (integer nread, const char *header) {
	for (integer i = 2; i < nread; i += 2) {
		if (header [i] == '\0' && header [i + 1] == '\t')
			return true;
		if (header [i] == '\0' && (header [i + 1] == '\n' || header [i + 1] == '\r'))
			return false;
	}
	return false;
}

static bool isTabSeparated_utf16le (integer nread, const char *header) {
	for (integer i = 2; i < nread; i += 2) {
		if (header [i + 1] == '\0' && header [i] == '\t')
			return true;
		if (header [i + 1] == '\0' && (header [i] == '\n' || header [i] == '\r'))
			return false;
	}
	return false;
}

static autoDaata tabSeparatedFileRecognizer (integer nread, const char *header, MelderFile file) {
	/*
		A table is recognized if it has at least one tab symbol,
		which must be before the first newline symbol (if any).
	*/
	unsigned char *uheader = (unsigned char *) header;
	const bool isTabSeparated =
		uheader [0] == 0xef && uheader [1] == 0xff ? isTabSeparated_utf16be (nread, header) :
		uheader [0] == 0xff && uheader [1] == 0xef ? isTabSeparated_utf16le (nread, header) :
		isTabSeparated_8bit (nread, header)
	;
	if (! isTabSeparated)
		return autoDaata ();
	return Table_readFromCharacterSeparatedTextFile (file, U'\t', false);
}


// MARK: - buttons

void praat_uvafon_init () {
	Thing_recognizeClassesByName (classPolygon, classParamCurve,
		classSpectrum, classLtas, classSpectrogram, classFormant,
		classExcitation, classCochleagram, classVocalTract,
		classLabel, classTier, classAutosegment,   // three obsolete classes (pre-1997)
		classIntensity, classPitch, classHarmonicity,
		classTransition,
		classManipulation, classTextPoint, classTextInterval, classTextTier,
		classIntervalTier, classTextGrid, classWordList, classSpellingChecker,
		classCorpus
	);
	Thing_recognizeClassByOtherName (classManipulation, U"Psola");      // obsolete name (pre-1997)
	Thing_recognizeClassByOtherName (classManipulation, U"Analysis");   // obsolete name (pre-2001)
	Thing_recognizeClassByOtherName (classPitchTier, U"StylPitch");     // obsolete name (pre-1996)

	Data_recognizeFileType (cgnSyntaxFileRecognizer);
	Data_recognizeFileType (chronologicalTextGridTextFileRecognizer);
	Data_recognizeFileType (IDXFormattedMatrixFileRecognizer);
	
	structFunctionArea       :: f_preferences ();
	structSpectrumArea       :: f_preferences ();
	structPointArea          :: f_preferences ();
	structRealTierArea       :: f_preferences ();
	structPitchTierArea      :: f_preferences ();
	structIntensityTierArea  :: f_preferences ();
	structDurationTierArea   :: f_preferences ();
	structAmplitudeTierArea  :: f_preferences ();
	structFormantGridArea    :: f_preferences ();
	structManipulationPitchTierArea :: f_preferences ();

	structSpectrumEditor     :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"Technical", U"Praat test...", nullptr, 0, INFO_Praat_test);

	/*
		The user interfaces for the classes are included in the order
		in which they have to appear in the New menu:
	*/
	praat_Sound_init ();

	praat_addMenuCommand (U"Objects", U"New", U"-- new textgrid --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"New", U"Create TextGrid...",
			nullptr, 0, NEW1_TextGrid_create);
	praat_uvafon_TextGrid_init ();
	praat_Tiers_init ();
	praat_addMenuCommand (U"Objects", U"New", U"Create Corpus...",
			nullptr, 0, NEW1_Corpus_create);

	praat_addMenuCommand (U"Objects", U"New", U"-- new stats --",
			nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_stat_init)   // from stat
	praat_addMenuCommand (U"Objects", U"New", U"Stats", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_MDS_new_init)   // from dwtools

	praat_addMenuCommand (U"Objects", U"New", U"Generics", nullptr, 0, nullptr);
		praat_Matrix_init ();
		praat_addMenuCommand (U"Objects", U"New", U"Polygon", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create Polygon from values...",
				nullptr, 2, NEW1_Polygon_createFromValues);
		praat_addMenuCommand (U"Objects", U"New", U"-- new strings --",
				nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Strings", nullptr, 1, nullptr);
			praat_addMenuCommand (U"Objects", U"New", U"Create Strings from texts...",
					nullptr, 2, NEW1_Strings_createFromTexts);
			praat_addMenuCommand (U"Objects", U"New", U"Create Strings as file list...",
					nullptr, 2, NEW1_Strings_createAsFileList);
			praat_addMenuCommand (U"Objects", U"New", U"Create Strings as folder list... || Create Strings as directory list...",
					nullptr, 2, NEW1_Strings_createAsFolderList);
		praat_addMenuCommand (U"Objects", U"New", U"-- new David generics --",
				nullptr, 1, nullptr);
		INCLUDE_LIBRARY (praat_David_generics_new_init)

	praat_addMenuCommand (U"Objects", U"Open", U"Read Strings from raw text file...",
			nullptr, 0, READ_ONE__Strings_readFromRawTextFile);

	praat_addMenuCommand (U"Objects", U"Open", U"-- read tier --", nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Open", U"Read from special annotation file...", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read TextGrid from Xwaves... || Read TextGrid from ESPS label file...",
				nullptr, 1, READ_ONE__TextGrid_readFromEspsLabelFile);
	praat_addMenuCommand (U"Objects", U"Open", U"Read Sound with adjacent annotation files...", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound with adjacent annotation files (Buckeye)...",
				nullptr, 1, NEW_Sound_readWithAdjacentAnnotationFiles_buckeye);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound with adjacent annotation files (TIMIT)...",
				nullptr, 1, NEW_Sound_readWithAdjacentAnnotationFiles_timit);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Sound with adjacent annotation files (Corpus Gesproken Nederlands)...",
				nullptr, 1, NEW_Sound_readWithAdjacentAnnotationFiles_cgn);
	praat_addMenuCommand (U"Objects", U"Open", U"Open Corpus...", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"Open", U"Read Corpus from CGN...",
				nullptr, 1, NEW_Corpus_readFromCGN);

	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Praat Intro", nullptr, '?', HELP_PraatIntro);
	#ifndef macintosh
		praat_addMenuCommand (U"Objects", U"Help", U"Objects window", nullptr, 0, HELP_ObjectWindow);
	#endif
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Frequently asked questions",
			nullptr, 0, HELP_FrequentlyAskedQuestions);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- reference help --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Types of objects",
			nullptr, 0, HELP_TypesOfObjects);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Editors",
			nullptr, 0, HELP_Editors);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- version help --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"What’s new?",
			nullptr, 0, HELP_WhatsNew);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Acknowledgments",
			nullptr, 0, HELP_Acknowledgments);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"License",
			nullptr, 0, HELP_License);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- privacy help --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Privacy and security",
			nullptr, 0, HELP_PrivacyAndSecurity);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Checking for updates?",
			nullptr, 0, HELP_CheckingForUpdates);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Reporting a problem?",
			nullptr, 0, HELP_ReportingAProblem);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- shell help --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Formulas tutorial",
			nullptr, 0, HELP_FormulasTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Scripting tutorial",
			nullptr, 0, HELP_ScriptingTutorial);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Functions",
			nullptr, 0, HELP_Functions);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"-- control help --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Demo window",
			nullptr, 0, HELP_DemoWindow);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Interoperability",
			nullptr, 0, HELP_Interoperability);
	praat_addMenuCommand (U"Objects", U"ApplicationHelp", U"Programming",
			nullptr, 0, HELP_Programming);
#ifdef macintosh
	praat_addMenuCommand (U"Objects", U"Help", U"Praat Intro",
			nullptr, '?', HELP_PraatIntro);
	praat_addMenuCommand (U"Objects", U"Help", U"Objects window help",
			nullptr, 0, HELP_ObjectWindow);
	praat_addMenuCommand (U"Objects", U"Help", U"-- manual --",
			nullptr, 0, nullptr);
	praat_addMenuCommand (U"Objects", U"Help", U"Search Praat manual...",
			nullptr, 'M', HELP_SearchManual_Fon);
#endif

	praat_addAction1 (classCochleagram, 0, U"Cochleagram help",
			nullptr, 0, HELP_Cochleagram_help);
	praat_addAction1 (classCochleagram, 1, U"Play movie || Movie",
			nullptr, 0, MOVIE_Cochleagram_playMovie);
praat_addAction1 (classCochleagram, 0, U"Info", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 2, U"Difference...",
			nullptr, 0, REAL_Cochleagram_difference);
praat_addAction1 (classCochleagram, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Paint...",
			nullptr, 0, GRAPHICS_Cochleagram_paint);
praat_addAction1 (classCochleagram, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"Formula...",
			nullptr, 0, MODIFY_Cochleagram_formula);
praat_addAction1 (classCochleagram, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Excitation (slice)...",
			nullptr, 0, NEW_Cochleagram_to_Excitation);
praat_addAction1 (classCochleagram, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classCochleagram, 0, U"To Matrix",
			nullptr, 0, NEW_Cochleagram_to_Matrix);

	praat_addAction1 (classCorpus, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Corpus_edit);

praat_addAction1 (classDistributions, 0, U"Learn", nullptr, 0, nullptr);
	praat_addAction1 (classDistributions, 1, U"To Transition...",
			nullptr, 0, NEW_Distributions_to_Transition);
	praat_addAction1 (classDistributions, 2, U"To Transition (noise)...",
			nullptr, 0, NEW1_Distributions_to_Transition_noise);

	praat_addAction1 (classExcitation, 0, U"Excitation help",
			nullptr, 0, HELP_Excitation_help);
praat_addAction1 (classExcitation, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Draw...",
			nullptr, 0, GRAPHICS_Excitation_draw);
praat_addAction1 (classExcitation, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Formant...",
			nullptr, 0, NEW_Excitation_to_Formant);
praat_addAction1 (classExcitation, 1, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 1, U"Get loudness",
			nullptr, 0, REAL_Excitation_getLoudness);
praat_addAction1 (classExcitation, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"Formula...",
			nullptr, 0, MODIFY_Excitation_formula);
praat_addAction1 (classExcitation, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classExcitation, 0, U"To Matrix",
			nullptr, 0, NEW_Excitation_to_Matrix);

	praat_addAction1 (classFormant, 0, U"Formant help",
			nullptr, 0, HELP_Formant_help);
	praat_addAction1 (classFormant, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 0, U"Speckle...",
				nullptr, 1, GRAPHICS_Formant_drawSpeckles);
		praat_addAction1 (classFormant, 0, U"Draw tracks...",
				nullptr, 1, GRAPHICS_Formant_drawTracks);
		praat_addAction1 (classFormant, 0, U"Scatter plot...",
				nullptr, 1, GRAPHICS_Formant_scatterPlot);
	praat_addAction1 (classFormant, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classFormant, 1, U"List...",
				nullptr, 1, LIST_Formant_list);
		praat_addAction1 (classFormant, 0, U"Down to Table...",
				nullptr, 1, NEW_Formant_downto_Table);
	praat_addAction1 (classFormant, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classFormant);
		praat_addAction1 (classFormant, 1, U"Get number of formants...",
				nullptr, 1, INTEGER_Formant_getNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get minimum number of formants",
				nullptr, 1, INTEGER_Formant_getMinimumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"Get maximum number of formants",
				nullptr, 1, INTEGER_Formant_getMaximumNumberOfFormants);
		praat_addAction1 (classFormant, 1, U"-- get value --",
				nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get value at time...",
				nullptr, 1, REAL_Formant_getValueAtTime);
		praat_addAction1 (classFormant, 1, U"Get bandwidth at time...",
				nullptr, 1, REAL_Formant_getBandwidthAtTime);
		praat_addAction1 (classFormant, 1, U"-- get extreme --",
				nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get minimum...",
				nullptr, 1, REAL_Formant_getMinimum);
		praat_addAction1 (classFormant, 1, U"Get time of minimum...",
				nullptr, 1, REAL_Formant_getTimeOfMinimum);
		praat_addAction1 (classFormant, 1, U"Get maximum...",
				nullptr, 1, REAL_Formant_getMaximum);
		praat_addAction1 (classFormant, 1, U"Get time of maximum...",
				nullptr, 1, REAL_Formant_getTimeOfMaximum);
		praat_addAction1 (classFormant, 1, U"-- get distribution --",
				nullptr, 1, nullptr);
		praat_addAction1 (classFormant, 1, U"Get quantile...",
				nullptr, 1, REAL_Formant_getQuantile);
		praat_addAction1 (classFormant, 1, U"Get quantile of bandwidth...",
				nullptr, 1, REAL_Formant_getQuantileOfBandwidth);
		praat_addAction1 (classFormant, 1, U"Get mean...",
				nullptr, 1, REAL_Formant_getMean);
		praat_addAction1 (classFormant, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Formant_getStandardDeviation);
	praat_addAction1 (classFormant, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classFormant);
		praat_addAction1 (classFormant, 0, U"Sort",
				nullptr, 1, MODIFY_Formant_sort);
		praat_addAction1 (classFormant, 0, U"Formula (frequencies)...",
				nullptr, 1, MODIFY_Formant_formula_frequencies);
		praat_addAction1 (classFormant, 0, U"Formula (bandwidths)...",
				nullptr, 1, MODIFY_Formant_formula_bandwidths);
praat_addAction1 (classFormant, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"Track...",
			nullptr, 0, NEW_Formant_tracker);
	praat_addAction1 (classFormant, 0, U"Down to FormantTier",
			nullptr, GuiMenu_HIDDEN, NEW_Formant_downto_FormantTier);
	praat_addAction1 (classFormant, 0, U"Down to FormantGrid",
			nullptr, 0, NEW_Formant_downto_FormantGrid);
praat_addAction1 (classFormant, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classFormant, 0, U"To Matrix...",
			nullptr, 0, NEW_Formant_to_Matrix);

	praat_addAction1 (classHarmonicity, 0, U"Harmonicity help",
			nullptr, 0, HELP_Harmonicity_help);
praat_addAction1 (classHarmonicity, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classHarmonicity, 0, U"Draw...",
			nullptr, 0, GRAPHICS_Harmonicity_draw);
	praat_addAction1 (classHarmonicity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classHarmonicity);
		praat_addAction1 (classHarmonicity, 1, U"-- get content --",
				nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get value at time...",
				nullptr, 1, REAL_Harmonicity_getValueAtTime);
		praat_addAction1 (classHarmonicity, 1, U"Get value in frame...",
				nullptr, 1, REAL_Harmonicity_getValueInFrame);
		praat_addAction1 (classHarmonicity, 1, U"-- get extreme --",
				nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get minimum...",
				nullptr, 1, REAL_Harmonicity_getMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of minimum...",
				nullptr, 1, REAL_Harmonicity_getTimeOfMinimum);
		praat_addAction1 (classHarmonicity, 1, U"Get maximum...",
				nullptr, 1, REAL_Harmonicity_getMaximum);
		praat_addAction1 (classHarmonicity, 1, U"Get time of maximum...",
				nullptr, 1, REAL_Harmonicity_getTimeOfMaximum);
		praat_addAction1 (classHarmonicity, 1, U"-- get statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (classHarmonicity, 1, U"Get mean...",
				nullptr, 1, REAL_Harmonicity_getMean);
		praat_addAction1 (classHarmonicity, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Harmonicity_getStandardDeviation);
praat_addAction1 (classHarmonicity, 0, U"Modify", nullptr, 0, nullptr);
	praat_TimeFunction_modify_init (classHarmonicity);
	praat_addAction1 (classHarmonicity, 0, U"Formula...",
			nullptr, 0, MODIFY_Harmonicity_formula);
praat_addAction1 (classHarmonicity, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classHarmonicity, 0, U"To Matrix",
			nullptr, 0, NEW_Harmonicity_to_Matrix);

	praat_addAction1 (classIntensity, 0, U"Intensity help",
			nullptr, 0, HELP_Intensity_help);
	praat_addAction1 (classIntensity, 0, U"Draw...",
			nullptr, 0, GRAPHICS_Intensity_draw);
	praat_addAction1 (classIntensity, 1, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classIntensity);
		praat_addAction1 (classIntensity, 1, U"-- get content --",
				nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get value at time...",
				nullptr, 1, REAL_Intensity_getValueAtTime);
		praat_addAction1 (classIntensity, 1, U"Get value in frame...",
				nullptr, 1, REAL_Intensity_getValueInFrame);
		praat_addAction1 (classIntensity, 1, U"-- get extreme --",
				nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get minimum...",
				nullptr, 1, REAL_Intensity_getMinimum);
		praat_addAction1 (classIntensity, 1, U"Get time of minimum...",
				nullptr, 1, REAL_Intensity_getTimeOfMinimum);
		praat_addAction1 (classIntensity, 1, U"Get maximum...",
				nullptr, 1, REAL_Intensity_getMaximum);
		praat_addAction1 (classIntensity, 1, U"Get time of maximum...",
				nullptr, 1, REAL_Intensity_getTimeOfMaximum);
		praat_addAction1 (classIntensity, 1, U"-- get statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (classIntensity, 1, U"Get quantile...",
				nullptr, 1, REAL_Intensity_getQuantile);
		praat_addAction1 (classIntensity, 1, U"Get mean...",
				nullptr, 1, REAL_Intensity_getMean);
		praat_addAction1 (classIntensity, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Intensity_getStandardDeviation);
	praat_addAction1 (classIntensity, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classIntensity);
		praat_addAction1 (classIntensity, 0, U"Formula...",
				nullptr, 1, MODIFY_Intensity_formula);
praat_addAction1 (classIntensity, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classIntensity, 0, U"To IntensityTier (peaks)",
			nullptr, 0, NEW_Intensity_to_IntensityTier_peaks);
	praat_addAction1 (classIntensity, 0, U"To IntensityTier (valleys)",
			nullptr, 0, NEW_Intensity_to_IntensityTier_valleys);
praat_addAction1 (classIntensity, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classIntensity, 0, U"Down to IntensityTier",
			nullptr, 0, NEW_Intensity_downto_IntensityTier);
	praat_addAction1 (classIntensity, 0, U"Down to Matrix",
			nullptr, 0, NEW_Intensity_downto_Matrix);

	praat_addAction1 (classLtas, 0, U"Ltas help",
			nullptr, 0, HELP_Ltas_help);
	praat_addAction1 (classLtas, 0, U"Draw...",
			nullptr, 0, GRAPHICS_Ltas_draw);
	praat_addAction1 (classLtas, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classLtas, 1, U"Frequency domain", nullptr, 1, nullptr);
			praat_addAction1 (classLtas, 1, U"Get lowest frequency",
					nullptr, 2, REAL_Ltas_getLowestFrequency);
			praat_addAction1 (classLtas, 1, U"Get highest frequency",
					nullptr, 2, REAL_Ltas_getHighestFrequency);
		praat_addAction1 (classLtas, 1, U"Frequency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classLtas, 1, U"Get number of bins || Get number of bands",
					nullptr, 2, INTEGER_Ltas_getNumberOfBins);   // alternative GuiMenu_DEPRECATED_2004
			praat_addAction1 (classLtas, 1, U"Get bin width || Get band width",
					nullptr, 2, REAL_Ltas_getBinWidth);
			praat_addAction1 (classLtas, 1, U"Get frequency from bin number... || Get frequency from band...",
					nullptr, 2, REAL_Ltas_getFrequencyFromBinNumber);
			praat_addAction1 (classLtas, 1, U"Get bin number from frequency... || Get band from frequency...",
					nullptr, 2, REAL_Ltas_getBinNumberFromFrequency);
		praat_addAction1 (classLtas, 1, U"-- get content --",
				nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get value at frequency...",
				nullptr, 1, REAL_Ltas_getValueAtFrequency);
		praat_addAction1 (classLtas, 1, U"Get value in bin... || Get value in band...",
				nullptr, 1, REAL_Ltas_getValueInBin);
		praat_addAction1 (classLtas, 1, U"-- get extreme --",
				nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get minimum...",
				nullptr, 1, REAL_Ltas_getMinimum);
		praat_addAction1 (classLtas, 1, U"Get frequency of minimum...",
				nullptr, 1, REAL_Ltas_getFrequencyOfMinimum);
		praat_addAction1 (classLtas, 1, U"Get maximum...",
				nullptr, 1, REAL_Ltas_getMaximum);
		praat_addAction1 (classLtas, 1, U"Get frequency of maximum...",
				nullptr, 1, REAL_Ltas_getFrequencyOfMaximum);
		praat_addAction1 (classLtas, 1, U"-- get statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (classLtas, 1, U"Get mean...",
				nullptr, 1, REAL_Ltas_getMean);
		praat_addAction1 (classLtas, 1, U"Get slope...",
				nullptr, 1, REAL_Ltas_getSlope);
		praat_addAction1 (classLtas, 1, U"Get local peak height...",
				nullptr, 1, REAL_Ltas_getLocalPeakHeight);
		praat_addAction1 (classLtas, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Ltas_getStandardDeviation);
praat_addAction1 (classLtas, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Formula...",
			nullptr, 0, MODIFY_Ltas_formula);
praat_addAction1 (classLtas, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To SpectrumTier (peaks)",
			nullptr, 0, NEW_Ltas_to_SpectrumTier_peaks);
praat_addAction1 (classLtas, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Compute trend line...",
			nullptr, 0, NEW_Ltas_computeTrendLine);
	praat_addAction1 (classLtas, 0, U"Subtract trend line...",
			nullptr, 0, NEW_Ltas_subtractTrendLine);
praat_addAction1 (classLtas, 0, U"Combine", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"Merge",
			nullptr, GuiMenu_DEPRECATED_2005, NEW1_Ltases_merge);
	praat_addAction1 (classLtas, 0, U"Average",
			nullptr, 0, NEW1_Ltases_average);
praat_addAction1 (classLtas, 0, U"Hack", nullptr, 0, nullptr);
	praat_addAction1 (classLtas, 0, U"To Matrix",
			nullptr, 0, NEW_Ltas_to_Matrix);

	praat_addAction1 (classManipulation, 0, U"Manipulation help",
			nullptr, 0, HELP_Manipulation_help);
	praat_addAction1 (classManipulation, 1, U"View & Edit || Edit",
			nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Manipulation_viewAndEdit);
	praat_addAction1 (classManipulation, 0, U"Play (overlap-add) || Play (PSOLA)",
			nullptr, 0, PLAY_Manipulation_play_overlapAdd);   // alternative GuiMenu_DEPRECATED_2007
	praat_addAction1 (classManipulation, 0, U"Play (LPC)",
			nullptr, 0, PLAY_Manipulation_play_lpc);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (overlap-add) || Get resynthesis (PSOLA)",
			nullptr, 0, NEW_Manipulation_getResynthesis_overlapAdd);
	praat_addAction1 (classManipulation, 0, U"Get resynthesis (LPC)",
			nullptr, 0, NEW_Manipulation_getResynthesis_lpc);
	praat_addAction1 (classManipulation, 0, U"Extract original sound",
			nullptr, 0, NEW_Manipulation_extractOriginalSound);
	praat_addAction1 (classManipulation, 0, U"Extract pulses",
			nullptr, 0, NEW_Manipulation_extractPulses);
	praat_addAction1 (classManipulation, 0, U"Extract pitch tier",
			nullptr, 0, NEW_Manipulation_extractPitchTier);
	praat_addAction1 (classManipulation, 0, U"Extract duration tier",
			nullptr, 0, NEW_Manipulation_extractDurationTier);
	praat_addAction1 (classManipulation, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classManipulation);
		praat_addAction1 (classManipulation, 0, U"Replace pitch tier?",
				nullptr, 1, HELP_Manipulation_replacePitchTier_help);
		praat_addAction1 (classManipulation, 0, U"Replace duration tier?",
				nullptr, 1, HELP_Manipulation_replaceDurationTier_help);
	praat_addAction1 (classManipulation, 1, U"Save as text file without Sound... || Write to text file without Sound...",
			nullptr, 0, SAVE_Manipulation_writeToTextFileWithoutSound);
	praat_addAction1 (classManipulation, 1, U"Save as binary file without Sound... || Write to binary file without Sound...",
			nullptr, 0, SAVE_Manipulation_writeToBinaryFileWithoutSound);

	praat_addAction1 (classParamCurve, 0, U"ParamCurve help",
			nullptr, 0, HELP_ParamCurve_help);
praat_addAction1 (classParamCurve, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classParamCurve, 0, U"Draw...",
			nullptr, 0, GRAPHICS_ParamCurve_draw);

	praat_addAction1 (classPitch, 0, U"Pitch help",
			nullptr, 0, HELP_Pitch_help);
	praat_addAction1 (classPitch, 1, U"View & Edit || Edit",
			nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Pitch_viewAndEdit);
	praat_addAction1 (classPitch, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Play pulses",
				nullptr, 1, PLAY_Pitch_play);
		praat_addAction1 (classPitch, 0, U"Hum",
				nullptr, 1, PLAY_Pitch_hum);
		praat_addAction1 (classPitch, 0, U"-- to sound --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"To Sound (pulses)",
				nullptr, 1, NEW_Pitch_to_Sound_pulses);
		praat_addAction1 (classPitch, 0, U"To Sound (hum)",
				nullptr, 1, NEW_Pitch_to_Sound_hum);
		praat_addAction1 (classPitch, 0, U"To Sound (sine)...",
				nullptr, 1, NEW_Pitch_to_Sound_sine);
	praat_addAction1 (classPitch, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Draw...",
				nullptr, 1, GRAPHICS_Pitch_draw);
		praat_addAction1 (classPitch, 0, U"Draw logarithmic...",
				nullptr, 1, GRAPHICS_Pitch_drawLogarithmic);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 100 Hz)... || Draw semitones...",
				nullptr, 1, GRAPHICS_Pitch_drawSemitones100);   // alternativeGuiMenu_DEPRECATED_2012
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 200 Hz)...",
				nullptr, 1, GRAPHICS_Pitch_drawSemitones200);
		praat_addAction1 (classPitch, 0, U"Draw semitones (re 440 Hz)...",
				nullptr, 1, GRAPHICS_Pitch_drawSemitones440);
		praat_addAction1 (classPitch, 0, U"Draw mel...",
				nullptr, 1, GRAPHICS_Pitch_drawMel);
		praat_addAction1 (classPitch, 0, U"Draw erb...",
				nullptr, 1, GRAPHICS_Pitch_drawErb);
		praat_addAction1 (classPitch, 0, U"Speckle...",
				nullptr, 1, GRAPHICS_Pitch_speckle);
		praat_addAction1 (classPitch, 0, U"Speckle logarithmic...",
				nullptr, 1, GRAPHICS_Pitch_speckleLogarithmic);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 100 Hz)... || Speckle semitones...",
				nullptr, 1, GRAPHICS_Pitch_speckleSemitones100);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 200 Hz)...",
				nullptr, 1, GRAPHICS_Pitch_speckleSemitones200);
		praat_addAction1 (classPitch, 0, U"Speckle semitones (re 440 Hz)...",
				nullptr, 1, GRAPHICS_Pitch_speckleSemitones440);
		praat_addAction1 (classPitch, 0, U"Speckle mel...",
				nullptr, 1, GRAPHICS_Pitch_speckleMel);
		praat_addAction1 (classPitch, 0, U"Speckle erb...",
				nullptr, 1, GRAPHICS_Pitch_speckleErb);
	praat_addAction1 (classPitch, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classPitch);
		praat_addAction1 (classPitch, 1, U"-- get content --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Count voiced frames",
				nullptr, 1, INTEGER_Pitch_getNumberOfVoicedFrames);
		praat_addAction1 (classPitch, 1, U"Get value at time...",
				nullptr, 1, REAL_Pitch_getValueAtTime);
		praat_addAction1 (classPitch, 1, U"List values at times...",
				nullptr, 1, NUMVEC_Pitch_listValuesAtTimes);
		praat_addAction1 (classPitch, 1, U"Get value in frame...",
				nullptr, 1, REAL_Pitch_getValueInFrame);
		praat_addAction1 (classPitch, 1, U"List values in all frames...",
				nullptr, 1, NUMVEC_Pitch_listValuesInAllFrames);
		praat_addAction1 (classPitch, 1, U"-- get extreme --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get minimum...",
				nullptr, 1, REAL_Pitch_getMinimum);
		praat_addAction1 (classPitch, 1, U"Get time of minimum...",
				nullptr, 1, REAL_Pitch_getTimeOfMinimum);
		praat_addAction1 (classPitch, 1, U"Get maximum...",
				nullptr, 1, REAL_Pitch_getMaximum);
		praat_addAction1 (classPitch, 1, U"Get time of maximum...",
				nullptr, 1, REAL_Pitch_getTimeOfMaximum);
		praat_addAction1 (classPitch, 1, U"-- get statistics --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get quantile...",
				nullptr, 1, REAL_Pitch_getQuantile);
		/*praat_addAction1 (classPitch, 1, U"Get spreading...",
				nullptr, 1, REAL_Pitch_getSpreading);*/
		praat_addAction1 (classPitch, 1, U"Get mean...",
				nullptr, 1, REAL_Pitch_getMean);
		praat_addAction1 (classPitch, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Pitch_getStandardDeviation);
		praat_addAction1 (classPitch, 1, U"-- get slope --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Get mean absolute slope...",
				nullptr, 1, REAL_Pitch_getMeanAbsoluteSlope);
		praat_addAction1 (classPitch, 1, U"Get slope without octave jumps",
				nullptr, 1, REAL_Pitch_getMeanAbsSlope_noOctave);
		praat_addAction1 (classPitch, 2, U"-- query two --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 2, U"Count differences",
				nullptr, 1, INFO_Pitch_difference);
		praat_addAction1 (classPitch, 2, U"-- hack --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 1, U"Internal", nullptr, 1, nullptr);
			praat_addAction1 (classPitch, 0, U"Tabulate candidates",
					nullptr, 2, NEW_Pitch_tabulateCandidates);
			praat_addAction1 (classPitch, 0, U"Tabulate candidates in frame...",
					nullptr, 2, NEW_Pitch_tabulateCandidatesInFrame);
			praat_addAction1 (classPitch, 1, U"Get all candidates in frame...",
					nullptr, 2, NUMMAT_Pitch_getAllCandidatesInFrame);
	praat_addAction1 (classPitch, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classPitch);
		praat_addAction1 (classPitch, 0, U"Formula...",
				nullptr, 1, MODIFY_Pitch_formula);
	praat_addAction1 (classPitch, 0, U"Annotate -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextGrid...",
				nullptr, 1, NEW_Pitch_to_TextGrid);
		praat_addAction1 (classPitch, 0, U"-- to single tier --",
				nullptr, GuiMenu_HIDDEN + GuiMenu_DEPTH_1, nullptr);
		praat_addAction1 (classPitch, 0, U"To TextTier",
				nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1, NEW_Pitch_to_TextTier);
		praat_addAction1 (classPitch, 0, U"To IntervalTier",
				nullptr, GuiMenu_HIDDEN | GuiMenu_DEPTH_1, NEW_Pitch_to_IntervalTier);
	praat_addAction1 (classPitch, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"To PointProcess",
				nullptr, 1, NEW_Pitch_to_PointProcess);
	praat_addAction1 (classPitch, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classPitch, 0, U"Interpolate",
				nullptr, 1, NEW_Pitch_interpolate);
		praat_addAction1 (classPitch, 0, U"Smooth...",
				nullptr, 1, NEW_Pitch_smooth);
		praat_addAction1 (classPitch, 0, U"Subtract linear fit...",
				nullptr, 1, NEW_Pitch_subtractLinearFit);
		praat_addAction1 (classPitch, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classPitch, 0, U"Kill octave jumps",
					nullptr, 2, NEW_Pitch_killOctaveJumps);
		praat_addAction1 (classPitch, 0, U"-- to other types --",
				nullptr, 1, nullptr);
		praat_addAction1 (classPitch, 0, U"Down to PitchTier",
				nullptr, 1, NEW_Pitch_downto_PitchTier);
		praat_addAction1 (classPitch, 0, U"To Matrix",
				nullptr, 1, NEW_Pitch_to_Matrix);

	praat_addAction1 (classPolygon, 0, U"Polygon help",
			nullptr, 0, HELP_Polygon_help);
	praat_addAction1 (classPolygon, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classPolygon, 0, U"Draw...",
				nullptr, 1, GRAPHICS_Polygon_draw);
		praat_addAction1 (classPolygon, 0, U"Draw closed...",
				nullptr, 1, GRAPHICS_Polygon_drawClosed);
		praat_addAction1 (classPolygon, 0, U"Paint...",
				nullptr, 1, GRAPHICS_Polygon_paint);
		praat_addAction1 (classPolygon, 0, U"Draw circles...",
				nullptr, 1, GRAPHICS_Polygon_drawCircles);
		praat_addAction1 (classPolygon, 0, U"Paint circles...",
				nullptr, 1, GRAPHICS_Polygon_paintCircles);
		praat_addAction1 (classPolygon, 2, U"Draw connection...",
				nullptr, 1, GRAPHICS_Polygons_drawConnection);
	praat_addAction1 (classPolygon, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classPolygon, 0, U"Randomize",
				nullptr, 1, MODIFY_Polygon_randomize);
		praat_addAction1 (classPolygon, 0, U"Salesperson...",
				nullptr, 1, MODIFY_Polygon_salesperson);
	praat_addAction1 (classPolygon, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classPolygon, 0, U"To Matrix", nullptr, 1, NEW_Polygon_to_Matrix);

	praat_addAction1 (classSpectrogram, 0, U"Spectrogram help",
			nullptr, 0, HELP_Spectrogram_help);
	praat_addAction1 (classSpectrogram, 1, U"View",
			nullptr, 0, EDITOR_ONE_Spectrogram_view);
	praat_addAction1 (classSpectrogram, 1, U"Play movie || Movie",
			nullptr, 0, MOVIE_Spectrogram_playMovie);
	praat_addAction1 (classSpectrogram, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFrameSampled_query_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 1, U"Get power at...",
				nullptr, 1, REAL_Spectrogram_getPowerAt);
	praat_addAction1 (classSpectrogram, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"Paint...",
				nullptr, 1, GRAPHICS_Spectrogram_paint);
	praat_addAction1 (classSpectrogram, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Spectrum (slice)...",
				nullptr, 1, NEW_Spectrogram_to_Spectrum);
	praat_addAction1 (classSpectrogram, 0, U"Synthesize -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Sound...",
				nullptr, 1, NEW_Spectrogram_to_Sound);
	praat_addAction1 (classSpectrogram, 0, U"Modify -", nullptr, 0, nullptr);
		praat_TimeFunction_modify_init (classSpectrogram);
		praat_addAction1 (classSpectrogram, 0, U"Formula...",
				nullptr, 1, MODIFY_Spectrogram_formula);
	praat_addAction1 (classSpectrogram, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrogram, 0, U"To Matrix",
				nullptr, 1, NEW_Spectrogram_to_Matrix);

	praat_addAction1 (classSpectrum, 0, U"Spectrum help",
			nullptr, 0, HELP_Spectrum_help);
	praat_addAction1 (classSpectrum, 1, U"View & Edit || Edit",
			nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Spectrum_viewAndEdit);
	praat_addAction1 (classSpectrum, 0, U"Sound -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Sound || To Sound (fft)",
				nullptr, 1, NEW_Spectrum_to_Sound);   // alternative GuiMenu_DEPRECATED_2004
	praat_addAction1 (classSpectrum, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Draw...",
				nullptr, 1, GRAPHICS_Spectrum_draw);
		praat_addAction1 (classSpectrum, 0, U"Draw (log freq)...",
				nullptr, 1, GRAPHICS_Spectrum_drawLogFreq);
	praat_addAction1 (classSpectrum, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Tabulate...",
				nullptr, 1, NEW_Spectrum_tabulate);
		praat_addAction1 (classSpectrum, 1, U"Tabulate (verbose)",
				nullptr, 1, NEW_Spectrum_tabulate_verbose);
	praat_addAction1 (classSpectrum, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Frequency domain", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get lowest frequency",
					nullptr, 2, REAL_Spectrum_getLowestFrequency);
			praat_addAction1 (classSpectrum, 1, U"Get highest frequency",
					nullptr, 2, REAL_Spectrum_getHighestFrequency);
		praat_addAction1 (classSpectrum, 1, U"Frequency sampling", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 1, U"Get number of bins",
					nullptr, 2, INTEGER_Spectrum_getNumberOfBins);
			praat_addAction1 (classSpectrum, 1, U"Get bin width",
					nullptr, 2, REAL_Spectrum_getBinWidth);
			praat_addAction1 (classSpectrum, 1, U"Get frequency from bin number... || Get frequency from bin...",
					nullptr, 2, REAL_Spectrum_getFrequencyFromBin);   // alternative GuiMenu_DEPRECATED_2004
			praat_addAction1 (classSpectrum, 1, U"Get bin number from frequency... || Get bin from frequency...",
					nullptr, 2, REAL_Spectrum_getBinNumberFromFrequency);
		praat_addAction1 (classSpectrum, 1, U"-- get content --",
				nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get real value in bin...",
				nullptr, 1, REAL_Spectrum_getRealValueInBin);
		praat_addAction1 (classSpectrum, 1, U"Get imaginary value in bin...",
				nullptr, 1, REAL_Spectrum_getImaginaryValueInBin);
		praat_addAction1 (classSpectrum, 1, U"-- get energy --",
				nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get band energy...",
				nullptr, 1, REAL_Spectrum_getBandEnergy);
		praat_addAction1 (classSpectrum, 1, U"Get band density...",
				nullptr, 1, REAL_Spectrum_getBandDensity);
		praat_addAction1 (classSpectrum, 1, U"Get band energy difference...",
				nullptr, 1, REAL_Spectrum_getBandEnergyDifference);
		praat_addAction1 (classSpectrum, 1, U"Get band density difference...",
				nullptr, 1, REAL_Spectrum_getBandDensityDifference);
		praat_addAction1 (classSpectrum, 1, U"-- get moments --",
				nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get centre of gravity...",
				nullptr, 1, REAL_Spectrum_getCentreOfGravity);
		praat_addAction1 (classSpectrum, 1, U"Get standard deviation...",
				nullptr, 1, REAL_Spectrum_getStandardDeviation);
		praat_addAction1 (classSpectrum, 1, U"Get skewness...",
				nullptr, 1, REAL_Spectrum_getSkewness);
		praat_addAction1 (classSpectrum, 1, U"Get kurtosis...",
				nullptr, 1, REAL_Spectrum_getKurtosis);
		praat_addAction1 (classSpectrum, 1, U"Get central moment...",
				nullptr, 1, REAL_Spectrum_getCentralMoment);
		praat_addAction1 (classSpectrum, 1, U"-- search --",
				nullptr, 1, nullptr);
		praat_addAction1 (classSpectrum, 1, U"Get frequency of nearest maximum...",
				nullptr, 1, REAL_Spectrum_getFrequencyOfNearestMaximum);
		praat_addAction1 (classSpectrum, 1, U"Get sound pressure level of nearest maximum...",
				nullptr, 1, REAL_Spectrum_getSoundPressureLevelOfNearestMaximum);
	praat_addAction1 (classSpectrum, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Formula...",
				nullptr, 1, MODIFY_Spectrum_formula);
		praat_addAction1 (classSpectrum, 0, U"Filter (pass Hann band)...",
				nullptr, 1, MODIFY_Spectrum_passHannBand);
		praat_addAction1 (classSpectrum, 0, U"Filter (stop Hann band)...",
				nullptr, 1, MODIFY_Spectrum_stopHannBand);
	praat_addAction1 (classSpectrum, 0, U"Analyse -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"To Excitation...",
				nullptr, 1, NEW_Spectrum_to_Excitation);
		praat_addAction1 (classSpectrum, 0, U"To SpectrumTier (peaks)",
				nullptr, 1, NEW_Spectrum_to_SpectrumTier_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Formant (peaks)...",
				nullptr, 1, NEW_Spectrum_to_Formant_peaks);
		praat_addAction1 (classSpectrum, 0, U"To Ltas...",
				nullptr, 1, NEW_Spectrum_to_Ltas);
		praat_addAction1 (classSpectrum, 0, U"To Ltas (1-to-1)",
				nullptr, 1, NEW_Spectrum_to_Ltas_1to1);
		praat_addAction1 (classSpectrum, 0, U"To Spectrogram",
				nullptr, 1, NEW_Spectrum_to_Spectrogram);
	praat_addAction1 (classSpectrum, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classSpectrum, 0, U"Cepstral smoothing...",
				nullptr, 1, NEW_Spectrum_cepstralSmoothing);
		praat_addAction1 (classSpectrum, 0, U"LPC smoothing...",
				nullptr, 1, NEW_Spectrum_lpcSmoothing);
		praat_addAction1 (classSpectrum, 0, U"Hack", nullptr, 1, nullptr);
			praat_addAction1 (classSpectrum, 0, U"To Matrix",
					nullptr, 2, NEW_Spectrum_to_Matrix);

	praat_addAction1 (classStrings, 0, U"Strings help", nullptr, 0, HELP_Strings_help);
	praat_addAction1 (classStrings, 1, U"Save as raw text file... || Write to raw text file...",
			nullptr, 0, SAVE_Strings_writeToRawTextFile);
	praat_addAction1 (classStrings, 1, U"View & Edit || Edit",
			nullptr, GuiMenu_ATTRACTIVE, EDITOR_ONE_Strings_viewAndEdit);
	praat_addAction1 (classStrings, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 2, U"Equal?",
				nullptr, 1, BOOLEAN_Strings_equal);
		praat_addAction1 (classStrings, 1, U"Get number of strings",
				nullptr, 1, INTEGER_Strings_getNumberOfStrings);
		praat_addAction1 (classStrings, 1, U"Get string...",
				nullptr, 1, STRING_Strings_getString);
		praat_addAction1 (classStrings, 1, U"List all strings",
				nullptr, 1, STRVEC_Strings_listAllStrings);
	praat_addAction1 (classStrings, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"Set string...",
				nullptr, 1, MODIFY_Strings_setString);
		praat_addAction1 (classStrings, 0, U"Insert string...",
				nullptr, 1, MODIFY_Strings_insertString);
		praat_addAction1 (classStrings, 0, U"Remove string...",
				nullptr, 1, MODIFY_Strings_removeString);
		praat_addAction1 (classStrings, 0, U"-- modify order --",
				nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Randomize",
				nullptr, 1, MODIFY_Strings_randomize);
		praat_addAction1 (classStrings, 0, U"Sort",
				nullptr, 1, MODIFY_Strings_sort);
		praat_addAction1 (classStrings, 0, U"-- convert --",
				nullptr, 1, nullptr);
		praat_addAction1 (classStrings, 0, U"Convert to backslash trigraphs || Genericize",
				nullptr, 1, MODIFY_Strings_genericize);   // alternative GuiMenu_DEPRECATED_2016
		praat_addAction1 (classStrings, 0, U"Convert to Unicode || Nativize",
				nullptr, 1, MODIFY_Strings_nativize);
	praat_addAction1 (classStrings, 0, U"Convert -", nullptr, 0, nullptr);
		praat_addAction1 (classStrings, 0, U"Replace all...",
				nullptr, 1, NEW_Strings_replaceAll);
praat_addAction1 (classStrings, 0, U"Analyze", nullptr, 0, nullptr);
	praat_addAction1 (classStrings, 0, U"To Distributions",
			nullptr, 0, NEW_Strings_to_Distributions);
praat_addAction1 (classStrings, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classStrings, 0, U"To WordList",
			nullptr, 0, NEW_Strings_to_WordList);

	praat_addAction1 (classTransition, 0, U"Transition help", nullptr, 0, HELP_Transition_help);
praat_addAction1 (classTransition, 0, U"Draw", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Draw as numbers...",
			nullptr, 0, GRAPHICS_Transition_drawAsNumbers);
praat_addAction1 (classTransition, 0, U"Analyse", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Eigen",
			nullptr, 0, NEWTIMES2_Transition_eigen);
	praat_addAction1 (classTransition, 0, U"Conflate",
			nullptr, 0, NEW_Transition_conflate);
praat_addAction1 (classTransition, 0, U"Synthesize", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"Power...",
			nullptr, 0, NEW_Transition_power);
praat_addAction1 (classTransition, 0, U"Cast", nullptr, 0, nullptr);
	praat_addAction1 (classTransition, 0, U"To Matrix",
			nullptr, 0, NEW_Transition_to_Matrix);

	praat_addAction2 (classDistributions, 1, classTransition, 1, U"Map",
			nullptr, 0, NEW1_Distributions_Transition_map);
	praat_addAction2 (classDistributions, 1, classTransition, 1, U"To Transition...",
			nullptr, 0, NEW1_Distributions_to_Transition_adj);
	praat_addAction2 (classDistributions, 2, classTransition, 1, U"To Transition (noise)...",
			nullptr, 0, NEW1_Distributions_to_Transition_noise_adj);

	praat_addAction2 (classFormant, 1, classPointProcess, 1, U"To FormantTier",
			nullptr, 0, NEW1_Formant_PointProcess_to_FormantTier);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter",
			nullptr, 0, NEW1_Sound_Formant_filter);
	praat_addAction2 (classFormant, 1, classSound, 1, U"Filter (no scale)",
			nullptr, 0, NEW1_Sound_Formant_filter_noscale);

praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw", nullptr, 0, nullptr);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Draw (phonetogram)...",
			nullptr, 0, GRAPHICS_Pitch_Intensity_draw);
praat_addAction2 (classIntensity, 1, classPitch, 1, U"Query", nullptr, 0, nullptr);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Get mean",
			nullptr, 0, REAL_Pitch_Intensity_getMean);
	praat_addAction2 (classIntensity, 1, classPitch, 1, U"Get mean absolute slope",
			nullptr, 0, REAL_Pitch_Intensity_getMeanAbsoluteSlope);

	praat_addAction2 (classIntensity, 1, classPointProcess, 1, U"To IntensityTier",
			nullptr, 0, NEW1_Intensity_PointProcess_to_IntensityTier);

	praat_addAction2 (classManipulation, 1, classSound, 1, U"Replace original sound",
			nullptr, 0, MODIFY_Manipulation_replaceOriginalSound);
	praat_addAction2 (classManipulation, 1, classPointProcess, 1, U"Replace pulses",
			nullptr, 0, MODIFY_Manipulation_replacePulses);
	praat_addAction2 (classManipulation, 1, classPitchTier, 1, U"Replace pitch tier",
			nullptr, 0, MODIFY_Manipulation_replacePitchTier);
	praat_addAction2 (classManipulation, 1, classDurationTier, 1, U"Replace duration tier",
			nullptr, 0, MODIFY_Manipulation_replaceDurationTier);
	praat_addAction2 (classManipulation, 1, classTextTier, 1, U"To Manipulation",
			nullptr, 0, NEW1_Manipulation_TextTier_to_Manipulation);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"Draw...",
			nullptr, 0, GRAPHICS_PitchTier_Pitch_draw);
	praat_addAction2 (classPitch, 1, classPitchTier, 1, U"To Pitch",
			nullptr, 0, NEW1_Pitch_PitchTier_to_Pitch);
	praat_addAction2 (classPitch, 1, classPointProcess, 1, U"To PitchTier",
			nullptr, 0, NEW1_Pitch_PointProcess_to_PitchTier);
	praat_addAction3 (classPitch, 1, classPointProcess, 1, classSound, 1, U"Voice report...",
			nullptr, 0, INFO_Sound_Pitch_PointProcess_voiceReport);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (cc)",
			nullptr, 0, NEW1_Sound_Pitch_to_PointProcess_cc);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To PointProcess (peaks)...",
			nullptr, 0, NEW1_Sound_Pitch_to_PointProcess_peaks);
	praat_addAction2 (classPitch, 1, classSound, 1, U"To Manipulation",
			nullptr, 0, NEW1_Sound_Pitch_to_Manipulation);

	praat_addAction4 (classDurationTier, 1, classPitchTier, 1, classPointProcess, 1, classSound, 1, U"To Sound...",
			nullptr, 0, NEW1_Sound_Point_Pitch_Duration_to_Sound);

	Melder_stopwatch ();
	INCLUDE_MANPAGES (manual_Manual_init)
	INCLUDE_MANPAGES (manual_commands_init)
	INCLUDE_MANPAGES (manual_formulas_init)
	INCLUDE_MANPAGES (manual_scripting_init)
	INCLUDE_MANPAGES (manual_demoWindow_init)
	INCLUDE_MANPAGES (manual_functions_init)
	INCLUDE_MANPAGES (manual_Picture_init)

	INCLUDE_MANPAGES (manual_Fon_init)
	INCLUDE_MANPAGES (manual_whatsnew_init)
	INCLUDE_MANPAGES (manual_licenses_init)
	INCLUDE_MANPAGES (manual_tutorials_init)
	INCLUDE_MANPAGES (manual_glossary_init)
	INCLUDE_MANPAGES (manual_references_init)
	INCLUDE_MANPAGES (manual_soundFiles_init)
	INCLUDE_MANPAGES (manual_statistics_init)
	INCLUDE_MANPAGES (manual_voice_init)
	INCLUDE_MANPAGES (manual_programming_init)
	INCLUDE_MANPAGES (manual_gram_init)
	INCLUDE_MANPAGES (manual_ExperimentMFC_init)
	INCLUDE_MANPAGES (manual_EEG_init)
	//TRACE
	trace (U"manuals: ", Melder_stopwatch ());

	INCLUDE_LIBRARY (praat_EEG_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new synthesis --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_KlattGrid_init)   // from dwtools
	INCLUDE_LIBRARY (praat_uvafon_Artsynth_init)
	INCLUDE_LIBRARY (praat_David_init)   // starting with SpeechSynthesizer (last checked 2024-09-07)
	INCLUDE_LIBRARY (praat_uvafon_sensors_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new recognition --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_SpeechRecognizer_init)
	praat_addMenuCommand (U"Objects", U"New", U"-- new grammars --", nullptr, 0, nullptr);
	INCLUDE_LIBRARY (praat_uvafon_gram_init)
	INCLUDE_LIBRARY (praat_uvafon_FFNet_init)
	INCLUDE_LIBRARY (praat_uvafon_LPC_init)
	praat_ExperimentMFC_init ();

	Data_recognizeFileType (tabSeparatedFileRecognizer);   // at end, as a sort of last resort
}

/* End of file praat_uvafon_init.cpp */
