Praats/external/flac/READ_ME.TXT
Paul Boersma, 29 December 2025

This file describes the adaptations to the FLAC 1.5.0 sources
that are needed to make them compatible with Praat.

The FLAC sources are distributed over multiple folders, which are in different deep
branches of the FLAC root folder, as listed below. We flatten this hierarchy as shown.

flac-1.5.0
	config.cmake.h.in -> flac_config.h  BUT COMPLETELY CHANGED
	include
		FLAC
			all.h -> UNUSED
			assert.h -> flac_FLAC_assert.h
			callback.h -> flac_FLAC_callback.h
			export.h -> flac_FLAC_export.h
			format.h -> flac_FLAC_format.h
			metadata.h -> flac_FLAC_metadata.h
			ordinals.h -> flac_FLAC_ordinals.h
			stream_decoder.h -> flac_FLAC_stream_decoder.h
			stream_encoder.h -> flac_FLAC_stream_encoder.h
		share
			alloc.h -> flac_share_alloc.h
			compat.h -> flac_share_compat.h
			endswap.h -> flac_share_endswap.h
			macros.h -> flac_share_macros.h
			private.h -> flac_share_private.h
			safe_str.h -> flac_share_safe_str.h
			win_utf8_io.h -> flac_share_win_utf8_io.h
	src
		libFLAC
			bitmath.c -> flac_bitmath.c
			bitreader.c -> flac_bitreader.c
			bitwriter.c -> flac_bitwriter.c
			cpu.c -> flac_cpu.c
			crc.c -> flac_crc.c
			deduplication
				bitreader_read_rice_signed_block.c -> flac_deduplication_bitreader_read_rice_signed_block_c.h
				bitreader_lpc_compute_autocorrelation_intrin.c -> flac_deduplication_lpc_compute_autocorrelation_intrin_c.h
			fixed.c -> flac_fixed.c
			float.c -> flac_float.c
			format.c -> flac_format.c
			include
				private
					all.h -> UNUSED
					bitmath.h -> flac_private_bitmath.h
					bitreader.h -> flac_private_bitreader.h
					bitwriter.h -> flac_private_bitwriter.h
					cpu.h -> flac_private_cpu.h
					crc.h -> flac_private_crc.h
					fixed.h -> flac_private_fixed.h
					float.h -> flac_private_float.h
					format.h -> flac_private_format.h
					lpc.h -> flac_private_lpc.h
					macros.h -> flac_private_macros.h
					md5.h -> flac_private_md5.h
					memory.h -> flac_private_memory.h
					metadata.h -> flac_private_metadata.h
					ogg_decoder_aspect.h -> UNUSED
					ogg_encoder_aspect.h -> UNUSED
					ogg_helper.h -> UNUSED
					ogg_mapping.h -> UNUSED
					stream_encoder_framing.h -> flac_private_stream_encoder_framing.h
					stream_encoder.h -> flac_private_stream_encoder.h
					window.h -> flac_private_window.h
				protected
					all.h -> UNUSED
					stream_decoder.h -> flac_protected_stream_decoder.h
					stream_encoder.h -> flac_protected_stream_encoder.h
			lpc.c -> flac_lpc.c
			md5.c -> flac_md5.c
			memory.c -> flac_memory.c
			metadata_iterators.c -> flac_metadata_iterators.c
			metadata_object.c -> flac_metadata_object.c
			ogg_decoder_aspect.c -> flac_ogg_decoder_aspect.c
			ogg_encoder_aspect.c -> flac_ogg_encoder_aspect.c
			ogg_helper.c -> flac_ogg_helper.c
			ogg_mapping.c -> flac_ogg_mapping.c
			stream_decoder.c -> flac_stream_decoder.c
			stream_encoder.c -> flac_stream_encoder.c
			stream_encoder_framing.c -> flac_stream_encoder_framing.c
			window.c -> flac_window.c
		share
			win_utf8_io
				win_utf8_io.c -> flac_share_win_utf8_io.c

All these files are put into the single external/flac source folder.
The #include statements are flattened, e.g.
#include private/float.h becomes #include flac_private_float.h.

Any Praat-specific declarations should go in flac_config.h,
so we have to #include flac_config.h in most *.c files.
Including the config file is done in FLAC inside #if HAVE_CONFIG_H,
just as in many other libraries. This is appropriate for libraries that are compiled
separately, but not for libraries that are included in a larger project as source code.
So we remove all instances of #if HAVE_CONFIG_H. Specifically, we replace

#if HAVE_CONFIG_H
#  include <config.h>
#endif

with

//ppgb #if HAVE_CONFIG_H
#include "flac_config.h"
//ppgb #endif

in all *.c files.

We have #ifdef NDEBUG in flac_FLAC_assert.h. That is fine, because Praat wants to keep
assertions in its code.

flac_FLAC_stream_decoder.h:

typedef struct {
	struct FLAC__StreamDecoderProtected *protected_; /* avoid the C++ keyword 'protected' */
	struct FLAC__StreamDecoderPrivate *private_; /* avoid the C++ keyword 'private' */
} FLAC__StreamDecoder;

into

typedef struct FLAC__StreamDecoder {   //ppgb 20071120
	struct FLAC__StreamDecoderProtected *protected_; /* avoid the C++ keyword 'protected' */
	struct FLAC__StreamDecoderPrivate *private_; /* avoid the C++ keyword 'private' */
} FLAC__StreamDecoder;

flac_FLAC_stream_encoder.h:

typedef struct {
	struct FLAC__StreamEncoderProtected *protected_; /* avoid the C++ keyword 'protected' */
	struct FLAC__StreamEncoderPrivate *private_; /* avoid the C++ keyword 'private' */
} FLAC__StreamEncoder;

into

typedef struct FLAC__StreamEncoder {   //ppgb 20071120
	struct FLAC__StreamEncoderProtected *protected_; /* avoid the C++ keyword 'protected' */
	struct FLAC__StreamEncoderPrivate *private_; /* avoid the C++ keyword 'private' */
} FLAC__StreamEncoder;

Before #including any FLAC header files, do

#define FLAC__NO_DLL

Put #ifdef _WIN32 around flac_share_win_utf8_io.c.

Replace all occurrences of
	#if WORDS_BIGENDIAN
with
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

To minimize warnings, do something to
	#ifdef HAVE_STDINT_H
such as replacing it with
	#if HAVE_STDINT_H
