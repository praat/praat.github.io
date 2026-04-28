/* TextGrid_Sound.cpp
 *
 * Copyright (C) 1992-2020,2022,2024,2025 Paul Boersma
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

#include "TextGrid_Sound.h"
#include "Pitch_to_PitchTier.h"
#include "SpeechSynthesizer_and_TextGrid.h"
#include "SpeechRecognizer.h"
#include "LongSound.h"

static bool IntervalTier_check (IntervalTier me) {
	for (integer iinterval = 1; iinterval <= my intervals.size; iinterval ++) {
		TextInterval interval = my intervals.at [iinterval];
		if (interval -> xmin >= interval -> xmax) {
			Melder_casual (U"Interval ", iinterval, U" starts at ", interval -> xmin,
				U" but ends at ", interval -> xmax, U" seconds.");
			return false;
		}
	}
	if (my intervals.size < 2)
		return true;
	for (integer iinterval = 1; iinterval < my intervals.size; iinterval ++) {
		TextInterval thisInterval = my intervals.at [iinterval];
		TextInterval nextInterval = my intervals.at [iinterval + 1];
		if (thisInterval -> xmax != nextInterval -> xmin) {
			Melder_casual (U"Interval ", iinterval, U" ends at ", thisInterval -> xmax,
				U" but the next interval starts at ", nextInterval -> xmin, U" seconds.");
			return false;
		}
	}
	return true;
}

static void IntervalTier_insertIntervalDestructively (IntervalTier me, double tmin, double tmax) {
	Melder_assert (tmin < tmax);
	Melder_assert (tmin >= my xmin);
	Melder_assert (tmax <= my xmax);
	/*
		Make sure a boundary exists at tmin.
	*/
	if (! IntervalTier_hasTime (me, tmin)) {   // tmin is not a border but in the middle of some interval
		integer intervalNumber = IntervalTier_timeToIndex (me, tmin);   // this is the interval we will split in two
		Melder_require (intervalNumber > 0,
				U"Cannot add a boundary at ", Melder_fixed (tmin, 6),
				U" seconds, because this is outside the time domain of the intervals.");
		TextInterval leftPart = my intervals.at [intervalNumber];
		/*
			Move the text to the left of the boundary.
		*/
		autoTextInterval rightPart = TextInterval_create (tmin, leftPart -> xmax, U"");
		leftPart -> xmax = tmin;
		my intervals. addItem_move (rightPart.move());
	}

	/*
		Make sure a boundary exists at tmax.
	*/
	if (! IntervalTier_hasTime (me, tmax)) {   // tmin is not a border but in the middle of some interval
		integer intervalNumber = IntervalTier_timeToIndex (me, tmax);   // this is the interval we will split in two
		Melder_require (intervalNumber > 0,
				U"Cannot add a boundary at ", Melder_fixed (tmax, 6),
				U" seconds, because this is outside the time domain of the intervals.");
		TextInterval rightPart = my intervals.at [intervalNumber];
		/*
			Move the text to the right of the boundary.
		*/
		autoTextInterval leftPart = TextInterval_create (rightPart -> xmin, tmax, U"");
		rightPart -> xmin = tmax;
		my intervals. addItem_move (leftPart.move());
	}

	/*
		Collapse all intervals between tmin and tmax into one empty interval.
	*/
	integer firstIntervalNumber = IntervalTier_timeToIndex (me, tmin);
	integer lastIntervalNumber = IntervalTier_hasTime (me, tmax);   // either last or one after last
	Melder_assert (my intervals.at [firstIntervalNumber] -> xmin == tmin);
	Melder_assert (firstIntervalNumber >= 1 && firstIntervalNumber <= my intervals.size);
	Melder_assert (lastIntervalNumber >= 1 && lastIntervalNumber <= my intervals.size);

	trace (U"Remove intervals from ", lastIntervalNumber, U" down to (but not including) ", firstIntervalNumber);
	for (integer iinterval = lastIntervalNumber; iinterval > firstIntervalNumber; iinterval --) {
		TextInterval interval = my intervals.at [iinterval];
		if (interval -> xmin < tmax)   // excluding the last one if it is in fact after last
			my intervals. removeItem (iinterval);
	}
	trace (U"Extend interval ", firstIntervalNumber, U" (starting at ", tmin, U") to ", tmax);
	TextInterval newEmptyInterval = my intervals.at [firstIntervalNumber];
	newEmptyInterval -> xmax = tmax;
	TextInterval_setText (newEmptyInterval, U"");
}

static double IntervalTier_boundaryTimeClosestTo (IntervalTier me, double tmin, double tmax) {
	integer intervalNumber = IntervalTier_timeToLowIndex (me, tmax);
	if (intervalNumber != 0) {
		TextInterval interval = my intervals.at [intervalNumber];
		if (interval -> xmin > tmin && interval -> xmin < tmax)
			return interval -> xmin;
	}
	return 0.5 * (tmin + tmax);
}

static void IntervalTier_removeEmptyIntervals (IntervalTier me, IntervalTier boss) {
	IntervalTier_removeBoundariesBetweenIdenticallyLabeledIntervals (me, U"");
	if (my intervals.size < 2)
		return;
	TextInterval firstInterval = my intervals.at [1];
	if (Melder_equ (firstInterval -> text.get(), U""))
		IntervalTier_removeLeftBoundary (me, 2);
	if (my intervals.size < 2)
		return;
	TextInterval lastInterval = my intervals.at [my intervals.size];
	if (Melder_equ (lastInterval -> text.get(), U""))
		IntervalTier_removeLeftBoundary (me, my intervals.size);
	if (my intervals.size < 3)
		return;
	for (integer iinterval = my intervals.size - 1; iinterval >= 2; iinterval --) {
		TextInterval interval = my intervals.at [iinterval];
		if (Melder_equ (interval -> text.get(), U"")) {
			/*
				Distribute the empty interval between its neigbours.
			*/
			const double newBoundaryTime =
				boss ?
				IntervalTier_boundaryTimeClosestTo (boss, interval -> xmin, interval -> xmax) :
				0.5 * (interval -> xmin + interval -> xmax);
			TextInterval previous = my intervals.at [iinterval - 1];
			TextInterval next = my intervals.at [iinterval + 1];
			previous -> xmax = newBoundaryTime;
			next -> xmin = newBoundaryTime;
			my intervals. removeItem (iinterval);
		}
	}
}

