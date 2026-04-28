/* SpeechSynthesizer.cpp
 *
 * Copyright (C) 2011-2023 David Weenink, 2012,2013,2015-2026 Paul Boersma
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

#include "espeak_ng.h"
#include "espeak_praat.h"    // for theEspeakPraatFileInMemorySet

#include "SpeechSynthesizer.h"
#include "Strings_extensions.h"
#include "speak_lib.h"
#include "synthdata.h"
#include "encoding.h"
#include "dictionary.h"
#include "translate.h"
#include "voice.h"

#include "oo_DESTROY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_COPY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_EQUAL.h"
#include "SpeechSynthesizer_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SpeechSynthesizer_def.h"
#include "oo_WRITE_TEXT.h"
#include "SpeechSynthesizer_def.h"
#include "oo_WRITE_BINARY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_READ_TEXT.h"
#include "SpeechSynthesizer_def.h"
#include "oo_READ_BINARY.h"
#include "SpeechSynthesizer_def.h"
#include "oo_DESCRIPTION.h"
#include "SpeechSynthesizer_def.h"

#include "UnicodeData.h"

extern int option_phoneme_events;   // BUG: external declaration outside header file (ppgb 20210307)

static autostring8 ipa_to_kirshenbaum (conststring32 text) {
	const struct ipaksymbol {
		char32 ipa; 
		char32 kirshenbaum;
	} ipaksymbols [] = {
		{UNICODE_LATIN_SMALL_LETTER_A, U'a'},					// a
		{UNICODE_LATIN_SMALL_LETTER_B, U'b'},					// b
		{UNICODE_LATIN_SMALL_LETTER_C, U'c'},					// c
		{UNICODE_LATIN_SMALL_LETTER_D, U'd'},					// d
		{UNICODE_LATIN_SMALL_LETTER_E, U'e'},					// e
		{UNICODE_LATIN_SMALL_LETTER_F, U'f'},					// f
		{UNICODE_LATIN_SMALL_LETTER_G, U'g'},					// g
		{UNICODE_LATIN_SMALL_LETTER_SCRIPT_G, U'g'},			// ɡ
		{UNICODE_LATIN_SMALL_LETTER_H, U'h'},					// h
		{UNICODE_LATIN_SMALL_LETTER_I, U'i'},					// i
		{UNICODE_LATIN_SMALL_LETTER_J, U'j'},					// j
		{UNICODE_LATIN_SMALL_LETTER_K, U'k'},					// k
		{UNICODE_LATIN_SMALL_LETTER_L, U'l'},					// l
		{UNICODE_LATIN_SMALL_LETTER_M, U'm'},					// m
		{UNICODE_LATIN_SMALL_LETTER_N, U'n'},					// n
		{UNICODE_LATIN_SMALL_LETTER_O, U'o'},					// o
		{UNICODE_LATIN_SMALL_LETTER_P, U'p'},					// p
		{UNICODE_LATIN_SMALL_LETTER_Q, U'q'},					// q
		{UNICODE_LATIN_SMALL_LETTER_TURNED_R, U'r'},			// ɹ
		{UNICODE_LATIN_SMALL_LETTER_S, U's'},					// s
		{UNICODE_LATIN_SMALL_LETTER_T, U't'},					// t
		{UNICODE_LATIN_SMALL_LETTER_U, U'u'},					// u
		{UNICODE_LATIN_SMALL_LETTER_V, U'v'},					// v
		{UNICODE_LATIN_SMALL_LETTER_W, U'w'},					// w
		{UNICODE_LATIN_SMALL_LETTER_X, U'x'},					// x
		{UNICODE_LATIN_SMALL_LETTER_Y, U'y'},					// y
		{UNICODE_LATIN_SMALL_LETTER_Z, U'z'},					// z
		{UNICODE_LATIN_SMALL_LETTER_ALPHA, U'A'},				// ɑ
		{UNICODE_GREEK_SMALL_LETTER_BETA, U'B'},				// β
		{UNICODE_LATIN_SMALL_LETTER_C_WITH_CEDILLA, U'C'},		// ç
		{UNICODE_LATIN_SMALL_LETTER_ETH, U'D'},					// ð
		{UNICODE_LATIN_SMALL_LETTER_OPEN_E, U'E'},				// ɛ
		// F unused
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_G, U'G'},			// ɢ
		{UNICODE_LATIN_SMALL_LETTER_H_WITH_STROKE, U'H'},		// ħ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_I, U'I'},			// ɪ
		{UNICODE_LATIN_SMALL_LETTER_IOTA, U'I'},				// ɩ
		{UNICODE_LATIN_SMALL_LETTER_DOTLESS_J_WITH_STROKE, U'J'},// ɟ
		// K unused
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_MIDDLE_TILDE, U'L'},	// ɫ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_L, U'L'},			// ʟ
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_BELT, U'L'},			// ɬ
		{UNICODE_LATIN_SMALL_LETTER_M_WITH_HOOK, U'M'},			// ɱ
		{UNICODE_LATIN_SMALL_LETTER_ENG, U'N'},					// ŋ
		{UNICODE_LATIN_SMALL_LETTER_OPEN_O, U'O'},				// ɔ
		{UNICODE_GREEK_CAPITAL_LETTER_PHI, U'P'},				// Φ
		{UNICODE_LATIN_SMALL_LETTER_GAMMA, U'Q'},				// ɣ
		{UNICODE_LATIN_SMALL_LETTER_SCHWA_WITH_HOOK, U'R'},		// ɚ
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_R, U'R'},			// ʀ
		{UNICODE_LATIN_SMALL_LETTER_ESH, U'S'},					// ʃ
		{UNICODE_GREEK_SMALL_LETTER_THETA, U'T'},				// θ
		{UNICODE_LATIN_SMALL_LETTER_UPSILON, U'U'},				// ʊ
		{UNICODE_LATIN_SMALL_LETTER_CLOSED_OMEGA, U'U'},		// ɷ
		{UNICODE_LATIN_SMALL_LETTER_TURNED_V, U'V'},			// ʌ
		{UNICODE_LATIN_SMALL_LIGATURE_OE, U'W'},				// œ
		{UNICODE_GREEK_SMALL_LETTER_CHI, U'X'},					// χ
		{UNICODE_LATIN_SMALL_LETTER_O_WITH_STROKE, U'Y'},		// ø
		{UNICODE_LATIN_SMALL_LETTER_EZH, U'Z'},					// ʒ
		{UNICODE_LATIN_LETTER_GLOTTAL_STOP, U'?'},				// ʔ
		{UNICODE_LATIN_SMALL_LETTER_SCHWA, U'@'},				// ə
		{UNICODE_LATIN_SMALL_LETTER_AE, U'&'},					// æ
		{UNICODE_LATIN_SMALL_LETTER_R_WITH_FISHHOOK, U'*'}		// ɾ	
	};
	const struct ipaksymbol2 {
		char32 ipa; 
		char32 kirshenbaum[3];
	} ipaksymbols2 [] = {
		// alveoalars
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_B, U"b"},			// ʙ unsupported: b<trl>
		{UNICODE_LATIN_SMALL_LETTER_B_WITH_HOOK, U"b"},			// ɓ unsupported: b`
		{UNICODE_LATIN_SMALL_LETTER_D_WITH_HOOK, U"d"},			// ɗ unsupported: d`
		{UNICODE_LATIN_SMALL_LETTER_TURNED_T, U"t"},			// ʇ unsupported: t!
		{UNICODE_LATIN_SMALL_LETTER_L_WITH_BELT, U"s"},			// ɬ unsupported: s<lat>
		{UNICODE_LATIN_SMALL_LETTER_LEZH, U"z"},				// ɮ unsupported: z<lat>
		{UNICODE_LATIN_LETTER_SMALL_CAPITAL_R, U"r"},			// ʀ unsupported: r<trl>
		{UNICODE_LATIN_SMALL_LETTER_R_WITH_FISHHOOK, U"*"},		// ɾ 
		{UNICODE_LATIN_SMALL_LETTER_TURNED_R_WITH_LONG_LEG, U"*"},// ɺ unsupported: *<lat>
		{UNICODE_LATIN_LETTER_STRETCHED_C, U"c"},		// ʗ unsupported: c!
		{UNICODE_LATIN_LETTER_INVERTED_GLOTTAL_STOP, U"l"},		// ʖ unsupported: l!
/*		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
		{, U""},		//  
*/		
	};
	autostring8 result;
	return result;
	
}

