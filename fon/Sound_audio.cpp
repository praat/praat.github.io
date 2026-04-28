/* Sound_audio.cpp
 *
 * Copyright (C) 1992-2020,2022-2026 Paul Boersma
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

#include <errno.h>

#ifdef linux
	#define DEV_AUDIO  "/dev/dsp"
#else
	#define DEV_AUDIO  "/dev/audio"
#endif

#include "Sound.h"
#include "Preferences.h"
#include "../external/portaudio/portaudio.h"

#if defined (macintosh)
	#include "macport_on.h"
	#include "pa_mac_core.h"
	#include "macport_off.h"
#elif defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include <mmsystem.h>
	#include "winport_off.h"
#elif defined (linux)
	#include <fcntl.h>
	#if ! defined (NO_AUDIO)
		#if defined (__OpenBSD__) || defined (__NetBSD__)
			#include <soundcard.h>
		#else
			#include <sys/soundcard.h>
		#endif
	#endif
	#include <sys/ioctl.h>   /* ioctl */
	#include <unistd.h>   /* open write close read */
#else
	#include <fcntl.h>
#endif

struct Sound_recordFixedTime_Info {
	integer numberOfSamples, numberOfSamplesRead;
	short *buffer;
};
static integer getNumberOfSamplesRead (volatile struct Sound_recordFixedTime_Info *info) {
	volatile integer numberOfSamplesRead = info -> numberOfSamplesRead;
	return numberOfSamplesRead;
}

static int portaudioStreamCallback (
    const void *input, void * /*output*/,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo *  /*timeInfo*/,
    PaStreamCallbackFlags /*statusFlags*/,
    void *void_info)
{
	struct Sound_recordFixedTime_Info *info = (struct Sound_recordFixedTime_Info *) void_info;
	integer samplesLeft = info -> numberOfSamples - info -> numberOfSamplesRead;
	if (samplesLeft > 0) {
		integer dsamples = std::min (samplesLeft, uinteger_to_integer_a (frameCount));
		memcpy (info -> buffer + info -> numberOfSamplesRead, input, integer_to_uinteger_a (2 * dsamples));
		info -> numberOfSamplesRead += dsamples;
		const short *input2 = (const short *) input;
		//Melder_casual (U"read ", dsamples, U" samples: ", input2 [0], U", ", input2 [1], U", ", input2 [3], U"...");
		if (info -> numberOfSamplesRead >= info -> numberOfSamples)
			return paComplete;
	} else /*if (info -> numberOfSamplesRead >= info -> numberOfSamples)*/ {
		info -> numberOfSamplesRead = info -> numberOfSamples;
		return paComplete;
	}
	return paContinue;
}

