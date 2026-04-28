#ifndef _SpeechRecognizer_h_
#define _SpeechRecognizer_h_
/* SpeechRecognizer.h
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

#include "Sound.h"
#include <set>

struct structSpeechRecognizer;
struct whisper_context;
struct whisper_vad_context;
struct whisper_vad_segments;
struct diarize_context;

/*
	Default Whisper model parameters.
*/
inline conststring32 theSpeechRecognizerDefaultModelName = U"ggml-base.bin";
inline conststring32 theSpeechRecognizerDefaultLanguageName = U"Autodetect language";

/*
	Default Silero-VAD parameters.
*/
inline constexpr double theVadDefaultThreshold = 0.5;
inline conststring32 theVadDefaultThresholdStr = U"0.5";   // for UI
inline constexpr double theVadDefaultMinSpeechDuration = 0.25;
inline conststring32 theVadDefaultMinSpeechDurationStr = U"0.25";   // for UI
inline constexpr double theVadDefaultMinNonSpeechDuration = 0.1;
inline conststring32 theVadDefaultMinNonSpeechDurationStr = U"0.1";   // for UI
inline constexpr double theVadDefaultSpeechPad = 0.03;
inline conststring32 theVadDefaultSpeechPadStr = U"0.03";   // for UI
inline conststring32 theVadDefaultSpeechLabel = U"speech";   // for UI
inline conststring32 theVadDefaultNonSpeechLabel = U"non-speech";   // for UI

struct autoWhisperContext {
	whisper_context *ptr;

	autoWhisperContext (whisper_context * p = nullptr) : ptr(p) {}
	~autoWhisperContext ();

	autoWhisperContext (const autoWhisperContext&) = delete;
	autoWhisperContext& operator= (const autoWhisperContext&) = delete;

	autoWhisperContext (autoWhisperContext&& other) noexcept : ptr(other.ptr) {
		other.ptr = nullptr;
	}
	autoWhisperContext& operator= (autoWhisperContext&& other) noexcept;

	[[nodiscard]]
	whisper_context * get () const { return ptr; }
};

struct autoWhisperVadContext {
	whisper_vad_context * ptr;

	autoWhisperVadContext (whisper_vad_context * p = nullptr) : ptr (p) {}
	~autoWhisperVadContext ();

	autoWhisperVadContext (const autoWhisperVadContext &) = delete;
	autoWhisperVadContext & operator= (const autoWhisperVadContext &) = delete;

	autoWhisperVadContext (autoWhisperVadContext && other) noexcept : ptr (other.ptr) {
		other.ptr = nullptr;
	}
	autoWhisperVadContext & operator= (autoWhisperVadContext && other) noexcept;

	[[nodiscard]]
	whisper_vad_context * get () const { return ptr; }
};

struct autoWhisperVadSegments {
	whisper_vad_segments * ptr;

	autoWhisperVadSegments (whisper_vad_segments * p = nullptr) : ptr (p) {}
	~autoWhisperVadSegments ();

	autoWhisperVadSegments (const autoWhisperVadSegments &) = delete;
	autoWhisperVadSegments & operator= (const autoWhisperVadSegments &) = delete;

	autoWhisperVadSegments (autoWhisperVadSegments && other) noexcept : ptr (other.ptr) {
		other.ptr = nullptr;
	}
	autoWhisperVadSegments & operator= (autoWhisperVadSegments && other) noexcept;

	[[nodiscard]]
	whisper_vad_segments * get () const { return ptr; }
};

struct autoDiarizeContext {
	diarize_context *ptr;

	autoDiarizeContext (diarize_context * p = nullptr) : ptr(p) {}
	~autoDiarizeContext ();

	autoDiarizeContext (const autoDiarizeContext&) = delete;
	autoDiarizeContext& operator= (const autoDiarizeContext&) = delete;

	autoDiarizeContext (autoDiarizeContext&& other) noexcept : ptr(other.ptr) {
		other.ptr = nullptr;
	}
	autoDiarizeContext& operator= (autoDiarizeContext&& other) noexcept;

	[[nodiscard]]
	diarize_context * get () const { return ptr; }
};


struct SileroVadParams {
	double speechProbabilityThreshold = theVadDefaultThreshold;   // probability threshold to decide that sound is speech
	double minSpeechDuration = theVadDefaultMinSpeechDuration;   // min duration of a speech segment
	double minNonSpeechDuration = theVadDefaultMinNonSpeechDuration;   // min duration of a non-speech segment
	double speechPad = theVadDefaultSpeechPad;   // padding added before and after each speech segment
};

/*
	This should be extended, and also store the default labels above.
*/
struct DiarizationParams {
	float segmentDuration = 10.0f;
};

struct WhisperSegment {
	autostring32 text;
	double tmin;
	double tmax;
};

struct WhisperTranscription {
	WhisperSegment fullTranscription;
	autovector <WhisperSegment> words;
	autovector <WhisperSegment> sentences;
	autovector <autovector <WhisperSegment>> speakers;
};

inline std::set<structSpeechRecognizer *> theLivingSpeechRecognizers;
#include "SpeechRecognizer_def.h"

/*
	Functions to access lists of models and languages from everywhere.
*/
constSTRVEC theCurrentSpeechRecognizerModelNames ();
constSTRVEC theSpeechRecognizerLanguageNames ();

/*
	Class SpeechRecognizer functions.
*/
autoSpeechRecognizer SpeechRecognizer_create (conststring32 modelName, conststring32 languageName);
WhisperTranscription SpeechRecognizer_recognize (SpeechRecognizer me, constSound sound,
		bool useVad, const SileroVadParams &sileroVadParams, bool diarize);

/*
	Silero-VAD functions.
*/
autovector <WhisperSegment> doSileroVad (constSound sound, const SileroVadParams &sileroVadParams,
		conststring32 nonSpeechLabel, conststring32 speechLabel);

/*
	Diarization functions.
*/
autovector <autovector <WhisperSegment>> doDiarization (constSound sound);

/* End of file SpeechRecognizer.h */
#endif
