#ifndef _Sound_and_TextGrid_extensions_h_
#define _Sound_and_TextGrid_extensions_h_
/* Sound_and_TextGrid_extensions.h
 *
 * Copyright (C) 1993-2023 David Weenink, 2026 Anastasia Shchupak
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

#include "Sound.h"
#include "TextGrid.h"

autoIntervalTier Sound_to_IntervalTier_highMidLowIntervals (Sound me, double min, double max);
autoTextGrid Sound_to_TextGrid_highMidLowIntervals (Sound me, double min, double max);

autoTextGrid Sound_to_TextGrid_detectSilences (Sound me, double minPitch, double timeStep,
	double silenceThreshold, double minSilenceDuration, double minSoundingDuration,
	conststring32 silentLabel, conststring32 soundingLabel);

autoTextGrid Sound_to_TextGrid_speechActivity_silero (constSound me, double speechProbabilityThreshold,
	double minNonSpeechDuration, double minSpeechDuration, double speechPad,
	conststring32 nonSpeechLabel, conststring32 speechLabel);

autoTextGrid Sound_to_TextGrid_speechActivity_ltsf (Sound me, double timeStep, double longTermWindow_r,
	double shorttimeAveragingWindow, double lowFrequencyThreshold, double highFrequencyThreshold, double ltsfThreshold,
	double nonspeechThreshold_dB, double minNonspeechDuration, 
	double minSpeechDuration,	conststring32 nonSpeechLabel, conststring32 speechLabel);

#endif /* _Sound_and_TextGrid_extensions_h_ */