Thing_implement (SpeechSynthesizer, Daata, 1);

void structSpeechSynthesizer :: v1_info () {
	SpeechSynthesizer_Parent :: v1_info ();
	MelderInfo_writeLine (U"Language: ", our d_languageName.get());
	MelderInfo_writeLine (U"Voice: ", our d_voiceName.get());
	MelderInfo_writeLine (U"Phoneme set: ", our d_phonemeSetName.get());
	MelderInfo_writeLine (U"Input text format: ", (our d_inputTextFormat == SpeechSynthesizer_INPUT_TEXTONLY ? U"text only" :
		d_inputTextFormat == SpeechSynthesizer_INPUT_PHONEMESONLY ? U"phonemes only" : U"tagged text"));
	MelderInfo_writeLine (U"Input phoneme coding: ", (our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" : U"???"));
	MelderInfo_writeLine (U"Sampling frequency: ", our d_samplingFrequency, U" Hz");
	MelderInfo_writeLine (U"Word gap: ", our d_wordGap, U" s");
	MelderInfo_writeLine (U"Pitch multiplier: ", our d_pitchAdjustment, U" (0.5-2.0)");
	MelderInfo_writeLine (U"Pitch range multiplier: ", our d_pitchRange, U" (0.0-2.0)");
	MelderInfo_writeLine (U"Speaking rate: ", our d_wordsPerMinute, U" words per minute",
		our d_estimateSpeechRate ? U" (but estimated from speech if possible)" : U" (fixed)");
	MelderInfo_writeLine (U"Output phoneme coding: ",
		our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM ? U"Kirshenbaum" :
		our d_inputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA ? U"IPA" : U"???"
	);
}

static int synthCallback (short *wav, int numsamples, espeak_EVENT *events)
{
	char phoneme_name [9];
	if (wav == nullptr)
		return 1;	
	/*
		It is essential that the SpeechSynthesizer is identified here by the user_data,
		because the espeakEVENT_LIST_TERMINATED event may still be accompanied by
		a piece of audio data!!
	*/
	SpeechSynthesizer me = (SpeechSynthesizer) (events -> user_data);
	while (events -> type != espeakEVENT_LIST_TERMINATED) {
		if (events -> type == espeakEVENT_SAMPLERATE) {
			my d_internalSamplingFrequency = events -> id.number;
		} else {
			/*
				my events = Table "time type type-t t-pos length a-pos sample id uniq";
									1    2    3      4      5     6     7     8   9
			*/
			Table_appendRow (my d_events.get());
			const integer irow = my d_events -> rows.size;
			const double time = events -> audio_position * 0.001;
			Table_setNumericValue (my d_events.get(), irow, 1, time);
			Table_setNumericValue (my d_events.get(), irow, 2, events -> type);
			// Column 3 will be filled afterwards
			Table_setNumericValue (my d_events.get(), irow, 4, events -> text_position);
			Table_setNumericValue (my d_events.get(), irow, 5, events -> length);
			Table_setNumericValue (my d_events.get(), irow, 6, events -> audio_position);
			Table_setNumericValue (my d_events.get(), irow, 7, events -> sample);
			if (events -> type == espeakEVENT_MARK || events -> type == espeakEVENT_PLAY) {
				Table_setStringValue (my d_events.get(), irow, 8, Melder_peek8to32_u (events -> id.name));
			} else if (events -> type == espeakEVENT_PHONEME) {
				memcpy (phoneme_name, events -> id.string, 8);
				phoneme_name [8] = 0;   // because id.string is not 0-terminated if 8 chars long
				Table_setStringValue (my d_events.get(), irow, 8, Melder_peek8to32_u (phoneme_name));
			} else if (events -> type == espeakEVENT_SAMPLERATE ||1) {
				Table_setNumericValue (my d_events.get(), irow, 8, events -> id.number);
			}
			Table_setNumericValue (my d_events.get(), irow, 9, events -> unique_identifier);
		}
		events++;
	}
	if (me) {
		my d_wav. resize (my d_numberOfSamples + numsamples);
		for (integer i = 1; i <= numsamples; i++)
			my d_wav [my d_numberOfSamples + i] = wav [i - 1];
		my d_numberOfSamples = my d_wav.size;   // maintain invariant, *after* copy of content
	}
	return 0;
}

static conststring32 SpeechSynthesizer_getLanguageCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (theSpeechSynthesizerLanguagePropertiesTable, 1, my d_languageName.get());
		Melder_require (irow != 0,
			U"Cannot find language \"", my d_languageName.get(), U"\".");
		return Table_getStringValue_a (theSpeechSynthesizerLanguagePropertiesTable, irow, 2);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find language code.");
	}
}

static conststring32 SpeechSynthesizer_getPhonemeSetCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (theSpeechSynthesizerLanguagePropertiesTable, 1, my d_phonemeSetName.get());
		Melder_require (irow != 0,
			U"Cannot find phoneme set \"", my d_phonemeSetName.get(), U"\".");
		return Table_getStringValue_a (theSpeechSynthesizerLanguagePropertiesTable, irow, 2);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find phoneme set code.");
	}
}