void TextGrid_anySound_alignInterval (
	const TextGrid me, const Function anySound,
	const integer tierNumber, const integer intervalNumber,
	const conststring32 languageName,
	const bool includeWords, const bool includePhonemes
) {
	try {
		//TRACE
		IntervalTier headTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		if (intervalNumber < 1 || intervalNumber > headTier -> intervals.size)
			Melder_throw (U"Interval ", intervalNumber, U" does not exist.");
		TextInterval interval = headTier -> intervals.at [intervalNumber];
		if (! includeWords && ! includePhonemes)
			Melder_throw (U"Nothing to be done, because you asked neither for word alignment nor for phoneme alignment.");
		if (str32str (headTier -> name.get(), U"/"))
			Melder_throw (U"The current tier already has a slash (\"/\") in its name. Cannot create a word or phoneme tier from it.");
		trace (U"tier ", tierNumber, U" interval ", intervalNumber,
				U" (", interval -> xmin, U" .. ", interval -> xmax, U" “", interval -> text.get(), U"”)");
		autoSound part =
			anySound -> classInfo == classLongSound ? 
				LongSound_extractPart (static_cast <LongSound> (anySound), interval -> xmin, interval -> xmax, true) :
				Sound_extractPart (static_cast <Sound> (anySound), interval -> xmin, interval -> xmax, kSound_windowShape::RECTANGULAR, 1.0, true);
		autoSpeechSynthesizer synthesizer = SpeechSynthesizer_create (languageName, U"Female1");
		synthesizer -> d_samplingFrequency = round (
			anySound -> classInfo == classLongSound ?
				static_cast <LongSound> (anySound) -> sampleRate :
				1.0 / static_cast <Sound> (anySound) -> dx
		);
		double silenceThreshold = -30.0, minSilenceDuration = 0.1, minSoundingDuration = 0.1;
		autoTextGrid analysis;
int tries = 0;
constexpr int maxTries = 5;
again:
		if (! Melder_equ (interval -> text.get(), U""))
			try {
				analysis = SpeechSynthesizer_Sound_TextInterval_align
						(synthesizer.get(), part.get(), interval, silenceThreshold, minSilenceDuration, minSoundingDuration);
			} catch (MelderError) {
				if (++ tries < maxTries) {
					Melder_casual (U"TRY ", tries, U" FAILED (SpeechSynthesizer & Sound & TextInterval: align):");
					Melder_casual (U"    Tier ", tierNumber);
					Melder_casual (U"    Interval ", intervalNumber, U": ", interval -> xmin, U" .. ", interval -> xmax, U" “", interval -> text.get(), U"”");
					Melder_casual (U"REASON OF THIS TEMPORARY FAILURE (now comes a suppressed Praat error message):\n", Melder_getError (), U"(GOING TO RETRY...)");
					Melder_clearError ();
					goto again;
				} else
					Melder_throw (U"All ", maxTries, U" tries failed.");
			}
		if (analysis) {
			/*
				Clean up the analysis.
			*/
			Melder_assert (fabs (analysis -> xmin - interval -> xmin) < 1e-12);
			if (analysis -> xmax != interval -> xmax) {
				Melder_casual (U"Analysis ends at ", analysis -> xmax, U" but interval at ", interval -> xmax, U" seconds.");
				analysis -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (1) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (2) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (3) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (4) -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (1) -> intervals.at [analysis -> intervalTier_cast (1) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (2) -> intervals.at [analysis -> intervalTier_cast (2) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (3) -> intervals.at [analysis -> intervalTier_cast (3) -> intervals.size] -> xmax = interval -> xmax;
				analysis -> intervalTier_cast (4) -> intervals.at [analysis -> intervalTier_cast (4) -> intervals.size] -> xmax = interval -> xmax;
			}
			Melder_assert (analysis -> tiers->size == 4);
			IntervalTier analysisWordTier = analysis -> intervalTier_cast (3);
			tries += 1;
			if (! IntervalTier_check (analysisWordTier)) {
				if (tries < maxTries) {
					Melder_casual (U"TRY ", tries, U" FAILED (SpeechSynthesizer & Sound & TextInterval: align):");
					Melder_casual (U"    Tier ", tierNumber);
					Melder_casual (U"    Interval ", intervalNumber, U": ", interval -> xmin, U" .. ", interval -> xmax, U" “", interval -> text.get(), U"”");
					Melder_casual (U"REASON OF THIS TEMPORARY FAILURE:\n");
					Melder_casual (U"Analysis word tier out of order.\n(GOING TO RETRY...)");
					goto again;
				} else
					Melder_throw (U"All ", maxTries, U" tries failed.");
			} else if (tries > 1)
				Melder_casual (U"(TRY ", tries, U" SUCCEEDED)");
			IntervalTier_removeEmptyIntervals (analysisWordTier, nullptr);
			Melder_assert (analysisWordTier -> xmax == analysis -> xmax);
			Melder_assert (analysisWordTier -> intervals.size >= 1);
			TextInterval firstInterval = analysisWordTier -> intervals.at [1];
			TextInterval lastInterval = analysisWordTier -> intervals.at [analysisWordTier -> intervals.size];
			firstInterval -> xmin = analysis -> xmin;
			lastInterval  -> xmax = analysis -> xmax;
			if (lastInterval -> xmax != analysis -> xmax)
				Melder_crash (U"analysis ends at ", analysis -> xmax, U", but last interval at ", lastInterval -> xmax, U" seconds");
			if (! IntervalTier_check (analysisWordTier))
				Melder_throw (U"Analysis word tier out of order (2).");
			IntervalTier analysisPhonemeTier = analysis -> intervalTier_cast (4);
			if (! IntervalTier_check (analysisPhonemeTier))
				Melder_throw (U"Analysis phoneme tier out of order.");
			IntervalTier_removeEmptyIntervals (analysisPhonemeTier, analysisWordTier);
			Melder_assert (analysisPhonemeTier -> xmax == analysis -> xmax);
			Melder_assert (analysisPhonemeTier -> intervals.size >= 1);
			firstInterval = analysisPhonemeTier -> intervals.at [1];
			lastInterval  = analysisPhonemeTier -> intervals.at [analysisPhonemeTier -> intervals.size];
			firstInterval -> xmin = analysis -> xmin;
			lastInterval  -> xmax = analysis -> xmax;
			Melder_assert (lastInterval -> xmax == analysis -> xmax);
			if (! IntervalTier_check (analysisPhonemeTier))
				Melder_throw (U"Analysis phoneme tier out of order (2).");
		}
		integer wordTierNumber = 0, phonemeTierNumber = 0;
		IntervalTier wordTier = nullptr, phonemeTier = nullptr;
		/*
			Include a word tier.
		*/
		if (includeWords) {
			/*
			 * Make sure that the word tier exists.
			 */
			autoMelderString newWordTierName;
			MelderString_copy (& newWordTierName, headTier -> name.get(), U"/word");
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function tier = my tiers->at [itier];
				if (Melder_equ (newWordTierName.string, tier -> name.get())) {
					if (tier -> classInfo != classIntervalTier)
						Melder_throw (U"A tier with the prospective word tier name (", tier -> name.get(), U") already exists, but it is not an interval tier."
							U"\nPlease change its name or remove it.");
					wordTierNumber = itier;
					break;
				}
			}
			if (! wordTierNumber) {
				autoIntervalTier newWordTier = IntervalTier_create (my xmin, my xmax);
				Thing_setName (newWordTier.get(), newWordTierName.string);
				my tiers -> addItemAtPosition_move (newWordTier.move(), wordTierNumber = tierNumber + 1);
			}
			Melder_assert (wordTierNumber >= 1 && wordTierNumber <= my tiers->size);
			wordTier = static_cast <IntervalTier> (my tiers->at [wordTierNumber]);
			/*
				Make sure that the word tier has boundaries at the edges of the interval.
			*/
			IntervalTier_insertIntervalDestructively (wordTier, interval -> xmin, interval -> xmax);
			/*
				Copy the contents of the word analysis into the interval in the word tier.
			*/
			/* mutable */ integer wordIntervalNumber = IntervalTier_hasTime (wordTier, interval -> xmin);
			Melder_assert (wordIntervalNumber != 0);
			if (analysis) {
				IntervalTier analysisWordTier = analysis -> intervalTier_cast (3);
				if (! IntervalTier_check (analysisWordTier))
					Melder_throw (U"Analysis word tier out of order (3).");
				if (! IntervalTier_check (wordTier))
					Melder_throw (U"Word tier out of order (3).");
				for (integer ianalysisInterval = 1; ianalysisInterval <= analysisWordTier -> intervals.size; ianalysisInterval ++) {
					TextInterval analysisInterval = analysisWordTier -> intervals.at [ianalysisInterval];
					TextInterval wordInterval = nullptr;
					const double tmin = analysisInterval -> xmin, tmax = analysisInterval -> xmax;
					if (tmax == analysis -> xmax) {
						wordInterval = wordTier -> intervals.at [wordIntervalNumber];
						TextInterval_setText (wordInterval, analysisInterval -> text.get());
					} else {
						wordInterval = wordTier -> intervals.at [wordIntervalNumber];
						autoTextInterval newInterval = TextInterval_create (tmin, tmax, analysisInterval -> text.get());
						wordInterval -> xmin = tmax;
						wordTier -> intervals. addItem_move (newInterval.move());
						wordIntervalNumber ++;
					}
				}
				if (! IntervalTier_check (analysisWordTier))
					Melder_throw (U"Analysis word tier out of order (4).");
				if (! IntervalTier_check (wordTier))
					Melder_throw (U"Word tier out of order (4).");
			}
		}
		/*
			Include a phoneme tier.
		*/
		if (includePhonemes) {
			/*
				Make sure that the phoneme tier exists.
			*/
			autoMelderString newPhonemeTierName;
			MelderString_copy (& newPhonemeTierName, headTier -> name.get(), U"/phon");
			for (integer itier = 1; itier <= my tiers->size; itier ++) {
				Function tier = my tiers->at [itier];
				if (Melder_equ (newPhonemeTierName.string, tier -> name.get())) {
					if (tier -> classInfo != classIntervalTier)
						Melder_throw (U"A tier with the prospective phoneme tier name (", tier -> name.get(), U") already exists, but it is not an interval tier."
							U"\nPlease change its name or remove it.");
					phonemeTierNumber = itier;
					break;
				}
			}
			if (! phonemeTierNumber) {
				autoIntervalTier newPhonemeTier = IntervalTier_create (my xmin, my xmax);
				Thing_setName (newPhonemeTier.get(), newPhonemeTierName.string);
				my tiers -> addItemAtPosition_move (newPhonemeTier.move(),
					phonemeTierNumber = wordTierNumber ? wordTierNumber + 1 : tierNumber + 1);
			}
			Melder_assert (phonemeTierNumber >= 1 && phonemeTierNumber <= my tiers->size);
			phonemeTier = my intervalTier_cast (phonemeTierNumber);
			/*
				Make sure that the phoneme tier has boundaries at the edges of the interval.
			*/
			IntervalTier_insertIntervalDestructively (phonemeTier, interval -> xmin, interval -> xmax);
			/*
				Copy the contents of the phoneme analysis into the interval in the phoneme tier.
			*/
			integer phonemeIntervalNumber = IntervalTier_hasTime (phonemeTier, interval -> xmin);
			Melder_assert (phonemeIntervalNumber != 0);
			if (analysis) {
				IntervalTier analysisPhonemeTier = analysis -> intervalTier_cast (4);
				for (integer ianalysisInterval = 1; ianalysisInterval <= analysisPhonemeTier -> intervals.size; ianalysisInterval ++) {
					TextInterval analysisInterval = analysisPhonemeTier -> intervals.at [ianalysisInterval];
					TextInterval phonemeInterval = nullptr;
					double tmin = analysisInterval -> xmin, tmax = analysisInterval -> xmax;
					if (tmax == analysis -> xmax) {
						phonemeInterval = phonemeTier -> intervals.at [phonemeIntervalNumber];
						TextInterval_setText (phonemeInterval, analysisInterval -> text.get());
					} else {
						phonemeInterval = phonemeTier -> intervals.at [phonemeIntervalNumber];
						autoTextInterval newInterval = TextInterval_create (tmin, tmax, analysisInterval -> text.get());
						phonemeInterval -> xmin = tmax;
						phonemeTier -> intervals. addItem_move (newInterval.move());
						phonemeIntervalNumber ++;
					}
				}
			}
			if (includeWords) {
				/*
					Synchronize the boundaries between the word tier and the phoneme tier.
				*/
				//for (integer iinterval = 1; iinterval <=
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", anySound, U": interval not aligned.");
	}
}

void splitIntervalIntoWhisperSegments (const IntervalTier& tier, const integer tierNumber,
	const double original_tmin, const double original_tmax,
	const autovector<WhisperSegment>& segments
) {
	for (integer i = 1; i <= segments.size; ++ i) {
		WhisperSegment& segment = segments [i];

		double current_tmin = original_tmin + segment.tmin;
		double current_tmax = original_tmin + segment.tmax;
		if (i == segments.size) {
			current_tmax = original_tmax;
		}

		if (i == 1) {
			integer originalIntervalNumber = IntervalTier_hasTime (tier, original_tmin);
			TextInterval originalInterval = tier -> intervals.at [originalIntervalNumber];
			originalInterval -> xmax = current_tmax;
			TextInterval_setText (originalInterval, segment.text.get());
		} else {
			autoTextInterval newInterval = TextInterval_create (current_tmin, current_tmax, segment.text.get());
			tier -> intervals. addItem_move (newInterval.move ());
		}
	}

	if (! IntervalTier_check (tier))
		Melder_throw (U"Tier ", tierNumber, U" is out of order.");
}

void TextGrid_Sound_transcribeInterval (
	const TextGrid me, const Sound sound,
	const integer tierNumber, const integer intervalNumber,
	const conststring32 modelName, const conststring32 languageName,
	const bool includeWords, bool diarize, const bool useVad, const double speechProbabilityThreshold,
	const double minNonSpeechDuration, const double minSpeechDuration, const double speechPad
) {
	/*
		Lambda function to create and return a new tier after a specified tier.
		If overwrite == true and the tier with the given name already exists, return this tier.
	*/
	auto getIntervalTier = [me] (const autoMelderString & tierName, const integer prevTierNumber, const bool overwrite) {
		integer newTierNumber = 0;
		if (overwrite) {
			for (integer i = 1; i <= my tiers->size; i ++) {
				Function tier = my tiers->at [i];
				if (Melder_equ (tierName.string, tier -> name.get())) {
					if (tier -> classInfo != classIntervalTier)
						Melder_throw (U"A tier with the prospective tier name (", tier -> name.get(),
								U") already exists, but it is not an interval tier."
								U"\nPlease change its name or remove it.");
					newTierNumber = i;
					break;
				}
			}
		}
		if (! newTierNumber) {
			autoIntervalTier newTier = IntervalTier_create (my xmin, my xmax);
			Thing_setName (newTier.get(), tierName.string);
			newTierNumber = prevTierNumber + 1;
			my tiers -> addItemAtPosition_move (newTier.move(), newTierNumber);
			Melder_assert (newTierNumber >= 1 && newTierNumber <= my tiers -> size);
		}
		return newTierNumber;
	};

	/*
		Lambda function to compute an overlap of a given word with the given speaker activity (encoded in a speakerTier).
	*/
	auto computeSpeakerOverlap = [] (const WhisperSegment & wordSegment, IntervalTier speakerTier) {
		double overlap = 0.0;

		integer intervalStart = IntervalTier_timeToLowIndex (speakerTier, wordSegment.tmin);
		TextInterval speakerInterval = speakerTier -> intervals.at[intervalStart];
		conststring32 speakerIntervalText = speakerInterval -> text.get();

		/*
			Note: the assumption here is that one word spans maximum two speaker intervals (silence->speech or speech->silence)
		*/
		if (! speakerIntervalText || speakerIntervalText [0] == U'\0') {   // this word starts during this speaker's silence
			if (speakerInterval -> xmax > wordSegment.tmax)   // the whole word is during this speaker's silence
				overlap = 0.0;
			else
				overlap = wordSegment.tmax - speakerInterval -> xmax;   // second part of the word is during this speaker's speech
		} else {   // this word starts during this speaker's speech
			if (speakerInterval -> xmax > wordSegment.tmax)   // the whole word is during this speaker's speech
				overlap = wordSegment.tmax - wordSegment.tmin;
			else
				overlap = speakerInterval -> xmax - wordSegment.tmin;   // second part of the word is during this speaker's speech
		}
		return overlap;
	};

	try {
		//TRACE
		integer headTierNumber = tierNumber;
		IntervalTier headTier = TextGrid_checkSpecifiedTierIsIntervalTier (me, headTierNumber);
		autostring32 headTierName = Melder_dup (headTier -> name.get());
		TextInterval originalInterval = headTier -> intervals.at [intervalNumber];
		double original_tmin = originalInterval -> xmin;
		double original_tmax = originalInterval -> xmax;

		if (intervalNumber < 1 || intervalNumber > headTier -> intervals.size)
			Melder_throw (U"Interval ", intervalNumber, U" does not exist.");
		if (str32str (headTier -> name.get(), U"/"))
			Melder_throw (U"The current tier already has a slash (\"/\") in its name. Cannot create a word tier from it.");

		trace (U"tier ", headTierNumber, U" interval ", intervalNumber,	U" (", original_tmin, U" .. ", original_tmax, U")");
		autoSound soundPart = Sound_extractPart (sound, original_tmin, original_tmax,
			kSound_windowShape::RECTANGULAR, 1.0, false);
		autoSpeechRecognizer speechRecognizer = SpeechRecognizer_create (modelName, languageName);
		SileroVadParams sileroVadParams;
		sileroVadParams.speechProbabilityThreshold = speechProbabilityThreshold;
		sileroVadParams.minSpeechDuration = minSpeechDuration;
		sileroVadParams.minNonSpeechDuration = minNonSpeechDuration;
		sileroVadParams.speechPad = speechPad;
		trace(U"speechPad = ", speechPad);
		WhisperTranscription whisperTranscription = SpeechRecognizer_recognize (
				speechRecognizer.get(), soundPart.get(), useVad, sileroVadParams, diarize);

		autovector <WhisperSegment> wordSegments = whisperTranscription.words.move();
		autovector <WhisperSegment> sentenceSegments = whisperTranscription.sentences.move();
		autovector <autovector <WhisperSegment>> speakerSegments = whisperTranscription.speakers.move();

		integer n_speakers = speakerSegments.size;
		if (diarize && n_speakers < 1) {
			Melder_warning (U"Diarization detected 0 speakers. Diarization tiers are not created.");
			diarize = false;   // falling back to just transcription without diarization
		}

		/*
			----- Sentence transcription. -----
		*/
		if (! includeWords && ! diarize)
			splitIntervalIntoWhisperSegments (headTier, headTierNumber, original_tmin, original_tmax, sentenceSegments);

		/*
			----- Sentence transcription + word transcription. -----
		*/
		else if (includeWords && ! diarize) {
			splitIntervalIntoWhisperSegments (headTier, headTierNumber, original_tmin, original_tmax, sentenceSegments);

			/*
				Make sure that the word tier exists.
			*/
			autoMelderString wordTierName;
			MelderString_copy (& wordTierName, headTier -> name.get(), U"/word");
			const integer wordTierNumber = getIntervalTier(wordTierName, headTierNumber, true);
			const auto wordTier = static_cast <IntervalTier> (my tiers -> at [wordTierNumber]);

			/*
				Insert the interval, and split this big interval into the set of intervals, one interval per word.
			*/
			IntervalTier_insertIntervalDestructively (wordTier, original_tmin, original_tmax);
			splitIntervalIntoWhisperSegments (wordTier, wordTierNumber, original_tmin, original_tmax, wordSegments);
		}

		/*
			----- Diarization: transcription assigned to speakers. -----
		*/
		else if (diarize) {
			autovector <IntervalTier> speakerSentenceTiers = newvectorzero<IntervalTier> (n_speakers);
			autovector <IntervalTier> speakerWordTiers = newvectorzero<IntervalTier> (n_speakers);
			autovector <autoIntervalTier> virtualSpeakerTiers = newvectorzero<autoIntervalTier> (n_speakers);

			struct WordWithContext {
				WhisperSegment * whisperSegment;
				autovector <double> overlaps;
				integer resolvedSpeaker;
			};
			autovector<WordWithContext> wordsWithContext = newvectorzero<WordWithContext> (0);

			/*
				Create new sentence tiers for all speakers. Reuse the head tier for speaker 1.
			*/
			autoMelderString speakerSentenceTierName;
			MelderString_copy (& speakerSentenceTierName, headTierName.get(), U"/sp1");
			Thing_setName (headTier, speakerSentenceTierName.string);   // rename the head tier to make it "diarized tier" to prevent running diarization on it in the future
			speakerSentenceTiers [1] = static_cast <IntervalTier> (my tiers -> at [headTierNumber]);
			for (integer i = 2; i <= n_speakers; i ++) {
				MelderString_copy (& speakerSentenceTierName, headTierName.get(), U"/sp", i);
				const integer speakerSentenceTierNumber = getIntervalTier(
						speakerSentenceTierName, headTierNumber + i - 2, false);
				speakerSentenceTiers [i] = static_cast <IntervalTier> (my tiers -> at [speakerSentenceTierNumber]);
			}

			/*
				Create virtual diarization tiers , one per speaker, with speevch and non-speech intervals.
			*/
			for (integer i = 1; i <= n_speakers; i ++) {
				autoIntervalTier virtualSpeakerTier = IntervalTier_create (my xmin, my xmax);
				splitIntervalIntoWhisperSegments (virtualSpeakerTier.get(), 0, original_tmin, original_tmax, speakerSegments [i]);
				virtualSpeakerTiers [i] = virtualSpeakerTier.move();
			}

			/*
				Fill in the wordsWithContext vector: words (in a chronological order) and overlaps for each word
			*/
			for (integer s = 1; s <= wordSegments.size; s ++) {
				conststring32 wordSegmentText = wordSegments [s] .text.get();
				if (! wordSegmentText || wordSegmentText [0] == U'\0')
					continue;

				WordWithContext * wordWithContext = wordsWithContext.append();
				wordWithContext -> whisperSegment = & wordSegments [s];
				wordWithContext -> overlaps = newvectorzero<double> (n_speakers);
				wordWithContext -> resolvedSpeaker = 0;

				for (integer i = 1; i <= n_speakers; i ++)
					wordWithContext -> overlaps [i] = computeSpeakerOverlap (wordSegments [s], virtualSpeakerTiers [i].get());
			}

			/*
				Find a dominant speaker for each word.
			*/
			for (integer s = 1; s <= wordsWithContext.size; s ++) {
				constvector <double> overlaps = wordsWithContext [s] . overlaps.get();
				conststring32 text = wordsWithContext [s].whisperSegment -> text.get();
				double tmin = wordsWithContext [s].whisperSegment -> tmin;
				double tmax = wordsWithContext [s].whisperSegment -> tmax;
				double prev_tmax = s > 1 ? wordsWithContext [s - 1].whisperSegment -> tmax : wordsWithContext [1].whisperSegment -> tmin;
				integer prev_resolvedSpeaker = s > 1 ? wordsWithContext [s - 1] .resolvedSpeaker : 0;

				integer resolvedSpeaker = 1;
				double longestOverlap = overlaps [1];
				for (integer i = 2; i <= n_speakers; i ++) {
					if (overlaps [i] > longestOverlap) {
						resolvedSpeaker = i;
						longestOverlap = overlaps [i];
					} else if (overlaps [i] == longestOverlap && prev_resolvedSpeaker == i) {   // fallback to the last speaker
						resolvedSpeaker = i;
					}
				}

				if (s > 1 &&  longestOverlap < (tmax - tmin) / 2 && prev_tmax == tmin)   // if overlap is less than half, fallback to the last speaker
					resolvedSpeaker = prev_resolvedSpeaker;
				wordsWithContext [s] . resolvedSpeaker = resolvedSpeaker;
			}

			/*
				Lambda function to insert a subsentence to a speaker tier.
			*/
			auto insertSubsentenceToSpeakerTier = [&] (const integer subsentenceSpeaker, const autoMelderString & subsentenceText,
				const integer i_firstWordInSubsentence, const integer i_lastWordInSubsentence,
				const integer i_firstWordInSentence, const integer i_lastWordInSentence)
			{
				const double subsentence_tmin = original_tmin + wordsWithContext [i_firstWordInSubsentence] .whisperSegment -> tmin;
				const double subsentence_tmax = original_tmin + wordsWithContext [i_lastWordInSubsentence] .whisperSegment -> tmax;
				IntervalTier speakerSubsentenceTier = speakerSentenceTiers [subsentenceSpeaker];
				Melder_assert (subsentence_tmin < subsentence_tmax);
				IntervalTier_insertIntervalDestructively (speakerSubsentenceTier, subsentence_tmin, subsentence_tmax);
				integer subsentenceIntervalNumber = IntervalTier_hasTime (speakerSubsentenceTier, subsentence_tmin);

				autoMelderString fullText;
				if (i_firstWordInSubsentence != i_firstWordInSentence)   // before
					MelderString_append (& fullText, U"...");
				MelderString_append (& fullText, subsentenceText.string);   // text
				if (i_lastWordInSubsentence == i_lastWordInSentence)   // after
					MelderString_append (& fullText, U".");
				else
					MelderString_append (& fullText, U"...");

				TextInterval_setText (speakerSubsentenceTier -> intervals .at [subsentenceIntervalNumber], fullText.string);
			};

			/*
				Create sub-sentences, with first-word-speaker correction.
			*/
			integer i_currentWord = 1;
			for (integer sentence = 1; sentence <= sentenceSegments.size; sentence ++) {
				double sentence_tmax = sentenceSegments [sentence].tmax;
				conststring32 sentence_text = sentenceSegments [sentence].text.get();
				if (! sentence_text || sentence_text [0] == U'\0')
					continue;

				/*
					Find the first and the last words in the current sentence.
				*/
				integer i_firstWordInSentence = i_currentWord;
				while (i_currentWord <= wordsWithContext.size && wordsWithContext [i_currentWord] .whisperSegment -> tmin < sentence_tmax)
					i_currentWord ++;
				integer i_lastWordInSentence = i_currentWord - 1;

				/*
					If the first word speaker is different from the second word speaker, reassign the first word to the second's word speaker.
				*/
				if (i_firstWordInSentence < i_lastWordInSentence) {
					integer firstSpeaker = wordsWithContext [i_firstWordInSentence].resolvedSpeaker;
					integer secondSpeaker = wordsWithContext [i_firstWordInSentence + 1].resolvedSpeaker;
					if (firstSpeaker != secondSpeaker)
						wordsWithContext [i_firstWordInSentence].resolvedSpeaker = secondSpeaker;
				}

				/*
					Initialise the first subsentence.
				*/
				integer subsentenceSpeaker = wordsWithContext [i_firstWordInSentence] .resolvedSpeaker;
				integer i_firstWordInSubsentence = i_firstWordInSentence;
				autoMelderString subsentence_text;
				MelderString_copy (& subsentence_text, wordsWithContext [i_firstWordInSubsentence] .whisperSegment -> text.get());

				/*
					Iterate over all the words in the current sentence, inserting all the subsentence intervals except the last one.
				*/
				for (integer i = i_firstWordInSentence + 1; i <= i_lastWordInSentence; i ++) {
					integer currentSpeaker = wordsWithContext [i] .resolvedSpeaker;
					if (currentSpeaker != subsentenceSpeaker) {
						insertSubsentenceToSpeakerTier(subsentenceSpeaker, subsentence_text,
								i_firstWordInSubsentence, i - 1, i_firstWordInSentence, i_lastWordInSentence);
						subsentenceSpeaker = currentSpeaker;
						i_firstWordInSubsentence = i;
						MelderString_copy (& subsentence_text, wordsWithContext [i] .whisperSegment -> text.get());
					} else {
						MelderString_append (& subsentence_text, U" ", wordsWithContext [i] .whisperSegment -> text.get());
					}
				}

				/*
					Insert the last interval subsentence.
				*/
				insertSubsentenceToSpeakerTier(subsentenceSpeaker, subsentence_text,
						i_firstWordInSubsentence, i_lastWordInSentence, i_firstWordInSentence, i_lastWordInSentence);
			}

			/*
				----- Word transcription assigned to speakers. -----
			*/
			if (includeWords)  {
				/*
					Create new word tiers for all speakers.
				*/
				for (integer i = 1; i <= n_speakers; i ++) {
					autoMelderString speakerWordTierName;
					MelderString_copy (& speakerWordTierName, headTierName.get(), U"/sp", i, U"/w");
					const integer speakerWordTierNumber = getIntervalTier(   // headTierNumber headTierNumber+(1+1) headTierNumber+(1+1)+(1+1)
							speakerWordTierName, headTierNumber + 2 * (i - 1), false);
					speakerWordTiers [i] = static_cast <IntervalTier> (my tiers -> at [speakerWordTierNumber]);
				}

				/*
					Insert words into speaker word tiers.
				*/
				for (integer s = 1; s <= wordsWithContext.size; s ++) {
					integer resolvedSpeaker = wordsWithContext [s] .resolvedSpeaker;
					double tmin = original_tmin + wordsWithContext [s] .whisperSegment -> tmin;
					double tmax = original_tmin + wordsWithContext [s] .whisperSegment -> tmax;
					conststring32 text = wordsWithContext [s] .whisperSegment -> text.get();

					Melder_assert (tmin < tmax);
					IntervalTier_insertIntervalDestructively (speakerWordTiers [resolvedSpeaker], tmin, tmax);
					integer wordIntervalNumber = IntervalTier_hasTime (speakerWordTiers [resolvedSpeaker], tmin);
					TextInterval_setText (speakerWordTiers [resolvedSpeaker] -> intervals .at [wordIntervalNumber], text);
				}
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": interval not transcribed.");
	}
}

void TextGrid_Sound_draw (
	const TextGrid me, const Sound sound, const Graphics g,
	/* mutable autowindow */ double tmin, /* mutable autowindow */ double tmax,
	const bool showBoundaries, const bool useTextStyles, const bool garnish
) {
	const integer numberOfTiers = my tiers ->size;

	Function_unidirectionalAutowindow (me, & tmin, & tmax);

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, -1.0 - 0.5 * numberOfTiers, 1.0);

	/*
		Draw sound in upper part.
	*/
	integer first, last;
	if (sound && Sampled_getWindowSamples (sound, tmin, tmax, & first, & last) > 1) {
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_line (g, tmin, 0.0, tmax, 0.0);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_function (g, & sound -> z [1] [0], first, last,
				Sampled_indexToX (sound, first), Sampled_indexToX (sound, last));
	}

	/*
		Draw labels in lower part.
	*/
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_setPercentSignIsItalic (g, useTextStyles);
	Graphics_setNumberSignIsBold (g, useTextStyles);
	Graphics_setCircumflexIsSuperscript (g, useTextStyles);
	Graphics_setUnderscoreIsSubscript (g, useTextStyles);
	for (integer itier = 1; itier <= numberOfTiers; itier ++) {
		Function anyTier = my tiers->at [itier];
		const double ymin = -1.0 - 0.5 * itier, ymax = ymin + 0.5;
		Graphics_rectangle (g, tmin, tmax, ymin, ymax);
		if (anyTier -> classInfo == classIntervalTier) {
			IntervalTier tier = static_cast <IntervalTier> (anyTier);
			const integer ninterval = tier -> intervals.size;
			for (integer iinterval = 1; iinterval <= ninterval; iinterval ++) {
				TextInterval interval = tier -> intervals.at [iinterval];
				const double intmin = Melder_clippedLeft (tmin, interval -> xmin);
				const double intmax = Melder_clippedRight (interval -> xmax, tmax);
				if (intmin >= intmax)
					continue;
				if (showBoundaries && intmin > tmin && intmin < tmax) {
					Graphics_setLineType (g, Graphics_DOTTED);
					Graphics_line (g, intmin, -1.0, intmin, 1.0);   // in sound part
					Graphics_setLineType (g, Graphics_DRAWN);
				}
				/*
					Draw left boundary.
				*/
				if (intmin > tmin && intmin < tmax)
					Graphics_line (g, intmin, ymin, intmin, ymax);
				/*
					Draw label text.
				*/
				if (interval -> text && intmax >= tmin && intmin <= tmax) {
					const double t1 = tmin > intmin ? tmin : intmin;
					const double t2 = tmax < intmax ? tmax : intmax;
					Graphics_text (g, 0.5 * (t1 + t2), 0.5 * (ymin + ymax), interval -> text.get());
				}
			}
		} else {
			TextTier tier = static_cast <TextTier> (anyTier);
			const integer numberOfPoints = tier -> points.size;
			for (integer ipoint = 1; ipoint <= numberOfPoints; ipoint ++) {
				TextPoint point = tier -> points.at [ipoint];
				const double t = point -> number;
				if (t > tmin && t < tmax) {
					if (showBoundaries) {
						Graphics_setLineType (g, Graphics_DOTTED);
						Graphics_line (g, t, -1.0, t, 1.0);   // in sound part
						Graphics_setLineType (g, Graphics_DRAWN);
					}
					Graphics_line (g, t, ymin, t, 0.8 * ymin + 0.2 * ymax);
					Graphics_line (g, t, 0.2 * ymin + 0.8 * ymax, t, ymax);
					if (point -> mark)
						Graphics_text (g, t, 0.5 * (ymin + ymax), point -> mark.get());
				}
			}
		}
	}
	Graphics_setPercentSignIsItalic (g, true);
	Graphics_setNumberSignIsBold (g, true);
	Graphics_setCircumflexIsSuperscript (g, true);
	Graphics_setUnderscoreIsSubscript (g, true);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

autoSoundList TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			TextInterval segment = tier -> intervals.at [iseg];
			autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
			Thing_setName (interval.get(), segment -> text ? segment -> text.get() : U"untitled");
			list -> addItem_move (interval.move());
		}
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": intervals not extracted.");
	}
}

