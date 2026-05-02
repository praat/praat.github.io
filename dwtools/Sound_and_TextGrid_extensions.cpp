/* Sound_and_TextGrid_extensions.cpp
 *
 * Copyright (C) 1993-2022 David Weenink, 2024,2025 Paul Boersma, 2026 Anastasia Shchupak
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

#include "Intensity_extensions.h"
#include "Sound_extensions.h"
#include "Sound_and_TextGrid_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_Intensity.h"
#include "SpeechRecognizer.h"
#include "TextGrid_Sound.h"

autoIntervalTier Sound_to_IntervalTier_highMidLowIntervals (Sound me, double min, double max) {
	try {
		autoIntervalTier thee = IntervalTier_create (my xmin, my xmax);
		double startTime = my xmin, endTime = my xmax;
		bool firstInterval = true;
		VEC z (my z[1]);
		int level = ( z [1] > max ? 1 : z [1] < min ? -1 : 0 );
		for (integer isamp = 2; isamp <= z.size; isamp ++) {
			const int leveli = ( z [isamp] > max ? 1 : z [isamp] < min ? -1 : 0 );
			if (leveli == level)
				continue;
			// new interval
			endTime = my x1 + (isamp - 1 - 0.5) * my dx;   // midpoint
			conststring32 label = ( level == 1 ? U"High" : level == -1 ? U"Low" : U"Mid" );
			if (firstInterval) {
				TextInterval interval = thy intervals.at [1];
				interval -> xmax = endTime;
				autostring32 firstLabel = Melder_dup (label);
				interval -> text = firstLabel.move();
				firstInterval = false;
			} else {
				autoTextInterval interval = TextInterval_create (startTime, endTime, label);
				thy intervals. addItem_move (interval.move());
			}
			startTime = endTime;
			level = leveli;
		}
		conststring32 last_label = ( level == 1 ? U"High" : level == -1 ? U"Low" : U"Mid" );
		autoTextInterval interval = TextInterval_create (startTime, my xmax, last_label);
		thy intervals. addItem_move (interval.move());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no IntervalTier with High/Mid/Low intervals created.");
	}
}

autoTextGrid Sound_to_TextGrid_highMidLowIntervals (Sound me, double min, double max) {
	try {
		autoTextGrid thee = TextGrid_createWithoutTiers (my xmin, my xmax);
		autoIntervalTier intervals = Sound_to_IntervalTier_highMidLowIntervals (me, min, max);
		thy tiers -> addItem_move (intervals.move());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no TextGrid with High/Mid/Low intervals created.");
	}
}

autoSound Sound_IntervalTier_cutPartsMatchingLabel (Sound me, IntervalTier thee, conststring32 match) {
	try {
		/*
			Count samples of the trimmed sound
		*/
		integer ixmin, ixmax, numberOfSamples = 0, previous_ixmax = 0;
		double xmin = my xmin; // start time of output sound is start time of input sound
		for (integer iint = 1; iint <= thy intervals.size; iint ++) {
			TextInterval interval = thy intervals.at [iint];
			if (! Melder_equ (interval -> text.get(), match)) {
				numberOfSamples += Sampled_getWindowSamples (me, interval -> xmin, interval -> xmax, & ixmin, & ixmax);
				/*
					If two contiguous intervals have to be copied then the last sample of previous interval
					and first sample of current interval might sometimes be equal
				*/
				if (ixmin == previous_ixmax)
					-- numberOfSamples;
				previous_ixmax = ixmax;
			} else { // matches label
				if (iint == 1)   // start time of output sound is end time of first interval
					xmin = interval -> xmax;
			}
		}
		/*
			Now copy the parts. The output sound starts at xmin
		*/
		autoSound him = Sound_create (my ny, xmin, xmin + numberOfSamples * my dx, numberOfSamples, my dx, xmin + 0.5 * my dx);
		numberOfSamples = 0;
		previous_ixmax = 0;
		for (integer iint = 1; iint <= thy intervals.size; iint ++) {
			const TextInterval interval = thy intervals.at [iint];
			if (! Melder_equ (interval -> text.get(), match)) {
				Sampled_getWindowSamples (me, interval -> xmin, interval -> xmax, & ixmin, & ixmax);
				if (ixmin == previous_ixmax)
					ixmin ++;
				previous_ixmax = ixmax;
				integer numberOfSamplesToCopy = ixmax - ixmin + 1;
				his z.part (1, my ny, numberOfSamples + 1, numberOfSamples + numberOfSamplesToCopy)  <<=  my z.part (1, my ny, ixmin, ixmax);
				numberOfSamples += numberOfSamplesToCopy;
			}
		}
		Melder_assert (numberOfSamples == his nx);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": intervals not trimmed.");
	}
}

autoTextGrid Sound_to_TextGrid_speechActivity_silero (constSound me, const double speechProbabilityThreshold,
	const double minNonSpeechDuration, const double minSpeechDuration, const double speechPad,
	const conststring32 nonSpeechLabel, const conststring32 speechLabel
) {
	try {
		SileroVadParams sileroVadParams;
		sileroVadParams. speechProbabilityThreshold = speechProbabilityThreshold;
		sileroVadParams. minNonSpeechDuration = minNonSpeechDuration;
		sileroVadParams. minSpeechDuration = minSpeechDuration;
		sileroVadParams. speechPad = speechPad;
		autovector <WhisperSegment> vadIntervals = doSileroVad (me, sileroVadParams, nonSpeechLabel, speechLabel);

		autoTextGrid thee = TextGrid_create (my xmin, my xmax, U"VAD", U"");
		const IntervalTier vadTier = static_cast <IntervalTier> (thy tiers->at [1]);
		splitIntervalIntoWhisperSegments (vadTier, 1, my xmin, my xmax, vadIntervals);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": could not detect speech activity.");
	}
}

autoTextGrid Sound_to_TextGrid_detectSilences (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration,
	conststring32 silentLabel, conststring32 soundingLabel)
{
	try {
		const bool subtractMeanPressure = true;
		autoSound filtered = Sound_filter_passHannBand (me, 80.0, 8000.0, 80.0);
		autoIntensity thee = Sound_to_Intensity (filtered.get(), minPitch, timeStep, subtractMeanPressure);
		autoTextGrid him = Intensity_to_TextGrid_detectSilences (thee.get(), silenceThreshold, minSilenceDuration, minSoundingDuration, silentLabel, soundingLabel);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no TextGrid with silences created.");
	}
}

/* End of file Sound_and_TextGrid_extensions.cpp */
