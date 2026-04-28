/* melder_audiofiles.cpp
 *
 * Copyright (C) 1992-2008,2010-2019,2021,2023-2026 Paul Boersma & David Weenink, 2007 Erez Volk (for FLAC)
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

#include "melder.h"
#define FLAC__NO_DLL
#include "../external/flac/flac_FLAC_metadata.h"
#include "../external/flac/flac_FLAC_stream_decoder.h"
#include "../external/flac/flac_FLAC_stream_encoder.h"
#include "../external/mp3/mp3.h"

/***** WRITING *****/

#ifndef WAVE_FORMAT_PCM
	#define WAVE_FORMAT_PCM  0x0001
#endif
#define WAVE_FORMAT_IEEE_FLOAT  0x0003
#define WAVE_FORMAT_ALAW  0x0006
#define WAVE_FORMAT_MULAW  0x0007
#define WAVE_FORMAT_DVI_ADPCM  0x0011
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

void MelderFile_writeAudioFileHeader (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint) {
	try {
		FILE *f = file -> filePointer;
		if (! f) return;
		const int numberOfBytesPerSamplePoint = (numberOfBitsPerSamplePoint + 7) / 8;
		switch (audioFileType) {
			case Melder_AIFF: {
				try {
					integer dataSize = numberOfSamples * numberOfBytesPerSamplePoint * numberOfChannels;

					/* Form Chunk: contains all other chunks. */
					if (fwrite ("FORM", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the FORM statement.");
					binputi32 (4 + (8 + 4) + (8 + 18) + (8 + 8 + dataSize), f);   // the size of the Form Chunk
					if (fwrite ("AIFF", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the AIFF file type.");

					/* Format Version Chunk: 8 + 4 bytes. */
					if (fwrite ("FVER", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the FVER statement.");
					binputu32 (4, f);   // the size of what follows
					binputu32 (0xA2805140, f);   // time of version

					/* Common Chunk: 8 + 18 bytes. */
					if (fwrite ("COMM", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the COMM statement.");
					binputi32 (18, f);   // the size of what follows
					binputi16 (numberOfChannels, f);
					binputi32 (numberOfSamples, f);
					binputi16 (numberOfBitsPerSamplePoint, f);
					binputr80 (sampleRate, f);

					/* Sound Data Chunk: 8 + 8 bytes + samples. */
					if (fwrite ("SSND", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the SSND statement.");
					binputi32 (8 + dataSize, f);   // the size of what follows
					binputi32 (0, f);   // offset
					binputi32 (0, f);   // block size
				} catch (MelderError) {
					Melder_throw (U"AIFF header not written.");
				}
			} break;
			case Melder_AIFC: {
				try {
					integer dataSize = numberOfSamples * numberOfBytesPerSamplePoint * numberOfChannels;

					/* Form Chunk: contains all other chunks. */
					if (fwrite ("FORM", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the FORM statement.");
					binputi32 (4 + (8 + 4) + (8 + 24) + (8 + 8 + dataSize), f);   // the size of the Form Chunk
					if (fwrite ("AIFC", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the AIFC file type.");

					/* Format Version Chunk: 8 + 4 bytes. */
					if (fwrite ("FVER", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the FVER statement.");
					binputu32 (4, f);   // the size of what follows
					binputu32 (0xA2805140, f);   // time of version

					/* Common Chunk: 8 + 24 bytes. */
					if (fwrite ("COMM", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the COMM statement.");
					binputi32 (24, f);   // the size of what follows
					binputi16 (numberOfChannels, f);
					binputi32 (numberOfSamples, f);
					binputi16 (numberOfBitsPerSamplePoint, f);
					binputr80 (sampleRate, f);
					if (fwrite ("NONE", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the compression type.");
					binputi16 (0, f);   // name of compression

					/* Sound Data Chunk: 8 + 8 bytes + samples. */
					if (fwrite ("SSND", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the SSND statement.");
					binputi32 (8 + dataSize, f);   // the size of what follows
					binputi32 (0, f);   // offset
					binputi32 (0, f);   // block size
				} catch (MelderError) {
					Melder_throw (U"AIFC header not written.");
				}
			} break;
			case Melder_WAV: {
				try {
					bool needExtensibleFormat =
						numberOfBitsPerSamplePoint > 16 ||
						numberOfChannels > 2 ||
						numberOfBitsPerSamplePoint != numberOfBytesPerSamplePoint * 8;
					const int formatSize = needExtensibleFormat ? 40 : 16;
					double dataSize_f = (double) numberOfSamples * (double) numberOfBytesPerSamplePoint * (double) numberOfChannels;
					if (dataSize_f > INT54_MAX)
						Melder_throw (U"Cannot save data over the 9-petabyte limit.");
					int64 dataSize = (int64) dataSize_f;

					/* RIFF Chunk: contains all other chunks. */
					if (fwrite ("RIFF", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the RIFF statement.");
					int64 sizeOfRiffChunk_i64 = 4 + (12 + formatSize) + (4 + dataSize);
					if (sizeOfRiffChunk_i64 > UINT32_MAX)
						Melder_throw (U"Cannot save a WAV file with more than ", UINT32_MAX, U" bytes.");
					uint32 sizeOfRiffChunk_u32 = (uint32) sizeOfRiffChunk_i64;
					binputu32LE (sizeOfRiffChunk_u32, f);
					if (fwrite ("WAVE", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the WAV file type.");

					/* Format Chunk: if 16-bits audio, then 8 + 16 bytes; else 8 + 40 bytes. */
					if (fwrite ("fmt ", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the FMT statement.");
					binputi32LE (formatSize, f);
					binputi16LE (needExtensibleFormat ? WAVE_FORMAT_EXTENSIBLE : WAVE_FORMAT_PCM, f);
					binputi16LE (numberOfChannels, f);
					binputi32LE (sampleRate, f);   // number of samples per second
					binputi32LE (sampleRate * numberOfBytesPerSamplePoint * numberOfChannels, f);   // average number of bytes per second
					binputi16LE (numberOfBytesPerSamplePoint * numberOfChannels, f);   // block alignment
					binputi16LE (numberOfBytesPerSamplePoint * 8, f);   // padded bits per sample
					if (needExtensibleFormat) {
						binputi16LE (22, f);   // extensionSize
						binputi16LE (numberOfBitsPerSamplePoint, f);   // valid bits per sample
						binputi32LE (0, f);   // speaker position mask
						binputi16LE (WAVE_FORMAT_PCM, f);
						if (fwrite ("\x00\x00\x00\x00\x10\x00\x80\x00\x00\xAA\x00\x38\x9B\x71",
							1, 14, f) != 14) Melder_throw (U"Error in file while trying to write the subformat.");
					}

					/* Data Chunk: 8 bytes + samples. */
					if (fwrite ("data", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the DATA statement.");
					binputi32LE (dataSize, f);
				} catch (MelderError) {
					Melder_throw (U"WAV header not written.");
				}
			} break;
			case Melder_NEXT_SUN: {
				try {
					if (fwrite (".snd", 1, 4, f) != 4) Melder_throw (U"Error in file while trying to write the .snd file type tag.");
					binputi32 (32, f);   // length of header
					binputi32 (numberOfSamples * 2 * numberOfChannels, f);   // length of data
					binputi32 (3, f);   // 16-bits linear, not mu-law or A-law
					binputi32 (sampleRate, f);
					binputi32 (numberOfChannels, f);
					binputi32 (0, f);
					binputi32 (0, f);
				} catch (MelderError) {
					Melder_throw (U"NeXT/Sun header not written.");
				}
			} break;
			case Melder_NIST: {
				try {
					char header [1024];
					memset (header, 0, 1024);
					snprintf (header,1024, "NIST_1A\n   1024\n"
						"channel_count -i %s\n"
						"sample_count -i %s\n"
						"sample_n_bytes -i %s\n"
						"sample_byte_format -s2 01\n" /* 01=LE 10=BE */
						"sample_coding -s3 pcm\n"
						"sample_rate -i %s\n"
						"sample_min -i -32768\n"
						"sample_max -i 32767\n"
						"end_head\n",
						Melder8_integer (numberOfChannels), Melder8_integer (numberOfSamples),
						Melder8_integer (numberOfBytesPerSamplePoint), Melder8_integer (sampleRate));
					if (fwrite (header, 1, 1024, f) != 1024) Melder_throw (U"Error in file while trying to write the NIST header.");
				} catch (MelderError) {
					Melder_throw (U"NIST header not written.");
				}
			} break;
			case Melder_FLAC: {
				try {
					if (numberOfChannels > (int) FLAC__MAX_CHANNELS)
						Melder_throw (U"FLAC files cannot have more than 8 channels.");
					FLAC__StreamEncoder *encoder = FLAC__stream_encoder_new ();
					if (! encoder)
						Melder_throw (U"Error creating FLAC stream encoder.");
					FLAC__stream_encoder_set_bits_per_sample (encoder, numberOfBitsPerSamplePoint);
					FLAC__stream_encoder_set_channels (encoder, numberOfChannels);
					FLAC__stream_encoder_set_sample_rate (encoder, sampleRate);
					FLAC__stream_encoder_set_total_samples_estimate (encoder, numberOfSamples);
					if (FLAC__stream_encoder_init_FILE (encoder, file -> filePointer, nullptr, nullptr) != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
						FLAC__stream_encoder_delete (encoder);
						Melder_throw (U"Error creating FLAC stream encoder");
					}
					file -> flacEncoder = encoder;   // only after we know it's correct (for MelderFile_close)
					file -> outputEncoding = kMelder_textOutputEncoding_FLAC;   // only after we know it's correct (for MelderFile_close)
				} catch (MelderError) {
					Melder_throw (U"FLAC header not written.");
				}
			} break;
			default: Melder_throw (U"Unknown audio file type ", audioFileType, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"16-bit audio file header not written.");
	}
}

void MelderFile_writeAudioFileTrailer (MelderFile file, int audioFileType, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint) {
	(void) sampleRate;
	bool shouldPadTheDataToAnEvenNumberOfBytes = audioFileType == Melder_WAV;
	bool numberOfSamplesIsOdd = (numberOfSamples & 1) != 0;
	bool numberOfChannelsIsOdd = (numberOfChannels & 1) != 0;
	int numberOfBytesPerSamplePoint = (numberOfBitsPerSamplePoint + 7) / 8;
	bool numberOfBytesPerSamplePointIsOdd = (numberOfBytesPerSamplePoint & 1) != 0;
	bool needToPadOneByte = shouldPadTheDataToAnEvenNumberOfBytes && numberOfSamplesIsOdd && numberOfChannelsIsOdd && numberOfBytesPerSamplePointIsOdd;
	if (needToPadOneByte && file -> filePointer)
		binputi8 (0, file -> filePointer);
}

static const conststring32 audioFileTypeString [] = { U"none", U"AIFF", U"AIFC", U"WAV", U"NeXT/Sun", U"NIST", U"FLAC", U"MP3" };
conststring32 Melder_audioFileTypeString (int audioFileType) { return audioFileType > Melder_NUMBER_OF_AUDIO_FILE_TYPES ? U"unknown" : audioFileTypeString [audioFileType]; }
static const conststring32 macAudioFileType [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { U"", U"AIFF", U"AIFC", U"WAVE", U"ULAW", U"NIST", U"FLAC", U"MP3" };
static const conststring32 winAudioFileExtension [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { U"", U".aiff", U".aifc", U".wav", U".au", U".nist", U".flac", U".mp3" };
static int defaultAudioFileEncoding16 [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { 0, Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_LITTLE_ENDIAN,
	     Melder_LINEAR_16_BIG_ENDIAN, Melder_LINEAR_16_LITTLE_ENDIAN,
	     Melder_FLAC_COMPRESSION_16, Melder_MPEG_COMPRESSION_16 };
static int defaultAudioFileEncoding24 [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { 0, Melder_LINEAR_24_BIG_ENDIAN, Melder_LINEAR_24_BIG_ENDIAN, Melder_LINEAR_24_LITTLE_ENDIAN,
	     Melder_LINEAR_24_BIG_ENDIAN, Melder_LINEAR_24_LITTLE_ENDIAN,
	     Melder_FLAC_COMPRESSION_24, Melder_MPEG_COMPRESSION_24 };
static int defaultAudioFileEncoding32 [1+Melder_NUMBER_OF_AUDIO_FILE_TYPES]
	= { 0, Melder_LINEAR_32_BIG_ENDIAN, Melder_LINEAR_32_BIG_ENDIAN, Melder_LINEAR_32_LITTLE_ENDIAN,
	     Melder_LINEAR_32_BIG_ENDIAN, Melder_LINEAR_32_LITTLE_ENDIAN,
	     Melder_FLAC_COMPRESSION_32, Melder_MPEG_COMPRESSION_32 };
int Melder_defaultAudioFileEncoding (int audioFileType, int numberOfBitsPerSamplePoint) {
	return
		numberOfBitsPerSamplePoint == 16 ? defaultAudioFileEncoding16 [audioFileType] :
		numberOfBitsPerSamplePoint == 24 ? defaultAudioFileEncoding24 [audioFileType] :
		defaultAudioFileEncoding32 [audioFileType];
}

void MelderFile_writeAudioFile (MelderFile file, int audioFileType, const short *buffer, integer sampleRate, integer numberOfSamples, integer numberOfChannels, int numberOfBitsPerSamplePoint) {
	try {
		autoMelderFile mfile = MelderFile_create (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, sampleRate, numberOfSamples, numberOfChannels, numberOfBitsPerSamplePoint);
		MelderFile_writeShortToAudio (file, numberOfChannels, defaultAudioFileEncoding16 [audioFileType], buffer, numberOfSamples);
		MelderFile_writeAudioFileTrailer (file, audioFileType, sampleRate, numberOfSamples, numberOfChannels, numberOfBitsPerSamplePoint);
		mfile.close ();
	} catch (MelderError) {
		Melder_throw (U"16-bit audio file not written.");
	}
}

/***** READING *****/

int Melder_bytesPerSamplePoint (int encoding) {
	return
		encoding == Melder_LINEAR_16_BIG_ENDIAN || encoding == Melder_LINEAR_16_LITTLE_ENDIAN ? 2 :
		encoding == Melder_LINEAR_24_BIG_ENDIAN || encoding == Melder_LINEAR_24_LITTLE_ENDIAN ? 3 :
		encoding == Melder_LINEAR_32_BIG_ENDIAN || encoding == Melder_LINEAR_32_LITTLE_ENDIAN ||
		encoding == Melder_IEEE_FLOAT_32_BIG_ENDIAN || encoding == Melder_IEEE_FLOAT_32_LITTLE_ENDIAN ? 4 :
		encoding == Melder_IEEE_FLOAT_64_BIG_ENDIAN || encoding == Melder_IEEE_FLOAT_64_LITTLE_ENDIAN ? 8 :
		1;
}

static int16 ulaw2linear [] = {
  -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
  -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
  -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
  -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
   -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
   -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
   -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
   -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
   -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
   -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
    -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
    -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
    -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
    -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
    -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
     -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
   32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
   23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
   15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
   11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
    7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
    5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
    3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
    2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
    1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
    1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
     876,    844,    812,    780,    748,    716,    684,    652,
     620,    588,    556,    524,    492,    460,    428,    396,
     372,    356,    340,    324,    308,    292,    276,    260,
     244,    228,    212,    196,    180,    164,    148,    132,
     120,    112,    104,     96,     88,     80,     72,     64,
      56,     48,     40,     32,     24,     16,      8,      0
};

static int16 alaw2linear [] = {
   -5504,  -5248,  -6016,  -5760,  -4480,  -4224,  -4992,  -4736,
   -7552,  -7296,  -8064,  -7808,  -6528,  -6272,  -7040,  -6784,
   -2752,  -2624,  -3008,  -2880,  -2240,  -2112,  -2496,  -2368,
   -3776,  -3648,  -4032,  -3904,  -3264,  -3136,  -3520,  -3392,
  -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944,
  -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136,
  -11008, -10496, -12032, -11520,  -8960,  -8448,  -9984,  -9472,
  -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568,
    -344,   -328,   -376,   -360,   -280,   -264,   -312,   -296,
    -472,   -456,   -504,   -488,   -408,   -392,   -440,   -424,
     -88,    -72,   -120,   -104,    -24,     -8,    -56,    -40,
    -216,   -200,   -248,   -232,   -152,   -136,   -184,   -168,
   -1376,  -1312,  -1504,  -1440,  -1120,  -1056,  -1248,  -1184,
   -1888,  -1824,  -2016,  -1952,  -1632,  -1568,  -1760,  -1696,
    -688,   -656,   -752,   -720,   -560,   -528,   -624,   -592,
    -944,   -912,  -1008,   -976,   -816,   -784,   -880,   -848,
    5504,   5248,   6016,   5760,   4480,   4224,   4992,   4736,
    7552,   7296,   8064,   7808,   6528,   6272,   7040,   6784,
    2752,   2624,   3008,   2880,   2240,   2112,   2496,   2368,
    3776,   3648,   4032,   3904,   3264,   3136,   3520,   3392,
   22016,  20992,  24064,  23040,  17920,  16896,  19968,  18944,
   30208,  29184,  32256,  31232,  26112,  25088,  28160,  27136,
   11008,  10496,  12032,  11520,   8960,   8448,   9984,   9472,
   15104,  14592,  16128,  15616,  13056,  12544,  14080,  13568,
     344,    328,    376,    360,    280,    264,    312,    296,
     472,    456,    504,    488,    408,    392,    440,    424,
      88,     72,    120,    104,     24,      8,     56,     40,
     216,    200,    248,    232,    152,    136,    184,    168,
    1376,   1312,   1504,   1440,   1120,   1056,   1248,   1184,
    1888,   1824,   2016,   1952,   1632,   1568,   1760,   1696,
     688,    656,    752,    720,    560,    528,    624,    592,
     944,    912,   1008,    976,    816,    784,    880,    848
};

/*
	The Shorten encoding employs Alaw, but because they use prediction,
	they have to use *sorted* Alaw, so we provide the table here.

	The table was created from the previous table as follows:
	1. Copy all lines between the braces to a file `alawValues.txt`.
	2. Remove all commas (with Replace in BBEdit).
	3. Run the following script:
	{
		matrix = Read Matrix from raw text file: "alawValues.txt"
		matrix## = Get all values
		vector# = sort#: combine# (matrix##)
		writeInfo: ""
		for i to 256
			if i mod 8 = 1
				appendInfo: newline$, "  "
			else
				appendInfo: " "
			endif
			value$ = string$ (vector# [i])
			spaces$ = left$ ("         ", 6 - length (value$))
			appendInfo: spaces$, value$, ","
		endfor
	}
*/
static int16 sortedAlawValues [] = {
  -32256, -31232, -30208, -29184, -28160, -27136, -26112, -25088,
  -24064, -23040, -22016, -20992, -19968, -18944, -17920, -16896,
  -16128, -15616, -15104, -14592, -14080, -13568, -13056, -12544,
  -12032, -11520, -11008, -10496,  -9984,  -9472,  -8960,  -8448,
   -8064,  -7808,  -7552,  -7296,  -7040,  -6784,  -6528,  -6272,
   -6016,  -5760,  -5504,  -5248,  -4992,  -4736,  -4480,  -4224,
   -4032,  -3904,  -3776,  -3648,  -3520,  -3392,  -3264,  -3136,
   -3008,  -2880,  -2752,  -2624,  -2496,  -2368,  -2240,  -2112,
   -2016,  -1952,  -1888,  -1824,  -1760,  -1696,  -1632,  -1568,
   -1504,  -1440,  -1376,  -1312,  -1248,  -1184,  -1120,  -1056,
   -1008,   -976,   -944,   -912,   -880,   -848,   -816,   -784,
    -752,   -720,   -688,   -656,   -624,   -592,   -560,   -528,
    -504,   -488,   -472,   -456,   -440,   -424,   -408,   -392,
    -376,   -360,   -344,   -328,   -312,   -296,   -280,   -264,
    -248,   -232,   -216,   -200,   -184,   -168,   -152,   -136,
    -120,   -104,    -88,    -72,    -56,    -40,    -24,     -8,
       8,     24,     40,     56,     72,     88,    104,    120,
     136,    152,    168,    184,    200,    216,    232,    248,
     264,    280,    296,    312,    328,    344,    360,    376,
     392,    408,    424,    440,    456,    472,    488,    504,
     528,    560,    592,    624,    656,    688,    720,    752,
     784,    816,    848,    880,    912,    944,    976,   1008,
    1056,   1120,   1184,   1248,   1312,   1376,   1440,   1504,
    1568,   1632,   1696,   1760,   1824,   1888,   1952,   2016,
    2112,   2240,   2368,   2496,   2624,   2752,   2880,   3008,
    3136,   3264,   3392,   3520,   3648,   3776,   3904,   4032,
    4224,   4480,   4736,   4992,   5248,   5504,   5760,   6016,
    6272,   6528,   6784,   7040,   7296,   7552,   7808,   8064,
    8448,   8960,   9472,   9984,  10496,  11008,  11520,  12032,
   12544,  13056,  13568,  14080,  14592,  15104,  15616,  16128,
   16896,  17920,  18944,  19968,  20992,  22016,  23040,  24064,
   25088,  26112,  27136,  28160,  29184,  30208,  31232,  32256
};

static void Melder_checkAiffFile (FILE *f, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	/*
		Read header of AIFF(-C) file: 12 bytes.
	*/
	char data [8];
	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no FORM statement.");
	Melder_require (strnequ (data, "FORM", 4),
		U"Not an AIFF or AIFC file (FORM statement expected).");
	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no size of FORM chunk.");
	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no file type info (expected AIFF or AIFC).");
	Melder_require (strnequ (data, "AIFF", 4) || strnequ (data, "AIFC", 4),
		U"Not an AIFF or AIFC file (wrong file type info).");
	const bool isAifc = strnequ (data, "AIFC", 4);

	/* Search for Common Chunk and Data Chunk. */

	char chunkID [4];
	bool commonChunkPresent = false, dataChunkPresent = false;
	int numberOfBitsPerSamplePoint;
	while (fread (chunkID, 1, 4, f) == 4) {
		integer chunkSize = bingeti32 (f);
		if (chunkSize & 1)
			++ chunkSize;   // round up to nearest even number
		/* IN SOUND FILES PRODUCED BY THE SGI'S soundeditor PROGRAM, */
		/* THE COMMON CHUNK HAS A chunkSize OF 18 INSTEAD OF 38, */
		/* ALTHOUGH THE COMMON CHUNK CONTAINS */
		/* THE 20-BYTE SEQUENCE "\016not compressed\0". */
		/* START FIX OF FOREIGN BUG */
		if(strnequ(chunkID,"NONE",4)&&
			(chunkSize==(14<<24)+('n'<<16)+('o'<<8)+'t'||chunkSize==('t'<<24)+('o'<<16)+('n'<<8)+14))
		{Melder_casual(U"Ha! a buggy SGI \"soundeditor\" file...");for(integer i=1;i<=20/*diff*/-8/*header*/;i++)fread(data,1,1,f);continue;}
		/* FINISH FIX OF FOREIGN BUG */
		if (strnequ (chunkID, "COMM", 4)) {
			/*
				Found a Common Chunk.
			*/
			commonChunkPresent = true;
			*numberOfChannels = bingeti16 (f);
			Melder_require (*numberOfChannels > 0,
				U"Too few sound channels (", *numberOfChannels, U").");
			*numberOfSamples = bingeti32 (f);
			Melder_require (*numberOfSamples > 0,
				U"Too few samples ", *numberOfSamples, U").");
			numberOfBitsPerSamplePoint = bingeti16 (f);
			Melder_require (numberOfBitsPerSamplePoint <= 32,
				U"Too many bits per sample (", numberOfBitsPerSamplePoint, U"; the maximum is 32).");
			*encoding =
				numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_BIG_ENDIAN :
				numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_BIG_ENDIAN :
				numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_BIG_ENDIAN :
				Melder_LINEAR_8_SIGNED;
			*sampleRate = bingetr80 (f);
			Melder_require (*sampleRate > 0.0,
				U"Wrong sampling frequency (", *sampleRate, U" Hz).");
			if (isAifc) {
				/*
					Read compression data; should be "NONE" or "sowt".
				*/
				Melder_require (fread (data, 1, 4, f) == 4,
					U"File too small: no compression info.");
				if (! strnequ (data, "NONE", 4) && ! strnequ (data, "sowt", 4)) {
					data [4] = '\0';
					Melder_throw (U"Cannot read compressed AIFC files (compression type ", Melder_peek8to32_u (data), U").");
				}
				if (strnequ (data, "sowt", 4))
					*encoding =
						numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_LITTLE_ENDIAN :
						Melder_LINEAR_8_SIGNED;
				/*
					Read rest of compression info.
				*/
				for (integer i = 23; i <= chunkSize; i ++)
					Melder_require (fread (data, 1, 1, f) == 1,
						U"File too small: expected chunk of ", chunkSize, U" bytes, but found ", i + 22, U".");
			}
		} else if (strnequ (chunkID, "SSND", 4)) {
			/*
				Found a Data Chunk.
			*/
			dataChunkPresent = true;
			*startOfData = ftell (f) + 8;   // ignore "offset" (4 bytes) and "blocksize" (4 bytes)
			if (commonChunkPresent)
				break;   // optimization: do not read whole data chunk if we have already read the common chunk
		} else // ignore Version Chunk and unknown chunks
			for (integer i = 1; i <= chunkSize; i ++)
				Melder_require (fread (data, 1, 1, f) == 1,
					U"File too small: expected ", chunkSize, U" bytes, but found ", i, U".");
	}

	Melder_require (commonChunkPresent,
		U"Found no Common Chunk.");
	Melder_require (dataChunkPresent,
		U"Found no Data Chunk.");
}

static void Melder_checkWavFile (FILE *f, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	char data [14], chunkID [4];
	bool formatChunkPresent = false, dataChunkPresent = false;
	int numberOfBitsPerSamplePoint = -1;
	uint32 dataChunkSize = 0xffffffff;

	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no RIFF statement.");
	Melder_require (strnequ (data, "RIFF", 4),
		U"Not a WAV file (RIFF statement expected).");
	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no size of RIFF chunk.");
	Melder_require (fread (data, 1, 4, f) == 4,
		U"File too small: no file type info (expected WAVE statement).");
	Melder_require (strnequ (data, "WAVE", 4) || strnequ (data, "CDDA", 4),
		U"Not a WAVE or CD audio file (wrong file type info).");

	/*
		Search for Format Chunk and Data Chunk.
	*/
	while (fread (chunkID, 1, 4, f) == 4) {
		uint32 chunkSize = bingetu32LE (f);
		if (Melder_debug == 23) {
			Melder_warning (chunkID [0], U" ", chunkID [1], U" ",
				chunkID [2], U" ", chunkID [3], U" ", chunkSize);
		}
		if (strnequ (chunkID, "fmt ", 4)) {
			/*
				Found a Format Chunk.
			*/
			uint16 winEncoding = bingetu16LE (f);
			formatChunkPresent = true;			
			*numberOfChannels = bingeti16LE (f);
			Melder_require (*numberOfChannels >= 1,
				U"Too few sound channels (", *numberOfChannels, U").");
			*sampleRate = (double) bingeti32LE (f);
			Melder_require (*sampleRate > 0.0,
				U"Wrong sampling frequency (", *sampleRate, U" Hz).");
			(void) bingeti32LE (f);   // avgBytesPerSec
			(void) bingeti16LE (f);   // blockAlign
			numberOfBitsPerSamplePoint = bingeti16LE (f);
			if (numberOfBitsPerSamplePoint == 0)
				numberOfBitsPerSamplePoint = 16;   // the default
			Melder_require (numberOfBitsPerSamplePoint >= 4,
				U"Too few bits per sample (", numberOfBitsPerSamplePoint, U"; the minimum is 4).");
			Melder_require (numberOfBitsPerSamplePoint <= 64,
				U"Too many bits per sample (", numberOfBitsPerSamplePoint, U"; the maximum is 64).");
			switch (winEncoding) {
				case WAVE_FORMAT_PCM:
					*encoding =
						numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_LITTLE_ENDIAN :
						numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_LITTLE_ENDIAN :
						Melder_LINEAR_8_UNSIGNED;
					break;
				case WAVE_FORMAT_IEEE_FLOAT:
					*encoding =
						numberOfBitsPerSamplePoint == 64 ? Melder_IEEE_FLOAT_64_LITTLE_ENDIAN :
						Melder_IEEE_FLOAT_32_LITTLE_ENDIAN;
					break;
				case WAVE_FORMAT_ALAW:
					*encoding = Melder_ALAW;
					break;
				case WAVE_FORMAT_MULAW:
					*encoding = Melder_MULAW;
					break;
				case WAVE_FORMAT_DVI_ADPCM:
					Melder_throw (U"Cannot read lossy compressed audio files (this one is DVI ADPCM).\n"
						U"Please use uncompressed audio files. If you must open this file,\n"
						U"please use an audio converter program to convert it first to normal (PCM) WAV format\n"
						U"(Praat may have difficulty analysing the poor recording, though).");
				case WAVE_FORMAT_EXTENSIBLE: {
					Melder_require (chunkSize >= 40,
						U"Not enough format data in extensible WAV format");
					(void) bingeti16LE (f);   // extensionSize
					(void) bingeti16LE (f);   // validBitsPerSample
					(void) bingeti32LE (f);   // channelMask
					uint16 winEncoding2 = bingetu16LE (f);   // override
					switch (winEncoding2) {
						case WAVE_FORMAT_PCM:
							*encoding =
								numberOfBitsPerSamplePoint > 24 ? Melder_LINEAR_32_LITTLE_ENDIAN :
								numberOfBitsPerSamplePoint > 16 ? Melder_LINEAR_24_LITTLE_ENDIAN :
								numberOfBitsPerSamplePoint > 8 ? Melder_LINEAR_16_LITTLE_ENDIAN :
								Melder_LINEAR_8_UNSIGNED;
							break;
						case WAVE_FORMAT_IEEE_FLOAT:
							*encoding =
								numberOfBitsPerSamplePoint == 64 ? Melder_IEEE_FLOAT_64_LITTLE_ENDIAN :
								Melder_IEEE_FLOAT_32_LITTLE_ENDIAN;
							break;
						case WAVE_FORMAT_ALAW:
							*encoding = Melder_ALAW;
							break;
						case WAVE_FORMAT_MULAW:
							*encoding = Melder_MULAW;
							break;
						case WAVE_FORMAT_DVI_ADPCM:
							Melder_throw (U"Cannot read lossy compressed audio files (this one is DVI ADPCM).\n"
								U"Please use uncompressed audio files. If you must open this file,\n"
								U"please use an audio converter program to convert it first to normal (PCM) WAV format\n"
								U"(Praat may have difficulty analysing the poor recording, though).");
						default:
							Melder_throw (U"Unsupported Windows audio encoding ", winEncoding2, U".");
					}
					Melder_require (fread (data, 1, 14, f) == 14,
						U"File too small: no SubFormat data.");
					continue;   // next chunk
				}
				default:
					Melder_throw (U"Unsupported Windows audio encoding ", winEncoding, U".");
			}
			if (chunkSize & 1)
				chunkSize += 1;
			for (integer i = 17; i <= chunkSize; i ++)
				Melder_require (fread (data, 1, 1, f) == 1,
					U"File too small: expected ", chunkSize, U" bytes in fmt chunk, but found ", i, U".");
		} else if (strnequ (chunkID, "data", 4)) {
			/*
				Found a Data Chunk.
			*/
			dataChunkPresent = true;
			dataChunkSize = chunkSize;
			*startOfData = ftell (f);
			if (chunkSize > UINT32_MAX - 100) {   // incorrect data chunk (sometimes -1 or -44); assume that the data run till the end of the file
				fseeko (f, 0LL, SEEK_END);
				off_t endOfData = ftello (f);
				dataChunkSize = chunkSize = endOfData - *startOfData;
				fseeko (f, *startOfData, SEEK_SET);
			}
			/*
				ppgb 20191211:
				The following check had to be moved here (from six lines up),
				because the original chunkSize could have been 0xffffffff,
				leading to it becoming 0 here and skipping the above block,
				thus violating the assert on dataChunkSize below.
			*/
			if (chunkSize & 1)
				chunkSize += 1;
			if (Melder_debug == 23) {
				for (integer i = 1; i <= chunkSize; i ++)
					Melder_require (fread (data, 1, 1, f) == 1,
						U"File too small: expected ", chunkSize, U" bytes of data, but found ", i, U".");
			} else {
				if (formatChunkPresent)
					break;   // OPTIMIZE: do not read the whole data chunk if we have already read the format chunk
			}
		} else {   // ignore other chunks
			if (chunkSize & 1)
				chunkSize += 1;
			for (integer i = 1; i <= chunkSize; i ++)
				Melder_require (fread (data, 1, 1, f) == 1,
					U"File too small: expected ", chunkSize, U" bytes, but found ", i, U".");
		}
	}

	Melder_require (formatChunkPresent,
		U"Found no Format Chunk.");
	Melder_require (dataChunkPresent,
		U"Found no Data Chunk.");
	Melder_assert (numberOfBitsPerSamplePoint != -1);
	Melder_assert (dataChunkSize != 0xffffffff);
	*numberOfSamples = dataChunkSize / *numberOfChannels / ((numberOfBitsPerSamplePoint + 7) / 8);
}

static void Melder_checkNextSunFile (FILE *f, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	char tag [4];
	fread (tag, 1, 4, f);
	if (strncmp (tag, ".snd", 4)) Melder_throw (U"Not a Sun audio file.");
	*startOfData = bingeti32 (f);
	if (*startOfData < 24 || *startOfData > 320)
		Melder_throw (U"Cannot read header of audio file. Length ", *startOfData, U".");
	integer dataSize = bingeti32 (f);
	if (dataSize <= 0) {
		/*
		 * Incorrect information. Get it from file length.
		 */
		integer save = ftell (f);
		fseek (f, 0, SEEK_END);
		dataSize = ftell (f) - *startOfData;
		fseek (f, save, SEEK_SET);
	}
	integer sunEncoding = bingeti32 (f);
	switch (sunEncoding) {
		case 1: *encoding = Melder_MULAW; break;
		case 2: *encoding = Melder_LINEAR_8_SIGNED; break;
		case 3: *encoding = Melder_LINEAR_16_BIG_ENDIAN; break;
		case 27: *encoding = Melder_ALAW; break;
		default: Melder_throw (U"Cannot translate audio file encoding ", sunEncoding, U".");
	}
	*sampleRate = bingeti32 (f);
	if (*sampleRate <= 0) Melder_throw (U"Impossible sampling frequency ", *sampleRate, U" Hz.");
	*numberOfChannels = bingeti32 (f);
	if (*numberOfChannels < 1)
		Melder_throw (U"Wrong number of channels in audio file (", *numberOfChannels, U").");
	*numberOfSamples = dataSize / Melder_bytesPerSamplePoint (*encoding) / *numberOfChannels;
	integer skip = *startOfData - 24;
	while (skip -- > 0) (void) fgetc (f);
}

static int nistGetValue (const char *header, const char *object, double *rval, char *sval) {
	char obj [30], type [10];
	const char *match = strstr (header, object);
	if (! match)
		return 0;
	if (sscanf (match, "%29s%9s%99s", obj, type, sval) != 3)
		return 0;
	if (strequ (type, "-i") || strequ (type, "-r"))
		*rval = atof (sval);
	else if (strncmp (type, "-s", 2))
		return 0;
	return 1;
}
static void Melder_checkNistFile (FILE *f, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	char header [2048+1], sval [100];
 	double rval = 0.0;
	int numberOfBytesPerSamplePoint;
	if (fread (header, 1, 1024, f) != 1024)
		Melder_throw (U"Cannot read NISTheader.");
	if (! strnequ (header, "NIST_1A", 7))
		Melder_throw (U"Not a NIST sound file.");
	if (strnequ (header + 7, "\n   1024", 8)) {
		header [1024] = '\0';
	} else if (strnequ (header + 7, "\n   2048", 8)) {
		if (fread (header + 1024, 1, 1024, f) != 1024)
			Melder_throw (U"Cannot read longer NISTheader.");
		header [2048] = '\0';
	} else
		Melder_throw (U"Incomplete or too long NISTheader.");
	*startOfData = atol (header + 9);
	if (! nistGetValue (header, "sample_count", & rval, sval) || rval < 1.0)
		Melder_throw (U"Incorrect number of samples in NIST file.");
	*numberOfSamples = Melder_iround (rval);
	if (! nistGetValue (header, "sample_n_bytes", & rval, sval) || rval < 1.0 || rval > 2.0)
		Melder_throw (U"Incorrect number of bytes per sample (should be 1 or 2).");
	numberOfBytesPerSamplePoint = Melder_iround (rval);
	if (! nistGetValue (header, "channel_count", & rval, sval) || rval < 1.0)
		Melder_throw (U"Incorrect number of channels.");
	*numberOfChannels = Melder_iround (rval);
	if (! nistGetValue (header, "sample_rate", sampleRate, sval) || *sampleRate < 1)
		Melder_throw (U"Incorrect sampling frequency ", *sampleRate, U" Hz.");
	*encoding = Melder_LINEAR_16_BIG_ENDIAN;
	if (nistGetValue (header, "sample_byte_format", & rval, sval) && strequ (sval, "01"))
		*encoding = Melder_LINEAR_16_LITTLE_ENDIAN;
	if (numberOfBytesPerSamplePoint == 1)
		*encoding = Melder_LINEAR_8_SIGNED;
	if (nistGetValue (header, "sample_coding", & rval, sval)) {
		if (strnequ (sval, "ulaw", 4))
			*encoding = Melder_MULAW;
		else if (strstr (sval, "embedded-shorten-v"))
			if (nistGetValue (header, "database_id", & rval, sval) && strequ (sval, "POLYPHONE-NL"))
				*encoding = Melder_POLYPHONE;
			else
				*encoding = Melder_SHORTEN;
		else if (strnequ (sval, "alaw", 4))   // must be after previous, because some files have "alaw,embedded..."
			*encoding = Melder_ALAW;
	}
}

static void Melder_checkFlacFile (MelderFile file, integer *numberOfChannels_out, int *encoding_out,
	double *sampleRate_out, integer *startOfData_out, integer *numberOfSamples_out)
{
	const conststring8 fileName_utf8 = MelderFile_peekPath8 (file);
	FLAC__StreamMetadata metadata;
	if (! FLAC__metadata_get_streaminfo (fileName_utf8, & metadata))   // Unicode-savvy (test/fon/soundFiles.praat 2024-08-11)
		Melder_throw (U"Invalid FLAC file");
	FLAC__StreamMetadata_StreamInfo *info = & metadata. data. stream_info;

	/*
		Get and check number of channels.
	*/
	const uint32 numberOfChannels_uint32 = info -> channels;   // no conversion
	if (sizeof (integer) == 4)
		Melder_require (numberOfChannels_uint32 <= (uint32) INT32_MAX,
			U"FLAC file has too many channels (", Melder_bigInteger (numberOfChannels_uint32), U") for 32-bit Praat.");
	*numberOfChannels_out = (integer) numberOfChannels_uint32;   // guarded conversion down (on 32-bit)

	*encoding_out = Melder_FLAC_COMPRESSION_16;

	/*
		Get and check sampling frequency.
	*/
	const uint32 samplingFrequency_uint32 = info -> sample_rate;   // no conversion
	Melder_require (samplingFrequency_uint32 > 0,
		U"FLAC file cannot have a sampling frequency of 0 Hz.");
	*sampleRate_out = (double) samplingFrequency_uint32;   // safe conversion from uint32 to 54-bit-mantissa float

	*startOfData_out = 0;   // meaningless: libFLAC does the I/O

	/*
		Get and check number of samples.
	*/
	const uint64 numberOfSamples_uint64 = info -> total_samples;   // no conversion
	Melder_require (numberOfSamples_uint64 <= (uint64) INT64_MAX,
		U"FLAC file claims that it contains more than ", Melder_bigInteger (INT64_MAX), U" samples; probably a mistake.");
	const int64 numberOfSamples_int64 = (int64) numberOfSamples_uint64;   // guarded conversion down
	if (sizeof (integer) == 4)
		Melder_require (numberOfSamples_int64 <= (int64) INT32_MAX,
			U"FLAC file too long for 32-bit Praat, namely ", Melder_bigInteger (numberOfSamples_int64), U" samples."
			"You may want to try a 64-bit edition of Praat."
		);
	*numberOfSamples_out = (integer) numberOfSamples_int64;   // guarded conversion down
}

static void Melder_checkMp3File (FILE *f, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	MP3_FILE mp3f = mp3f_new ();
	mp3f_set_file (mp3f, f);
	if (! mp3f_analyze (mp3f)) {
		mp3f_delete (mp3f);
		Melder_throw (U"Cannot analyze MP3 file");
	}
	*encoding = Melder_MPEG_COMPRESSION_16;
	*numberOfChannels = mp3f_channels (mp3f);
	*sampleRate = mp3f_frequency (mp3f);
	*numberOfSamples = mp3f_samples (mp3f);
	if ((MP3F_OFFSET)*numberOfSamples != mp3f_samples (mp3f))   // BUG: loses bits above INT32_MAX
		Melder_throw (U"MP3 file too long.");
	*startOfData = 0;   // meaningless
	mp3f_delete (mp3f);
}

int MelderFile_checkSoundFile (MelderFile file, integer *numberOfChannels, int *encoding,
	double *sampleRate, integer *startOfData, integer *numberOfSamples)
{
	char data [16];
	FILE *f = file -> filePointer;
	if (! f || fread (data, 1, 16, f) < 16) return 0;
	rewind (f);
	if (strnequ (data, "FORM", 4) && strnequ (data + 8, "AIFF", 4)) {
		Melder_checkAiffFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_AIFF;
	}
	if (strnequ (data, "FORM", 4) && strnequ (data + 8, "AIFC", 4)) {
		Melder_checkAiffFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_AIFC;
	}
	if (strnequ (data, "RIFF", 4) && (strnequ (data + 8, "WAVE", 4) || strnequ (data + 8, "CDDA", 4))) {
		Melder_checkWavFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_WAV;
	}
	if (strnequ (data, ".snd", 4)) {
		Melder_checkNextSunFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_NEXT_SUN;
	}
	if (strnequ (data, "NIST_1A", 7)) {
		Melder_checkNistFile (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_NIST;
	}
	if (strnequ (data, "fLaC", 4)) {
		Melder_checkFlacFile (file, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_FLAC;
	}
	if (mp3_recognize (16, data)) {
		Melder_checkMp3File (f, numberOfChannels, encoding, sampleRate, startOfData, numberOfSamples);
		return Melder_MP3;
	}
	return 0;   // not a recognized sound file
}

/* libFLAC works through callbacks, so we need a context struct. */

typedef struct {
	FILE *file;
	integer numberOfChannels;
	integer numberOfSamples;
	double *channels [FLAC__MAX_CHANNELS];
} MelderDecodeFlacContext;

/* The same goes for MP3 */

typedef struct {
	integer numberOfChannels;
	integer numberOfSamples;
	double *channels [2];
} MelderDecodeMp3Context;

static FLAC__StreamDecoderReadStatus Melder_DecodeFlac_read (const FLAC__StreamDecoder *decoder,
	FLAC__byte buffer [], size_t *bytes, void *client_data)
{
	MelderDecodeFlacContext *c = (MelderDecodeFlacContext *) client_data;
	(void) decoder;
	if (*bytes <= 0) 
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	*bytes = fread (buffer, sizeof (FLAC__byte), *bytes, c -> file);
	if (ferror (c -> file))
		return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	if (*bytes == 0)
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

static FLAC__StreamDecoderWriteStatus Melder_DecodeFlac_convert (const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	MelderDecodeFlacContext *c = (MelderDecodeFlacContext *) client_data;
	const FLAC__FrameHeader *header = & frame -> header;
	integer count = header -> blocksize;
	double multiplier;

	(void) decoder;

	switch (header -> bits_per_sample) {
		case 8: multiplier = (1.0f / 128); break;
		case 16: multiplier = (1.0f / 32768); break;
		case 24: multiplier = (1.0f / 8388608); break;
		case 32: multiplier = (1.0f / 32768 / 65536); break;
		default: return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	for (integer i = 0; i < c -> numberOfChannels; ++ i) {
		const FLAC__int32 *input = buffer [i];
		double *output = c -> channels [i];
		for (integer j = 0; j < count; ++ j)
			output [j] = ((integer) input [j]) * multiplier;
		c -> channels [i] += count;
	}
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void Melder_DecodeMp3_convert (const MP3F_SAMPLE *channels [MP3F_MAX_CHANNELS], integer count, void *context) {
	MelderDecodeMp3Context *c = (MelderDecodeMp3Context *) context;
	const MP3F_SAMPLE *input;
	double *output;
	for (integer i = 0; i < c -> numberOfChannels; ++ i) {
		input = channels [i];
		output = c -> channels [i];
		for (integer j = 0; j < count; ++ j)
			output [j] = mp3f_sample_to_float (input [j]);
		c -> channels [i] += count;
	}
}

static void Melder_DecodeFlac_error (const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
	(void) decoder;
	(void) client_data;
	Melder_warning (U"FLAC decoder error: ", Melder_peek8to32_u (FLAC__StreamDecoderErrorStatusString [status]));
}

static void Melder_readFlacFile (FILE *f, MAT buffer) {
	int result = 0;

	MelderDecodeFlacContext c;
	c.file = f;
	c.numberOfChannels = buffer.nrow;
	for (int ichan = 1; ichan <= buffer.nrow; ichan ++)
		c.channels [ichan - 1] = & buffer [ichan] [1];
	c.numberOfSamples = buffer.ncol;

	FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new ();
	if (! decoder) {
		Melder_appendError (U"Couldn’t create FLAC decoder.");
		goto end;
	}
	if (FLAC__stream_decoder_init_stream (decoder,
				Melder_DecodeFlac_read,
				nullptr, nullptr, nullptr, nullptr,
				Melder_DecodeFlac_convert, nullptr,
				Melder_DecodeFlac_error, & c) != FLAC__STREAM_DECODER_INIT_STATUS_OK)
	{
		Melder_appendError (U"Couldn’t initialize FLAC decoder.");
		goto end;
	}
	result = FLAC__stream_decoder_process_until_end_of_stream (decoder);
	FLAC__stream_decoder_finish (decoder);
end:
	if (decoder)
		FLAC__stream_decoder_delete (decoder);
	if (result == 0)
		Melder_throw (U"Error decoding FLAC file.");
}

static void Melder_readMp3File (FILE *f, MAT buffer) {
	int result = 0;
	MelderDecodeMp3Context c;
	c.numberOfChannels = buffer.nrow;
	for (int ichan = 1; ichan <= buffer.nrow; ichan ++)
		c.channels [ichan - 1] = & buffer [ichan] [1];
	c.numberOfSamples = buffer.ncol;
	MP3_FILE mp3f = mp3f_new ();
	mp3f_set_file (mp3f, f);
	mp3f_set_callback (mp3f, Melder_DecodeMp3_convert, & c);
	result = mp3f_read (mp3f, buffer.ncol);
	mp3f_delete (mp3f);
	if (result == 0)
		Melder_throw (U"Error decoding MP3 file.");
}

static int bitsInReadBuffer = 0;
static uint32 readOneBit (FILE *f) {
	static uint8 readBuffer;
	if (bitsInReadBuffer == 0) {
		int externalValue = fgetc (f);
		Melder_require (externalValue >= 0,
			U"Could not read a bit.");
		readBuffer = (uint8) externalValue;
		bitsInReadBuffer = 8;
	}
	return (readBuffer >> -- bitsInReadBuffer) & 1;
}

static uint32 bingetu32_shortened_direct (FILE *f, const uint32 mantissaLength) {
	//TRACE
	uint32 numberOfLeadingZeroes = 0;
	for (; readOneBit (f) == 0; numberOfLeadingZeroes ++)
		;
	uint32 result = numberOfLeadingZeroes * (1u << mantissaLength);
	for (uint32 i = 1; i <= mantissaLength; i ++)
		if (readOneBit (f) != 0)
			result += 1u << (mantissaLength - i);
	trace (numberOfLeadingZeroes, U" ", mantissaLength);
	return result;
}

static uint32 bingetu32_shortened_indirect (FILE *stream) {
	const uint32 mantissaLength = bingetu32_shortened_direct (stream, 2);
	return bingetu32_shortened_direct (stream, mantissaLength);
}

static int32 bingeti32_shortened_direct (FILE *f, const uint32 mantissaLength) {
	uint32 unsignedVersion = bingetu32_shortened_direct (f, mantissaLength + 1);   // a sign bit at the end
	uint32 signBit = unsignedVersion & 1u;
	uint32 absoluteValue = unsignedVersion >> 1;
	return signBit ? (int32) ~ absoluteValue: (int32) absoluteValue;   // two's complement
}

/*
	The table `alawBitshiftTable` was created by the following script (last checked 2024-12-03):

sortedAlawValues# = {
...  -32256, -31232, -30208, -29184, -28160, -27136, -26112, -25088,
...  -24064, -23040, -22016, -20992, -19968, -18944, -17920, -16896,
...  -16128, -15616, -15104, -14592, -14080, -13568, -13056, -12544,
...  -12032, -11520, -11008, -10496,  -9984,  -9472,  -8960,  -8448,
...   -8064,  -7808,  -7552,  -7296,  -7040,  -6784,  -6528,  -6272,
...   -6016,  -5760,  -5504,  -5248,  -4992,  -4736,  -4480,  -4224,
...   -4032,  -3904,  -3776,  -3648,  -3520,  -3392,  -3264,  -3136,
...   -3008,  -2880,  -2752,  -2624,  -2496,  -2368,  -2240,  -2112,
...   -2016,  -1952,  -1888,  -1824,  -1760,  -1696,  -1632,  -1568,
...   -1504,  -1440,  -1376,  -1312,  -1248,  -1184,  -1120,  -1056,
...   -1008,   -976,   -944,   -912,   -880,   -848,   -816,   -784,
...    -752,   -720,   -688,   -656,   -624,   -592,   -560,   -528,
...    -504,   -488,   -472,   -456,   -440,   -424,   -408,   -392,
...    -376,   -360,   -344,   -328,   -312,   -296,   -280,   -264,
...    -248,   -232,   -216,   -200,   -184,   -168,   -152,   -136,
...    -120,   -104,    -88,    -72,    -56,    -40,    -24,     -8,
...       8,     24,     40,     56,     72,     88,    104,    120,
...     136,    152,    168,    184,    200,    216,    232,    248,
...     264,    280,    296,    312,    328,    344,    360,    376,
...     392,    408,    424,    440,    456,    472,    488,    504,
...     528,    560,    592,    624,    656,    688,    720,    752,
...     784,    816,    848,    880,    912,    944,    976,   1008,
...    1056,   1120,   1184,   1248,   1312,   1376,   1440,   1504,
...    1568,   1632,   1696,   1760,   1824,   1888,   1952,   2016,
...    2112,   2240,   2368,   2496,   2624,   2752,   2880,   3008,
...    3136,   3264,   3392,   3520,   3648,   3776,   3904,   4032,
...    4224,   4480,   4736,   4992,   5248,   5504,   5760,   6016,
...    6272,   6528,   6784,   7040,   7296,   7552,   7808,   8064,
...    8448,   8960,   9472,   9984,  10496,  11008,  11520,  12032,
...   12544,  13056,  13568,  14080,  14592,  15104,  15616,  16128,
...   16896,  17920,  18944,  19968,  20992,  22016,  23040,  24064,
...   25088,  26112,  27136,  28160,  29184,  30208,  31232,  32256
...}

procedure convert: .bitshift, .value
	.value -= 2 ^ (.bitshift + 2)
	for .lookup to 256
		if sortedAlawValues# [.lookup] >= .value
			goto .found
		endif
	endfor
	label .found
	.result = .lookup
endproc
procedure convertAndCheck: .bitshift, .value, .target
	@convert: .bitshift, .value
	if sortedAlawValues# [convert.result] <> .target
		appendInfoLine: sortedAlawValues# [convert.result], " ", .target
	endif
endproc

#
# Tests of known conversions appearing in the Polyphone database.
#
@convertAndCheck: 2, 2*24, 40
@convertAndCheck: 2, 2*40, 72
@convertAndCheck: 5, 128, 8
@convertAndCheck: 5, 1408, 1312
@convertAndCheck: 5, 1152, 1056
@convertAndCheck: 5, -3456, -3520
@convertAndCheck: 5, -2431, -2496
@convertAndCheck: 5, -3968, -4032
@convertAndCheck: 5, -3712, -3776
@convertAndCheck: 7, 1536, 1056
@convertAndCheck: 7, -2560, -3008
@convertAndCheck: 7, -6656, -7040
@convertAndCheck: 7, -8704, -8960
@convertAndCheck: 7, -10752, -11008
@convertAndCheck: 7, -12800, -13056
@convertAndCheck: 7, -14848, -15104
@convertAndCheck: 8, -5120, -6016
@convertAndCheck: 8, -9216, -9984
@convertAndCheck: 9, -10240, -12032
@convertAndCheck: 10, -4096, -8064
@convertAndCheck: 11, -8192, -16128
@convertAndCheck: 12, -16384, -32256
@convertAndCheck: 12, 16384, 8

shifted# = zero# (256)
for bitshift from 1 to 12
	for i to 256
		shifted# [i] = -1
		linearValue = sortedAlawValues# [i]
		#
		# First approximation: bitshift the linear value.
		#
		shiftedLinearValue = linearValue * 2 ^ (bitshift - 1)
		#
		# We have to round to a linear value that corresponds to an actual A-law value.
		# The direction of rounding is down. We first go down by a full step.
		#
		shiftedLinearValue -= 2 ^ (bitshift + 2)
		#
		# We then go up until we pass the current value.
		#
		for lookup from 1 to 256
			if shiftedLinearValue <= sortedAlawValues# [lookup]
				shifted# [i] = lookup - 1
				if i > 1 and shifted# [i] = 0
					shifted# [i - 1] = -1   ; accept only one zero in each table
				endif
				goto found
			endif
		endfor
		label found
	endfor
	appendInfoLine: "Bitshift ", bitshift, ":"
	@writeTable: shifted#
endfor

procedure writeTable: .table#
	maximum = max (.table#)
	minimum = min (.table#)
	maximum$ = string$ (maximum)
	minimum$ = string$ (minimum)
	maximumTextLength = max (length (maximum$), length (minimum$))
	appendInfo: "{"
	for i to 256
		if i mod 16 = 1
			appendInfo: newline$, tab$
		endif
		appendInfo: padLeft$ (string$ (.table# [i]), maximumTextLength + 1)
		if i <> 256
			appendInfo: ","
		endif
		if i mod 4 = 0 and i mod 16 <> 0
			appendInfo: "  "
		endif
		if i mod 64 = 0
			appendInfo: newline$
		endif
	endfor
	appendInfoLine: "}"
endproc
*/
static int alawBitshiftTable [12] [256] = {
	{ // 1
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,
		  32,  33,  34,  35,    36,  37,  38,  39,    40,  41,  42,  43,    44,  45,  46,  47,
		  48,  49,  50,  51,    52,  53,  54,  55,    56,  57,  58,  59,    60,  61,  62,  63,

		  64,  65,  66,  67,    68,  69,  70,  71,    72,  73,  74,  75,    76,  77,  78,  79,
		  80,  81,  82,  83,    84,  85,  86,  87,    88,  89,  90,  91,    92,  93,  94,  95,
		  96,  97,  98,  99,   100, 101, 102, 103,   104, 105, 106, 107,   108, 109, 110, 111,
		 112, 113, 114, 115,   116, 117, 118, 119,   120, 121, 122, 123,   124, 125, 126, 127,

		 128, 129, 130, 131,   132, 133, 134, 135,   136, 137, 138, 139,   140, 141, 142, 143,
		 144, 145, 146, 147,   148, 149, 150, 151,   152, 153, 154, 155,   156, 157, 158, 159,
		 160, 161, 162, 163,   164, 165, 166, 167,   168, 169, 170, 171,   172, 173, 174, 175,
		 176, 177, 178, 179,   180, 181, 182, 183,   184, 185, 186, 187,   188, 189, 190, 191,

		 192, 193, 194, 195,   196, 197, 198, 199,   200, 201, 202, 203,   204, 205, 206, 207,
		 208, 209, 210, 211,   212, 213, 214, 215,   216, 217, 218, 219,   220, 221, 222, 223,
		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255
	},
	{ // 2
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,
		  32,  33,  34,  35,    36,  37,  38,  39,    40,  41,  42,  43,    44,  45,  46,  47,

		  48,  49,  50,  51,    52,  53,  54,  55,    56,  57,  58,  59,    60,  61,  62,  63,
		  64,  65,  66,  67,    68,  69,  70,  71,    72,  73,  74,  75,    76,  77,  78,  79,
		  80,  81,  82,  83,    84,  85,  86,  87,    88,  89,  90,  91,    92,  93,  94,  95,
		  96,  98, 100, 102,   104, 106, 108, 110,   112, 114, 116, 118,   120, 122, 124, 126,

		 128, 130, 132, 134,   136, 138, 140, 142,   144, 146, 148, 150,   152, 154, 156, 158,
		 160, 161, 162, 163,   164, 165, 166, 167,   168, 169, 170, 171,   172, 173, 174, 175,
		 176, 177, 178, 179,   180, 181, 182, 183,   184, 185, 186, 187,   188, 189, 190, 191,
		 192, 193, 194, 195,   196, 197, 198, 199,   200, 201, 202, 203,   204, 205, 206, 207,

		 208, 209, 210, 211,   212, 213, 214, 215,   216, 217, 218, 219,   220, 221, 222, 223,
		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 3
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,

		  32,  33,  34,  35,    36,  37,  38,  39,    40,  41,  42,  43,    44,  45,  46,  47,
		  48,  49,  50,  51,    52,  53,  54,  55,    56,  57,  58,  59,    60,  61,  62,  63,
		  64,  65,  66,  67,    68,  69,  70,  71,    72,  73,  74,  75,    76,  77,  78,  79,
		  80,  82,  84,  86,    88,  90,  92,  94,    96, 100, 104, 108,   112, 116, 120, 124,

		 128, 132, 136, 140,   144, 148, 152, 156,   160, 162, 164, 166,   168, 170, 172, 174,
		 176, 177, 178, 179,   180, 181, 182, 183,   184, 185, 186, 187,   188, 189, 190, 191,
		 192, 193, 194, 195,   196, 197, 198, 199,   200, 201, 202, 203,   204, 205, 206, 207,
		 208, 209, 210, 211,   212, 213, 214, 215,   216, 217, 218, 219,   220, 221, 222, 223,

		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 4
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,

		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,
		  32,  33,  34,  35,    36,  37,  38,  39,    40,  41,  42,  43,    44,  45,  46,  47,
		  48,  49,  50,  51,    52,  53,  54,  55,    56,  57,  58,  59,    60,  61,  62,  63,
		  64,  66,  68,  70,    72,  74,  76,  78,    80,  84,  88,  92,    96, 104, 112, 120,

		 128, 136, 144, 152,   160, 164, 168, 172,   176, 178, 180, 182,   184, 186, 188, 190,
		 192, 193, 194, 195,   196, 197, 198, 199,   200, 201, 202, 203,   204, 205, 206, 207,
		 208, 209, 210, 211,   212, 213, 214, 215,   216, 217, 218, 219,   220, 221, 222, 223,
		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,

		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 5
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,
		  32,  33,  34,  35,    36,  37,  38,  39,    40,  41,  42,  43,    44,  45,  46,  47,
		  48,  50,  52,  54,    56,  58,  60,  62,    64,  68,  72,  76,    80,  88,  96, 112,

		 128, 144, 160, 168,   176, 180, 184, 188,   192, 194, 196, 198,   200, 202, 204, 206,
		 208, 209, 210, 211,   212, 213, 214, 215,   216, 217, 218, 219,   220, 221, 222, 223,
		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 6
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  17,  18,  19,    20,  21,  22,  23,    24,  25,  26,  27,    28,  29,  30,  31,
		  32,  34,  36,  38,    40,  42,  44,  46,    48,  52,  56,  60,    64,  72,  80,  96,

		 128, 160, 176, 184,   192, 196, 200, 204,   208, 210, 212, 214,   216, 218, 220, 222,
		 224, 225, 226, 227,   228, 229, 230, 231,   232, 233, 234, 235,   236, 237, 238, 239,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 7
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   1,   2,   3,     4,   5,   6,   7,     8,   9,  10,  11,    12,  13,  14,  15,
		  16,  18,  20,  22,    24,  26,  28,  30,    32,  36,  40,  44,    48,  56,  64,  80,

		 128, 176, 192, 200,   208, 212, 216, 220,   224, 226, 228, 230,   232, 234, 236, 238,
		 240, 241, 242, 243,   244, 245, 246, 247,   248, 249, 250, 251,   252, 253, 254, 255,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 8
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		   0,   2,   4,   6,     8,  10,  12,  14,    16,  20,  24,  28,    32,  40,  48,  64,

		 128, 192, 208, 216,   224, 228, 232, 236,   240, 242, 244, 246,   248, 250, 252, 254,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 9
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,     0,   4,   8,  12,    16,  24,  32,  48,

		 128, 208, 224, 232,   240, 244, 248, 252,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 10
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,     0,   8,  16,  32,

		 128, 224, 240, 248,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 11
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,   0,  16,

		 128, 240,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	},
	{ // 12
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,   0,

		 128,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,

		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,
		  -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1,    -1,  -1,  -1,  -1
	}
};

static void Melder_readPolyphoneFile (MelderFile file, MAT buffer) {
	//TRACE
	FILE *f = file -> filePointer;
	Melder_require (buffer.nrow == 1,
		U"Can read Polyphone files only if they are mono. Write to paul.boersma@uva.nl for more information.");
	trace (U"expecting ", buffer.ncol, U" samples");
	const integer startOfData = ftell (f);
	fseek (f, 0, SEEK_END);
	const integer endOfData = ftell (f);
	const integer numberOfDataBytesInFile = endOfData - startOfData;
	trace (numberOfDataBytesInFile, U" data bytes in file");
	fseek (f, startOfData, SEEK_SET);
	/*
		The format of Shorten files is described in:
		Tony Robinson (1994): "Shorten: sample lossless and near-lossless waveform compression",
			Technical report CUED/F-INFENG/TR.156,
			Cambridge University Engineering Department.

		The first four bytes are lower-case letters, namelu "ajkg";
		"aj" seem to be Tony Robinson's initials.
	*/
	uint32 letter = bingetu8 (f);
	Melder_require (letter == 'a',
		U"First letter of magic word “ajkg” is character ", letter, U" instead of “a”.");
	letter = bingetu8 (f);
	Melder_require (letter == 'j',
		U"Second letter of magic word “ajkg” is character ", letter, U" instead of “j”.");
	letter = bingetu8 (f);
	Melder_require (letter == 'k',
		U"Third letter of magic word “ajkg” is character ", letter, U" instead of “k”.");
	letter = bingetu8 (f);
	Melder_require (letter == 'g',
		U"Fourth letter of magic word “ajkg” is character ", letter, U" instead of “g”.");
	const uint32 version = bingetu8 (f);
	Melder_require (version == 1,
		U"Can read only Shorten version 1, not ", version, U".");
	integer numberOfDataBytesLeft = numberOfDataBytesInFile - 5;
	trace (numberOfDataBytesLeft, U" data bytes left in file");
	/*
		The following six bytes are always 255 114 224 19 50 214.
		In binary:
			255 = 0b11111111
			114 = 0b01110010
			224 = 0b11100000
			 19 = 0b00010011
			 50 = 0b00110010
			214 = 0b11010110
		These contain several uint32 numbers in indirect shortened format.
		This indirected format first encodes the length of the mantissa
		of the uint32 number, in direct shortened format, with a mantissa of 2 bits.
		According to the Shorten paper, the number of fours is given by the number
		of leading zeroes, then there's a one to signal the end of the zeroes,
		and finally there are the two bits of the mantissa, as follows:
			100 -> 0
			101 -> 1
			110 -> 2
			111 -> 3
			0100 -> 4
			0101 -> 5
			0110 -> 6
			0111 -> 7
			00100 -> 8
			00101 -> 9
			00110 -> 10
			00111 -> 11
			000100 -> 12
			000101 -> 13
			000110 -> 14
			000111 -> 15
			0000100 -> 16
			00000100 -> 20
			000000100 -> 24
			0000000100 -> 28
		Now we have just only implemented the number of bits of the mantissa
		of the whole number. Let's walk through the fixed bits.

		The first mantissa length is encoded as 111 (the first 1+2 bits of 255
		that follow the 0 zeroes), meaning 3. So the whole number is encoded
		with a mantissa length of 3. The table for that is:
			1000 -> 0
			1001 -> 1
			1111 -> 7
			01000 -> 8
		After the first 111 of 255 we find 0 zeroes, follow by 1+3 ones,
		which means 7. This whole number has been encoded as 111.1111.
	*/
	bitsInReadBuffer = 0;   // flush
	const uint32 fileType = bingetu32_shortened_indirect (f);
	trace (U"File type: ", fileType);
	Melder_require (fileType == 7,
		U"Polyphone files should have file type 7, not ", fileType, U".");
	/*
		The remainder of the bits is now found after the pipe:
			111.1111|10111001011100000000100110011001011010110
			       7
		The next mantissa length code is 101, i.e. 1. So we need the following table:
			10 -> 0
			11 -> 1
			010 -> 2
			011 -> 3
			0010 -> 4
			0011 -> 5
			00010 -> 6
		What we find after 101 is 11, meaning 1.
	*/
	const uint32 numberOfChannels = bingetu32_shortened_indirect (f);
	trace (U"Number of channels: ", numberOfChannels);
	Melder_require (numberOfChannels == 1,
		U"Polyphone files should have 1 channel (mono), not ", numberOfChannels);
	/*
		The remainder of the bits is now:
			111.1111|101.11|001011100000000100110011001011010110
			       7      1
		The next mantissa length code is 00101, i.e. 9. So we need the following table:
			1000000000 -> 0
			1000000001 -> 1
			1100000000 -> 256
			1111111111 -> 511
		What we find after 00101 is 1100000000, meaning 256.
	*/
	uint32 blockSize = bingetu32_shortened_indirect (f);   // the standard block size, until it's changed
	trace (U"Block size: ", blockSize);
	Melder_require (blockSize == 256,
		U"Polyphone files should have a block size of 256, not ", blockSize, U".");
	/*
		The remainder of the bits is now:
			111.1111|101.11|00101.1100000000|100110011001011010110
			       7      1              256
		The next mantissa length code is 100, i.e. 0. So we need the following table:
			1 -> 0
			01 -> 1
			001 -> 2
			0001 -> 3
		What we find after 100 is 1, meaning 0.
	*/
	const uint32 numberOfLpcCoefficients = bingetu32_shortened_indirect (f);
	trace (U"Number of LPC coefficients: ", numberOfLpcCoefficients);
	Melder_require (numberOfLpcCoefficients == 0,
		U"Polyphone files should not have LPC encoding.");
	/*
		The remainder of the bits is now:
			111.1111|101.11|00101.1100000000|100.1|10011001011010110
			       7      1              256     0
		The next mantissa length code is again 100, i.e. 0.
		What we find after 100 is 1, meaning 0 again.
		This is only one of two zeroes:
	*/
	const uint32 dummy1 = bingetu32_shortened_indirect (f);
	trace (U"dummy1: ", dummy1);
	Melder_require (dummy1 == 0,
		U"Polyphone files should have a “dummy1” of 0.");
	const uint32 dummy2 = bingetu32_shortened_indirect (f);
	trace (U"dummy2: ", dummy2);
	Melder_require (dummy2 == 0,
		U"Polyphone files should have a “dummy2” of 0.");
	/*
		The remainder of the bits is now:
			111.1111|101.11|00101.1100000000|100.1|100.1|100.1|01101011011
			       7      1              256     0     0     0
		Note that we already appended the high two bits of the number,
		which is always over 192.
		The next mantissa length code is 0110, i.e. 6.
		What we find after 0110 is 1011011, meaning 27.
		So we have found:
			111.1111|101.11|00101.1100000000|100.1|100.1|100.1|0110.1011011|
			       7      1              256     0     0     0           27
		Great. We stuffed 7 numbers into 6 bytes plus 2 bits,
		instead of wasting 28 (or perhaps 14) bytes on it.

		But... what is this 27?
		We are supposed to see "commands", i.e. numbers between 0 and 8,
		encoded as direct shortened numbers with a length-2 mantissa.
		Thus, 0110 would actually represent the number 6, which means "bitshift",
		which should be followed by the bitshift itself, which is also
		mantissa-2-encoded, so the bitshift is 101, which means 1:
			111.1111|101.11|00101.1100000000|100.1|100.1|100.1|0110|101|1011
			       7      1              256     0     0     0    6   1
		The next command then seems to be 101, which means 1 ("diff1"):
			111.1111|101.11|00101.1100000000|100.1|100.1|100.1|0110|101|101|1
			       7      1              256     0     0     0    6   1   1
	*/
	autovector <int32> sampleBuffer = newvectorzero <int32> (blockSize + 3);
	constexpr uint32 COMMAND_DIFF0 = 0, COMMAND_DIFF1 = 1, COMMAND_DIFF2 = 2, COMMAND_DIFF3 = 3;
	constexpr uint32 COMMAND_QUIT = 4, COMMAND_BLOCKSIZE = 5, COMMAND_BITSHIFT = 6;
	integer numberOfSamplesRead = 0;
	uint32 bitshift = 0;
	for (;;) {
		uint32 command = bingetu32_shortened_direct (f, 2);
		trace (U"Command ", command);
		if (command == COMMAND_QUIT)
			break;
		if (command == COMMAND_BITSHIFT) {
			bitshift = bingetu32_shortened_direct (f, 2);
			trace (U"bitshift ", bitshift);
			Melder_require (bitshift <= 12,
				U"Can handle only a bitshift up to 12, not ", bitshift, U".");
		} else if (command >= COMMAND_DIFF0 && command <= COMMAND_DIFF3) {
			uint32 mantissaLength = bingetu32_shortened_direct (f, 3);
			trace (U"diff", command, U" length ", mantissaLength);
			for (uint32 i = 4; i <= blockSize + 3; i ++) {
				const int32 storedValue = bingeti32_shortened_direct (f, mantissaLength);
				if (command == COMMAND_DIFF0)
					sampleBuffer [i] = storedValue;   // Robinson (1994), eq. 3
				else if (command == COMMAND_DIFF1)
					sampleBuffer [i] = storedValue
							+ sampleBuffer [i - 1];   // Robinson (1994), eq. 4
				else if (command == COMMAND_DIFF2)
					sampleBuffer [i] = storedValue
							+ 2 * sampleBuffer [i - 1] - sampleBuffer [i - 2];   // Robinson (1994), eq. 5
				else if (command == COMMAND_DIFF3)
					sampleBuffer [i] = storedValue
							+ 3 * (sampleBuffer [i - 1] - sampleBuffer [i - 2]) + sampleBuffer [i - 3];   // Robinson (1994), eq. 6
				trace (storedValue, U" ", sampleBuffer [i]);
			}
			for (uint32 i = 1; i <= 3; i ++)
				sampleBuffer [i] = sampleBuffer [blockSize + i];   // move them here, waiting for the next block
			Melder_require (bitshift > 0,
				U"Cannot work with a bitshift of 0.");
			Melder_require (bitshift <= 12,
				U"Can handle only a bitshift up to 12, not ", bitshift, U".");
			for (uint32 i = 4; i <= blockSize + 3; i ++) {
				const int32 alawValue = sampleBuffer [i];
				Melder_require (alawValue >= -128,
					U"A-law sample value is ", alawValue, U" but should be at least -128.");
				Melder_require (alawValue <= +127,
					U"A-law sample value is ", alawValue, U" but should be at most +127.");
				const integer placeInTable = alawBitshiftTable [bitshift - 1] [alawValue + 128];
				Melder_require (placeInTable >= 0,   // because minus 1 means undefined
					U"The A-law value ", alawValue, U" is too large for a bitshift of ", bitshift, U".");
				const int32 linearValue = sortedAlawValues [placeInTable];
				buffer [1] [++ numberOfSamplesRead] = linearValue / 32768.0;
			}
		} else if (command == COMMAND_BLOCKSIZE) {
			/*
				This might often occur before the last block,
				namely if the number of samples is not an integer multiple
				of the earlier block sizes (which are typically 256).
			*/
			const uint32 newBlockSize = bingetu32_shortened_indirect (f);
			Melder_require (newBlockSize <= blockSize,
				U"Cannot grow block size from ", blockSize, U" to ", newBlockSize, U".");
			trace (newBlockSize);
			blockSize = newBlockSize;
		} else {
			Melder_throw (U"Unknown command ", command);
		}
	}
	Melder_require (numberOfSamplesRead == buffer.ncol,
		U"Expected ", buffer.ncol, U" samples but found ", numberOfSamplesRead, U".");
}

static void warning_fileTooSmall (MelderFile file, integer numberOfChannels, const char32 *type) {
	Melder_warning (U"File ", MelderFile_messageName (file), U" too small (",
			numberOfChannels, U"-channel ", type, U").\nMissing samples were set to zero.");
}

void Melder_readAudioToFloat (MelderFile file, int encoding, MAT buffer) {
	FILE *f = file -> filePointer;   // this function must have a MelderFile argument, because it can warn (which needs a filename)
	try {
		integer numberOfChannels = buffer.nrow;
		integer numberOfSamples = buffer.ncol;
		switch (encoding) {
			case Melder_LINEAR_8_SIGNED: {
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
							int8 value;
							if (fread (& value, 1, 1, f) < 1) throw MelderError ();
							buffer [ichan] [isamp] = value * (1.0 / 128);
						}
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"8-bit signed");
				}
			} break;
			case Melder_LINEAR_8_UNSIGNED:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = bingetu8 (f) * (1.0 / 128) - 1.0;
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"8-bit unsigned");
				}
				break;
			case Melder_LINEAR_16_BIG_ENDIAN:
			case Melder_LINEAR_16_LITTLE_ENDIAN:
			case Melder_LINEAR_24_BIG_ENDIAN:
			case Melder_LINEAR_24_LITTLE_ENDIAN:
			case Melder_LINEAR_32_BIG_ENDIAN:
			case Melder_LINEAR_32_LITTLE_ENDIAN:
			{
				const int numberOfBytesPerSamplePerChannel =
					encoding == Melder_LINEAR_16_BIG_ENDIAN || encoding == Melder_LINEAR_16_LITTLE_ENDIAN ? 2 :
					encoding == Melder_LINEAR_24_BIG_ENDIAN || encoding == Melder_LINEAR_24_LITTLE_ENDIAN ? 3 : 4;
				const double numberOfBytes_f = (double) numberOfChannels * (double) numberOfSamples * (double) numberOfBytesPerSamplePerChannel;
				if (isinf (numberOfBytes_f) || numberOfBytes_f > (double) (1LL << 53)) {
					Melder_throw (U"Cannot read ", numberOfBytes_f, U" bytes, "
						U"because that crosses the 9-petabyte limit.");
				}
				if (numberOfBytes_f > (double) SIZE_MAX) {
					Melder_throw (U"Cannot read ", numberOfBytes_f, U" bytes. "
						U"Perhaps try a 64-bit edition of Praat?");
				}
				Melder_assert (numberOfBytes_f >= 0.0);
				const size_t numberOfBytes = (size_t) numberOfBytes_f;   // cast is safe because overflow and signedness have been checked
				/* mutable read */ uint8 * /* mutable cycle */ bytes =
						(uint8 *) & buffer [numberOfChannels] [numberOfSamples] + sizeof (double) - numberOfBytes;
				/*
					Read 16-bit data into the last quarter of the buffer,
					or 24-bit data into the last three-eighths of the buffer,
					or 32-bit data into the last half of the buffer.
				*/
				const size_t numberOfBytesRead = fread (bytes, 1, numberOfBytes, f);
				if (numberOfChannels == 1) {
					switch (encoding) {
						case Melder_LINEAR_16_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++;
								const int value = (int) (int16) (((uint16) byte1 << 8) | (uint16) byte2);   // extend sign
								buffer [1] [isamp] = value * (1.0 / 32768);
							}
						} break;
						case Melder_LINEAR_16_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++;
								const int value = (int) (int16) (((uint16) byte2 << 8) | (uint16) byte1);   // extend sign
								buffer [1] [isamp] = value * (1.0 / 32768);
							}
						} break;
						case Melder_LINEAR_24_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
								const int32 value = (int32)
									((uint32) ((uint32) byte1 << 24) |
									 (uint32) ((uint32) byte2 << 16) |
											  ((uint32) byte3 << 8));
								buffer [1] [isamp] = value * (1.0 / 32768 / 65536);
							}
						} break;
						case Melder_LINEAR_24_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
								const int32 value = (int32)
									((uint32) ((uint32) byte3 << 24) |
									 (uint32) ((uint32) byte2 << 16) |
											  ((uint32) byte1 << 8));
								buffer [1] [isamp] = value * (1.0 / 32768 / 65536);
							}
						} break;
						case Melder_LINEAR_32_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
								const int32 value = (int32)
									((uint32) ((uint32) byte1 << 24) |
									 (uint32) ((uint32) byte2 << 16) |
									 (uint32) ((uint32) byte3 << 8) |
											   (uint32) byte4);
								buffer [1] [isamp] = value * (1.0 / 32768 / 65536);
							}
						} break;
						case Melder_LINEAR_32_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
								const unsigned char byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
								const int32 value = (int32)
									((uint32) ((uint32) byte4 << 24) |
									 (uint32) ((uint32) byte3 << 16) |
									 (uint32) ((uint32) byte2 << 8) |
											   (uint32) byte1);
								buffer [1] [isamp] = value * (1.0 / 32768 / 65536);
							}
						} break;
					}
				} else if (numberOfChannels <= (int) sizeof (double) / numberOfBytesPerSamplePerChannel) {
					switch (encoding) {
						case Melder_LINEAR_16_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++;
									const int value = (int) (int16) (uint16) ((uint16) ((uint16) byte1 << 8) | (uint16) byte2);
									buffer [ichan] [isamp] = value * (1.0 / 32768);
								}
						} break;
						case Melder_LINEAR_16_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++;
									const int value = (int) (int16) (uint16) ((uint16) ((uint16) byte2 << 8) | (uint16) byte1);
									buffer [ichan] [isamp] = value * (1.0 / 32768);
								}
						} break;
						case Melder_LINEAR_24_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
									/* mutable sign */ uint32 unsignedValue =
										(uint32) ((uint32) byte1 << 16) |
										(uint32) ((uint32) byte2 << 8) |
												  (uint32) byte3;
									if ((byte1 & 128) != 0)
										unsignedValue |= 0xFF000000;   // extend sign
									buffer [ichan] [isamp] = (int32) unsignedValue * (1.0 / 8388608);
								}
						} break;
						case Melder_LINEAR_24_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
									/* mutable sign */ uint32 unsignedValue = ((uint32) byte3 << 16) | ((uint32) byte2 << 8) | (uint32) byte1;
									if ((byte3 & 128) != 0)
										unsignedValue |= 0xFF000000;
									buffer [ichan] [isamp] = (int32) unsignedValue * (1.0 / 8388608);
								}
						} break;
						case Melder_LINEAR_32_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
									const int32 value = (int32)
										((uint32) ((uint32) byte1 << 24) |
										 (uint32) ((uint32) byte2 << 16) |
										 (uint32) ((uint32) byte3 << 8) |
												   (uint32) byte4);
									buffer [ichan] [isamp] = value * (1.0 / 32768 / 65536);
								}
						} break;
						case Melder_LINEAR_32_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
									const int32 value = (int32)
										((uint32) ((uint32) byte4 << 24) |
										 (uint32) ((uint32) byte3 << 16) |
										 (uint32) ((uint32) byte2 << 8) |
												   (uint32) byte1);
									buffer [ichan] [isamp] = value * (1.0 / 32768 / 65536);
								}
						} break;
					}
				} else {
					Melder_assert (sizeof (double) == 8);
					int32 *ints = (int32 *) & buffer [1] [1];
					switch (encoding) {
						case Melder_LINEAR_16_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++;
									const int32 value = (int32)
										((uint32) ((uint32) byte1 << 24) |
										 (uint32) ((uint32) byte2 << 16));
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						} break;
						case Melder_LINEAR_16_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++;
									const int32 value = (int32)
										(((uint32) byte2 << 24) |
										 ((uint32) byte1 << 16));
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						} break;
						case Melder_LINEAR_24_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
									const int32 value = (int32)
										(((uint32) byte1 << 24) |
										 ((uint32) byte2 << 16) |
										 ((uint32) byte3 << 8));
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						} break;
						case Melder_LINEAR_24_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++;
									const int32 value = (int32)
										(((uint32) byte3 << 24) |
										 ((uint32) byte2 << 16) |
										 ((uint32) byte1 << 8));
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						} break;
						case Melder_LINEAR_32_BIG_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
									const int32 value = (int32)
										(((uint32) byte1 << 24) |
										 ((uint32) byte2 << 16) |
										 ((uint32) byte3 << 8) |
										  (uint32) byte4);
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						} break;
						case Melder_LINEAR_32_LITTLE_ENDIAN: {
							for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
								for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
									const uint8 byte1 = * bytes ++, byte2 = * bytes ++, byte3 = * bytes ++, byte4 = * bytes ++;
									const int32 value = (int32)
										(((uint32) byte4 << 24) |
										 ((uint32) byte3 << 16) |
										 ((uint32) byte2 << 8) |
										  (uint32) byte1);
									* ints ++ = value;
									* ints ++ = 0;   // the marker
								}
						}
					}
					double *doubles = & buffer [1] [1];
					integer n = numberOfSamples * numberOfChannels;
					for (integer i = 0; i < n; i ++) {
						int32 *valuePosition = (int32 *) & doubles [i];
						int32 *markerPosition = valuePosition + 1;
						if (! *markerPosition) {
							int32 value = *valuePosition;
							integer ichan = i / numberOfSamples, isamp = i % numberOfSamples;
							for (integer other = isamp * numberOfChannels + ichan; other != i; ) {
								int32 *otherValuePosition = (int32 *) & doubles [other];
								*valuePosition = *otherValuePosition;
								*markerPosition = 1;
								valuePosition = otherValuePosition;
								markerPosition = valuePosition + 1;
								ichan = other / numberOfSamples, isamp = other % numberOfSamples;
								other = isamp * numberOfChannels + ichan;
							}
							*valuePosition = value;
							*markerPosition = 1;
						}
					}
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = * (int32 *) & buffer [ichan] [isamp] * (1.0 / 32768 / 65536);
				}
				if (numberOfBytesRead < numberOfBytes)
					Melder_warning (U"File too small (", numberOfChannels, U"-channel ", numberOfBytesPerSamplePerChannel * 8, U"-bit).\n"
							U"Missing samples were set to zero.");
			} break;
			case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = bingetr32 (f);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"32-bit floating point");
				}
				break;
			case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = bingetr32LE (f);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"32-bit floating point");
				}
				break;
			case Melder_IEEE_FLOAT_64_BIG_ENDIAN:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = bingetr64 (f);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"64-bit floating point");
				}
				break;
			case Melder_IEEE_FLOAT_64_LITTLE_ENDIAN:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = bingetr64LE (f);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"64-bit floating point");
				}
				break;
			case Melder_MULAW:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = ulaw2linear [bingetu8 (f)] * (1.0 / 32768);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"8-bit μ-law");
				}
				break;
			case Melder_ALAW:
				try {
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
						for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
							buffer [ichan] [isamp] = alaw2linear [bingetu8 (f)] * (1.0 / 32768);
				} catch (MelderError) {
					Melder_clearError ();
					warning_fileTooSmall (file, numberOfChannels, U"8-bit A-law");
				}
				break;
			case Melder_FLAC_COMPRESSION_16:
			case Melder_FLAC_COMPRESSION_24:
			case Melder_FLAC_COMPRESSION_32:
				Melder_readFlacFile (f, buffer);
				break;
			case Melder_MPEG_COMPRESSION_16:
			case Melder_MPEG_COMPRESSION_24:
			case Melder_MPEG_COMPRESSION_32:
				Melder_readMp3File (f, buffer);
				break;
			case Melder_POLYPHONE:
				Melder_readPolyphoneFile (file, buffer);
				break;
			default:
				Melder_throw (U"Unknown encoding ", encoding, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"Audio samples not read from file.");
	}
}

