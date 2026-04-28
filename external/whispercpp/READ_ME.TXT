praat/external/whispercpp/READ_ME.TXT
Anastasia Shchupak, 13 March 2026

This file describes the adaptations to the Whisper.cpp current sources
that are needed to make them compatible with Praat.
Last maintenance release of Whisper.cpp as of 12 March 2026 was v1.8.3 (Jan 15, 2025).
The source code in this edition was taken from commit 30c5194c9691e4e9a98b3dea9f19727397d3f46e.


1. Selecting files and flattening the file structure
----------------------------------------------------
The Whisper.cpp sources are distributed over multiple folders, which are in different deep branches
of the whisper.cpp root folder. We use only a subset of files and flatten this hierarchy as shown below.

whisper.cpp
    include
        whisper.h
    src
        whisper-arch.h
        whisper.cpp
    ggml
        include
            ggml-alloc.h
            ggml-backend.h
            ggml-cpp.h
            ggml-cpu.h
            ggml.h
            gguf.h
        src
            ggml-alloc.c -> ggml-alloc.cpp
            ggml-backend-dl.cpp
            ggml-backend-dl.h
            ggml-backend-impl.h
            ggml-backend-reg.cpp
            ggml-backend.cpp
            ggml-common.h
            ggml-quants.cpp -> ggml-quants.cpp
            ggml.c -> ggml.cpp
        ggml-cpu
            amx
                amx.h
            arch-fallback.h
            binary-ops.cpp
            binary-ops.h
            common.h
            ggml-cpu.cpp -> ggml-cpu.cpp
            ggml-cpu.cpp -> ggml-cpu-cpp.cpp
            ggml-cpu-impl.h
            ops.cpp
            ops.h
            quants.c
            quants.h
            repack.h
            simd-gemm.h
            simd-mappings.h
            traits.cpp
            traits.h
            unary-ops.cpp
            unary-ops.h
            vec.cpp
            vec.h
            ggml-impl.h
            ggml-quants.h
            ggml-threading.cpp
            ggml-threading.h

All these files are put into the single external/whispercpp source folder.

2. New Files
------------
2.1. New file: ggml-type-traits.c
---------------------------------
When selecting files, we renamed the following files to enable C++ exceptions for graceful cleanup after GGML aborts and failing assertions:
ggml.c -> ggml.cpp
ggml-cpu.c -> ggml-cpu.cpp

The following step is necessary to be able to compile ggml.cpp and ggml-cpu.cpp as C++,
because C++ doesn't support non-trivial designated initializers.

We extracted the following array from ggml.cpp to ggml-type-traits.c.
```
    const struct ggml_type_traits type_traits[GGML_TYPE_COUNT]
```
We also extracted the following array from ggml-cpu.cpp to ggml-type-traits.c.
```
    const struct ggml_type_traits_cpu type_traits_cpu[GGML_TYPE_COUNT]
```

2.2. New files: ggml-memory-pool.cpp and ggml-memory-pool.h
----------------------------------------------------------
To implement GGML cleanup in case it needs to abort, we added a memory pool, which is going to track all
memory allocations by GGML. If a new allocation fails, or there is any other reason for GGML to stop,
instead of abort (which causes Praat to crash), we will free all the memory registered in the pool.
This will allow Praat to continue running after a graceful end of whatever was using GGML: transcription or diarizarion.


3. File modifications
---------------------
3.1. ggml.h
-----------
To make GGML and Whisper-cpp compatible with Praat, we add the following to the top of `ggml.h`:
```
#define WHISPER_VERSION  "1.8.3"
#define GGML_VERSION  "0.9.7"
#define GGML_COMMIT  "unknown"
#define GGML_USE_CPU
#define GGML_CPU_GENERIC
#if ! defined (_GNU_SOURCE)
	#define _GNU_SOURCE
#endif
```
This works because `ggml.h` is included, directly or indirectly,
at the very top of all `ggml` and `whisper` files (last checked 20260312), except:
ggml-common.h, ggml-backend-dl.cpp, ggml-backend-dl.h, unary-ops.cpp, and unary-ops.h.
Also, we add GGML_API memory wrappers declaration in `ggml.h` after declaring `ggml_abort()``:
```
    GGML_API void * ggml_malloc(size_t size);
    GGML_API void * ggml_calloc(size_t num, size_t size);
    GGML_API void * ggml_realloc(void * ptr, size_t size);
