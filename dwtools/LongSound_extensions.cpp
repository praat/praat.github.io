/* LongSound_extensions.c
 *
 * Copyright (C) 1993-2021,2024,2026 David Weenink
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

/*
 djmw 20020627 GPL header
 djmw 20030913 changed 'f' to 'file' as argument in Melder_checkSoundFile
 djmw 20060206 Set errno = 0:  "An application that needs to examine the value
 	of errno to determine the error should set it to 0 before a function call,
 	then inspect it before a subsequent function call."
 djmw 20061213 MelderFile_truncate also works for MacOS X
 djmw 20061212 Header unistd.h for MacOS X added.
 djmw 20070129 Sounds may be multichannel
 djmw 20071030 MelderFile->wpath to  MelderFile->path
*/

#include "LongSound_extensions.h"

#if defined (_WIN32)
#include "winport_on.h"
#include <windows.h>
#include "winport_off.h"
#elif defined(linux)
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#elif defined (macintosh)
#include <unistd.h>
#endif

#include "NUM2.h"
#include <errno.h>

/*
  Precondition: size (my buffer) >= nbuf
*/
static void _LongSound_to_multichannel_buffer (LongSound me, short *buffer, integer nbuf, int nchannels, int ichannel, integer ibuf) {
	const integer numberOfReads = (my nx - 1) / nbuf + 1;
	integer n_to_read = 0;

	if (ibuf <= numberOfReads) {
		n_to_read = ( ibuf == numberOfReads ? (my nx - 1) % nbuf + 1 : nbuf );
		const integer imin = (ibuf - 1) * nbuf + 1;
		my invalidateBuffer();
		LongSound_readAudioToShort (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), imin, n_to_read);

		for (integer i = 1; i <= n_to_read; i ++)
			buffer [nchannels * (i - 1) + ichannel] = my buffer [i];
	}
	if (ibuf >= numberOfReads)
		for (integer i = n_to_read + 1; i <= nbuf; i ++) 
			buffer [nchannels * (i - 1) + ichannel] = 0;
}

void LongSounds_writeToStereoAudioFile16 (LongSound me, LongSound thee, int audioFileType, MelderFile file) {
	try {
		const integer nbuf = std::min (my nmax, thy nmax);
		const integer nx = std::max (my nx, thy nx);
		const integer numberOfReads = (nx - 1) / nbuf + 1, numberOfBitsPerSamplePoint = 16;
		
		Melder_require (thy numberOfChannels == my numberOfChannels && my numberOfChannels == 1,
			U"The two LongSounds should be mono.");
		Melder_require (my sampleRate == thy sampleRate,
			U"The two sampling frequencies should be equal.");
		/*
			Allocate a stereo buffer of size (2 * the smallest)!
			WE SUPPOSE THAT SMALL IS LARGE ENOUGH.
			Read the same number of samples from both files, despite
			potential differences in internal buffer size.
		*/

		const integer nchannels = 2;
		autovector <short> buffer = newvectorzero <short> (nchannels * nbuf);

		autoMelderFile f = MelderFile_create (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, Melder_ifloor (my sampleRate), nx, nchannels, numberOfBitsPerSamplePoint);

		for (integer i = 1; i <= numberOfReads; i ++) {
			const integer n_to_write = ( i == numberOfReads ? (nx - 1) % nbuf + 1 : nbuf );
			_LongSound_to_multichannel_buffer (me, buffer.asArgumentToFunctionThatExpectsOneBasedArray(), nbuf, nchannels, 1, i);
			_LongSound_to_multichannel_buffer (thee, buffer.asArgumentToFunctionThatExpectsOneBasedArray(), nbuf, nchannels, 2, i);
			MelderFile_writeShortToAudio (file, nchannels, Melder_defaultAudioFileEncoding (audioFileType,
                numberOfBitsPerSamplePoint), & buffer [1], n_to_write);
		}
		MelderFile_writeAudioFileTrailer (file, audioFileType, Melder_ifloor (my sampleRate), nx, nchannels, numberOfBitsPerSamplePoint);
		f.close ();
	} catch (MelderError) {
		Melder_throw (me, U": no stereo audio file created.");
	}
}