static conststring32 SpeechSynthesizer_getVoiceCode (SpeechSynthesizer me) {
	try {
		const integer irow = Table_searchColumn (theSpeechSynthesizerVoicePropertiesTable, 1, my d_voiceName.get());
		Melder_require (irow != 0,
			U": Cannot find voice \"", my d_voiceName.get(), U"\".");
		return Table_getStringValue_a (theSpeechSynthesizerVoicePropertiesTable, irow, 5);
	} catch (MelderError) {
		Melder_throw (me, U": Cannot find voice code.");
	}
}

autoSpeechSynthesizer SpeechSynthesizer_create (conststring32 languageName, conststring32 voiceName) {
	try {
		autoSpeechSynthesizer me = Thing_new (SpeechSynthesizer);

		my d_languageName = Melder_dup (languageName);
		my d_voiceName = Melder_dup (voiceName);
		my d_phonemeSetName = Melder_dup (languageName);
		SpeechSynthesizer_checkAndRepairLanguageAndVoiceNames (me.get());

		SpeechSynthesizer_setTextInputSettings (me.get(), SpeechSynthesizer_INPUT_TAGGEDTEXT, SpeechSynthesizer_PHONEMECODINGS_KIRSHENBAUM);
		SpeechSynthesizer_setSpeechOutputSettings (me.get(), 44100.0, 0.01, 1.0, 1.0, 175.0, SpeechSynthesizer_PHONEMECODINGS_IPA);
		SpeechSynthesizer_setEstimateSpeechRateFromSpeech (me.get(), true);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SpeechSynthesizer not created.");
	}
}

void SpeechSynthesizer_setTextInputSettings (SpeechSynthesizer me, int inputTextFormat, int inputPhonemeCoding) {
	my d_inputTextFormat = inputTextFormat;
	my d_inputPhonemeCoding = inputPhonemeCoding;
}

void SpeechSynthesizer_setEstimateSpeechRateFromSpeech (SpeechSynthesizer me, bool estimate) {
	my d_estimateSpeechRate = estimate;
}

void SpeechSynthesizer_setSpeechOutputSettings (SpeechSynthesizer me, double samplingFrequency, double wordGap, double pitchAdjustment, double pitchRange, double wordsPerMinute, int outputPhonemeCoding) {
	my d_samplingFrequency = samplingFrequency;
	my d_wordGap = wordGap;
	my d_pitchAdjustment = Melder_clipped (0.5, pitchAdjustment, 2.0);
	my d_pitchRange = Melder_clipped (0.0, pitchRange, 2.0);
	if (wordsPerMinute <= 0.0)
		wordsPerMinute = 175.0;   // SMELL: looks like an arbitrary default
	my d_wordsPerMinute = Melder_clipped (80.0, wordsPerMinute, 450.0);
	my d_outputPhonemeCoding = outputPhonemeCoding;
}

void SpeechSynthesizer_playText (SpeechSynthesizer me, conststring32 text) {
	autoSound thee = SpeechSynthesizer_to_Sound (me, text, nullptr, nullptr);
	Sound_play (thee.get(), nullptr, nullptr);
}

static autoSound buffer_to_Sound (constINTVEC const& wav, double samplingFrequency)
{
	try {
		const double dx = 1.0 / samplingFrequency;
		const double xmax = wav.size * dx;
		autoSound thee = Sound_create (1, 0.0, xmax, wav.size, dx, dx / 2.0);
		for (integer i = 1; i <= wav.size; i++)
			thy z [1] [i] = wav [i] / 32768.0;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from synthesizer data.");
	}
}

static void IntervalTier_addBoundaryUnsorted (IntervalTier me, integer iinterval, double time, conststring32 newLabel, bool isNewleftLabel) {
	//TRACE
	trace (U"size before: ", my intervals.size);
	for (integer i = 1; i <= my intervals.size; i ++)
		trace (U"phoneme ", i, U" ", my intervals.at [i] -> xmin, U" ", my intervals.at [i] -> xmax,
				U" ", Melder_length (my intervals.at [i] -> text.get()), U" ", my intervals.at [i] -> text.get());

	trace (U"isNewLeftLabel: ", isNewleftLabel);
	trace (U"iinterval: ", iinterval);
	Melder_require (time > my xmin && time < my xmax,
		U"The time (", time, U" seconds) is not inside the domain of the interval tier (", my xmin, U" .. ", my xmax, U" seconds).");
	/*
		Find interval to split
	*/
	if (iinterval <= 0)
		iinterval = IntervalTier_timeToLowIndex (me, time);
	/*
		Modify end time of left label
	*/
	const TextInterval ti = my intervals.at [iinterval];
	trace (U"end time ", ti -> xmax, U" overridden by ", time);
	ti -> xmax = time;
	if (isNewleftLabel) {
		trace (U"label <<", ti -> text.get(), U">> overridden by ", newLabel);
		TextInterval_setText (ti, newLabel);
	}
	autoTextInterval ti_new = TextInterval_create (time, my xmax, (! isNewleftLabel ? newLabel : U"" ));
	my intervals. addItem_unsorted_move (ti_new.move());

	trace (U"size after: ", my intervals.size);
	for (integer i = 1; i <= my intervals.size; i ++)
		trace (U"phoneme ", i, U" ", my intervals.at [i] -> xmin, U" ", my intervals.at [i] -> xmax,
				U" ", Melder_length (my intervals.at [i] -> text.get()), U" ", my intervals.at [i] -> text.get());
}

static void Table_setEventTypeString (Table me) {
	try {
		for (integer i = 1; i <= my rows.size; i ++) {
			const int type = Table_getNumericValue_a (me, i, 2);
			conststring32 label = U"0";
			if (type == espeakEVENT_WORD)
				label = U"word";
			else if (type == espeakEVENT_SENTENCE)
				label = U"sent";
			else if (type == espeakEVENT_MARK)
				label = U"mark";
			else if (type == espeakEVENT_PLAY)
				label = U"play";
			else if (type == espeakEVENT_END)
				label = U"s-end";
			else if (type == espeakEVENT_MSG_TERMINATED)
				label = U"msg_term";
			else if (type == espeakEVENT_PHONEME)
				label = U"phoneme";
			Table_setStringValue (me, i, 3, label);
		}
	} catch (MelderError) {
		Melder_throw (U"Event types not set.");
	}
}

static void MelderString_trimWhiteSpaceAtEnd (MelderString *me) {
	while (my length > 1 && (my string [my length - 1] == U' ' || my string [my length - 1] == U'\t'
		|| my string [my length - 1] == U'\r' || my string [my length - 1] == U'\n'))
			my string [-- my length] = U'\0';
}