#ifdef __cplusplus
    GGML_API void ggml_raw_free(void * ptr, bool toRemoveFromPool = true);
#else
    GGML_API void ggml_raw_free(void * ptr, bool toRemoveFromPool);
#endif
```

3.2. ggml.cpp
-------------
3.2.1. To make use of Melder_throw and TRACE/trace as well as GGML memory pool, we add the following to the top of `ggml.cpp`:
```
#include "melder.h"
#include "ggml-memory-pool.h"
```

3.2.2. To intercept GGML abortions and throw Melder exception instead, we change the following line in ggml_abort():
```
//abort();   // this is the old line which needs to be removed
Melder_throw (Melder_peek8to32_u (message));   // this is the new line
```

3.2.3. We also change the functions ggml_aligned_malloc(), ggml_aligned_free(), ggml_malloc(), ggml_calloc(),
so that all of them except ggml_calloc() do not return NULL, instead calling GGML_ABORT().
Also, to register the allocations in our GGML memory pool, we add the following to the ggml_aligned_malloc():
```
    theGgmlMemoryPool.add (aligned_memory, size, true);   // third argument bool aligned = true
```

and the following to both ggml_malloc() and ggml_calloc():
```
    theGgmlMemoryPool.add (result, size, false);   // third argument bool aligned = false