/*
	BSD systems provide ftruncate, several others supply chsize, and a few
	may provide a (possibly undocumented) fcntl option F_FREESP. Under MS-DOS,
	you can sometimes use write(fd, "", 0). However, there is no portable
	solution, nor a way to delete blocks at the beginning.
*/
static void MelderFile_truncate (MelderFile me, integer size) {
#if defined (_WIN32)
	HANDLE hFile;
	DWORD fdwAccess = GENERIC_READ | GENERIC_WRITE, fPos;
	DWORD fdwShareMode = 0;   // file cannot be shared
	LPSECURITY_ATTRIBUTES lpsa = nullptr;
	DWORD fdwCreate = OPEN_EXISTING;
	LARGE_INTEGER fileSize;

	MelderFile_close (me);

	hFile = CreateFileW (Melder_peek32toW_fileSystem (my path),
			fdwAccess, fdwShareMode, lpsa, fdwCreate, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	Melder_require (hFile != INVALID_HANDLE_VALUE,
		U"Cannot open file ", me, U".");

	fileSize.LowPart = size; // Set current file pointer to position 'size'
	fileSize.HighPart = 0; // Limit the file size to 2^32 - 2 bytes
	fPos = SetFilePointer (hFile, fileSize.LowPart, & fileSize.HighPart, FILE_BEGIN);
	Melder_require (fPos != 0xFFFF'FFFF,
		U"Can't set the position at size ", size, U" for file ", me, U".");

	// Limit the file size as the current position of the file pointer.

	SetEndOfFile (hFile);
	CloseHandle (hFile);
#elif defined (linux) || defined (macintosh)
	MelderFile_close (me);
	const int success = truncate (Melder_peek32to8_fileSystem (my path), size);
	Melder_require (success == 0,
		U"Truncating failed for file ", me, U" (", Melder_peek8to32_u (strerror (errno)), U").");
#else
	Melder_throw (U"Don't know what to do.");
#endif
}

static void writePartToOpenFile16 (LongSound me, int audioFileType, integer imin, integer n, MelderFile file) {
	const integer numberOfBuffers = (n - 1) / my nmax + 1, numberOfBitsPerSamplePoint = 16;
	const integer numberOfSamplesInLastBuffer = (n - 1) % my nmax + 1;
	integer offset = imin;
	if (file -> filePointer) {
		for (integer ibuffer = 1; ibuffer <= numberOfBuffers; ibuffer ++) {
			const integer numberOfSamplesToCopy = ( ibuffer < numberOfBuffers ? my nmax : numberOfSamplesInLastBuffer );
			my invalidateBuffer();
			LongSound_readAudioToShort (me, my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), offset, numberOfSamplesToCopy);
			offset += numberOfSamplesToCopy;
			MelderFile_writeShortToAudio (file, my numberOfChannels, Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), my buffer.asArgumentToFunctionThatExpectsZeroBasedArray(), numberOfSamplesToCopy);
		}
	}
}

void LongSounds_appendToExistingSoundFile (OrderedOf<structSampled>* me, MelderFile file) {
	integer pre_append_endpos = 0, numberOfBitsPerSamplePoint = 16;
	try {
		Melder_require (my size > 0,
			U"No Sound or LongSound objects to append.");
		/*
			We have to open with "r+" mode because this will position the stream
			at the beginning of the file. The "a" mode does not allow us to
			seek before the end-of-file.

			For Linux: If the file is already opened (e.g. by a LongSound) object we
			should deny access!
			Under Windows deny access is default?!
		*/

		autofile f = Melder_fopen (file, "r+b");
		file -> filePointer = f; // essential !!
		double sampleRate_d;
		integer startOfData, numberOfSamples, numberOfChannels;
		int encoding;
		const int audioFileType = MelderFile_checkSoundFile (file, & numberOfChannels, & encoding, & sampleRate_d, & startOfData, & numberOfSamples);
		Melder_require (audioFileType > 0,
			U"Not a sound file.");
		/*
			Check whether all the sampling frequencies and channels match.
		*/
		integer sampleRate = Melder_ifloor (sampleRate_d);
		for (integer i = 1; i <= my size; i ++) {
			bool sampleRatesMatch, numbersOfChannelsMatch;
			const Sampled data = my at [i];
			if (data -> classInfo == classSound) {
				Sound sound = (Sound) data;
				sampleRatesMatch = Melder_iround (1.0 / sound -> dx) == sampleRate;
				numbersOfChannelsMatch = sound -> ny == numberOfChannels;
				numberOfSamples += sound -> nx;
			} else {
				LongSound longSound = (LongSound) data;
				sampleRatesMatch = longSound -> sampleRate == sampleRate;
				numbersOfChannelsMatch = longSound -> numberOfChannels == numberOfChannels;
				numberOfSamples += longSound -> nx;
			}
			Melder_require (sampleRatesMatch,
				U"Sampling frequencies should match.");
			Melder_require (numbersOfChannelsMatch,
				U"The number of channels should match.");
		}
		/*
			Search the end of the file, count the number of bytes and append.
		*/
		MelderFile_seek (file, 0, SEEK_END);
		pre_append_endpos = MelderFile_tell (file);

		errno = 0;
		for (integer i = 1; i <= my size; i ++) {
			const Sampled data = my at [i];
			if (data -> classInfo == classSound) {
				Sound sound = (Sound) data;
				MelderFile_writeFloatToAudio (file, sound -> z.get(),
						Melder_defaultAudioFileEncoding (audioFileType, numberOfBitsPerSamplePoint), true);
			} else {
				LongSound longSound = (LongSound) data;
				writePartToOpenFile16 (longSound, audioFileType, 1, longSound -> nx, file);
			}
			Melder_require (errno == 0,
				U"Error during writing.");
		}
		/*
			Update header
		*/
		MelderFile_rewind (file);
		MelderFile_writeAudioFileHeader (file, audioFileType, sampleRate, numberOfSamples, numberOfChannels, numberOfBitsPerSamplePoint);
		MelderFile_writeAudioFileTrailer (file, audioFileType, sampleRate, numberOfSamples, numberOfChannels, numberOfBitsPerSamplePoint);
		f.close (file);
		return;
	} catch (MelderError) {
		if (errno != 0 && pre_append_endpos > 0) {
			// Restore file at original size
			int error = errno;
			MelderFile_truncate (file, pre_append_endpos);
			Melder_throw (U"File ", MelderFile_messageName (file), U" restored to original size (", Melder_peek8to32_u (strerror (error)), U").");
		} throw;
	}
}

/* End of file LongSound_extensions.cpp */
