/* SoundAnalysisArea.cpp
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

#include "SoundAnalysisArea.h"
#include "Sound_and_Spectrogram.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_Pitch.h"
#include "Sound_to_Intensity.h"
#include "Sound_to_Formant.h"
#include "Pitch_to_PointProcess.h"
#include "VoiceAnalysis.h"
#include "EditorM.h"
#include "praat_script.h"

Thing_implement (SoundAnalysisArea, FunctionArea, 0);

#include "enums_getText.h"
#include "SoundAnalysisArea_enums.h"
#include "enums_getValue.h"
#include "SoundAnalysisArea_enums.h"

#include "Prefs_define.h"
#include "SoundAnalysisArea_prefs.h"
#include "Prefs_install.h"
#include "SoundAnalysisArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "SoundAnalysisArea_prefs.h"

void structSoundAnalysisArea :: v_reset_analysis () {
	our d_spectrogram. reset();
	our d_pitch. reset();
	our d_intensity. reset();
	our d_formant. reset();
	our d_pulses. reset();
}

#define DEFINE_dynamic_instancePref_pitch(type,setting) \
type structSoundAnalysisArea :: dynamic_instancePref_pitch_##setting () { \
	switch (our instancePref_pitch_method()) { \
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_AUTOCORRELATION: \
			return our instancePref_pitch_filteredAC_##setting(); \
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION: \
			return our instancePref_pitch_rawCC_##setting(); \
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION: \
			return our instancePref_pitch_rawAC_##setting(); \
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_CROSS_CORRELATION: \
			return our instancePref_pitch_filteredCC_##setting(); \
		default: \
			Melder_crash (U"Unknown pitch analysis method ", (int) our instancePref_pitch_method(), U"."); \
	} \
}
DEFINE_dynamic_instancePref_pitch (double, floor)
DEFINE_dynamic_instancePref_pitch (kPitch_unit, unit)
DEFINE_dynamic_instancePref_pitch (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod)
DEFINE_dynamic_instancePref_pitch (double, viewFrom)
DEFINE_dynamic_instancePref_pitch (double, viewTo)
DEFINE_dynamic_instancePref_pitch (bool, veryAccurate)
DEFINE_dynamic_instancePref_pitch (integer, maximumNumberOfCandidates)
DEFINE_dynamic_instancePref_pitch (double, silenceThreshold)
DEFINE_dynamic_instancePref_pitch (double, voicingThreshold)

double structSoundAnalysisArea :: dynamic_instancePref_pitch_ceilingOrTop () {
	switch (our instancePref_pitch_method()) {
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_AUTOCORRELATION:
			return our instancePref_pitch_filteredAC_top();
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION:
			return our instancePref_pitch_rawCC_ceiling();
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION:
			return our instancePref_pitch_rawAC_ceiling();
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_CROSS_CORRELATION:
			return our instancePref_pitch_filteredCC_top();
		default:
			Melder_crash (U"Unknown pitch analysis method ", (int) our instancePref_pitch_method(), U".");
	}
}

static double dynamic_instancePref_pitch_attenuationAtTop (SoundAnalysisArea me) {
	switch (my instancePref_pitch_method()) {
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_AUTOCORRELATION:
			return my instancePref_pitch_filteredAC_attenuationAtTop();
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_CROSS_CORRELATION:
			return my instancePref_pitch_filteredCC_attenuationAtTop();
		default:
			Melder_crash (U"Unknown pitch analysis method ", (int) my instancePref_pitch_method(), U".");
	}
}
static double periodsPerAnalysisWindow (SoundAnalysisArea me) {
	switch (my instancePref_pitch_method()) {
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_AUTOCORRELATION:
			return 3.0;
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION:
			return 1.0;
		case kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION:
			return 3.0;
		case kSoundAnalysisArea_pitch_analysisMethod::FILTERED_CROSS_CORRELATION:
			return 1.0;
		default:
			Melder_crash (U"Unknown pitch analysis method ", (int) my instancePref_pitch_method(), U".");
	}
}

/*
	Some tryToCompute<Analysis>() functions.
	The "try" means that any exceptions that are generated, will be ignored;
	this is necessary because these functions have to be used in an editor window,
	where they have to work in the background,
	because they are not explicly called by a user action.

	Postcondition:
		- If a tryToCompute<Analysis>() function fails, the <Analysis> should be null;
		  this is how tryToCompute<Analysis>() signals failure.
*/
static autoSound extractSoundOrNull (SoundAnalysisArea me, double tmin, double tmax) {
	autoSound sound;
	if (my longSound()) {
		if (Function_intersectRangeWithDomain (my longSound(), & tmin, & tmax))
			sound = LongSound_extractPart (my longSound(), tmin, tmax, true);
	} else if (my sound()) {
		if (Function_intersectRangeWithDomain (my sound(), & tmin, & tmax))
			sound = Sound_extractPart (my sound(), tmin, tmax, kSound_windowShape::RECTANGULAR, 1.0, true);
	}
	return sound;
}
static void tryToComputeSpectrogram (SoundAnalysisArea me) {
	autoMelderProgressOff progress;
	const double margin = ( my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::GAUSSIAN ?
			my instancePref_spectrogram_windowLength() : 0.5 * my instancePref_spectrogram_windowLength() );
	try {
		autoSound sound = extractSoundOrNull (me, my startWindow() - margin, my endWindow() + margin);
		if (! sound)
			return;
		my d_spectrogram = Sound_to_Spectrogram_e (sound.get(),
			my instancePref_spectrogram_windowLength(),
			my instancePref_spectrogram_viewTo(),
			(my endWindow() - my startWindow()) / my instancePref_spectrogram_timeSteps(),
			my instancePref_spectrogram_viewTo() / my instancePref_spectrogram_frequencySteps(),
			my instancePref_spectrogram_windowShape(), 8.0, 8.0
		);
		my d_spectrogram -> xmin = my startWindow();
		my d_spectrogram -> xmax = my endWindow();
	} catch (MelderError) {
		my d_spectrogram. reset();   // signal a failure
		Melder_clearError ();
	}
}
static void tryToComputePitch (SoundAnalysisArea me) {
	autoMelderProgressOff progress;
	const double margin = ( my dynamic_instancePref_pitch_veryAccurate() ? 3.0 : 1.5 ) / my dynamic_instancePref_pitch_floor();
	try {
		autoSound sound = extractSoundOrNull (me, my startWindow() - margin, my endWindow() + margin);
		if (! sound)
			return;
		const double pitchTimeStep = (
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow() - my startWindow()) / my instancePref_numberOfTimeStepsPerView() :
			0.0   // the default: determined by pitch floor
		);
		if (my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::FILTERED_AUTOCORRELATION)
			my d_pitch = Sound_to_Pitch_filteredAc (sound.get(),
				pitchTimeStep, my instancePref_pitch_filteredAC_floor(), my instancePref_pitch_filteredAC_top(),
				my instancePref_pitch_filteredAC_maximumNumberOfCandidates(), my instancePref_pitch_filteredAC_veryAccurate(),
				my instancePref_pitch_filteredAC_attenuationAtTop(),
				my instancePref_pitch_filteredAC_silenceThreshold(), my instancePref_pitch_filteredAC_voicingThreshold(),
				my instancePref_pitch_filteredAC_octaveCost(), my instancePref_pitch_filteredAC_octaveJumpCost(),
				my instancePref_pitch_filteredAC_voicedUnvoicedCost()
			);
		else if	(my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION)
			my d_pitch = Sound_to_Pitch_rawCc (sound.get(),
				pitchTimeStep, my instancePref_pitch_rawCC_floor(), my instancePref_pitch_rawCC_ceiling(),
				my instancePref_pitch_rawCC_maximumNumberOfCandidates(), my instancePref_pitch_rawCC_veryAccurate(),
				my instancePref_pitch_rawCC_silenceThreshold(), my instancePref_pitch_rawCC_voicingThreshold(),
				my instancePref_pitch_rawCC_octaveCost(), my instancePref_pitch_rawCC_octaveJumpCost(),
				my instancePref_pitch_rawCC_voicedUnvoicedCost()
			);
		else if (my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION)
			my d_pitch = Sound_to_Pitch_rawAc (sound.get(),
				pitchTimeStep, my instancePref_pitch_rawAC_floor(), my instancePref_pitch_rawAC_ceiling(),
				my instancePref_pitch_rawAC_maximumNumberOfCandidates(), my instancePref_pitch_rawAC_veryAccurate(),
				my instancePref_pitch_rawAC_silenceThreshold(), my instancePref_pitch_rawAC_voicingThreshold(),
				my instancePref_pitch_rawAC_octaveCost(), my instancePref_pitch_rawAC_octaveJumpCost(),
				my instancePref_pitch_rawAC_voicedUnvoicedCost()
			);
		else if (my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::FILTERED_CROSS_CORRELATION)
			my d_pitch = Sound_to_Pitch_filteredCc (sound.get(), pitchTimeStep,
				my instancePref_pitch_filteredCC_floor(), my instancePref_pitch_filteredCC_top(),
				my instancePref_pitch_filteredCC_maximumNumberOfCandidates(), my instancePref_pitch_filteredCC_veryAccurate(),
				my instancePref_pitch_filteredCC_attenuationAtTop(),
				my instancePref_pitch_filteredCC_silenceThreshold(), my instancePref_pitch_filteredCC_voicingThreshold(),
				my instancePref_pitch_filteredCC_octaveCost(), my instancePref_pitch_filteredCC_octaveJumpCost(),
				my instancePref_pitch_filteredCC_voicedUnvoicedCost()
			);
		else
			Melder_crash (U"Unknown pitch method ", (int) my instancePref_pitch_method(), U".");
		my d_pitch -> xmin = my startWindow();
		my d_pitch -> xmax = my endWindow();
	} catch (MelderError) {
		my d_pitch. reset();   // signal a failure
		Melder_clearError ();
	}
}
static void tryToComputeIntensity (SoundAnalysisArea me) {
	autoMelderProgressOff progress;
	const double margin = 3.2 / my dynamic_instancePref_pitch_floor();
	try {
		autoSound sound = extractSoundOrNull (me, my startWindow() - margin, my endWindow() + margin);
		if (! sound)
			return;
		my d_intensity = Sound_to_Intensity (sound.get(), my dynamic_instancePref_pitch_floor(),
			my endWindow() - my startWindow() > my instancePref_longestAnalysis() ? (my endWindow() - my startWindow()) / 100 : 0.0,
			my instancePref_intensity_subtractMeanPressure()
		);
		my d_intensity -> xmin = my startWindow();
		my d_intensity -> xmax = my endWindow();
	} catch (MelderError) {
		my d_intensity. reset();   // signal a failure
		Melder_clearError ();
	}
}
static void tryToComputeFormants (SoundAnalysisArea me) {
	autoMelderProgressOff progress;
	const double margin = my instancePref_formant_windowLength();
	try {
		autoSound sound = ( my endWindow() - my startWindow() > my instancePref_longestAnalysis() ?
			extractSoundOrNull (me,
				0.5 * (my startWindow() + my endWindow() - my instancePref_longestAnalysis()) - margin,
				0.5 * (my startWindow() + my endWindow() + my instancePref_longestAnalysis()) + margin
			) :
			extractSoundOrNull (me, my startWindow() - margin, my endWindow() + margin)
		);
		if (! sound)
			return;
		const double formantTimeStep = (
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow() - my startWindow()) / my instancePref_numberOfTimeStepsPerView() :
			0.0   // the default: determined by analysis window length
		);
		my d_formant = Sound_to_Formant_any (sound.get(), formantTimeStep,
			Melder_iround (my instancePref_formant_numberOfFormants() * 2.0), my instancePref_formant_ceiling(),
			my instancePref_formant_windowLength(), (int) my instancePref_formant_method(), my instancePref_formant_preemphasisFrom(), 50.0
		);
		my d_formant -> xmin = my startWindow();
		my d_formant -> xmax = my endWindow();
	} catch (MelderError) {
		my d_formant. reset();   // signal a failure
		Melder_clearError ();
	}
}
static void tryToComputePulses (SoundAnalysisArea me) {
	if (! my d_pitch)
		tryToComputePitch (me);
	if (my d_pitch) {
		autoMelderProgressOff progress;
		try {
			autoSound sound = extractSoundOrNull (me, my startWindow(), my endWindow());
			if (! sound)
				return;
			my d_pulses = Sound_Pitch_to_PointProcess_cc (sound.get(), my d_pitch.get());
		} catch (MelderError) {
			my d_pulses. reset();   // signal a failure
			Melder_clearError ();
		}
	}
}