void Melder_readAudioToShort (MelderFile file, integer numberOfChannels, int encoding, short *buffer, integer numberOfSamples) {
	FILE *f = file -> filePointer;   // this function must have a MelderFile argument, because it can warn (which needs a filename)
	try {
		const integer n = numberOfSamples * numberOfChannels;
		static const unsigned short byteSwapTest = 3 * 256 + 1;
		switch (encoding) {
			case Melder_LINEAR_8_SIGNED:
				for (integer i = 0; i < n; i ++) {
					signed char value;
					if (! fread (& value, 1, 1, f))
						Melder_throw (U"File too small (mono 8-bit).");
					buffer [i] = value * 256;
				}
				break;
			case Melder_LINEAR_8_UNSIGNED:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingetu8 (f) * 256L - 32768;
				break;
			case Melder_LINEAR_16_BIG_ENDIAN:
				static_assert (sizeof (short) == 2);
				fread (buffer, sizeof (short), n, f);
				if (* (uint8 *) & byteSwapTest == 1) {
					for (integer i = 0; i < n; i ++) {
						const uint16 value = (uint16) buffer [i];   // cast sign
						buffer [i] = (int16) ((value >> 8) + (value << 8));   // truncate from int32 (left shifts are signed) to int16
					}
				}
				break;
			case Melder_LINEAR_16_LITTLE_ENDIAN:
				fread (buffer, sizeof (short), n, f);
				if (* (unsigned char *) & byteSwapTest == 3) {
					for (integer i = 0; i < n; i ++) {
						const uint16 value = (uint16) buffer [i];   // cast sign
						buffer [i] = (int16) ((value >> 8) + (value << 8));   // truncate from int32 (left shifts are signed) to int16
					}
				}
				break;
			case Melder_LINEAR_24_BIG_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingeti24 (f) / 256;   // BUG: truncation; not ideal
				break;
			case Melder_LINEAR_24_LITTLE_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingeti24LE (f) / 256;   // BUG: truncation; not ideal
				break;
			case Melder_LINEAR_32_BIG_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingeti32 (f) / 65536;   // BUG: truncation; not ideal
				break;
			case Melder_LINEAR_32_LITTLE_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingeti32LE (f) / 65536;   // BUG: truncation; not ideal
				break;
			case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingetr32 (f) * 32768;   // BUG: truncation; not ideal
				break;
			case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingetr32LE (f) * 32768;   // BUG: truncation; not ideal
				break;
			case Melder_IEEE_FLOAT_64_BIG_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingetr64 (f) * 32768;   // BUG: truncation; not ideal
				break;
			case Melder_IEEE_FLOAT_64_LITTLE_ENDIAN:
				for (integer i = 0; i < n; i ++)
					buffer [i] = bingetr64LE (f) * 32768;   // BUG: truncation; not ideal
				break;
			case Melder_MULAW:
				for (integer i = 0; i < n; i ++)
					buffer [i] = ulaw2linear [bingetu8 (f)];
				break;
			case Melder_ALAW:
				for (integer i = 0; i < n; i ++)
					buffer [i] = alaw2linear [bingetu8 (f)];
				break;
			default:
				Melder_throw (U"Unknown encoding ", encoding, U".");
		}
	} catch (MelderError) {
		Melder_clearError ();
		Melder_warning (U"Audio file ", MelderFile_messageName (file), U" too short. Missing samples were set to zero.");
	}
}

