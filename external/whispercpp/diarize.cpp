/* diarize.cpp
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

/* Speaker diarization pipeline in C++.
 * ------------------------------------
 * Reimplements pyannote.audio SpeakerDiarization pipeline:
 *   Segmentation (pyannote/segmentation-3.0):
 *     WavNorm -> SincNet (GGML graph) -> LSTM (plain C++) -> Linear -> Classifier -> Log-softmax
 *     Input: 10s waveform at 16kHz -> Output: 589 frames × 7 powerset classes
 *
 *   Embedding (wespeaker-voxceleb-resnet34-LM):
 *     Fbank -> ResNet34 (GGML conv2d + plain C++) -> TSTP pooling -> Linear
 *     Input: variable-length waveform -> Output: 256-d speaker embedding
 *
 *   Clustering:
 *     AgglomerativeClustering (centroid linkage, threshold=0.7046)
 *
 * Pipeline flow:
 *   1. Sliding window segmentation (10s chunks, 90% overlap)
 *   2. Powerset -> multilabel conversion (7 classes -> 3 speakers)
 *   3. Embedding extraction per (chunk, speaker) pair with mask
 *   4. Filter embeddings (activity ≥ 20%, no NaN)
 *   5. Agglomerative clustering (L2-norm -> centroid linkage -> fcluster)
 *   6. Assign all embeddings to clusters
 *   7. Reconstruct: remap local -> global speakers
 *   8. Aggregate overlapping windows
 *   9. Top-N speaker selection per frame
 *  10. Frame-level -> time-stamped speaker segments
 */

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "ggml.h"
#include "ggml-alloc.h"
#include "ggml-backend.h"
#include "ggml-cpu.h"
#include "diarize.h"
#include "melder.h"

#define GGML_FILE_MAGIC 0x67676d6c

/*
	Maximum size for GGML computation graphs.
	sincnet_forward builds a graph with 58 nodes (44 nodes and 14 leafs).
	conv2d_forward builds a graph with 9 nodes (7 nodes and 2 leafs).
	These numbers were found out by including the following print after ggml_build_forward_expand():
		GGML_LOG_INFO("sincnet_forward: graph nodes = %d, leafs = %d\n", ggml_graph_n_nodes(gf), gf->n_leafs);
	These number of nodes are rounded up to the next power of 2.
*/
#define SINCNET_MAX_NODES 64
#define CONV2D_MAX_NODES 16

// ============================================================================
// Big-endian support
// ============================================================================
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
template<typename T>
static T byteswap(T value) {
	T swapped;
	char * src = reinterpret_cast<char *>(&value);
	char * dst = reinterpret_cast<char *>(&swapped);
	for (int i = 0; i < (int)sizeof(T); i++) {
		dst[sizeof(T) - 1 - i] = src[i];
	}
	return swapped;
}

#define BYTESWAP_VALUE(d) d = byteswap(d)

static void byteswap_tensor(ggml_tensor * tensor) {
	if (tensor->type == GGML_TYPE_F32) {
		auto * data = reinterpret_cast<float *>(tensor->data);
		for (int i = 0; i < ggml_nelements(tensor); i++) data[i] = byteswap(data[i]);
	} else if (tensor->type == GGML_TYPE_F16) {
		auto * data = reinterpret_cast<ggml_fp16_t *>(tensor->data);
		for (int i = 0; i < ggml_nelements(tensor); i++) data[i] = byteswap(data[i]);
	} else if (tensor->type == GGML_TYPE_I32) {
		auto * data = reinterpret_cast<int32_t *>(tensor->data);
		for (int i = 0; i < ggml_nelements(tensor); i++) data[i] = byteswap(data[i]);
	}
	// GGML_TYPE_I8 needs no swap
}

#define BYTESWAP_TENSOR(t) byteswap_tensor(t)
#else
#define BYTESWAP_VALUE(d) do {} while (0)
#define BYTESWAP_TENSOR(t) do {} while (0)
#endif

// ============================================================================
// Model loading
// ============================================================================
template<typename T>
static void read_safe(diarize_model_loader * loader, T & dest) {
	loader->read(loader->context, &dest, sizeof(T));
	BYTESWAP_VALUE(dest);
}

// ============================================================================
// Segmentation: Model structs
// ============================================================================
// Hyperparameters of the PyanNet segmentation model.
// Values are read from the model file header and overwrite these defaults.
// Defaults here match pyannote/segmentation-3.0.
struct segmentation_hparams {
	// SincNet block 0 (SincConv — learned bandpass filterbank):
	// 80 bandpass filters (40 cos + 40 sin), each 251 samples long, applied with stride 10.
	int32_t sincnet_filters_0  = 80;
    int32_t sincnet_kernel_0   = 251;
    int32_t sincnet_stride_0   = 10;

	// SincNet block 1 (regular Conv1d):
	// Standard convolution 80→60 channels, kernel 5, stride 1.
    int32_t sincnet_filters_1  = 60;
    int32_t sincnet_kernel_1   = 5;

	// SincNet block 2 (regular Conv1d):
	// Standard convolution 60→60 channels, kernel 5, stride 1.
	int32_t sincnet_filters_2  = 60;
    int32_t sincnet_kernel_2   = 5;

	// LSTM: processes the 589×60 SincNet output as a sequence.
	int32_t lstm_input_size    = 60;
    int32_t lstm_hidden_size   = 128;   // hidden state size per direction
    int32_t lstm_layers        = 4;   // number of stacked LSTM layers
    int32_t lstm_bidirectional = 1;   // 1 = bidirectional (forward + reverse)\

    // Feed-forward layers after LSTM:
    int32_t linear_hidden      = 128;   // hidden size of each linear layer
    int32_t linear_layers      = 2;   // number of linear layers (each with LeakyReLU)

	// Powerset classifier (final output layer, no activation — log-softmax applied after):
	int32_t n_classes          = 7;    // powerset classes for up to 3 speakers

	// Derived
	int n_filters() const { return sincnet_filters_0; }                          // 80
	int n_sinc_params() const { return sincnet_filters_0 / 2; }                  // 40 (learned frequency pairs)
	int half_kernel() const { return sincnet_kernel_0 / 2; }                     // 125 (symmetric kernel half)
	int lstm_output_size() const { return lstm_hidden_size * (lstm_bidirectional ? 2 : 1); } // 256 features per frame
};

struct segmentation_model {
    segmentation_hparams hparams;

	// Waveform normalization (InstanceNorm1d on raw input, 1 channel)
	struct ggml_tensor * sincnet_wav_norm_w;    // [1]
	struct ggml_tensor * sincnet_wav_norm_b;    // [1]

	// SincConv learned filterbank parameters (block 0)
	struct ggml_tensor * sincnet_low_hz;        // [40]  low cutoff frequency per filter pair
	struct ggml_tensor * sincnet_band_hz;       // [40]  bandwidth per filter pair
	struct ggml_tensor * sincnet_window;        // [125] Hamming window (half-kernel)
	struct ggml_tensor * sincnet_n;             // [125] normalized frequency grid (half-kernel)

	// Conv1d weights (blocks 1–2; block 0 kernel is computed from the params above)
	struct ggml_tensor * sincnet_conv1_w;       // [5, 80, 60]
	struct ggml_tensor * sincnet_conv1_b;       // [60]
	struct ggml_tensor * sincnet_conv2_w;       // [5, 60, 60]
	struct ggml_tensor * sincnet_conv2_b;       // [60]

	// InstanceNorm affine parameters (one per block)
	struct ggml_tensor * sincnet_norm0_w;       // [80]
	struct ggml_tensor * sincnet_norm0_b;       // [80]
	struct ggml_tensor * sincnet_norm1_w;       // [60]
	struct ggml_tensor * sincnet_norm1_b;       // [60]
	struct ggml_tensor * sincnet_norm2_w;       // [60]
	struct ggml_tensor * sincnet_norm2_b;       // [60]

	// LSTM (4 layers × 2 directions, hsd = 4*hidden_size = 512)
	// Forward direction
	struct ggml_tensor * lstm_weight_ih[4];     // [in, 512]  input-to-hidden (in=60 for layer 0, 256 for layers 1–3)
	struct ggml_tensor * lstm_weight_hh[4];     // [128, 512] hidden-to-hidden
	struct ggml_tensor * lstm_bias_ih[4];       // [512]
	struct ggml_tensor * lstm_bias_hh[4];       // [512]
	// Reverse direction
	struct ggml_tensor * lstm_weight_ih_rev[4]; // [in, 512]
	struct ggml_tensor * lstm_weight_hh_rev[4]; // [128, 512]
	struct ggml_tensor * lstm_bias_ih_rev[4];   // [512]
	struct ggml_tensor * lstm_bias_hh_rev[4];   // [512]

	// Feed-forward layers (2 layers with LeakyReLU)
	struct ggml_tensor * linear_w[2];           // [256, 128] for layer 0, [128, 128] for layer 1
	struct ggml_tensor * linear_b[2];           // [128]

	// Powerset classifier
	struct ggml_tensor * classifier_w;             // [128, 7]
	struct ggml_tensor * classifier_b;             // [7]

	// Bookkeeping
	std::map<std::string, struct ggml_tensor *> tensors;
	int n_loaded = 0;
	std::vector<ggml_context *> ctxs;
	std::vector<ggml_backend_buffer_t> buffers;
};

struct segmentation_context {
    segmentation_model model;

    // Pre-computed SincNet filters [n_filters * kernel_size]
    std::vector<float> sinc_filters;

    // GGML backend for SincNet
    ggml_backend_t backend = nullptr;
};