/*
	Some tryToHave<Analysis>() functions.
	The "try" again means that any exceptions that are generated, will be ignored;
	failure is again signalled by the Analysis being null.
	The "have" means that these functions do nothing if the Analysis already exists,
	but will (try to) create an Analysis if it does not exist.
*/
static void tryToHaveSpectrogram (SoundAnalysisArea me) {
	if (! my d_spectrogram && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		tryToComputeSpectrogram (me);
}
static void tryToHavePitch (SoundAnalysisArea me) {
	if (! my d_pitch && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		tryToComputePitch (me);
}
static void tryToHaveIntensity (SoundAnalysisArea me) {
	if (! my d_intensity && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		tryToComputeIntensity (me);
}
static void tryToHaveFormants (SoundAnalysisArea me) {
	if (! my d_formant && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		tryToComputeFormants (me);
}
static void tryToHavePulses (SoundAnalysisArea me) {
	if (! my d_pulses && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		tryToComputePulses (me);
}

/*
	Some SoundAnalysisArea_haveVisible<Analysis>() functions.
	These add error reporting that is specific to having editor windows,
	thereby replacing the ignored errors that would have been generated
	if the analysis had been performed by an explicit user action.

	These can be used for querying, extracting, and drawing to the Picture window,
	because those require the user to be aware of the need to have a visible Analysis.
	They cannot be used when drawing into the analysis part of the editor window,
	because of the low degree of explicitness of the need to draw an Analysis there.
*/
void SoundAnalysisArea_haveVisibleSpectrogram (SoundAnalysisArea me) {
	if (! my instancePref_spectrogram_show())
		Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrogram menu.");
	tryToHaveSpectrogram (me);
	if (! my d_spectrogram)
		Melder_throw (U"The spectrogram is not defined at the edge of the sound.");
}
void SoundAnalysisArea_haveVisiblePitch (SoundAnalysisArea me) {
	if (! my instancePref_pitch_show())
		Melder_throw (U"No pitch contour is visible.\nFirst choose \"Show pitch\" from the Pitch menu.");
	tryToHavePitch (me);
	if (! my d_pitch)
		Melder_throw (U"The pitch contour is not defined at the edge of the sound.");
}
void SoundAnalysisArea_haveVisibleIntensity (SoundAnalysisArea me) {
	if (! my instancePref_intensity_show())
		Melder_throw (U"No intensity contour is visible.\nFirst choose \"Show intensity\" from the Intensity menu.");
	tryToHaveIntensity (me);
	if (! my d_intensity)
		Melder_throw (U"The intensity curve is not defined at the edge of the sound.");
}
void SoundAnalysisArea_haveVisibleFormants (SoundAnalysisArea me) {
	if (! my instancePref_formant_show())
		Melder_throw (U"No formant contour is visible.\nFirst choose \"Show formants\" from the Formants menu.");
	tryToHaveFormants (me);
	if (! my d_formant)
		Melder_throw (U"The formants are not defined at the edge of the sound.");
}
void SoundAnalysisArea_haveVisiblePulses (SoundAnalysisArea me) {
	if (! my instancePref_pulses_show())
		Melder_throw (U"No pulses are visible.\nFirst choose \"Show pulses\" from the Pulses menu.");
	tryToHavePulses (me);
	if (! my d_pulses)
		Melder_throw (U"The pulses are not defined at the edge of the sound.");
}


#pragma mark - SoundAnalysisArea tracking

bool structSoundAnalysisArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	#if 0
	BUG: NYI
	if (event -> isClick()) {
		if (our instancePref_pitch_show()) {
			if (x_world >= our endWindow() && localY_fraction > 0.96 && localY_fraction <= 1.00) {
				our setInstancePref_pitch_ceiling (our instancePref_pitch_ceiling() * 1.26);
				our d_pitch. reset();
				our d_intensity. reset();
				our d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
			if (x_world >= our endWindow() && localY_fraction > 0.92 && localY_fraction <= 0.96) {
				our setInstancePref_pitch_ceiling (our instancePref_pitch_ceiling() / 1.26);
				our d_pitch. reset();
				our d_intensity. reset();
				our d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
		}
	}
	#endif
	if (x_world > our startWindow() && x_world < our endWindow())
		our d_spectrogram_cursor = localY_fraction * our instancePref_spectrogram_viewTo()
				+ (1.0 - localY_fraction) * our instancePref_spectrogram_viewFrom();
	return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world);
}

bool SoundAnalysisArea_mouse (SoundAnalysisArea me, GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction) {
	const double localY_fraction = my y_fraction_globalToLocal (globalY_fraction);
	return my v_mouse (event, x_world, localY_fraction);
}

#pragma mark - SoundAnalysisArea info

void structSoundAnalysisArea :: v_spectrogramInfo () const {
	/* Spectrogram flag: */
	MelderInfo_writeLine (U"Spectrogram show: ", our instancePref_spectrogram_show());
	/* Spectrogram settings: */
	MelderInfo_writeLine (U"Spectrogram view from: ", our instancePref_spectrogram_viewFrom(), U" Hz");
	MelderInfo_writeLine (U"Spectrogram view to: ", our instancePref_spectrogram_viewTo(), U" Hz");
	MelderInfo_writeLine (U"Spectrogram window length: ", our instancePref_spectrogram_windowLength(), U" seconds");
	MelderInfo_writeLine (U"Spectrogram dynamic range: ", our instancePref_spectrogram_dynamicRange(), U" dB");
	/* Advanced spectrogram settings: */
	MelderInfo_writeLine (U"Spectrogram number of time steps: ", our instancePref_spectrogram_timeSteps());
	MelderInfo_writeLine (U"Spectrogram number of frequency steps: ", our instancePref_spectrogram_frequencySteps());
	MelderInfo_writeLine (U"Spectrogram method: ", U"Fourier");
	MelderInfo_writeLine (U"Spectrogram window shape: ", kSound_to_Spectrogram_windowShape_getText (our instancePref_spectrogram_windowShape()));
	MelderInfo_writeLine (U"Spectrogram autoscaling: ", our instancePref_spectrogram_autoscaling());
	MelderInfo_writeLine (U"Spectrogram maximum: ", our instancePref_spectrogram_maximum(), U" dB/Hz");
	MelderInfo_writeLine (U"Spectrogram pre-emphasis: ", our instancePref_spectrogram_preemphasis(), U" dB/octave");
	MelderInfo_writeLine (U"Spectrogram dynamicCompression: ", our instancePref_spectrogram_dynamicCompression());
	/* Dynamic information: */
	MelderInfo_writeLine (U"Spectrogram cursor frequency: ", our d_spectrogram_cursor, U" Hz");
}
void structSoundAnalysisArea :: v_pitchInfo () const {
	/*
		Pitch flag:
	*/
	MelderInfo_writeLine (U"Pitch show: ", our instancePref_pitch_show());
	/*
		General pitch settings:
	*/
	MelderInfo_writeLine (U"Pitch method: ", kSoundAnalysisArea_pitch_analysisMethod_getText (our instancePref_pitch_method()));
	/*
		Pitch settings for filtered autocorrelation:
	*/
	conststring32 unitText = Function_getUnitText (Thing_dummyObject (Pitch),
			Pitch_LEVEL_FREQUENCY, (int) our instancePref_pitch_filteredAC_unit(), Function_UNIT_TEXT_MENU);
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) floor: ",
			our instancePref_pitch_filteredAC_floor(), U" Hz");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) top: ",
			our instancePref_pitch_filteredAC_top(), U" Hz");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) unit: ",
			unitText);
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) drawing method: ",
			kSoundAnalysisArea_pitch_drawingMethod_getText (our instancePref_pitch_filteredAC_drawingMethod()));
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) view from: ",
			our instancePref_pitch_filteredAC_viewFrom(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) view to: ",
			our instancePref_pitch_filteredAC_viewTo(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) max. number of candidates: ",
			our instancePref_pitch_filteredAC_maximumNumberOfCandidates());
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) very accurate: ",
			our instancePref_pitch_filteredAC_veryAccurate());
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) attenuation at top: ",
			our instancePref_pitch_filteredAC_attenuationAtTop());
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) silence threshold: ",
			our instancePref_pitch_filteredAC_silenceThreshold(), U" of global peak");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) voicing threshold: ",
			our instancePref_pitch_filteredAC_voicingThreshold(), U" (periodic power / total power)");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) octave cost: ",
			our instancePref_pitch_filteredAC_octaveCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) octave jump cost: ",
			our instancePref_pitch_filteredAC_octaveJumpCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (filtered autocorrelation) voiced/unvoiced cost: ",
			our instancePref_pitch_filteredAC_voicedUnvoicedCost());
	/*
		Pitch settings for raw cross-correlation:
	*/
	unitText = Function_getUnitText (Thing_dummyObject (Pitch),
			Pitch_LEVEL_FREQUENCY, (int) our instancePref_pitch_rawCC_unit(), Function_UNIT_TEXT_MENU);
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) floor: ",
			our instancePref_pitch_rawCC_floor(), U" Hz");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) ceiling: ",
			our instancePref_pitch_rawCC_ceiling(), U" Hz");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) unit: ",
			unitText);
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) drawing method: ",
			kSoundAnalysisArea_pitch_drawingMethod_getText (our instancePref_pitch_rawCC_drawingMethod()));
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) view from: ",
			our instancePref_pitch_rawCC_viewFrom(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) view to: ",
			our instancePref_pitch_rawCC_viewTo(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) max. number of candidates: ",
			our instancePref_pitch_rawCC_maximumNumberOfCandidates());
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) very accurate: ",
			our instancePref_pitch_rawCC_veryAccurate());
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) silence threshold: ",
			our instancePref_pitch_rawCC_silenceThreshold(), U" of global peak");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) voicing threshold: ",
			our instancePref_pitch_rawCC_voicingThreshold(), U" (periodic power / total power)");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) octave cost: ",
			our instancePref_pitch_rawCC_octaveCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) octave jump cost: ",
			our instancePref_pitch_rawCC_octaveJumpCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (raw cross-correlation) voiced/unvoiced cost: ",
			our instancePref_pitch_rawCC_voicedUnvoicedCost());
	/*
		Pitch settings for raw autocorrelation:
	*/
	unitText = Function_getUnitText (Thing_dummyObject (Pitch),
			Pitch_LEVEL_FREQUENCY, (int) our instancePref_pitch_rawAC_unit(), Function_UNIT_TEXT_MENU);
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) floor: ",
			our instancePref_pitch_rawAC_floor(), U" Hz");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) ceiling: ",
			our instancePref_pitch_rawAC_ceiling(), U" Hz");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) unit: ",
			unitText);
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) drawing method: ",
			kSoundAnalysisArea_pitch_drawingMethod_getText (our instancePref_pitch_rawAC_drawingMethod()));
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) view from: ",
			our instancePref_pitch_rawAC_viewFrom(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) view to: ",
			our instancePref_pitch_rawAC_viewTo(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) max. number of candidates: ",
			our instancePref_pitch_rawAC_maximumNumberOfCandidates());
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) very accurate: ",
			our instancePref_pitch_rawAC_veryAccurate());
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) silence threshold: ",
			our instancePref_pitch_rawAC_silenceThreshold(), U" of global peak");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) voicing threshold: ",
			our instancePref_pitch_rawAC_voicingThreshold(), U" (periodic power / total power)");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) octave cost: ",
			our instancePref_pitch_rawAC_octaveCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) octave jump cost: ",
			our instancePref_pitch_rawAC_octaveJumpCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (raw autocorrelation) voiced/unvoiced cost: ",
			our instancePref_pitch_rawAC_voicedUnvoicedCost());
	/*
		Pitch settings for filtered cross-correlation:
	*/
	unitText = Function_getUnitText (Thing_dummyObject (Pitch),
			Pitch_LEVEL_FREQUENCY, (int) our instancePref_pitch_filteredCC_unit(), Function_UNIT_TEXT_MENU);
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) floor: ",
			our instancePref_pitch_filteredCC_floor(), U" Hz");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) top: ",
			our instancePref_pitch_filteredCC_top(), U" Hz");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) unit: ",
			unitText);
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) drawing method: ",
			kSoundAnalysisArea_pitch_drawingMethod_getText (our instancePref_pitch_filteredCC_drawingMethod()));
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) view from: ",
			our instancePref_pitch_filteredCC_viewFrom(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) view to: ",
			our instancePref_pitch_filteredCC_viewTo(), U" ", unitText);
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) max. number of candidates: ",
			our instancePref_pitch_filteredCC_maximumNumberOfCandidates());
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) very accurate: ",
			our instancePref_pitch_filteredCC_veryAccurate());
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) attenuation at top: ",
			our instancePref_pitch_filteredCC_attenuationAtTop());
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) silence threshold: ",
			our instancePref_pitch_filteredCC_silenceThreshold(), U" of global peak");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) voicing threshold: ",
			our instancePref_pitch_filteredCC_voicingThreshold(), U" (periodic power / total power)");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) octave cost: ",
			our instancePref_pitch_filteredCC_octaveCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) octave jump cost: ",
			our instancePref_pitch_filteredCC_octaveJumpCost(), U" per octave");
	MelderInfo_writeLine (U"Pitch (filtered cross-correlation) voiced/unvoiced cost: ",
			our instancePref_pitch_filteredCC_voicedUnvoicedCost());
}
void structSoundAnalysisArea :: v_intensityInfo () const {
	/* Intensity flag: */
	MelderInfo_writeLine (U"Intensity show: ", our instancePref_intensity_show());
	/* Intensity settings: */
	MelderInfo_writeLine (U"Intensity view from: ", our instancePref_intensity_viewFrom(), U" dB");
	MelderInfo_writeLine (U"Intensity view to: ", our instancePref_intensity_viewTo(), U" dB");
	MelderInfo_writeLine (U"Intensity averaging method: ", kSoundAnalysisArea_intensity_averagingMethod_getText (our instancePref_intensity_averagingMethod()));
	MelderInfo_writeLine (U"Intensity subtract mean pressure: ", our instancePref_intensity_subtractMeanPressure());
}
void structSoundAnalysisArea :: v_formantsInfo () const {
	/* Formants flag: */
	MelderInfo_writeLine (U"Formant show: ", our instancePref_formant_show());
	/* Formant settings: */
	MelderInfo_writeLine (U"Formant ceiling: ", our instancePref_formant_ceiling(), U" Hz");
	MelderInfo_writeLine (U"Formant number of poles: ", Melder_iround (2.0 * our instancePref_formant_numberOfFormants()));   // should be a whole number
	MelderInfo_writeLine (U"Formant window length: ", our instancePref_formant_windowLength(), U" seconds");
	MelderInfo_writeLine (U"Formant dynamic range: ", our instancePref_formant_dynamicRange(), U" dB");
	MelderInfo_writeLine (U"Formant dot size: ", our instancePref_formant_dotSize(), U" mm");
	/* Advanced formant settings: */
	MelderInfo_writeLine (U"Formant method: ", kSoundAnalysisArea_formant_analysisMethod_getText (our instancePref_formant_method()));
	MelderInfo_writeLine (U"Formant pre-emphasis from: ", our instancePref_formant_preemphasisFrom(), U" Hz");
}
void structSoundAnalysisArea :: v_pulsesInfo () const {
	/* Pulses flag: */
	MelderInfo_writeLine (U"Pulses show: ", our instancePref_pulses_show());
	MelderInfo_writeLine (U"Pulses maximum period factor: ", our instancePref_pulses_maximumPeriodFactor());
	MelderInfo_writeLine (U"Pulses maximum amplitude factor: ", our instancePref_pulses_maximumAmplitudeFactor());
}
void structSoundAnalysisArea :: v1_info () {
	MelderInfo_writeLine (U"Longest analysis: ", our instancePref_longestAnalysis(), U" seconds");
	if (our v_hasSpectrogram ())
		our v_spectrogramInfo ();
	if (v_hasPitch ())
		our v_pitchInfo ();
	if (v_hasIntensity ())
		our v_intensityInfo ();
	if (v_hasFormants ())
		our v_formantsInfo ();
	if (v_hasPulses ())
		our v_pulsesInfo ();
	const conststring32 logTargetTexts [3] = { U"log file only", U"Info window only", U"log file and Info window" };
	const conststring32 log1TargetText = logTargetTexts [our instancePref_log1_toLogFile() + 2 * our instancePref_log1_toInfoWindow() - 1];
	MelderInfo_writeLine (U"Log 1 target: ", log1TargetText);
	MelderInfo_writeLine (U"Log 1 file: ", our instancePref_log1_fileName());
	MelderInfo_writeLine (U"Log 1 format: ", our instancePref_log1_format());
	const conststring32 log2TargetText = logTargetTexts [our instancePref_log2_toLogFile() + 2 * our instancePref_log2_toInfoWindow() - 1];
	MelderInfo_writeLine (U"Log 2 target: ", log2TargetText);
	MelderInfo_writeLine (U"Log 2 file: ", our instancePref_log2_fileName());
	MelderInfo_writeLine (U"Log 2 format: ", our instancePref_log2_format());
	MelderInfo_writeLine (U"Log 3 script: ", our instancePref_logScript3());
	MelderInfo_writeLine (U"Log 4 script: ", our instancePref_logScript4());
	/*
		COMPATIBILITY < 6400
	*/
	if (v_hasPitch ()) {
		kSoundAnalysisArea_pitch_analysisMethod pitchMethod = our instancePref_pitch_method();
		if (pitchMethod == kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION) {
			MelderInfo_writeLine (U"\nCOMPATIBILITY (obsolete settings for the benefit of old scripts):");
			MelderInfo_writeLine (U"Pitch max. number of candidates: ", our instancePref_pitch_rawAC_maximumNumberOfCandidates());
			MelderInfo_writeLine (U"Pitch very accurate: ", our instancePref_pitch_rawAC_veryAccurate());
			MelderInfo_writeLine (U"Pitch silence threshold: ", our instancePref_pitch_rawAC_silenceThreshold(), U" of global peak");
			MelderInfo_writeLine (U"Pitch voicing threshold: ", our instancePref_pitch_rawAC_voicingThreshold(), U" (periodic power / total power)");
			MelderInfo_writeLine (U"Pitch octave cost: ", our instancePref_pitch_rawAC_octaveCost(), U" per octave");
			MelderInfo_writeLine (U"Pitch octave jump cost: ", our instancePref_pitch_rawAC_octaveJumpCost(), U" per octave");
			MelderInfo_writeLine (U"Pitch voiced/unvoiced cost: ", our instancePref_pitch_rawAC_voicedUnvoicedCost());
		} else if (pitchMethod == kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION) {
			MelderInfo_writeLine (U"\nCOMPATIBILITY (obsolete settings for the benefit of old scripts):");
			MelderInfo_writeLine (U"Pitch max. number of candidates: ", our instancePref_pitch_rawCC_maximumNumberOfCandidates());
			MelderInfo_writeLine (U"Pitch very accurate: ", our instancePref_pitch_rawCC_veryAccurate());
			MelderInfo_writeLine (U"Pitch silence threshold: ", our instancePref_pitch_rawCC_silenceThreshold(), U" of global peak");
			MelderInfo_writeLine (U"Pitch voicing threshold: ", our instancePref_pitch_rawCC_voicingThreshold(), U" (periodic power / total power)");
			MelderInfo_writeLine (U"Pitch octave cost: ", our instancePref_pitch_rawCC_octaveCost(), U" per octave");
			MelderInfo_writeLine (U"Pitch octave jump cost: ", our instancePref_pitch_rawCC_octaveJumpCost(), U" per octave");
			MelderInfo_writeLine (U"Pitch voiced/unvoiced cost: ", our instancePref_pitch_rawCC_voicedUnvoicedCost());
		}
	}
}


#pragma mark - SoundAnalysisArea Analysis settings menu

static void menu_cb_showAnalyses (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Show analyses", nullptr)
		BOOLEAN  (showSpectrogram, U"Show spectrogram",     my default_spectrogram_show ())
		BOOLEAN  (showPitch,       U"Show pitch",           my default_pitch_show       ())
		BOOLEAN  (showIntensity,   U"Show intensity",       my default_intensity_show   ())
		BOOLEAN  (showFormants,    U"Show formants",        my default_formant_show     ())
		BOOLEAN  (showPulses,      U"Show pulses",          my default_pulses_show      ())
		POSITIVE (longestAnalysis, U"Longest analysis (s)", my default_longestAnalysis  ())
	EDITOR_OK
		SET_BOOLEAN (showSpectrogram, my instancePref_spectrogram_show())
		SET_BOOLEAN (showPitch,       my instancePref_pitch_show())
		SET_BOOLEAN (showIntensity,   my instancePref_intensity_show())
		SET_BOOLEAN (showFormants,    my instancePref_formant_show())
		SET_BOOLEAN (showPulses,      my instancePref_pulses_show())
		SET_REAL    (longestAnalysis, my instancePref_longestAnalysis())
	EDITOR_DO
		my setInstancePref_spectrogram_show (showSpectrogram);
		my setInstancePref_pitch_show       (showPitch);
		my setInstancePref_intensity_show   (showIntensity);
		my setInstancePref_formant_show     (showFormants);
		my setInstancePref_pulses_show      (showPulses);
		GuiMenuItem_check (my spectrogramToggle, showSpectrogram);
		GuiMenuItem_check (my pitchToggle,       showPitch);
		GuiMenuItem_check (my intensityToggle,   showIntensity);
		GuiMenuItem_check (my formantToggle,     showFormants);
		GuiMenuItem_check (my pulsesToggle,      showPulses);
		my setInstancePref_longestAnalysis (longestAnalysis);
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_timeStepSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Time step settings", U"Time step settings...")
		OPTIONMENU_ENUM (kSoundAnalysisArea_timeStepStrategy, timeStepStrategy,
				U"Time step strategy", my default_timeStepStrategy ())
		COMMENT (U"")
		COMMENT (U"If the time step strategy is \"fixed\":")
		POSITIVE (fixedTimeStep, U"Fixed time step (s)", my default_fixedTimeStep ())
		COMMENT (U"")
		COMMENT (U"If the time step strategy is \"view-dependent\":")
		NATURAL (numberOfTimeStepsPerView, U"Number of time steps per view", my default_numberOfTimeStepsPerView ())
	EDITOR_OK
		SET_ENUM (timeStepStrategy, kSoundAnalysisArea_timeStepStrategy, my instancePref_timeStepStrategy())
		SET_REAL (fixedTimeStep, my instancePref_fixedTimeStep())
		SET_INTEGER (numberOfTimeStepsPerView, my instancePref_numberOfTimeStepsPerView())
	EDITOR_DO
		my setInstancePref_timeStepStrategy (timeStepStrategy);
		my setInstancePref_fixedTimeStep (fixedTimeStep);
		my setInstancePref_numberOfTimeStepsPerView (numberOfTimeStepsPerView);
		my d_pitch. reset();
		my d_formant. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}


#pragma mark - SoundAnalysisArea Analyses settings menu

enum {
	SoundAnalysisArea_PART_CURSOR = 1,
	SoundAnalysisArea_PART_SELECTION = 2
};

static const conststring32 SoundAnalysisArea_partString (int part) {
	static const conststring32 strings [] = { U"", U"CURSOR", U"SELECTION" };
	return strings [part];
}

static const conststring32 SoundAnalysisArea_partString_locative (int part) {
	static const conststring32 strings [] = { U"", U"at CURSOR", U"in SELECTION" };
	return strings [part];
}

static int makeQueriable (SoundAnalysisArea me, bool allowCursor, double *tmin, double *tmax) {
	if (my endWindow() - my startWindow() > my instancePref_longestAnalysis())
		Melder_throw (U"Window too long to show analyses. "
			U"Zoom in to at most ", Melder_half (my instancePref_longestAnalysis()), U" seconds "
			U"or set the \"longest analysis\" to at least ", Melder_half (my endWindow() - my startWindow()),
			U" seconds (with \"Show analyses\" in the Analysis menu)."
		);
	if (my startSelection() == my endSelection()) {
		if (allowCursor) {
			*tmin = *tmax = my startSelection();
			return SoundAnalysisArea_PART_CURSOR;
		} else {
			Melder_throw (U"Make a selection first.");
		}
	} else if (my startSelection() < my startWindow() || my endSelection() > my endWindow()) {
		Melder_throw (U"Command ambiguous: a part of the selection (", my startSelection(), U", ", my endSelection(), U") "
			U"is outside of the window (", my startWindow(), U", ", my endWindow(), U"). "
			U"Either zoom or re-select.");
	}
	*tmin = my startSelection();
	*tmax = my endSelection();
	return SoundAnalysisArea_PART_SELECTION;
}

