/* SpeechRecognizer.cpp
 *
 * Copyright (C) 2025,2026 Anastasia Shchupak
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

#include "SpeechRecognizer.h"
#include "Sound.h"
#include "whisper.h"
#include "diarize.h"
#include "melder.h"
#include "ggml-memory-pool.h"
#include "ggml-silero-vad-model-data.h"

#include "oo_DESTROY.h"
#include "SpeechRecognizer_def.h"
#include "oo_COPY.h"
#include "SpeechRecognizer_def.h"
#include "oo_EQUAL.h"
#include "SpeechRecognizer_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SpeechRecognizer_def.h"
#include "oo_WRITE_TEXT.h"
#include "SpeechRecognizer_def.h"
#include "oo_WRITE_BINARY.h"
#include "SpeechRecognizer_def.h"
#include "oo_READ_TEXT.h"
#include "SpeechRecognizer_def.h"
#include "oo_READ_BINARY.h"
#include "SpeechRecognizer_def.h"
#include "oo_DESCRIPTION.h"
#include "SpeechRecognizer_def.h"

extern unsigned char model_ggml_segmentation_data[];
extern unsigned int model_ggml_segmentation_length;
extern unsigned char model_ggml_embedding_data[];
extern unsigned int model_ggml_embedding_length;

autoWhisperContext :: ~autoWhisperContext () {
	//TRACE
	trace (U"Destroying whisper context at ", Melder_pointer (ptr));
	if (ptr)
		whisper_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool. n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool. sizeInBytes());
}
autoWhisperContext& autoWhisperContext :: operator= (autoWhisperContext&& other) noexcept {
	if (this != & other) {
		whisper_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

autoWhisperVadContext :: ~autoWhisperVadContext () {
	//TRACE
	trace (U"Destroying Silero-VAD context at ", Melder_pointer (ptr));
	whisper_vad_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool. n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool. sizeInBytes());
}
autoWhisperVadContext& autoWhisperVadContext :: operator= (autoWhisperVadContext&& other) noexcept {
	if (this != & other) {
		whisper_vad_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

autoWhisperVadSegments :: ~autoWhisperVadSegments () {
	//TRACE
	trace (U"Destroying Silero-VAD segments at ", Melder_pointer (ptr));
	whisper_vad_free_segments (ptr);
}
autoWhisperVadSegments& autoWhisperVadSegments :: operator= (autoWhisperVadSegments&& other) noexcept {
	if (this != & other) {
		whisper_vad_free_segments (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

autoDiarizeContext :: ~autoDiarizeContext () {
	//TRACE
	trace (U"Destroying diarize context at ", Melder_pointer (ptr));
	diarize_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool. n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool. sizeInBytes());
}
autoDiarizeContext& autoDiarizeContext :: operator= (autoDiarizeContext&& other) noexcept {
	if (this != & other) {
		diarize_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return *this;
}

Thing_implement (SpeechRecognizer, Daata, 0);

void structSpeechRecognizer :: v1_info () {
	SpeechRecognizer_Parent :: v1_info ();
	MelderInfo_writeLine (U"Model: ", our d_modelName.get());
	MelderInfo_writeLine (U"Language: ", our d_languageName.get());
}

static void whisper_log_silent (ggml_log_level level, const char *text, void *user_data) {
	(void) level;
	(void) text;
	(void) user_data;
}

static void supressWhisperLogging () {
	if (Melder_debug == 2001)
		whisper_log_set (nullptr, nullptr);
	else
		whisper_log_set (whisper_log_silent, nullptr);
}

static conststring32 theWhisperModelsFolder ();

autoSpeechRecognizer SpeechRecognizer_create (const conststring32 modelName, const conststring32 languageName) {
	try {
		autoSpeechRecognizer me = Thing_new (SpeechRecognizer);
		my d_modelName = Melder_dup (modelName);
		my d_languageName = Melder_dup (languageName);

		/*
			Check if selected model and language are compatible.
		*/
		if (str32str (modelName, U".en.bin")) {
			Melder_require (str32str (languageName, U"Autodetect") || str32str (languageName, U"English"),
					U"Model ", modelName, U" cannot be used for ", languageName, U" transcription. "
					U"Either select a multilingual model (the model name does not include .en) "
					U"or select \"Autodetect language\"/\"English\" from the language list."
			);
		}

		/*
			Create Whisper context.
		*/
		whisper_context_params contextParams = whisper_context_default_params  ();
		contextParams. use_gpu = false;
		contextParams. flash_attn = false;   // needs to be false to use DTW!!!

		/*
			Enable DTW (Dynamic Time Warping algorithm used for more precise token boundaries).
		*/
		Melder_assert (! contextParams. flash_attn);
		contextParams. dtw_token_timestamps = true;
		if (str32str (modelName, U"tiny.en"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_TINY_EN;
		else if (str32str (modelName, U"tiny"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_TINY;
		else if (str32str (modelName, U"base.en"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_BASE_EN;
		else if (str32str (modelName, U"base"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_BASE;
		else if (str32str (modelName, U"small.en"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_SMALL_EN;
		else if (str32str (modelName, U"small"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_SMALL;
		else if (str32str (modelName, U"medium.en"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_MEDIUM_EN;
		else if (str32str (modelName, U"medium"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_MEDIUM;
		else if (str32str (modelName, U"large-v3-turbo") || str32str (modelName, U"turbo"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_LARGE_V3_TURBO;
		else if (str32str (modelName, U"large-v3"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_LARGE_V3;
		else if (str32str (modelName, U"large-v2"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_LARGE_V2;
		else if (str32str (modelName, U"large-v1") || str32str (modelName, U"large"))
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_LARGE_V1;
		else
			contextParams. dtw_aheads_preset = WHISPER_AHEADS_N_TOP_MOST;

		const conststring32 modelPath = Melder_cat (theWhisperModelsFolder(), U"/", modelName);
		const conststring8 utf8ModelPath = Melder_peek32to8 (modelPath);

		/*
			Initialise Whisper model.
		*/
		supressWhisperLogging ();
		whisper_context *ctx = whisper_init_from_file_with_params (utf8ModelPath, contextParams);
		if (! ctx)
			Melder_throw (U"Cannot create Whisper context from: ", modelPath, U". Model file not found?");

		my whisperContext = autoWhisperContext (ctx);
		theLivingSpeechRecognizers. insert (me.get());
		return me;
	} catch (MelderError) {
		theGgmlMemoryPool. clear();
		Melder_throw (U"SpeechRecognizer not created.");
	}
}

static autovector <float> resampleForWhisper (constSound sound) {
	/*
		Resample the sound to 16kHz if needed.
	*/
	autoSound resampled = Sound_resample (sound, 16000.0, 50);
	sound = resampled.get();

	/*
		Convert the sound to float32 for Whisper.
	*/
	autovector <float> samples32 = newvectorzero <float> (sound -> nx);;
	for (integer i = 1; i <= sound -> nx; i ++)
		samples32 [i] = static_cast <float> (sound -> z [1] [i]);

	return samples32;
}

static void SpeechRecognizer_runWhisper (constSpeechRecognizer me, constSound sound,
		const bool useVad, SileroVadParams const& sileroVadParams) {
	//TRACE
	/*
		Prepare sound for Whispercpp.
	*/
	autovector <float> samples32 = resampleForWhisper (sound);

	/*
		Set Whisper parameters.
	*/
	whisper_sampling_strategy samplingStrategy =
			Melder_debug == 2002 ? WHISPER_SAMPLING_GREEDY : WHISPER_SAMPLING_BEAM_SEARCH;
	trace (U"Sampling strategy = ", samplingStrategy == WHISPER_SAMPLING_GREEDY ? U"greedy" : U"beam search");
	whisper_full_params params = whisper_full_default_params (samplingStrategy);
	params. token_timestamps = true;   // must be true to use t0 and t1 (non-DTW) token timestamps
	if (useVad) {
		params. vad = true;   // enable Silero VAD (Voice Activity Detection used to chop away the silences)
		params. vad_model_data = ggml_silero_bin;   // set up either params.vad_model_data or params.vad_model_path
		params. vad_model_data_size = ggml_silero_bin_len;
		params. vad_params = whisper_vad_default_params();
		params. vad_params. threshold = sileroVadParams. speechProbabilityThreshold;
		params. vad_params. min_speech_duration_ms = sileroVadParams. minSpeechDuration * 1000.0;
		params. vad_params. min_silence_duration_ms = sileroVadParams. minNonSpeechDuration * 1000.0;
		params. vad_params. speech_pad_ms = sileroVadParams. speechPad * 1000.0;
	}
	if (whisper_is_multilingual (my whisperContext.get ())) {
		if (my d_languageName && ! str32str (my d_languageName.get(), U"Autodetect")) {
			autostring32 name = Melder_dup (my d_languageName.get());
			name [0] = Melder_toLowerCase (name [0]);   // e.g. "Dutch" -> "dutch"
			params.language = whisper_lang_str (whisper_lang_id (Melder_peek32to8 (name.get())));
		} else {
			params.language = "auto";
		}
	}

	/*
		Run Whisper.
	*/
	supressWhisperLogging ();
	try {
		if (whisper_full (my whisperContext.get(), params, samples32. asArgumentToFunctionThatExpectsZeroBasedArray(),
				static_cast <int> (samples32.size)) != 0)
			Melder_throw (U"Whisper failed to process audio");
		whisper_print_timings(my whisperContext.get());
	} catch (MelderError) {
		theGgmlMemoryPool. clear();
		Melder_throw (U"Whisper run out of memory. SpeechRecognizer objects are no longer usable and must be recreated.");
	}
}

static bool endsWithTerminalPunctuation(const conststring32 token) {
	if (! token || token [0] == U'\0')
		return false;

	size_t lengthOfToken = Melder_length (token);
	char32 lastChar = token [lengthOfToken - 1];
	return lastChar == U'.' || lastChar == U'!' || lastChar == U'?' ||
			lastChar == U'。' || lastChar == U'！' || lastChar == U'？';
}

static bool endsWithPunctuation(const conststring32 token) {
	if (! token || token [0] == U'\0')
		return false;

	size_t lengthOfToken = Melder_length (token);
	char32 lastChar = token [lengthOfToken - 1];
	return ! Melder_isAlphanumeric (lastChar) && lastChar != U' ';
}

WhisperTranscription SpeechRecognizer_recognize (constSpeechRecognizer me, constSound sound,
		const bool useVad, SileroVadParams const& sileroVadParams, const bool diarize) {
	try {
		//TRACE
		Melder_require (my whisperContext.get(),
				U"This SpeechRecognizer object is not usable anymore as it ran out of memory. ",
				U"Please remove it and create a new one.");
		trace (U"Sound xmin = ", sound -> xmin, U", sound xmax = ", sound -> xmax);

		/*
			Run Whisper and control for blank audio.
		*/
		SpeechRecognizer_runWhisper (me, sound, useVad, sileroVadParams);
		const int numberOfSegments = whisper_full_n_segments (my whisperContext.get());
		if (! numberOfSegments) {
			WhisperTranscription transcription;
			transcription. fullTranscription. text = Melder_dup (U"[BLANK_AUDIO]");
			transcription. fullTranscription. tmin = sound -> xmin;
			transcription. fullTranscription. tmax = sound -> xmax;
			transcription. words = newvectorzero <WhisperSegment> (1);
			transcription. words [1]. text = Melder_dup (U"BLANKAUDIO");
			transcription. words [1]. tmin = sound -> xmin;
			transcription. words [1]. tmax = sound -> xmax;
			transcription. sentences = newvectorzero <WhisperSegment> (1);
			transcription. sentences [1]. text = Melder_dup (U"[BLANK_AUDIO]");
			transcription. sentences [1]. tmin = sound -> xmin;
			transcription. sentences [1]. tmax = sound -> xmax;
			return transcription;
		}

		/*
			Collect all Whisper tokens into one flat list, repairing incomplete UTF8 tokens.
		*/
		struct Token {
			autostring32 textWithPunctuation;
			autostring32 textWithoutPunctuation;
			double tmax;   // DTW timestamp (end of token), in seconds
			bool isSilence;
			bool isNewWord;
			bool isLastTokenInSentence;
		};
		autovector <Token> whisperTokens = newvectorzero <Token> (0);

		/* mutable accumulate */ std::string partialTokenText;   // here we will accumulate whisper token texts until it is a proper UTF8 string
		const whisper_token firstSpecialTokenId = whisper_token_eot (my whisperContext.get());   // eot is the first special token (see struct whisper_vocab in whisper.cpp)
		for (int i = 0; i < numberOfSegments; i ++) {
			const int numberOfTokensInCurrentSegment = whisper_full_n_tokens (my whisperContext.get(), i);
			for (int j = 0; j < numberOfTokensInCurrentSegment; j ++) {
				const whisper_token_data tokenData = whisper_full_get_token_data (my whisperContext.get(), i, j);
				if (tokenData. id >= firstSpecialTokenId) {
					trace (U"Skipping special token: ", tokenData. id);
					continue;   // skip special tokens
				}

				partialTokenText += whisper_full_get_token_text (my whisperContext.get(), i, j);
				const double currentTokenTmax = tokenData. t_dtw / 100.0;

				if (! Melder_str8IsValidUtf8 (partialTokenText.c_str()))
					continue;   // continue accumulating token texts until partialTokenText is a proper UTF8 string

				/*
					Now partialTokenText is valid UTF8.
				*/
				autostring32 fullTokenText = Melder_8to32_e (partialTokenText.c_str());
				partialTokenText. clear();
				const integer fullTokenTextLength = Melder_length (fullTokenText.get());

				/* mutable clean */ mutablestring32 cleanTokenText = fullTokenText.get();
				/* mutable adjust */ integer cleanTokenTextLength = Melder_length (cleanTokenText);
				/* mutable flag */ bool isNewWord = false;

				if (fullTokenTextLength && fullTokenText.get() [0] == U' ') {   // first, remove the leading silence in case of the new word
					++ cleanTokenText;
					-- cleanTokenTextLength;
					isNewWord = true;
				}
				autostring32 textWithPunctuation = Melder_dup (cleanTokenText);   // store it without leading silence but with trailing punctuation
				const integer textWithPunctuationLength = Melder_length (textWithPunctuation.get());

				while (cleanTokenTextLength > 0 && endsWithPunctuation (cleanTokenText)) {   // strip ALL trailing punctuation (e.g., all dots in ...)
					cleanTokenText [cleanTokenTextLength - 1] = U'\0';
					-- cleanTokenTextLength;
				}
				autostring32 textWithoutPunctuation = Melder_dup (cleanTokenText);   // store it without leading silence and without trailing punctuation
				const integer textWithoutPunctuationLength = Melder_length (textWithoutPunctuation.get());

				/*
					Check if token is pure punctuation (and not the first one), then add its text and the left half of the interval to the previous one.
				*/
				if (textWithPunctuationLength && ! textWithoutPunctuationLength && whisperTokens.size >= 1) {
					Token& lastToken = whisperTokens [whisperTokens.size];
					lastToken. tmax = (lastToken. tmax + currentTokenTmax) / 2;
					lastToken. textWithPunctuation = Melder_dup (Melder_cat (lastToken. textWithPunctuation.get(), textWithPunctuation.get()));
					lastToken. isLastTokenInSentence = endsWithTerminalPunctuation (lastToken. textWithPunctuation.get());
				} else {
					Token *token = whisperTokens. append();
					token -> tmax = currentTokenTmax;
					token -> isSilence = ! textWithPunctuationLength;   // if there is no text in a token, mark it as a silence as well
					token -> isNewWord = isNewWord;
					token -> isLastTokenInSentence = endsWithTerminalPunctuation (textWithPunctuation.get());
					token -> textWithPunctuation = textWithPunctuation.move();
					token -> textWithoutPunctuation = textWithoutPunctuation.move();
				}
			}
		}

		/*
			Build the final token list with VAD-adjustments if VAD is in use.
		*/
		autovector <Token> allTokens = newvectorzero <Token> (0);
		const int numberOfVadSegments = whisper_full_n_vad_segments (my whisperContext.get());

		if (! useVad || ! numberOfVadSegments) {
			/*
				VAD is not used or no VAD segments detected: no VAD adjustments needed.
			*/
			allTokens = whisperTokens.move();
		} else {
			/*
				VAD is used and at least one VAD segment detected: translate timestamps back to original time
				and insert silence tokens between consecutive VAD segments.
			*/
			struct VadSegment {
				double origStart;
				double origEnd;
				double vadStart;
				double vadEnd;
			};
			autovector <VadSegment> vadSegments = newvectorzero <VadSegment> (numberOfVadSegments);
			for (integer i = 1; i <= numberOfVadSegments; i ++) {
				vadSegments [i]. origStart = std::min (
						static_cast <double> (whisper_full_get_vad_segment_orig_start (my whisperContext.get(), i - 1)) / 100.0,
						sound ->xmax
					);
				vadSegments [i]. origEnd = std::min (
						static_cast <double> (whisper_full_get_vad_segment_orig_end (my whisperContext.get(), i - 1)) / 100.0,
						sound ->xmax
					);
				vadSegments [i]. vadStart  =
						static_cast <double> (whisper_full_get_vad_segment_vad_start (my whisperContext.get(), i - 1)) / 100.0;
				vadSegments [i]. vadEnd  =
						static_cast <double> (whisper_full_get_vad_segment_vad_end (my whisperContext.get(), i - 1)) / 100.0;
				trace (U"VAD segment ", i, U", orig_start = ", vadSegments [i]. origStart, U", orig_end = ", vadSegments [i]. origEnd,
						U", vad_start = ", vadSegments [i]. vadStart, U", vad_end = ", vadSegments [i]. vadEnd);
			}

			/*
				Lambda to append a silent token with the given tmax to allTokens.
				Note: text fields (textWithPunctuation and textWithoutPunctuation) are default-constructed (empty autostring32).
			*/
			auto appendSilence = [& allTokens] (double tmax) {
				Token *silence = allTokens. append();
				silence -> tmax = tmax;
				silence -> isSilence = true;
				silence -> isNewWord = true;
				silence -> isLastTokenInSentence = false;
				return silence;
			};

			/*
				Lambda to append a whisper token to allTokens, with the given tmax
				(the original whisper tmax is translated from VAD timestamps to original timestamps).
				Note: this moves the text members out of whisperToken, leaving them empty.
			*/
			auto appendWhisperToken = [& allTokens] (Token& whisperToken, double tmax) -> Token * {
				Token *token = allTokens. append();
				token -> textWithPunctuation = whisperToken. textWithPunctuation.move();
				token -> textWithoutPunctuation = whisperToken. textWithoutPunctuation.move();
				token -> tmax = tmax;
				token -> isSilence = whisperToken. isSilence;
				token -> isNewWord = whisperToken. isNewWord;
				token -> isLastTokenInSentence = whisperToken. isLastTokenInSentence;
				return token;
			};

			/*
				First, append a silence token in case speech does not start directly from the beginning of the sound.
			*/
			if (vadSegments [1]. origStart > sound -> xmin) {
				Token *silence = appendSilence (vadSegments [1]. origStart);
				trace (U"Leading silence token: ", allTokens.size, U": text = \"", silence -> textWithPunctuation.get(),
						U"\", tmax = ", silence -> tmax);
			}

			/*
				Then iterate over the flat list of whisper tokens, inserting silences between VAD segments.
			*/
			/* mutable increment */ integer currentVadSegment = 1;
			for (integer i = 1; i <= whisperTokens.size; i ++) {
				/*
					Append a silence token if we progressed into the next VAD segment and current token is more than just one punctuation symbol.
				*/
				if (currentVadSegment < numberOfVadSegments && whisperTokens [i]. tmax > vadSegments [currentVadSegment + 1]. vadStart) {
					if (Melder_debug == 2003 && allTokens.size >= 1)
						allTokens [allTokens.size]. tmax = vadSegments [currentVadSegment]. origEnd;   // extend the last token to the end of current VAD segment
					Token *silence = appendSilence (vadSegments [currentVadSegment + 1]. origStart);   // insert silence between current and next VAD segments
					trace (U"Segment ", i, U"; between VAD segments ", currentVadSegment, U" and ", currentVadSegment + 1,
							U"; silent token ", allTokens.size, U": text = \"", silence -> textWithPunctuation.get(),
							U"\", tmax = ", silence -> tmax);
					currentVadSegment ++;
				}

				/*
					Append (move) the current token with tmax translated back to original time.
				*/
				/* mutable adjust */ double tokenTmax = std::min (
					whisperTokens [i]. tmax + vadSegments [currentVadSegment]. origStart - vadSegments [currentVadSegment]. vadStart,   // original tmmax
					vadSegments [currentVadSegment]. origEnd   // clamp to the size of VAD interval
				);
				if (Melder_debug == 2003 && currentVadSegment == numberOfVadSegments && i == whisperTokens.size)
				    tokenTmax = vadSegments [currentVadSegment]. origEnd;   // extend to the size of VAD interval, if this is the last token
				Token *token = appendWhisperToken (whisperTokens [i], tokenTmax);   // this moves the autostring32 fields of whisperTokens [i]
				trace (U"Segment ", i, U"; VAD segment ", currentVadSegment,
						U"; token ", allTokens.size, U": text = ", token -> textWithPunctuation.get(),
						U", tmax = ", token -> tmax);
			}

			/*
				Insert trailing silence.
			*/
			if (allTokens [allTokens.size]. tmax < sound -> xmax) {
				Token *silence = appendSilence (sound -> xmax);
				trace (U"Trailing silence token: ", allTokens.size, U": text = \"", silence -> textWithPunctuation.get(),
						U"\", tmax = ", silence -> tmax);
			}
		}

		/*
			Build word and sentence segments from the flat token list.
		*/
		autoMelderString sentenceText;
		autoMelderString fullText;
		autovector <WhisperSegment> words = newvectorzero <WhisperSegment> (0);
		autovector <WhisperSegment> sentences = newvectorzero <WhisperSegment> (0);

		/* mutable per-sentence */ double sentenceTmin = sound -> xmin;   // (re)set at each sentence start, default assignment is just a guard
		/* mutable per-token */ double previousTokenTmax = sound -> xmin;   // default for the first token
		/* mutable flag */ bool isFirstTokenInSentence = true;
		/* mutable flag */ bool isFirstSentence = true;

		for (integer i = 1; i <= allTokens.size; i ++) {
			const double tokenTmin = previousTokenTmax;
			const double tokenTmax = allTokens [i]. tmax;
			previousTokenTmax = tokenTmax;

			if (isFirstTokenInSentence)
				sentenceTmin = tokenTmin;   // update start of sentence timestamp

			const bool isSilentToken = allTokens [i]. isSilence;
			const bool isNewWord = allTokens [i]. isNewWord;
			const bool isLastTokenInSentence = allTokens [i]. isLastTokenInSentence;
			const conststring32 tokenTextWithPunctuation = allTokens [i]. textWithPunctuation.get();
			const conststring32 tokenTextWithoutPunctuation = allTokens [i]. textWithoutPunctuation.get();

			/*
				Add token text to the sentence and to the full transcription text, unless it is a silence token.
			*/
			if (! isSilentToken) {
				if (isNewWord && ! isFirstTokenInSentence) {   // new word, middle of a sentence
					MelderString_append (& sentenceText, U" ", tokenTextWithPunctuation);
					MelderString_append (& fullText, U" ", tokenTextWithPunctuation);
				} else if (isNewWord && ! isFirstSentence) {   // new word, beginning of not the first sentence
					MelderString_append (& sentenceText, tokenTextWithPunctuation);
					MelderString_append (& fullText, U" ", tokenTextWithPunctuation);
				} else {   // new word, beginning of the first sentence OR continuation word
					MelderString_append (& sentenceText, tokenTextWithPunctuation);
					MelderString_append (& fullText, tokenTextWithPunctuation);
				}
			}

			/*
				Create word-level segment.
			*/
			Melder_assert (tokenTmax >= tokenTmin);
			trace (U"Token ", i, U": \"", tokenTextWithoutPunctuation, U"\" [ ", tokenTmin, U" - ", tokenTmax, U" ], isSilentToken = ", isSilentToken);
			if ((isNewWord || words.size == 0) && tokenTmax > tokenTmin) {   // new word
				WhisperSegment *word = words. append();
				word -> text = Melder_dup (tokenTextWithoutPunctuation);
				word -> tmin = tokenTmin;
				word -> tmax = tokenTmax;
				trace (U"Word ", words.size, U": \"", word -> text.get(), U"\" [ ", word -> tmin, U" - ", word -> tmax, U" ]");
			} else if (isNewWord && tokenTmax == tokenTmin && words.size > 0) {   // zero-length new word: append to previous with a space
				WhisperSegment& word = words [words.size];
				word.text = Melder_dup (Melder_cat (word.text.get(), U" ", tokenTextWithoutPunctuation));
				word.tmax = tokenTmax;
				trace (U"Word ", words.size, U": \"", word.text.get(), U"\" [ ", word. tmin, U" - ", word. tmax, U" ] (appended zero-length)");
			} else if (words.size > 0) {   // continuation token: append to the last word
				WhisperSegment& word = words [words.size];
				word. text = Melder_dup (Melder_cat (word.text.get(), tokenTextWithoutPunctuation));
				word. tmax = tokenTmax;
				trace (U"Word ", words.size, U": \"", word.text.get(), U"\" [ ", word. tmin, U" - ", word. tmax, U" ]");
			}

			/*
				Finalize and store sentence segment if sentence is complete.
			*/
			const bool isLastTokenOverall = (i == allTokens.size);
			if (isLastTokenInSentence || isLastTokenOverall || (isSilentToken && isFirstTokenInSentence)) {
				WhisperSegment *sentence = sentences. append();
				sentence -> text = Melder_dup (sentenceText.string);
				sentence -> tmin = sentenceTmin;
				sentence -> tmax = tokenTmax;
				trace (U"Sentence: ", sentences.size, U": \"", sentence -> text.get(), U"\" [ ", sentence -> tmin, U" - ", sentence -> tmax, U" ]");
				MelderString_empty (& sentenceText);
				isFirstTokenInSentence = true;   // current sentence is finalized, start with the new one on the next iteration
				if (isFirstSentence && ! isSilentToken)
					isFirstSentence = false;
			} else {
				isFirstTokenInSentence = false;   // continue with the current sentence
			}
		}

		WhisperTranscription transcription;
		transcription. fullTranscription. text = Melder_dup (fullText.string);
		transcription. fullTranscription. tmin = sound -> xmin;
		transcription. fullTranscription. tmax = sound -> xmax;
		transcription. words = words.move();
		transcription. sentences = sentences.move();

		trace (U"Full transcription:",
				U" [ ", transcription. fullTranscription. tmin, U" - ",
				transcription. fullTranscription. tmax, U" ]", transcription. fullTranscription. text.get());

		if (diarize) {
			transcription. speakers = doDiarization (sound);
		} else {
			transcription. speakers = newvectorzero <autovector <WhisperSegment>> (0);
		}
		return transcription;

	} catch (MelderError) {
		Melder_throw (U"Sound not transcribed.");
	}
}

autovector <WhisperSegment> doSileroVad (constSound sound, SileroVadParams const& sileroVadParams,
		const conststring32 nonSpeechLabel, const conststring32 speechLabel) {
	//TRACE
	trace (U"Sound xmin = ", sound -> xmin, U", sound xmax = ", sound -> xmax);
	supressWhisperLogging ();

	/*
		Remember original sound -> xmin and sound -> xmax before resampling; and resample.
	*/
	const double soundStart = sound -> xmin;
	const double soundEnd = sound -> xmax;
	autovector <float> samples32 = resampleForWhisper (sound);

	try {
		/*
			Initialize VAD context.
		*/
		whisper_vad_context_params vad_ctx_params = whisper_vad_default_context_params ();
		autoWhisperVadContext vad_ctx = whisper_vad_init_from_memory_with_params (ggml_silero_bin, ggml_silero_bin_len, vad_ctx_params);
		if (! vad_ctx.get())
			Melder_throw (U"Failed to initialize VAD context.");

		/*
			Set VAD parameters and run Silero VAD.
		*/
		whisper_vad_params vad_params = whisper_vad_default_params ();
		vad_params. threshold = sileroVadParams. speechProbabilityThreshold;
		vad_params. min_speech_duration_ms = sileroVadParams. minSpeechDuration * 1000.0;
		vad_params. min_silence_duration_ms = sileroVadParams. minNonSpeechDuration * 1000.0;
		vad_params. speech_pad_ms = sileroVadParams. speechPad * 1000.0;
		autoWhisperVadSegments vad_segments = whisper_vad_segments_from_samples (
			vad_ctx.get(), vad_params, samples32.asArgumentToFunctionThatExpectsZeroBasedArray(), static_cast <int> (samples32.size));
		if (! vad_segments.get()) {
			Melder_throw (U"Failed to obtain VAD segments.");
		}

		/*
			Collect all VAD segments and wrap them with "non-voice" intervals.
		*/
		const int numberOfVadSegments = whisper_vad_segments_n_segments (vad_segments.get());
		autovector <WhisperSegment> allIntervals = newvectorzero <WhisperSegment> (0);

		for (int i = 0; i < numberOfVadSegments; i ++) {
			const double t0 = whisper_vad_segments_get_segment_t0 (vad_segments.get(), i) / 100.0;
			const double t1 = whisper_vad_segments_get_segment_t1 (vad_segments.get(), i) / 100.0;

			/*
				Insert a non-voice interval before the first voice interval or between two voice intervals.
			*/
			const double previousEnd = (i == 0) ? soundStart : whisper_vad_segments_get_segment_t1 (vad_segments.get(), i - 1) / 100.0;
			if (t0 > previousEnd) {
				WhisperSegment *nonVoiceInterval = allIntervals. append ();
				nonVoiceInterval -> text = Melder_dup (nonSpeechLabel);
				nonVoiceInterval -> tmin = previousEnd;
				nonVoiceInterval -> tmax = t0;
				trace (U"Non-voice: t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);
			}
			/*
				Insert the voice interval.
			*/
			WhisperSegment *voiceInterval = allIntervals. append();
			voiceInterval -> text = Melder_dup (speechLabel);
			voiceInterval -> tmin = t0;
			voiceInterval -> tmax = t1;
			trace (U"VAD segment ", i + 1, U": t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);

			/*
				After the last voice interval, add non-voice if there is remaining audio.
			*/
			if (i == numberOfVadSegments - 1 && t1 < soundEnd) {
				WhisperSegment *nonVoiceInterval = allIntervals. append ();
				nonVoiceInterval -> text = Melder_dup (nonSpeechLabel);
				nonVoiceInterval -> tmin = t1;
				nonVoiceInterval -> tmax = soundEnd;
				trace (U"Non-voice: t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);
			}
		}

		/*
			If no voice activity detected at all, then entire sound is non-voice.
		*/
		if (! numberOfVadSegments) {
			WhisperSegment *nonVoice = allIntervals. append ();
			nonVoice -> text = Melder_dup (nonSpeechLabel);
			nonVoice -> tmin = soundStart;
			nonVoice -> tmax = soundEnd;
		}

		return allIntervals;
	} catch (MelderError) {
		theGgmlMemoryPool. clear();   // this runs after destructors of local objects, therefore it must be safe!
		Melder_throw (U"Voice Activity not detected for sound.");
	}
}

autovector <autovector <WhisperSegment>> doDiarization (constSound sound) {
	//TRACE
	autovector <float> samples32 = resampleForWhisper (sound);
	try {
		autoDiarizeContext diarizeContext = diarize_init_from_memory (
			model_ggml_segmentation_data, model_ggml_segmentation_length,
			model_ggml_embedding_data, model_ggml_embedding_length
			);
		diarize_params diarizeParams = diarize_default_params ();
		diarize_full (diarizeContext.get(), diarizeParams,samples32.asArgumentToFunctionThatExpectsZeroBasedArray(),
				static_cast <int> (samples32.size));
		const unsigned int numberOfSegments = diarize_full_n_segments (diarizeContext.get());
		const int numberOfSpeakers = diarize_full_n_speakers(diarizeContext.get());

		trace(U"Speakers:", numberOfSpeakers, U", Segments: ", numberOfSegments);

		autovector <autovector <WhisperSegment>> speakers = newvectorzero <autovector <WhisperSegment>> (numberOfSpeakers);
		/*
			Collect segments for each speaker.
		*/
		for (int i = 1; i <= numberOfSpeakers; i ++) {
			/* mutable increment */ double currentIntervalStart = sound -> xmin;

			for (int segment = 0; segment < numberOfSegments; segment ++) {
				if (diarize_full_get_segment_speaker (diarizeContext.get(), segment) + 1 != i)   // this segment is from a different speaker
					continue;

				const double tmin = diarize_full_get_segment_t0 (diarizeContext.get(), segment);
				const double tmax = diarize_full_get_segment_t1 (diarizeContext.get(), segment);

				if (tmin > currentIntervalStart) {
					WhisperSegment *gap = speakers [i]. append();
					gap -> text = Melder_dup (U"");
					gap -> tmin = currentIntervalStart;
					gap -> tmax = tmin;
				}

				WhisperSegment *speakerSegment = speakers [i]. append();
				speakerSegment -> text = Melder_dup (Melder_cat (U"SPEAKER_", Melder_integer (i)));
				speakerSegment -> tmin = tmin;
				speakerSegment -> tmax = tmax;

				trace (U"Diarization: [ ", speakerSegment -> tmin, U" - ", speakerSegment -> tmax, U" ] \"",
						speakerSegment -> text.get(), U"\"");

				currentIntervalStart = tmax;
			}

			if (currentIntervalStart < sound -> xmax) {
				WhisperSegment *gap = speakers [i].append();
				gap -> text = Melder_dup (U"");
				gap -> tmin = currentIntervalStart;
				gap -> tmax = sound -> xmax;
			}
		}

		return speakers;

	} catch (MelderError) {
		theGgmlMemoryPool. clear();
		Melder_throw (U"Diarization failed.");
	}
}

static conststring32 theWhisperModelsFolder () {
	static autostring32 whisperModelFolderPath;
	if (! whisperModelFolderPath) {
		try {
			structMelderFolder modelsFolder { };
			MelderFolder_getSubfolder (Melder_preferencesFolder (),	U"models", & modelsFolder);
			if (! MelderFolder_exists (& modelsFolder))
				MelderFolder_create (& modelsFolder);
			structMelderFolder whispercppFolder { };
			MelderFolder_getSubfolder (& modelsFolder,	U"whispercpp", & whispercppFolder);
			if (! MelderFolder_exists (& whispercppFolder))
				MelderFolder_create (& whispercppFolder);
			whisperModelFolderPath = Melder_dup (MelderFolder_peekPath (& whispercppFolder));
			if (Melder_debug == 2001)
				Melder_casual (U"Whispercpp model folder path: ", whisperModelFolderPath.get());
		} catch (MelderError) {
			Melder_clearError ();
		}
	}
	return whisperModelFolderPath.get();
}

constSTRVEC theCurrentSpeechRecognizerModelNames () {
	static autoSTRVEC whisperModelNames;
	try {
		whisperModelNames = fileNames_STRVEC (Melder_cat (theWhisperModelsFolder (), U"/*.bin"));
	} catch (MelderError) {
		Melder_clearError ();
	}
	return whisperModelNames.get();
}

constSTRVEC theSpeechRecognizerLanguageNames () {
	static autoSTRVEC sortedWhisperLanguageNames;
	if (! sortedWhisperLanguageNames) {
		autoSTRVEC unsorted;
		try {
			const uint8 numberOfLanguages = whisper_lang_max_id ();
			for (uint8 i = 0; i < numberOfLanguages; i ++) {
				autostring32 languageName = Melder_8to32_e (whisper_lang_str_full (i));

				/*
					Capitalize the first letter, e.g. "dutch" -> "Dutch".
				*/
				if (languageName [0] != U'\0')
					languageName [0] = Melder_toUpperCase (languageName [0]);

				unsorted. append (languageName.get());
			}
			autoSTRVEC sorted = sort_STRVEC (unsorted.get());

			/*
				Create a list to return (with autodetect option).
			*/
			sortedWhisperLanguageNames. append (U"Autodetect language");
			for (integer i = 1; i <= sorted.size; i ++)
				sortedWhisperLanguageNames. append (sorted [i].get());

		} catch (MelderError) {
			unsorted. reset();
			sortedWhisperLanguageNames. reset();
			Melder_clearError ();
		}
	}
	return sortedWhisperLanguageNames.get();
}

/* End of file SpeechRecognizer.cpp */