// ============================================================================
// Segmentation: Model loading
// ============================================================================
static void segmentation_load_model(diarize_model_loader * loader, segmentation_model & model) {
    //TRACE
	uint32_t magic;
	read_safe(loader, magic);
	if (magic != GGML_FILE_MAGIC)
		Melder_throw(U"Invalid model data (bad magic).");

    auto & hp = model.hparams;
    read_safe(loader, hp.sincnet_filters_0);
	read_safe(loader, hp.sincnet_kernel_0);
    read_safe(loader, hp.sincnet_stride_0);
	read_safe(loader, hp.sincnet_filters_1);
    read_safe(loader, hp.sincnet_kernel_1);
	read_safe(loader, hp.sincnet_filters_2);
    read_safe(loader, hp.sincnet_kernel_2);
	read_safe(loader, hp.lstm_input_size);
    read_safe(loader, hp.lstm_hidden_size);
	read_safe(loader, hp.lstm_layers);
    read_safe(loader, hp.lstm_bidirectional);
	read_safe(loader, hp.linear_hidden);
    read_safe(loader, hp.linear_layers);
	read_safe(loader, hp.n_classes);

    trace (U"sincnet=", hp.sincnet_filters_0, U"/", hp.sincnet_kernel_0, U"/", hp.sincnet_stride_0,
            U" lstm=", hp.lstm_layers, U"x", hp.lstm_hidden_size, U"(bidir=", hp.lstm_bidirectional, U")",
            U" linear=", hp.linear_layers, U"x", hp.linear_hidden,
            U" classes=", hp.n_classes, U"");

    const int nsp = hp.n_sinc_params();
	const int hk = hp.half_kernel();
	const int hsd = hp.lstm_hidden_size * 4;
    const size_t nt = 16 + 8 * hp.lstm_layers + 2 * hp.linear_layers + 2;

    std::map<ggml_backend_buffer_type_t, ggml_context*> ctx_map;
    auto cpu_buft = ggml_backend_cpu_buffer_type();

    auto get_ctx = [&](ggml_backend_buffer_type_t buft) -> ggml_context* {
        auto it = ctx_map.find(buft);
        if (it == ctx_map.end()) {
            ggml_init_params p = {
	            nt * ggml_tensor_overhead(),
            	nullptr,
            	true
            };
            auto c = ggml_init(p);
        	ctx_map[buft] = c;
        	model.ctxs.push_back(c);
        	return c;
        }
        return it->second;
    };

    auto CT = [&](const std::string & name, ggml_tensor * meta) -> ggml_tensor* {
        auto c = get_ctx(cpu_buft);
        auto t = ggml_dup_tensor(c, meta);
        ggml_set_name(t, name.c_str());
        model.tensors[name] = t;
        return t;
    };

    {
        ggml_init_params mp = {
	        nt * ggml_tensor_overhead(),
        	nullptr,
        	true
        };
        auto ctx = ggml_init(mp);

        // SincNet
        model.sincnet_wav_norm_w = CT("sincnet.wav_norm1d.weight", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, 1));
        model.sincnet_wav_norm_b = CT("sincnet.wav_norm1d.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, 1));
        model.sincnet_low_hz  = CT("sincnet.conv1d.0.filterbank.low_hz_",  ggml_new_tensor_1d(ctx, GGML_TYPE_F32, nsp));
        model.sincnet_band_hz = CT("sincnet.conv1d.0.filterbank.band_hz_", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, nsp));
        model.sincnet_window  = CT("sincnet.conv1d.0.filterbank.window_",  ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hk));
        model.sincnet_n       = CT("sincnet.conv1d.0.filterbank.n_",       ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hk));

        model.sincnet_conv1_w = CT("sincnet.conv1d.1.weight", ggml_new_tensor_3d(ctx, GGML_TYPE_F32, hp.sincnet_kernel_1, hp.sincnet_filters_0, hp.sincnet_filters_1));
        model.sincnet_conv1_b = CT("sincnet.conv1d.1.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_1));
        model.sincnet_conv2_w = CT("sincnet.conv1d.2.weight", ggml_new_tensor_3d(ctx, GGML_TYPE_F32, hp.sincnet_kernel_2, hp.sincnet_filters_1, hp.sincnet_filters_2));
        model.sincnet_conv2_b = CT("sincnet.conv1d.2.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_2));

        model.sincnet_norm0_w = CT("sincnet.norm1d.0.weight", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_0));
        model.sincnet_norm0_b = CT("sincnet.norm1d.0.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_0));
        model.sincnet_norm1_w = CT("sincnet.norm1d.1.weight", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_1));
        model.sincnet_norm1_b = CT("sincnet.norm1d.1.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_1));
        model.sincnet_norm2_w = CT("sincnet.norm1d.2.weight", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_2));
        model.sincnet_norm2_b = CT("sincnet.norm1d.2.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.sincnet_filters_2));

        // LSTM
        for (int l = 0; l < hp.lstm_layers; l++) {
            int in_sz = (l == 0) ? hp.lstm_input_size : hp.lstm_output_size();
            std::string ls = std::to_string(l);
            model.lstm_weight_ih[l]     = CT("lstm.weight_ih_l"+ls,           ggml_new_tensor_2d(ctx, GGML_TYPE_F32, in_sz, hsd));
            model.lstm_weight_hh[l]     = CT("lstm.weight_hh_l"+ls,           ggml_new_tensor_2d(ctx, GGML_TYPE_F32, hp.lstm_hidden_size, hsd));
            model.lstm_bias_ih[l]       = CT("lstm.bias_ih_l"+ls,             ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hsd));
            model.lstm_bias_hh[l]       = CT("lstm.bias_hh_l"+ls,             ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hsd));
            model.lstm_weight_ih_rev[l] = CT("lstm.weight_ih_l"+ls+"_reverse", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, in_sz, hsd));
            model.lstm_weight_hh_rev[l] = CT("lstm.weight_hh_l"+ls+"_reverse", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, hp.lstm_hidden_size, hsd));
            model.lstm_bias_ih_rev[l]   = CT("lstm.bias_ih_l"+ls+"_reverse",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hsd));
            model.lstm_bias_hh_rev[l]   = CT("lstm.bias_hh_l"+ls+"_reverse",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hsd));
        }

        // Linear
        int lo = hp.lstm_output_size();
        for (int l = 0; l < hp.linear_layers; l++) {
            int inf = (l == 0) ? lo : hp.linear_hidden;
            model.linear_w[l] = CT("linear."+std::to_string(l)+".weight", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, inf, hp.linear_hidden));
            model.linear_b[l] = CT("linear."+std::to_string(l)+".bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.linear_hidden));
        }

        // Classifier
        int ci = (hp.linear_layers > 0) ? hp.linear_hidden : lo;
        model.classifier_w = CT("classifier.weight", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, ci, hp.n_classes));
        model.classifier_b = CT("classifier.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.n_classes));

        ggml_free(ctx);
    }

    // Allocate buffers
    for (auto & p : ctx_map) {
        auto buf = ggml_backend_alloc_ctx_tensors_from_buft(p.second, p.first);
        if (buf)
        	model.buffers.push_back(buf);
    }

    // Load weights
	model.n_loaded = 0;
	std::vector<char> read_buf;
	while (!loader->eof(loader->context)) {
		int32_t n_dims, length, ttype;
		read_safe(loader, n_dims);
		read_safe(loader, length);
		read_safe(loader, ttype);
		if (loader->eof(loader->context)) break;

		int32_t n_elements = 1, ne[4] = {1,1,1,1};
		for (int i = 0; i < n_dims; i++) {
			read_safe(loader, ne[i]);
			n_elements *= ne[i];
		}

		std::string name(length, 0);
		loader->read(loader->context, name.data(), length);

		auto it = model.tensors.find(name);
		if (it == model.tensors.end())
		    Melder_throw(U": unknown tensor '", Melder_peek8to32_u(name.c_str()), U"'");

		auto tensor = it->second;
		const size_t bpe = ggml_type_size(ggml_type(ttype));

		if (ggml_backend_buffer_is_host(tensor->buffer)) {
			loader->read(loader->context, tensor->data, n_elements * bpe);
			BYTESWAP_TENSOR(tensor);
		} else {
			read_buf.resize(ggml_nbytes(tensor));
			loader->read(loader->context, read_buf.data(), read_buf.size());
			// For non-host buffers, we'd need to byteswap in the temp buffer
			// before copying to device, but as for now we are CPU-only, this path is not taken
			ggml_backend_tensor_set(tensor, read_buf.data(), 0, ggml_nbytes(tensor));
		}
		model.n_loaded++;
	}

    trace (U"Loaded ", model.n_loaded, U"/", model.tensors.size(), U" tensors");
    if (model.n_loaded != model.tensors.size())
        Melder_throw (U"Segmentation model incomplete: loaded ", model.n_loaded, U" of ", model.tensors.size(), U" tensors.");
}

// ============================================================================
// Segmentation: Pre-compute SincNet bandpass filters
// ============================================================================
static std::vector<float> compute_sinc_filters(const segmentation_model & model) {
    auto & hp = model.hparams;
    const int np = hp.n_sinc_params();
	const int hk = hp.half_kernel();
	const int ks = hp.sincnet_kernel_0;
	const int nf = hp.n_filters();

    const float mlh = 50.f;
	const float mbh = 50.f;
	const float sr = 16000.f;

    const float *lh = (const float*)model.sincnet_low_hz->data;
    const float *bh = (const float*)model.sincnet_band_hz->data;
    const float *w  = (const float*)model.sincnet_window->data;
    const float *n  = (const float*)model.sincnet_n->data;

    std::vector<float> f(nf * ks, 0.f);
    for (int i = 0; i < np; i++) {
        float lo = mlh + fabsf(lh[i]);   // low cutoff (minimum 50 Hz)
        float hi = lo + mbh + fabsf(bh[i]);   // high cutoff (at least 50 Hz above lo)
        if (hi > sr/2)
        	hi = sr/2;   // clamped to Nyquist (8000 Hz = 16000/2).
        float b = hi - lo;
    	float nm = 2.f * b;

        // Cos filter
        float *fc = &f[i * ks];
        for (int j = 0; j < hk; j++) fc[j] = (sinf(hi*n[j]) - sinf(lo*n[j])) / (n[j]/2.f) * w[j];
        fc[hk] = 2.f * b;
        for (int j = 0; j < hk; j++) fc[hk+1+j] = fc[hk-1-j];
        for (int j = 0; j < ks; j++) fc[j] /= nm;

        // Sin filter
        float *fs = &f[(np+i) * ks];
        for (int j = 0; j < hk; j++) fs[j] = (cosf(lo*n[j]) - cosf(hi*n[j])) / (n[j]/2.f) * w[j];
        fs[hk] = 0.f;
        for (int j = 0; j < hk; j++) fs[hk+1+j] = -fs[hk-1-j];
        for (int j = 0; j < ks; j++) fs[j] /= nm;
    }
    return f;
}

// ============================================================================
// Segmentation: F32 conv1d (avoids F16 im2col precision loss)
// ============================================================================
static ggml_tensor * ggml_conv_1d_f32(ggml_context * ctx, ggml_tensor * a, ggml_tensor * b,
                                       int s0, int p0, int d0) {
    auto im2col = ggml_im2col(ctx, a, b, s0, 0, p0, 0, d0, 0, false, GGML_TYPE_F32);
    auto result = ggml_mul_mat(ctx,
        ggml_reshape_2d(ctx, im2col, im2col->ne[0], im2col->ne[2] * im2col->ne[1]),
        ggml_reshape_2d(ctx, a, a->ne[0] * a->ne[1], a->ne[2]));
    return ggml_reshape_3d(ctx, result, im2col->ne[1], a->ne[2], im2col->ne[2]);
}

// ============================================================================
// Segmentation: SincNet forward pass (GGML graph)
//     Block 0: SincConv(80, k=251, s=10) -> abs -> MaxPool(3,s=3) -> InstanceNorm(80) -> LeakyReLU
//     Block 1: Conv1d(80->60, k=5) + bias -> MaxPool(3,s=3) -> InstanceNorm(60) -> LeakyReLU
//     Block 2: Conv1d(60->60, k=5) + bias -> MaxPool(3,s=3) -> InstanceNorm(60) -> LeakyReLU
// Returns output tensor from the graph after compute.
// Output shape: [ne0=time, ne1=channels] = [589, 60]
// ============================================================================
static bool sincnet_forward(segmentation_context & sctx,
                            const float * samples, int n_samples,
                            std::vector<float> & output, int & out_time, int & out_ch) {
    auto & model = sctx.model;
    auto & hp = model.hparams;

	ggml_init_params ctx_params = {
	    ggml_tensor_overhead() * SINCNET_MAX_NODES + ggml_graph_overhead(),
    	nullptr,
    	true
    };
    ggml_context * ctx0 = ggml_init(ctx_params);

    // Input
    ggml_tensor * inp = ggml_new_tensor_1d(ctx0, GGML_TYPE_F32, n_samples);
    ggml_set_name(inp, "input");
    ggml_set_input(inp);

    // Sinc filter weights
    ggml_tensor * sinc_w = ggml_new_tensor_3d(ctx0, GGML_TYPE_F32,hp.sincnet_kernel_0, 1, hp.n_filters());
    ggml_set_name(sinc_w, "sinc_w");
    ggml_set_input(sinc_w);

    // wav_norm1d
    ggml_tensor * cur = ggml_norm(ctx0, inp, 1e-5f);
    cur = ggml_mul(ctx0, cur, model.sincnet_wav_norm_w);
    cur = ggml_add(ctx0, cur, model.sincnet_wav_norm_b);

    // Helper lambda for one SincNet block
    auto block = [&](ggml_tensor * conv_w, ggml_tensor * conv_b, int stride, ggml_tensor * nw, ggml_tensor * nb,
                     int nch, bool do_abs) {
        cur = ggml_conv_1d_f32(ctx0, conv_w, cur, stride, 0, 1);
        if (conv_b) cur = ggml_add(ctx0, cur, ggml_reshape_3d(ctx0, conv_b, 1, nch, 1));
        if (do_abs) cur = ggml_abs(ctx0, cur);
        cur = ggml_pool_1d(ctx0, cur, GGML_OP_POOL_MAX, 3, 3, 0);
        cur = ggml_norm(ctx0, cur, 1e-5f);
        cur = ggml_mul(ctx0, cur, ggml_reshape_2d(ctx0, nw, 1, nch));
        cur = ggml_add(ctx0, cur, ggml_reshape_2d(ctx0, nb, 1, nch));
        cur = ggml_leaky_relu(ctx0, cur, 0.01f, false);
    };

    // Block 0: SincConv
    block(sinc_w, nullptr, hp.sincnet_stride_0,
          model.sincnet_norm0_w, model.sincnet_norm0_b, hp.sincnet_filters_0, true);

    // Block 1
    block(model.sincnet_conv1_w, model.sincnet_conv1_b, 1,
          model.sincnet_norm1_w, model.sincnet_norm1_b, hp.sincnet_filters_1, false);

    // Block 2
    block(model.sincnet_conv2_w, model.sincnet_conv2_b, 1,
          model.sincnet_norm2_w, model.sincnet_norm2_b, hp.sincnet_filters_2, false);

    ggml_set_name(cur, "sincnet_out");
    ggml_set_output(cur);

    // Build and compute
    ggml_cgraph * gf = ggml_new_graph_custom(ctx0, SINCNET_MAX_NODES, false);
    ggml_build_forward_expand(gf, cur);
	//GGML_LOG_INFO("sincnet_forward: graph nodes = %d, leafs = %d\n", ggml_graph_n_nodes(gf), gf->n_leafs);

    ggml_gallocr_t galloc = ggml_gallocr_new(ggml_backend_get_default_buffer_type(sctx.backend));
    ggml_gallocr_alloc_graph(galloc, gf);

    ggml_backend_tensor_set(inp, samples, 0, n_samples * sizeof(float));
    ggml_backend_tensor_set(sinc_w, sctx.sinc_filters.data(), 0,
                            sctx.sinc_filters.size() * sizeof(float));

    ggml_backend_graph_compute(sctx.backend, gf);

    out_time = (int)cur->ne[0];
    out_ch   = (int)cur->ne[1];
    int total = out_time * out_ch;
    output.resize(total);
    ggml_backend_tensor_get(cur, output.data(), 0, total * sizeof(float));

    ggml_gallocr_free(galloc);
    ggml_free(ctx0);
    return true;
}

