/* whisper_espeak_stubs.cpp
 *
 * Minimal stubs for whisper, diarize, and espeak symbols needed when building
 * the core praat-wasm without the full espeak/whisper libraries.
 * This saves ~58 MB of model data and synthesis tables.
 *
 * Speech recognition and synthesis will not work at runtime.
 * All other Praat functions work normally.
 */

#include "../external/whispercpp/whisper.h"
#include "../external/whispercpp/diarize.h"
#include "../dwsys/FileInMemory.h"
#include <cstring>

/* ======================== Whisper model data ======================== */

unsigned char model_ggml_segmentation_data [] = { 0 };
unsigned int model_ggml_segmentation_length = 0;
unsigned char model_ggml_embedding_data [] = { 0 };
unsigned int model_ggml_embedding_length = 0;

/* ======================== Whisper API stubs ======================== */

void whisper_log_set (ggml_log_callback cb, void *ud) {
	(void) cb; (void) ud;
}

struct whisper_context_params whisper_context_default_params () {
	struct whisper_context_params p;
	memset (&p, 0, sizeof (p));
	return p;
}

struct whisper_context * whisper_init_from_file_with_params (
	const char *path, struct whisper_context_params params)
{
	(void) path; (void) params;
	return nullptr;
}

void whisper_free (struct whisper_context *ctx) {
	(void) ctx;
}

int whisper_is_multilingual (struct whisper_context *ctx) {
	(void) ctx;
	return 0;
}

int whisper_lang_max_id () {
	return 0;
}

int whisper_lang_id (const char *lang) {
	(void) lang;
	return -1;
}

const char * whisper_lang_str (int id) {
	(void) id;
	return "";
}

const char * whisper_lang_str_full (int id) {
	(void) id;
	return "";
}

whisper_token whisper_token_eot (struct whisper_context *ctx) {
	(void) ctx;
	return 0;
}

void whisper_print_timings (struct whisper_context *ctx) {
	(void) ctx;
}

struct whisper_full_params whisper_full_default_params (enum whisper_sampling_strategy strategy) {
	(void) strategy;
	struct whisper_full_params p;
	memset (&p, 0, sizeof (p));
	return p;
}

int whisper_full (struct whisper_context *ctx, struct whisper_full_params params,
	const float *samples, int n_samples)
{
	(void) ctx; (void) params; (void) samples; (void) n_samples;
	return -1;
}

int whisper_full_n_segments (struct whisper_context *ctx) {
	(void) ctx;
	return 0;
}

int whisper_full_n_tokens (struct whisper_context *ctx, int i_segment) {
	(void) ctx; (void) i_segment;
	return 0;
}

const char * whisper_full_get_token_text (struct whisper_context *ctx, int i_segment, int i_token) {
	(void) ctx; (void) i_segment; (void) i_token;
	return "";
}

whisper_token_data whisper_full_get_token_data (struct whisper_context *ctx, int i_segment, int i_token) {
	(void) ctx; (void) i_segment; (void) i_token;
	whisper_token_data d;
	memset (&d, 0, sizeof (d));
	return d;
}

int whisper_full_n_vad_segments (struct whisper_context *ctx) {
	(void) ctx;
	return 0;
}

int64_t whisper_full_get_vad_segment_orig_start (struct whisper_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0;
}

int64_t whisper_full_get_vad_segment_orig_end (struct whisper_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0;
}

int64_t whisper_full_get_vad_segment_vad_start (struct whisper_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0;
}

int64_t whisper_full_get_vad_segment_vad_end (struct whisper_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0;
}

/* ======================== Whisper VAD stubs ======================== */

struct whisper_vad_params whisper_vad_default_params () {
	struct whisper_vad_params p;
	memset (&p, 0, sizeof (p));
	return p;
}

struct whisper_vad_context_params whisper_vad_default_context_params () {
	struct whisper_vad_context_params p;
	memset (&p, 0, sizeof (p));
	return p;
}

struct whisper_vad_context * whisper_vad_init_from_memory_with_params (
	const void *data, size_t size, struct whisper_vad_context_params params)
{
	(void) data; (void) size; (void) params;
	return nullptr;
}

void whisper_vad_free (struct whisper_vad_context *ctx) {
	(void) ctx;
}

void whisper_vad_free_segments (struct whisper_vad_segments *segs) {
	(void) segs;
}

struct whisper_vad_segments * whisper_vad_segments_from_samples (
	struct whisper_vad_context *ctx, struct whisper_vad_params params,
	const float *samples, int n_samples)
{
	(void) ctx; (void) params; (void) samples; (void) n_samples;
	return nullptr;
}

float whisper_vad_segments_get_segment_t0 (struct whisper_vad_segments *segs, int i) {
	(void) segs; (void) i;
	return 0.0f;
}

float whisper_vad_segments_get_segment_t1 (struct whisper_vad_segments *segs, int i) {
	(void) segs; (void) i;
	return 0.0f;
}