static void IntervalTier_mergeSpecialIntervals (IntervalTier me) {
	integer intervalIndex = my intervals.size;
	TextInterval right = my intervals.at [intervalIndex];
	const integer labelLength_right = TextInterval_labelLength (right);
	bool isEmptyInterval_right = labelLength_right == 0 || (labelLength_right == 1 && Melder_equ (right -> text.get(), U"\001"));
	while (intervalIndex > 1) {
		const TextInterval left = my intervals.at [intervalIndex - 1];
		const integer labelLength_left = TextInterval_labelLength (left);
		const bool isEmptyInterval_left = labelLength_left == 0 || (labelLength_left == 1 && Melder_equ (left -> text.get(), U"\001"));
		if (isEmptyInterval_right && isEmptyInterval_left) {
			/*
				Remove right interval and empty left interval
			*/
			left -> xmax = right -> xmax;
			TextInterval_setText (left, U""); 
			my intervals. removeItem (intervalIndex);
		}
		right = left;
		isEmptyInterval_right = isEmptyInterval_left;
		intervalIndex --;
	}
}

#if 0   // BUG unused (ppgb 20210307)
/* insert boundary at time t and merge/delete intervals after this time */
static void IntervalTier_insertBoundaryAndMergeIntervalsAfter (IntervalTier me, double t) {
	if (t <= my xmin || t >= my xmax)
		return;
	
	const integer intervalNumber = IntervalTier_timeToLowIndex (me, t);
	while (my intervals.size > intervalNumber + 1)
		my intervals. removeItem (my intervals.size);

	// there can be maximally one interval left to the right of intervalNumber
	const TextInterval ti = my intervals.at [intervalNumber];
	if (ti -> xmin == t) {   // if t happens to be on a boundary: remove the next interval if it exists
		if (my intervals.size > intervalNumber)
			my intervals. removeItem (my intervals .size);
		ti -> xmax = my xmax;
		TextInterval_setText (ti, U"");
	} else {
		ti -> xmax = t;
		TextInterval last = my intervals.at [my intervals.size];
		last -> xmin = t;
		last -> xmax = my xmax;
		TextInterval_setText (last, U"");
	}
}
#endif

static bool almost_equal (double t1, double t2) {
	// the "=" sign is essential for a difference of zero if t1 == 0
	return fabs (t1 - t2) <= 1e-12 * fabs (t1);
}

static void IntervalTier_insertEmptyIntervalsFromOtherTier (IntervalTier to, IntervalTier from) {
	for (integer iint = 1; iint <= from -> intervals.size; iint ++) {
		const TextInterval tifrom = from -> intervals.at [iint];
		if (TextInterval_labelLength (tifrom) == 0) {   // found empty interval
			const double t_left = tifrom -> xmin, t_right = tifrom -> xmax;
			integer intervalIndex_to = IntervalTier_timeToLowIndex (to, t_left);
			if (intervalIndex_to > 0) {   // insert to the right of intervalIndex_to
				const TextInterval tito = to -> intervals.at [intervalIndex_to];
				if (! almost_equal (tito -> xmin, t_left)) {   // not on the start boundary of the interval, it cannot be at xmax
					autoTextInterval newInterval = TextInterval_create (t_left, tito -> xmax, U"");
					tito -> xmax = t_left;
					to -> intervals. addItem_move (newInterval.move());
				}
			}
			intervalIndex_to = IntervalTier_timeToHighIndex (to, t_right);
			const TextInterval tito = to -> intervals.at [intervalIndex_to];
			if (intervalIndex_to > 0) {
				if (! almost_equal (t_right, tito -> xmax)) {   // insert to the left of intervalIndex_to
					autoTextInterval newInterval = TextInterval_create (tito -> xmin, t_right, U"");
					tito -> xmin = t_right;
					to -> intervals. addItem_move (newInterval.move());
				}
			}
		}
	}
}

static void IntervalTier_removeVeryShortIntervals (IntervalTier me) {
	integer iint = 1;
	while (iint <= my intervals.size) {
		const TextInterval ti = my intervals.at [iint];
		if (almost_equal (ti -> xmin, ti -> xmax))
			my intervals. removeItem (iint);
		else
			iint ++;
	}
}

static autoTextGrid Table_to_TextGrid (Table me, conststring32 text, double xmin, double xmax) {
	//Table_createWithColumnNames (0, L"time type type-t t-pos length a-pos sample id uniq");
	//TRACE
	if (Melder_isTracingLocally)
		Table_list (me, false);
	try {
		const integer textLength = Melder_length (text);
		const integer numberOfRows = my rows.size;
		const integer timeColumnIndex = Table_columnNameToNumber_e (me, U"time");
		const integer typeColumnIndex = Table_columnNameToNumber_e (me, U"type");
		const integer tposColumnIndex = Table_columnNameToNumber_e (me, U"t-pos");
		const integer   idColumnIndex = Table_columnNameToNumber_e (me, U"id");
		autoTextGrid thee = TextGrid_create (xmin, xmax, U"sentence clause word phoneme", U"");

		TextGrid_setIntervalText (thee.get(), 1, 1, text);

		integer p1c = 1, p1w = 1;
		double time_phon_p = xmin;
		bool wordEnd = false;
		autoMelderString mark;

		const IntervalTier clauses = (IntervalTier) thy tiers->at [2];
		const IntervalTier words = (IntervalTier) thy tiers->at [3];
		const IntervalTier phonemes = (IntervalTier) thy tiers->at [4];
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			/*const*/ double time = Table_getNumericValue_a (me, irow, timeColumnIndex);
			const int type = Table_getNumericValue_a (me, irow, typeColumnIndex);
			const integer pos = Table_getNumericValue_a (me, irow, tposColumnIndex);
			integer length;
			if (type == espeakEVENT_SENTENCE) {
				/*
					Insert only a new boundary, no text
					(text will be inserted at an "end sentence" event).
				*/
				if (time > xmin && time < xmax)
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, U"", true);
				p1c = pos;
			} else if (type == espeakEVENT_END) {
				/*
					End of clause: insert new boundary, and fill left interval with text.
				*/
				length = pos - p1c + 1;
				MelderString_ncopy (&mark, text + p1c - 1, length);
				MelderString_trimWhiteSpaceAtEnd (& mark);
				if (time > xmin && time < xmax)
					IntervalTier_addBoundaryUnsorted (clauses, clauses -> intervals.size, time, mark.string, true);
				else
					TextGrid_setIntervalText (thee.get(), 2, clauses -> intervals.size, mark.string);
				p1c = pos;
				/*
					End of clause always signals "end of a word".
				*/
				if (pos <= textLength) {
					length = pos - p1w + 1;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					if (time > xmin && time < xmax)
						IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, mark.string, true);
					else
						TextGrid_setIntervalText (thee.get(), 3, words -> intervals.size, mark.string);
					/*
						Now the next word event should not trigger setting the left interval text.
					*/
					wordEnd = false;
				}
			} else if (type == espeakEVENT_WORD) {
				if (pos < p1w)
					continue;
				if (time > xmin && time < xmax) {
					length = pos - p1w;
					if (pos == textLength)
						length++;
					MelderString_ncopy (&mark, text + p1w - 1, length);
					MelderString_trimWhiteSpaceAtEnd (& mark);
					IntervalTier_addBoundaryUnsorted (words, words -> intervals.size, time, ( wordEnd ? mark.string : U"" ), true);
				}
				wordEnd = true;
				p1w = pos;
			} else if (type == espeakEVENT_PHONEME) {
				const conststring32 phoneme = Table_getStringValue_a (me, irow, idColumnIndex);
				//TRACE
				trace (U"found in table the phoneme <<", phoneme, U">>, to be inserted at ", time, U" (usually after ", time_phon_p, U")");
				const TextInterval lastPhonemeInterval = phonemes -> intervals.at [phonemes -> intervals.size];
				if (false) {
					if (time > time_phon_p) {
						/*
							Insert new boundary and label interval with the phoneme.
							TODO: Translate the phoneme to the correct notation
						*/
						if (time > lastPhonemeInterval -> xmin && time < lastPhonemeInterval -> xmax) {
							trace (U"add boundary (1) for phoneme <<", phoneme, U">> at ", time);
							IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
						}
					} else {
						/*
							Just in case the phoneme starts at xmin, we only need to set interval text.
						*/
						trace (U"overriding <<", lastPhonemeInterval -> text.get(), U">> with <<", phoneme, U">>");
						TextGrid_setIntervalText (thee.get(), 4, phonemes -> intervals.size, phoneme);
					}
				} else {
					if (time > time_phon_p) {
						/*
							Insert new boundary and label interval with the phoneme.
							TODO: Translate the phoneme to the correct notation
						*/
						if (time > lastPhonemeInterval -> xmin && time < lastPhonemeInterval -> xmax) {
							trace (U"add boundary (1) for phoneme <<", phoneme, U">> at ", time);
							IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
						}
					} else if (! Melder_equ (phoneme, U"")) {
						/*
							Just in case the phoneme starts at xmin, we only need to set interval text.
						*/
						trace (U"overriding <<", lastPhonemeInterval -> text.get(), U">> with <<", phoneme, U">>");
						TextGrid_setIntervalText (thee.get(), 4, phonemes -> intervals.size, phoneme);
					} else {
						//TRACE
						trace (U"stupid repair, houtjes-touwtjes");
						time = 0.001 * lastPhonemeInterval -> xmax + 0.999 * lastPhonemeInterval -> xmin;
						trace (U"add boundary (2) for phoneme <<", phoneme, U">> at ", time);
						IntervalTier_addBoundaryUnsorted (phonemes, phonemes -> intervals.size, time, phoneme, false);
					}
				}
				time_phon_p = time;
			}
		}
		clauses -> intervals. sort ();
		words -> intervals. sort ();
		phonemes -> intervals. sort ();
		
		IntervalTier_mergeSpecialIntervals (phonemes); // Merge neighbouring empty U"" and U"\001" intervals
		
		IntervalTier_removeVeryShortIntervals (words);
		IntervalTier_removeVeryShortIntervals (clauses);
		/*
			Use empty intervals in phoneme tier for more precision in the word tier.
		*/
		IntervalTier_insertEmptyIntervalsFromOtherTier (words, phonemes);
		IntervalTier_mergeSpecialIntervals (words); // Merge neighbouring empty U"" and U"\001" intervals

		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created from Table with events.");
	}
}