// ============================================================================
// Segmentation: LSTM one direction (plain C++)
// ============================================================================
static void lstm_one_direction(
        const float * x, float * out,
        const int T, const int input_size, const int H,
        const float * w_ih, const float * w_hh,
        const float * b_ih, const float * b_hh,
        bool reverse) {
    std::vector<float> h(H, 0.f), c(H, 0.f), gates(4*H);

    for (int step = 0; step < T; step++) {
        int t = reverse ? (T-1-step) : step;
        const float * xt = &x[t * input_size];

        for (int g = 0; g < 4*H; g++) {
            float val = b_ih[g] + b_hh[g];
            for (int j = 0; j < input_size; j++) val += w_ih[g*input_size + j] * xt[j];
            for (int j = 0; j < H; j++)          val += w_hh[g*H + j] * h[j];
            gates[g] = val;
        }

        for (int j = 0; j < H; j++) {
            float ig = 1.f / (1.f + expf(-gates[0*H+j]));
            float fg = 1.f / (1.f + expf(-gates[1*H+j]));
            float gg = tanhf(gates[2*H+j]);
            float og = 1.f / (1.f + expf(-gates[3*H+j]));
            c[j] = fg * c[j] + ig * gg;
            h[j] = og * tanhf(c[j]);
        }
        for (int j = 0; j < H; j++) out[t*H + j] = h[j];
    }
}

// ============================================================================
// Segmentation: LSTM full forward pass (plain C++)
//		4-layer bidirectional LSTM
//		Input: [T, input_size] (e.g. [589, 60])
//		Output: [T, 2*H] (e.g. [589, 256])
// ============================================================================
static std::vector<float> lstm_forward(const segmentation_model & model, const float * input, int T) {
    auto & hp = model.hparams;
    const int H = hp.lstm_hidden_size;
    const int out_dim = hp.lstm_output_size();

    std::vector<float> cur(input, input + T * hp.lstm_input_size);
    int cur_input_size = hp.lstm_input_size;

    for (int l = 0; l < hp.lstm_layers; l++) {
        const float * w_ih_f = (const float*)model.lstm_weight_ih[l]->data;
        const float * w_hh_f = (const float*)model.lstm_weight_hh[l]->data;
        const float * b_ih_f = (const float*)model.lstm_bias_ih[l]->data;
        const float * b_hh_f = (const float*)model.lstm_bias_hh[l]->data;
        const float * w_ih_r = (const float*)model.lstm_weight_ih_rev[l]->data;
        const float * w_hh_r = (const float*)model.lstm_weight_hh_rev[l]->data;
        const float * b_ih_r = (const float*)model.lstm_bias_ih_rev[l]->data;
        const float * b_hh_r = (const float*)model.lstm_bias_hh_rev[l]->data;

        std::vector<float> out_fwd(T * H), out_rev(T * H);
        lstm_one_direction(cur.data(), out_fwd.data(), T, cur_input_size, H,
                           w_ih_f, w_hh_f, b_ih_f, b_hh_f, false);
        lstm_one_direction(cur.data(), out_rev.data(), T, cur_input_size, H,
                           w_ih_r, w_hh_r, b_ih_r, b_hh_r, true);

        std::vector<float> next(T * out_dim);
        for (int t = 0; t < T; t++) {
            for (int j = 0; j < H; j++) {
                next[t * out_dim + j]     = out_fwd[t * H + j];
                next[t * out_dim + H + j] = out_rev[t * H + j];
            }
        }
        cur = std::move(next);
        cur_input_size = out_dim;
    }
    return cur;
}

// ============================================================================
// Segmentation: Linear + Classifier + Activation (plain C++)
//   Linear: 2 layers, 256->128->128, LeakyReLU
//   Classifier: 128->7
//   Activation: log-softmax
// ============================================================================
static std::vector<float> linear_leaky_relu(const float * x, int T, int in_f, int out_f,
                                             const float * weight, const float * bias) {
    std::vector<float> out(T * out_f);
    for (int t = 0; t < T; t++) {
        for (int o = 0; o < out_f; o++) {
            float val = bias[o];
            for (int i = 0; i < in_f; i++) val += weight[o * in_f + i] * x[t * in_f + i];
            out[t * out_f + o] = (val < 0) ? 0.01f * val : val;
        }
    }
    return out;
}

static std::vector<float> linear_forward_raw(const float * x, int T, int in_f, int out_f,
                                              const float * weight, const float * bias) {
    std::vector<float> out(T * out_f);
    for (int t = 0; t < T; t++) {
        for (int o = 0; o < out_f; o++) {
            float val = bias[o];
            for (int i = 0; i < in_f; i++) val += weight[o * in_f + i] * x[t * in_f + i];
            out[t * out_f + o] = val;
        }
    }
    return out;
}

static std::vector<float> log_softmax(const float * x, const int T, const int C) {
    std::vector<float> out(T * C);
    for (int t = 0; t < T; t++) {
        const float * row = &x[t * C];
        float mx = row[0];
        for (int j = 1; j < C; j++) if (row[j] > mx) mx = row[j];
        float sum = 0.f;
        for (int j = 0; j < C; j++) sum += expf(row[j] - mx);
        float ls = mx + logf(sum);
        for (int j = 0; j < C; j++) out[t * C + j] = row[j] - ls;
    }
    return out;
}

// ============================================================================
// Segmentation: Full forward pass
// ============================================================================
// Input:  samples[n_samples] — 10s waveform at 16kHz (n_samples = 160000)
// Output: result[n_frames * n_classes] — log-softmax probabilities
//         n_frames is set to the actual frame count (589 for 10s input)
static bool segmentation_forward(segmentation_context & sctx, const float * samples, int n_samples,
		std::vector<float> & result, int & n_frames, int & n_classes) {
    //TRACE
    const auto & model = sctx.model;
    auto & hp = model.hparams;

    // 1. SincNet (GGML graph)
    std::vector<float> sincnet_out;
    int sn_time, sn_ch;
    if (!sincnet_forward(sctx, samples, n_samples, sincnet_out, sn_time, sn_ch)) {
        return false;
    }
    trace (U"SincNet output: [", sn_time, U", ", sn_ch, U"]");

    // SincNet output is [ne0=time, ne1=ch] in GGML memory order.
    // This is the same as [ch, time] in PyTorch.
    // PyanNet does: rearrange(sincnet_out, "batch feature frame -> batch frame feature")
    // So we need to transpose: [ch, time] -> [time, ch]
    // In memory: sincnet_out[c * time + t] -> lstm_in[t * ch + c]
    const int T = sn_time;
    std::vector<float> lstm_in(T * sn_ch);
    for (int t = 0; t < T; t++) {
        for (int c = 0; c < sn_ch; c++) {
            lstm_in[t * sn_ch + c] = sincnet_out[c * T + t];
        }
    }

    // 2. LSTM (plain C++)
    const auto lstm_out = lstm_forward(model, lstm_in.data(), T);
    trace (U"LSTM output: [", T, U", ", hp.lstm_output_size(), U"]");

    // 3. Linear layers
    const float * cur = lstm_out.data();
    int cur_dim = hp.lstm_output_size();

    std::vector<float> lin_out;
    for (int l = 0; l < hp.linear_layers; l++) {
        lin_out = linear_leaky_relu(cur, T, cur_dim, hp.linear_hidden,
            (const float*)model.linear_w[l]->data,
            (const float*)model.linear_b[l]->data);
        cur = lin_out.data();
        cur_dim = hp.linear_hidden;
    }

    // 4. Classifier
    auto cls_out = linear_forward_raw(cur, T, cur_dim, hp.n_classes,
        (const float*)model.classifier_w->data,
        (const float*)model.classifier_b->data);

    // 5. Log-softmax activation
    result = log_softmax(cls_out.data(), T, hp.n_classes);

    n_frames = T;
    n_classes = hp.n_classes;

    return true;
}

// ============================================================================
// Segmentation: Init / Free
// ============================================================================
static void segmentation_init(segmentation_context & sctx, diarize_model_loader * loader) {
    //TRACE
    try {
        segmentation_load_model(loader, sctx.model);
    } catch (MelderError) {
        Melder_throw (U"Could not load segmentation model.");
    }

	sctx.sinc_filters = compute_sinc_filters(sctx.model);
    sctx.backend = ggml_backend_cpu_init();

    trace (U"Initialized (", sctx.model.hparams.n_filters(), U" sinc filters, ", sctx.model.hparams.sincnet_kernel_0, U" kernel)");
}

static void segmentation_free(segmentation_context & sctx) {
    if (sctx.backend) ggml_backend_free(sctx.backend);
    for (auto buf : sctx.model.buffers) ggml_backend_buffer_free(buf);
    for (auto ctx : sctx.model.ctxs) ggml_free(ctx);
}


// ============================================================================
// WeSpeaker ResNet34 speaker embedding inference in C++
// Complete forward pass: Fbank extraction -> ResNet34 -> TSTP pooling -> Linear
//
// Architecture (wespeaker-voxceleb-resnet34-LM):
//   Fbank:  Kaldi-compatible, 80 mel bins, 25ms frame, 10ms shift, Hamming
//   Stem:   Conv2d(1->32, k=3, s=1, p=1) + BN + ReLU
//   Layer1: 3x BasicBlock(32->32, s=1)
//   Layer2: 4x BasicBlock(32->64, first s=2, shortcut)
//   Layer3: 6x BasicBlock(64->128, first s=2, shortcut)
//   Layer4: 3x BasicBlock(128->256, first s=2, shortcut)
//   TSTP:   temporal statistics pooling (mean+std) -> 5120
//   seg_1:  Linear(5120->256)
// Input:  float[n_samples] — raw PCM audio at 16kHz, normalized to [-1, 1]
// Output: float[256] — speaker embedding
// ============================================================================
// Embedding: Model structs
// ============================================================================
struct embedding_hparams {
    int32_t m_channels;       // 32
    int32_t feat_dim;         // 80 (mel bins)
    int32_t embed_dim;        // 256
    int32_t num_blocks[4];    // [3, 4, 6, 3]
    int32_t two_emb_layer;    // 0

    int ch(int layer) const { return m_channels * (1 << (layer - 1)); }
};

struct embedding_model {
    embedding_hparams hparams;
    std::map<std::string, struct ggml_tensor *> tensors;
    int n_loaded = 0;
    std::vector<ggml_context *> ctxs;
    std::vector<ggml_backend_buffer_t> buffers;
};

struct embedding_context {
    embedding_model model;
    ggml_backend_t backend = nullptr;
};