autoSoundList TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes) {
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			TextInterval segment = tier -> intervals.at [iseg];
			if (segment -> text && segment -> text [0] != U'\0') {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
				Thing_setName (interval.get(), segment -> text ? segment -> text.get() : U"untitled");
				list -> addItem_move (interval.move());
			}
		}
		if (list->size == 0)
			Melder_warning (U"No non-empty intervals were found.");
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": non-empty intervals not extracted.");
	}
}

autoSoundList TextGrid_Sound_extractIntervalsWhere (TextGrid me, Sound sound, integer tierNumber,
	kMelder_string which, conststring32 text, bool preserveTimes)
{
	try {
		IntervalTier tier = TextGrid_checkSpecifiedTierIsIntervalTier (me, tierNumber);
		autoSoundList list = SoundList_create ();
		/* mutable counter */ integer count = 0;
		for (integer iseg = 1; iseg <= tier -> intervals.size; iseg ++) {
			const TextInterval segment = tier -> intervals.at [iseg];
			if (Melder_stringMatchesCriterion (segment -> text.get(), which, text, true)) {
				autoSound interval = Sound_extractPart (sound, segment -> xmin, segment -> xmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
				Thing_setName (interval.get(), Melder_cat (sound -> name ? sound -> name.get() : U"", U"_", text, U"_", ++ count));
				list -> addItem_move (interval.move());
			}
		}
		if (list->size == 0)
			Melder_warning (U"No label that ", kMelder_string_getText (which), U" the text \"", text, U"\" was found.");
		return list;
	} catch (MelderError) {
		Melder_throw (me, U" & ", sound, U": intervals not extracted.");
	}
}

static void autoMarks (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	const double dy = ymax - ymin;
	if (dy < 26.0) {
		const integer imin = Melder_iroundUp ((ymin + 2.0) / 5.0), imax = Melder_ifloor ((ymax - 2.0) / 5.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 5.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 110.0) {
		const integer imin = Melder_iroundUp ((ymin + 8.0) / 20.0), imax = Melder_ifloor ((ymax - 8.0) / 20.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 20.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 260.0) {
		const integer imin = Melder_iroundUp ((ymin + 20.0) / 50.0), imax = Melder_ifloor ((ymax - 20.0) / 50.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 50.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 510.0) {
		const integer imin = Melder_iroundUp ((ymin + 40.0) / 100.0), imax = Melder_ifloor ((ymax - 40.0) / 100.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 100.0, true, true, haveDottedLines, nullptr);
	}
}

static void autoMarks_logarithmic (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	const double fy = ymax / ymin;
	for (int i = -12; i <= 12; i ++) {
		const double power = pow (10, i);
		/* mutable */ double y = power;
		if (y > ymin * 1.2 && y < ymax / 1.2)
			Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
		if (fy > 2100) {
			;   // enough
		} else if (fy > 210) {
			y = 3.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
		} else {
			y = 2.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			y = 5.0 * power;
			if (y > ymin * 1.2 && y < ymax / 1.2)
				Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			if (fy < 21) {
				y = 3.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
				y = 7.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			}
			if (fy < 4.1) {
				y = 1.5 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
				y = 4.0 * power;
				if (y > ymin * 1.2 && y < ymax / 1.2)
					Graphics_markLeftLogarithmic (g, y, true, true, haveDottedLines, nullptr);
			}
		}
	}
}

static void autoMarks_semitones (Graphics g, double ymin, double ymax, bool haveDottedLines) {
	const double dy = ymax - ymin;
	if (dy < 16.0) {
		const integer imin = Melder_iroundUp ((ymin + 1.2) / 3.0), imax = Melder_ifloor ((ymax - 1.2) / 3.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 3.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 32.0) {
		const integer imin = Melder_iroundUp ((ymin + 2.4) / 6.0), imax = Melder_ifloor ((ymax - 2.4) / 6.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 6.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 64.0) {
		const integer imin = Melder_iroundUp ((ymin + 4.8) / 12.0), imax = Melder_ifloor ((ymax - 4.8) / 12.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 12.0, true, true, haveDottedLines, nullptr);
	} else if (dy < 128.0) {
		const integer imin = Melder_iroundUp ((ymin + 9.6) / 24.0), imax = Melder_ifloor ((ymax - 9.6) / 24.0);
		for (integer i = imin; i <= imax; i ++)
			Graphics_markLeft (g, i * 24.0, true, true, haveDottedLines, nullptr);
	}
}

void TextGrid_Pitch_drawSeparately (
	const TextGrid grid, const Pitch pitch, const Graphics g,
	/* mutable autowindow */ double tmin, /* mutable autowindow */ double tmax,
	/* mutable conversion */ double fmin, /* mutable conversion */ double fmax,
	const bool showBoundaries, const bool useTextStyles, const bool garnish, const bool speckle, const kPitch_unit unit
) {
	const integer numberOfTiers = grid -> tiers->size;
	Function_unidirectionalAutowindow (grid, & tmin, & tmax);
	if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, (int) unit)) {
		fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, (int) unit);
		fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, (int) unit);
	}
	if (unit == kPitch_unit::HERTZ_LOGARITHMIC)
		Pitch_draw (pitch, g, tmin, tmax, pow (10.0, fmin - 0.25 * (fmax - fmin) * numberOfTiers), pow (10.0, fmax), false, speckle, unit);
	else
		Pitch_draw (pitch, g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * numberOfTiers, fmax, false, speckle, unit);
	TextGrid_Sound_draw (grid, nullptr, g, tmin, tmax, showBoundaries, useTextStyles, false);
	/*
		Restore window for the sake of margin drawing.
	*/
	Graphics_setWindow (g, tmin, tmax, fmin - 0.25 * (fmax - fmin) * numberOfTiers, fmax);
	if (unit == kPitch_unit::HERTZ_LOGARITHMIC)
		fmin = pow (10, fmin), fmax = pow (10.0, fmax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		if (unit == kPitch_unit::HERTZ_LOGARITHMIC) {
			Graphics_markLeftLogarithmic (g, fmin, true, true, false, nullptr);
			Graphics_markLeftLogarithmic (g, fmax, true, true, false, nullptr);
			autoMarks_logarithmic (g, fmin, fmax, false);
		} else if (unit == kPitch_unit::SEMITONES_100) {
			Graphics_markLeft (g, fmin, true, true, false, nullptr);
			Graphics_markLeft (g, fmax, true, true, false, nullptr);
			autoMarks_semitones (g, fmin, fmax, false);
		} else {
			Graphics_markLeft (g, fmin, true, true, false, nullptr);
			Graphics_markLeft (g, fmax, true, true, false, nullptr);
			autoMarks (g, fmin, fmax, false);
		}
		Graphics_textLeft (g, true, Melder_cat (U"Pitch (", Function_getUnitText (pitch, Pitch_LEVEL_FREQUENCY, (int) unit, Function_UNIT_TEXT_GRAPHICAL), U")"));
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

void TextGrid_Pitch_draw (
	const TextGrid grid, const Pitch pitch, const Graphics g,
	const integer tierNumber,
	/* mutable autowindow */ double tmin, /* mutable autowindow */ double tmax,
	/* mutable conversion */ double fmin, /* mutable conversion */ double fmax,
	const double fontSize, const bool useTextStyles, const int horizontalAlignment, const bool garnish, const bool speckle, const kPitch_unit unit
) {
	try {
		Function anyTier = TextGrid_checkSpecifiedTierNumberWithinRange (grid, tierNumber);
		const double oldFontSize = Graphics_inqFontSize (g);
		Pitch_draw (pitch, g, tmin, tmax, fmin, fmax, garnish, speckle, unit);
		Function_unidirectionalAutowindow (grid, & tmin, & tmax);
		autoPitchTier pitchTier = Pitch_to_PitchTier (pitch);
		if (Function_isUnitLogarithmic (pitch, Pitch_LEVEL_FREQUENCY, (int) unit)) {
			fmin = Function_convertStandardToSpecialUnit (pitch, fmin, Pitch_LEVEL_FREQUENCY, (int) unit);
			fmax = Function_convertStandardToSpecialUnit (pitch, fmax, Pitch_LEVEL_FREQUENCY, (int) unit);
		}
		Graphics_setTextAlignment (g, (kGraphics_horizontalAlignment) horizontalAlignment, Graphics_BOTTOM);
		Graphics_setInner (g);
		Graphics_setFontSize (g, fontSize);
		Graphics_setPercentSignIsItalic (g, useTextStyles);
		Graphics_setNumberSignIsBold (g, useTextStyles);
		Graphics_setCircumflexIsSuperscript (g, useTextStyles);
		Graphics_setUnderscoreIsSubscript (g, useTextStyles);
		if (anyTier -> classInfo == classIntervalTier) {
			const IntervalTier tier = static_cast <IntervalTier> (anyTier);
			for (integer i = 1; i <= tier -> intervals.size; i ++) {
				TextInterval interval = tier -> intervals.at [i];
				if (! interval -> text || ! interval -> text [0])
					continue;
				const double tleft = Melder_clippedLeft (pitch -> xmin, interval -> xmin);
				const double tright = Melder_clippedRight (interval -> xmax, pitch -> xmax);
				const double tmid = 0.5 * (tleft + tright);
				if (tmid < tmin || tmid > tmax)
					continue;
				const double f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.get(), tmid), Pitch_LEVEL_FREQUENCY, (int) unit);
				if (f0 < fmin || f0 > fmax)
					continue;
				Graphics_text (g,
					horizontalAlignment == (int) Graphics_LEFT ? tleft : horizontalAlignment == (int) Graphics_RIGHT ? tright : tmid,
					f0, interval -> text.get()
				);
			}
		} else {
			const TextTier tier = static_cast <TextTier> (anyTier);
			for (integer i = 1; i <= tier -> points.size; i ++) {
				TextPoint point = tier -> points.at [i];
				const double t = point -> number;
				if (! point -> mark || ! point -> mark [0])
					continue;
				if (t < tmin || t > tmax)
					continue;
				const double f0 = Function_convertStandardToSpecialUnit (pitch, RealTier_getValueAtTime (pitchTier.get(), t), Pitch_LEVEL_FREQUENCY, (int) unit);
				if (f0 < fmin || f0 > fmax)
					continue;
				Graphics_text (g, t, f0, point -> mark.get());
			}
		}
		Graphics_setPercentSignIsItalic (g, true);
		Graphics_setNumberSignIsBold (g, true);
		Graphics_setCircumflexIsSuperscript (g, true);
		Graphics_setUnderscoreIsSubscript (g, true);
		Graphics_setFontSize (g, oldFontSize);
		Graphics_unsetInner (g);
	} catch (MelderError) {
		Melder_throw (grid, U" & ", pitch, U": not drawn.");
	}
}

autoSound Sound_readWithAdjacentAnnotationFiles_buckeye (conststring32 soundFileName, autoTextGrid *out_textgrid) {
	try {
		structMelderFile file { };
		Melder_pathToFile (soundFileName, & file);
		char32 *lastPeriod = str32rchr (file.path, U'.');
		Melder_require (lastPeriod,
			U"Sound file name should have an extension, but is ", & file, U".");
		Melder_require (Melder_equ (lastPeriod, U".wav"),
			U"Sound file name should end in “.wav”, not in “", lastPeriod, U"”.");
		autoSound sound = Sound_readFromSoundFile (& file);
		OrderedOf <structTextGrid> textgrids;

		/*
			Read the .phones file.
		*/
		lastPeriod [1] = U'p';
		lastPeriod [2] = U'h';
		lastPeriod [3] = U'o';
		lastPeriod [4] = U'n';
		lastPeriod [5] = U'e';
		lastPeriod [6] = U's';
		lastPeriod [7] = U'\0';
		autoTextGrid phones = TextGrid_readFromEspsLabelFile (& file, false, 2);
		Melder_assert (phones -> tiers->size == 2);
		Thing_setName (phones -> tiers->at [1], U"phon");
		Thing_setName (phones -> tiers->at [2], U"phon*");
		textgrids. addItem_ref (phones.get());

		/*
			Read the .words file.
		*/
		lastPeriod [1] = U'w';
		lastPeriod [2] = U'o';
		lastPeriod [3] = U'r';
		lastPeriod [4] = U'd';
		lastPeriod [5] = U's';
		lastPeriod [6] = U'\0';
		autoTextGrid words = TextGrid_readFromEspsLabelFile (& file, false, 4);
		Melder_assert (words -> tiers->size == 4);
		Thing_setName (words -> tiers->at [1], U"words");
		Thing_setName (words -> tiers->at [2], U"dict");
		Thing_setName (words -> tiers->at [3], U"trans");
		Thing_setName (words -> tiers->at [4], U"pos");
		textgrids. addItem_ref (words.get());

		/*
			Read the .log file.
		*/
		lastPeriod [1] = U'l';
		lastPeriod [2] = U'o';
		lastPeriod [3] = U'g';
		lastPeriod [4] = U'\0';
		autoTextGrid log = TextGrid_readFromEspsLabelFile (& file, true, 1);
		Thing_setName (log -> tiers->at [1], U"log");
		textgrids. addItem_ref (log.get());

		*out_textgrid = TextGrids_merge (& textgrids, true);
		Melder_assert ((*out_textgrid) -> tiers->size == 7);
		lastPeriod [0] = U'\0';
		Thing_setName (sound.get(), MelderFile_name (& file));
		Thing_setName ((*out_textgrid).get(), MelderFile_name (& file));

		return sound;
	} catch (MelderError) {
		Melder_throw (U"Sound “", soundFileName, U"” not read with adjacent Buckeye annotation files.");
	}
}

autoSound Sound_readWithAdjacentAnnotationFiles_timit (conststring32 soundFileName, autoTextGrid *out_textgrid) {
	try {
		structMelderFile file { };
		Melder_pathToFile (soundFileName, & file);
		char32 *lastPeriod = str32rchr (file.path, U'.');
		Melder_require (lastPeriod,
			U"Sound file name should have an extension, but is ", & file, U".");
		Melder_require (Melder_equ (lastPeriod, U".wav"),
			U"Sound file name should end in “.wav”, not in “", lastPeriod, U"”.");
		autoSound sound = Sound_readFromSoundFile (& file);
		OrderedOf <structTextGrid> textgrids;

		/*
			Read the .phn file.
		*/
		lastPeriod [1] = U'p';
		lastPeriod [2] = U'h';
		lastPeriod [3] = U'n';
		autoTextGrid phones = TextGrid_readFromTimitLabelFile (& file, true);
		textgrids. addItem_ref (phones.get());

		/*
			Read the .wrd file.
		*/
		lastPeriod [1] = U'w';
		lastPeriod [2] = U'r';
		lastPeriod [3] = U'd';
		autoTextGrid words = TextGrid_readFromTimitLabelFile (& file, false);
		textgrids. addItem_ref (words.get());

		/*
			Read the .txt file.
		*/
		lastPeriod [1] = U't';
		lastPeriod [2] = U'x';
		lastPeriod [3] = U't';
		autoTextGrid text = TextGrid_readFromTimitLabelFile (& file, false);
		textgrids. addItem_ref (text.get());

		*out_textgrid = TextGrids_merge (& textgrids, true);
		Thing_setName ((*out_textgrid) -> tiers->at [1], U"phon-ipa");
		Thing_setName ((*out_textgrid) -> tiers->at [2], U"phon-arpa");
		Thing_setName ((*out_textgrid) -> tiers->at [3], U"words");
		Thing_setName ((*out_textgrid) -> tiers->at [4], U"text");
		return sound;
	} catch (MelderError) {
		Melder_throw (U"Sound “", soundFileName, U"” not read with adjacent TIMIT annotation files.");
	}
}

/* End of file TextGrid_Sound.cpp */
