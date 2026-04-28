/* diarize.h
*
 * Copyright (C) 2026 Anastasia Shchupak
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

/*
 *  Speaker diarization API for Praat integration
 *
 * Provides a C API following the same pattern as whisper.h:
 *   - Context init/free (loads segmentation + embedding models)
 *   - diarize_full() runs the entire pipeline on audio samples
 *   - Accessor functions to read results (segments with speaker labels)
 */

#ifndef DIARIZE_H
#define DIARIZE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque context — holds loaded models and last result
struct diarize_context;

// Parameters for diarization
struct diarize_params {
    // Segmentation
    float   seg_duration;       // chunk duration in seconds (default: 10.0)
    float   seg_step_ratio;     // step as fraction of duration (default: 0.1 = 90% overlap)

    // Clustering
    float   cluster_threshold;  // agglomerative clustering threshold (default: 0.7046)
    int     cluster_min_size;   // minimum cluster size (default: 12)

    // Embedding filter
    float   min_active_ratio;   // minimum non-overlapping activity to include (default: 0.2)

    // Output
    int     max_speakers;       // upper bound on number of speakers (default: 20)
};

// Get default parameters
struct diarize_params diarize_default_params(void);


// Model loader — abstraction for reading from file or memory
struct diarize_model_loader {
	void * context;

	size_t (*read) (void * ctx, void * output, size_t read_size);
	bool   (*eof)  (void * ctx);
	void   (*close)(void * ctx);
};

// Various functions for loading both models. Return NULL on failure
struct diarize_context * diarize_init_from_file(const char * seg_model_path, const char * emb_model_path);
struct diarize_context * diarize_init_from_memory(const void * seg_data, size_t seg_size, const void * emb_data, size_t emb_size);
struct diarize_context * diarize_init(struct diarize_model_loader * seg_loader, struct diarize_model_loader * emb_loader);

// Free context and all associated memory
void diarize_free(struct diarize_context * ctx);

// Run the full diarization pipeline on audio samples
// samples: float array, 16kHz mono, normalized to [-1, 1]
// n_samples: number of samples
// Throws MelderError on failure
void diarize_full(
    struct diarize_context * ctx,
    struct diarize_params    params,
    const float            * samples,
    int                      n_samples);

// --- Result accessors ---

// Number of segments in the last result
unsigned int diarize_full_n_segments(struct diarize_context * ctx);

// Number of speakers detected in the last result
int diarize_full_n_speakers(struct diarize_context * ctx);

// Get start time (seconds) of segment i
float diarize_full_get_segment_t0(struct diarize_context * ctx, int i_segment);

// Get end time (seconds) of segment i
float diarize_full_get_segment_t1(struct diarize_context * ctx, int i_segment);

// Get speaker ID (0-indexed) of segment i
int diarize_full_get_segment_speaker(struct diarize_context * ctx, int i_segment);

#ifdef __cplusplus
}
#endif

#endif // DIARIZE_H