// ============================================================================
// Embedding: Model loading
// ============================================================================
static void embedding_load_model(diarize_model_loader * loader, embedding_model & model) {
    //TRACE
	uint32_t magic;
	read_safe(loader, magic);
	if (magic != GGML_FILE_MAGIC)
	    Melder_throw (U"Invalid model data (bad magic).");

    auto & hp = model.hparams;
    read_safe(loader, hp.m_channels);
    read_safe(loader, hp.feat_dim);
    read_safe(loader, hp.embed_dim);
    for (int i = 0; i < 4; i++)
    	read_safe(loader, hp.num_blocks[i]);
    read_safe(loader, hp.two_emb_layer);

    trace (U"m_ch=", hp.m_channels, U" feat=", hp.feat_dim, U" embed=", hp.embed_dim,
    		U" blocks=[", hp.num_blocks[0], U",", hp.num_blocks[1], U",", hp.num_blocks[2], U",", hp.num_blocks[3],
    		U"] two_emb=", hp.two_emb_layer);

    // Count tensors
    int total_blocks = hp.num_blocks[0] + hp.num_blocks[1] + hp.num_blocks[2] + hp.num_blocks[3];
    size_t n_tensors = 5 + total_blocks * 10 + 3 * 5 + 2;
    if (hp.two_emb_layer)
    	n_tensors += 2;

    // Create tensors
    std::map<ggml_backend_buffer_type_t, ggml_context *> ctx_map;
    ggml_backend_buffer_type_t cpu_buft = ggml_backend_cpu_buffer_type();

    auto get_ctx = [&](ggml_backend_buffer_type_t buft) -> ggml_context * {
        auto it = ctx_map.find(buft);
        if (it == ctx_map.end()) {
            ggml_init_params params = {
	            (n_tensors + 16) * ggml_tensor_overhead(),
            	nullptr,
            	true
            };
            ggml_context * ctx = ggml_init(params);
            ctx_map[buft] = ctx;
            model.ctxs.push_back(ctx);
            return ctx;
        }
        return it->second;
    };

    auto CT = [&](const std::string & name, ggml_tensor * meta) -> ggml_tensor * {
        ggml_context * ctx = get_ctx(cpu_buft);
        ggml_tensor * t = ggml_dup_tensor(ctx, meta);
        ggml_set_name(t, name.c_str());
        model.tensors[name] = t;
        return t;
    };

    auto create_bn = [&](ggml_context * ctx, const std::string & prefix, int channels) {
        CT(prefix + ".weight",       ggml_new_tensor_1d(ctx, GGML_TYPE_F32, channels));
        CT(prefix + ".bias",         ggml_new_tensor_1d(ctx, GGML_TYPE_F32, channels));
        CT(prefix + ".running_mean", ggml_new_tensor_1d(ctx, GGML_TYPE_F32, channels));
        CT(prefix + ".running_var",  ggml_new_tensor_1d(ctx, GGML_TYPE_F32, channels));
    };

    {
        ggml_init_params meta_params = {
	        (n_tensors + 16) * ggml_tensor_overhead(),
        	nullptr,
        	true
        };
        ggml_context * ctx = ggml_init(meta_params);

        // Stem
        CT("resnet.conv1.weight", ggml_new_tensor_4d(ctx, GGML_TYPE_F32, 3, 3, 1, hp.m_channels));
        create_bn(ctx, "resnet.bn1", hp.m_channels);

        // ResNet layers
        int in_ch = hp.m_channels;
        for (int layer = 1; layer <= 4; layer++) {
            int out_ch = hp.ch(layer);
            int stride_first = (layer == 1) ? 1 : 2;
            std::string lp = "resnet.layer" + std::to_string(layer);
            for (int blk = 0; blk < hp.num_blocks[layer-1]; blk++) {
                std::string bp = lp + "." + std::to_string(blk);
                int blk_in = (blk == 0) ? in_ch : out_ch;
                CT(bp + ".conv1.weight", ggml_new_tensor_4d(ctx, GGML_TYPE_F32, 3, 3, blk_in, out_ch));
                create_bn(ctx, bp + ".bn1", out_ch);
                CT(bp + ".conv2.weight", ggml_new_tensor_4d(ctx, GGML_TYPE_F32, 3, 3, out_ch, out_ch));
                create_bn(ctx, bp + ".bn2", out_ch);
                if (blk == 0 && (stride_first != 1 || in_ch != out_ch)) {
                    CT(bp + ".shortcut.0.weight", ggml_new_tensor_4d(ctx, GGML_TYPE_F32, 1, 1, in_ch, out_ch));
                    create_bn(ctx, bp + ".shortcut.1", out_ch);
                }
            }
            in_ch = out_ch;
        }

        // seg_1
        int stats_dim = (hp.feat_dim / 8) * hp.m_channels * 8;
        int pool_out = stats_dim * 2;
        CT("resnet.seg_1.weight", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, pool_out, hp.embed_dim));
        CT("resnet.seg_1.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.embed_dim));
        if (hp.two_emb_layer) {
            CT("resnet.seg_2.weight", ggml_new_tensor_2d(ctx, GGML_TYPE_F32, hp.embed_dim, hp.embed_dim));
            CT("resnet.seg_2.bias",   ggml_new_tensor_1d(ctx, GGML_TYPE_F32, hp.embed_dim));
        }
        ggml_free(ctx);
    }

    // Allocate
    for (auto & p : ctx_map) {
        ggml_backend_buffer_t buf = ggml_backend_alloc_ctx_tensors_from_buft(p.second, p.first);
        if (buf)
        	model.buffers.push_back(buf);
    }

    // Load weights
    model.n_loaded = 0;
    std::vector<char> read_buf;
    while (true) {
        int32_t n_dims, length, ttype;
        read_safe(loader, n_dims);
    	read_safe(loader, length);
    	read_safe(loader, ttype);
        if (loader->eof(loader->context)) break;

        int32_t n_elements = 1;
    	int32_t ne[4] = {1,1,1,1};

        for (int i = 0; i < n_dims; ++i) {
	        read_safe(loader, ne[i]);
        	n_elements *= ne[i];
        }
        std::string name(length, 0);
    	loader->read(loader->context, name.data(), length);

        auto it = model.tensors.find(name);
        if (it == model.tensors.end())
            Melder_throw (U"Unknown tensor '", Melder_peek8to32_u (name.c_str()), U"'");

        auto tensor = it->second;
    	const size_t bpe = ggml_type_size(ggml_type(ttype));

    	if (ggml_backend_buffer_is_host(tensor->buffer)) {
    		loader->read(loader->context, tensor->data, n_elements * bpe);
    		BYTESWAP_TENSOR(tensor);
    	} else {
    		read_buf.resize(ggml_nbytes(tensor));
    		loader->read(loader->context, read_buf.data(), read_buf.size());
    		// For non-host buffers, we'd need to byteswap in the temp buffer
    		// before copying to device, but as for now we are CPU-only, this path is not taken
    		ggml_backend_tensor_set(tensor, read_buf.data(), 0, ggml_nbytes(tensor));
    	}
    	model.n_loaded++;
    }

    trace (U"Loaded ", model.n_loaded, U"/", model.tensors.size(), U" tensors");
    if (model.n_loaded != model.tensors.size())
        Melder_throw (U"Embedding model incomplete: loaded ", model.n_loaded, U" of ", model.tensors.size(), U" tensors.");
}

// ============================================================================
// Embedding: Fbank extraction (Kaldi-compatible)
// ============================================================================
static float mel_scale(float freq) {
    return 1127.0f * logf(1.0f + freq / 700.0f);
}

static void build_mel_filterbank(int num_mel_bins, int padded_window_size,
                                 float sample_rate, float low_freq, float high_freq,
                                 std::vector<std::vector<float>> & filterbank) {
    if (high_freq <= 0.0f) high_freq = sample_rate / 2.0f;
    int num_fft_bins = padded_window_size / 2;
    float fft_bin_width = sample_rate / padded_window_size;
    float mel_low = mel_scale(low_freq), mel_high = mel_scale(high_freq);
    int num_bins = num_mel_bins + 2;
    std::vector<float> mel_points(num_bins);
    for (int i = 0; i < num_bins; i++)
        mel_points[i] = mel_low + (mel_high - mel_low) * i / (num_bins - 1);
    filterbank.resize(num_mel_bins);
    for (int m = 0; m < num_mel_bins; m++) {
        float left = mel_points[m], center = mel_points[m+1], right = mel_points[m+2];
        filterbank[m].assign(num_fft_bins, 0.0f);
        for (int k = 0; k < num_fft_bins; k++) {
            float fmel = mel_scale(fft_bin_width * k);
            if (fmel > left && fmel <= center)
                filterbank[m][k] = (fmel - left) / (center - left);
            else if (fmel > center && fmel < right)
                filterbank[m][k] = (right - fmel) / (right - center);
        }
    }
}

static void fft_radix2(const float * in, int N, float * out) {
    std::vector<float> buf(2 * N);
    for (int i = 0; i < N; i++) { buf[2*i] = in[i]; buf[2*i+1] = 0.0f; }
    int bits = 0;
    for (int tmp = N; tmp > 1; tmp >>= 1) bits++;
    for (int i = 0; i < N; i++) {
        int j = 0;
        for (int b = 0; b < bits; b++) if (i & (1 << b)) j |= (1 << (bits-1-b));
        if (j > i) { std::swap(buf[2*i], buf[2*j]); std::swap(buf[2*i+1], buf[2*j+1]); }
    }
    for (int s = 1; s <= bits; s++) {
        int m = 1 << s, hm = m / 2;
        float wm_re = cosf(-2.0f * (float)M_PI / m), wm_im = sinf(-2.0f * (float)M_PI / m);
        for (int k = 0; k < N; k += m) {
            float w_re = 1.0f, w_im = 0.0f;
            for (int j = 0; j < hm; j++) {
                int e = k+j, o = k+j+hm;
                float t_re = w_re*buf[2*o] - w_im*buf[2*o+1], t_im = w_re*buf[2*o+1] + w_im*buf[2*o];
                buf[2*o] = buf[2*e] - t_re; buf[2*o+1] = buf[2*e+1] - t_im;
                buf[2*e] += t_re; buf[2*e+1] += t_im;
                float nw = w_re*wm_re - w_im*wm_im; w_im = w_re*wm_im + w_im*wm_re; w_re = nw;
            }
        }
    }
    for (int i = 0; i <= N/2; i++) { out[2*i] = buf[2*i]; out[2*i+1] = buf[2*i+1]; }
}

// Compute fbank features from raw PCM audio
// Input:  samples (float, [-1, 1] normalized), n_samples
// Output: fbank_out (num_frames × 80), row-major
static void compute_fbank(const float * samples, int n_samples,
                          std::vector<float> & fbank_out, int & num_frames_out) {
    const int sample_rate = 16000;
    const int num_mel_bins = 80;
    const int frame_length = 400;   // 25ms
    const int frame_shift  = 160;   // 10ms
    const int padded_window_size = 512;
    const int n_fft_bins = padded_window_size / 2;
    const float preemphasis = 0.97f;
    const float epsilon = 1.1920928955078125e-07f;

    // Scale by 32768 (WeSpeaker convention)
    std::vector<float> scaled(n_samples);
    for (int i = 0; i < n_samples; i++) scaled[i] = samples[i] * 32768.0f;

    // Number of frames (snip_edges=true)
    int num_frames = (n_samples >= frame_length) ? 1 + (n_samples - frame_length) / frame_shift : 0;
    num_frames_out = num_frames;
    if (num_frames == 0) { fbank_out.clear(); return; }

    // Hamming window
    std::vector<float> hamming(frame_length);
    for (int i = 0; i < frame_length; i++)
        hamming[i] = 0.54f - 0.46f * cosf(2.0f * (float)M_PI * i / (frame_length - 1));

    // Mel filterbank
    std::vector<std::vector<float>> mel_filters;
    build_mel_filterbank(num_mel_bins, padded_window_size, sample_rate, 20.0f, 0.0f, mel_filters);

    // Process frames
    fbank_out.resize(num_frames * num_mel_bins);
    std::vector<float> frame(padded_window_size, 0.0f);
    std::vector<float> fft_out(2 * (padded_window_size / 2 + 1));

    for (int f = 0; f < num_frames; f++) {
        int offset = f * frame_shift;
        for (int i = 0; i < frame_length; i++) frame[i] = scaled[offset + i];
        for (int i = frame_length; i < padded_window_size; i++) frame[i] = 0.0f;

        // Remove DC offset
        float dc = 0.0f;
        for (int i = 0; i < frame_length; i++) dc += frame[i];
        dc /= frame_length;
        for (int i = 0; i < frame_length; i++) frame[i] -= dc;

        // Pre-emphasis
        for (int i = frame_length - 1; i > 0; i--) frame[i] -= preemphasis * frame[i-1];
        frame[0] -= preemphasis * frame[0];

        // Hamming window
        for (int i = 0; i < frame_length; i++) frame[i] *= hamming[i];

        // FFT -> power spectrum
        fft_radix2(frame.data(), padded_window_size, fft_out.data());
        std::vector<float> power(n_fft_bins + 1);
        for (int k = 0; k <= n_fft_bins; k++)
            power[k] = fft_out[2*k]*fft_out[2*k] + fft_out[2*k+1]*fft_out[2*k+1];

        // Mel filterbank + log
        for (int m = 0; m < num_mel_bins; m++) {
            double sum = 0.0;
            for (int k = 0; k < n_fft_bins; k++) sum += power[k] * mel_filters[m][k];
            float val = (float)sum;
            if (val < epsilon) val = epsilon;
            fbank_out[f * num_mel_bins + m] = logf(val);
        }
    }

    // Cepstral mean normalization
    for (int m = 0; m < num_mel_bins; m++) {
        double sum = 0.0;
        for (int f = 0; f < num_frames; f++) sum += fbank_out[f * num_mel_bins + m];
        float mean = (float)(sum / num_frames);
        for (int f = 0; f < num_frames; f++) fbank_out[f * num_mel_bins + m] -= mean;
    }
}