static void SpeechSynthesizer_generateSynthesisData (SpeechSynthesizer me, conststring32 text) {
	try {
		int synth_flags = 0;
		espeak_ng_InitializePath (nullptr); // PATH_ESPEAK_DATA
		espeak_ng_ERROR_CONTEXT context = { 0 };
		espeak_ng_STATUS status = espeak_ng_Initialize (& context);
		Melder_require (status == ENS_OK,
			U"Internal eSpeak error. ", status);
		if (my d_inputTextFormat == SpeechSynthesizer_INPUT_TAGGEDTEXT)
			synth_flags |= espeakSSML;
		if (my d_inputTextFormat != SpeechSynthesizer_INPUT_TEXTONLY)
			synth_flags |= espeakPHONEMES;
		option_phoneme_events = espeakINITIALIZE_PHONEME_EVENTS; // extern int option_phoneme_events;
		if (my d_outputPhonemeCoding == SpeechSynthesizer_PHONEMECODINGS_IPA)
			option_phoneme_events |= espeakINITIALIZE_PHONEME_IPA;

		espeak_ng_SetParameter (espeakRATE, my d_wordsPerMinute, 0);
		/*
			pitchAdjustment_0_99 = a * log10 (my d_pitchAdjustment) + b,
			where 0.5 <= my d_pitchAdjustment <= 2
			pitchRange_0_99 = my d_pitchRange * 49.5,
			where 0 <= my d_pitchRange <= 2
		*/
		const int pitchAdjustment_0_99 = (int) ((49.5 / NUMlog10_2) * log10 (my d_pitchAdjustment) + 49.5);   // rounded towards zero
		espeak_ng_SetParameter (espeakPITCH, pitchAdjustment_0_99, 0);
		const int pitchRange_0_99 = (int) (my d_pitchRange * 49.5);   // rounded towards zero
		espeak_ng_SetParameter (espeakRANGE, pitchRange_0_99, 0);
		const conststring32 languageCode = SpeechSynthesizer_getLanguageCode (me);
		const conststring32 voiceCode = SpeechSynthesizer_getVoiceCode (me);
		
		espeak_ng_SetVoiceByName (Melder_peek32to8 (Melder_cat (languageCode, U"+", voiceCode)));
		const int wordGap_10ms = my d_wordGap * 100;   // eSpeak word gap is in units of 10 ms
		espeak_ng_SetParameter (espeakWORDGAP, wordGap_10ms, 0);
		espeak_ng_SetParameter (espeakCAPITALS, 0, 0);
		espeak_ng_SetParameter (espeakPUNCTUATION, espeakPUNCT_NONE, 0);

		status = espeak_ng_InitializeOutput (ENOUTPUT_MODE_SYNCHRONOUS, 2048, nullptr);
		espeak_SetSynthCallback (synthCallback);
		if (! Melder_equ (my d_phonemeSetName.get(), my d_languageName.get())) {
			const conststring32 phonemeSetCode = SpeechSynthesizer_getPhonemeSetCode (me);
			const int index_phon_table_list = LookupPhonemeTable (Melder_peek32to8 (phonemeSetCode));
			if (index_phon_table_list > 0) {
				voice -> phoneme_tab_ix = index_phon_table_list;
				(void) DoVoiceChange(voice);
			}
		}

		const conststring32 columnNames [] =
				{ U"time", U"type", U"type-t", U"t-pos", U"length", U"a-pos", U"sample", U"id", U"uniq" };
		my d_events = Table_createWithColumnNames (0, ARRAY_TO_STRVEC (columnNames));
		unsigned int unique_identifier = 0;
		#ifdef _WIN32
			conststringW textW = Melder_peek32toW (text);
			synth_flags |= espeakCHARS_WCHAR;
			espeak_ng_Synthesize (textW, wcslen (textW) + 1, 0, POS_CHARACTER, 0, synth_flags, & unique_identifier, me);
		#else
			conststring8 textUTF8 = Melder_peek32to8 (text);
			synth_flags |= espeakCHARS_UTF8;
			espeak_ng_Synthesize (textUTF8, Melder_length_utf8 (text, false) + 1, 0, POS_CHARACTER, 0, synth_flags, & unique_identifier, me);
		#endif

		espeak_ng_Terminate ();
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw (U"SpeechSynthesizer: synthesis data not generated.");
	}	
}