int whisper_vad_segments_n_segments (struct whisper_vad_segments *segs) {
	(void) segs;
	return 0;
}

/* ======================== Diarize stubs ======================== */

struct diarize_context * diarize_init_from_memory (
	const void *seg_data, size_t seg_size,
	const void *emb_data, size_t emb_size)
{
	(void) seg_data; (void) seg_size; (void) emb_data; (void) emb_size;
	return nullptr;
}

struct diarize_params diarize_default_params () {
	struct diarize_params p;
	memset (&p, 0, sizeof (p));
	return p;
}

void diarize_free (struct diarize_context *ctx) {
	(void) ctx;
}

void diarize_full (struct diarize_context *ctx, struct diarize_params params,
	const float *samples, int n_samples)
{
	(void) ctx; (void) params; (void) samples; (void) n_samples;
}

unsigned int diarize_full_n_segments (struct diarize_context *ctx) {
	(void) ctx;
	return 0;
}

int diarize_full_n_speakers (struct diarize_context *ctx) {
	(void) ctx;
	return 0;
}

float diarize_full_get_segment_t0 (struct diarize_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0.0f;
}

float diarize_full_get_segment_t1 (struct diarize_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0.0f;
}

int diarize_full_get_segment_speaker (struct diarize_context *ctx, int i) {
	(void) ctx; (void) i;
	return 0;
}

/* ======================== eSpeak stubs ======================== */

#include "../external/espeak/espeak_ng.h"
#include "../external/espeak/voice.h"
#include "../external/espeak/synthdata.h"
#include "../external/espeak/espeak_praat.h"

extern "C" {
	int option_phoneme_events = 0;
}

/*
	'voice' is a global variable of type voice_t* in espeak.
	It's declared in voice.h.
*/
voice_t *voice = nullptr;

void espeak_SetSynthCallback (t_espeak_callback cb) {
	(void) cb;
}

espeak_ERROR espeak_Terminate () {
	return EE_OK;
}

espeak_ng_STATUS espeak_ng_Initialize (espeak_ng_ERROR_CONTEXT *context) {
	(void) context;
	return ENS_OK;
}

espeak_ng_STATUS espeak_ng_InitializeOutput (espeak_ng_OUTPUT_MODE mode, int buflen, const char *device) {
	(void) mode; (void) buflen; (void) device;
	return ENS_OK;
}

void espeak_ng_InitializePath (const char *path) {
	(void) path;
}

espeak_ng_STATUS espeak_ng_SetParameter (espeak_PARAMETER param, int value, int relative) {
	(void) param; (void) value; (void) relative;
	return ENS_OK;
}

espeak_ng_STATUS espeak_ng_SetVoiceByName (const char *name) {
	(void) name;
	return ENS_OK;
}

espeak_ng_STATUS espeak_ng_Synthesize (const void *text, size_t size,
	unsigned int position, espeak_POSITION_TYPE position_type, unsigned int end_position,
	unsigned int flags, unsigned int *unique_identifier, void *user_data)
{
	(void) text; (void) size; (void) position; (void) position_type;
	(void) end_position; (void) flags; (void) unique_identifier; (void) user_data;
	return ENS_OK;
}

espeak_ng_STATUS espeak_ng_Terminate () {
	return ENS_OK;
}

FileInMemorySet theEspeakPraatFileInMemorySet () {
	static autoFileInMemorySet theSet;
	if (! theSet) {
		theSet = FileInMemorySet_create ();
		/*
			classSpeechSynthesizer_initClass() requires at least one "language" file
			(path containing "./data/lang/") and one "voice" file (path containing "/voices/!v/").
			Without these, Melder_assert(numberOfMatches > 0) fails.
			We provide minimal dummy entries so that the SpeechSynthesizer class
			can be registered even though actual synthesis is unavailable.
		*/
		{// scope: dummy language file
			static uint8 langData [] = "name English (stub)\n";
			autoFileInMemory fim = FileInMemory_createWithData (
				(integer) (sizeof (langData) - 1),   // exclude null terminator
				langData,
				true,   // isStaticData
				U"./data/lang/en"
			);
			theSet -> addItem_move (fim.move ());
		}
		{// scope: dummy voice file
			static uint8 voiceData [] = "name Female1\ngender 2\nage 0\nvariant 0\n";
			autoFileInMemory fim = FileInMemory_createWithData (
				(integer) (sizeof (voiceData) - 1),
				voiceData,
				true,
				U"./data/voices/!v/Female1"
			);
			theSet -> addItem_move (fim.move ());
		}
	}
	return theSet.get ();
}

int LookupPhonemeTable (const char *name) {
	(void) name;
	return -1;
}

espeak_ng_STATUS DoVoiceChange (voice_t *v) {
	(void) v;
	return ENS_OK;
}
