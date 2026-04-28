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
	TRACE
	trace (U"Destroying whisper context at ", Melder_pointer (ptr));
	if (ptr)
		whisper_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool.n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool.sizeInBytes());
}
autoWhisperContext& autoWhisperContext :: operator= (autoWhisperContext&& other) noexcept {
	if (this != & other) {
		whisper_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return * this;
}

autoWhisperVadContext :: ~autoWhisperVadContext () {
	TRACE
	trace (U"Destroying Silero-VAD context at ", Melder_pointer (ptr));
	whisper_vad_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool.n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool.sizeInBytes());
}
autoWhisperVadContext & autoWhisperVadContext :: operator= (autoWhisperVadContext && other) noexcept {
	if (this != & other) {
		whisper_vad_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return * this;
}

autoWhisperVadSegments :: ~autoWhisperVadSegments () {
	TRACE
	trace (U"Destroying Silero-VAD segments at ", Melder_pointer (ptr));
	whisper_vad_free_segments (ptr);
}
autoWhisperVadSegments & autoWhisperVadSegments :: operator= (autoWhisperVadSegments && other) noexcept {
	if (this != & other) {
		whisper_vad_free_segments (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return * this;
}

autoDiarizeContext :: ~autoDiarizeContext () {
	TRACE
	trace (U"Destroying diarize context at ", Melder_pointer (ptr));
	diarize_free (ptr);
	trace (U"Number of allocations in the memory pool is  ", theGgmlMemoryPool.n_allocations());
	trace (U"Total memory in bytes is  ", theGgmlMemoryPool.sizeInBytes());
}
autoDiarizeContext& autoDiarizeContext :: operator= (autoDiarizeContext&& other) noexcept {
	if (this != & other) {
		diarize_free (ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
	}
	return * this;
}

Thing_implement (SpeechRecognizer, Daata, 0);

void structSpeechRecognizer :: v1_info () {
	SpeechRecognizer_Parent :: v1_info ();
	MelderInfo_writeLine (U"Model: ", our d_modelName.get());
	MelderInfo_writeLine (U"Language: ", our d_languageName.get());
}

static void whisper_log_silent (ggml_log_level level, const char * text, void * user_data) {
	(void) level;
	(void) text;
	(void) user_data;
}

static void supressWhisperLogging () {
	if (Melder_debug == 2001)
		whisper_log_set(nullptr, nullptr);
	else
		whisper_log_set(whisper_log_silent, nullptr);
}

static conststring32 theWhisperModelsFolder ();

autoSpeechRecognizer SpeechRecognizer_create (conststring32 modelName, conststring32 languageName) {
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
		whisper_context_params contextParams = whisper_context_default_params();
		contextParams.use_gpu = false;
		contextParams.flash_attn = false;   // needs to be false to use DTW!!!

		/*
			Enable DTW (Dynamic Time Warping algorithm used for more precise token boundaries).
		*/
		Melder_assert (! contextParams.flash_attn);
		contextParams.dtw_token_timestamps = true;
		if (str32str (modelName, U"tiny.en"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_TINY_EN;
		else if (str32str (modelName, U"tiny"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_TINY;
		else if (str32str (modelName, U"base.en"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_BASE_EN;
		else if (str32str (modelName, U"base"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_BASE;
		else if (str32str (modelName, U"small.en"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_SMALL_EN;
		else if (str32str (modelName, U"small"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_SMALL;
		else if (str32str (modelName, U"medium.en"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_MEDIUM_EN;
		else if (str32str (modelName, U"medium"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_MEDIUM;
		else if (str32str (modelName, U"large-v3-turbo") || str32str (modelName, U"turbo"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_LARGE_V3_TURBO;
		else if (str32str (modelName, U"large-v3"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_LARGE_V3;
		else if (str32str (modelName, U"large-v2"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_LARGE_V2;
		else if (str32str (modelName, U"large-v1") || str32str (modelName, U"large"))
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_LARGE_V1;
		else
			contextParams.dtw_aheads_preset = WHISPER_AHEADS_N_TOP_MOST;

		conststring32 modelPath = Melder_cat (theWhisperModelsFolder(), U"/", modelName);
		conststring8 utf8ModelPath = Melder_peek32to8 (modelPath);

		/*
			Initialise Whisper model.
		*/
		supressWhisperLogging();
		whisper_context *ctx = whisper_init_from_file_with_params (utf8ModelPath, contextParams);
		if (! ctx)
			Melder_throw (U"Cannot create Whisper context from: ", modelPath, U". Model file not found?");

		my whisperContext = autoWhisperContext (ctx);
		theLivingSpeechRecognizers.insert (me.get());
		return me;
	} catch (MelderError) {
		theGgmlMemoryPool.clear();
		Melder_throw (U"SpeechRecognizer not created.");
	}
}

static std::vector <float> resampleForWhisper (constSound sound) {
	/*
		Resample the sound to 16kHz if needed.
	*/
	autoSound resampled = Sound_resample (sound, 16000.0, 50);
	sound = resampled.get();

	/*
		Convert the sound to float32 for Whisper.
	*/
	std::vector <float> samples32;
	samples32. reserve (integer_to_uinteger_a (sound -> nx));
	for (integer i = 1; i <= sound -> nx; ++ i)
		samples32. push_back (static_cast <float> (sound -> z [1] [i]));

	return samples32;
}

static void SpeechRecognizer_runWhisper (SpeechRecognizer me, constSound sound,
		bool useVad, const SileroVadParams &sileroVadParams) {
	//TRACE
	/*
		Prepare sound for Whispercpp.
	*/
	std::vector <float> samples32 = resampleForWhisper (sound);

	/*
		Set Whisper parameters.
	*/
	whisper_sampling_strategy sampling_strategy =
			Melder_debug == 2002 ? WHISPER_SAMPLING_GREEDY : WHISPER_SAMPLING_BEAM_SEARCH;
	trace (U"Sampling strategy = ", sampling_strategy == WHISPER_SAMPLING_GREEDY ? U"greedy" : U"beam search");
	whisper_full_params params = whisper_full_default_params (sampling_strategy);
	params.token_timestamps = true;   // must be true to use t0 and t1 (non-DTW) token timestamps
	if (useVad) {
		params.vad = true;   // enable Silero VAD (Voice Activity Detection used to chop away the silences)
		params.vad_model_data = ggml_silero_bin;   // set up either params.vad_model_data or params.vad_model_path
		params.vad_model_data_size = ggml_silero_bin_len;
		params.vad_params = whisper_vad_default_params();
		params.vad_params.threshold = sileroVadParams.speechProbabilityThreshold;
		params.vad_params.min_speech_duration_ms = sileroVadParams.minSpeechDuration * 1000.0;
		params.vad_params.min_silence_duration_ms = sileroVadParams.minNonSpeechDuration * 1000.0;
		params.vad_params.speech_pad_ms = sileroVadParams.speechPad * 1000.0;
	}
	if (whisper_is_multilingual (my whisperContext.get ())) {
		if (my d_languageName && ! str32str (my d_languageName.get(), U"Autodetect")) {
			autostring32 name = Melder_dup (my d_languageName.get());
			name [0] = Melder_toLowerCase (name [0]);   // e.g. "Dutch" -> "dutch"
			params. language = whisper_lang_str (whisper_lang_id (Melder_peek32to8 (name.get())));
		} else {
			params. language = "auto";
		}
	}

	/*
		Run Whisper.
	*/
	supressWhisperLogging ();
	try {
		if (whisper_full (my whisperContext.get(), params, samples32.data(), static_cast <int> (samples32.size())) != 0)
			Melder_throw (U"Whisper failed to process audio");
		whisper_print_timings(my whisperContext.get());
	} catch (MelderError) {
		theGgmlMemoryPool.clear();
		Melder_throw (U"Whisper run out of memory. SpeechRecognizer objects are no longer usable and must be recreated.");
	}
}

static bool endsWithTerminalPunctuation(conststring32 token) {
	if (! token || token [0] == U'\0')
		return false;

	size_t token_length = Melder_length (token);
	char32 last_char = token [token_length - 1];
	return last_char == U'.' || last_char == U'!' || last_char == U'?' ||
		last_char == U'。' || last_char == U'！' || last_char == U'？';
}

static bool endsWithPunctuation(conststring32 token) {
	if (! token || token [0] == U'\0')
		return false;

	size_t token_length = Melder_length (token);
	char32 last_char = token [token_length - 1];
	return ! Melder_isAlphanumeric (last_char) && last_char != U' ';
}

autovector <WhisperSegment> doSileroVad (constSound sound, const SileroVadParams &sileroVadParams,
		conststring32 nonSpeechLabel, conststring32 speechLabel) {
	//TRACE
	trace (U"Sound xmin = ", sound -> xmin, U", sound xmax = ", sound -> xmax);
	supressWhisperLogging ();

	/*
		Remember original sound -> xmin and sound -> xmax before resampling; and resample.
	*/
	double soundStart = sound -> xmin;
	double soundEnd = sound -> xmax;
	std::vector <float> samples32 = resampleForWhisper (sound);

	try {
		/*
			Initialize VAD context.
		*/
		whisper_vad_context_params vad_ctx_params = whisper_vad_default_context_params();
		autoWhisperVadContext vad_ctx = whisper_vad_init_from_memory_with_params (
				ggml_silero_bin, ggml_silero_bin_len, vad_ctx_params);
		if (! vad_ctx.get())
			Melder_throw (U"Failed to initialize VAD context.");

		/*
			Set VAD parameters and run Silero VAD.
		*/
		whisper_vad_params vad_params = whisper_vad_default_params();
		vad_params.threshold = sileroVadParams.speechProbabilityThreshold;
		vad_params.min_speech_duration_ms = sileroVadParams.minSpeechDuration * 1000.0;
		vad_params.min_silence_duration_ms = sileroVadParams.minNonSpeechDuration * 1000.0;
		vad_params.speech_pad_ms = sileroVadParams.speechPad * 1000.0;
		autoWhisperVadSegments vad_segments = whisper_vad_segments_from_samples(
			vad_ctx.get(), vad_params, samples32.data(), static_cast <int> (samples32.size()));
		if (!vad_segments.get()) {
			Melder_throw (U"Failed to obtain VAD segments.");
		}

		/*
			Collect all VAD segments and wrap them with "non-voice" intervals.
		*/
		int nVadSegments = whisper_vad_segments_n_segments (vad_segments.get());
		autovector <WhisperSegment> allIntervals = newvectorzero<WhisperSegment>(0);

		for (int i = 0; i < nVadSegments; ++ i) {
			double t0 = whisper_vad_segments_get_segment_t0 (vad_segments.get(), i) / 100.0;
			double t1 = whisper_vad_segments_get_segment_t1 (vad_segments.get(), i) / 100.0;

			/*
				Insert a non-voice interval before the first voice interval or between two voice intervals.
			*/
			double previousEnd = (i == 0) ? soundStart : whisper_vad_segments_get_segment_t1 (vad_segments.get(), i - 1) / 100.0;
			if (t0 > previousEnd) {
				WhisperSegment * nonVoiceInterval = allIntervals. append ();
				nonVoiceInterval -> text = Melder_dup (nonSpeechLabel);
				nonVoiceInterval -> tmin = previousEnd;
				nonVoiceInterval -> tmax = t0;
				trace (U"Non-voice: t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);
			}
			/*
				Insert the voice interval.
			*/
			WhisperSegment *voiceInterval = allIntervals.append();
			voiceInterval -> text = Melder_dup(speechLabel);
			voiceInterval -> tmin = t0;
			voiceInterval -> tmax = t1;
			trace (U"VAD segment ", i + 1, U": t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);

			/*
				After the last voice interval, add non-voice if there is remaining audio.
			*/
			if (i == nVadSegments - 1 && t1 < soundEnd) {
				WhisperSegment * nonVoiceInterval = allIntervals. append ();
				nonVoiceInterval -> text = Melder_dup (nonSpeechLabel);
				nonVoiceInterval -> tmin = t1;
				nonVoiceInterval -> tmax = soundEnd;
				trace (U"Non-voice: t0 = ", allIntervals [allIntervals.size]. tmin, U", t1 = ", allIntervals [allIntervals.size]. tmax);
			}
		}

		/*
			If no voice activity detected at all, then entire sound is non-voice.
		*/
		if (! nVadSegments) {
			WhisperSegment * nonVoice = allIntervals. append ();
			nonVoice -> text = Melder_dup (nonSpeechLabel);
			nonVoice -> tmin = soundStart;
			nonVoice -> tmax = soundEnd;
		}

		return allIntervals;
	} catch (MelderError) {
		theGgmlMemoryPool.clear();   // this runs after destructors of local objects, therefore it must be safe!
		Melder_throw (U"Voice Activity not detected for sound.");
	}
}

autovector <autovector <WhisperSegment>> doDiarization (constSound sound) {
	//TRACE
	std::vector <float> samples32 = resampleForWhisper (sound);
	try {
		autoDiarizeContext diarizeContext = diarize_init_from_memory (
			model_ggml_segmentation_data, model_ggml_segmentation_length,
			model_ggml_embedding_data, model_ggml_embedding_length
			);
		diarize_params diarizeParams = diarize_default_params();
		diarize_full(diarizeContext.get(), diarizeParams, samples32.data(), static_cast <int> (samples32.size()));
		const unsigned int n_diarization_segments = diarize_full_n_segments(diarizeContext.get());
		const int n_speakers = diarize_full_n_speakers(diarizeContext.get());

		trace(U"Speakers:", n_speakers, U", Segments: ", n_diarization_segments);

		autovector <autovector <WhisperSegment>> speakers = newvectorzero <autovector <WhisperSegment>> (n_speakers);
		/*
			Collect segments for each speaker.
		*/
		for (int i = 1; i <= n_speakers; ++i) {
			double currentIntervalStart = sound -> xmin;

			for (int segment = 0; segment < n_diarization_segments; ++ segment) {
				if (diarize_full_get_segment_speaker(diarizeContext.get(), segment) + 1 != i)   // this segment is from a different speaker
					continue;

				const double tmin = diarize_full_get_segment_t0(diarizeContext.get(), segment);
				const double tmax = diarize_full_get_segment_t1(diarizeContext.get(), segment);

				if (tmin > currentIntervalStart) {
					WhisperSegment *gap = speakers [i].append();
					gap -> text = Melder_dup (U"");
					gap -> tmin = currentIntervalStart;
					gap -> tmax = tmin;
				}

				WhisperSegment *speakerSegment = speakers [i].append();
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
		theGgmlMemoryPool.clear();
		Melder_throw (U"Diarization failed.");
	}
}

WhisperTranscription SpeechRecognizer_recognize (SpeechRecognizer me, constSound sound,
		bool useVad, const SileroVadParams &sileroVadParams, bool diarize) {
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
		const int n_segments = whisper_full_n_segments (my whisperContext.get());
		if (! n_segments) {
			WhisperTranscription transcription;
			transcription.fullTranscription.text = Melder_dup (U"[BLANK_AUDIO]");
			transcription.fullTranscription.tmin = sound -> xmin;
			transcription.fullTranscription.tmax = sound -> xmax;
			transcription.words = newvectorzero <WhisperSegment> (1);
			transcription.words [1].text = Melder_dup (U"BLANKAUDIO");
			transcription.words [1].tmin = sound -> xmin;
			transcription.words [1].tmax = sound -> xmax;
			transcription.sentences = newvectorzero <WhisperSegment> (1);
			transcription.sentences [1].text = Melder_dup (U"[BLANK_AUDIO]");
			transcription.sentences [1].tmin = sound -> xmin;
			transcription.sentences [1].tmax = sound -> xmax;
			return transcription;
		}

		/*
			Collect VAD segments.
		*/
		const int n_vad_segments = whisper_full_n_vad_segments (my whisperContext.get());
		struct VadSegment {
			double orig_start;
			double orig_end;
			double vad_start;
			double vad_end;
		};
		autovector <VadSegment> vadSegments = newvectorzero<VadSegment>(n_vad_segments);
		for (int i = 1; i <= n_vad_segments; ++ i) {
			vadSegments [i].orig_start =
					static_cast<double> (whisper_full_get_vad_segment_orig_start (my whisperContext.get(), i - 1)) / 100.0;
			vadSegments [i].orig_end =
					static_cast<double> (whisper_full_get_vad_segment_orig_end (my whisperContext.get(), i - 1)) / 100.0;
			vadSegments [i].vad_start  =
					static_cast<double> (whisper_full_get_vad_segment_vad_start (my whisperContext.get(), i - 1)) / 100.0;
			vadSegments [i].vad_end  =
					static_cast<double> (whisper_full_get_vad_segment_vad_end (my whisperContext.get(), i - 1)) / 100.0;
			trace (U"VAD segment ", i, U", orig_start = ", vadSegments [i]. orig_start, U", orig_end = ", vadSegments [i]. orig_end,
					U", vad_start = ", vadSegments [i]. vad_start, U", vad_end = ", vadSegments [i]. vad_end);
		}

		/*
			Collect all tokens, including silences in case of VAD, into one flat list.
		*/
		struct Token {
			autostring32 text;
			double tmax;   // DTW timestamp (end of token), in seconds
			bool isPunctuation;
			bool isSilence;
			bool isNewWord;
		};
		autovector <Token> allTokens = newvectorzero <Token> (0);

		/*
			First, insert a silence token in case speech does not start directly from the beginning of the sound.
		*/
		if (useVad && n_vad_segments && vadSegments [1]. orig_start > sound -> xmin) {
			Token *silence = allTokens.append();
			silence -> tmax = vadSegments [1]. orig_start;
			silence -> isPunctuation = false;
			silence -> isSilence = true;
			silence -> isNewWord = true;
		}

		/*
			Then, collect tokens from each segment, inserting silences between VAD segments.
		*/
		std::string partialTokenText;
		double partialTokenTmax;
		int current_vad_segment = 1;
		for (int i = 0; i < n_segments; ++ i) {
			for (int j = 0; j < whisper_full_n_tokens (my whisperContext.get(), i); ++ j) {
				whisper_token_data token_data = whisper_full_get_token_data (my whisperContext.get(), i, j);
				if (token_data.id >= whisper_token_eot (my whisperContext.get())) {
					trace (U"Skipping special token: ", token_data.id);
					continue;   // skip special tokens
				}

				partialTokenText += whisper_full_get_token_text (my whisperContext.get(), i, j);
				partialTokenTmax = token_data.t_dtw / 100.0;

				if (! Melder_str8IsValidUtf8 (partialTokenText.c_str()))
					continue;   // continue accumulating token texts until partialTokenText is a proper UTF8 string

				/*
					Now partialTokenText is valid UTF8.
				*/
				autostring32 raw_token_text = Melder_8to32_e (partialTokenText.c_str());
				conststring32 token_text = raw_token_text.get();
				integer length_token_text = Melder_length (token_text);
				double tmax = partialTokenTmax;
				bool isPunctuation = length_token_text == 1 && endsWithPunctuation (token_text);

				/*
					In case of VAD, translate timestamps back to original time and insert silence tokens (" ") between VAD segments.
				*/
				if (useVad && n_vad_segments) {
					/*
						Insert a silence token if we progressed into the next VAD segment
						and current token is more than just one punctuation symbol.
					*/
					if (current_vad_segment < n_vad_segments
						&& tmax > vadSegments [current_vad_segment + 1]. vad_start
						&& ! isPunctuation
					) {
						Token *silence = allTokens.append();
						silence -> tmax = vadSegments [current_vad_segment + 1]. orig_start;
						silence -> isPunctuation = false;
						silence -> isSilence = true;
						silence -> isNewWord = true;
						trace (U"Segment ", i, U"; between VAD segments ", current_vad_segment, U" and ", current_vad_segment + 1,
								U"; silent token ", allTokens.size, U": text = \"", silence -> text.get(),
								U"\", tmax = ", silence -> tmax);
						++ current_vad_segment;
					}
					tmax += vadSegments [current_vad_segment]. orig_start - vadSegments [current_vad_segment]. vad_start;
					tmax = std::min (tmax, vadSegments [current_vad_segment]. orig_end);   // clamp to the size of VAD interval
				}

				Token *token = allTokens.append();
				token -> tmax = tmax;
				token -> isPunctuation = isPunctuation;
				token -> isSilence = ! length_token_text;   // if there is no text in a token, mark it as a silence as well
				token -> isNewWord = ! length_token_text || token_text [0] == U' ';
				if (token -> isNewWord && ! token -> isSilence)
					token_text ++;
				token -> text = Melder_dup (token_text);
				trace (U"Segment ", i, U"; VAD segment ", current_vad_segment,
						U"; token ", allTokens.size, U": text = ", token -> text.get(),
						U", tmax = ", token -> tmax);

				partialTokenText.clear();
			}
		}

		/*
			Insert trailing silence.
		*/
		if (allTokens [allTokens.size]. tmax < sound -> xmax) {
			Token *silence = allTokens.append();
			silence -> tmax = sound -> xmax;
			silence -> isPunctuation = false;
			silence -> isSilence = true;
			silence -> isNewWord = true;
			trace (U"Trailing silence token: ", allTokens.size, U": text = \"", silence -> text.get(),
					U"\", tmax = ", silence -> tmax);
		}

		/*
			Build word and sentence segments from the flat token list.
		*/
		autoMelderString sentence_text;
		autoMelderString full_text;
		autovector <WhisperSegment> words = newvectorzero <WhisperSegment> (0);
		autovector <WhisperSegment> sentences = newvectorzero <WhisperSegment> (0);

		double token_tmin = sound -> xmin;   // default for first token
		double sentence_tmin = sound -> xmin;   // default for first sentence
		bool isFirstTokenInSentence = true;
		bool isFirstSentence = true;

		for (integer i = 1; i <= allTokens.size; ++ i) {
			mutablestring32 token_text = allTokens [i].text.get();
			double token_tmax = allTokens [i]. tmax;
			if (i > 1)
				token_tmin = allTokens [i - 1]. tmax;
			bool isSilentToken = allTokens [i].isSilence;
			bool isPunctuationToken = allTokens [i].isPunctuation;
			bool isNewWord = allTokens [i].isNewWord;
			trace (U"Token ", i, U": \"", token_text, U"\" [ ", token_tmin, U" - ", token_tmax, U" ], isSilentToken = ", isSilentToken);

			/*
				Add token text to the sentence and to the full transcription text, unless it is a silence token.
			*/
			if (! isSilentToken) {
				if (isNewWord && ! isFirstTokenInSentence) {
					MelderString_append (& sentence_text, U" ", token_text);
					MelderString_append (& full_text, U" ", token_text);
				} else if (isNewWord && ! isFirstSentence) {
					MelderString_append (& sentence_text, token_text);
					MelderString_append (& full_text, U" ", token_text);
				} else {
					MelderString_append (& sentence_text, token_text);
					MelderString_append (& full_text, token_text);
				}
			}

			/*
				Store start of sentence timestamp.
			*/
			if (isFirstTokenInSentence)
				sentence_tmin = token_tmin;

			/*
				Remove punctuation from the end of the token, preserving information about the end of the sentence.
			*/
			bool isLastTokenInSentence = false;
			if (endsWithTerminalPunctuation (token_text))
				isLastTokenInSentence = true;
			if (isPunctuationToken) {   // for any punctuation (terminal or not), divide it's interval between the right and left neighbours
				token_tmax = (token_tmax + token_tmin) / 2;
				allTokens [i]. tmax = token_tmax;
			}
			integer length_token_text = Melder_length (token_text);
			while (length_token_text > 0 && endsWithPunctuation (token_text)) {   // strip ALL trailing punctuation (e.g., all dots in ...)
				token_text [length_token_text - 1] = U'\0';
				-- length_token_text;
			}

			/*
				Create word-level segment.
			*/
			Melder_assert (token_tmax >= token_tmin);
			if ((isNewWord || words.size == 0) && token_tmax > token_tmin) {   // new word
				WhisperSegment *word = words.append();
				word -> text = Melder_dup (token_text);
				word -> tmin = token_tmin;
				word -> tmax = token_tmax;
				trace (U"Word ", words.size, U": \"", word -> text.get(), U"\" [ ", word -> tmin, U" - ", word -> tmax, U" ]");
			} else if (isNewWord && token_tmax == token_tmin && words.size > 0) {   // zero-length new word: append to previous with a space
				WhisperSegment & word = words [words.size];
				word.text = Melder_dup (Melder_cat (word.text.get(), U" ", token_text));
				word.tmax = token_tmax;
				trace (U"Word ", words.size, U": \"", word . text.get(), U"\" [ ", word . tmin, U" - ", word . tmax, U" ] (appended zero-length)");
			} else if (words.size > 0) {   // continuation token: append to the last word
				WhisperSegment & word = words [words.size];
				word.text = Melder_dup (Melder_cat (word.text.get(), token_text));
				word.tmax = token_tmax;
				trace (U"Word ", words.size, U": \"", word . text.get(), U"\" [ ", word . tmin, U" - ", word . tmax, U" ]");
			}

			/*
				Finalize and store sentence segment if sentence is complete.
			*/
			bool isLastTokenOverall = (i == allTokens.size);
			if (isLastTokenInSentence || isLastTokenOverall || (isSilentToken && isFirstTokenInSentence)) {
				WhisperSegment *sentence = sentences.append();
				sentence -> text = Melder_dup (sentence_text.string);
				sentence -> tmin = sentence_tmin;
				sentence -> tmax = token_tmax;
				trace (U"Sentence: ", sentences.size, U": \"", sentence -> text.get(), U"\" [ ", sentence -> tmin, U" - ", sentence -> tmax, U" ]");
				MelderString_empty (& sentence_text);
				isFirstTokenInSentence = true;   // current sentence is finalized, start with the new one on the next iteration
				if (isFirstSentence && ! isSilentToken)
					isFirstSentence = false;
			} else {
				isFirstTokenInSentence = false;   // continue with the current sentence
			}
		}

		WhisperTranscription transcription;
		transcription.fullTranscription.text = Melder_dup (full_text.string);
		transcription.fullTranscription.tmin = sound -> xmin;
		transcription.fullTranscription.tmax = sound -> xmax;
		transcription.words = words.move();
		transcription.sentences = sentences.move();

		trace (U"Full transcription:",
				U" [ ", transcription.fullTranscription.tmin, U" - ",
				transcription.fullTranscription.tmax, U" ]", transcription.fullTranscription.text.get());

		if (diarize) {
			transcription.speakers = doDiarization(sound).move();
		} else {
			transcription.speakers = newvectorzero <autovector <WhisperSegment>> (0);
		}
		return transcription;

	} catch (MelderError) {
		Melder_throw (U"Sound not transcribed.");
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
			const uint8 nLanguages = whisper_lang_max_id();
			for (uint8 i = 0; i < nLanguages; i ++) {
				autostring32 languageName = Melder_8to32_e (whisper_lang_str_full (i));

				/*
					Capitalize the first letter, e.g. "dutch" -> "Dutch".
				*/
				if (languageName [0] != U'\0')
					languageName [0] = Melder_toUpperCase (languageName [0]);

				unsorted.append (languageName.get());
			}
			autoSTRVEC sorted = sort_STRVEC (unsorted.get());

			/*
				Create a list to return (with autodetect option).
			*/
			sortedWhisperLanguageNames. append (U"Autodetect language");
			for (integer i = 1; i <= sorted.size; i ++)
				sortedWhisperLanguageNames. append (sorted [i].get());

		} catch (MelderError) {
			unsorted.reset();
			sortedWhisperLanguageNames.reset();
			Melder_clearError ();
		}
	}
	return sortedWhisperLanguageNames.get();
}

/* End of file SpeechRecognizer.cpp */