autostring32 SpeechSynthesizer_getPhonemesFromText (SpeechSynthesizer me, conststring32 text, bool separateBySpaces) {
	try {
		/*
			Clear the wave buffer.
		*/
		my d_wav. resize (0);
		my d_numberOfSamples = my d_wav.size;   // maintain invariant

		SpeechSynthesizer_generateSynthesisData (me, text);
		const double dt = 1.0 / my d_internalSamplingFrequency;
		const double tmin = 0.0, tmax = my d_wav.size * dt;
		double xmin = Table_getNumericValue_a (my d_events.get(), 1, 1);
		Melder_clipLeft (tmin, & xmin);
		double xmax = Table_getNumericValue_a (my d_events.get(), my d_events -> rows.size, 1);
		Melder_clipRight (& xmax, tmax);
		autoTextGrid tg = Table_to_TextGrid (my d_events.get(), text, xmin, xmax);
		/*
			Get phonemes from the phoneme tier
		*/
		Melder_assert (tg -> tiers -> size == 4);
		IntervalTier phonemeTier = static_cast<IntervalTier> (tg -> tiers -> at [4]);
		const integer numberOfIntervals = phonemeTier -> intervals.size;
		Melder_require (numberOfIntervals > 0,
			U"There are no phonemes in the tier.");
		autoMelderString phonemes;
		const conststring32 phonemeSeparator = ( separateBySpaces ? U" " : U"" );
		const conststring32 wordSeparator = ( separateBySpaces ? U"  " : U" " );
		for (integer iint = 1; iint <= numberOfIntervals; iint ++) {
			TextInterval interval = phonemeTier -> intervals.at [iint];
			const conststring32 phonemeLabel = interval -> text.get();
			const bool isEmptyLabel = Melder_equ (phonemeLabel, U"");
			if (isEmptyLabel) {
				if (iint > 1 && iint < numberOfIntervals)
					MelderString_append (& phonemes, wordSeparator);
			} else
				MelderString_append (& phonemes, phonemeLabel, (iint < numberOfIntervals ? phonemeSeparator : U"") );
		}
		//my d_events. reset();   // ppgb: this was not in the code, but is this in fact superflous?
		return Melder_dup (phonemes.string);   // TODO: implement MelderString_move()
	} catch (MelderError) {
		Melder_throw (U"Phonemes not generated.");
	}
}

autoSound SpeechSynthesizer_to_Sound (SpeechSynthesizer me, conststring32 text, autoTextGrid *tg, autoTable *events) {
	try {
		/*
			Clear the wave buffer.
		*/
		my d_wav. resize (0);
		my d_numberOfSamples = my d_wav.size;   // maintain invariant

		SpeechSynthesizer_generateSynthesisData (me, text);
		autoSound thee = buffer_to_Sound (my d_wav.get(), my d_internalSamplingFrequency);

		if (my d_samplingFrequency != my d_internalSamplingFrequency)
			thee = Sound_resample (thee.get(), my d_samplingFrequency, 50);
		if (tg) {
			const double xmin = Melder_clippedRight (Table_getNumericValue_a (my d_events.get(), 1, 1), thy xmin);   // !
			const double xmax = Melder_clippedLeft (thy xmax, Table_getNumericValue_a (my d_events.get(), my d_events -> rows.size, 1));   // !
			autoTextGrid tg1 = Table_to_TextGrid (my d_events.get(), text, xmin, xmax);
			*tg = TextGrid_extractPart (tg1.get(), thy xmin, thy xmax, 0);
		}
		if (events) {
			Table_setEventTypeString (my d_events.get());
			*events = my d_events.move();
		}
		my d_events. reset();
		return thee;
	} catch (MelderError) {
		espeak_Terminate ();
		Melder_throw (U"SpeechSynthesizer: text not converted to Sound.");
	}
}

static conststring32 get_wordAfterPrecursor_u8 (constvector<unsigned char> const& text8, conststring32 precursor) {
	static char32 word [100];
	/*
		1. Find (first occurrence of) 'precursor' at the start of a line (with optional leading whitespace).
		2. Get the words after 'precursor' (skip leading and trailing whitespace).
	*/
	autoMelderString regex;
	const conststring32 text = Melder_peek8to32_u (reinterpret_cast <const char *> (text8.asArgumentToFunctionThatExpectsZeroBasedArray()));
	MelderString_append (& regex, U"^\\s*", precursor, U"\\s+");
	char32 *p = nullptr;
	const char32 *pmatch = strstr_regexp (text, regex.string);
	if (pmatch) {
		pmatch += Melder_length (precursor); // skip 'precursor'
		while (*pmatch == U' ' || *pmatch == U'\t')
			pmatch ++; // skip whitespace after 'precursor'
		p = word;
		char32 *p_end = p + 99;
		while ((*p = *pmatch ++) && *p != U' ' && *p != U'\t' && *p != U'\n' && *p != U'\r' && p < p_end)
			p ++;
		*p = U'\0';
		p = word;
	}
	return p;
}

static conststring32 get_stringAfterPrecursor_u8 (constvector<unsigned char> const& text8, conststring32 precursor) {
	static char32 word [100];
	/*
		1. Find (first occurrence of) 'precursor' at the start of a line (with optional leading whitespace).
		2. Get the words after 'precursor' (skip leading and trailing whitespace).
	*/
	autoMelderString regex;
	const conststring32 text = Melder_peek8to32_u (reinterpret_cast <const char *> (text8.asArgumentToFunctionThatExpectsZeroBasedArray()));
	//const conststring32 text = Melder_peek8to32_u ((const char *) & (text8.cells[1]));
	MelderString_append (& regex, U"^\\s*", precursor, U"\\s+");
	char32 *p = nullptr;
	const char32 *pmatch = strstr_regexp (text, regex.string);
	if (pmatch) {
		pmatch += Melder_length (precursor); // skip 'precursor'
		while (*pmatch == U' ' || *pmatch == U'\t')
			pmatch ++; // skip whitespace after 'precursor'
		//pmatch --;
		p = word;
		char32 *p_end = p + 99;
		// also discard text after comment '//'
		while ((*p = *pmatch ++) && *p != U'\n' && *p != U'\r' && *p != U'/' && *(p+1) != U'/' && p < p_end)
			p ++; // copy to end of line
		while (*p == U' ' || *p == U'\t' || *p == U'\n' || *p == U'\r')
			p --; // remove trailing white space
		*(++ p) = U'\0';
		p = word;
	}
	return p;
}