// ============================================================================
// Embedding: Conv2d via GGML graph
// ============================================================================
static bool conv2d_forward(ggml_backend_t backend, ggml_tensor * weight,
                           const float * input, int N, int IC, int IH, int IW,
                           int stride, int padding,
                           std::vector<float> & output, int & OH, int & OW, int & OC) {
    OC = (int)weight->ne[3];
    OH = (IH + 2*padding - (int)weight->ne[1]) / stride + 1;
    OW = (IW + 2*padding - (int)weight->ne[0]) / stride + 1;

    ggml_init_params ctx_params = { ggml_tensor_overhead() * CONV2D_MAX_NODES + ggml_graph_overhead(),
    		nullptr, true };
    ggml_context * ctx0 = ggml_init(ctx_params);

    ggml_tensor * inp = ggml_new_tensor_4d(ctx0, GGML_TYPE_F32, IW, IH, IC, N);
    ggml_set_name(inp, "inp"); ggml_set_input(inp);
    ggml_tensor * conv_out = ggml_conv_2d(ctx0, weight, inp, stride, stride, padding, padding, 1, 1);
    ggml_set_name(conv_out, "conv_out"); ggml_set_output(conv_out);

    ggml_cgraph * gf = ggml_new_graph_custom(ctx0, CONV2D_MAX_NODES, false);
    ggml_build_forward_expand(gf, conv_out);
	//GGML_LOG_INFO("conv2d_forward: graph nodes = %d, leafs = %d\n", ggml_graph_n_nodes(gf), gf->n_leafs);
    ggml_gallocr_t galloc = ggml_gallocr_new(ggml_backend_get_default_buffer_type(backend));
    ggml_gallocr_alloc_graph(galloc, gf);
    ggml_backend_tensor_set(inp, input, 0, N * IC * IH * IW * sizeof(float));
    ggml_backend_graph_compute(backend, gf);

    output.resize(N * OC * OH * OW);
    ggml_backend_tensor_get(conv_out, output.data(), 0, output.size() * sizeof(float));
    ggml_gallocr_free(galloc);
    ggml_free(ctx0);
    return true;
}

// ============================================================================
// Embedding: Plain C++ ops: BN, ReLU, residual add
// ============================================================================
static void batchnorm2d(float * data, int N, int C, int H, int W,
                        const float * w, const float * b,
                        const float * mean, const float * var, float eps = 1e-5f) {
    for (int n = 0; n < N; n++)
        for (int c = 0; c < C; c++) {
            float scale = w[c] / sqrtf(var[c] + eps);
            float shift = b[c] - mean[c] * scale;
            float * p = data + ((n*C + c) * H * W);
            for (int i = 0; i < H*W; i++) p[i] = p[i] * scale + shift;
        }
}

static void apply_bn(embedding_model & m, const std::string & pfx,
                     float * data, int N, int C, int H, int W) {
    batchnorm2d(data, N, C, H, W,
        (float*)m.tensors[pfx+".weight"]->data, (float*)m.tensors[pfx+".bias"]->data,
        (float*)m.tensors[pfx+".running_mean"]->data, (float*)m.tensors[pfx+".running_var"]->data);
}

static void relu_inplace(float * d, int n) { for (int i = 0; i < n; i++) if (d[i] < 0.f) d[i] = 0.f; }
static void add_inplace(float * a, const float * b, int n) { for (int i = 0; i < n; i++) a[i] += b[i]; }

// ============================================================================
// Embedding: BasicBlock forward
// ============================================================================
static bool basic_block_forward(ggml_backend_t backend, embedding_model & model,
                                const std::string & prefix, const float * input,
                                int N, int in_C, int H, int W, int stride, bool has_shortcut,
                                std::vector<float> & output, int & oH, int & oW, int & oC) {
    // Main path: conv1 + bn1 + relu
    std::vector<float> t1;
    int h1, w1, c1;
    conv2d_forward(backend, model.tensors[prefix+".conv1.weight"], input, N, in_C, H, W, stride, 1, t1, h1, w1, c1);
    apply_bn(model, prefix+".bn1", t1.data(), N, c1, h1, w1);
    relu_inplace(t1.data(), t1.size());

    // Main path: conv2 + bn2
    std::vector<float> t2;
    conv2d_forward(backend, model.tensors[prefix+".conv2.weight"], t1.data(), N, c1, h1, w1, 1, 1, t2, oH, oW, oC);
    apply_bn(model, prefix+".bn2", t2.data(), N, oC, oH, oW);

    // Residual
    if (has_shortcut) {
        std::vector<float> sc;
        int sH, sW, sC;
        conv2d_forward(backend, model.tensors[prefix+".shortcut.0.weight"], input, N, in_C, H, W, stride, 0, sc, sH, sW, sC);
        apply_bn(model, prefix+".shortcut.1", sc.data(), N, sC, sH, sW);
        add_inplace(t2.data(), sc.data(), t2.size());
    } else {
        add_inplace(t2.data(), input, t2.size());
    }
    relu_inplace(t2.data(), t2.size());
    output = std::move(t2);
    return true;
}

// ============================================================================
// Embedding: Run a full ResNet layer
// ============================================================================
static bool run_layer(ggml_backend_t backend, embedding_model & model, int layer_idx,
                      std::vector<float> & cur, int & N, int & C, int & H, int & W) {
    int out_ch = model.hparams.ch(layer_idx);
    int stride_first = (layer_idx == 1) ? 1 : 2;
    std::string lp = "resnet.layer" + std::to_string(layer_idx);

    for (int blk = 0; blk < model.hparams.num_blocks[layer_idx-1]; blk++) {
        std::string prefix = lp + "." + std::to_string(blk);
        int blk_stride = (blk == 0) ? stride_first : 1;
        int blk_in_ch = (blk == 0) ? C : out_ch;
        bool has_sc = (blk == 0) && (blk_stride != 1 || blk_in_ch != out_ch);

        std::vector<float> out;
        int bH, bW, bC;
        if (!basic_block_forward(backend, model, prefix, cur.data(), N, blk_in_ch, H, W,
                                 blk_stride, has_sc, out, bH, bW, bC)) return false;
        cur = std::move(out);
        C = bC; H = bH; W = bW;
    }
    return true;
}

// ============================================================================
// Embedding: TSTP pooling
// ============================================================================
static void tstp_pooling(const float * data, int N, int C, int H, int W,
                         std::vector<float> & stats) {
    int feat_dim = C * H;
    stats.resize(N * feat_dim * 2);
    for (int n = 0; n < N; n++) {
        float * mean_out = &stats[n * feat_dim * 2];
        float * std_out  = mean_out + feat_dim;
        for (int c = 0; c < C; c++) {
            for (int h = 0; h < H; h++) {
                const float * p = data + ((n*C + c)*H + h)*W;
                double sum = 0.0;
                for (int w = 0; w < W; w++) sum += (double)p[w];
                double m = sum / W;
                mean_out[c*H + h] = (float)m;
                double var_sum = 0.0;
                for (int w = 0; w < W; w++) { double d = (double)p[w] - m; var_sum += d*d; }
                std_out[c*H + h] = (float)sqrt(var_sum / (W - 1) + 1e-7);
            }
        }
    }
}

// ============================================================================
// Embedding: Linear layer
// ============================================================================
static void linear_forward(const float * input, int N, int in_dim,
                           const float * weight, const float * bias, int out_dim,
                           std::vector<float> & output) {
    output.resize(N * out_dim);
    for (int n = 0; n < N; n++)
        for (int o = 0; o < out_dim; o++) {
            float sum = bias ? bias[o] : 0.f;
            for (int i = 0; i < in_dim; i++)
                sum += input[n*in_dim + i] * weight[o*in_dim + i];
            output[n*out_dim + o] = sum;
        }
}

// ============================================================================
// Embedding: Full forward pass: raw PCM -> 256-d embedding
// ============================================================================
static bool embedding_forward(embedding_context & ectx,
                               const float * samples, int n_samples,
                               std::vector<float> & embedding_out) {
    //TRACE
    auto & model = ectx.model;
    auto & hp = model.hparams;

    // 1. Fbank extraction
    std::vector<float> fbank;
    int num_frames;
    compute_fbank(samples, n_samples, fbank, num_frames);
    if (num_frames == 0) {
        trace (U"Audio too short for fbank");
        return false;
    }
    trace (U"fbank: (", num_frames, U", ", hp.feat_dim, U")");

    // 2. Prepare input: transpose (T, F) -> (1, 1, F, T)
    int N = 1, C = 1, H = hp.feat_dim, W = num_frames;
    std::vector<float> cur(N * C * H * W);
    for (int t = 0; t < W; t++)
        for (int f = 0; f < H; f++)
            cur[f * W + t] = fbank[t * H + f];

    // 3. Stem: conv1 + bn1 + relu
    {
        std::vector<float> conv_out;
        int OH, OW, OC;
        conv2d_forward(ectx.backend, model.tensors["resnet.conv1.weight"],
                       cur.data(), N, C, H, W, 1, 1, conv_out, OH, OW, OC);
        apply_bn(model, "resnet.bn1", conv_out.data(), N, OC, OH, OW);
        relu_inplace(conv_out.data(), conv_out.size());
        cur = conv_out; C = OC; H = OH; W = OW;
    }
    trace (U"stem: (", N, U", ", C, U", ", H, U", ", W, U")");

    // 4. ResNet layers 1-4
    for (int layer = 1; layer <= 4; layer++) {
        if (!run_layer(ectx.backend, model, layer, cur, N, C, H, W)) return false;
        trace (U"layer ", layer, U": (", N, U", ", C, U", ", H, U", ", W, U")");
    }

    // 5. TSTP pooling
    std::vector<float> pool;
    tstp_pooling(cur.data(), N, C, H, W, pool);
    trace (U"pool: (", N, U", ", (int)pool.size() / N, U")");

    // 6. seg_1 linear
    linear_forward(pool.data(), N, (int)pool.size() / N,
                   (float*)model.tensors["resnet.seg_1.weight"]->data,
                   (float*)model.tensors["resnet.seg_1.bias"]->data,
                   hp.embed_dim, embedding_out);
    trace (U"embedding: (", N, U", ", hp.embed_dim, U")");

    return true;
}

// ============================================================================
// Embedding: Init / Free
// ============================================================================
static void embedding_init(embedding_context & ectx, diarize_model_loader * loader) {
    //TRACE
    try {
        embedding_load_model(loader, ectx.model);
    } catch (MelderError) {
        Melder_throw (U"Could not load embedding model.");
    }

	ectx.backend = ggml_backend_cpu_init();

	trace (U"Embedding model initialized (", ectx.model.n_loaded, U" tensors)");
}

static void embedding_free(embedding_context & ectx) {
    if (ectx.backend) ggml_backend_free(ectx.backend);
    for (auto buf : ectx.model.buffers) ggml_backend_buffer_free(buf);
    for (auto ctx : ectx.model.ctxs) ggml_free(ctx);
}


/*
 * Agglomerative clustering + reconstruction.
 */
// ============================================================================
// Pipeline parameters (from config.yaml)
// ============================================================================
struct diarization_params {
    // Segmentation
    float   seg_duration       = 10.0f;   // chunk duration (seconds)
    float   seg_step_ratio     = 0.1f;    // step = 10% of duration (90% overlap)
    int     seg_num_frames     = 589;     // frames per chunk (segmentation-3.0)
    int     seg_num_classes    = 7;       // powerset classes
    int     seg_num_speakers   = 3;       // max local speakers per chunk
    int     seg_max_set_size   = 2;       // powerset max set size

    // Embedding
    int     emb_dimension      = 256;     // WeSpeaker ResNet34 output dim
    int     emb_sample_rate    = 16000;
    bool    emb_exclude_overlap = true;

    // Clustering
    double  cluster_threshold  = 0.7045654963945799;
    int     cluster_min_size   = 12;
    float   min_active_ratio   = 0.2f;    // for filter_embeddings