static void menu_cb_logSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Log settings", U"Log files")
		OPTIONMENU (writeLog1To, U"Write log 1 to", 3)
			OPTION (U"log file only")
			OPTION (U"Info window only")
			OPTION (U"log file and Info window")
		OUTFILE (logFile1,   U"Log file 1:",   my default_log1_fileName ())
		TEXTFIELD (log1format, U"Log 1 format:", my default_log1_format (), 3)
		OPTIONMENU (writeLog2To, U"Write log 2 to", 3)
			OPTION (U"log file only")
			OPTION (U"Info window only")
			OPTION (U"log file and Info window")
		OUTFILE (logFile2,   U"Log file 2:",   my default_log2_fileName ())
		TEXTFIELD (log2format, U"Log 2 format:", my default_log2_format (), 3)
		OUTFILE (logScript3, U"Log script 3:", my default_logScript3    ())
		OUTFILE (logScript4, U"Log script 4:", my default_logScript4    ())
	EDITOR_OK
		SET_OPTION (writeLog1To,   my instancePref_log1_toLogFile() + 2 * my instancePref_log1_toInfoWindow())
		SET_STRING (logFile1,      my instancePref_log1_fileName())
		SET_STRING (log1format,    my instancePref_log1_format())
		SET_OPTION (writeLog2To,   my instancePref_log2_toLogFile() + 2 * my instancePref_log2_toInfoWindow())
		SET_STRING (logFile2,      my instancePref_log2_fileName())
		SET_STRING (log2format,    my instancePref_log2_format())
		SET_STRING (logScript3,    my instancePref_logScript3())
		SET_STRING (logScript4,    my instancePref_logScript4())
	EDITOR_DO
		my setInstancePref_log1_toLogFile    ((writeLog1To & 1) != 0);
		my setInstancePref_log1_toInfoWindow ((writeLog1To & 2) != 0);
		my setInstancePref_log1_fileName     (logFile1);
		my setInstancePref_log1_format       (log1format);
		my setInstancePref_log2_toLogFile    ((writeLog2To & 1) != 0);
		my setInstancePref_log2_toInfoWindow ((writeLog2To & 2) != 0);
		my setInstancePref_log2_fileName     (logFile2);
		my setInstancePref_log2_format       (log2format);
		my setInstancePref_logScript3        (logScript3);
		my setInstancePref_logScript4        (logScript4);
	EDITOR_END
}

static void menu_cb_deleteLogFile1 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		structMelderFile file { };
		Melder_pathToFile (my instancePref_log1_fileName(), & file);
		MelderFile_delete (& file);
	VOID_EDITOR_END
}

static void menu_cb_deleteLogFile2 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		structMelderFile file { };
		Melder_pathToFile (my instancePref_log2_fileName(), & file);
		MelderFile_delete (& file);
	VOID_EDITOR_END
}

static void do_log (SoundAnalysisArea me, int which) {
	PrefsString format;
	double tmin, tmax;
	const int part = makeQueriable (me, true, & tmin, & tmax);
	str32cpy (format, which == 1 ? my instancePref_log1_format() : my instancePref_log2_format());
	for (char32 *p = format; *p != U'\0'; p ++) if (*p == U'\'') {
		/*
			Found a left quote. Search for a matching right quote.
		*/
		char32 *q = p + 1, varName [300], *r, *s, *colon;
		integer precision = -1;
		double value = undefined;
		conststring32 stringValue = nullptr;
		while (*q != U'\0' && *q != U'\'')
			q ++;
		if (*q == U'\0')
			break;   // no matching right quote: done with this line
		if (q - p == 1)
			continue;   // ignore empty variable names
		/*
			Found a right quote. Get potential variable name.
		*/
		for (r = p + 1, s = varName; q - r > 0; r ++, s ++)
			*s = *r;
		*s = U'\0';   // trailing null byte
		colon = str32chr (varName, U':');
		if (colon) {
			precision = Melder_atoi (colon + 1);
			*colon = U'\0';
		}
		if (str32equ (varName, U"time")) {
			value = 0.5 * (tmin + tmax);
		} else if (str32equ (varName, U"t1")) {
			value = tmin;
		} else if (str32equ (varName, U"t2")) {
			value = tmax;
		} else if (str32equ (varName, U"dur")) {
			value = tmax - tmin;
		} else if (str32equ (varName, U"freq")) {
			value = my d_spectrogram_cursor;
		} else if (str32equ (varName, U"tab$")) {
			stringValue = U"\t";
		} else if (str32equ (varName, U"editor$")) {
			stringValue = my boss() -> name.get();
		} else if (str32equ (varName, U"f0")) {
			SoundAnalysisArea_haveVisiblePitch (me);
			const kPitch_unit unit = my dynamic_instancePref_pitch_unit();
			if (part == SoundAnalysisArea_PART_CURSOR)
				value = Pitch_getValueAtTime (my d_pitch.get(), tmin, unit, 1);
			else
				value = Pitch_getMean (my d_pitch.get(), tmin, tmax, unit);
			value = Function_convertToNonlogarithmic (my d_pitch.get(), value, Pitch_LEVEL_FREQUENCY, (int) unit);
		} else if (varName [0] == U'f' && varName [1] >= U'1' && varName [1] <= U'5' && varName [2] == U'\0') {
			SoundAnalysisArea_haveVisibleFormants (me);
			if (part == SoundAnalysisArea_PART_CURSOR)
				value = Formant_getValueAtTime (my d_formant.get(), (int) (varName [1] - U'0'), tmin, kFormant_unit::HERTZ);
			else
				value = Formant_getMean (my d_formant.get(), (int) (varName [1] - U'0'), tmin, tmax, kFormant_unit::HERTZ);
		} else if (varName [0] == U'b' && varName [1] >= U'1' && varName [1] <= U'5' && varName [2] == U'\0') {
			SoundAnalysisArea_haveVisibleFormants (me);
			value = Formant_getBandwidthAtTime (my d_formant.get(), (int) (varName [1] - U'0'), 0.5 * (tmin + tmax), kFormant_unit::HERTZ);
		} else if (str32equ (varName, U"intensity")) {
			SoundAnalysisArea_haveVisibleIntensity (me);
			if (part == SoundAnalysisArea_PART_CURSOR)
				value = Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
			else
				value = Intensity_getAverage (my d_intensity.get(), tmin, tmax, (int) my instancePref_intensity_averagingMethod());
		} else if (str32equ (varName, U"power")) {
			SoundAnalysisArea_haveVisibleSpectrogram (me);
			if (part != SoundAnalysisArea_PART_CURSOR)
				Melder_throw (U"Click inside the spectrogram first.");
			value = Matrix_getValueAtXY (my d_spectrogram.get(), tmin, my d_spectrogram_cursor);
		}
		if (isdefined (value)) {
			const integer varlen = (q - p) - 1, headlen = p - format;
			char32 formattedNumber [400];
			if (precision >= 0)
				Melder_sprint (formattedNumber,400, Melder_fixed (value, precision));
			else
				Melder_sprint (formattedNumber,400, value);
			const integer arglen = Melder_length (formattedNumber);
			static MelderString buffer;
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append (& buffer, formattedNumber, p + varlen + 2);
			str32cpy (format, buffer.string);
			p += arglen - 1;
		} else if (stringValue) {
			const integer varlen = (q - p) - 1, headlen = p - format, arglen = Melder_length (stringValue);
			static MelderString buffer;
			MelderString_ncopy (& buffer, format, headlen);
			MelderString_append (& buffer, stringValue, p + varlen + 2);
			str32cpy (format, buffer.string);
			p += arglen - 1;
		} else {
			p = q - 1;   // go to before next quote
		}
	}
	if (which == 1 && my instancePref_log1_toInfoWindow()  ||  which == 2 && my instancePref_log2_toInfoWindow()) {
		MelderInfo_write (format);
		MelderInfo_close ();
	}
	if (which == 1 && my instancePref_log1_toLogFile()  ||  which == 2 && my instancePref_log2_toLogFile()) {
		structMelderFile file { };
		str32cat (format, U"\n");
		Melder_relativePathToFile (which == 1 ? my instancePref_log1_fileName() : my instancePref_log2_fileName(), & file);
		MelderFile_appendText (& file, format);
	}
}

static void menu_cb_log1 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		do_log (me, 1);
	VOID_EDITOR_END
}
static void menu_cb_log2 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		do_log (me, 2);
	VOID_EDITOR_END
}
static void menu_cb_logScript3 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		praat_executeScriptFromEditorCommand (my functionEditor(), cmd, my instancePref_logScript3());
	VOID_EDITOR_END
}
static void menu_cb_logScript4 (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		praat_executeScriptFromEditorCommand (my functionEditor(), cmd, my instancePref_logScript4());
	VOID_EDITOR_END
}


#pragma mark - SoundAnalysisArea Spectrogram menu

static void menu_cb_showSpectrogram (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		my setInstancePref_spectrogram_show (! my instancePref_spectrogram_show());   // toggle
		GuiMenuItem_check (my spectrogramToggle, my instancePref_spectrogram_show());   // in case we're called from a script
		FunctionEditor_redraw (my functionEditor());
	VOID_EDITOR_END
}

