/* Sound_to_Formant_mt.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "Sound_to_Formant_mt.h"
#include "Sound_extensions.h"
#include "SoundFrames.h"
#include "Sound_and_LPC.h"
#include "LPC_and_Formant.h"

static void Sound_to_Formant_common (constSound inputSound, double& dt, double numberOfFormants_real, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency,	double safetyMargin, 
	autoFormant& outputFormant, autoLPC& outputLPC, autoSound& sound)
{
	try {
		if (dt <= 0.0)
			dt = effectiveAnalysisWidth / 4.0;
		autoSound resampled = Sound_resampleAndOrPreemphasize (inputSound, maximumFrequency, 50, preEmphasisFrequency);
		const integer numberOfPoles = 2.0 * numberOfFormants_real;
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		integer numberOfFrames;
		double t1;
		Sampled_shortTermAnalysis (sound.get(), physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		const integer numberOfFormants = ( safetyMargin == 0.0 ? numberOfPoles : (numberOfPoles + 1) / 2 );
		autoFormant formant = Formant_create (sound -> xmin, sound -> xmax, numberOfFrames, dt, t1, numberOfFormants);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			Formant_Frame formantFrame = &formant -> frames [iframe];
			Formant_Frame_init (formantFrame, numberOfFormants);
		}
		outputFormant = formant.move();
		autoLPC lpc = LPC_createCompletelyInitialized (sound -> xmin, sound -> xmax, formant -> nx, formant -> dx,
				formant -> x1, numberOfPoles, sound -> dx);
		outputLPC = lpc.move();
		sound = resampled.move();
	} catch (MelderError) {
		Melder_throw (U"Cannot create Formant or LPC or Sound.");
	}
}

static autoFormant createFormant_common (constSound me, double dt, integer numberOfPoles, double effectiveAnalysisWidth,
	double safetyMargin)
{
	integer numberOfFrames;
	double t1;
	const double physicalAnalysisWidth = getPhysicalAnalysisWidth2 (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	Sampled_shortTermAnalysis (me, physicalAnalysisWidth, dt, & numberOfFrames, & t1);
	const integer numberOfFormants = numberOfFormantsFromNumberOfCoefficients2 (numberOfPoles, safetyMargin);
	autoFormant formant = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
	return formant;
}

void Sound_into_Formant_burg (constSound me, mutableLPC intermediateLPC, mutableFormant outputFormant, 
	double effectiveAnalysisWidth, double safetyMargin)
{
	Sampleds_requireEqualDomains (me, intermediateLPC);
	Sampleds_assertEqualDomainsAndSampling (intermediateLPC, outputFormant);
	const integer order = intermediateLPC -> maxnCoefficients;
	const integer thresholdNumberOfFramesPerThread = 40;
	const double samplingPeriod = intermediateLPC -> samplingPeriod;

	MelderThread_PARALLEL (outputFormant -> nx, thresholdNumberOfFramesPerThread) {
		autoSoundFrames soundFrames = SoundFrames_create_fixedSampling (me, intermediateLPC, effectiveAnalysisWidth,
				kSound_windowShape::GAUSSIAN_2, true);
		integer info;
		autoVEC aa = raw_VEC (order);
		autoVEC b1 = raw_VEC (soundFrames -> soundFrameSize);
		autoVEC b2 = raw_VEC (soundFrames -> soundFrameSize);
		autoVEC polynomialIntoRootsWorkspace = raw_VEC (order * order + order + order + 11 * order);
		autoPolynomial p = Polynomial_create (-1.0, 1.0, order);
		autoRoots roots = Roots_create (order);
		MelderThread_FOR (iframe) {
			const LPC_Frame lpcFrame = & intermediateLPC -> d_frames [iframe];
			VEC soundFrame = soundFrames -> getMonoFrame (iframe);
			soundFrameIntoLPCFrame_burg (soundFrame, lpcFrame, b1.get(), b2.get(), aa.get(), info);
			Formant_Frame formantFrame = & outputFormant -> frames [iframe];
			LPC_Frame_into_Formant_Frame (lpcFrame, formantFrame, samplingPeriod,
					safetyMargin, p.get(), roots.get(), polynomialIntoRootsWorkspace.get());
		}
	} MelderThread_ENDPARALLEL
}

autoFormant Sound_to_Formant_burg_mt (constSound me, double dt, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin)
{
	try {
		autoSound sound;
		autoFormant outputFormant;
		autoLPC intermediateLPC;
		Sound_to_Formant_common (me, dt, numberOfFormants, maximumFrequency, effectiveAnalysisWidth,
				preEmphasisFrequency, safetyMargin, outputFormant, intermediateLPC, sound);
		Sound_into_Formant_burg (me, intermediateLPC.get(), outputFormant.get(), effectiveAnalysisWidth, safetyMargin);
		return outputFormant;
	} catch (MelderError) {
		Melder_throw (U"Could not create Formant (burg).");
	}
}

#if 0
void Sound_and_LPC_into_Formant (constSound inputSound, constLPC inputLPC, mutableLPC intermediateLPC,
	mutableFormant outputFormant, double effectiveAnalysisWidth, double safetyMargin, double k_stdev, 
	integer itermax, double tol, double location, bool wantlocation) 
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (inputSound, inputLPC);
	SampledAndSampled_requireEqualDomainsAndSampling (inputLPC, intermediateLPC);
	SampledAndSampled_requireEqualDomainsAndSampling (inputLPC, outputFormant);
	const integer order = inputLPC -> maxnCoefficients;
	const integer thresholdNumberOfFramesPerThread = 40;
	const double samplingPeriod = inputLPC -> samplingPeriod;
	MelderThread_PARALLEL (outputFormant -> nx, thresholdNumberOfFramesPerThread) {
		autoSoundFrames soundFrames = SoundFrames_createForSampled (inputSound, intermediateLPC, effectiveAnalysisWidth,
				kSound_windowShape::GAUSSIAN_2, true, false, 0_integer);
		integer info;
		autoRobustLPCWorkspace ws = RobustLPCWorkspace_create (inputLPC, inputSound, intermediateLPC,
				effectiveAnalysisWidth, kSound_windowShape :: GAUSSIAN_2, k_stdev, itermax, tol, wantlocation);
		autoVEC polynomialIntoRootsWorkspace = raw_VEC (order * order + order + order + 11 * order);		
		autoPolynomial p = Polynomial_create (-1.0, 1.0, order);
		autoRoots roots = Roots_create (order);
		MelderThread_FOR (iframe) {
			const LPC_Frame inputLPCFrame = & inputLPC -> d_frames [iframe];
			const LPC_Frame intermediateLPCFrame = & intermediateLPC -> d_frames [iframe];
			const Formant_Frame formantFrame = & outputFormant -> frames [iframe];
			VEC soundFrame = soundFrames -> getFrame (iframe);

			soundFrameAndLPCFrameIntoLPCFrame (soundFrame, inputLPCFrame, intermediateLPCFrame, ws.get(), info);
			LPC_Frame_into_Formant_Frame (intermediateLPCFrame, formantFrame, samplingPeriod,
					safetyMargin, p.get(), roots.get(), polynomialIntoRootsWorkspace.get());
		}
	} MelderThread_ENDPARALLEL
}

autoFormant Sound_and_LPC_to_Formant (constSound inputSound, constLPC inputLPC, double effectiveAnalysisWidth, double preEmphasisFrequency, 
	double safetyMargin, double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	try {
		const double maximumFrequency = 1.0 / inputLPC -> samplingPeriod;
		autoSound sound = Sound_resampleAndOrPreemphasize (inputSound, maximumFrequency, 50, preEmphasisFrequency);
		autoFormant outputFormant = createFormant_common (sound.get(), inputLPC -> dx, 
			inputLPC -> maxnCoefficients, effectiveAnalysisWidth, safetyMargin);
		autoLPC intermediateLPC = Data_copy (inputLPC);
		Sound_and_LPC_into_Formant (inputSound, inputLPC, intermediateLPC.get(), outputFormant.get(),
				effectiveAnalysisWidth, safetyMargin, k_stdev, itermax, tol, location, wantlocation);
		return outputFormant;
	} catch (MelderError) {
		Melder_throw (U"Could not create Formant from Sound and LPC.");
	}
}
#endif

autoFormant Sound_to_Formant_robust_mt (constSound inputSound, double dt, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin, double k_stdev, integer itermax, double tol,
	double location, bool wantlocation)
{
	try {
		autoSound sound;
		autoFormant outputFormant;
		autoLPC outputLPC;		
		Sound_to_Formant_common (inputSound, dt, numberOfFormants, maximumFrequency, effectiveAnalysisWidth, preEmphasisFrequency,
				safetyMargin, outputFormant, outputLPC, sound);
		const kSound_windowShape windowShape = kSound_windowShape::GAUSSIAN_2;
		
		Sound_into_LPC_robust (sound.get(), outputLPC.get(), effectiveAnalysisWidth, k_stdev, itermax, tol, wantlocation);
		LPC_into_Formant (outputLPC.get(), outputFormant.get(), safetyMargin);
		return outputFormant;
	} catch (MelderError) {
		Melder_throw (inputSound, U": no robust Formant created.");
	}
}

autoFormant Sound_to_Formant_robust (Sound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin,
	double numberOfStandardDeviations, integer maximumNumberOfIterations, double tolerance,
	double location, bool wantlocation)
{
	const double dt = ( dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0 );
	const double nyquist = 0.5 / my dx;
	const integer predictionOrder = Melder_ifloor (2 * numberOfFormants);
	try {
		autoSound sound;
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1.0) < 1.0e-12)
			sound = Data_copy (me);   // will be modified
		else
			sound = Sound_resample (me, maximumFrequency * 2.0, 50);

		autoLPC lpc = Sound_to_LPC_auto (sound.get(), predictionOrder, effectiveAnalysisWidth, dt, preEmphasisFrequency);
		autoLPC lpcRobust = LPC_and_Sound_to_LPC_robust (lpc.get(), sound.get(), effectiveAnalysisWidth, preEmphasisFrequency,
				numberOfStandardDeviations, maximumNumberOfIterations, tolerance, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcRobust.get(), safetyMargin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}


/*
void Sound_into_Formant_robust_mt (constSound me, mutableFormant thee, double effectiveAnalysisWidth, integer numberOfPoles, double safetyMargin,
	double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	autoSoundFrameIntoFormantFrameRobust ws = SoundFrameIntoFormantFrameRobust_create (me, thee,
			effectiveAnalysisWidth, kSound_windowShape :: GAUSSIAN_2, k_stdev, itermax, tol, location, wantlocation, numberOfPoles, safetyMargin);
	
	
	SampledToSampled_analyseThreaded (ws.get());
}

autoFormant Sound_to_Formant_robust_mt (constSound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin, double k_stdev, integer itermax, double tol,
	double location, bool wantlocation)
{
	const double dt = dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0;
	try {
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preEmphasisFrequency);
		integer numberOfFrames;
		double t1;
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		Sampled_shortTermAnalysis (sound.get(), physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		const integer numberOfPoles = numberOfPolesFromNumberOfFormants (numberOfFormants);
		const integer numberOfFormants = numberOfFormantsFromNumberOfCoefficients (numberOfPoles, safetyMargin);
		autoFormant formant = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		Sound_into_Formant_robust_mt (sound.get(), formant.get(), effectiveAnalysisWidth, numberOfPoles, safetyMargin, k_stdev,
				itermax, tol, location, wantlocation);
		return formant;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}
*/
/* End of file Sound_to_Formant_mt.cpp */