    // Post-processing
    float   min_duration_off   = 0.0f;

    // Derived
    int     seg_step_samples() const { return (int)(seg_duration * seg_step_ratio * emb_sample_rate); }
    int     seg_chunk_samples() const { return (int)(seg_duration * emb_sample_rate); }
    int     seg_step_frames() const { return (int)(seg_num_frames * seg_step_ratio); }
};

// ============================================================================
// Output structures
// ============================================================================
struct diarization_segment {
    float start;     // seconds
    float end;       // seconds
    int   speaker;   // 0-indexed speaker ID
};

struct diarization_result {
    std::vector<diarization_segment> segments;
    int num_speakers;
};

// ============================================================================
// Powerset -> multilabel conversion
//
// segmentation-3.0 uses num_classes=3, max_set_size=2 -> 7 powerset classes:
//   0: {}         -> [0,0,0]
//   1: {0}        -> [1,0,0]
//   2: {1}        -> [0,1,0]
//   3: {2}        -> [0,0,1]
//   4: {0,1}      -> [1,1,0]
//   5: {0,2}      -> [1,0,1]
//   6: {1,2}      -> [0,1,1]
//
// Hard conversion: argmax over 7 classes -> lookup in mapping table.
// ============================================================================

// Mapping table: powerset_mapping[powerset_class][speaker] = 0 or 1
static const int powerset_mapping[7][3] = {
    {0, 0, 0},  // class 0: silence
    {1, 0, 0},  // class 1: speaker 0
    {0, 1, 0},  // class 2: speaker 1
    {0, 0, 1},  // class 3: speaker 2
    {1, 1, 0},  // class 4: speakers 0+1
    {1, 0, 1},  // class 5: speakers 0+2
    {0, 1, 1},  // class 6: speakers 1+2
};

// Convert powerset logits -> binary multilabel
// Input:  logits[num_frames * 7] (log-softmax output from segmentation)
// Output: binary[num_frames * 3]  (0.0 or 1.0 per speaker)
static void powerset_to_multilabel(
    const float * logits, int num_frames, int num_powerset_classes, int num_speakers,
    std::vector<float> & binary
) {
    binary.resize(num_frames * num_speakers);
    for (int f = 0; f < num_frames; f++) {
        // Find argmax over powerset classes
        int best_class = 0;
        float best_val = logits[f * num_powerset_classes];
        for (int c = 1; c < num_powerset_classes; c++) {
            float v = logits[f * num_powerset_classes + c];
            if (v > best_val) {
                best_val = v;
                best_class = c;
            }
        }
        // Map to multilabel
        for (int s = 0; s < num_speakers; s++) {
            binary[f * num_speakers + s] = (float)powerset_mapping[best_class][s];
        }
    }
}

// Convert powerset logits -> soft multilabel probabilities
// Input:  logits[num_chunks * num_frames * 7] (log-softmax from segmentation)
// Output: soft[num_chunks * num_frames * 3]   (soft speaker probabilities)
//
// This is the soft=True path from Powerset.to_multilabel():
//   probs = exp(logits)          — convert log-probs to probs
//   multilabel = probs @ mapping — sum probabilities of sets containing each speaker
//
// Used for the reconstruct step, which needs continuous scores (not binary).
static void powerset_to_soft_multilabel(
    const float * logits, int num_chunks, int num_frames,
    int num_powerset_classes, int num_speakers,
    std::vector<float> & soft
) {
    soft.resize((size_t)num_chunks * num_frames * num_speakers);
    for (int c = 0; c < num_chunks; c++) {
        for (int f = 0; f < num_frames; f++) {
            const float * row = logits + (c * num_frames + f) * num_powerset_classes;
            float * out = soft.data() + (c * num_frames + f) * num_speakers;

            // Initialize output to 0
            for (int s = 0; s < num_speakers; s++) out[s] = 0.0f;

            // probs = exp(logits), then accumulate into speakers via mapping
            for (int p = 0; p < num_powerset_classes; p++) {
                float prob = expf(row[p]);
                for (int s = 0; s < num_speakers; s++) {
                    out[s] += prob * (float)powerset_mapping[p][s];
                }
            }
        }
    }
}

// ============================================================================
// Speaker count from binarized segmentations
//
// For overlapping chunks, count how many speakers are active per global frame.
// Uses simple overlap-add of per-chunk speaker counts.
// ============================================================================
static void compute_speaker_count(
    const float * binarized_segs,   // [num_chunks, num_frames, num_speakers]
    int num_chunks, int num_frames, int num_speakers,
    int chunk_step_frames, int total_frames,
    std::vector<int32_t> & count    // [total_frames]
) {
    // Sum speakers per frame, then aggregate over overlapping chunks
    std::vector<double> sum(total_frames, 0.0);
    std::vector<double> cnt(total_frames, 0.0);

    for (int c = 0; c < num_chunks; c++) {
        int start = c * chunk_step_frames;
        int end = std::min(start + num_frames, total_frames);
        for (int f = 0; f < end - start; f++) {
            float spk_count = 0.0f;
            for (int s = 0; s < num_speakers; s++) {
                spk_count += binarized_segs[(c * num_frames + f) * num_speakers + s];
            }
            sum[start + f] += spk_count;
            cnt[start + f] += 1.0;
        }
    }

    count.resize(total_frames);
    for (int f = 0; f < total_frames; f++) {
        count[f] = (cnt[f] > 0) ? (int32_t)std::round(sum[f] / cnt[f]) : 0;
    }
}

// ============================================================================
// Filter embeddings
//
// Keep only embeddings where:
//   - The speaker has ≥ min_active_ratio non-overlapping (clean) frames
//   - The embedding is not NaN
// ============================================================================
struct filter_result {
    std::vector<float>   filtered_embeddings;  // [num_filtered * dim]
    std::vector<int32_t> chunk_idx;
    std::vector<int32_t> speaker_idx;
    int num_filtered;
};

static filter_result filter_embeddings(
    const float * embeddings,       // [num_chunks * num_speakers * dim]
    const float * segmentations,    // [num_chunks * num_frames * num_speakers]
    int num_chunks, int num_frames, int num_speakers, int dim,
    float min_active_ratio
) {
    filter_result result;
    result.num_filtered = 0;

    for (int c = 0; c < num_chunks; c++) {
        for (int s = 0; s < num_speakers; s++) {
            // Check NaN
            const float * emb = embeddings + (c * num_speakers + s) * dim;
            bool has_nan = false;
            for (int d = 0; d < dim; d++) {
                if (std::isnan(emb[d])) { has_nan = true; break; }
            }
            if (has_nan) continue;

            // Count clean frames (non-overlapping)
            int num_clean = 0;
            for (int f = 0; f < num_frames; f++) {
                const float * frame = segmentations + (c * num_frames + f) * num_speakers;
                if (frame[s] <= 0.0f) continue;
                float total = 0.0f;
                for (int ss = 0; ss < num_speakers; ss++) total += frame[ss];
                if (total == 1.0f) num_clean++;
            }

            if ((float)num_clean < min_active_ratio * num_frames) continue;

            result.chunk_idx.push_back(c);
            result.speaker_idx.push_back(s);
            for (int d = 0; d < dim; d++) {
                result.filtered_embeddings.push_back(emb[d]);
            }
            result.num_filtered++;
        }
    }
    return result;
}

// ============================================================================
// Agglomerative clustering (centroid linkage)
// ============================================================================
static void l2_normalize_f32_to_f64(const float * src, double * dst, int dim) {
    double norm = 0.0;
    for (int j = 0; j < dim; j++) { double v = (double)src[j]; norm += v * v; }
    norm = std::sqrt(norm);
    if (norm > 0.0) {
        for (int j = 0; j < dim; j++) dst[j] = (double)src[j] / norm;
    } else {
        for (int j = 0; j < dim; j++) dst[j] = (double)src[j];
    }
}

static double euclidean_dist(const double * a, const double * b, int dim) {
    double s = 0.0;
    for (int j = 0; j < dim; j++) { double d = a[j] - b[j]; s += d * d; }
    return std::sqrt(s);
}

static double cosine_distance(const double * a, const double * b, int dim) {
    double dot = 0.0, na = 0.0, nb = 0.0;
    for (int j = 0; j < dim; j++) { dot += a[j]*b[j]; na += a[j]*a[j]; nb += b[j]*b[j]; }
    double denom = std::sqrt(na) * std::sqrt(nb);
    return (denom < 1e-30) ? 1.0 : 1.0 - dot / denom;
}

struct linkage_merge { double idx1, idx2, distance, count; };

static void centroid_linkage(const double * emb, int n, int dim,
                              std::vector<linkage_merge> & dendrogram) {
    dendrogram.resize(n - 1);
    int max_cl = 2 * n - 1;
    std::vector<double> centroids(max_cl * dim);
    std::vector<int>    counts(max_cl, 0);
    std::vector<bool>   active(max_cl, false);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < dim; j++) centroids[i * dim + j] = emb[i * dim + j];
        counts[i] = 1; active[i] = true;
    }

    for (int step = 0; step < n - 1; step++) {
        double best_d = 1e30; int bi = -1, bj = -1;
        for (int i = 0; i < n + step; i++) {
            if (!active[i]) continue;
            for (int j = i + 1; j < n + step; j++) {
                if (!active[j]) continue;
                double d = euclidean_dist(centroids.data()+i*dim, centroids.data()+j*dim, dim);
                if (d < best_d) { best_d = d; bi = i; bj = j; }
            }
        }
        int ni = n + step;
        dendrogram[step] = {(double)bi, (double)bj, best_d, (double)(counts[bi]+counts[bj])};
        int nc = counts[bi] + counts[bj];
        for (int j = 0; j < dim; j++)
            centroids[ni*dim+j] = (counts[bi]*centroids[bi*dim+j]+counts[bj]*centroids[bj*dim+j])/nc;
        counts[ni] = nc; active[bi] = false; active[bj] = false; active[ni] = true;
    }
}

static void fcluster_distance(const std::vector<linkage_merge> & dend, int n, double threshold,
                               std::vector<int32_t> & labels) {
    int mx = 2 * n - 1;
    std::vector<int> parent(mx);
    for (int i = 0; i < mx; i++) parent[i] = i;
    auto find = [&](int x) -> int {
        while (parent[x] != x) { parent[x] = parent[parent[x]]; x = parent[x]; } return x;
    };
    for (int i = 0; i < n - 1; i++) {
        int a = (int)dend[i].idx1, b = (int)dend[i].idx2, ni = n + i;
        parent[ni] = ni;
        if (dend[i].distance <= threshold) { parent[find(a)] = ni; parent[find(b)] = ni; }
    }
    labels.resize(n);
    for (int i = 0; i < n; i++) labels[i] = find(i);
    // Renumber 0..K-1
    std::vector<int> uq;
    for (int i = 0; i < n; i++) {
        bool found = false;
        for (int u : uq) { if (u == labels[i]) { found = true; break; } }
        if (!found) uq.push_back(labels[i]);
    }
    std::vector<int> mp(mx, -1);
    for (int k = 0; k < (int)uq.size(); k++) mp[uq[k]] = k;
    for (int i = 0; i < n; i++) labels[i] = mp[labels[i]];
}