static void menu_cb_spectrogramSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Spectrogram settings", U"Intro 3.2. Configuring the spectrogram")
		REAL     (viewFrom,     U"left View range (Hz)",  my default_spectrogram_viewFrom())
		POSITIVE (viewTo,       U"right View range (Hz)", my default_spectrogram_viewTo())
		POSITIVE (windowLength, U"Window length (s)",     my default_spectrogram_windowLength())
		POSITIVE (dynamicRange, U"Dynamic range (dB)",    my default_spectrogram_dynamicRange())
		MUTABLE_COMMENT (note1, U"")
		MUTABLE_COMMENT (note2, U"")
	EDITOR_OK
		SET_REAL (viewFrom,     my instancePref_spectrogram_viewFrom())
		SET_REAL (viewTo,       my instancePref_spectrogram_viewTo())
		SET_REAL (windowLength, my instancePref_spectrogram_windowLength())
		SET_REAL (dynamicRange, my instancePref_spectrogram_dynamicRange())
		if (my instancePref_spectrogram_timeSteps()          != Melder_atof (my default_spectrogram_timeSteps()) ||
			my instancePref_spectrogram_frequencySteps()     != Melder_atof (my default_spectrogram_frequencySteps()) ||
			my instancePref_spectrogram_method()             != my default_spectrogram_method() ||
			my instancePref_spectrogram_windowShape()        != my default_spectrogram_windowShape() ||
			my instancePref_spectrogram_maximum()            != Melder_atof (my default_spectrogram_maximum()) ||
			my instancePref_spectrogram_autoscaling()        != my default_spectrogram_autoscaling()||
			my instancePref_spectrogram_preemphasis()        != Melder_atof (my default_spectrogram_preemphasis()) ||
			my instancePref_spectrogram_dynamicCompression() != Melder_atof (my default_spectrogram_dynamicCompression()))
		{
			SET_STRING (note1, U"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (note1, U"(all of your \"advanced settings\" have their standard values)")
		}
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		Melder_require (viewFrom < viewTo,
			U"The ceiling of the spectrogram view range should be greater than the floor.");
		my setInstancePref_spectrogram_viewFrom (viewFrom);
		my setInstancePref_spectrogram_viewTo (viewTo);
		my setInstancePref_spectrogram_windowLength (windowLength);
		my setInstancePref_spectrogram_dynamicRange (dynamicRange);
		my d_spectrogram. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_advancedSpectrogramSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced spectrogram settings", U"Advanced spectrogram settings...")
		COMMENT (U"Time and frequency resolutions:")
		NATURAL (numberOfTimeSteps,      U"Number of time steps",      my default_spectrogram_timeSteps())
		NATURAL (numberOfFrequencySteps, U"Number of frequency steps", my default_spectrogram_frequencySteps())
		COMMENT (U"Spectrogram analysis settings:")
		OPTIONMENU_ENUM (kSound_to_Spectrogram_method, method,
				U"Method", my default_spectrogram_method ())
		OPTIONMENU_ENUM (kSound_to_Spectrogram_windowShape, windowShape,
				U"Window shape", my default_spectrogram_windowShape ())
		COMMENT (U"Spectrogram view settings:")
		BOOLEAN (autoscaling, U"Autoscaling", my default_spectrogram_autoscaling())
		REAL (maximum, U"Maximum (dB/Hz)", my default_spectrogram_maximum())
		REAL (preemphasis, U"Pre-emphasis (dB/oct)", my default_spectrogram_preemphasis())
		REAL (dynamicCompression, U"Dynamic compression (0-1)", my default_spectrogram_dynamicCompression())
	EDITOR_OK
		SET_INTEGER (numberOfTimeSteps,      my instancePref_spectrogram_timeSteps())
		SET_INTEGER (numberOfFrequencySteps, my instancePref_spectrogram_frequencySteps())
		SET_ENUM    (method,      kSound_to_Spectrogram_method,      my instancePref_spectrogram_method())
		SET_ENUM    (windowShape, kSound_to_Spectrogram_windowShape, my instancePref_spectrogram_windowShape())
		SET_BOOLEAN (autoscaling,              my instancePref_spectrogram_autoscaling())
		SET_REAL    (maximum,                  my instancePref_spectrogram_maximum())
		SET_REAL    (preemphasis,              my instancePref_spectrogram_preemphasis())
		SET_REAL    (dynamicCompression,       my instancePref_spectrogram_dynamicCompression())
	EDITOR_DO
		my setInstancePref_spectrogram_timeSteps (numberOfTimeSteps);
		my setInstancePref_spectrogram_frequencySteps (numberOfFrequencySteps);
		my setInstancePref_spectrogram_method (method);
		my setInstancePref_spectrogram_windowShape (windowShape);
		my setInstancePref_spectrogram_autoscaling (autoscaling);
		my setInstancePref_spectrogram_maximum (maximum);
		my setInstancePref_spectrogram_preemphasis (preemphasis);
		my setInstancePref_spectrogram_dynamicCompression (dynamicCompression);
		my d_spectrogram. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void QUERY_EDITOR_FOR_REAL__getFrequency (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_EDITOR_FOR_REAL
		const double result = my d_spectrogram_cursor;
	QUERY_EDITOR_FOR_REAL_END (U" Hz")
}

static void QUERY_DATA_FOR_REAL__getSpectralPowerAtCursorCross (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleSpectrogram (me);
		if (part != SoundAnalysisArea_PART_CURSOR)
			Melder_throw (U"Click inside the spectrogram first.");
		const double result = Matrix_getValueAtXY (my d_spectrogram.get(), tmin, my d_spectrogram_cursor);
	QUERY_DATA_FOR_REAL_END (U" Pa2/Hz (at time = ", tmin, U" seconds and frequency = ", my d_spectrogram_cursor, U" Hz)");
}

static void menu_cb_moveFrequencyCursorTo (SoundAnalysisArea me, EDITOR_ARGS) {
	if (! my instancePref_spectrogram_show())
		Melder_throw (U"No spectrogram is visible.\nFirst choose \"Show spectrogram\" from the Spectrogram menu.");
	EDITOR_FORM (U"Move frequency cursor to", nullptr)
		REAL (frequency, U"Frequency (Hz)", U"0.0")
	EDITOR_OK
		SET_REAL (frequency, my d_spectrogram_cursor)
	EDITOR_DO
		my d_spectrogram_cursor = frequency;
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleSpectrogram (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		SoundAnalysisArea_haveVisibleSpectrogram (me);
		autoSpectrogram result = Data_copy (my d_spectrogram.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ViewSpectralSlice (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		const double start = ( my startSelection() == my endSelection() ?
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::GAUSSIAN ? my startSelection() - my instancePref_spectrogram_windowLength() :
			my startSelection() - my instancePref_spectrogram_windowLength() / 2 : my startSelection()
		);
		const double finish = ( my startSelection() == my endSelection() ?
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::GAUSSIAN ? my endSelection() + my instancePref_spectrogram_windowLength() :
			my endSelection() + my instancePref_spectrogram_windowLength() / 2 : my endSelection()
		);
		autoSound sound = extractSoundOrNull (me, start, finish);
		if (! sound)
			return;
		Sound_multiplyByWindow (sound.get(),
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::SQUARE ? kSound_windowShape::RECTANGULAR :
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::HAMMING ? kSound_windowShape::HAMMING :
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::BARTLETT ? kSound_windowShape::TRIANGULAR :
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::WELCH ? kSound_windowShape::PARABOLIC :
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::HANNING ? kSound_windowShape::HANNING :
			my instancePref_spectrogram_windowShape() == kSound_to_Spectrogram_windowShape::GAUSSIAN ? kSound_windowShape::GAUSSIAN_2 : kSound_windowShape::RECTANGULAR
		);
		autoSpectrum result = Sound_to_Spectrum (sound.get(), true);
	CONVERT_DATA_TO_ONE_END (Melder_cat (( my data() ? my data() -> name.get() : U"untitled" ),
			U"_", Melder_fixed (0.5 * (my startSelection() + my endSelection()), 3)))
}

static void menu_cb_paintVisibleSpectrogram (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Paint visible spectrogram", nullptr)
		my v_form_pictureWindow (cmd);   // BUG: isn't really virtual
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_spectrogram_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_spectrogram_picture_garnish (garnish);
		SoundAnalysisArea_haveVisibleSpectrogram (me);
		DataGui_openPraatPicture (me);
		Spectrogram_paint (my d_spectrogram.get(), my pictureGraphics(), my startWindow(), my endWindow(),
			my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo(),
			my instancePref_spectrogram_maximum(), my instancePref_spectrogram_autoscaling(),
			my instancePref_spectrogram_dynamicRange(), my instancePref_spectrogram_preemphasis(),
			my instancePref_spectrogram_dynamicCompression(), garnish
		);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}


#pragma mark - SoundAnalysisArea Pitch menu

static void menu_cb_showPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		my setInstancePref_pitch_show (! my instancePref_pitch_show());   // toggle
		GuiMenuItem_check (my pitchToggle, my instancePref_pitch_show());   // in case we're called from a script
		FunctionEditor_redraw (my functionEditor());
	VOID_EDITOR_END
}

static void menu_cb_pitchSettings_BEFORE_6400 (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings", U"Intro 4.2. Configuring the pitch contour")
		COMMENT   (U"YOU SHOULD NEVER SEE THIS OBSOLETE SETTINGS WINDOW;")
		COMMENT   (U"IT IS HERE ONLY FOR COMPATIBILITY WITH OLD SCRIPTS.")
		POSITIVE (pitchFloor,   U"left Pitch range (Hz)",  my default_pitch_rawAC_floor())   // the default is ignored if not shown
		POSITIVE (pitchCeiling, U"right Pitch range (Hz)", my default_pitch_rawAC_ceiling())
		OPTIONMENU_ENUM (kPitch_unit, unit, U"Unit", my default_pitch_rawAC_unit ())
		CHOICE_ENUM (kSoundAnalysisArea_pitch_analysisMethod, analysisMethod, U"Analysis method", my default_pitch_method())
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod, U"Drawing method", my default_pitch_rawAC_drawingMethod())
	EDITOR_OK
	EDITOR_DO
		Melder_require (
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION ||
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION,
			U"Obsolete pitch settings command can handle only raw AC and CC methods."
			U"Use one of the modern pitch setting commands instead."
		);
		Melder_require (pitchCeiling > pitchFloor,
			U"The pitch ceiling has to be greater than the pitch floor, so they cannot be ",
			pitchCeiling, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		my setInstancePref_pitch_method (analysisMethod);
		my setInstancePref_pitch_rawAC_floor (pitchFloor);
		my setInstancePref_pitch_rawCC_floor (pitchFloor);
		my setInstancePref_pitch_rawAC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawCC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawAC_unit (unit);
		my setInstancePref_pitch_rawCC_unit (unit);
		my setInstancePref_pitch_rawAC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_rawCC_drawingMethod (drawingMethod);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_pitchSettings_BEFORE_6414 (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings", U"Intro 4.2. Configuring the pitch contour")
		COMMENT   (U"YOU SHOULD NEVER SEE THIS OBSOLETE SETTINGS WINDOW;")
		COMMENT   (U"IT IS HERE ONLY FOR COMPATIBILITY WITH OLD SCRIPTS.")
		POSITIVE (pitchFloor, U"left Pitch range (Hz)",  my default_pitch_rawAC_floor())   // the default is ignored if not shown
		POSITIVE (pitchCeiling, U"right Pitch range (Hz)", my default_pitch_rawAC_ceiling())
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit", my default_pitch_rawAC_unit ())
		CHOICE_ENUM (kSoundAnalysisArea_pitch_analysisMethod, analysisMethod,
				U"Analysis method", my default_pitch_method())
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod,
				U"Drawing method", my default_pitch_rawAC_drawingMethod())
		REAL    (viewFrom,                  U"left View range (units)",   my default_pitch_rawAC_viewFrom                  ())
		REAL    (viewTo,                    U"right View range (units)",  my default_pitch_rawAC_viewTo                    ())
	EDITOR_OK
	EDITOR_DO_ALTERNATIVE (menu_cb_pitchSettings_BEFORE_6400)
		Melder_require (
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_AUTOCORRELATION ||
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION ||
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION ||
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_CROSS_CORRELATION,
			U"Obsolete pitch settings command can handle only filtered or raw AC and CC methods."
			U"Use one of the modern pitch setting commands instead."
		);
		Melder_require (pitchCeiling > pitchFloor,
			U"The pitch ceiling has to be greater than the pitch floor, so they cannot be ",
			pitchCeiling, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		my setInstancePref_pitch_method (analysisMethod);
		my setInstancePref_pitch_filteredAC_floor (pitchFloor);
		my setInstancePref_pitch_filteredAC_top (pitchCeiling);
		my setInstancePref_pitch_filteredAC_unit (unit);
		my setInstancePref_pitch_filteredAC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_filteredAC_viewFrom (viewFrom);
		my setInstancePref_pitch_filteredAC_viewTo (viewTo);
		my setInstancePref_pitch_rawCC_floor (pitchFloor);
		my setInstancePref_pitch_rawCC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawCC_unit (unit);
		my setInstancePref_pitch_rawCC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_rawCC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawCC_viewTo (viewTo);
		my setInstancePref_pitch_rawAC_floor (pitchFloor);
		my setInstancePref_pitch_rawAC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawAC_unit (unit);
		my setInstancePref_pitch_rawAC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_rawAC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawAC_viewTo (viewTo);
		my setInstancePref_pitch_filteredCC_floor (pitchFloor);
		my setInstancePref_pitch_filteredCC_top (pitchCeiling);
		my setInstancePref_pitch_filteredCC_unit (unit);
		my setInstancePref_pitch_filteredCC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_filteredCC_viewFrom (viewFrom);
		my setInstancePref_pitch_filteredCC_viewTo (viewTo);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_advancedPitchSettings_BEFORE_6400 (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced pitch settings", U"Advanced pitch settings...")
		COMMENT   (U"YOU SHOULD NEVER SEE THIS OBSOLETE SETTINGS WINDOW;")
		COMMENT   (U"IT IS HERE ONLY FOR COMPATIBILITY WITH OLD SCRIPTS.")
		REAL    (viewFrom,                  U"left View range (units)",   my default_pitch_rawAC_viewFrom                  ())   // the default is ignored if not shown
		REAL    (viewTo,                    U"right View range (units)",  my default_pitch_rawAC_viewTo                    ())
		BOOLEAN (veryAccurate,              U"Very accurate",             false)
		NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_rawAC_maximumNumberOfCandidates ())
		REAL    (silenceThreshold,          U"Silence threshold",         my default_pitch_rawAC_silenceThreshold          ())
		REAL    (voicingThreshold,          U"Voicing threshold",         my default_pitch_rawAC_voicingThreshold          ())
		REAL    (octaveCost,                U"Octave cost",               my default_pitch_rawAC_octaveCost                ())
		REAL    (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_rawAC_octaveJumpCost            ())
		REAL    (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_rawAC_voicedUnvoicedCost        ())
	EDITOR_OK
	EDITOR_DO
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		kSoundAnalysisArea_pitch_analysisMethod analysisMethod = my instancePref_pitch_method();
		Melder_require (
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION ||
			analysisMethod == kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION,
			U"Obsolete advanced pitch settings command can handle only raw AC and CC methods."
			U"Use one of the modern pitch setting commands instead."
		);
		my setInstancePref_pitch_rawAC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawCC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawAC_viewTo (viewTo);
		my setInstancePref_pitch_rawCC_viewTo (viewTo);
		my setInstancePref_pitch_rawAC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawCC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawAC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawCC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawAC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawCC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawAC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawCC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawAC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawCC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawAC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawCC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawAC_voicedUnvoicedCost (voicedUnvoicedCost);
		my setInstancePref_pitch_rawCC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch.     reset();
		my d_intensity. reset();
		my d_pulses.    reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_advancedPitchSettings_rawAcCc_BEFORE_6414 (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced pitch settings (raw AC and CC)", U"Advanced pitch settings (raw AC and CC)...")
		COMMENT   (U"YOU SHOULD NEVER SEE THIS OBSOLETE SETTINGS WINDOW;")
		COMMENT   (U"IT IS HERE ONLY FOR COMPATIBILITY WITH OLD SCRIPTS.")
		NATURAL (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_rawAC_maximumNumberOfCandidates ())   // the default is ignored if not shown
		BOOLEAN (veryAccurate,              U"Very accurate", false)
		REAL    (silenceThreshold,          U"Silence threshold",         my default_pitch_rawAC_silenceThreshold          ())
		REAL    (voicingThreshold,          U"Voicing threshold",         my default_pitch_rawAC_voicingThreshold          ())
		REAL    (octaveCost,                U"Octave cost",               my default_pitch_rawAC_octaveCost                ())
		REAL    (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_rawAC_octaveJumpCost            ())
		REAL    (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_rawAC_voicedUnvoicedCost        ())
	EDITOR_OK
	EDITOR_DO
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_rawAC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawCC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawAC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawCC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawAC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawCC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawAC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawCC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawAC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawCC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawAC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawCC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawAC_voicedUnvoicedCost (voicedUnvoicedCost);
		my setInstancePref_pitch_rawCC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch.     reset();
		my d_intensity. reset();
		my d_pulses.    reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_advancedPitchSettings_filteredAcCc_BEFORE_6414 (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced pitch settings (filtered AC and CC)", U"Advanced pitch settings (filtered AC and CC)...")
		COMMENT   (U"YOU SHOULD NEVER SEE THIS OBSOLETE SETTINGS WINDOW;")
		COMMENT   (U"IT IS HERE ONLY FOR COMPATIBILITY WITH OLD SCRIPTS.")
		NATURAL  (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_filteredAC_maximumNumberOfCandidates ())   // the default is ignored if not shown
		BOOLEAN  (veryAccurate,              U"Very accurate",             my default_pitch_filteredAC_veryAccurate              ())
		POSITIVE (attenuationAtTop,          U"Attenuation at top",        my default_pitch_filteredAC_attenuationAtTop          ())
		REAL     (silenceThreshold,          U"Silence threshold",         my default_pitch_filteredAC_silenceThreshold          ())
		REAL     (voicingThreshold,          U"Voicing threshold",         my default_pitch_filteredAC_voicingThreshold          ())
		REAL     (octaveCost,                U"Octave cost",               my default_pitch_filteredAC_octaveCost                ())
		REAL     (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_filteredAC_octaveJumpCost            ())
		REAL     (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_filteredAC_voicedUnvoicedCost        ())
	EDITOR_OK
	EDITOR_DO
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_filteredAC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_filteredCC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_filteredAC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_filteredCC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_filteredAC_attenuationAtTop (attenuationAtTop);
		my setInstancePref_pitch_filteredCC_attenuationAtTop (attenuationAtTop);
		my setInstancePref_pitch_filteredAC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_filteredCC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_filteredAC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_filteredCC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_filteredAC_octaveCost (octaveCost);
		my setInstancePref_pitch_filteredCC_octaveCost (octaveCost);
		my setInstancePref_pitch_filteredAC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_filteredCC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_filteredAC_voicedUnvoicedCost (voicedUnvoicedCost);
		my setInstancePref_pitch_filteredCC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch.     reset();
		my d_intensity. reset();
		my d_pulses.    reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_pitchSettings_filteredAC (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings for the filtered autocorrelation method", U"Intro 4.2. Configuring the pitch contour")
		MUTABLE_COMMENT (methodMatchWarning, U"")
		HEADING (U"Where to search...")
		MUTABLE_CAPTION (note, U"")
		POSITIVE (pitchFloor, U"left Pitch floor and top (Hz)",            my default_pitch_filteredAC_floor())
		POSITIVE (pitchTop,   U"right Pitch floor and top (Hz)",           my default_pitch_filteredAC_top())
		HEADING (U"How to view...")
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit",                                                   my default_pitch_filteredAC_unit ())
		REAL    (viewFrom, U"left View range (units)",                     my default_pitch_filteredAC_viewFrom())
		REAL    (viewTo,   U"right View range (units)",                    my default_pitch_filteredAC_viewTo())
		CAPTION (U"(“auto” means ‘same as pitch floor and top’)")
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod,
				U"Drawing method",                                         my default_pitch_filteredAC_drawingMethod())
		HEADING (U"How to find the candidates...")
		NATURAL  (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_filteredAC_maximumNumberOfCandidates ())
		BOOLEAN  (veryAccurate,              U"Very accurate",             my default_pitch_filteredAC_veryAccurate())
		HEADING (U"How to preprocess the sound...")
		POSITIVE (attenuationAtTop,          U"Attenuation at top",        my default_pitch_filteredAC_attenuationAtTop          ())
		HEADING (U"How to find a path through the candidates...")
		REAL     (silenceThreshold,          U"Silence threshold",         my default_pitch_filteredAC_silenceThreshold          ())
		REAL     (voicingThreshold,          U"Voicing threshold",         my default_pitch_filteredAC_voicingThreshold          ())
		REAL     (octaveCost,                U"Octave cost",               my default_pitch_filteredAC_octaveCost                ())
		REAL     (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_filteredAC_octaveJumpCost            ())
		REAL     (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_filteredAC_voicedUnvoicedCost        ())
	EDITOR_OK
		const bool methodMatch = ( my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_AUTOCORRELATION );
		const conststring32 currentMethodText = kSoundAnalysisArea_pitch_analysisMethod_getText (my instancePref_pitch_method());
		SET_STRING (methodMatchWarning, methodMatch ?
			Melder_cat (U"(your current pitch analysis method is indeed ", currentMethodText, U")") :
			Melder_cat (U"WARNING: your current pitch analysis method is ", currentMethodText, U"!")
		)
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy ())
			SET_STRING (note, U"(Warning: your time step settings are non-standard; see Analysis menu)")
		else
			SET_STRING (note, U"(you have standard time step settings; see Analysis menu)")
		SET_REAL (pitchFloor,                   my instancePref_pitch_filteredAC_floor())
		SET_REAL (pitchTop,                     my instancePref_pitch_filteredAC_top())
		SET_ENUM (unit, kPitch_unit,            my instancePref_pitch_filteredAC_unit())
		SET_REAL (viewFrom,                     my instancePref_pitch_filteredAC_viewFrom())
		SET_REAL (viewTo,                       my instancePref_pitch_filteredAC_viewTo())
		SET_ENUM (drawingMethod, kSoundAnalysisArea_pitch_drawingMethod,
				                                my instancePref_pitch_filteredAC_drawingMethod())
		SET_INTEGER (maximumNumberOfCandidates, my instancePref_pitch_filteredAC_maximumNumberOfCandidates())
		SET_BOOLEAN (veryAccurate,              my instancePref_pitch_filteredAC_veryAccurate())
		SET_REAL    (attenuationAtTop,          my instancePref_pitch_filteredAC_attenuationAtTop())
		SET_REAL    (silenceThreshold,          my instancePref_pitch_filteredAC_silenceThreshold())
		SET_REAL    (voicingThreshold,          my instancePref_pitch_filteredAC_voicingThreshold())
		SET_REAL    (octaveCost,                my instancePref_pitch_filteredAC_octaveCost())
		SET_REAL    (octaveJumpCost,            my instancePref_pitch_filteredAC_octaveJumpCost())
		SET_REAL    (voicedUnvoicedCost,        my instancePref_pitch_filteredAC_voicedUnvoicedCost())
	EDITOR_DO
		Melder_require (pitchTop > pitchFloor,
			U"The pitch top has to be greater than the pitch floor, so they cannot be ",
			pitchTop, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_filteredAC_floor (pitchFloor);
		my setInstancePref_pitch_filteredAC_top (pitchTop);
		my setInstancePref_pitch_filteredAC_unit (unit);
		my setInstancePref_pitch_filteredAC_viewFrom (viewFrom);
		my setInstancePref_pitch_filteredAC_viewTo (viewTo);
		my setInstancePref_pitch_filteredAC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_filteredAC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_filteredAC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_filteredAC_attenuationAtTop (attenuationAtTop);
		my setInstancePref_pitch_filteredAC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_filteredAC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_filteredAC_octaveCost (octaveCost);
		my setInstancePref_pitch_filteredAC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_filteredAC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_pitchSettings_rawCC (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings for the raw cross-correlation method", U"Intro 4.2. Configuring the pitch contour")
		MUTABLE_COMMENT (methodMatchWarning, U"")
		HEADING (U"Where to search...")
		MUTABLE_CAPTION (note, U"")
		POSITIVE (pitchFloor,   U"left Pitch floor and ceiling (Hz)",      my default_pitch_rawCC_floor())
		POSITIVE (pitchCeiling, U"right Pitch floor and ceiling (Hz)",     my default_pitch_rawCC_ceiling())
		HEADING (U"How to view...")
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit",                                                   my default_pitch_rawCC_unit ())
		REAL    (viewFrom, U"left View range (units)",                     my default_pitch_rawCC_viewFrom())
		REAL    (viewTo,   U"right View range (units)",                    my default_pitch_rawCC_viewTo())
		CAPTION (U"(“auto” means ‘same as pitch floor and ceiling’)")
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod,
				U"Drawing method",                                         my default_pitch_rawCC_drawingMethod())
		HEADING (U"How to find the candidates...")
		NATURAL  (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_rawCC_maximumNumberOfCandidates ())
		BOOLEAN  (veryAccurate,              U"Very accurate",             my default_pitch_rawCC_veryAccurate())
		HEADING (U"How to find a path through the candidates...")
		REAL     (silenceThreshold,          U"Silence threshold",         my default_pitch_rawCC_silenceThreshold          ())
		REAL     (voicingThreshold,          U"Voicing threshold",         my default_pitch_rawCC_voicingThreshold          ())
		REAL     (octaveCost,                U"Octave cost",               my default_pitch_rawCC_octaveCost                ())
		REAL     (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_rawCC_octaveJumpCost            ())
		REAL     (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_rawCC_voicedUnvoicedCost        ())
	EDITOR_OK
		const bool methodMatch = ( my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION );
		const conststring32 currentMethodText = kSoundAnalysisArea_pitch_analysisMethod_getText (my instancePref_pitch_method());
		SET_STRING (methodMatchWarning, methodMatch ?
			Melder_cat (U"(your current pitch analysis method is indeed ", currentMethodText, U")") :
			Melder_cat (U"WARNING: your current pitch analysis method is ", currentMethodText, U"!")
		)
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy ())
			SET_STRING (note, U"(Warning: your time step settings are non-standard; see Analysis menu)")
		else
			SET_STRING (note, U"(you have standard time step settings; see Analysis menu)")
		SET_REAL (pitchFloor,                   my instancePref_pitch_rawCC_floor())
		SET_REAL (pitchCeiling,                 my instancePref_pitch_rawCC_ceiling())
		SET_ENUM (unit, kPitch_unit,            my instancePref_pitch_rawCC_unit())
		SET_REAL (viewFrom,                     my instancePref_pitch_rawCC_viewFrom())
		SET_REAL (viewTo,                       my instancePref_pitch_rawCC_viewTo())
		SET_ENUM (drawingMethod, kSoundAnalysisArea_pitch_drawingMethod,
				                                my instancePref_pitch_rawCC_drawingMethod())
		SET_INTEGER (maximumNumberOfCandidates, my instancePref_pitch_rawCC_maximumNumberOfCandidates())
		SET_BOOLEAN (veryAccurate,              my instancePref_pitch_rawCC_veryAccurate())
		SET_REAL    (silenceThreshold,          my instancePref_pitch_rawCC_silenceThreshold())
		SET_REAL    (voicingThreshold,          my instancePref_pitch_rawCC_voicingThreshold())
		SET_REAL    (octaveCost,                my instancePref_pitch_rawCC_octaveCost())
		SET_REAL    (octaveJumpCost,            my instancePref_pitch_rawCC_octaveJumpCost())
		SET_REAL    (voicedUnvoicedCost,        my instancePref_pitch_rawCC_voicedUnvoicedCost())
	EDITOR_DO
		Melder_require (pitchCeiling > pitchFloor,
			U"The pitch ceiling has to be greater than the pitch floor, so they cannot be ",
			pitchCeiling, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_rawCC_floor (pitchFloor);
		my setInstancePref_pitch_rawCC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawCC_unit (unit);
		my setInstancePref_pitch_rawCC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawCC_viewTo (viewTo);
		my setInstancePref_pitch_rawCC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_rawCC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawCC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawCC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawCC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawCC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawCC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawCC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_pitchSettings_rawAC (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings for the raw autocorrelation method", U"Intro 4.2. Configuring the pitch contour")
		MUTABLE_COMMENT (methodMatchWarning, U"")
		HEADING (U"Where to search...")
		MUTABLE_CAPTION (note, U"")
		POSITIVE (pitchFloor,   U"left Pitch floor and ceiling (Hz)",      my default_pitch_rawAC_floor())
		POSITIVE (pitchCeiling, U"right Pitch floor and ceiling (Hz)",     my default_pitch_rawAC_ceiling())
		HEADING (U"How to view...")
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit",                                                   my default_pitch_rawAC_unit ())
		REAL    (viewFrom, U"left View range (units)",                     my default_pitch_rawAC_viewFrom())
		REAL    (viewTo,   U"right View range (units)",                    my default_pitch_rawAC_viewTo())
		CAPTION (U"(“auto” means ‘same as pitch floor and ceiling’)")
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod,
				U"Drawing method",                                         my default_pitch_rawAC_drawingMethod())
		HEADING (U"How to find the candidates...")
		NATURAL  (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_rawAC_maximumNumberOfCandidates ())
		BOOLEAN  (veryAccurate,              U"Very accurate",             my default_pitch_rawAC_veryAccurate())
		HEADING (U"How to find a path through the candidates...")
		REAL     (silenceThreshold,          U"Silence threshold",         my default_pitch_rawAC_silenceThreshold          ())
		REAL     (voicingThreshold,          U"Voicing threshold",         my default_pitch_rawAC_voicingThreshold          ())
		REAL     (octaveCost,                U"Octave cost",               my default_pitch_rawAC_octaveCost                ())
		REAL     (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_rawAC_octaveJumpCost            ())
		REAL     (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_rawAC_voicedUnvoicedCost        ())
	EDITOR_OK
		const bool methodMatch = ( my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION );
		const conststring32 currentMethodText = kSoundAnalysisArea_pitch_analysisMethod_getText (my instancePref_pitch_method());
		SET_STRING (methodMatchWarning, methodMatch ?
			Melder_cat (U"(your current pitch analysis method is indeed ", currentMethodText, U")") :
			Melder_cat (U"WARNING: your current pitch analysis method is ", currentMethodText, U"!")
		)
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy ())
			SET_STRING (note, U"(Warning: your time step settings are non-standard; see Analysis menu)")
		else
			SET_STRING (note, U"(you have standard time step settings; see Analysis menu)")
		SET_REAL (pitchFloor,                   my instancePref_pitch_rawAC_floor())
		SET_REAL (pitchCeiling,                 my instancePref_pitch_rawAC_ceiling())
		SET_ENUM (unit, kPitch_unit,            my instancePref_pitch_rawAC_unit())
		SET_REAL (viewFrom,                     my instancePref_pitch_rawAC_viewFrom())
		SET_REAL (viewTo,                       my instancePref_pitch_rawAC_viewTo())
		SET_ENUM (drawingMethod, kSoundAnalysisArea_pitch_drawingMethod,
				                                my instancePref_pitch_rawAC_drawingMethod())
		SET_INTEGER (maximumNumberOfCandidates, my instancePref_pitch_rawAC_maximumNumberOfCandidates())
		SET_BOOLEAN (veryAccurate,              my instancePref_pitch_rawAC_veryAccurate())
		SET_REAL    (silenceThreshold,          my instancePref_pitch_rawAC_silenceThreshold())
		SET_REAL    (voicingThreshold,          my instancePref_pitch_rawAC_voicingThreshold())
		SET_REAL    (octaveCost,                my instancePref_pitch_rawAC_octaveCost())
		SET_REAL    (octaveJumpCost,            my instancePref_pitch_rawAC_octaveJumpCost())
		SET_REAL    (voicedUnvoicedCost,        my instancePref_pitch_rawAC_voicedUnvoicedCost())
	EDITOR_DO
		Melder_require (pitchCeiling > pitchFloor,
			U"The pitch ceiling has to be greater than the pitch floor, so they cannot be ",
			pitchCeiling, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_rawAC_floor (pitchFloor);
		my setInstancePref_pitch_rawAC_ceiling (pitchCeiling);
		my setInstancePref_pitch_rawAC_unit (unit);
		my setInstancePref_pitch_rawAC_viewFrom (viewFrom);
		my setInstancePref_pitch_rawAC_viewTo (viewTo);
		my setInstancePref_pitch_rawAC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_rawAC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_rawAC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_rawAC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_rawAC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_rawAC_octaveCost (octaveCost);
		my setInstancePref_pitch_rawAC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_rawAC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_pitchSettings_filteredCC (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Pitch settings for the filtered cross-correlation method", U"Intro 4.2. Configuring the pitch contour")
		MUTABLE_COMMENT (methodMatchWarning, U"")
		HEADING (U"Where to search...")
		MUTABLE_CAPTION (note, U"")
		POSITIVE (pitchFloor, U"left Pitch floor and top (Hz)",            my default_pitch_filteredCC_floor())
		POSITIVE (pitchTop, U"right Pitch floor and top (Hz)",             my default_pitch_filteredCC_top())
		HEADING (U"How to view...")
		OPTIONMENU_ENUM (kPitch_unit, unit,
				U"Unit",                                                   my default_pitch_filteredCC_unit ())
		REAL    (viewFrom, U"left View range (units)",                     my default_pitch_filteredCC_viewFrom())
		REAL    (viewTo,   U"right View range (units)",                    my default_pitch_filteredCC_viewTo())
		CAPTION (U"(“auto” means ‘same as pitch floor and top’)")
		OPTIONMENU_ENUM (kSoundAnalysisArea_pitch_drawingMethod, drawingMethod,
				U"Drawing method",                                         my default_pitch_filteredCC_drawingMethod())
		HEADING (U"How to find the candidates...")
		NATURAL  (maximumNumberOfCandidates, U"Max. number of candidates", my default_pitch_filteredCC_maximumNumberOfCandidates ())
		BOOLEAN  (veryAccurate,              U"Very accurate",             my default_pitch_filteredCC_veryAccurate())
		HEADING (U"How to preprocess the sound...")
		POSITIVE (attenuationAtTop,          U"Attenuation at top",        my default_pitch_filteredCC_attenuationAtTop          ())
		HEADING (U"How to find a path through the candidates...")
		REAL     (silenceThreshold,          U"Silence threshold",         my default_pitch_filteredCC_silenceThreshold          ())
		REAL     (voicingThreshold,          U"Voicing threshold",         my default_pitch_filteredCC_voicingThreshold          ())
		REAL     (octaveCost,                U"Octave cost",               my default_pitch_filteredCC_octaveCost                ())
		REAL     (octaveJumpCost,            U"Octave-jump cost",          my default_pitch_filteredCC_octaveJumpCost            ())
		REAL     (voicedUnvoicedCost,        U"Voiced / unvoiced cost",    my default_pitch_filteredCC_voicedUnvoicedCost        ())
	EDITOR_OK
		const bool methodMatch = ( my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_CROSS_CORRELATION );
		const conststring32 currentMethodText = kSoundAnalysisArea_pitch_analysisMethod_getText (my instancePref_pitch_method());
		SET_STRING (methodMatchWarning, methodMatch ?
			Melder_cat (U"(your current pitch analysis method is indeed ", currentMethodText, U")") :
			Melder_cat (U"WARNING: your current pitch analysis method is ", currentMethodText, U"!")
		)
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy ())
			SET_STRING (note, U"(Warning: your time step settings are non-standard; see Analysis menu)")
		else
			SET_STRING (note, U"(you have standard time step settings; see Analysis menu)")
		SET_REAL (pitchFloor,                   my instancePref_pitch_filteredCC_floor())
		SET_REAL (pitchTop,                     my instancePref_pitch_filteredCC_top())
		SET_ENUM (unit, kPitch_unit,            my instancePref_pitch_filteredCC_unit())
		SET_REAL (viewFrom,                     my instancePref_pitch_filteredCC_viewFrom())
		SET_REAL (viewTo,                       my instancePref_pitch_filteredCC_viewTo())
		SET_ENUM (drawingMethod, kSoundAnalysisArea_pitch_drawingMethod,
				                                my instancePref_pitch_filteredCC_drawingMethod())
		SET_INTEGER (maximumNumberOfCandidates, my instancePref_pitch_filteredCC_maximumNumberOfCandidates())
		SET_BOOLEAN (veryAccurate,              my instancePref_pitch_filteredCC_veryAccurate())
		SET_REAL    (attenuationAtTop,          my instancePref_pitch_filteredCC_attenuationAtTop())
		SET_REAL    (silenceThreshold,          my instancePref_pitch_filteredCC_silenceThreshold())
		SET_REAL    (voicingThreshold,          my instancePref_pitch_filteredCC_voicingThreshold())
		SET_REAL    (octaveCost,                my instancePref_pitch_filteredCC_octaveCost())
		SET_REAL    (octaveJumpCost,            my instancePref_pitch_filteredCC_octaveJumpCost())
		SET_REAL    (voicedUnvoicedCost,        my instancePref_pitch_filteredCC_voicedUnvoicedCost())
	EDITOR_DO
		Melder_require (pitchTop > pitchFloor,
			U"The pitch top has to be greater than the pitch floor, so they cannot be ",
			pitchTop, U" and ", pitchFloor, U" ", kPitch_unit_getText (unit), U", respectively."
		);
		if (maximumNumberOfCandidates < 2)
			Melder_throw (U"Your maximum number of candidates should be greater than 1.");
		my setInstancePref_pitch_filteredCC_floor (pitchFloor);
		my setInstancePref_pitch_filteredCC_top (pitchTop);
		my setInstancePref_pitch_filteredCC_unit (unit);
		my setInstancePref_pitch_filteredCC_viewFrom (viewFrom);
		my setInstancePref_pitch_filteredCC_viewTo (viewTo);
		my setInstancePref_pitch_filteredCC_drawingMethod (drawingMethod);
		my setInstancePref_pitch_filteredCC_maximumNumberOfCandidates (maximumNumberOfCandidates);
		my setInstancePref_pitch_filteredCC_veryAccurate (veryAccurate);
		my setInstancePref_pitch_filteredCC_attenuationAtTop (attenuationAtTop);
		my setInstancePref_pitch_filteredCC_silenceThreshold (silenceThreshold);
		my setInstancePref_pitch_filteredCC_voicingThreshold (voicingThreshold);
		my setInstancePref_pitch_filteredCC_octaveCost (octaveCost);
		my setInstancePref_pitch_filteredCC_octaveJumpCost (octaveJumpCost);
		my setInstancePref_pitch_filteredCC_voicedUnvoicedCost (voicedUnvoicedCost);
		my d_pitch. reset();
		my d_intensity. reset();
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_howToChooseAPitchAnalysisMethod (SoundAnalysisArea /* me */, EDITOR_ARGS) {
	Melder_help (U"how to choose a pitch analysis method");
}

static void shared_menu_cb_pitchMethodIsXXX (SoundAnalysisArea me, kSoundAnalysisArea_pitch_analysisMethod method) {
	my setInstancePref_pitch_method (method);
	my d_pitch.     reset();
	my d_intensity. reset();
	my d_pulses.    reset();
	FunctionEditor_redraw (my functionEditor());   // this will call v_updateMenuItems (last checked 2024-07-18 for Mac, Win, Linux)
}
static void menu_cb_pitchMethodIsFilteredAutocorrelation (SoundAnalysisArea me, EDITOR_ARGS) {
	shared_menu_cb_pitchMethodIsXXX (me, kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_AUTOCORRELATION);
}
static void menu_cb_pitchMethodIsRawCrossCorrelation (SoundAnalysisArea me, EDITOR_ARGS) {
	shared_menu_cb_pitchMethodIsXXX (me, kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION);
}
static void menu_cb_pitchMethodIsRawAutocorrelation (SoundAnalysisArea me, EDITOR_ARGS) {
	shared_menu_cb_pitchMethodIsXXX (me, kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION);
}
static void menu_cb_pitchMethodIsFilteredCrossCorrelation (SoundAnalysisArea me, EDITOR_ARGS) {
	shared_menu_cb_pitchMethodIsXXX (me, kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_CROSS_CORRELATION);
}

static void INFO_DATA__pitchListing (SoundAnalysisArea me, EDITOR_ARGS) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePitch (me);
		MelderInfo_open ();
		const kPitch_unit unit = my dynamic_instancePref_pitch_unit();
		MelderInfo_writeLine (U"Time_s   F0_",
				Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) unit, Function_UNIT_TEXT_SHORT));
		if (part == SoundAnalysisArea_PART_CURSOR) {
			double f0 = Pitch_getValueAtTime (my d_pitch.get(), tmin, unit, true);
			f0 = Function_convertToNonlogarithmic (my d_pitch.get(), f0, Pitch_LEVEL_FREQUENCY, (int) unit);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (f0, 6));
		} else {
			integer i, i1, i2;
			Sampled_getWindowSamples (my d_pitch.get(), tmin, tmax, & i1, & i2);
			for (i = i1; i <= i2; i ++) {
				const double t = Sampled_indexToX (my d_pitch.get(), i);
				double f0 = Sampled_getValueAtSample (my d_pitch.get(), i, Pitch_LEVEL_FREQUENCY, (int) unit);
				f0 = Function_convertToNonlogarithmic (my d_pitch.get(), f0, Pitch_LEVEL_FREQUENCY, (int) unit);
				MelderInfo_writeLine (Melder_fixed (t, 6), U"   ", Melder_fixed (f0, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void QUERY_DATA_FOR_REAL__getPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePitch (me);
		const kPitch_unit unit = my dynamic_instancePref_pitch_unit();
		double result =
			part == SoundAnalysisArea_PART_CURSOR
				? Pitch_getValueAtTime (my d_pitch.get(), tmin, unit, true)
				: Pitch_getMean (my d_pitch.get(), tmin, tmax, unit)
		;
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_DATA_FOR_REAL_END (U" ",
		Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) unit, 0),
		part == SoundAnalysisArea_PART_CURSOR
			? U" (interpolated pitch at CURSOR)"
			: Melder_cat (U" (mean pitch ", SoundAnalysisArea_partString_locative (part), U")")
	);
}

static void QUERY_DATA_FOR_REAL__getMinimumPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePitch (me);
		const kPitch_unit unit = my dynamic_instancePref_pitch_unit();
		double result = Pitch_getMinimum (my d_pitch.get(), tmin, tmax, unit, true);
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_DATA_FOR_REAL_END (U" ", Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) unit, 0),
			U" (minimum pitch ", SoundAnalysisArea_partString_locative (part), U")");
}

static void QUERY_DATA_FOR_REAL__getMaximumPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePitch (me);   // BUG
		const kPitch_unit unit = my dynamic_instancePref_pitch_unit();
		double result = Pitch_getMaximum (my d_pitch.get(), tmin, tmax, unit, true);
		result = Function_convertToNonlogarithmic (my d_pitch.get(), result, Pitch_LEVEL_FREQUENCY, (int) unit);
	QUERY_DATA_FOR_REAL_END (U" ", Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) unit, 0),
			U" (maximum pitch ", SoundAnalysisArea_partString_locative (part), U")");
}