autoSound Sound_record_fixedTime (int inputSource, double gain, double balance, double sampleRate, double duration) {
	bool inputUsesPortAudio =
		#if defined (_WIN32)
			MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem::MME_VIA_PORTAUDIO;
		#elif defined (macintosh)
			MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem::COREAUDIO_VIA_PORTAUDIO;
		#elif defined (raspberrypi)
			MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem::JACK_VIA_PORTAUDIO;
		#else
			MelderAudio_getInputSoundSystem () == kMelder_inputSoundSystem::ALSA_OR_JACK_VIA_PORTAUDIO;
		#endif
	PaStream *portaudioStream = nullptr;
	#if defined (macintosh)
	#elif defined (_WIN32)
		HWAVEIN hWaveIn = 0;
	#else
		int fd = -1;   // other systems use stream I/O with a file descriptor
		int fd_mixer = -1;
	#endif
	try {
		integer numberOfSamples, i;

		/*
			Declare platform-dependent data structures.
		*/
		volatile struct Sound_recordFixedTime_Info info = { 0 };
		PaStreamParameters streamParameters = { 0 };
		#if defined (macintosh)
			(void) gain;
			(void) balance;
		#elif defined (_WIN32)
			WAVEFORMATEX waveFormat;
			WAVEHDR waveHeader;
			MMRESULT err;
			(void) inputSource;
			(void) gain;
			(void) balance;
		#elif defined (linux)
			int dev_mask;
			int val;
		#endif

		/*
			Check representation of shorts.
		*/
		if (sizeof (short) != 2)
			Melder_throw (U"Cannot record a sound on this computer.");

		/*
			Check sampling frequency.
		*/
		bool supportsSamplingFrequency = true;
		if (inputUsesPortAudio) {
			#if defined (macintosh)
				if (sampleRate != 44100 && sampleRate != 48000 && sampleRate != 96000)
					supportsSamplingFrequency = false;
			#endif
		} else {
			#if defined (macintosh)
				if (sampleRate != 44100 && sampleRate != 48000)
					supportsSamplingFrequency = false;
			#elif defined (linux)
				if (sampleRate != 8000 && sampleRate != 11025 &&
						sampleRate != 16000 && sampleRate != 22050 &&
						sampleRate != 32000 && sampleRate != 44100 &&
						sampleRate != 48000)
					supportsSamplingFrequency = false;
			#elif defined (_WIN32)
				if (sampleRate != 8000 && sampleRate != 11025 &&
						sampleRate != 16000 && sampleRate != 22050 &&
						sampleRate != 32000 && sampleRate != 44100 &&
						sampleRate != 48000 && sampleRate != 96000)
					supportsSamplingFrequency = false;
			#endif
		}
		if (! supportsSamplingFrequency)
			Melder_throw (U"Your audio hardware does not support a sampling frequency of ", sampleRate, U" Hz.");

		/*
			Open phase 1.
			On some platforms, the info is filled in before the audio port is opened.
			On other platforms, the info is filled in after the port is opened.
		*/
		if (inputUsesPortAudio) {
			if (! MelderAudio_hasBeenInitialized) {
				PaError err = Pa_Initialize ();
				if (err)
					Melder_throw (U"Pa_Initialize: ", Melder_peek8to32_u (Pa_GetErrorText (err)));
				MelderAudio_hasBeenInitialized = true;
			}
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
			#elif ! defined (NO_AUDIO)
				/*
					We must open the port now, because we use an ioctl to set the info to an open port.
				*/
				fd = open (DEV_AUDIO, O_RDONLY);
				if (fd == -1) {
					if (errno == EBUSY)
						Melder_throw (U"Audio device in use by another program.");
					else
						#ifdef linux
							Melder_throw (U"Cannot open audio device.\nPlease switch on PortAudio in the Sound Recording Preferences.");
						#else
							Melder_throw (U"Cannot open audio device.");
						#endif
				}
				/*
					The device immediately started recording into its buffer,
					but probably at the wrong rate etc.
					Pause and flush this rubbish.
				*/
				#if defined (linux)
					ioctl (fd, SNDCTL_DSP_RESET, nullptr);
				#endif
			#endif
		}

		/*
			Set the input source; the default is the microphone.
		*/
		if (inputUsesPortAudio) {
			if (inputSource < 1 || inputSource > Pa_GetDeviceCount ())
				Melder_throw (U"Unknown device #", inputSource, U".");
			/*
				Saying
					streamParameters. device = inputSource - 1;
				would presuppose that the input devices are listed before the output devices.
				TODO: cycle through all devices, and determine which of them are input devices
			*/
			streamParameters. device = Pa_GetDefaultInputDevice ();
			Melder_casual (U"streamParameters. device: ", (integer) streamParameters. device);
			const PaDeviceInfo *paDeviceInfo = Pa_GetDeviceInfo (streamParameters. device);
			Melder_casual (U"Name: ", Melder_peek8to32_u (paDeviceInfo -> name));
		} else {
			#if defined (macintosh)
			#elif defined (linux) && ! defined (NO_AUDIO)
				fd_mixer = open ("/dev/mixer", O_WRONLY);		
				if (fd_mixer == -1)
					Melder_throw (U"Cannot open /dev/mixer.");
				dev_mask = inputSource == 1 ? SOUND_MASK_MIC : SOUND_MASK_LINE;
				if (ioctl (fd_mixer, SOUND_MIXER_WRITE_RECSRC, & dev_mask) == -1)
					Melder_throw (U"Cannot set recording device in mixer");
			#endif
		}

		/*
			Set gain and balance.
		*/
		if (inputUsesPortAudio) {
			/* Taken from Audio Control Panel. */
		} else {
			#if defined (macintosh) || defined (_WIN32)
				/* Taken from Audio Control Panel. */
			#elif defined (linux) && ! defined (NO_AUDIO)
				val = ( gain <= 0.0 ? 0 : gain >= 1.0 ? 100 : Melder_iround (gain * 100) );
				balance = ( balance <= 0.0 ? 0 : balance >= 1 ? 1 : balance );
				if (balance >= 0.5) {
					val = (int)(((int)(val*balance/(1-balance)) << 8) | val);
				} else {
					val = (int)(val | ((int)(val*(1-balance)/balance) << 8));
				}
				val = (int)((std::min(2.0-2.0*balance,1.0))*val) | ((int)((std::min(2.0*balance,1.0))*val) << 8);
				if (inputSource == 1) {			
					/* MIC */		       
					if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_MIC), & val) == -1)
						Melder_throw (U"Cannot set gain and balance.");
				} else {
					/* LINE */
					if (ioctl (fd_mixer, MIXER_WRITE (SOUND_MIXER_LINE), & val) == -1)
						Melder_throw (U"Cannot set gain and balance.");
				}
				close (fd_mixer);
				fd_mixer = -1;
			#endif
		}

		/*
			Set the sampling frequency.
		*/
		if (inputUsesPortAudio) {
			// Set while opening.
		} else {
			#if defined (macintosh)
			#elif defined (linux) && ! defined (NO_AUDIO)
				int sampleRate_int = (int) sampleRate;
				if (ioctl (fd, SNDCTL_DSP_SPEED, & sampleRate_int) == -1)
					Melder_throw (U"Cannot set sampling frequency to ", sampleRate, U" Hz.");
			#elif defined (_WIN32)
				waveFormat. nSamplesPerSec = sampleRate;
			#endif
		}

		/*
			Set the number of channels to 1 (mono), if possible.
		*/
		if (inputUsesPortAudio) {
			streamParameters. channelCount = 1;
		} else {
			#if defined (macintosh)
			#elif defined (linux) && ! defined (NO_AUDIO)
				val = 1;
				if (ioctl (fd, SNDCTL_DSP_CHANNELS, & val) == -1)
					Melder_throw (U"Cannot set to mono.");
			#elif defined (_WIN32)
				waveFormat. nChannels = 1;
			#endif
		}

		/*
			Set the encoding to 16-bit linear (or to 8-bit linear, if 16-bit is not available).
		*/
		if (inputUsesPortAudio) {
			streamParameters. sampleFormat = paInt16;
		} else {
			#if defined (macintosh)
			#elif defined (linux) && ! defined (NO_AUDIO)
				#if __BYTE_ORDER == __BIG_ENDIAN
					val = AFMT_S16_BE;
				#else
					val = AFMT_S16_LE;
				#endif
				if (ioctl (fd, SNDCTL_DSP_SETFMT, & val) == -1)
					Melder_throw (U"Cannot set 16-bit linear.");
			#elif defined (_WIN32)
				waveFormat. wFormatTag = WAVE_FORMAT_PCM;
				waveFormat. wBitsPerSample = 16;
				waveFormat. nBlockAlign = waveFormat. nChannels * waveFormat. wBitsPerSample / 8;
				waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
			#endif
		}

		/*
			Create a buffer for recording, and the resulting sound.
		*/
		numberOfSamples = Melder_iround (sampleRate * duration);
		if (numberOfSamples < 1)
			Melder_throw (U"Duration too short.");
		autovector<short> buffer = newvectorzero <short> (numberOfSamples);
		autoSound me = Sound_createSimple (1, numberOfSamples / sampleRate, sampleRate);
		Melder_assert (my nx == numberOfSamples);

		/*
			Open phase 2.
			This starts recording now.
		*/
		if (inputUsesPortAudio) {
			streamParameters. suggestedLatency = Pa_GetDeviceInfo (streamParameters. device) -> defaultLowInputLatency;
			#if defined (macintosh)
				PaMacCoreStreamInfo macCoreStreamInfo = { 0 };
				macCoreStreamInfo. size = sizeof (PaMacCoreStreamInfo);
				macCoreStreamInfo. hostApiType = paCoreAudio;
				macCoreStreamInfo. version = 0x01;
				macCoreStreamInfo. flags = paMacCoreChangeDeviceParameters | paMacCoreFailIfConversionRequired;
				macCoreStreamInfo. channelMap = nullptr;
				macCoreStreamInfo. channelMapSize = 0;
				streamParameters. hostApiSpecificStreamInfo = & macCoreStreamInfo;
			#endif
			info. numberOfSamples = numberOfSamples;
			info. numberOfSamplesRead = 0;
			info. buffer = buffer.asArgumentToFunctionThatExpectsZeroBasedArray();
			PaError err = Pa_OpenStream (& portaudioStream, & streamParameters, nullptr,
				sampleRate,
				0,   // this gives the default of 64 samples per buffer on Paul's 2018 MacBook Pro (checked 20200813)
				paNoFlag, portaudioStreamCallback, (void *) & info);
			if (err)
				Melder_throw (U"open ", Melder_peek8to32_u (Pa_GetErrorText (err)));
			Pa_StartStream (portaudioStream);
			if (err)
				Melder_throw (U"start ", Melder_peek8to32_u (Pa_GetErrorText (err)));
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				waveFormat. cbSize = 0;
				err = waveInOpen (& hWaveIn, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL);
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while opening.");
			#endif
		}