void MelderFile_writeShortToAudio (MelderFile file, integer numberOfChannels, int encoding, const short *buffer, integer numberOfSamples) {
	try {
		FILE *f = file -> filePointer;
		if (! f)
			Melder_throw (U"File ", MelderFile_messageName (file), U" is not open.");
		integer n = numberOfSamples * numberOfChannels, start = 0, step = 1;
		if (numberOfChannels < 0) {
			n = numberOfSamples * 2;   // stereo
			step = 2;   // only one channel will be written
			if (numberOfChannels == -2)
				start = 1;   // right channel
			numberOfChannels = 1;
		}
		switch (encoding) {
			case Melder_LINEAR_8_SIGNED:
				for (integer i = start; i < n; i += step)
					binputi8 (buffer [i] >> 8, f);
			break; case Melder_LINEAR_8_UNSIGNED:
				for (integer i = start; i < n; i += step)
					binputu8 ((buffer [i] >> 8) + 128, f);
			break; case Melder_LINEAR_16_BIG_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi16 (buffer [i], f);
			break; case Melder_LINEAR_16_LITTLE_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi16LE (buffer [i], f);
			break; case Melder_LINEAR_24_BIG_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi24 (buffer [i] << 8, f);
			break; case Melder_LINEAR_24_LITTLE_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi24LE (buffer [i] << 8, f);
			break; case Melder_LINEAR_32_BIG_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi32 (buffer [i] << 16, f);
			break; case Melder_LINEAR_32_LITTLE_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputi32LE (buffer [i] << 16, f);
			break; case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputr32 (buffer [i] / 32768.0, f);
			break; case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputr32LE (buffer [i] / 32768.0, f);
			break; case Melder_IEEE_FLOAT_64_BIG_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputr64 (buffer [i] / 32768.0, f);
			break; case Melder_IEEE_FLOAT_64_LITTLE_ENDIAN:
				for (integer i = start; i < n; i += step)
					binputr64LE (buffer [i] / 32768.0, f);
			break;
			case Melder_FLAC_COMPRESSION_16:
			case Melder_FLAC_COMPRESSION_24:
			case Melder_FLAC_COMPRESSION_32:
				if (! file -> flacEncoder)
					Melder_throw (U"FLAC encoder not initialized.");
				for (integer i = start; i < n; i += step * numberOfChannels) {
					FLAC__int32 samples [FLAC__MAX_CHANNELS];
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++)
						samples [ichan - 1] = buffer [i + ichan - 1];
					if (! FLAC__stream_encoder_process_interleaved (file -> flacEncoder, samples, 1))
						Melder_throw (U"Error encoding FLAC stream.");
				}
			break; case Melder_MULAW: case Melder_ALAW: default:
				Melder_throw (U"Unknown encoding ", encoding, U".");
		}
	} catch (MelderError) {
		Melder_throw (U"Samples not written to audio file.");
	}
}