static std::vector<int32_t> agglomerative_cluster(
    const float * embeddings_raw, int n, int dim,
    double threshold, int min_cluster_size_param
) {
    int mcs = std::min(min_cluster_size_param, std::max(1, (int)std::round(0.1 * n)));

    if (n == 1) return {0};

    // L2-normalize
    std::vector<double> emb(n * dim);
    for (int i = 0; i < n; i++)
        l2_normalize_f32_to_f64(embeddings_raw + i * dim, emb.data() + i * dim, dim);

    // Centroid linkage
    std::vector<linkage_merge> dend;
    centroid_linkage(emb.data(), n, dim, dend);

    // fcluster
    std::vector<int32_t> clusters;
    fcluster_distance(dend, n, threshold, clusters);

    // Large/small split
    std::map<int32_t, int> cc;
    for (int i = 0; i < n; i++) cc[clusters[i]]++;
    std::vector<int32_t> large, small;
    for (auto & [l, c] : cc) { if (c >= mcs) large.push_back(l); else small.push_back(l); }

    if (large.empty()) {
        return std::vector<int32_t>(n, 0);
    }

    if (!small.empty()) {
        // Reassign small -> nearest large (cosine distance between centroids)
        int nl = (int)large.size(), ns = (int)small.size();
        std::vector<double> lc(nl * dim, 0.0), sc(ns * dim, 0.0);
        std::vector<int> lcnt(nl, 0), scnt(ns, 0);
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < nl; k++) if (clusters[i] == large[k]) {
                for (int j = 0; j < dim; j++) lc[k*dim+j] += emb[i*dim+j];
                lcnt[k]++; break;
            }
            for (int k = 0; k < ns; k++) if (clusters[i] == small[k]) {
                for (int j = 0; j < dim; j++) sc[k*dim+j] += emb[i*dim+j];
                scnt[k]++; break;
            }
        }
        for (int k = 0; k < nl; k++) for (int j = 0; j < dim; j++) lc[k*dim+j] /= lcnt[k];
        for (int k = 0; k < ns; k++) for (int j = 0; j < dim; j++) sc[k*dim+j] /= scnt[k];

        for (int sk = 0; sk < ns; sk++) {
            double bd = 1e30; int blk = 0;
            for (int lk = 0; lk < nl; lk++) {
                double d = cosine_distance(lc.data()+lk*dim, sc.data()+sk*dim, dim);
                if (d < bd) { bd = d; blk = lk; }
            }
            int ol = small[sk], nl_ = large[blk];
            for (int i = 0; i < n; i++) if (clusters[i] == ol) clusters[i] = nl_;
        }
    }

    // Renumber
    std::vector<int32_t> us;
    { auto t = clusters; std::sort(t.begin(),t.end()); t.erase(std::unique(t.begin(),t.end()),t.end()); us = t; }
    std::map<int32_t,int32_t> rm;
    for (int k = 0; k < (int)us.size(); k++) rm[us[k]] = k;
    for (int i = 0; i < n; i++) clusters[i] = rm[clusters[i]];
    return clusters;
}

// ============================================================================
// Assign embeddings — compute centroids, then assign ALL embeddings
// ============================================================================
struct assign_result {
    std::vector<int32_t> hard_clusters;  // [num_chunks * num_speakers]
    std::vector<double>  centroids;       // [num_clusters * dim]
    int num_clusters;
};

static assign_result assign_embeddings(
    const float * embeddings,       // [num_chunks * num_speakers * dim]
    int num_chunks, int num_speakers, int dim,
    const int32_t * chunk_idx, const int32_t * speaker_idx,
    const int32_t * train_clusters, int num_train
) {
    int num_clusters = 0;
    for (int i = 0; i < num_train; i++)
        num_clusters = std::max(num_clusters, train_clusters[i] + 1);

    assign_result r;
    r.num_clusters = num_clusters;
    r.centroids.resize(num_clusters * dim, 0.0);
    std::vector<int> cc(num_clusters, 0);

    for (int i = 0; i < num_train; i++) {
        int c = chunk_idx[i], s = speaker_idx[i], k = train_clusters[i];
        const float * e = embeddings + (c * num_speakers + s) * dim;
        for (int d = 0; d < dim; d++) r.centroids[k*dim+d] += (double)e[d];
        cc[k]++;
    }
    for (int k = 0; k < num_clusters; k++)
        if (cc[k] > 0) for (int d = 0; d < dim; d++) r.centroids[k*dim+d] /= cc[k];

    int total = num_chunks * num_speakers;
    r.hard_clusters.resize(total);
    for (int cs = 0; cs < total; cs++) {
        const float * e = embeddings + cs * dim;
        std::vector<double> ef(dim);
        for (int d = 0; d < dim; d++) ef[d] = (double)e[d];
        int bk = 0; double bs = -1e30;
        for (int k = 0; k < num_clusters; k++) {
            double dist = cosine_distance(ef.data(), r.centroids.data()+k*dim, dim);
            double score = 2.0 - dist;
            if (score > bs) { bs = score; bk = k; }
        }
        r.hard_clusters[cs] = bk;
    }
    return r;
}

// ============================================================================
// Reconstruct — remap local speakers -> global clusters
// ============================================================================
struct reconstruct_result {
    std::vector<float> clustered;   // [num_chunks * num_frames * num_global]
    std::vector<int8_t> nan_mask;   // 1=NaN, 0=valid
    int num_global_speakers;
};

static reconstruct_result reconstruct(
    const float * segmentations,   // [num_chunks * num_frames * local_num_speakers]
    const int32_t * hard_clusters, // [num_chunks * local_num_speakers]
    int num_chunks, int num_frames, int local_num_speakers
) {
    int ng = 0;
    for (int i = 0; i < num_chunks * local_num_speakers; i++)
        if (hard_clusters[i] >= 0) ng = std::max(ng, hard_clusters[i] + 1);

    reconstruct_result r;
    r.num_global_speakers = ng;
    size_t total = (size_t)num_chunks * num_frames * ng;
    r.clustered.resize(total, -1.0f);
    r.nan_mask.resize(total, 1);

    for (int c = 0; c < num_chunks; c++) {
        for (int k = 0; k < ng; k++) {
            for (int f = 0; f < num_frames; f++) {
                float mx = -1e30f; bool found = false;
                for (int s = 0; s < local_num_speakers; s++) {
                    if (hard_clusters[c * local_num_speakers + s] == k) {
                        float v = segmentations[(c * num_frames + f) * local_num_speakers + s];
                        if (!found || v > mx) { mx = v; found = true; }
                    }
                }
                size_t idx = (c * num_frames + f) * ng + k;
                if (found) { r.clustered[idx] = mx; r.nan_mask[idx] = 0; }
            }
        }
    }
    return r;
}

// ============================================================================
// Aggregate overlapping windows
// ============================================================================
static std::vector<float> aggregate_overlapping(
    const float * clustered, const int8_t * nan_mask,
    int num_chunks, int nf, int ns, int step_frames, int total_frames
) {
    std::vector<double> sum(total_frames * ns, 0.0);
    std::vector<double> cnt(total_frames * ns, 0.0);
    for (int c = 0; c < num_chunks; c++) {
        int start = c * step_frames, end = std::min(start + nf, total_frames);
        for (int f = 0; f < end - start; f++)
            for (int s = 0; s < ns; s++) {
                size_t si = (c*nf+f)*ns+s, di = (start+f)*ns+s;
                if (nan_mask[si] == 0) { sum[di] += (double)clustered[si]; cnt[di] += 1.0; }
            }
    }
    std::vector<float> r(total_frames * ns);
    for (int i = 0; i < total_frames * ns; i++)
        r[i] = (cnt[i] > 0) ? (float)(sum[i] / cnt[i]) : 0.0f;
    return r;
}

// ============================================================================
// to_diarization — top-N speaker selection per frame
// ============================================================================
static std::vector<float> to_diarization(
    const float * act, const int32_t * count, int total_frames, int ns
) {
    std::vector<float> bin(total_frames * ns, 0.0f);
    std::vector<int> idx(ns);
    for (int t = 0; t < total_frames; t++) {
        int c = count[t]; if (c <= 0) continue;
        for (int s = 0; s < ns; s++) idx[s] = s;
        std::sort(idx.begin(), idx.end(), [&](int a, int b) {
            return act[t*ns+a] > act[t*ns+b];
        });
        for (int i = 0; i < std::min(c, ns); i++) bin[t*ns+idx[i]] = 1.0f;
    }
    return bin;
}

// ============================================================================
// to_annotation — binary frames -> time-stamped segments
// ============================================================================
static std::vector<diarization_segment> to_annotation(
    const float * binary, int total_frames, int ns, float frame_step
) {
    std::vector<diarization_segment> segs;
    for (int s = 0; s < ns; s++) {
        bool in_seg = false; int sf = 0;
        for (int t = 0; t < total_frames; t++) {
            bool a = binary[t*ns+s] >= 0.5f;
            if (a && !in_seg) { sf = t; in_seg = true; }
            else if (!a && in_seg) {
                segs.push_back({sf * frame_step, t * frame_step, s});
                in_seg = false;
            }
        }
        if (in_seg) segs.push_back({sf * frame_step, total_frames * frame_step, s});
    }
    std::sort(segs.begin(), segs.end(), [](auto & a, auto & b) {
        if (std::abs(a.start-b.start) > 1e-6f) return a.start < b.start;
        return a.speaker < b.speaker;
    });
    return segs;
}

// ============================================================================
// Full diarization pipeline (clustering + reconstruction)
//
// This is the function you call after running segmentation and embedding
// models on all chunks. It takes:
//   - binarized segmentations from powerset conversion
//   - raw segmentation scores (for reconstruct)
//   - embeddings from the WeSpeaker model
// And returns time-stamped speaker segments.
// ============================================================================
static diarization_result run_diarization_pipeline(
    const float * powerset_logits,          // [num_chunks * num_frames * num_powerset_classes] log-softmax
    const float * binarized_segmentations,  // [num_chunks * num_frames * num_speakers] binary
    const float * embeddings,               // [num_chunks * num_speakers * emb_dim]
    int num_chunks,
    int num_frames,
    int num_powerset_classes,               // powerset classes (7)
    int num_speakers,                       // local speakers per chunk (3)
    int emb_dim,
    const diarization_params & params,
    int max_speakers = 20                   // upper bound on global speakers
) {
    //TRACE
    int step_frames = params.seg_step_frames();
    int total_frames = (num_chunks - 1) * step_frames + num_frames;
    float frame_step = params.seg_duration / num_frames;

    trace (U"diarize: ", num_chunks, U" chunks, ", num_frames, U" frames/chunk, step=",
    		step_frames, U", total=", total_frames, U" frames");

    // --- Convert powerset logits -> soft multilabel for reconstruct ---
    // pyannote's Inference (with skip_conversion=False) returns soft multilabel
    // probabilities (3-class), which are then used in the reconstruct step.
    // Our C++ segmentation model outputs raw 7-class log-softmax, so we need
    // to convert: probs = exp(logits), then multilabel = probs @ mapping.
    std::vector<float> soft_segmentations;
    powerset_to_soft_multilabel(powerset_logits, num_chunks, num_frames,
                                num_powerset_classes, num_speakers,
                                soft_segmentations);

    // --- Speaker count ---
    std::vector<int32_t> spk_count;
    compute_speaker_count(binarized_segmentations, num_chunks, num_frames,
                          num_speakers, step_frames, total_frames, spk_count);

    // Cap at max_speakers
    for (auto & c : spk_count) c = std::min(c, (int32_t)max_speakers);

    // Check if any speaker is ever active
    int max_count = *std::max_element(spk_count.begin(), spk_count.end());
    if (max_count == 0) {
        trace (U"diarize: no speakers detected");
        return {{}, 0};
    }

    // --- Filter embeddings ---
    auto filt = filter_embeddings(embeddings, binarized_segmentations,
                                   num_chunks, num_frames, num_speakers, emb_dim,
                                   params.min_active_ratio);
    trace (U"diarize: ", filt.num_filtered, U"/", num_chunks * num_speakers, U" embeddings pass filter");

    if (filt.num_filtered < 2) {
        trace (U"diarize: too few embeddings, single speaker");
        // Single speaker fallback — all frames assigned to speaker 0
        diarization_result result;
        result.num_speakers = 1;
        result.segments.push_back({0.0f, total_frames * frame_step, 0});
        return result;
    }

    // --- Cluster ---
    auto train_clusters = agglomerative_cluster(
        filt.filtered_embeddings.data(), filt.num_filtered, emb_dim,
        params.cluster_threshold, params.cluster_min_size);

    int num_clusters = *std::max_element(train_clusters.begin(), train_clusters.end()) + 1;
    trace (U"diarize: ", num_clusters, U" clusters");

    // --- Assign all embeddings ---
    auto asgn = assign_embeddings(embeddings, num_chunks, num_speakers, emb_dim,
                                   filt.chunk_idx.data(), filt.speaker_idx.data(),
                                   train_clusters.data(), filt.num_filtered);

    // --- Mark inactive speakers as -2 ---
    std::vector<int32_t> hard = asgn.hard_clusters;
    for (int c = 0; c < num_chunks; c++) {
        for (int s = 0; s < num_speakers; s++) {
            // Check if this speaker has any activity in this chunk
            float total_activity = 0.0f;
            for (int f = 0; f < num_frames; f++) {
                total_activity += binarized_segmentations[(c*num_frames+f)*num_speakers+s];
            }
            if (total_activity == 0.0f) {
                hard[c * num_speakers + s] = -2;
            }
        }
    }

    // --- Reconstruct ---
    // Use soft multilabel scores (not binarized, not raw powerset logits).
    // This matches pyannote which passes the Inference output (already converted
    // from powerset to soft multilabel by the Powerset module in Inference.infer()).
    auto recon = reconstruct(soft_segmentations.data(), hard.data(),
                              num_chunks, num_frames, num_speakers);
    trace (U"diarize: ", recon.num_global_speakers, U" global speakers in reconstruction");

    // --- Aggregate ---
    auto aggregated = aggregate_overlapping(recon.clustered.data(), recon.nan_mask.data(),
                                            num_chunks, num_frames, recon.num_global_speakers,
                                            step_frames, total_frames);

    // --- to_diarization ---
    auto binary = to_diarization(aggregated.data(), spk_count.data(),
                                  total_frames, recon.num_global_speakers);

    // --- to_annotation ---
    auto segs = to_annotation(binary.data(), total_frames,
                               recon.num_global_speakers, frame_step);

    diarization_result result;
    result.segments = segs;
    result.num_speakers = recon.num_global_speakers;

    trace (U"diarize: ", segs.size(), U" segments, ", result.num_speakers, U" speakers");

    return result;
}