for (i = 1; i <= numberOfSamples; i ++) trace (U"Started ", buffer [i]);

		/*
			Read the sound into the buffer.
		*/
		if (inputUsesPortAudio) {
			// The callback will do this. Just wait.
			while (/*getNumberOfSamplesRead (& info)*/ info. numberOfSamplesRead < numberOfSamples) {
				//Pa_Sleep (1);
				trace (U"filled ", getNumberOfSamplesRead (& info), U"/", numberOfSamples);
			}
for (i = 1; i <= numberOfSamples; i ++) trace (U"Recorded ", buffer [i]);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				waveHeader. dwFlags = 0;
				waveHeader. lpData = (char *) buffer.asArgumentToFunctionThatExpectsZeroBasedArray();
				waveHeader. dwBufferLength = numberOfSamples * 2;
				waveHeader. dwLoops = 0;
				waveHeader. lpNext = nullptr;
				waveHeader. reserved = 0;
				err = waveInPrepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while preparing header.");
				err = waveInAddBuffer (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while listening.");
				err = waveInStart (hWaveIn);
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while starting.");
					while (! (waveHeader. dwFlags & WHDR_DONE)) { Pa_Sleep (1); }
				err = waveInUnprepareHeader (hWaveIn, & waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while unpreparing header.");
			#else
				integer bytesLeft = 2 * numberOfSamples, dbytes, bytesRead = 0;
				while (bytesLeft) {
					dbytes = read (fd, (char *) buffer.asArgumentToFunctionThatExpectsZeroBasedArray() + bytesRead, std::min (bytesLeft, 4000_integer));
					if (dbytes <= 0)
						break;
					bytesLeft -= dbytes;
					bytesRead += dbytes;
				};
			#endif
		}

		/*
			Copy the buffered data to the sound object, and discard the buffer.
		*/
		for (i = 1; i <= numberOfSamples; i ++)
			my z [1] [i] = buffer [i] * (1.0 / 32768);

		/*
			Close the audio device.
		*/
		if (inputUsesPortAudio) {
			Pa_StopStream (portaudioStream);
			Pa_CloseStream (portaudioStream);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				err = waveInClose (hWaveIn);
				if (err != MMSYSERR_NOERROR)
					Melder_throw (U"Error ", err, U" while closing.");
			#else
				close (fd);
			#endif
		}

		/*
			Hand the resulting sound to the caller.
		*/
		return me;
	} catch (MelderError) {
		if (inputUsesPortAudio) {
			if (portaudioStream)
				Pa_StopStream (portaudioStream);
			if (portaudioStream)
				Pa_CloseStream (portaudioStream);
		} else {
			#if defined (macintosh)
			#elif defined (_WIN32)
				if (hWaveIn != 0)
					waveInClose (hWaveIn);
			#else
				if (fd_mixer != -1)
					close (fd_mixer);
				if (fd != -1)
					close (fd);
			#endif
		}
		Melder_throw (U"Sound not recorded.");
	}
}