/*
	Four global singleton-references.
	The four singletons that they refer to contain data,
	are to be initialized once during start-up (i.e. during some `praat_xxx_init()`),
	and are kept throughout program execution.
*/
Table theSpeechSynthesizerVoicePropertiesTable;
Table theSpeechSynthesizerLanguagePropertiesTable;
STRVEC theSpeechSynthesizerLanguageNames;
STRVEC theSpeechSynthesizerVoiceNames;

void classSpeechSynthesizer_initClass () {
	try {
		/*
			Initialization is to take place only once.
		*/
		Melder_assert (! theSpeechSynthesizerVoicePropertiesTable);
		Melder_assert (! theSpeechSynthesizerLanguagePropertiesTable);
		Melder_assert (! theSpeechSynthesizerLanguageNames);
		Melder_assert (! theSpeechSynthesizerVoiceNames);

		(void) theEspeakPraatFileInMemorySet();   // create the singleton now (not strictly necessary)
		{// scope
			static autoTable me;   // singleton
			constexpr conststring32 criterion = U"./data/lang/";   // 12 characters
			constexpr integer criterionLength = Melder_length (criterion);
			FileInMemorySet they = theEspeakPraatFileInMemorySet();
			Melder_assert (they);
			const integer numberOfMatches = FileInMemorySet_findNumberOfMatches_path (they, kMelder_string :: CONTAINS, criterion);
			Melder_assert (numberOfMatches > 0);
			const conststring32 columnNames [] = { U"language name", U"ISO 639 family/language code", U"(internal index)" };
			me = Table_createWithColumnNames (numberOfMatches, ARRAY_TO_STRVEC (columnNames)); // old: Default English
			integer irow = 0;
			for (integer ifile = 1; ifile <= their size; ifile ++) {
				const FileInMemory fim = their at [ifile];
				if (Melder_stringMatchesCriterion (fim -> string.get(), kMelder_string :: CONTAINS, criterion, true)) {
					irow ++;
					const char32 *word = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"name");
					Table_setStringValue (me.get(), irow, 1, ( word ? word : & fim -> string [criterionLength] ));
					Table_setStringValue (me.get(), irow, 2, & fim -> string [criterionLength]);
					Table_setNumericValue (me.get(), irow, 3, ifile);
				}
			}
			Melder_assert (irow == numberOfMatches);
			const conststring32 sorters [] = { U"language name" };
			Table_sortRows (me.get(), ARRAY_TO_STRVEC (sorters));
			theSpeechSynthesizerLanguagePropertiesTable = me.get();
		}
		{// scope
			static autoTable me;   // singleton
			constexpr conststring32 criterion = U"/voices/!v/";
			FileInMemorySet they = theEspeakPraatFileInMemorySet();
			const integer numberOfMatches = FileInMemorySet_findNumberOfMatches_path (they, kMelder_string :: CONTAINS, criterion);
			const conststring32 columnNames [] = { U"voice name", U"gender", U"age", U"variant", U"(internal name)", U"(internal index)" };
			me = Table_createWithColumnNames (numberOfMatches, ARRAY_TO_STRVEC (columnNames));
			integer irow = 0;
			for (integer ifile = 1; ifile <= their size; ifile ++) {
				const FileInMemory fim = their at [ifile];
				if (Melder_stringMatchesCriterion (fim -> string.get(), kMelder_string :: CONTAINS, criterion, true)) {
					irow ++;
					const char32 *name = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"name");
					// The first character of name must be upper case
					if (name) {
						autoMelderString capitalFirst;
						MelderString_copy (& capitalFirst, name); // we cannot modify original
						capitalFirst.string [0] = Melder_toUpperCase (name [0]);
						Table_setStringValue (me.get(), irow, 1, capitalFirst.string);
					} else {
						Table_setStringValue (me.get(), irow, 1, str32rchr (fim -> string.get(), U'/') + 1);
					}
					conststring32 word = get_wordAfterPrecursor_u8 (fim -> d_data.get(), U"gender");
					Table_setStringValue (me.get(), irow, 2, (word ? word : U"0"));
					word = get_wordAfterPrecursor_u8 (fim -> d_data.get(), U"age");
					Table_setStringValue (me.get(), irow, 3, (word ? word : U"0"));
					word = get_stringAfterPrecursor_u8 (fim -> d_data.get(), U"variant");
					Table_setStringValue (me.get(), irow, 4, (word ? word : U"0"));
					Table_setStringValue (me.get(), irow, 5, str32rchr (fim -> string.get(), U'/') + 1);
					Table_setNumericValue (me.get(), irow, 6, ifile);
				}
			}
			Melder_assert (irow == numberOfMatches);
			const conststring32 sorters [] = { U"voice name" };
			Table_sortRows (me.get(), ARRAY_TO_STRVEC (sorters));
			theSpeechSynthesizerVoicePropertiesTable = me.get();
		}
		{// scope
			static autoSTRVEC list;   // singleton
			list = Table_getAllTextsInColumn (theSpeechSynthesizerLanguagePropertiesTable, 1);
			theSpeechSynthesizerLanguageNames = list.get();
		}
		{// scope
			static autoSTRVEC list;   // singleton
			list = Table_getAllTextsInColumn (theSpeechSynthesizerVoicePropertiesTable, 1);
			theSpeechSynthesizerVoiceNames = list.get();
		}
	} catch (MelderError) {
		Melder_crash (U"eSpeak-Praat initialization not performed.");
	}
}