```

We add one parameter to ggml_aligned_free() and extend this function as follows:
```
void ggml_aligned_free(void * ptr, size_t size, bool toRemoveFromPool) {
	bool removedFromPool = false;
	if (toRemoveFromPool)
		removedFromPool = theGgmlMemoryPool.remove (ptr, size);
	if (! toRemoveFromPool || removedFromPool) {
	    ...   // <------------------------- HERE GOES THE OLD CONTENT OF THIS FUNCTION
	}
```

We define these two functions in ggml.cpp:
```
void * ggml_realloc(void * ptr, size_t size) {
    if (size == 0)
        GGML_ABORT("Behavior may be unexpected when allocating 0 bytes for ggml_malloc!\n");

    void * result = realloc(ptr, size);
    if (! result)
        GGML_ABORT("%s: failed to reallocate %6.2f MB\n", __func__, size/(1024.0*1024.0));

    theGgmlMemoryPool.remove (ptr);
    theGgmlMemoryPool.add (result, size, false);
    return result;
}

void ggml_raw_free(void *ptr, bool toRemoveFromPool) {
	bool removedFromPool = false;
	if (toRemoveFromPool)
		removedFromPool = theGgmlMemoryPool.remove (ptr);
	if (! toRemoveFromPool || removedFromPool)
		free (ptr);
}
```

We remove `inline static` function specifiers from ggml_malloc() and ggml_calloc()
and move following three macro definitions to the top, after ggml_abort,
and we change `GGML_FREE` macro from `free(ptr)` to `ggml_raw_free(ptr)`:
```
#define GGML_MALLOC(size)      ggml_malloc(size)
#define GGML_CALLOC(num, size) ggml_calloc(num, size)
#define GGML_FREE(ptr)         ggml_raw_free(ptr)   // <------- CHANGE THIS
```

3.3. ggml-impl.h
----------------
We change the declaration of ggml_aligned_free(), adding a default parameter bool toRemoveFromPool = true
(also added support for C as a dummy, as this function is currently never called from C files)
```
#ifdef __cplusplus
GGML_API void ggml_aligned_free(void * ptr, size_t size, bool toRemoveFromPool = true);
#else
GGML_API void ggml_aligned_free(void * ptr, size_t size, bool toRemoveFromPool);
#endif
```

3.4. Find all the occurrences of raw `malloc`/`calloc`/`realloc`/`free` in the following files:
    ggml.cpp,
    ggml-alloc.cpp,
    ggml-backend.cpp
    ggml-quants.cpp
    whisper.cpp
and replace them with `ggml_malloc`/`ggml_calloc`/`ggml_realloc`/`ggml_raw_free`(be careful, NOT `ggml_free`, but `ggml_raw_free`).

3.5. whisper.h and whisper.cpp
------------------------------
3.5.1.
------
To use silero model for speech detection from memory rather than from the external binary file,
we add the following two variables to the `struct whisper_full_params` in whisper.h (in the section for VAD):
```
        const void * vad_model_data;		      // Pointer to in-memory model data
        size_t       vad_model_data_size;         // Size of in-memory model data
```
And we initialize them in whisper.cpp in whisper_full_default_params() (also in the section for VAD):
```
		/*.vad_model_data		       =*/ nullptr,
		/*.vad_model_data_size		   =*/ 0,
```

3.5.2.
------
Then we declare a function which is responsible for loading the silero model from internal memory.
We place it among other WHISPER_API in the section "Voice Activity Detection (VAD)"
right after function whisper_vad_init_from_file_with_params() in whisper.h:

	WHISPER_API struct whisper_vad_context * whisper_vad_init_from_memory_with_params(const void * data, size_t size, struct whisper_vad_context_params params);

And we define it in whisper.cpp:
```
struct whisper_vad_context * whisper_vad_init_from_memory_with_params (
		const void * data, size_t size,
		whisper_vad_context_params params) {
	WHISPER_LOG_INFO("%s: loading VAD model from memory\n", __func__);
	struct SileroVadStream {
		const void * data;
		size_t size;
		size_t pos;
	};
	SileroVadStream stream {
		data,
		size,
		0
	};
	whisper_model_loader loader {};
	loader.context = &stream;

	loader.read = [](void * ctx, void * output, size_t read_size) -> size_t {
		auto * s = (SileroVadStream *)ctx;
		size_t available = s->size - s->pos;
		size_t to_read = std::min(read_size, available);
		memcpy(output, (const unsigned char *)s->data + s->pos, to_read);
		s->pos += to_read;
		return to_read;
	};
	loader.eof = [](void * ctx) -> bool {
		auto * s = (SileroVadStream *)ctx;
		return s->pos >= s->size;
	};
	loader.close = [](void * ctx) { };
	return whisper_vad_init_with_params(&loader, params);
}
```

3.5.3.
------
In the same section "Voice Activity Detection (VAD)" in whisper.h, we also declare 5 functions which are meant to
provide an interface for accessing information about VAD segments.
```
	WHISPER_API int whisper_full_n_vad_segments(struct whisper_context * ctx);
	WHISPER_API int64_t whisper_full_get_vad_segment_orig_start(struct whisper_context * ctx, int i_vad_segment);
	WHISPER_API int64_t whisper_full_get_vad_segment_orig_end(struct whisper_context * ctx, int i_vad_segment);
	WHISPER_API int64_t whisper_full_get_vad_segment_vad_start(struct whisper_context * ctx, int i_vad_segment);
	WHISPER_API int64_t whisper_full_get_vad_segment_vad_end(struct whisper_context * ctx, int i_vad_segment);
```
And we define these functions in whisper.cpp:
```
int whisper_full_n_vad_segments(struct whisper_context * ctx) {
	if (!ctx->state->has_vad_segments) {
		return 0;
	}
	return static_cast<int>(ctx->state->vad_segments.size());
}

int64_t whisper_full_get_vad_segment_orig_start(struct whisper_context * ctx, int i_vad_segment) {
	return ctx->state->vad_segments[i_vad_segment].orig_start;
}

int64_t whisper_full_get_vad_segment_orig_end(struct whisper_context * ctx, int i_vad_segment) {
	return ctx->state->vad_segments[i_vad_segment].orig_end;
}

int64_t whisper_full_get_vad_segment_vad_start(struct whisper_context * ctx, int i_vad_segment) {
	return ctx->state->vad_segments[i_vad_segment].vad_start;
}

int64_t whisper_full_get_vad_segment_vad_end(struct whisper_context * ctx, int i_vad_segment) {
	return ctx->state->vad_segments[i_vad_segment].vad_end;
}
```

3.5.4.
------
In whisper.cpp, function whisper_vad() is modified to include the reading from memory. This line:
```
		struct whisper_vad_context * vctx = whisper_vad_init_from_file_with_params(params.vad_model_path, vad_ctx_params);
```
is changed to these lines:
```
		struct whisper_vad_context * vctx = nullptr;
		if (params.vad_model_data && params.vad_model_data_size) {
			vctx = whisper_vad_init_from_memory_with_params((void*)params.vad_model_data, params.vad_model_data_size, vad_ctx_params);
		} else {
			vctx = whisper_vad_init_from_file_with_params(params.vad_model_path, vad_ctx_params);
		}
```

3.5.5. Endianness
-----------------
Replace any occurrences of
```
	#if defined(WHISPER_BIG_ENDIAN)
```
with
```
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
```

4. General compatibility with C++
---------------------------------
4.1. Assigning from `void *`
----------------------------
This is ruled out in C++, so we cast according to the following examples:
```
	galloc->node_allocs = ggml_calloc(graph->n_nodes, sizeof(struct node_alloc));   // C
	galloc->node_allocs = (struct node_alloc *) ggml_calloc(graph->n_nodes, sizeof(struct node_alloc));   // C++

	result->vals = GGML_CALLOC(result->set.size, sizeof(struct ggml_tensor *));   // C
	result->vals = (struct ggml_tensor **) GGML_CALLOC(result->set.size, sizeof(struct ggml_tensor *));   // C++

	char * const data = tensor->data;   // C
	char * const data = (char *) tensor->data;   // C++

	const float * l = left;   // C
	const float * l = (const float *) left;   // C++

	quantize_row_q2_K_ref(src, dst, (int64_t)nrow*n_per_row);   // C
	quantize_row_q2_K_ref(src, (block_q2_K *) dst, (int64_t)nrow*n_per_row);   // C++

	char (*atomic_current_chunk)[CACHE_LINE_SIZE] = blabla   // C
	char (*atomic_current_chunk)[CACHE_LINE_SIZE] = (char (*)[CACHE_LINE_SIZE]) blabla   // C++
```

4.2. Assigning int to enum
--------------------------
This is also ruled out in C++, so we cast according to the following examples:
```
	const enum ggml_op_pool op = ggml_get_op_params_i32(tensor, 0);
	const enum ggml_op_pool op = (enum ggml_op_pool) ggml_get_op_params_i32(tensor, 0);

	p->prio = 0;   // C
	p->prio = (enum ggml_sched_priority) 0;   // C++

	static struct ggml_state g_state = {0};   // C
	static struct ggml_state g_state {};   // C++
```

4.3. Assignments in initializer lists
-------------------------------------
Types have to match more closely in C++, so we cast according to the following examples:
```
	*cgraph = (struct ggml_cgraph) { size, ...   // C
	*cgraph = (struct ggml_cgraph) { (int) size, ...   // C++

	int32_t params[] = { nb1, nb2, nb3, offset, inplace ? 1 : 0 };   // C
	int32_t params[] = { (int32_t) nb1, (int32_t) nb2, (int32_t) nb3, (int32_t) offset, inplace ? 1 : 0 };   // C++

	MMID_MATRIX_ROW(i02, matrix_row_counts[i02]) = (struct mmid_row_mapping) {id, iid1};   // C
	MMID_MATRIX_ROW(i02, matrix_row_counts[i02]) = (struct mmid_row_mapping) {id, (int32_t) iid1};   // C++

	union { uint16_t u16; ggml_fp16_t fp16; } u = {i};   // C
	union { uint16_t u16; ggml_fp16_t fp16; } u = { (uint16_t) i };   // C++

    struct ggml_compute_params params = {
        /*.ith        =*/ state->ith,
        /*.nth        =*/ atomic_load_explicit(&tp->n_graph, memory_order_relaxed) & GGML_THREADPOOL_N_THREADS_MASK,
        /*.wsize      =*/ cplan->work_size,
        /*.wdata      =*/ cplan->work_data,
        /*.threadpool =*/ tp,
        /*.use_ref    =*/ cplan->use_ref,
    };   // C
    struct ggml_compute_params params = {
        /*.ith        =*/ state->ith,
        /*.nth        =*/ (int) (atomic_load_explicit(&tp->n_graph, memory_order_relaxed) & GGML_THREADPOOL_N_THREADS_MASK),
        /*.wsize      =*/ cplan->work_size,
        /*.wdata      =*/ cplan->work_data,
        /*.threadpool =*/ tp,
        /*.use_ref    =*/ cplan->use_ref,
    };   // C++
```

5. Models
---------
5.1. Bringing Silero-VAD model to Praat source code
---------------------------------------------------
First, we download the ggml Silero model from the original whisper.cpp repository:
```
whisper.cpp/models/download-vad-model.sh silero-v6.2.0
```
The result is `ggml-silero-v6.2.0.bin`, which is a C-compatible binary file, which can be loaded by whisper.cpp.

We then convert this binary to a C header using `xxd` and copy it to external/whispercpp directory:
```
xxd -i -n ggml_silero_bin -n whisper.cpp/models/ggml-silero-v6.2.0.bin > praat/external/whispercpp/ggml-silero-vad-model-data.h
```

5.2. Segmentation
-----------------
todo

5.3. Embedding
--------------
todo