/*
 * Speaker diarization API.
 */
// ============================================================================
// Context structure — holds models and last result
// ============================================================================
struct diarize_context {
    segmentation_context seg_ctx;
    embedding_context    emb_ctx;
    bool                 models_loaded;

    // Last result
    diarization_result   result;
};

// ============================================================================
// Memory stream helper (for init_from_memory)
// ============================================================================
struct diarize_mem_stream {
	const uint8_t * data;
	size_t size;
	size_t pos;
};

// ============================================================================
// C API implementation
// ============================================================================
extern "C" {

struct diarize_params diarize_default_params(void) {
    struct diarize_params p{};
    p.seg_duration      = 10.0f;
    p.seg_step_ratio    = 0.1f;
    p.cluster_threshold = 0.7045654963945799f;
    p.cluster_min_size  = 12;
    p.min_active_ratio  = 0.2f;
    p.max_speakers      = 20;
    return p;
}

struct diarize_context * diarize_init(struct diarize_model_loader * seg_loader, struct diarize_model_loader * emb_loader) {
    //TRACE
	auto * ctx = new diarize_context();
	ctx->models_loaded = false;

    try {
        segmentation_init(ctx->seg_ctx, seg_loader);
    } catch (MelderError) {
        delete ctx;
        Melder_throw (U"Failed to initialize diarization: segmentation model could not be loaded.");
    }

    try {
        embedding_init(ctx->emb_ctx, emb_loader);
    } catch (MelderError) {
		segmentation_free(ctx->seg_ctx);
        delete ctx;
        Melder_throw (U"Failed to initialize diarization: embedding model could not be loaded.");
    }

	ctx->models_loaded = true;
	trace (U"diarize_init: models loaded");
	return ctx;
}

static diarize_model_loader create_file_loader(std::ifstream * fin) {
	diarize_model_loader loader = {};
	loader.context = fin;

	loader.read = [](void * ctx, void * output, size_t read_size) -> size_t {
		auto * f = (std::ifstream *)ctx;
		f->read((char *)output, read_size);
		return read_size;
	};

	loader.eof = [](void * ctx) -> bool {
		auto * f = (std::ifstream *)ctx;
		return f->eof();
	};

	loader.close = [](void * ctx) {
		auto * f = (std::ifstream *)ctx;
		f->close();
		delete f;
	};
	return loader;
}

struct diarize_context * diarize_init_from_file(const char * seg_model_path, const char * emb_model_path) {
    //TRACE
	auto * seg_fin = new std::ifstream(seg_model_path, std::ios::binary);
	if (!seg_fin->is_open()) {
		delete seg_fin;
	    Melder_throw (U"Failed to open '", Melder_peek8to32_u (seg_model_path), U"'.");
	}

	auto * emb_fin = new std::ifstream(emb_model_path, std::ios::binary);
	if (!emb_fin->is_open()) {
	    delete seg_fin;
	    delete emb_fin;
	    Melder_throw (U"Failed to open '", Melder_peek8to32_u (emb_model_path), U"'.");
	}

    auto seg_loader = create_file_loader(seg_fin);
    auto emb_loader = create_file_loader(emb_fin);

    try {
        auto * ctx = diarize_init(&seg_loader, &emb_loader);
        seg_loader.close(seg_loader.context);
        emb_loader.close(emb_loader.context);
        return ctx;
    } catch (MelderError) {
        seg_loader.close(seg_loader.context);
        emb_loader.close(emb_loader.context);
        Melder_throw (U"Failed to initialize diarization from file.");
    }
}

static diarize_model_loader make_memory_loader(const void * data, size_t size) {
	auto * stream = new diarize_mem_stream{(const uint8_t *)data, size, 0};
	diarize_model_loader loader = {};

	loader.context = stream;
	loader.read = [](void * ctx, void * output, size_t read_size) -> size_t {
		auto * s = (diarize_mem_stream *)ctx;
		size_t available = s->size - s->pos;
		size_t to_read = (read_size < available) ? read_size : available;
		memcpy(output, s->data + s->pos, to_read);
		s->pos += to_read;
		return to_read;
	};

	loader.eof = [](void * ctx) -> bool {
		auto * s = (diarize_mem_stream *)ctx;
		return s->pos >= s->size;
	};

	loader.close = [](void * ctx) {
		delete (diarize_mem_stream *)ctx;
	};
	return loader;
}

struct diarize_context * diarize_init_from_memory(const void * seg_data, size_t seg_size, const void * emb_data, size_t emb_size) {
	auto seg_loader = make_memory_loader(seg_data, seg_size);
	auto emb_loader = make_memory_loader(emb_data, emb_size);

    try {
        auto * ctx = diarize_init(&seg_loader, &emb_loader);
        seg_loader.close(seg_loader.context);
        emb_loader.close(emb_loader.context);
        return ctx;
    } catch (MelderError) {
        seg_loader.close(seg_loader.context);
        emb_loader.close(emb_loader.context);
        Melder_throw (U"Failed to initialize diarization from memory.");
    }
}

void diarize_free(struct diarize_context * ctx) {
    if (!ctx) return;
    if (ctx->models_loaded) {
        segmentation_free(ctx->seg_ctx);
        embedding_free(ctx->emb_ctx);
    }
    delete ctx;
}

void diarize_full(
    struct diarize_context * ctx,
    struct diarize_params    params,
    const float            * samples,
    int                      n_samples)
{
    //TRACE
    if (!ctx || !ctx->models_loaded || !samples || n_samples <= 0)
        Melder_throw (U"Invalid arguments in diarize_full.");

    // Clear previous result
    ctx->result = {};

    // --- Pipeline parameters ---
    diarization_params dp;
    dp.seg_duration      = params.seg_duration;
    dp.seg_step_ratio    = params.seg_step_ratio;
    dp.cluster_threshold = (double)params.cluster_threshold;
    dp.cluster_min_size  = params.cluster_min_size;
    dp.min_active_ratio  = params.min_active_ratio;

    const int sample_rate   = 16000;
    const int chunk_samples = (int)(dp.seg_duration * sample_rate);
    const int step_samples  = (int)(dp.seg_duration * dp.seg_step_ratio * sample_rate);

    float duration = (float)n_samples / sample_rate;
    trace (U"diarize_full: ", duration, U"s audio (", n_samples, U" samples)");

    // --- Step 1: Sliding window segmentation ---
    int num_chunks = 0;
    for (int start = 0; ; start += step_samples) {
        if (start + chunk_samples > n_samples && start > 0) break;
        num_chunks++;
    }

    int num_frames = 0, num_classes = 0;
    std::vector<float> all_segmentations;

    for (int c = 0; c < num_chunks; c++) {
        int start = c * step_samples;
        int end = std::min(start + chunk_samples, n_samples);
        int len = end - start;

        std::vector<float> chunk(chunk_samples, 0.0f);
        std::memcpy(chunk.data(), samples + start, len * sizeof(float));

        std::vector<float> seg_out;
        int nf, nc;
        if (!segmentation_forward(ctx->seg_ctx, chunk.data(), chunk_samples, seg_out, nf, nc))
            Melder_throw (U"Segmentation failed on chunk ", c, U".");

        if (c == 0) {
            num_frames = nf;
            num_classes = nc;
            dp.seg_num_frames = nf;
            dp.seg_num_classes = nc;
            all_segmentations.reserve(num_chunks * nf * nc);
        }

        all_segmentations.insert(all_segmentations.end(), seg_out.begin(), seg_out.end());
    }

    int num_speakers = dp.seg_num_speakers;  // 3

    trace (U"diarize_full: ", num_chunks, U" chunks, ", num_frames, U" frames, ", num_classes, U" powerset classes");

    // --- Step 2: Powerset -> multilabel (hard binarization) ---
    std::vector<float> all_binarized(num_chunks * num_frames * num_speakers);

    for (int c = 0; c < num_chunks; c++) {
        std::vector<float> chunk_binary;
        powerset_to_multilabel(
            all_segmentations.data() + c * num_frames * num_classes,
            num_frames, num_classes, num_speakers, chunk_binary);
        std::memcpy(all_binarized.data() + c * num_frames * num_speakers,
                     chunk_binary.data(), num_frames * num_speakers * sizeof(float));
    }

    // --- Step 3: Embedding extraction ---
    int emb_dim = dp.emb_dimension;
    std::vector<float> all_embeddings(num_chunks * num_speakers * emb_dim);
    std::fill(all_embeddings.begin(), all_embeddings.end(), std::nanf(""));

    for (int c = 0; c < num_chunks; c++) {
        int start_sample = c * step_samples;
        int end_sample = std::min(start_sample + chunk_samples, n_samples);
        int len = end_sample - start_sample;

        std::vector<float> chunk_wav(chunk_samples, 0.0f);
        std::memcpy(chunk_wav.data(), samples + start_sample, len * sizeof(float));

        for (int s = 0; s < num_speakers; s++) {
            const float * mask = all_binarized.data() +
                                  (c * num_frames + 0) * num_speakers + s;

            // Check if speaker has any activity
            float total_activity = 0.0f;
            for (int f = 0; f < num_frames; f++) {
                total_activity += mask[f * num_speakers];
            }
            if (total_activity < 1.0f) continue;

            // Interpolate frame-level mask to sample-level and compact
            std::vector<float> masked_wav(chunk_samples);
            int active_samples = 0;
            for (int t = 0; t < chunk_samples; t++) {
                int frame = (int)((float)t / chunk_samples * num_frames);
                frame = std::min(frame, num_frames - 1);
                float m = mask[frame * num_speakers];
                if (m > 0.5f) {
                    masked_wav[active_samples++] = chunk_wav[t];
                }
            }

            if (active_samples < 1000) continue;

            std::vector<float> emb_out;
            if (!embedding_forward(ctx->emb_ctx, masked_wav.data(), active_samples, emb_out)) {
                continue;
            }

            std::memcpy(all_embeddings.data() + (c * num_speakers + s) * emb_dim,
                         emb_out.data(), emb_dim * sizeof(float));
        }

        if ((c + 1) % 10 == 0 || c == num_chunks - 1) {
            trace (U"diarize_full: embeddings ", c + 1, U"/", num_chunks);
        }
    }

    // --- Step 4: Clustering + reconstruction pipeline ---
    ctx->result = run_diarization_pipeline(
        all_segmentations.data(),
        all_binarized.data(),
        all_embeddings.data(),
        num_chunks, num_frames, num_classes, num_speakers, emb_dim,
        dp, params.max_speakers);

    trace (U"diarize_full: ", ctx->result.num_speakers, U" speakers, ", ctx->result.segments.size(), U" segments");
}

unsigned int diarize_full_n_segments(struct diarize_context * ctx) {
    if (!ctx) return 0;
    return ctx->result.segments.size();
}

int diarize_full_n_speakers(struct diarize_context * ctx) {
    if (!ctx) return 0;
    return ctx->result.num_speakers;
}

float diarize_full_get_segment_t0(struct diarize_context * ctx, int i_segment) {
    if (!ctx || i_segment < 0 || i_segment >= (int)ctx->result.segments.size()) return 0.0f;
    return ctx->result.segments[i_segment].start;
}

float diarize_full_get_segment_t1(struct diarize_context * ctx, int i_segment) {
    if (!ctx || i_segment < 0 || i_segment >= (int)ctx->result.segments.size()) return 0.0f;
    return ctx->result.segments[i_segment].end;
}

int diarize_full_get_segment_speaker(struct diarize_context * ctx, int i_segment) {
    if (!ctx || i_segment < 0 || i_segment >= (int)ctx->result.segments.size()) return -1;
    return ctx->result.segments[i_segment].speaker;
}

} // extern "C"