/********** PLAYING A SOUND **********/

static struct SoundPlay {
	integer numberOfSamples, i1, i2, silenceBefore, silenceAfter;
	double tmin, tmax, dt, t1;
	Sound_PlayCallback callback;
	Thing boss;
	autovector <int16> outputBuffer;
} thePlayingSound;

static bool melderPlayCallback (void *closure, integer samplesPlayed) {
	struct SoundPlay *me = (struct SoundPlay *) closure;
	int phase = 2;
	double t = ( samplesPlayed <= my silenceBefore ? my tmin :
			samplesPlayed >= my silenceBefore + my numberOfSamples ? my tmax :
			my t1 + (my i1 - 1.5 + samplesPlayed - my silenceBefore) * my dt );
	if (! MelderAudio_isPlaying) {
		my outputBuffer. reset();   // get a bit of privacy
		phase = 3;
	}
	if (my callback)
		return my callback (my boss, phase, my tmin, my tmax, t);
	return true;
}

void Sound_playPart (constSound me, double tmin, double tmax, Sound_PlayCallback callback, Thing boss)
{
	try {
		integer ifsamp = Melder_iround (1.0 / my dx), bestSampleRate = MelderAudio_getOutputBestSampleRate (ifsamp);
		if (ifsamp == bestSampleRate) {
			struct SoundPlay *thee = (struct SoundPlay *) & thePlayingSound;
			double *fromLeft = & my z [1] [0], *fromRight = ( my ny > 1 ? & my z [2] [0] : nullptr );
			MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
			integer i1, i2;
			if ((thy numberOfSamples = Matrix_getWindowSamplesX (me, tmin, tmax, & i1, & i2)) < 1)
				return;
			thy tmin = tmin;
			thy tmax = tmax;
			thy dt = my dx;
			thy t1 = my x1;
			thy callback = callback;
			thy boss = boss;
			thy silenceBefore = Melder_iroundTowardsZero (ifsamp * MelderAudio_getOutputSilenceBefore ());
			thy silenceAfter = Melder_iroundTowardsZero (ifsamp * MelderAudio_getOutputSilenceAfter ());
			integer numberOfChannels = my ny;
			thy outputBuffer = newvectorzero <int16> ((i2 - i1 + 1 + thy silenceBefore + thy silenceAfter) * numberOfChannels);
			thy i1 = i1;
			thy i2 = i2;
			int16 *to = & thy outputBuffer [0] + thy silenceBefore * numberOfChannels;
			if (numberOfChannels > 2) {
				for (integer i = i1; i <= i2; i ++) {
					for (integer chan = 1; chan <= my ny; chan ++) {
						integer value = Melder_iround_tieDown (my z [chan] [i] * 32768.0);
						* ++ to = (int16) Melder_clipped (-32768_integer, value, +32767_integer);
					}
				}
			} else if (numberOfChannels == 2) {
				for (integer i = i1; i <= i2; i ++) {
					integer valueLeft = Melder_iround_tieDown (fromLeft [i] * 32768.0);
					* ++ to = (int16) Melder_clipped (-32768_integer, valueLeft, +32767_integer);
					integer valueRight = Melder_iround_tieDown (fromRight [i] * 32768.0);
					* ++ to = (int16) Melder_clipped (-32768_integer, valueRight, +32767_integer);
				}
			} else {
				for (integer i = i1; i <= i2; i ++) {
					integer value = Melder_iround_tieDown (fromLeft [i] * 32768.0);
					* ++ to = (int16) Melder_clipped (-32768_integer, value, +32767_integer);
				}
			}
			if (thy callback)
				thy callback (thy boss, 1, tmin, tmax, tmin);
			MelderAudio_play16 (thy outputBuffer.asArgumentToFunctionThatExpectsZeroBasedArray(), ifsamp,
				thy silenceBefore + thy numberOfSamples + thy silenceAfter, numberOfChannels, melderPlayCallback, thee);
		} else {
			autoSound part = Sound_extractPart (me, tmin, tmax, kSound_windowShape::RECTANGULAR, 1.0, true);
			autoSound resampled = Sound_resample (part.get(), bestSampleRate, 20);
			Sound_playPart (resampled.get(), tmin, tmax, callback, boss);   // recursively
		}
	} catch (MelderError) {
		Melder_throw (me, U": not played.");
	}
}

void Sound_play (constSound me, Sound_PlayCallback playCallback, Thing playClosure) {
	Sound_playPart (me, my xmin, my xmax, playCallback, playClosure);
}

void SoundList_play (SoundList me, Sound_PlayCallback playCallback, Thing playClosure) {
	autoSound cat = Sounds_concatenate (me, 0.0);
	Sound_play (cat.get(), playCallback, playClosure);
}

/* End of file Sound_audio.cpp */