void SpeechSynthesizer_checkAndRepairLanguageAndVoiceNames (SpeechSynthesizer me) {
	const bool languageAndPhonemeSetAreIdentical = Melder_equ (my d_languageName.get(), my d_phonemeSetName.get());

	/*
		Changes to the language name.
	*/
	{// scope
		bool languageNameHasBeenRepaired = false;
		integer languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_languageName.get());
		if (languageIndex == 0) {   // not found
			if (my d_languageName [0] >= U'a' && my d_languageName [0] <= U'z') {   // lower case?
				my d_languageName [0] -= 32;   // try upper case instead
				languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_languageName.get());
				if (languageIndex > 0) {   // FOUND
					languageNameHasBeenRepaired = true;
					if (languageAndPhonemeSetAreIdentical)
						my d_phonemeSetName [0] -= 32;   // make the same change to the phoneme set name as to the language name
				} else
					my d_languageName [0] += 32;   // revert to lower case
			} else if (my d_languageName [0] >= U'A' && my d_languageName [0] <= U'Z') {   // upper case?
				my d_languageName [0] += 32;   // try lower case instead
				languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_languageName.get());
				if (languageIndex > 0) {   // FOUND
					languageNameHasBeenRepaired = true;
					if (languageAndPhonemeSetAreIdentical)
						my d_phonemeSetName [0] += 32;   // make the same change to the phoneme set name as to the language name
				} else
					my d_languageName [0] -= 32;   // revert to upper case
			}
			if (! languageNameHasBeenRepaired) {   // the case change didn't help
				/*
					The user could have tried an eSpeak-internal family/language code, or just a language code without family.
				*/
				Table table = theSpeechSynthesizerLanguagePropertiesTable;
				for (integer irow = 1; irow <= table -> rows.size; irow ++) {
					conststring32 familyLanguageCode = Table_getStringValue_a (table, irow, 2);
					const char32 *slashPosition = str32rchr (familyLanguageCode, U'/');
					conststring32 languageCode = ( slashPosition ? slashPosition + 1 : familyLanguageCode );
					if (Melder_equ (my d_languageName.get(), familyLanguageCode) || Melder_equ (my d_languageName.get(), languageCode)) {   // FOUND
						my d_languageName = Melder_dup (Table_getStringValue_a (table, irow, 1));
						languageNameHasBeenRepaired = true;
						if (languageAndPhonemeSetAreIdentical)
							my d_phonemeSetName = Melder_dup (my d_languageName.get());
						break;
					}
				}
			}
			if (! languageNameHasBeenRepaired) {   // neither the case change nor the (family/)language code helped
				if (Melder_equ (my d_languageName.get(), U"Default") || Melder_equ (my d_languageName.get(), U"English")) {
					my d_languageName = Melder_dup (U"English (Great Britain)");
					languageNameHasBeenRepaired = true;
					if (languageAndPhonemeSetAreIdentical)
						my d_phonemeSetName = Melder_dup (my d_languageName.get());
				}
			}
			if (! languageNameHasBeenRepaired)   // nothing helped
				Melder_throw (U"Unknown language “", my d_languageName.get(), U"”.\n"
						"If you think that Praat should know this language, write to the authors (paul.boersma@uva.nl).");
		}
	}

	/*
		Changes to the voice name.
	*/
	{// scope
		bool voiceNameHasBeenRepaired = false;
		integer voiceIndex = NUMfindFirst (theSpeechSynthesizerVoiceNames, my d_voiceName.get());
		if (voiceIndex == 0) {   // not found
			if (my d_voiceName [0] >= U'a' && my d_voiceName [0] <= U'z') {   // lower case?
				my d_voiceName [0] -= 32;   // try upper case instead
				voiceIndex = NUMfindFirst (theSpeechSynthesizerVoiceNames, my d_voiceName.get());
				if (voiceIndex > 0) {   // FOUND
					voiceNameHasBeenRepaired = true;
				} else
					my d_voiceName [0] += 32;   // revert to lower case
			} else if (my d_voiceName [0] >= U'A' && my d_voiceName [0] <= U'Z') {   // upper case?
				my d_voiceName [0] += 32;   // try lower case instead
				voiceIndex = NUMfindFirst (theSpeechSynthesizerVoiceNames, my d_voiceName.get());
				if (voiceIndex > 0) {   // FOUND
					voiceNameHasBeenRepaired = true;
				} else
					my d_voiceName [0] -= 32;   // revert to upper case
			}
			if (! voiceNameHasBeenRepaired) {   // the case change didn't help
				if (Melder_equ (my d_voiceName.get(), U"default")) {
					my d_voiceName = Melder_dup (U"Male1");   // as in a very early version of eSpeak
					voiceNameHasBeenRepaired = true;
				} else if (my d_voiceName [0] == U'f' && my d_voiceName [1] >= U'1' && my d_voiceName [1] <= '5' && my d_voiceName [2] == U'\0') {
					my d_voiceName = Melder_dup (Melder_cat (U"Female", my d_voiceName [1]));   // e.g. change "f4" to "Female4"
					voiceNameHasBeenRepaired = true;
				}
			}
			if (! voiceNameHasBeenRepaired)   // nothing helped
				Melder_throw (U"Unknown voice “", my d_voiceName.get(), U"”.\n"
						"If you think that Praat should know this voice, write to the authors (paul.boersma@uva.nl).");
		}
	}

	/*
		If language name and phoneme set name were identical to start with,
		then any change in the language name have already been applied to the phoneme set name.
		So we have to change the phoneme set name only if it is independent from the language name.
	*/
	if (languageAndPhonemeSetAreIdentical)
		return;

	/*
		Changes to the phoneme set name.
	*/
	{// scope
		bool phonemeSetNameHasBeenRepaired = false;
		integer languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_phonemeSetName.get());
		if (languageIndex == 0) {   // not found
			if (my d_phonemeSetName [0] >= U'a' && my d_phonemeSetName [0] <= U'z') {   // lower case?
				my d_phonemeSetName [0] -= 32;   // try upper case instead
				languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_phonemeSetName.get());
				if (languageIndex > 0) {   // FOUND
					phonemeSetNameHasBeenRepaired = true;
				} else
					my d_phonemeSetName [0] += 32;   // revert to lower case
			} else if (my d_phonemeSetName [0] >= U'A' && my d_phonemeSetName [0] <= U'Z') {   // upper case?
				my d_phonemeSetName [0] += 32;   // try lower case instead
				languageIndex = NUMfindFirst (theSpeechSynthesizerLanguageNames, my d_phonemeSetName.get());
				if (languageIndex > 0) {   // FOUND
					phonemeSetNameHasBeenRepaired = true;
				} else
					my d_phonemeSetName [0] -= 32;   // revert to upper case
			}
			if (! phonemeSetNameHasBeenRepaired) {   // the case change didn't help
				/*
					The user could have tried an eSpeak-internal family/language code.
				*/
				Table table = theSpeechSynthesizerLanguagePropertiesTable;
				for (integer irow = 1; irow <= table -> rows.size; irow ++) {
					conststring32 familyLanguageCode = Table_getStringValue_a (table, irow, 2);
					const char32 *slashPosition = str32rchr (familyLanguageCode, U'/');
					conststring32 languageCode = ( slashPosition ? slashPosition + 1 : familyLanguageCode );
					if (Melder_equ (my d_phonemeSetName.get(), familyLanguageCode) || Melder_equ (my d_phonemeSetName.get(), languageCode)) {   // FOUND
						my d_phonemeSetName = Melder_dup (Table_getStringValue_a (table, irow, 1));
						phonemeSetNameHasBeenRepaired = true;
						break;
					}
				}
			}
			if (! phonemeSetNameHasBeenRepaired) {   // neither the case change nor the (family/)language code helped
				if (Melder_equ (my d_phonemeSetName.get(), U"Default") || Melder_equ (my d_phonemeSetName.get(), U"English")) {
					my d_phonemeSetName = Melder_dup (U"English (Great Britain)");
					phonemeSetNameHasBeenRepaired = true;
				}
			}
			if (! phonemeSetNameHasBeenRepaired)   // nothing helped
				Melder_throw (U"Unknown language “", my d_phonemeSetName.get(), U"”.\n"
						"If you think that Praat should know this language, write to the authors (paul.boersma@uva.nl).");
		}
	}
}

/* End of file SpeechSynthesizer.cpp */