void MelderFile_writeFloatToAudio (MelderFile file, constMATVU const& buffer, int encoding, bool warnIfClipped) {
	try {
		FILE *f = file -> filePointer;
		if (! f)
			Melder_throw (U"File ", MelderFile_messageName (file), U" is not open.");
		const integer numberOfSamples = buffer.ncol, numberOfChannels = buffer.nrow;
		integer nclipped = 0;
		switch (encoding) {
			case Melder_LINEAR_8_SIGNED:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 128.0);
						if (value < -128.0) { value = -128.0; nclipped ++; }
						if (value > 127.0) { value = 127.0; nclipped ++; }
						binputi8 ((int) value, f);
					}
				break;
			case Melder_LINEAR_8_UNSIGNED:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = floor ((buffer [ichan] [isamp] + 1.0) * 128.0);
						if (value < 0.0) { value = 0.0; nclipped ++; }
						if (value > 255.0) { value = 255.0; nclipped ++; }
						binputu8 ((int) value, f);
					}
				break;
			case Melder_LINEAR_16_BIG_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 32768.0);
						if (value < -32768.0) { value = -32768.0; nclipped ++; }
						if (value > 32767.0) { value = 32767.0; nclipped ++; }
						binputi16 ((int16) value, f);
					}
				break;
			case Melder_LINEAR_16_LITTLE_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 32768.0);
						if (value < -32768.0) { value = -32768.0; nclipped ++; }
						if (value > 32767.0) { value = 32767.0; nclipped ++; }
						binputi16LE ((int16) value, f);
					}
				break;
			case Melder_LINEAR_24_BIG_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 8388608.0);
						if (value < -8388608.0) { value = -8388608.0; nclipped ++; }
						if (value > 8388607.0) { value = 8388607.0; nclipped ++; }
						binputi24 ((int32) value, f);
					}
				break;
			case Melder_LINEAR_24_LITTLE_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 8388608.0);
						if (value < -8388608.0) { value = -8388608.0; nclipped ++; }
						if (value > 8388607.0) { value = 8388607.0; nclipped ++; }
						binputi24LE ((int32) value, f);
					}
				break;
			case Melder_LINEAR_32_BIG_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 2147483648.0);
						if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
						if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
						binputi32 ((int32) value, f);   // safe cast: rounding and range already handled
					}
				break;
			case Melder_LINEAR_32_LITTLE_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 2147483648.0);
						if (value < -2147483648.0) { value = -2147483648.0; nclipped ++; }
						if (value > 2147483647.0) { value = 2147483647.0; nclipped ++; }
						binputi32LE ((int32) value, f);
					}
				break;
			case Melder_IEEE_FLOAT_32_BIG_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						const double value = buffer [ichan] [isamp];
						binputr32 (value, f);
					}
				break;
			case Melder_IEEE_FLOAT_32_LITTLE_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						const double value = buffer [ichan] [isamp];
						binputr32LE (value, f);
					}
				break;
			case Melder_IEEE_FLOAT_64_BIG_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						const double value = buffer [ichan] [isamp];
						binputr64 (value, f);
					}
				break;
			case Melder_IEEE_FLOAT_64_LITTLE_ENDIAN:
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++)
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						const double value = buffer [ichan] [isamp];
						binputr64LE (value, f);
					}
				break;
			case Melder_FLAC_COMPRESSION_16:
			case Melder_FLAC_COMPRESSION_24:
			case Melder_FLAC_COMPRESSION_32:
				if (! file -> flacEncoder)
					Melder_throw (U"FLAC encoder not initialized.");
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
					FLAC__int32 samples [FLAC__MAX_CHANNELS];
					for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
						/* mutable clip */ double value = round (buffer [ichan] [isamp] * 32768.0);
						if (value < -32768.0) { value = -32768.0; nclipped ++; }
						if (value > 32767.0) { value = 32767.0; nclipped ++; }
						samples [ichan - 1] = (FLAC__int32) value;
					}
					if (! FLAC__stream_encoder_process_interleaved (file -> flacEncoder, samples, 1))
						Melder_throw (U"Error encoding FLAC stream.");
				}
				break;
			case Melder_MULAW:
			case Melder_ALAW:
			default:
				Melder_throw (U"Unknown format.");
		}
		if (nclipped > 0 && warnIfClipped)
			Melder_warning (U"Writing samples to audio file ", MelderFile_messageName (file), U": ",
				nclipped, U" out of ", numberOfSamples, U" samples have been clipped.\n"
				U"Advice: you could scale the amplitudes or write to a binary file."
			);
	} catch (MelderError) {
		Melder_throw (U"Samples not written to audio file.");
	}
}

void Melder_audiofiles_init () {
}

/* End of file melder_audiofiles.cpp */