static void menu_cb_moveCursorToMinimumPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		SoundAnalysisArea_haveVisiblePitch (me);
		if (my startSelection() == my endSelection()) {
			Melder_throw (U"Empty selection.");
		} else {
			double time;
			Pitch_getMinimumAndTime (my d_pitch.get(), my startSelection(), my endSelection(),
					my dynamic_instancePref_pitch_unit(), 1, nullptr, & time);
			if (isundef (time))
				Melder_throw (U"Selection is voiceless.");
			my setSelection (time, time);
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_selectionMarksChanged()
			FunctionEditor_selectionMarksChanged (my functionEditor());
		}
	VOID_EDITOR_END
}

static void menu_cb_moveCursorToMaximumPitch (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		SoundAnalysisArea_haveVisiblePitch (me);
		if (my startSelection() == my endSelection()) {
			Melder_throw (U"Empty selection.");
		} else {
			double time;
			Pitch_getMaximumAndTime (my d_pitch.get(), my startSelection(), my endSelection(),
					my dynamic_instancePref_pitch_unit(), 1, nullptr, & time);
			if (isundef (time))
				Melder_throw (U"Selection is voiceless.");
			my setSelection (time, time);
			Melder_assert (isdefined (my startSelection()));   // precondition of FunctionEditor_selectionMarksChanged()
			FunctionEditor_selectionMarksChanged (my functionEditor());
		}
	VOID_EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisiblePitchContour (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		SoundAnalysisArea_haveVisiblePitch (me);
		autoPitch result = Data_copy (my d_pitch.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisiblePitchContour (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible pitch contour", nullptr)
		my v_form_pictureWindow (cmd);
		COMMENT (U"Pitch:")
		BOOLEAN (speckle, U"Speckle", false)
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		SET_BOOLEAN (speckle, my instancePref_pitch_picture_speckle())
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_pitch_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my setInstancePref_pitch_picture_speckle (speckle);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_pitch_picture_garnish (garnish);
		SoundAnalysisArea_haveVisiblePitch (me);
		DataGui_openPraatPicture (me);
		const kPitch_unit pitchUnit = my dynamic_instancePref_pitch_unit();
		const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(),
				my dynamic_instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
		const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (my d_pitch.get(),
				my dynamic_instancePref_pitch_ceilingOrTop(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
		const double pitchFloor_overt = Function_convertToNonlogarithmic (my d_pitch.get(),
				pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
		const double pitchCeiling_overt = Function_convertToNonlogarithmic (my d_pitch.get(),
				pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
		const double pitchViewFrom = my dynamic_instancePref_pitch_viewFrom(), pitchViewTo = my dynamic_instancePref_pitch_viewTo();
		const double pitchViewFrom_overt = ( pitchViewFrom < pitchViewTo ? pitchViewFrom : pitchFloor_overt );
		const double pitchViewTo_overt = ( pitchViewFrom < pitchViewTo ? pitchViewTo : pitchCeiling_overt );
		Pitch_draw (my d_pitch.get(), my pictureGraphics(), my startWindow(), my endWindow(), pitchViewFrom_overt, pitchViewTo_overt,
				garnish, speckle, pitchUnit);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}


#pragma mark - SoundAnalysisArea Intensity menu

static void menu_cb_showIntensity (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		my setInstancePref_intensity_show (! my instancePref_intensity_show());   // toggle
		GuiMenuItem_check (my intensityToggle, my instancePref_intensity_show());   // in case we're called from a script
		FunctionEditor_redraw (my functionEditor());
	VOID_EDITOR_END
}

static void menu_cb_intensitySettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Intensity settings", U"Intro 6.2. Configuring the intensity contour")
		REAL (viewFrom, U"left View range (dB)",  my default_intensity_viewFrom ())
		REAL (viewTo,   U"right View range (dB)", my default_intensity_viewTo   ())
		CHOICE_ENUM (kSoundAnalysisArea_intensity_averagingMethod, averagingMethod,
				U"Averaging method", my default_intensity_averagingMethod ())
		BOOLEAN (subtractMeanPressure, U"Subtract mean pressure", my default_intensity_subtractMeanPressure ())
		COMMENT (U"Note: the pitch floor is taken from the pitch settings.")
		MUTABLE_COMMENT (note2, U"")
	EDITOR_OK
		SET_REAL (viewFrom,  my instancePref_intensity_viewFrom())
		SET_REAL (viewTo,    my instancePref_intensity_viewTo())
		SET_ENUM (averagingMethod, kSoundAnalysisArea_intensity_averagingMethod, my instancePref_intensity_averagingMethod())
		SET_BOOLEAN (subtractMeanPressure, my instancePref_intensity_subtractMeanPressure())
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy ()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		Melder_require (viewTo > viewFrom,
			U"The ceiling of the view range should be greater than the floor.");
		my setInstancePref_intensity_viewFrom (viewFrom);
		my setInstancePref_intensity_viewTo (viewTo);
		my setInstancePref_intensity_averagingMethod (averagingMethod);
		my setInstancePref_intensity_subtractMeanPressure (subtractMeanPressure);
		my d_intensity. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleIntensityContour (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		SoundAnalysisArea_haveVisibleIntensity (me);
		autoIntensity result = Data_copy (my d_intensity.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisibleIntensityContour (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible intensity contour", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_intensity_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_intensity_picture_garnish (garnish);
		SoundAnalysisArea_haveVisibleIntensity (me);
		DataGui_openPraatPicture (me);
		Intensity_draw (my d_intensity.get(), my pictureGraphics(), my startWindow(), my endWindow(),
				my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo(), garnish);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__intensityListing (SoundAnalysisArea me, EDITOR_ARGS) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleIntensity (me);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   Intensity_dB");
		if (part == SoundAnalysisArea_PART_CURSOR) {
			const double intensity = Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (intensity, 6));
		} else {
			integer i, i1, i2;
			Sampled_getWindowSamples (my d_intensity.get(), tmin, tmax, & i1, & i2);
			for (i = i1; i <= i2; i ++) {
				const double time = Sampled_indexToX (my d_intensity.get(), i);
				const double intensity = Vector_getValueAtX (my d_intensity.get(), time, Vector_CHANNEL_1, kVector_valueInterpolation :: NEAREST);
				MelderInfo_writeLine (Melder_fixed (time, 6), U"   ", Melder_fixed (intensity, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void QUERY_DATA_FOR_REAL__getIntensity (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleIntensity (me);
		const double result = ( part == SoundAnalysisArea_PART_CURSOR
			? Vector_getValueAtX (my d_intensity.get(), tmin, Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR)
			: Intensity_getAverage (my d_intensity.get(), tmin, tmax, (int) my instancePref_intensity_averagingMethod())
		);
	static const conststring32 methodString [] = { U"median", U"mean-energy", U"mean-sones", U"mean-dB" };
	QUERY_DATA_FOR_REAL_END (part == SoundAnalysisArea_PART_CURSOR
		? U" dB (intensity at CURSOR)"
		: Melder_cat (U" dB (", methodString [(int) my instancePref_intensity_averagingMethod()], U" intensity ",
				SoundAnalysisArea_partString_locative (part), U")")
	);
}

static void QUERY_DATA_FOR_REAL__getMinimumIntensity (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleIntensity (me);
		const double result = Vector_getMinimum (my d_intensity.get(), tmin, tmax, kVector_peakInterpolation :: PARABOLIC);
	QUERY_DATA_FOR_REAL_END (U" dB (minimum intensity ", SoundAnalysisArea_partString_locative (part), U")");
}

static void QUERY_DATA_FOR_REAL__getMaximumIntensity (SoundAnalysisArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleIntensity (me);
		const double result = Vector_getMaximum (my d_intensity.get(), tmin, tmax, kVector_peakInterpolation :: PARABOLIC);
	QUERY_DATA_FOR_REAL_END (U" dB (maximum intensity ", SoundAnalysisArea_partString_locative (part), U")");
}


#pragma mark - SoundAnalysisArea Formants menu

static void menu_cb_showFormants (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		my setInstancePref_formant_show (! my instancePref_formant_show());   // toggle
		GuiMenuItem_check (my formantToggle, my instancePref_formant_show());   // in case we're called from a script
		FunctionEditor_redraw (my functionEditor());
	VOID_EDITOR_END
}

static void menu_cb_formantSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Formant settings", U"Intro 5.2. Configuring the formant contours")
		POSITIVE (formantCeiling, U"Formant ceiling (Hz)",  my default_formant_ceiling          ())
		POSITIVE (numberOfFormants, U"Number of formants",  my default_formant_numberOfFormants ())
		POSITIVE (windowLength, U"Window length (s)",       my default_formant_windowLength     ())
		REAL     (dynamicRange, U"Dynamic range (dB)",      my default_formant_dynamicRange     ())
		POSITIVE (dotSize, U"Dot size (mm)",                my default_formant_dotSize          ())
		MUTABLE_COMMENT (note1, U"")
		MUTABLE_COMMENT (note2, U"")
	EDITOR_OK
		SET_REAL (formantCeiling,    my instancePref_formant_ceiling())
		SET_REAL (numberOfFormants,  my instancePref_formant_numberOfFormants())
		SET_REAL (windowLength,      my instancePref_formant_windowLength())
		SET_REAL (dynamicRange,      my instancePref_formant_dynamicRange())
		SET_REAL (dotSize,           my instancePref_formant_dotSize())
		if (my instancePref_formant_method() != my default_formant_method () || my instancePref_formant_preemphasisFrom() != Melder_atof (my default_formant_preemphasisFrom())) {
			SET_STRING (note1, U"Warning: you have non-standard \"advanced settings\".")
		} else {
			SET_STRING (note1, U"(all of your \"advanced settings\" have their standard values)")
		}
		if (my instancePref_timeStepStrategy() != my default_timeStepStrategy()) {
			SET_STRING (note2, U"Warning: you have a non-standard \"time step strategy\".")
		} else {
			SET_STRING (note2, U"(your \"time step strategy\" has its standard value: automatic)")
		}
	EDITOR_DO
		my setInstancePref_formant_ceiling (formantCeiling);
		my setInstancePref_formant_numberOfFormants (numberOfFormants);
		my setInstancePref_formant_windowLength (windowLength);
		my setInstancePref_formant_dynamicRange (dynamicRange);
		my setInstancePref_formant_dotSize (dotSize);
		my d_formant. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void menu_cb_advancedFormantSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced formant settings", U"Advanced formant settings...")
		CHOICE_ENUM (kSoundAnalysisArea_formant_analysisMethod, method,
				U"Method", my default_formant_method ())
		POSITIVE (preemphasisFrom, U"Pre-emphasis from (Hz)", my default_formant_preemphasisFrom ())
	EDITOR_OK
		SET_ENUM (method, kSoundAnalysisArea_formant_analysisMethod, my instancePref_formant_method())
		SET_REAL (preemphasisFrom, my instancePref_formant_preemphasisFrom())
	EDITOR_DO
		my setInstancePref_formant_method (method);
		my setInstancePref_formant_preemphasisFrom (preemphasisFrom);
		my d_formant. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisibleFormantContour (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		SoundAnalysisArea_haveVisibleFormants (me);
		autoFormant result = Data_copy (my d_formant.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisibleFormantContour (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible formant contour", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_formant_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_formant_picture_garnish (garnish);
		SoundAnalysisArea_haveVisibleFormants (me);
		DataGui_openPraatPicture (me);
		Formant_drawSpeckles (my d_formant.get(), my pictureGraphics(), my startWindow(), my endWindow(),
			my instancePref_spectrogram_viewTo(), my instancePref_formant_dynamicRange(),
			garnish
		);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__formantListing (SoundAnalysisArea me, EDITOR_ARGS) {
	INFO_DATA
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleFormants (me);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s   F1_Hz   F2_Hz   F3_Hz   F4_Hz");
		if (part == SoundAnalysisArea_PART_CURSOR) {
			const double f1 = Formant_getValueAtTime (my d_formant.get(), 1, tmin, kFormant_unit::HERTZ);
			const double f2 = Formant_getValueAtTime (my d_formant.get(), 2, tmin, kFormant_unit::HERTZ);
			const double f3 = Formant_getValueAtTime (my d_formant.get(), 3, tmin, kFormant_unit::HERTZ);
			const double f4 = Formant_getValueAtTime (my d_formant.get(), 4, tmin, kFormant_unit::HERTZ);
			MelderInfo_writeLine (Melder_fixed (tmin, 6), U"   ", Melder_fixed (f1, 6), U"   ", Melder_fixed (f2, 6), U"   ", Melder_fixed (f3, 6), U"   ", Melder_fixed (f4, 6));
		} else {
			integer i1, i2;
			Sampled_getWindowSamples (my d_formant.get(), tmin, tmax, & i1, & i2);
			for (integer i = i1; i <= i2; i ++) {
				const double t = Sampled_indexToX (my d_formant.get(), i);
				const double f1 = Formant_getValueAtTime (my d_formant.get(), 1, t, kFormant_unit::HERTZ);
				const double f2 = Formant_getValueAtTime (my d_formant.get(), 2, t, kFormant_unit::HERTZ);
				const double f3 = Formant_getValueAtTime (my d_formant.get(), 3, t, kFormant_unit::HERTZ);
				const double f4 = Formant_getValueAtTime (my d_formant.get(), 4, t, kFormant_unit::HERTZ);
				MelderInfo_writeLine (Melder_fixed (t, 6), U"   ", Melder_fixed (f1, 6), U"   ", Melder_fixed (f2, 6), U"   ", Melder_fixed (f3, 6), U"   ", Melder_fixed (f4, 6));
			}
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void do_getFormant (SoundAnalysisArea me, integer iformant, Interpreter optionalInterpreter) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleFormants (me);
		const double result = ( part == SoundAnalysisArea_PART_CURSOR
			? Formant_getValueAtTime (my d_formant.get(), iformant, tmin, kFormant_unit::HERTZ)
			: Formant_getMean (my d_formant.get(), iformant, tmin, tmax, kFormant_unit::HERTZ)
		);
	QUERY_DATA_FOR_REAL_END (part == SoundAnalysisArea_PART_CURSOR
		? Melder_cat (U" Hz (nearest F", iformant, U" to CURSOR)")
		: Melder_cat (U" Hz (mean F", iformant, U" ", SoundAnalysisArea_partString_locative (part), U")")
	)
}
static void do_getBandwidth (SoundAnalysisArea me, integer iformant, Interpreter optionalInterpreter) {
	QUERY_DATA_FOR_REAL
		double tmin, tmax;
		const int part = makeQueriable (me, true, & tmin, & tmax);
		SoundAnalysisArea_haveVisibleFormants (me);
		const double result = ( part == SoundAnalysisArea_PART_CURSOR
			? Formant_getBandwidthAtTime (my d_formant.get(), iformant, tmin, kFormant_unit::HERTZ)
			: Formant_getBandwidthAtTime (my d_formant.get(), iformant, 0.5 * (tmin + tmax), kFormant_unit::HERTZ)
		);
	QUERY_DATA_FOR_REAL_END (part == SoundAnalysisArea_PART_CURSOR
		? Melder_cat (U" Hz (nearest B", iformant, U" to CURSOR)")
		: Melder_cat ( U" Hz (B", iformant, U" in centre of ", SoundAnalysisArea_partString (part), U")")
	)
}
static void QUERY_DATA_FOR_REAL__getFirstFormant (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getFormant (me, 1, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getFirstBandwidth (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getBandwidth (me, 1, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getSecondFormant (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getFormant (me, 2, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getSecondBandwidth (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getBandwidth (me, 2, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getThirdFormant (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getFormant (me, 3, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getThirdBandwidth (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getBandwidth (me, 3, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getFourthFormant (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getFormant (me, 4, optionalInterpreter);
}
static void QUERY_DATA_FOR_REAL__getFourthBandwidth (SoundAnalysisArea me, EDITOR_ARGS) {
	do_getBandwidth (me, 4, optionalInterpreter);
}

static void QUERY_DATA_FOR_REAL__getFormant (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Get formant", nullptr)
		NATURAL (formantNumber, U"Formant number", U"5")
	EDITOR_OK
	EDITOR_DO
		do_getFormant (me, formantNumber, optionalInterpreter);
	EDITOR_END
}

static void QUERY_DATA_FOR_REAL__getBandwidth (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Get bandwidth", nullptr)
		NATURAL (formantNumber, U"Formant number", U"5")
	EDITOR_OK
	EDITOR_DO
		do_getBandwidth (me, formantNumber, optionalInterpreter);
	EDITOR_END
}


#pragma mark - SoundAnalysisArea Pulses menu

static void menu_cb_showPulses (SoundAnalysisArea me, EDITOR_ARGS) {
	VOID_EDITOR
		my setInstancePref_pulses_show (! my instancePref_pulses_show());   // toggle
		GuiMenuItem_check (my pulsesToggle, my instancePref_pulses_show());   // in case we're called from a script
		FunctionEditor_redraw (my functionEditor());
	VOID_EDITOR_END
}

static void menu_cb_advancedPulsesSettings (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Advanced pulses settings", U"Advanced pulses settings...")
		POSITIVE (maximumPeriodFactor,    U"Maximum period factor",    my default_pulses_maximumPeriodFactor    ())
		POSITIVE (maximumAmplitudeFactor, U"Maximum amplitude factor", my default_pulses_maximumAmplitudeFactor ())
	EDITOR_OK
		SET_REAL (maximumPeriodFactor,    my instancePref_pulses_maximumPeriodFactor())
		SET_REAL (maximumAmplitudeFactor, my instancePref_pulses_maximumAmplitudeFactor())
	EDITOR_DO
		my setInstancePref_pulses_maximumPeriodFactor (maximumPeriodFactor);
		my setInstancePref_pulses_maximumAmplitudeFactor (maximumAmplitudeFactor);
		my d_pulses. reset();
		FunctionEditor_redraw (my functionEditor());
	EDITOR_END
}

static void CONVERT_DATA_TO_ONE__ExtractVisiblePulses (SoundAnalysisArea me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		SoundAnalysisArea_haveVisiblePulses (me);
		autoPointProcess result = Data_copy (my d_pulses.get());
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void menu_cb_drawVisiblePulses (SoundAnalysisArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Draw visible pulses", nullptr)
		my v_form_pictureWindow (cmd);
		my v_form_pictureMargins (cmd);
		my v_form_pictureSelection (cmd);
		BOOLEAN (garnish, U"Garnish", true)
	EDITOR_OK
		my v_ok_pictureWindow (cmd);
		my v_ok_pictureMargins (cmd);
		my v_ok_pictureSelection (cmd);
		SET_BOOLEAN (garnish, my instancePref_pulses_picture_garnish())
	EDITOR_DO
		my v_do_pictureWindow (cmd);
		my v_do_pictureMargins (cmd);
		my v_do_pictureSelection (cmd);
		my setInstancePref_pulses_picture_garnish (garnish);
		SoundAnalysisArea_haveVisiblePulses (me);
		DataGui_openPraatPicture (me);
		PointProcess_draw (my d_pulses.get(), my pictureGraphics(), my startWindow(), my endWindow(), garnish);
		FunctionArea_garnishPicture (me);
		DataGui_closePraatPicture (me);
	EDITOR_END
}

static void INFO_DATA__voiceReport (SoundAnalysisArea me, EDITOR_ARGS) {
	INFO_DATA
		time_t today = time (nullptr);
		double tmin, tmax;
		const int part = makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePulses (me);
		autoSound sound = extractSoundOrNull (me, tmin, tmax);
		if (! sound)
			return;
		MelderInfo_open ();
		MelderInfo_writeLine (U"-- Voice report for ", my name.get(), U" --\nDate: ", Melder_peek8to32_u (ctime (& today)));
		if (my instancePref_pitch_method() != kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION) {
			if (my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION)
				MelderInfo_writeLine (U"WARNING: some of the following measurements may be imprecise.\n"
					"For more precision, go to “Pitch settings” and choose\n"
					"the raw cross-correlation analysis method to optimize for voice research.\n"
				);
			else
				MelderInfo_writeLine (U"WARNING: several of the following measurements, especially those on harmonicity, will be incorrect,\n"
					"because they are based on a sound from which higher frequencies have been filtered out.\n"
					"For more correctness, go to “Pitch settings” and choose\n"
					"the raw cross-correlation analysis method to optimize for voice research.\n"
				);
		}
		MelderInfo_writeLine (U"Time range of ", SoundAnalysisArea_partString (part));
		Sound_Pitch_PointProcess_voiceReport (sound.get(), my d_pitch.get(), my d_pulses.get(), tmin, tmax,
			my dynamic_instancePref_pitch_floor(), my dynamic_instancePref_pitch_ceilingOrTop(),
			my instancePref_pulses_maximumPeriodFactor(), my instancePref_pulses_maximumAmplitudeFactor(),
			my dynamic_instancePref_pitch_silenceThreshold(), my dynamic_instancePref_pitch_voicingThreshold()
		);
		if (my instancePref_pitch_method() != kSoundAnalysisArea_pitch_analysisMethod::RAW_CROSS_CORRELATION) {
			if (my instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod::RAW_AUTOCORRELATION)
				MelderInfo_writeLine (U"\nWARNING: some of the above measurements may be imprecise.\n"
					"For more precision, go to “Pitch settings” and choose\n"
					"the raw cross-correlation analysis method to optimize for voice research.\n"
				);
			else
				MelderInfo_writeLine (U"WARNING: several of the above measurements, especially those on harmonicity, will be incorrect,\n"
					"because they are based on a sound from which higher frequencies have been filtered out.\n"
					"For more correctness, go to “Pitch settings” and choose\n"
					"the raw cross-correlation analysis method to optimize for voice research.\n"
				);
		}
		MelderInfo_close ();
	INFO_DATA_END
}

static void INFO_DATA__pulseListing (SoundAnalysisArea me, EDITOR_ARGS) {
	INFO_DATA
		double tmin, tmax;
		makeQueriable (me, false, & tmin, & tmax);
		SoundAnalysisArea_haveVisiblePulses (me);
		MelderInfo_open ();
		MelderInfo_writeLine (U"Time_s");
		const integer i1 = PointProcess_getHighIndex (my d_pulses.get(), tmin);
		const integer i2 = PointProcess_getLowIndex (my d_pulses.get(), tmax);
		for (integer i = i1; i <= i2; i ++) {
			const double t = my d_pulses -> t [i];
			MelderInfo_writeLine (Melder_fixed (t, 12));
		}
		MelderInfo_close ();
	INFO_DATA_END
}

/*
static void cb_getJitter_xx (SoundAnalysisArea me, double (*PointProcess_getJitter_xx) (PointProcess, double, double, double, double, double)) {
	const double minimumPeriod = 0.8 / my p_pitch_ceiling, maximumPeriod = 1.25 / my p_pitch_floor;
	SoundAnalysisArea_haveVisiblePulses (me);
	if (my startSelection() == my endSelection())
		Melder_throw (U"Make a selection first.");
	makeQueriable
	Melder_informationReal (PointProcess_getJitter_xx (my d_pulses, my startSelection(), my endSelection(),
		minimumPeriod, maximumPeriod, my p_pulses_maximumPeriodFactor), nullptr);
}
DIRECT (SoundAnalysisArea, cb_getJitter_local) { cb_getJitter_xx (me, PointProcess_getJitter_local); END }
DIRECT (SoundAnalysisArea, cb_getJitter_local_absolute) { cb_getJitter_xx (me, PointProcess_getJitter_local_absolute); END }
DIRECT (SoundAnalysisArea, cb_getJitter_rap) { cb_getJitter_xx (me, PointProcess_getJitter_rap); END }
DIRECT (SoundAnalysisArea, cb_getJitter_ppq5) { cb_getJitter_xx (me, PointProcess_getJitter_ppq5); END }
DIRECT (SoundAnalysisArea, cb_getJitter_ddp) { cb_getJitter_xx (me, PointProcess_getJitter_ddp); END }

static void cb_getShimmer_xx (SoundAnalysisArea me, double (*PointProcess_Sound_getShimmer_xx) (PointProcess, Sound, double, double, double, double, double)) {
	const double minimumPeriod = 0.8 / my p_pitch_ceiling, maximumPeriod = 1.25 / my p_pitch_floor;
	SoundAnalysisArea_haveVisiblePulses (me);
	if (my startSelection() == my endSelection())
		Melder_throw (U"Make a selection first.");
	makeQueriable
	autoSound sound = extractSound (me, my startSelection(), my endSelection());
	Melder_informationReal (PointProcess_Sound_getShimmer_xx (my d_pulses, sound, my startSelection(), my endSelection(),
		minimumPeriod, maximumPeriod, my p_pulses_maximumAmplitudeFactor), nullptr);
}
DIRECT (SoundAnalysisArea, cb_getShimmer_local) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local); END }
DIRECT (SoundAnalysisArea, cb_getShimmer_local_dB) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_local_dB); END }
DIRECT (SoundAnalysisArea, cb_getShimmer_apq3) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq3); END }
DIRECT (SoundAnalysisArea, cb_getShimmer_apq5) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq5); END }
DIRECT (SoundAnalysisArea, cb_getShimmer_apq11) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_apq11); END }
DIRECT (SoundAnalysisArea, cb_getShimmer_dda) { cb_getShimmer_xx (me, PointProcess_Sound_getShimmer_dda); END }
*/


#pragma mark - SoundAnalysisArea All menus

void structSoundAnalysisArea :: v_createMenuItems_formant (EditorMenu menu) {
	our formantToggle = FunctionAreaMenu_addCommand (menu, U"Show formants",
		GuiMenu_CHECKBUTTON | (instancePref_formant_show() ? GuiMenu_TOGGLE_ON : 0),
		menu_cb_showFormants, this
	);
	FunctionAreaMenu_addCommand (menu, U"Formant settings...",
			0, menu_cb_formantSettings, this);
	FunctionAreaMenu_addCommand (menu, U"Advanced formant settings...",
			0, menu_cb_advancedFormantSettings, this);

	FunctionAreaMenu_addCommand (menu, U"- Query formants:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Formant listing", 1,
			INFO_DATA__formantListing, this);
	FunctionAreaMenu_addCommand (menu, U"Get first formant", GuiMenu_F1 | GuiMenu_DEPTH_1,
			QUERY_DATA_FOR_REAL__getFirstFormant, this);
	FunctionAreaMenu_addCommand (menu, U"Get first bandwidth", 1,
			QUERY_DATA_FOR_REAL__getFirstBandwidth, this);
	FunctionAreaMenu_addCommand (menu, U"Get second formant", GuiMenu_F2 | GuiMenu_DEPTH_1,
			QUERY_DATA_FOR_REAL__getSecondFormant, this);
	FunctionAreaMenu_addCommand (menu, U"Get second bandwidth", 1,
			QUERY_DATA_FOR_REAL__getSecondBandwidth, this);
	FunctionAreaMenu_addCommand (menu, U"Get third formant", GuiMenu_F3 | GuiMenu_DEPTH_1,
			QUERY_DATA_FOR_REAL__getThirdFormant, this);
	FunctionAreaMenu_addCommand (menu, U"Get third bandwidth", 1,
			QUERY_DATA_FOR_REAL__getThirdBandwidth, this);
	FunctionAreaMenu_addCommand (menu, U"Get fourth formant", GuiMenu_F4 | GuiMenu_DEPTH_1,
			QUERY_DATA_FOR_REAL__getFourthFormant, this);
	FunctionAreaMenu_addCommand (menu, U"Get fourth bandwidth", 1,
			QUERY_DATA_FOR_REAL__getFourthBandwidth, this);
	FunctionAreaMenu_addCommand (menu, U"Get formant...", 1,
			QUERY_DATA_FOR_REAL__getFormant, this);
	FunctionAreaMenu_addCommand (menu, U"Get bandwidth...", 1,
			QUERY_DATA_FOR_REAL__getBandwidth, this);

	FunctionAreaMenu_addCommand (menu, U"- Draw formants to picture window:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Draw visible formant contour...", 1,
			menu_cb_drawVisibleFormantContour, this);

	FunctionAreaMenu_addCommand (menu, U"- Extract formants to objects window:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Extract visible formant contour", 1,
			CONVERT_DATA_TO_ONE__ExtractVisibleFormantContour, this);
}

void structSoundAnalysisArea :: v_createMenus () {
	if (our v_hasSpectrogram () && our v_hasPitch () && our v_hasIntensity () && our v_hasPulses ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Analyses", 0);
		FunctionAreaMenu_addCommand (menu, U"Show analyses...", 0, menu_cb_showAnalyses, this);
		FunctionAreaMenu_addCommand (menu, U"Time step settings...", 0, menu_cb_timeStepSettings, this);
		FunctionAreaMenu_addCommand (menu, U"-- query log --", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Log settings...", 0, menu_cb_logSettings, this);
		FunctionAreaMenu_addCommand (menu, U"Delete log file 1", 0, menu_cb_deleteLogFile1, this);
		FunctionAreaMenu_addCommand (menu, U"Delete log file 2", 0, menu_cb_deleteLogFile2, this);
		FunctionAreaMenu_addCommand (menu, U"Log 1", GuiMenu_F12, menu_cb_log1, this);
		FunctionAreaMenu_addCommand (menu, U"Log 2", GuiMenu_F12 | GuiMenu_SHIFT, menu_cb_log2, this);
		FunctionAreaMenu_addCommand (menu, U"Log script 3 (...)", GuiMenu_F12 | GuiMenu_OPTION, menu_cb_logScript3, this);
		FunctionAreaMenu_addCommand (menu, U"Log script 4 (...)", GuiMenu_F12 | GuiMenu_COMMAND, menu_cb_logScript4, this);
	}

	if (our v_hasSpectrogram ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Spectrogram", 0);
		our spectrogramToggle = FunctionAreaMenu_addCommand (menu, U"Show spectrogram",
			GuiMenu_CHECKBUTTON | (instancePref_spectrogram_show() ? GuiMenu_TOGGLE_ON : 0),
			menu_cb_showSpectrogram, this
		);
		FunctionAreaMenu_addCommand (menu, U"Spectrogram settings...", 0,
				menu_cb_spectrogramSettings, this);
		FunctionAreaMenu_addCommand (menu, U"Advanced spectrogram settings...", 0,
				menu_cb_advancedSpectrogramSettings, this);
		FunctionAreaMenu_addCommand (menu, U"- Query spectrogram:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Get frequency at frequency cursor", 1,
				QUERY_EDITOR_FOR_REAL__getFrequency, this);
		FunctionAreaMenu_addCommand (menu, U"Get spectral power at cursor cross", GuiMenu_F7 | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getSpectralPowerAtCursorCross, this);
		FunctionAreaMenu_addCommand (menu, U"- Select frequency:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Move frequency cursor to...", 1,
				menu_cb_moveFrequencyCursorTo, this);
		FunctionAreaMenu_addCommand (menu, U"- Draw spectogram to picture window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Paint visible spectrogram...", 1,
				menu_cb_paintVisibleSpectrogram, this);
		FunctionAreaMenu_addCommand (menu, U"- Extract to objects window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Extract visible spectrogram", 1,
				CONVERT_DATA_TO_ONE__ExtractVisibleSpectrogram, this);
		FunctionAreaMenu_addCommand (menu, U"View spectral slice", 'L' | GuiMenu_DEPTH_1,
				CONVERT_DATA_TO_ONE__ViewSpectralSlice, this);
	}

	if (our v_hasPitch ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pitch", 0);
		our pitchToggle = FunctionAreaMenu_addCommand (menu, U"Show pitch",
			GuiMenu_CHECKBUTTON | ( our instancePref_pitch_show() ? GuiMenu_TOGGLE_ON : 0 ),
			menu_cb_showPitch, this
		);
		FunctionAreaMenu_addCommand (menu, U"- Pitch methods and settings:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"How to choose a pitch analysis method", GuiMenu_DEPTH_1,
				menu_cb_howToChooseAPitchAnalysisMethod, this);
		FunctionAreaMenu_addCommand (menu, U"", 0, nullptr, this);
		our pitchFilteredAutocorrelationToggle = FunctionAreaMenu_addCommand (menu, U"Pitch analysis method is filtered autocorrelation",
				GuiMenu_CHECKBUTTON | GuiMenu_DEPTH_1, menu_cb_pitchMethodIsFilteredAutocorrelation, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings (filtered autocorrelation)...", GuiMenu_DEPTH_1,
				menu_cb_pitchSettings_filteredAC, this);
		FunctionAreaMenu_addCommand (menu, U"", 0, nullptr, this);
		our pitchRawCrossCorrelationToggle = FunctionAreaMenu_addCommand (menu, U"Pitch analysis method is raw cross-correlation",
				GuiMenu_CHECKBUTTON | GuiMenu_DEPTH_1, menu_cb_pitchMethodIsRawCrossCorrelation, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings (raw cross-correlation)...", GuiMenu_DEPTH_1,
				menu_cb_pitchSettings_rawCC, this);
		FunctionAreaMenu_addCommand (menu, U"", 0, nullptr, this);
		our pitchRawAutocorrelationToggle = FunctionAreaMenu_addCommand (menu, U"Pitch analysis method is raw autocorrelation",
				GuiMenu_CHECKBUTTON | GuiMenu_DEPTH_1, menu_cb_pitchMethodIsRawAutocorrelation, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings (raw autocorrelation)...", GuiMenu_DEPTH_1,
				menu_cb_pitchSettings_rawAC, this);
		FunctionAreaMenu_addCommand (menu, U"", 0, nullptr, this);
		our pitchFilteredCrossCorrelationToggle = FunctionAreaMenu_addCommand (menu, U"Pitch analysis method is filtered cross-correlation",
				GuiMenu_CHECKBUTTON | GuiMenu_DEPTH_1, menu_cb_pitchMethodIsFilteredCrossCorrelation, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings (filtered cross-correlation)...", GuiMenu_DEPTH_1,
				menu_cb_pitchSettings_filteredCC, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch settings...", GuiMenu_HIDDEN,
				menu_cb_pitchSettings_BEFORE_6414, this);
		FunctionAreaMenu_addCommand (menu, U"Advanced pitch settings...", GuiMenu_HIDDEN,
				menu_cb_advancedPitchSettings_BEFORE_6400, this);
		FunctionAreaMenu_addCommand (menu, U"Advanced pitch settings (filtered AC and CC)...", GuiMenu_HIDDEN,
				menu_cb_advancedPitchSettings_filteredAcCc_BEFORE_6414, this);
		FunctionAreaMenu_addCommand (menu, U"Advanced pitch settings (raw AC and CC)...", GuiMenu_HIDDEN,
				menu_cb_advancedPitchSettings_rawAcCc_BEFORE_6414, this);
		FunctionAreaMenu_addCommand (menu, U"- Query pitch:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Pitch listing", 1,
				INFO_DATA__pitchListing, this);
		FunctionAreaMenu_addCommand (menu, U"Get pitch", GuiMenu_F5 | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getPitch, this);
		FunctionAreaMenu_addCommand (menu, U"Get minimum pitch", GuiMenu_F5 | GuiMenu_OPTION | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getMinimumPitch, this);
		FunctionAreaMenu_addCommand (menu, U"Get maximum pitch", GuiMenu_F5 | GuiMenu_SHIFT | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getMaximumPitch, this);
		FunctionAreaMenu_addCommand (menu, U"- Select by pitch:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Move cursor to minimum pitch", GuiMenu_COMMAND_EXTRA | 'L' | GuiMenu_DEPTH_1,
				menu_cb_moveCursorToMinimumPitch, this);
		FunctionAreaMenu_addCommand (menu, U"Move cursor to maximum pitch", GuiMenu_COMMAND_EXTRA | 'H' | GuiMenu_DEPTH_1,
				menu_cb_moveCursorToMaximumPitch, this);
		FunctionAreaMenu_addCommand (menu, U"- Draw pitch to picture window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Draw visible pitch contour...", 1,
				menu_cb_drawVisiblePitchContour, this);
		FunctionAreaMenu_addCommand (menu, U"- Extract pitch to objects window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Extract visible pitch contour", 1,
				CONVERT_DATA_TO_ONE__ExtractVisiblePitchContour, this);
	}

	if (our v_hasIntensity ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Intensity", 0);
		our intensityToggle = FunctionAreaMenu_addCommand (menu, U"Show intensity",
			GuiMenu_CHECKBUTTON | (instancePref_intensity_show() ? GuiMenu_TOGGLE_ON : 0),
			menu_cb_showIntensity, this
		);
		FunctionAreaMenu_addCommand (menu, U"Intensity settings...", 0,
				menu_cb_intensitySettings, this);
		FunctionAreaMenu_addCommand (menu, U"- Query intensity:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Intensity listing", 1,
				INFO_DATA__intensityListing, this);
		FunctionAreaMenu_addCommand (menu, U"Get intensity", GuiMenu_F8 | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getIntensity, this);
		FunctionAreaMenu_addCommand (menu, U"Get minimum intensity", GuiMenu_F8 | GuiMenu_OPTION | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getMinimumIntensity, this);
		FunctionAreaMenu_addCommand (menu, U"Get maximum intensity", GuiMenu_F8 | GuiMenu_SHIFT | GuiMenu_DEPTH_1,
				QUERY_DATA_FOR_REAL__getMaximumIntensity, this);
		FunctionAreaMenu_addCommand (menu, U"- Draw intensity to picture window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Draw visible intensity contour...", 1,
				menu_cb_drawVisibleIntensityContour, this);
		FunctionAreaMenu_addCommand (menu, U"- Extract intensity to objects window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Extract visible intensity contour", 1,
				CONVERT_DATA_TO_ONE__ExtractVisibleIntensityContour, this);
	}
	if (our v_hasFormants ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Formants", 0);
		our v_createMenuItems_formant (menu);
	}

	if (our v_hasPulses ()) {
		EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pulses", 0);
		our pulsesToggle = FunctionAreaMenu_addCommand (menu, U"Show pulses",
			GuiMenu_CHECKBUTTON | (instancePref_pulses_show() ? GuiMenu_TOGGLE_ON : 0),
			menu_cb_showPulses, this
		);
		FunctionAreaMenu_addCommand (menu, U"Advanced pulses settings...", 0,
				menu_cb_advancedPulsesSettings, this);
		FunctionAreaMenu_addCommand (menu, U"- Query pulses:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Voice report", 1,
				INFO_DATA__voiceReport, this);
		FunctionAreaMenu_addCommand (menu, U"Pulse listing", 1,
				INFO_DATA__pulseListing, this);
		/*
		FunctionAreaMenu_addCommand (menu, U"Get jitter (local)", 0, cb_getJitter_local, this);
		FunctionAreaMenu_addCommand (menu, U"Get jitter (local, absolute)", 0, cb_getJitter_local_absolute, this);
		FunctionAreaMenu_addCommand (menu, U"Get jitter (rap)", 0, cb_getJitter_rap, this);
		FunctionAreaMenu_addCommand (menu, U"Get jitter (ppq5)", 0, cb_getJitter_ppq5, this);
		FunctionAreaMenu_addCommand (menu, U"Get jitter (ddp)", 0, cb_getJitter_ddp, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (local)", 0, cb_getShimmer_local, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (local_dB)", 0, cb_getShimmer_local_dB, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq3)", 0, cb_getShimmer_apq3, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq5)", 0, cb_getShimmer_apq5, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (apq11)", 0, cb_getShimmer_apq11, this);
		FunctionAreaMenu_addCommand (menu, U"Get shimmer (dda)", 0, cb_getShimmer_dda, this);
		*/
		FunctionAreaMenu_addCommand (menu, U"- Draw pulses to picture window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Draw visible pulses...", 1,
				menu_cb_drawVisiblePulses, this);
		FunctionAreaMenu_addCommand (menu, U"- Extract pulses to objects window:", 0, nullptr, this);
		FunctionAreaMenu_addCommand (menu, U"Extract visible pulses", 1,
				CONVERT_DATA_TO_ONE__ExtractVisiblePulses, this);
	}
}

void structSoundAnalysisArea :: v_updateMenuItems () {
	if (our pitchFilteredAutocorrelationToggle) {
		GuiMenuItem_check (pitchFilteredAutocorrelationToggle,
				our instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_AUTOCORRELATION);
		GuiMenuItem_check (pitchRawCrossCorrelationToggle,
				our instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: RAW_CROSS_CORRELATION);
		GuiMenuItem_check (pitchRawAutocorrelationToggle,
				our instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: RAW_AUTOCORRELATION);
		GuiMenuItem_check (pitchFilteredCrossCorrelationToggle,
				our instancePref_pitch_method() == kSoundAnalysisArea_pitch_analysisMethod :: FILTERED_CROSS_CORRELATION);
	}
}

#pragma mark - SoundAnalysisArea Drawing

static void SoundAnalysisArea_v_draw_analysis (SoundAnalysisArea me) {
	/*
		d_pitch may not exist yet (if shown at all, it may be going to be created in tryToHavePitch (),
		and even if that fails the user should see what the pitch settings are). So we use a dummy object.
	*/
	const kPitch_unit pitchUnit = my dynamic_instancePref_pitch_unit();
	const double pitchFloor_hidden = Function_convertStandardToSpecialUnit (Thing_dummyObject (Pitch),
			my dynamic_instancePref_pitch_floor(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
	const double pitchCeiling_hidden = Function_convertStandardToSpecialUnit (Thing_dummyObject (Pitch),
			my dynamic_instancePref_pitch_ceilingOrTop(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
	const double pitchFloor_overt = Function_convertToNonlogarithmic (Thing_dummyObject (Pitch),
			pitchFloor_hidden, Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
	const double pitchCeiling_overt = Function_convertToNonlogarithmic (Thing_dummyObject (Pitch),
			pitchCeiling_hidden, Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
	const double pitchViewFrom = my dynamic_instancePref_pitch_viewFrom(), pitchViewTo = my dynamic_instancePref_pitch_viewTo();
	const bool verticalScaleIsLogarithmic = Function_isUnitLogarithmic (Thing_dummyObject (Pitch), Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
	const double pitchViewFrom_overt = (
		pitchViewFrom >= pitchViewTo ? pitchFloor_overt :
		pitchViewFrom == 0.0 && verticalScaleIsLogarithmic ? pitchViewTo / 1000.0 :
		pitchViewFrom
	);
	const double pitchViewTo_overt = ( pitchViewFrom < pitchViewTo ? pitchViewTo : pitchCeiling_overt );
	const double pitchViewFrom_hidden = ( verticalScaleIsLogarithmic ? log10 (pitchViewFrom_overt) : pitchViewFrom_overt );
	const double pitchViewTo_hidden = ( verticalScaleIsLogarithmic ? log10 (pitchViewTo_overt) : pitchViewTo_overt );

	if (my endWindow() - my startWindow() > my instancePref_longestAnalysis()) {
		Graphics_setWindow (my graphics(), 0.0, 1.0, 0.0, 1.0);
		Graphics_setFont (my graphics(), kGraphics_font::HELVETICA);
		Graphics_setFontSize (my graphics(), 10);
		Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
		Graphics_text (my graphics(), 0.5, 0.67,   U"(To see the analyses, zoom in to at most ", Melder_half (my instancePref_longestAnalysis()), U" seconds,");
		Graphics_text (my graphics(), 0.5, 0.33,   U"or raise the “longest analysis” setting with “Show analyses” in the Analyses menu.)");
		Graphics_setFontSize (my graphics(), 12);
		return;
	}
	if (my instancePref_spectrogram_show())
		tryToHaveSpectrogram (me);
	if (my instancePref_spectrogram_show() && my d_spectrogram) {
		Spectrogram_paintInside (my d_spectrogram.get(), my graphics(), my startWindow(), my endWindow(),
			my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo(),
			my instancePref_spectrogram_maximum(), my instancePref_spectrogram_autoscaling(),
			my instancePref_spectrogram_dynamicRange(), my instancePref_spectrogram_preemphasis(),
			my instancePref_spectrogram_dynamicCompression()
		);
	}
	if (my instancePref_pitch_show())
		tryToHavePitch (me);
	if (my instancePref_pitch_show() && my d_pitch) {
		const double greatestNonUndersamplingTimeStep = 0.5 * periodsPerAnalysisWindow (me) / my dynamic_instancePref_pitch_floor();
		const double defaultTimeStep = 0.5 * greatestNonUndersamplingTimeStep;
		const double timeStep = (
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::FIXED_ ? my instancePref_fixedTimeStep() :
			my instancePref_timeStepStrategy() == kSoundAnalysisArea_timeStepStrategy::VIEW_DEPENDENT ? (my endWindow() - my startWindow()) / my instancePref_numberOfTimeStepsPerView() :
			defaultTimeStep
		);
		const bool undersampled = ( timeStep > greatestNonUndersamplingTimeStep );
		const integer numberOfVisiblePitchPoints = (integer) ((my endWindow() - my startWindow()) / timeStep);   // BUG: why round down?
		Graphics_setColour (my graphics(), Melder_CYAN);
		Graphics_setLineWidth (my graphics(), 3.0);
		const kSoundAnalysisArea_pitch_drawingMethod drawingMethod = my dynamic_instancePref_pitch_drawingMethod();
		if ((drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::SPECKLE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics(), my startWindow(), my endWindow(), pitchViewFrom_overt, pitchViewTo_overt, true, pitchUnit);
		}
		if ((drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::AUTOMATIC && ! undersampled) ||
		    drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::CURVE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics(), my startWindow(), my endWindow(), pitchViewFrom_overt, pitchViewTo_overt, false, pitchUnit);
		}
		Graphics_setColour (my graphics(), Melder_BLUE);
		Graphics_setLineWidth (my graphics(), 1.0);
		if ((drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::AUTOMATIC && (undersampled || numberOfVisiblePitchPoints < 101)) ||
		    drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::SPECKLE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics(), my startWindow(), my endWindow(), pitchViewFrom_overt, pitchViewTo_overt, true, pitchUnit);
		}
		if ((drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::AUTOMATIC && ! undersampled) ||
		    drawingMethod == kSoundAnalysisArea_pitch_drawingMethod::CURVE)
		{
			Pitch_drawInside (my d_pitch.get(), my graphics(), my startWindow(), my endWindow(), pitchViewFrom_overt, pitchViewTo_overt, false, pitchUnit);
		}
		Graphics_setColour (my graphics(), Melder_BLACK);
	}
	if (my instancePref_intensity_show())
		tryToHaveIntensity (me);
	if (my instancePref_intensity_show() && my d_intensity) {
		Graphics_setColour (my graphics(), Melder_LIME);
		Graphics_setLineWidth (my graphics(), 3.0);
		Intensity_drawInside (my d_intensity.get(), my graphics(), my startWindow(), my endWindow(),
				my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo());
		Graphics_setColour (my graphics(), Melder_GREEN);
		Graphics_setLineWidth (my graphics(), 1.0);
		Intensity_drawInside (my d_intensity.get(), my graphics(), my startWindow(), my endWindow(),
				my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo());
		Graphics_setColour (my graphics(), Melder_BLACK);
	}

	my v_draw_analysis_formants ();

	/*
		Draw vertical scales.
	*/
	if (my instancePref_pitch_show()) {
		double pitchCursor_overt = undefined, pitchCursor_hidden = undefined;
		Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), pitchViewFrom_hidden, pitchViewTo_hidden);
		Graphics_setColour (my graphics(), 1.2 * Melder_BLUE);
		if (my d_pitch) {
			if (my startSelection() == my endSelection())
				pitchCursor_hidden = Pitch_getValueAtTime (my d_pitch.get(), my startSelection(), pitchUnit, 1);
			else
				pitchCursor_hidden = Pitch_getMean (my d_pitch.get(), my startSelection(), my endSelection(), pitchUnit);
			pitchCursor_overt = Function_convertToNonlogarithmic (my d_pitch.get(), pitchCursor_hidden, Pitch_LEVEL_FREQUENCY, (int) pitchUnit);
			if (isdefined (pitchCursor_hidden)) {
				Graphics_setColour (my graphics(), MelderColour (0.6, 0.0, 0.4));
				Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
				Graphics_text (my graphics(), my endWindow(), pitchCursor_hidden,
					Melder_float (Melder_half (pitchCursor_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
				Graphics_setColour (my graphics(), 1.2 * Melder_BLUE);
			}
			if (isundef (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics(), pitchCursor_hidden - pitchViewFrom_hidden) > 4.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
				Graphics_text (my graphics(), my endWindow(), pitchViewFrom_hidden - Graphics_dyMMtoWC (my graphics(), 0.5),
					Melder_float (Melder_half (pitchViewFrom_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
			}
			if (isundef (pitchCursor_hidden) || Graphics_dyWCtoMM (my graphics(), pitchViewTo_hidden - pitchCursor_hidden) > 4.0) {
				Graphics_setTextAlignment (my graphics(), Graphics_LEFT, Graphics_HALF);
				Graphics_text (my graphics(), my endWindow(), pitchViewTo_hidden,
					Melder_float (Melder_half (pitchViewTo_overt)), U" ",
					Function_getUnitText (my d_pitch.get(), Pitch_LEVEL_FREQUENCY, (int) pitchUnit, Function_UNIT_TEXT_SHORT | Function_UNIT_TEXT_GRAPHICAL)
				);
			}
		} else {
			Graphics_setTextAlignment (my graphics(), Graphics_CENTRE, Graphics_HALF);
			Graphics_setFontSize (my graphics(), 10);
			Graphics_text (my graphics(), 0.5 * (my startWindow() + my endWindow()), 0.5 * (pitchViewFrom_hidden + pitchViewTo_hidden),
					U"(Cannot show pitch contour. Zoom out or change bottom of pitch range in pitch settings.)");
			Graphics_setFontSize (my graphics(), 12);
		}
		Graphics_setColour (my graphics(), Melder_BLACK);
	}
	if (my instancePref_intensity_show()) {
		double intensityCursor = undefined;
		MelderColour textColour;
		kGraphics_horizontalAlignment hor;
		int vert;
		double y;
		if (! my instancePref_pitch_show()) {
			textColour = 1.2 * Melder_GREEN;
			hor = Graphics_LEFT;
			vert = Graphics_HALF;
			y = my endWindow();
		} else if (! my instancePref_spectrogram_show() && ! my instancePref_formant_show()) {
			textColour = 1.2 * Melder_GREEN;
			hor = Graphics_RIGHT;
			vert = Graphics_HALF;
			y = my startWindow();
		} else {
			textColour = ( my instancePref_spectrogram_show() ? Melder_LIME : 1.2 * Melder_GREEN );
			hor = Graphics_RIGHT;
			vert = -1;
			y = my endWindow();
		}
		if (my instancePref_intensity_viewTo() > my instancePref_intensity_viewFrom()) {
			Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), my instancePref_intensity_viewFrom(), my instancePref_intensity_viewTo());
			if (my d_intensity) {
				if (my startSelection() == my endSelection()) {
					intensityCursor = Vector_getValueAtX (my d_intensity.get(), my startSelection(), Vector_CHANNEL_1, kVector_valueInterpolation :: LINEAR);
				} else {
					intensityCursor = Intensity_getAverage (my d_intensity.get(), my startSelection(), my endSelection(), (int) my instancePref_intensity_averagingMethod());
				}
			}
			Graphics_setColour (my graphics(), textColour);
			const bool intensityCursorVisible = ( isdefined (intensityCursor) &&
					intensityCursor > my instancePref_intensity_viewFrom() && intensityCursor < my instancePref_intensity_viewTo() );
			if (intensityCursorVisible) {
				static const conststring32 methodString [] = { U" (.5)", U" (μE)", U" (μS)", U" (μ)" };
				Graphics_setTextAlignment (my graphics(), hor, Graphics_HALF);
				Graphics_text (my graphics(), y, intensityCursor,
					Melder_float (Melder_half (intensityCursor)), U" dB",
					my startSelection() == my endSelection() ? U"" : methodString [(int) my instancePref_intensity_averagingMethod()]
				);
			}
			if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics(), intensityCursor - my instancePref_intensity_viewFrom()) > 5.0) {
				Graphics_setTextAlignment (my graphics(), hor, vert == -1 ? Graphics_BOTTOM : Graphics_HALF);
				Graphics_text (my graphics(), y, my instancePref_intensity_viewFrom() - Graphics_dyMMtoWC (my graphics(), 0.5),
						Melder_float (Melder_half (my instancePref_intensity_viewFrom())), U" dB");
			}
			if (! intensityCursorVisible || Graphics_dyWCtoMM (my graphics(), my instancePref_intensity_viewTo() - intensityCursor) > 5.0) {
				Graphics_setTextAlignment (my graphics(), hor, vert == -1 ? Graphics_TOP : Graphics_HALF);
				Graphics_text (my graphics(), y, my instancePref_intensity_viewTo(),
						Melder_float (Melder_half (my instancePref_intensity_viewTo())), U" dB");
			}
			Graphics_setColour (my graphics(), Melder_BLACK);
		}
	}
	if (my instancePref_spectrogram_show() || my instancePref_formant_show()) {
		const bool frequencyCursorVisible = ( my d_spectrogram_cursor > my instancePref_spectrogram_viewFrom() && my d_spectrogram_cursor < my instancePref_spectrogram_viewTo() );
		Graphics_setWindow (my graphics(), my startWindow(), my endWindow(), my instancePref_spectrogram_viewFrom(), my instancePref_spectrogram_viewTo());
		/*
			Range marks.
		*/
		Graphics_setLineType (my graphics(), Graphics_DRAWN);
		Graphics_setColour (my graphics(), DataGuiColour_NONEDITABLE);
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics(), my d_spectrogram_cursor - my instancePref_spectrogram_viewFrom()) > 5.0) {
			Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
			Graphics_text (my graphics(), my startWindow(), my instancePref_spectrogram_viewFrom() - Graphics_dyMMtoWC (my graphics(), 0.5),
					Melder_float (Melder_half (my instancePref_spectrogram_viewFrom())), U" Hz");
		}
		if (! frequencyCursorVisible || Graphics_dyWCtoMM (my graphics(), my instancePref_spectrogram_viewTo() - my d_spectrogram_cursor) > 5.0) {
			Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
			Graphics_text (my graphics(), my startWindow(), my instancePref_spectrogram_viewTo(),
					Melder_float (Melder_half (my instancePref_spectrogram_viewTo())), U" Hz");
		}
		/*
			Cursor lines.
		*/
		Graphics_setLineType (my graphics(), Graphics_DOTTED);
		Graphics_setColour (my graphics(), DataGuiColour_NONEDITABLE_SELECTED);
		if (frequencyCursorVisible) {
			const double x = my startWindow(), y = my d_spectrogram_cursor;
			Graphics_setTextAlignment (my graphics(), Graphics_RIGHT, Graphics_HALF);
			Graphics_text (my graphics(), x, y,   Melder_float (Melder_half (y)), U" Hz");
			Graphics_line (my graphics(), x, y, my endWindow(), y);
		}
		/*
		if (our startSelection >= our startWindow && our startSelection <= our endWindow)
			Graphics_line (our graphics, our startSelection, our p_spectrogram_viewFrom, our startSelection, our p_spectrogram_viewTo);
		if (our endSelection > our startWindow && our endSelection < our endWindow && our endSelection != our startSelection)
			Graphics_line (our graphics, our endSelection, our p_spectrogram_viewFrom, our endSelection, our p_spectrogram_viewTo);*/
		Graphics_setLineType (my graphics(), Graphics_DRAWN);
	}
}
void structSoundAnalysisArea :: v_draw_analysis () {
	SoundAnalysisArea_v_draw_analysis (this);
}

void structSoundAnalysisArea :: v_draw_analysis_formants () {
	if (our instancePref_formant_show())
		tryToHaveFormants (this);
	if (our instancePref_formant_show() && our d_formant) {
		Graphics_setSpeckleSize (our graphics(), our instancePref_formant_dotSize());
		Formant_drawSpeckles_inside (our d_formant.get(), our graphics(), our startWindow(), our endWindow(),
			our instancePref_spectrogram_viewFrom(), our instancePref_spectrogram_viewTo(), our instancePref_formant_dynamicRange(),
			Melder_RED, Melder_PINK, true
		);
		Graphics_setColour (our graphics(), Melder_BLACK);
	}
}

void structSoundAnalysisArea :: v_draw_analysis_pulses () {
	if (our instancePref_pulses_show())
		tryToHavePulses (this);
	if (our instancePref_pulses_show() && our endWindow() - our startWindow() <= our instancePref_longestAnalysis() && our d_pulses) {
		PointProcess point = our d_pulses.get();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), -1.0, 1.0);
		Graphics_setColour (our graphics(), our editable() ? DataGuiColour_EDITABLE : Melder_SILVER);
		Graphics_setLineWidth (our graphics(), 2.0);
		if (point -> nt < 2000) for (integer i = 1; i <= point -> nt; i ++) {
			const double t = point -> t [i];
			if (t >= our startWindow() && t <= our endWindow())
				Graphics_line (our graphics(), t, -0.9, t, 0.9);
		}
		Graphics_setColour (our graphics(), Melder_BLACK);
		Graphics_setLineWidth (our graphics(), 1.0);
	}
}

void structSoundAnalysisArea :: v9_repairPreferences () {
	if (! (our instancePref_pitch_filteredAC_floor() < our instancePref_pitch_filteredAC_top())) {   // NaN-safe test
		our setInstancePref_pitch_filteredAC_floor (Melder_atof (our default_pitch_filteredAC_floor()));
		our setInstancePref_pitch_filteredAC_top (Melder_atof (our default_pitch_filteredAC_top()));
		our setInstancePref_pitch_filteredAC_unit (kPitch_unit::HERTZ);
	}
	if (! (our instancePref_pitch_rawCC_floor() < our instancePref_pitch_rawCC_ceiling())) {   // NaN-safe test
		our setInstancePref_pitch_rawCC_floor (Melder_atof (our default_pitch_rawCC_floor()));
		our setInstancePref_pitch_rawCC_ceiling (Melder_atof (our default_pitch_rawCC_ceiling()));
		our setInstancePref_pitch_rawCC_unit (kPitch_unit::HERTZ);
	}
	if (! (our instancePref_pitch_rawAC_floor() < our instancePref_pitch_rawAC_ceiling())) {   // NaN-safe test
		our setInstancePref_pitch_rawAC_floor (Melder_atof (our default_pitch_rawAC_floor()));
		our setInstancePref_pitch_rawAC_ceiling (Melder_atof (our default_pitch_rawAC_ceiling()));
		our setInstancePref_pitch_rawAC_unit (kPitch_unit::HERTZ);
	}
	if (! (our instancePref_pitch_filteredCC_floor() < our instancePref_pitch_filteredCC_top())) {   // NaN-safe test
		our setInstancePref_pitch_filteredCC_floor (Melder_atof (our default_pitch_filteredCC_floor()));
		our setInstancePref_pitch_filteredCC_top (Melder_atof (our default_pitch_filteredCC_top()));
		our setInstancePref_pitch_filteredCC_unit (kPitch_unit::HERTZ);
	}
	if (! (our instancePref_spectrogram_viewFrom() < our instancePref_spectrogram_viewTo())) {   // NaN-safe test
		our setInstancePref_spectrogram_viewFrom (Melder_atof (our default_spectrogram_viewFrom()));
		our setInstancePref_spectrogram_viewTo (Melder_atof (our default_spectrogram_viewTo()));
	}
	if (! (our instancePref_intensity_viewFrom() < our instancePref_intensity_viewTo())) {   // NaN-safe test
		our setInstancePref_intensity_viewFrom (Melder_atof (our default_intensity_viewFrom()));
		our setInstancePref_intensity_viewTo (Melder_atof (our default_intensity_viewTo()));
	}
	if (our instancePref_log1_toLogFile() == false && our instancePref_log1_toInfoWindow() == false) {
		our setInstancePref_log1_toLogFile (true);
		our setInstancePref_log1_toInfoWindow (true);
	}
	if (our instancePref_log2_toLogFile() == false && our instancePref_log2_toInfoWindow() == false) {
		our setInstancePref_log2_toLogFile (true);
		our setInstancePref_log2_toInfoWindow (true);
	}
	if (! our v_hasSpectrogram ())
		our setInstancePref_spectrogram_show (false);   // TODO: dubious, because what other editors will be affected?
	if (! our v_hasPitch ())
		our setInstancePref_pitch_show (false);
	if (! our v_hasIntensity ())
		our setInstancePref_intensity_show (false);
	if (! our v_hasFormants ())
		our setInstancePref_formant_show (false);
	if (! our v_hasPulses ())
		our setInstancePref_pulses_show (false);
	SoundAnalysisArea_Parent :: v9_repairPreferences ();
}

/* End of file SoundAnalysisArea.cpp */
