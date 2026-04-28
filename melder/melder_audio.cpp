/* melder_audio.cpp
 *
 * Copyright (C) 1992-2022,2025,2026 Paul Boersma, David Weenink
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

#if defined (macintosh)
	#include <sys/time.h>
	#include <CoreAudio/CoreAudio.h>
#elif defined (_WIN32)
	#include <windows.h>
#elif defined (linux)
	#include <sys/time.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/ioctl.h>
	#include <fcntl.h>
	#include <unistd.h>
	#if defined HAVE_PULSEAUDIO
		#include <pulse/pulseaudio.h>
	#endif
	#if ! defined (NO_AUDIO)
		#if defined (__OpenBSD__) || defined (__NetBSD__)
			#include <soundcard.h>
		#else
			#include <sys/soundcard.h>
		#endif
	#endif
	#include <errno.h>
#endif

#include "melder.h"
#include <time.h>
#include "../sys/Gui.h"
#include "../sys/Preferences.h"
#include "../external/portaudio/portaudio.h"

#ifdef HAVE_PULSEAUDIO
	#define PA_GETTINGINFO 1
	#define PA_GETTINGINFO_DONE 2
	#define PA_WRITING 4
	#define PA_WRITING_DONE 8
	#define PA_RECORDING 16
	#define PA_RECORDING_DONE 32
	#define PA_QUERY_NUMBEROFCHANNELS 64
	#define PA_QUERY_NUMBEROFCHANNELS_DONE 128
	void pulseAudio_initialize ();
	void pulseAudio_cleanup ();
	void pulseAudio_serverReport ();
	void stream_state_cb (pa_stream *stream, void *userdata);
	void stream_drain_complete_cb (pa_stream *stream, int success, void *userdata);
	void context_state_cb (pa_context *context, void *userdata);
	void context_drain_complete_cb (pa_context *context, void *userdata);
	void prepare_and_play (struct MelderPlay *me);
	void stream_write_cb (pa_stream *stream, size_t length, void *userdata);
	void stream_write_cb2 (pa_stream *stream, size_t length, void *userdata);
	void pulseAudio_server_info_cb (pa_context *context, const pa_server_info *info, void *userdata);
#endif

static struct {
	enum kMelder_asynchronicityLevel maximumAsynchronicity;
	enum kMelder_inputSoundSystem inputSoundSystem;
	enum kMelder_outputSoundSystem outputSoundSystem;
	double silenceBefore, silenceAfter;
} preferences;

void Melder_audio_prefs () {
	Preferences_addEnum (U"Audio.maximumAsynchronicity", & preferences. maximumAsynchronicity, kMelder_asynchronicityLevel, kMelder_asynchronicityLevel::DEFAULT);
	Preferences_addEnum (U"Audio.inputSoundSystem", & preferences. inputSoundSystem, kMelder_inputSoundSystem, kMelder_inputSoundSystem::DEFAULT);
	Preferences_addEnum (U"Audio.outputSoundSystem", & preferences. outputSoundSystem, kMelder_outputSoundSystem, kMelder_outputSoundSystem::DEFAULT);
	Preferences_addDouble (U"Audio.silenceBefore2", & preferences. silenceBefore, kMelderAudio_outputSilenceBefore_DEFAULT);
	Preferences_addDouble (U"Audio.silenceAfter2", & preferences. silenceAfter, kMelderAudio_outputSilenceAfter_DEFAULT);
}

void MelderAudio_setOutputMaximumAsynchronicity (enum kMelder_asynchronicityLevel maximumAsynchronicity) {
	//MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // BUG
	preferences. maximumAsynchronicity = maximumAsynchronicity;
}
enum kMelder_asynchronicityLevel MelderAudio_getOutputMaximumAsynchronicity () {
	return preferences. maximumAsynchronicity;
}

void MelderAudio_setInputSoundSystem (enum kMelder_inputSoundSystem inputSoundSystem) {
	preferences. inputSoundSystem = inputSoundSystem;
}
enum kMelder_inputSoundSystem MelderAudio_getInputSoundSystem () {
	return preferences. inputSoundSystem;
}

void MelderAudio_setOutputSoundSystem (enum kMelder_outputSoundSystem outputSoundSystem) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. outputSoundSystem = outputSoundSystem;
}
enum kMelder_outputSoundSystem MelderAudio_getOutputSoundSystem () {
	return preferences. outputSoundSystem;
}

void MelderAudio_setOutputSilenceBefore (double silenceBefore) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. silenceBefore = silenceBefore;
}
double MelderAudio_getOutputSilenceBefore () { return preferences. silenceBefore; }

void MelderAudio_setOutputSilenceAfter (double silenceAfter) {
	MelderAudio_stopPlaying (MelderAudio_IMPLICIT);
	preferences. silenceAfter = silenceAfter;
}
double MelderAudio_getOutputSilenceAfter () { return preferences. silenceAfter; }

integer MelderAudio_getOutputBestSampleRate (integer fsamp) {
	#if defined (macintosh)
		return fsamp == 44100 || fsamp == 48000 || fsamp == 96000 ? fsamp : 44100;
	#elif defined (_WIN32)
		return fsamp == 8000 || fsamp == 11025 || fsamp == 16000 || fsamp == 22050 ||
			fsamp == 32000 || fsamp == 44100 || fsamp == 48000 || fsamp == 96000 ? fsamp : 44100;
	#elif defined (raspberrypi)
		return 48000;
	#elif defined (linux)
		return fsamp == 44100 || fsamp == 48000 || fsamp == 96000 ? fsamp : 44100;
	#else
		return 44100;
	#endif
}

static double theStartingTime = 0.0;

static struct MelderPlay {
	int16 *playBuffer;
	integer sampleRate, numberOfSamples, samplesLeft, samplesSent, samplesPlayed;
	kMelder_asynchronicityLevel asynchronicity;
	integer numberOfChannels;
	bool explicitStop;
	volatile int volatile_interrupted;
	bool (*callback) (void *closure, integer samplesPlayed);
	void *closure;
	#if gtk
		gint workProcId_gtk = 0;
	#elif motif
		XtWorkProcId workProcId_motif;
	#elif cocoa
		CFRunLoopTimerRef cocoaTimer;
	#endif
	bool usePortAudio, supports_paComplete, usePulseAudio;
	PaStream *stream;
	double paStartingTime;
	#if defined (macintosh)
	#elif defined (linux)
		int audio_fd, val, err;
	#elif defined (_WIN32)
		HWAVEOUT hWaveOut;
		WAVEHDR waveHeader;
		MMRESULT status;
	#endif
	#ifdef HAVE_PULSEAUDIO
		struct {
			pa_sample_spec sample_spec;
			pa_threaded_mainloop *mainloop = nullptr;
			pa_mainloop_api *mainloop_api = nullptr;
			pa_context *context = nullptr;
			pa_stream *stream = nullptr;
			pa_operation *operation_drain = nullptr;
			pa_operation *operation_info = nullptr;
			pa_stream_flags_t stream_flags;
			const pa_timing_info *timing_info = nullptr;
			struct timeval startTime = {0, 0};
			pa_usec_t timer_event_usec = 50000;   // 50 ms
			pa_time_event *timer_event = nullptr;
			const pa_channel_map *channel_map = nullptr;
			pa_usec_t r_usec;
			pa_buffer_attr buffer_attr;
			uint32 latency = 0; // in bytes of buffer
			int32 latency_msec = 20;
			bool pulseAudioInitialized = false;
			unsigned int occupation = PA_WRITING;
		} pulseAudio;
	#endif
} thePlay;

integer MelderAudio_getSamplesPlayed () {
	return thePlay. samplesPlayed;
}

bool MelderAudio_stopWasExplicit () {
	return thePlay. explicitStop;
}

/*
	The flush () function will always have to be invoked after normal play, i.e. in the following cases:
	1. After synchronous play (asynchronicity = 0, 1, or 2).
	2. After interruption of asynchronicity 2 by the ESCAPE key.
	3. After asynchronous play, by the workProc.
	4. After interruption of asynchronicity 3 by MelderAudio_stopPlaying ().
*/
static bool flush () {
	struct MelderPlay *me = & thePlay;
	if (my usePortAudio) {
		if (my stream) {
			#ifdef linux

				Pa_Sleep (200);   // this reduces the chance of seeing the Alsa/PulseAudio deadlock:
				/*
					(gdb) thread apply all bt

					Thread 13 (Thread 0x7fffde1d2700 (LWP 25620)):
					#0  0x00007ffff65a3d67 in pthread_cond_wait@@GLIBC_2.3.2 ()
					   from /lib/x86_64-linux-gnu/libpthread.so.0
					#1  0x00007fffec0b3980 in pa_threaded_mainloop_wait () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#2  0x00007fffde407054 in pulse_wait_operation ()
					   from /usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_pcm_pulse.so
					#3  0x00007fffde405c10 in ?? ()
					   from /usr/lib/x86_64-linux-gnu/alsa-lib/libasound_module_pcm_pulse.so
					#4  0x00007ffff6843708 in alsa_snd_pcm_drop () from /usr/lib/x86_64-linux-gnu/libasound.so.2
					#5  0x0000000000812de0 in AlsaStop ()
					#6  0x00000000008183e1 in OnExit ()
					#7  0x0000000000818483 in CallbackThreadFunc ()
					#8  0x00007ffff659fe9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
					#9  0x00007ffff5aba3fd in clone () from /lib/x86_64-linux-gnu/libc.so.6
					#10 0x0000000000000000 in ?? ()

					Thread 12 (Thread 0x7fffdffff700 (LWP 25619)):
					#0  0x00007ffff659d9b0 in __pthread_mutex_lock_full () from /lib/x86_64-linux-gnu/libpthread.so.0
					#1  0x00007fffdf3d7e1e in pa_mutex_lock () from /usr/lib/x86_64-linux-gnu/libpulsecommon-1.1.so
					#2  0x00007fffec0b3369 in ?? () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#3  0x00007fffec0a476c in pa_mainloop_poll () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#4  0x00007fffec0a4dd9 in pa_mainloop_iterate () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#5  0x00007fffec0a4e90 in pa_mainloop_run () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#6  0x00007fffec0b330f in ?? () from /usr/lib/x86_64-linux-gnu/libpulse.so.0
					#7  0x00007fffdf3d8d18 in ?? () from /usr/lib/x86_64-linux-gnu/libpulsecommon-1.1.so
					#8  0x00007ffff659fe9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
					#9  0x00007ffff5aba3fd in clone () from /lib/x86_64-linux-gnu/libc.so.6
					#10 0x0000000000000000 in ?? ()

					Thread 3 (Thread 0x7fffefd8b700 (LWP 25610)):
					#0  0x00007ffff5aaea43 in poll () from /lib/x86_64-linux-gnu/libc.so.6
					#1  0x00007ffff6ae9ff6 in ?? () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#2  0x00007ffff6aea45a in g_main_loop_run () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#3  0x00007ffff4bb75e6 in ?? () from /usr/lib/x86_64-linux-gnu/libgio-2.0.so.0
					#4  0x00007ffff6b0b9b5 in ?? () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#5  0x00007ffff659fe9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
					---Type <return> to continue, or q <return> to quit---
					#6  0x00007ffff5aba3fd in clone () from /lib/x86_64-linux-gnu/libc.so.6
					#7  0x0000000000000000 in ?? ()

					Thread 2 (Thread 0x7ffff058c700 (LWP 25609)):
					#0  0x00007ffff5aaea43 in poll () from /lib/x86_64-linux-gnu/libc.so.6
					#1  0x00007ffff6ae9ff6 in ?? () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#2  0x00007ffff6aea45a in g_main_loop_run () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#3  0x00007ffff059698b in ?? () from /usr/lib/x86_64-linux-gnu/gio/modules/libdconfsettings.so
					#4  0x00007ffff6b0b9b5 in ?? () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#5  0x00007ffff659fe9a in start_thread () from /lib/x86_64-linux-gnu/libpthread.so.0
					#6  0x00007ffff5aba3fd in clone () from /lib/x86_64-linux-gnu/libc.so.6
					#7  0x0000000000000000 in ?? ()

					Thread 1 (Thread 0x7ffff7fce940 (LWP 25608)):
					#0  0x00007ffff65a1148 in pthread_join () from /lib/x86_64-linux-gnu/libpthread.so.0
					#1  0x000000000081073e in PaUnixThread_Terminate ()
					#2  0x0000000000818239 in RealStop ()
					#3  0x00000000008182c7 in AbortStream ()
					#4  0x0000000000811ce5 in Pa_CloseStream ()
					#5  0x0000000000753a6d in flush ()
					#6  0x0000000000753dae in MelderAudio_stopPlaying ()
					#7  0x00000000004d80e1 in Sound_playPart ()
					#8  0x00000000004f7b48 in structSoundEditor::v_play ()
					#9  0x00000000004e7197 in gui_drawingarea_cb_click ()
					#10 0x00000000007d0d35 in _GuiGtkDrawingArea_clickCallback ()
					#11 0x00007ffff78d6e78 in ?? () from /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0
					#12 0x00007ffff6da6ca2 in g_closure_invoke () from /usr/lib/x86_64-linux-gnu/libgobject-2.0.so.0
					#13 0x00007ffff6db7d71 in ?? () from /usr/lib/x86_64-linux-gnu/libgobject-2.0.so.0
					#14 0x00007ffff6dbfd4e in g_signal_emit_valist ()
					   from /usr/lib/x86_64-linux-gnu/libgobject-2.0.so.0
					#15 0x00007ffff6dc0212 in g_signal_emit () from /usr/lib/x86_64-linux-gnu/libgobject-2.0.so.0
					#16 0x00007ffff79f1231 in ?? () from /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0
					#17 0x00007ffff78d5003 in gtk_propagate_event () from /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0
					#18 0x00007ffff78d5363 in gtk_main_do_event () from /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0
					#19 0x00007ffff7549cac in ?? () from /usr/lib/x86_64-linux-gnu/libgdk-x11-2.0.so.0
					#20 0x00007ffff6ae9d13 in g_main_context_dispatch () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#21 0x00007ffff6aea060 in ?? () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					---Type <return> to continue, or q <return> to quit---
					#22 0x00007ffff6aea45a in g_main_loop_run () from /lib/x86_64-linux-gnu/libglib-2.0.so.0
					#23 0x00007ffff78d4397 in gtk_main () from /usr/lib/x86_64-linux-gnu/libgtk-x11-2.0.so.0
					#24 0x00000000007909eb in praat_run ()
					#25 0x000000000040e009 in main ()

					Also see http://sourceforge.net/p/audacity/mailman/audacity-devel/thread/200912181409.49839.businessmanprogrammersteve@gmail.com/
				*/
			#endif
			Pa_CloseStream (my stream);
			my stream = nullptr;
		}
	#ifdef HAVE_PULSEAUDIO
	} else if (my usePulseAudio) {
		if (my pulseAudio.mainloop) {
			pa_threaded_mainloop_lock (my pulseAudio.mainloop);
			if (my pulseAudio.stream) {
				pa_operation *operation = pa_stream_flush (my pulseAudio.stream, stream_drain_complete_cb, me);
				// wait because of a threaded_mainloop
				if (operation) {
					while (MelderAudio_isPlaying) {
						pa_threaded_mainloop_wait (my pulseAudio.mainloop);
						trace (U"wait");
					}
					pa_operation_unref (operation);
				}
			}
			MelderAudio_isPlaying = false;
			pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
			pulseAudio_cleanup ();
		}
	#endif
	} else {
	#if defined (macintosh)
	#elif defined (linux) && ! defined (NO_AUDIO)
		
		/*
			As on Sun.
		*/
		if (my audio_fd) {
			ioctl (my audio_fd, SNDCTL_DSP_RESET, (my val = 0, & my val));
			close (my audio_fd), my audio_fd = 0;
		}
	#elif defined (_WIN32)
		/*
			FIX: Do not reset the sound card if played to the end:
			the last 20 milliseconds may be truncated!
			This used to happen on Barbertje's Dell PC, not with SoundBlaster.
		*/
		if (my samplesPlayed != my numberOfSamples || Melder_debug == 2)
			waveOutReset (my hWaveOut);
		my status = waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
		if (/* Melder_debug == 3 && */ my status == WAVERR_STILLPLAYING) {
			waveOutReset (my hWaveOut);
			waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
		}
		waveOutClose (my hWaveOut), my hWaveOut = 0;
	#endif
	}
	MelderAudio_isPlaying = false;
	if (my samplesPlayed >= my numberOfSamples)
		my samplesPlayed = my numberOfSamples;
	if (my samplesPlayed <= 0)
		my samplesPlayed = 1;
	/*
		Call the callback for the last time, which is recognizable by the value of MelderAudio_isPlaying.
		In this way, the caller of Melder_play16 can be notified.
		The caller can examine the actual number of samples played by testing MelderAudio_getSamplesPlayed ().
	*/
	if (my callback)
		my callback (my closure, my samplesPlayed);
	my callback = 0;
	my closure = 0;
	return true;   /* Remove workProc if called from workProc. */
}

bool MelderAudio_stopPlaying (bool explicitStop) {
	//Melder_casual (U"stop playing!");
	struct MelderPlay *me = & thePlay;
	my explicitStop = explicitStop;
	trace (U"playing = ", MelderAudio_isPlaying);
	if (! MelderAudio_isPlaying || my asynchronicity < kMelder_asynchronicityLevel::ASYNCHRONOUS) return false;
	#if gtk
		if (thePlay.workProcId_gtk && ! my usePulseAudio) {
			g_source_remove (thePlay.workProcId_gtk);
		}
		thePlay.workProcId_gtk = 0;
	#elif motif
		XtRemoveWorkProc (thePlay. workProcId_motif);
	#elif cocoa
		CFRunLoopRemoveTimer (CFRunLoopGetCurrent (), thePlay. cocoaTimer, kCFRunLoopCommonModes);
	#endif
	(void) flush ();
	return true;
}

static bool workProc (void *closure) {
	struct MelderPlay *me = & thePlay;
//static integer n = 0;
//n ++;
//Melder_casual (U"workProc ", n);
	if (my usePortAudio) {
		#if defined (linux)
			const double timeElapsed = Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency;
			const integer samplesPlayed = timeElapsed * my sampleRate;
			if (my callback && ! my callback (my closure, samplesPlayed)) {
				my volatile_interrupted = 1;
				return flush ();
			}
			if (my samplesLeft == 0) {
				return flush ();
			}
		#elif defined (linuxXXX)
			/*
				Not all hostApis support paComplete or wait till all buffers have been played in Pa_StopStream.
				Once pa_win_ds implements this, we can simply do:
			*/
			if (Pa_IsStreamActive (my stream)) {
				if (my callback && ! my callback (my closure, my samplesPlayed))
					return flush ();
			} else {
				Pa_StopStream (my stream);
				my samplesPlayed = my numberOfSamples;
				return flush ();
			}
			/*
				But then we also have to use paComplete in the stream callback.
			*/
		#else
			const double timeElapsed = Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency;
			my samplesPlayed = (integer) floor (timeElapsed * my sampleRate);
			if (my supports_paComplete && Pa_IsStreamActive (my stream)) {
				if (my callback && ! my callback (my closure, my samplesPlayed)) {
					Pa_AbortStream (my stream);
					return flush ();
				}
			} else if (my samplesPlayed < my numberOfSamples + my sampleRate / 2) {   // allow the latency estimate to be 500 ms off.
				if (my callback && ! my callback (my closure, my samplesPlayed)) {
					Pa_AbortStream (my stream);
					return flush ();
				}
			} else {
				Pa_AbortStream (my stream);
				my samplesPlayed = my numberOfSamples;
				return flush ();
			}
			Pa_Sleep (10);
		#endif
	#ifdef HAVE_PULSEAUDIO
	} else if (my usePulseAudio) {
		if (my pulseAudio.mainloop) {
			pa_threaded_mainloop_lock (my pulseAudio.mainloop);
			integer samplesPlayed = 0;
			pa_usec_t diff_usec = 0;
			if (my pulseAudio.startTime.tv_usec != 0) {
				diff_usec = pa_timeval_age (& my pulseAudio.startTime);
				const double timeElapsed = diff_usec / 1000000.0;
				samplesPlayed = timeElapsed * my sampleRate;
			}
			pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
			trace (U"diff = ", diff_usec, U", samples played = ", samplesPlayed);
			if (my callback && ! my callback (my closure, samplesPlayed)) {
				my volatile_interrupted = 1;
				return flush ();
			} else if (my samplesLeft == 0 || samplesPlayed > my numberOfSamples) {
				return flush ();
			}
		}
	#endif
	} else {
	#if defined (macintosh)
	#elif defined (linux) && ! defined (NO_AUDIO)
		if (my samplesLeft > 0) {
			int dsamples = my samplesLeft > 500 ? 500 : my samplesLeft;
			write (my audio_fd, (char *) & my playBuffer [my samplesSent * my numberOfChannels], 2 * dsamples * my numberOfChannels);
			my samplesLeft -= dsamples;
			my samplesSent += dsamples;
			my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
			if (my callback && ! my callback (my closure, my samplesPlayed))
				return flush ();
		} else /*if (my samplesPlayed >= my numberOfSamples)*/ {
			close (my audio_fd), my audio_fd = 0;
			my samplesPlayed = my numberOfSamples;
			return flush ();
  		/*} else {
			my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
			if (my callback && ! my callback (my closure, my samplesPlayed))
				return flush ();*/
		}
	#elif defined (_WIN32)
  		if (my waveHeader. dwFlags & WHDR_DONE) {
			my samplesPlayed = my numberOfSamples;
			return flush ();
  		} else {
  			static double previousTime = 0.0;
  			const double currentTime = Melder_clock ();
  			if (Melder_debug == 1) {
				my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
  			} else {
	  			MMTIME mmtime;
	  			mmtime. wType = TIME_BYTES;
	  			waveOutGetPosition (my hWaveOut, & mmtime, sizeof (MMTIME));
				my samplesPlayed = mmtime. u.cb / (2 * my numberOfChannels);
			}
			if (/* Melder_debug != 4 || */ currentTime - previousTime > 0.01) {
				previousTime = currentTime;
				if (my callback && ! my callback (my closure, my samplesPlayed))
					return flush ();
			}
			Sleep (10);
  		}
	#endif
	}
	(void) closure;
	return false;
}
#if gtk
	static gint workProc_gtk (gpointer closure) {
		return ! workProc ((void *) closure);
	}
#elif motif
	static bool workProc_motif (XtPointer closure) {
		return workProc ((void *) closure);
	}
#elif cocoa
	static void workProc_cocoa (CFRunLoopTimerRef timer, void *closure) {
		bool result = workProc (closure);
		if (result) {
			CFRunLoopTimerInvalidate (timer);
			//CFRunLoopRemoveTimer (CFRunLoopGetCurrent (), timer);
		}
	}
#endif

static int thePaStreamCallback (const void *input, void *output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	(void) input;
	(void) timeInfo;
	(void) userData;
	struct MelderPlay *me = & thePlay;
	if (my volatile_interrupted) {
		memset (output, '\0', 2 * frameCount * my numberOfChannels);
		my samplesPlayed = my numberOfSamples;
		return my supports_paComplete ? paComplete : paContinue;
	}
	if (statusFlags & paOutputUnderflow) {
		if (Melder_debug == 20) Melder_casual (U"output underflow");
	}
	if (statusFlags & paOutputOverflow) {
		if (Melder_debug == 20) Melder_casual (U"output overflow");
	}
	if (my samplesLeft > 0) {
		const integer dsamples = Melder_clippedRight ((integer) frameCount, my samplesLeft);
		if (Melder_debug == 20) Melder_casual (U"play ", dsamples, U" ", Pa_GetStreamCpuLoad (my stream));
		memset (output, '\0', 2 * frameCount * my numberOfChannels);
		Melder_assert (my playBuffer);
		memcpy (output, (char *) & my playBuffer [my samplesSent * my numberOfChannels], 2 * dsamples * my numberOfChannels);
		my samplesLeft -= dsamples;
		my samplesSent += dsamples;
		my samplesPlayed = my samplesSent;
	} else /*if (my samplesPlayed >= my numberOfSamples)*/ {
		memset (output, '\0', 2 * frameCount * my numberOfChannels);
		//OSMemoryBarrier();
		my samplesPlayed = my numberOfSamples;
		trace (U"paComplete");
		return my supports_paComplete ? paComplete : paContinue;
	}
	return paContinue;
}

#ifdef HAVE_PULSEAUDIO
void pulseAudio_initialize () {
	struct MelderPlay *me = & thePlay;
	if (! my pulseAudio.pulseAudioInitialized) {
		my pulseAudio.mainloop = pa_threaded_mainloop_new ();
		if (! my pulseAudio.mainloop) {
			MelderAudio_isPlaying = false;
			Melder_throw (U"No connection to pulse audio server.");
		}
		pa_threaded_mainloop_start (my pulseAudio.mainloop);
		pa_threaded_mainloop_lock (my pulseAudio.mainloop);
		my pulseAudio.mainloop_api = pa_threaded_mainloop_get_api (my pulseAudio.mainloop);
		my pulseAudio.context = pa_context_new (my pulseAudio.mainloop_api, "praat");
		pa_context_set_state_callback (my pulseAudio.context, context_state_cb, me);
		pa_context_connect (my pulseAudio.context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
		trace (U"threading");
		my pulseAudio.pulseAudioInitialized = true;
		pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
	}
}

void pulseAudio_cleanup () {
	struct MelderPlay *me = & thePlay;

	pa_threaded_mainloop_lock (my pulseAudio.mainloop);
	trace (U"mainloop to be freed, occupation was ", my pulseAudio.occupation);

	if (my pulseAudio.context) {
		trace (U"still context");
		pa_context_unref (my pulseAudio.context);
		my pulseAudio.context = nullptr;
	}
	if (my pulseAudio.timer_event) {
		my pulseAudio.mainloop_api -> time_free (my pulseAudio.timer_event);
		my pulseAudio.timer_event = nullptr;
	}
	pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
	pa_threaded_mainloop_free (my pulseAudio.mainloop); // automatically frees the mainloop_api too!
	my pulseAudio.mainloop = nullptr;
	my pulseAudio.mainloop_api = nullptr;
	my pulseAudio.r_usec = 0;
	MelderAudio_isPlaying = false;
	my pulseAudio.occupation = 0;
	my pulseAudio.startTime = {0, 0};
	my pulseAudio.pulseAudioInitialized = false;
}

void pulseAudio_server_info_cb (pa_context *context, const pa_server_info *info, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;
	if (! info) {
		return;
	}
	const pa_sample_spec *sp = & (info -> sample_spec);
	my numberOfChannels = sp -> channels;
	Melder_assert (context == my pulseAudio.context);
	if ((my pulseAudio.occupation & PA_QUERY_NUMBEROFCHANNELS) != PA_QUERY_NUMBEROFCHANNELS) {
		MelderInfo_open ();
		MelderInfo_writeLine (U"PulseAudio Server characteristics:");
		MelderInfo_writeLine (U"User name: ", Melder_peek8to32_u (info -> user_name));
		MelderInfo_writeLine (U"Host name: ", Melder_peek8to32_u (info -> host_name));
		MelderInfo_writeLine (U"Server version: ", Melder_peek8to32_u (info -> server_version));
		MelderInfo_writeLine (U"Server name: ", Melder_peek8to32_u (info -> server_name));
		MelderInfo_writeLine (U"Sample specification: ");
			MelderInfo_writeLine (U"\tNumber of channels: ", my numberOfChannels);
			MelderInfo_writeLine (U"\tSampling frequency: ", sp -> rate);
			const char *sample_format_string = pa_sample_format_to_string (sp -> format);
			MelderInfo_writeLine (U"\tSample format: ", Melder_peek8to32_u (sample_format_string));
		MelderInfo_writeLine (U"Default sink name: ", Melder_peek8to32_u (info -> default_sink_name));
		MelderInfo_writeLine (U"Default source name: ", Melder_peek8to32_u (info -> default_source_name));
		const pa_channel_map *cm = &(info -> channel_map);
		MelderInfo_writeLine (U"Channel specification: ");
		for (integer channel = 1; channel <= cm -> channels; channel++) {
			const char *channel_text = pa_channel_position_to_pretty_string (cm -> map[channel - 1]); // 0-..
			MelderInfo_writeLine (U"\t Channel ", channel, U": ", Melder_peek8to32_u (channel_text));
		}
		MelderInfo_close ();
	}
	trace (U"before signal");
	my pulseAudio.occupation |= PA_GETTINGINFO_DONE;
	 // We are done, signal it to pulseAudio_serverReport
	pa_threaded_mainloop_signal (my pulseAudio.mainloop, 0);
}

void pulseAudio_serverReport () {
	// TODO: initialize context
	struct MelderPlay *me = & thePlay;
	if (my pulseAudio.mainloop) {
		pa_threaded_mainloop_lock (my pulseAudio.mainloop);
		my pulseAudio.occupation |= PA_GETTINGINFO;
		if (my pulseAudio.context) {
			pa_operation *operation = pa_context_get_server_info (my pulseAudio.context, pulseAudio_server_info_cb, me);
			trace (U"operation started");
			if (! operation) {
				Melder_throw (U"pulseAudioServer report: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
			}
			while ((my pulseAudio.occupation & PA_GETTINGINFO_DONE) != PA_GETTINGINFO_DONE) {
				pa_threaded_mainloop_wait (my pulseAudio.mainloop);
			}
			// Now it is save to unref because the server info operation has completed
			pa_operation_unref (operation);
			my pulseAudio.occupation &= ~ PA_GETTINGINFO_DONE;
		}
		my pulseAudio.occupation &= ~ PA_GETTINGINFO;
		pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
	} else {
		my pulseAudio.occupation |= PA_GETTINGINFO;
		pulseAudio_initialize ();
		/*
			First acquire a lock because the operation to get server info (in context_state_cb) may not have started yet.
			We therefore use our own signaling and wait until information has been acquired.
		 */
		pa_threaded_mainloop_lock (my pulseAudio.mainloop);

		while ((my pulseAudio.occupation & PA_GETTINGINFO_DONE) != PA_GETTINGINFO_DONE) {
			pa_threaded_mainloop_wait (my pulseAudio.mainloop);
		}
		// Now we know that the operation to get server info has succeeded!
		pa_operation_unref (my pulseAudio.operation_info);
		my pulseAudio.operation_info = nullptr;
		my pulseAudio.occupation &= ~ PA_GETTINGINFO;
		my pulseAudio.occupation &= ~ PA_GETTINGINFO_DONE;
		pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
		if (! MelderAudio_isPlaying) {
			my pulseAudio.occupation = 0;
			trace (U"before cleanup");
			pulseAudio_cleanup ();
		}
	}
}

void context_drain_complete_cb (pa_context *context, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;
	Melder_assert (context == my pulseAudio.context);
	if (context) {
		trace (U"context exists");
		pa_context_disconnect (my pulseAudio.context);
		pa_context_unref (my pulseAudio.context);
		my pulseAudio.context = nullptr;
	}
	trace (U"after context test");
	pulseAudio_cleanup ();
}

void stream_drain_complete_cb (pa_stream *stream, int success, void *userdata) {
	(void) stream;
	struct MelderPlay *me = (struct MelderPlay *) userdata;
	// Melder_assert (stream == my pulseAudio.stream); // not always true

	trace (U"succes = ", success);
	if (stream == my pulseAudio.stream) {
		trace (U"stream exists");
		pa_stream_disconnect (my pulseAudio.stream);
		pa_stream_unref (my pulseAudio.stream);
		my pulseAudio.stream = nullptr;
		my pulseAudio.occupation &= ~ PA_WRITING;
		my pulseAudio.occupation |= PA_WRITING_DONE;
		MelderAudio_isPlaying = false;
		if (my pulseAudio.timer_event) {
			my pulseAudio.mainloop_api -> time_free (my pulseAudio.timer_event);
			my pulseAudio.timer_event = nullptr;
		}
		pa_threaded_mainloop_signal (my pulseAudio.mainloop, 0);
	} else {
		trace (U"??");
	}
}

static void free_cb (void * /* p */) { // to prevent copying of data
}

// asynchronous version
void stream_write_cb2 (pa_stream *stream, size_t length, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;
	if (stream == my pulseAudio.stream) {
		//length = my pulseAudio.latency; // overrule length given by server
		integer writeSize_samples = length / (2 * my numberOfChannels);
		my samplesLeft = my numberOfSamples - my samplesSent;
		trace (U"length = ", length, U" left = ", my samplesLeft);
		MelderAudio_isPlaying = true;
		if (my samplesLeft == my numberOfSamples) {
			pa_gettimeofday (& my pulseAudio.startTime);
		}
		if (my samplesLeft > 0) {
			if (my volatile_interrupted) {
				my samplesPlayed = my numberOfSamples;
				my samplesLeft = 0;
				flush ();
			} else {
				writeSize_samples = my samplesLeft < writeSize_samples ? my samplesLeft : writeSize_samples;
				if (pa_stream_write (stream, my playBuffer + my samplesSent * my numberOfChannels, 2 * writeSize_samples * my numberOfChannels, free_cb, 0, PA_SEEK_RELATIVE) < 0) {
					 Melder_throw (U"pa_stream_write() failed: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
				}
				integer samplesSent = writeSize_samples;
				my samplesSent += samplesSent;
				my samplesPlayed = my samplesSent; // not true: use timer info
				trace (U"written ", samplesSent, U" (samples), total ", my samplesSent);
				if (my samplesSent == my numberOfSamples) {
					// my samplesLeft = 0; not here because still playing
					trace (U"nothing left 1");
					//pa_stream_set_write_callback(my pulseAudio.stream, nullptr, nullptr);
					my pulseAudio.operation_drain = pa_stream_drain (my pulseAudio.stream, stream_drain_complete_cb, me);
					if (! my pulseAudio.operation_drain) {
						pa_stream_disconnect (my pulseAudio.stream);
						pa_stream_unref (my pulseAudio.stream);
						my pulseAudio.stream = nullptr;
						trace (U"stream exists");
						my pulseAudio.occupation &= ~ PA_WRITING;
						my pulseAudio.occupation |= PA_WRITING_DONE;
						pa_threaded_mainloop_signal (my pulseAudio.mainloop, 0);
						if (my pulseAudio.timer_event) {
							my pulseAudio.mainloop_api -> time_free (my pulseAudio.timer_event);
							my pulseAudio.timer_event = nullptr;
						}
					}
				}
			}
		} else {
			trace (U"nothing left");
		}
	} else {
		trace (U"stream?");
	}
}

void stream_write_cb (pa_stream *stream, size_t length, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;
	if (stream == my pulseAudio.stream) {
		my samplesLeft = my numberOfSamples - my samplesSent;
		trace (U"length = ", length, U", left = ", my samplesLeft);
		if (my samplesLeft > 0) {
			if (my samplesLeft == my numberOfSamples) {
				pa_gettimeofday (& my pulseAudio.startTime);
			}
			if (my volatile_interrupted) {
				my samplesPlayed = my numberOfSamples;
				my samplesLeft = 0;
				flush ();
			} else {
				int16 *pa_buffer; // an internal pa buffer to minimize the number of memory transfers between client and server
				size_t nbytes_left = my samplesLeft * my numberOfChannels * 2; // how many bytes do we still need to write ?
				size_t nbytes = nbytes_left;
				// returns the address of an internal buffer and the number of bytes that can maximally be written to it.
				if (pa_stream_begin_write (stream, (void **) & pa_buffer, & nbytes) < 0) {
					trace (U"error: no memory");
					Melder_throw (U"Pulseaudio has no internal memory left.");
				}
				trace (U"buffer size = ", nbytes);
				// do we need the full buffer space ?
				nbytes = nbytes <= nbytes_left ? nbytes : nbytes_left;
				memcpy (pa_buffer, (void *) (my playBuffer + my samplesSent * my numberOfChannels), nbytes);
				//memset (pa_buffer, 0, nbytes);
				
				if (pa_stream_write (stream, pa_buffer, nbytes, nullptr, 0, PA_SEEK_RELATIVE) < 0) {
					 Melder_throw (U"pa_stream_write() failed: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
				}
				Melder_assert (nbytes % (my numberOfChannels * 2) == 0);
				integer samplesSent = nbytes / (my numberOfChannels * 2);
				my samplesSent += samplesSent;
				my samplesPlayed = my samplesSent; // not true: use timer info
				trace (U"written ", samplesSent, U" (samples), total ", my samplesSent, U", address = ", (integer) pa_buffer);
				if (my samplesSent == my numberOfSamples) {
					// my samplesLeft = 0; not here because still playing
					trace (U"nothing left 1");
					pa_stream_set_write_callback (my pulseAudio.stream, nullptr, nullptr);
					my pulseAudio.operation_drain = pa_stream_drain (my pulseAudio.stream, stream_drain_complete_cb, me);
					if (! my pulseAudio.operation_drain) {
						pa_stream_disconnect (my pulseAudio.stream);
						pa_stream_unref (my pulseAudio.stream);
						my pulseAudio.stream = nullptr;
						trace (U"stream exists");
						my pulseAudio.occupation &= ~ PA_WRITING;
						my pulseAudio.occupation |= PA_WRITING_DONE;
						pa_threaded_mainloop_signal (my pulseAudio.mainloop, 0);
						if (my pulseAudio.timer_event) {
							my pulseAudio.mainloop_api -> time_free (my pulseAudio.timer_event);
							my pulseAudio.timer_event = nullptr;
						}
					}
				}
			}
		} else {
			trace (U"nothing left");
		}
	} else {
		trace (U"stream?");
	}
}

void stream_state_cb (pa_stream *stream, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;

	if (stream == my pulseAudio.stream) {
		switch (pa_stream_get_state (stream)) {
			case PA_STREAM_UNCONNECTED:
				trace (U"PA_STREAM_UNCONNECTED");
				break;
			case PA_STREAM_CREATING:
				trace (U"PA_STREAM_CREATING");
				break;
			case PA_STREAM_TERMINATED:
				trace (U"PA_STREAM_TERMINATED");
				break;
			case PA_STREAM_READY: {
				const pa_buffer_attr* buffer_attr = pa_stream_get_buffer_attr (my pulseAudio.stream);
				trace (U"PA_STREAM_READY : tlength = ", buffer_attr -> tlength, U", prebuf = ", buffer_attr -> prebuf, U", maxlength = ", buffer_attr -> maxlength);
				break;
			}
			default:
				trace (U"default");
				break;
			case PA_STREAM_FAILED:
				trace (U"PA_STREAM_FAILED");
				break;
		}
	}
}

static void stream_underflow_callback (pa_stream *s, void *userdata) {
	(void) s;
	(void) userdata;
	trace (U"yes");
}

static void stream_overflow_callback (pa_stream *s, void *userdata) {
	(void) s;
	(void) userdata;
	trace (U"yes");
}

void prepare_and_play (struct MelderPlay *me) {
	my pulseAudio.sample_spec.format = ( Melder_integersAreBigEndian () ? PA_SAMPLE_S16BE : PA_SAMPLE_S16LE );
	my samplesPlayed = my samplesSent = 0;
	my pulseAudio.sample_spec.rate = my sampleRate;
	my pulseAudio.sample_spec.channels = my numberOfChannels;
	my pulseAudio.stream = pa_stream_new (my pulseAudio.context, "Praat", &(my pulseAudio.sample_spec), nullptr);
	if (! my pulseAudio.stream) {
		Melder_throw (U"Cannot connect to sound server: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
	}

	//my pulseAudio.stream_flags = PA_STREAM_NOFLAGS;
	my pulseAudio.stream_flags = (pa_stream_flags_t) (PA_STREAM_ADJUST_LATENCY | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_AUTO_TIMING_UPDATE);
	my pulseAudio.latency = pa_usec_to_bytes (1000 * my pulseAudio.latency_msec, & my pulseAudio.sample_spec);
	
	my pulseAudio.buffer_attr.maxlength = (uint32) -1;
	my pulseAudio.buffer_attr.prebuf = (uint32) -1;
	my pulseAudio.buffer_attr.minreq = (uint32) -1;
	my pulseAudio.buffer_attr.prebuf = (uint32) -1;
	my pulseAudio.buffer_attr.tlength = my pulseAudio.latency;
	my pulseAudio.buffer_attr.fragsize = (uint32) -1;

	pa_stream_set_state_callback (my pulseAudio.stream, stream_state_cb, me);
	pa_stream_set_write_callback (my pulseAudio.stream, stream_write_cb, me);
	pa_stream_set_underflow_callback (my pulseAudio.stream, stream_underflow_callback, me);
	pa_stream_set_overflow_callback (my pulseAudio.stream, stream_overflow_callback, me);
	
	// BUG in pa 5.9  & 6.0: the server doesn't honour our wish for a reasonable tlength. Instead it uses some
	//   ridiculously low value for tlength that will constantly give underflows.
	//   audio playback was completely shit if in the stream_write_cb we used pa_stream_write instead of
	// first pa_stream_begin_write followed by pa_stream_write.
	//
	my pulseAudio.stream_flags = PA_STREAM_NOFLAGS;
	if (pa_stream_connect_playback (my pulseAudio.stream, nullptr, & my pulseAudio.buffer_attr, my pulseAudio.stream_flags, nullptr, nullptr) < 0) {
		Melder_throw (U"pa_stream_connect_playback() failed: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
	}
	trace (U"tlength = ", my pulseAudio.buffer_attr.tlength, U", channels = ", my numberOfChannels);
}

void context_state_cb (pa_context *context, void *userdata) {
	struct MelderPlay *me = (struct MelderPlay *) userdata;

	pa_context_state context_state = pa_context_get_state (context);
	switch (context_state) {
		case PA_CONTEXT_UNCONNECTED:
			trace (U"PA_CONTEXT_UNCONNECTED");
			break;
		case PA_CONTEXT_CONNECTING:
			trace (U"PA_CONTEXT_CONNECTING");
			break;
		case PA_CONTEXT_AUTHORIZING:
			trace (U"PA_CONTEXT_AUTHORIZING");
			break;
		case PA_CONTEXT_SETTING_NAME:
			trace (U"PA_CONTEXT_SETTING_NAME");
			break;
		case PA_CONTEXT_FAILED:
			trace (U"PA_CONTEXT_FAILED");
			break;
		case PA_CONTEXT_TERMINATED:
			trace (U"PA_CONTEXT_TERMINATED");
			break;
		case PA_CONTEXT_READY: {
			trace (U"PA_CONTEXT_READY");
			if ((my pulseAudio.occupation & PA_GETTINGINFO) == PA_GETTINGINFO) {
				my pulseAudio.operation_info = pa_context_get_server_info (my pulseAudio.context, pulseAudio_server_info_cb, me);
				trace (U"operation started");
				if (! my pulseAudio.operation_info) {
					Melder_throw (U"pulseAudioServer report: ", Melder_peek8to32_u (pa_strerror (pa_context_errno (my pulseAudio.context))));
				}
			} else if ((my pulseAudio.occupation & PA_WRITING) == PA_WRITING) {
				prepare_and_play (me);
			} else if ((my pulseAudio.occupation & PA_RECORDING) == PA_RECORDING) {
				
			}
			break;
		}
		default:
			trace (U"default");
			break;
	}
}
#endif

static bool deviceHasChanged = false;
#if defined (macintosh)
static int theCoreaudioPropertyListener (unsigned int, unsigned int, const AudioObjectPropertyAddress * _Nonnull, void * _Nullable) {
	Melder_casual (U"coreaudio_property_listener");
	deviceHasChanged = true;
	return 0;
}
#endif

static bool interruptedByEscapeKey () {
	#if defined(_WIN32)
		MSG event;
		BOOL messageAvailable = PeekMessage (& event, nullptr, 0, 0, PM_REMOVE);
		if (messageAvailable) {
			trace (U"Windows message: ", event.message);
			if (event. message == WM_KEYDOWN) {
				trace (U"Windows key: ", event. wParam);
				if (LOWORD (event. wParam) == VK_ESCAPE)
					return true;
			} else if (event. message == WM_LBUTTONDOWN
				|| event.message == WM_NCLBUTTONDOWN)
			{
				// ignore all mouse events
			} else {
				// handle all other events, such as Chinese keyboard events
				XtDispatchEvent (& event);
				//TranslateMessage (& event);
				//DispatchMessage (& event);
			}
		}
	#endif
	return false;
}

void MelderAudio_play16 (int16 *buffer, integer sampleRate, integer numberOfSamples, integer numberOfChannels,
	bool (*playCallback) (void *playClosure, integer numberOfSamplesPlayed), void *playClosure)
{
	#if defined (macintosh)
	{// scope
		static bool inited;
		if (! inited) {
			OSStatus err = noErr;
			AudioObjectPropertyAddress audioDevicesAddress = { kAudioHardwarePropertyDefaultOutputDevice, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
			err = AudioObjectAddPropertyListener ( kAudioObjectSystemObject, & audioDevicesAddress, theCoreaudioPropertyListener, NULL);
			if (err)
				Melder_casual (U"error on AudioObjectAddPropertyListener");
			inited = true;
		}
	}
	#endif

	struct MelderPlay *me = & thePlay;
	#ifdef _WIN32
		bool wasPlaying = MelderAudio_isPlaying;
	#endif
	if (MelderAudio_isPlaying)
		MelderAudio_stopPlaying (MelderAudio_IMPLICIT);   // otherwise, keep "explicitStop" tag
	#ifdef HAVE_PULSEAUDIO
		if (my usePulseAudio && my pulseAudio.mainloop) {
			pulseAudio_cleanup ();
		}
	#endif
	my playBuffer = buffer;   // 0-based, as all buffers are
	static int16 *playBuffer2 = nullptr;   // in case we have to redistribute up (not ideal)
	my sampleRate = sampleRate;
	my numberOfSamples = numberOfSamples;
	my numberOfChannels = numberOfChannels;
	my callback = playCallback;
	my closure = playClosure;
	my asynchronicity =
		Melder_batch ? kMelder_asynchronicityLevel::SYNCHRONOUS :
		(Melder_backgrounding && ! Melder_asynchronous) ? kMelder_asynchronicityLevel::INTERRUPTABLE :
		kMelder_asynchronicityLevel::ASYNCHRONOUS;
	if (my asynchronicity > preferences. maximumAsynchronicity)
		my asynchronicity = preferences. maximumAsynchronicity;
	trace (U"asynchronicity ", (int) my asynchronicity);
	my usePortAudio =
		#if defined (_WIN32)
			preferences. outputSoundSystem == kMelder_outputSoundSystem::MME_VIA_PORTAUDIO;
		#elif defined (macintosh)
			preferences. outputSoundSystem == kMelder_outputSoundSystem::COREAUDIO_VIA_PORTAUDIO;
		#elif defined (raspberrypi)
			preferences. outputSoundSystem == kMelder_outputSoundSystem::JACK_VIA_PORTAUDIO;
		#else
			preferences. outputSoundSystem == kMelder_outputSoundSystem::ALSA_OR_JACK_VIA_PORTAUDIO;
		#endif
	my usePulseAudio = ! my usePortAudio;

	my explicitStop = MelderAudio_IMPLICIT;
	my volatile_interrupted = 0;

	my samplesLeft = numberOfSamples;
	my samplesSent = 0;
	my samplesPlayed = 0;
	MelderAudio_isPlaying = true;
	if (my usePortAudio) {
		PaError err;
		if (deviceHasChanged) {
			Pa_Terminate ();
			Pa_Initialize ();
			deviceHasChanged = false;
		}
		if (! MelderAudio_hasBeenInitialized) {
			err = Pa_Initialize ();
			if (err)
				Melder_crash (U"PortAudio does not initialize: ", Melder_peek8to32_u (Pa_GetErrorText (err)));
			MelderAudio_hasBeenInitialized = true;
		}
		my supports_paComplete = Pa_GetHostApiInfo (Pa_GetDefaultHostApi ()) -> type != paDirectSound &&0;
		PaStreamParameters outputParameters = { 0 };
		outputParameters. device = Pa_GetDefaultOutputDevice ();
		if (outputParameters. device == paNoDevice)
			Melder_throw (U"PortAudio sees no default output device.\nCannot play sound.");
		const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo (outputParameters. device);
		if (! deviceInfo)
			Melder_throw (U"PortAudio finds no info for device ", outputParameters. device, U".\nCannot play sound.");
		//Melder_casual (U"MelderAudio_play16: ", Melder_peek8to32_u (deviceInfo -> name));
		trace (U"the device can handle ", deviceInfo -> maxOutputChannels, U" channels");
		Melder_clipRight (& my numberOfChannels, integer (deviceInfo -> maxOutputChannels));
		if (numberOfChannels > my numberOfChannels) {
			/*
				Redistribute the in channels over the out channels.
			*/
			if (numberOfChannels == 4 && my numberOfChannels == 2) {   // a common case
				int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
					integer in1 = *in ++, in2 = *in ++, in3 = *in ++, in4 = *in ++;
					*out ++ = (in1 + in2) / 2;
					*out ++ = (in3 + in4) / 2;
				}
			} else {
				int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
					for (integer iout = 1; iout <= my numberOfChannels; iout ++) {
						integer outValue = 0;
						integer numberOfIn = numberOfChannels / my numberOfChannels;
						if (iout == my numberOfChannels)
							numberOfIn += numberOfChannels % my numberOfChannels;
						for (integer iin = 1; iin <= numberOfIn; iin ++)
							outValue += *in ++;
						outValue /= numberOfIn;
						*out ++ = outValue;
					}
				}
			}
		}
		outputParameters. channelCount = my numberOfChannels;
		outputParameters. sampleFormat = paInt16;
		outputParameters. suggestedLatency = deviceInfo -> defaultLowOutputLatency;
		outputParameters. hostApiSpecificStreamInfo = nullptr;
		err = Pa_OpenStream (& my stream, nullptr, & outputParameters, my sampleRate, paFramesPerBufferUnspecified,
			paDitherOff, thePaStreamCallback, me);
		if (err)
			Melder_throw (U"PortAudio cannot open sound output: ", Melder_peek8to32_u (Pa_GetErrorText (err)), U".");
		theStartingTime = Melder_clock ();
		err = Pa_StartStream (my stream);
		if (err)
			Melder_throw (U"PortAudio cannot start sound output: ", Melder_peek8to32_u (Pa_GetErrorText (err)), U".");
		my paStartingTime = Pa_GetStreamTime (my stream);
		if (my asynchronicity <= kMelder_asynchronicityLevel::INTERRUPTABLE) {
			for (;;) {
				#if defined (linux)
					/*
						This is how PortAudio was designed to work.
					*/
					if (my samplesLeft == 0) {
						my samplesPlayed = my numberOfSamples;
						break;
					}
				#else
					/*
						A version that doesn't trust that the stream callback will complete.
					*/
					const double timeElapsed = Melder_clock () - theStartingTime - Pa_GetStreamInfo (my stream) -> outputLatency;
					const integer samplesPlayed = (integer) floor (timeElapsed * my sampleRate);
					if (samplesPlayed >= my numberOfSamples + my sampleRate / 20) {
						my samplesPlayed = my numberOfSamples;
						break;
					}
				#endif
				bool interrupted = false;
				if (my asynchronicity != kMelder_asynchronicityLevel::SYNCHRONOUS && my callback &&
					! my callback (my closure, my samplesPlayed))
					interrupted = true;
				/*
					Safe operation: only listen to key-down events.
					Do this on the lowest level that will work.
				*/
				if (my asynchronicity == kMelder_asynchronicityLevel::INTERRUPTABLE && ! interrupted) {
					#if gtk
						// TODO: implement a reaction to the Escape key
					#elif cocoa
						// TODO: implement a reaction to the Escape key
					#elif defined (macintosh)
						EventRecord event;
						if (EventAvail (keyDownMask, & event)) {
							/*
								Remove the event, even if it was a different key.
								Otherwise, the key will block the future availability of the Escape key.
							*/
							FlushEvents (keyDownMask, 0);
							/*
								Catch Escape and Command-period.
							*/
							if ((event. message & charCodeMask) == 27 ||
								((event. modifiers & cmdKey) && (event. message & charCodeMask) == '.'))
							{
								my explicitStop = MelderAudio_EXPLICIT;
								interrupted = true;
							}
						}
					#elif defined (_WIN32)
						if (interruptedByEscapeKey ()) {
							my explicitStop = MelderAudio_EXPLICIT;
							interrupted = true;
						}
					#endif
				}
				if (interrupted) {
					flush ();
					return;
				}
				Pa_Sleep (10);
			}
			if (my samplesPlayed != my numberOfSamples)
				Melder_crash (U"Played ", my samplesPlayed, U" instead of ", my numberOfSamples, U" samples.");
			#ifndef linux
				Pa_AbortStream (my stream);
			#endif
		} else /* my asynchronicity == kMelder_asynchronicityLevel::ASYNCHRONOUS */ {
			#if gtk
				trace (U"g_idle_add");
				my workProcId_gtk = g_idle_add (workProc_gtk, nullptr);
			#elif motif
				my workProcId_motif = XtAddWorkProc (workProc_motif, nullptr);
			#elif cocoa
				CFRunLoopTimerContext context = { 0, nullptr, nullptr, nullptr, nullptr };
				my cocoaTimer = CFRunLoopTimerCreate (nullptr, CFAbsoluteTimeGetCurrent () + 0.02,
						0.02, 0, 0, workProc_cocoa, & context);
				CFRunLoopAddTimer (CFRunLoopGetCurrent (), my cocoaTimer, kCFRunLoopCommonModes);
			#endif
			return;
		}
		flush ();
		return;
	#ifdef HAVE_PULSEAUDIO
	} else if (my usePulseAudio) {
		my pulseAudio.occupation |= PA_QUERY_NUMBEROFCHANNELS; // set query bit on
		pulseAudio_serverReport ();
		my pulseAudio.occupation &= ~PA_QUERY_NUMBEROFCHANNELS; // set query bit off
		
		if (numberOfChannels > my numberOfChannels) {
			/*
				Redistribute the in channels over the out channels. TODO make channelmap
			*/
			if (numberOfChannels == 4 && my numberOfChannels == 2) {   // a common case
				int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
					integer in1 = *in ++, in2 = *in ++, in3 = *in ++, in4 = *in ++;
					*out ++ = (in1 + in2) / 2;
					*out ++ = (in3 + in4) / 2;
				}
			} else {
				int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
				for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
					for (integer iout = 1; iout <= my numberOfChannels; iout ++) {
						integer outValue = 0;
						integer numberOfIn = numberOfChannels / my numberOfChannels;
						if (iout == my numberOfChannels)
							numberOfIn += numberOfChannels % my numberOfChannels;
						for (integer iin = 1; iin <= numberOfIn; iin ++)
							outValue += *in ++;
						outValue /= numberOfIn;
						*out ++ = outValue;
					}
				}
			}
		} else if (numberOfChannels < my numberOfChannels) {
			// pulseaudio will divide the input over the output channels
			my numberOfChannels = numberOfChannels;
		}
		
		my pulseAudio.occupation |= PA_WRITING;
		pulseAudio_initialize ();
		if (pa_context_get_state (my pulseAudio.context) == PA_CONTEXT_READY) {
			prepare_and_play (me);
		}

		if (my asynchronicity < kMelder_asynchronicityLevel::ASYNCHRONOUS) {
			pa_threaded_mainloop_lock (my pulseAudio.mainloop);
			trace (U"occupation ", my pulseAudio.occupation);
			while ((my pulseAudio.occupation & PA_WRITING_DONE) != PA_WRITING_DONE) {
				trace (U"waiting");
				pa_threaded_mainloop_wait (my pulseAudio.mainloop);
			}
			// Now we know that the stream drain operation has succeeded !
			if (my pulseAudio.operation_drain) {
				pa_operation_unref (my pulseAudio.operation_drain);
			}
			my pulseAudio.operation_drain = nullptr;
			my pulseAudio.occupation &= ~PA_WRITING_DONE;

			pa_threaded_mainloop_unlock (my pulseAudio.mainloop);
			//pa_threaded_mainloop_accept (my pulseAudio.mainloop);
			trace (U"before cleanup");
			pulseAudio_cleanup ();
		} else {
			// my workProcId_gtk = g_timeout_add (ms, workProc_gtk, nullptr);
			// without a timer, on my computer the workproc would be called almost once in every sampling period.
			// Such frequent updates are not necessary, some 50 updates a second is fast enough for displaying a runnning cursor
			// the timeout will be automatically stopped if workProc_gtk returns false.
			#if gtk
				#ifndef NO_GRAPHICS
					my workProcId_gtk = g_timeout_add (20, workProc_gtk, nullptr);
				#endif
			#endif
		}
	#endif
	} else {
		#if defined (macintosh)
		#elif defined (linux) && ! defined (NO_AUDIO)
		#elif defined (_WIN32)
			try {
				WAVEFORMATEX waveFormat;
				MMRESULT err;
				waveFormat. wFormatTag = WAVE_FORMAT_PCM;
				waveFormat. nChannels = my numberOfChannels;
				waveFormat. nSamplesPerSec = my sampleRate;
				waveFormat. wBitsPerSample = 16;
				waveFormat. nBlockAlign = my numberOfChannels * waveFormat. wBitsPerSample / 8;
				waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
				waveFormat. cbSize = 0;
				err = waveOutOpen (& my hWaveOut, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC);
				if (err != MMSYSERR_NOERROR) {
					MelderAudio_isPlaying = false;
					if (err == MMSYSERR_ALLOCATED)
						Melder_throw (U"Previous sound is still playing? Should not occur!\n"
							U"Report bug to the author: ", err, U"; wasPlaying: ", wasPlaying, U".");
					if (err == MMSYSERR_BADDEVICEID)
						Melder_throw (U"Cannot play a sound. Perhaps another program is playing a sound at the same time?");
					if (err == MMSYSERR_NODRIVER)
						Melder_throw (U"This computer probably has no sound card.");
					if (err == MMSYSERR_NOMEM)
						Melder_throw (U"Not enough free memory to play any sound at all.");
					if (err == WAVERR_BADFORMAT) {
						if (my numberOfChannels > 2) {
							/*
							 * Retry with 2 channels.
							 */
							my numberOfChannels = 2;
							waveFormat. nChannels = my numberOfChannels;
							waveFormat. nBlockAlign = my numberOfChannels * waveFormat. wBitsPerSample / 8;
							waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
							err = waveOutOpen (& my hWaveOut, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC);
							if (err != MMSYSERR_NOERROR)
								Melder_throw (U"Bad sound format even after reduction to 2 channels? Should not occur! Report bug to the author!");
							MelderAudio_isPlaying = true;
						} else if (my numberOfChannels == 1) {
							/*
							 * Retry with 2 channels.
							 */
							my numberOfChannels = 2;
							waveFormat. nChannels = my numberOfChannels;
							waveFormat. nBlockAlign = my numberOfChannels * waveFormat. wBitsPerSample / 8;
							waveFormat. nAvgBytesPerSec = waveFormat. nBlockAlign * waveFormat. nSamplesPerSec;
							err = waveOutOpen (& my hWaveOut, WAVE_MAPPER, & waveFormat, 0, 0, CALLBACK_NULL | WAVE_ALLOWSYNC);
							if (err != MMSYSERR_NOERROR)
								Melder_throw (U"Bad sound format even after expansion to 2 channels? Should not occur! Report bug to the author!");
							MelderAudio_isPlaying = true;
						} else {
							Melder_throw (U"Bad sound format? Should not occur! Report bug to the author!");
						}
					} else {
						Melder_throw (U"Unknown error ", err, U" while trying to play a sound? Report bug to the author!");
					}
				}
				if (numberOfChannels > my numberOfChannels) {
					/*
						Redistribute the in channels over the out channels.
					*/
					if (numberOfChannels == 4 && my numberOfChannels == 2) {   // a common case
						int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
						for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
							integer in1 = *in ++, in2 = *in ++, in3 = *in ++, in4 = *in ++;
							*out ++ = (in1 + in2) / 2;
							*out ++ = (in3 + in4) / 2;
						}
					} else {
						int16 *in = & my playBuffer [0], *out = & my playBuffer [0];
						for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
							for (integer iout = 1; iout <= my numberOfChannels; iout ++) {
								integer outValue = 0;
								integer numberOfIn = numberOfChannels / my numberOfChannels;
								if (iout == my numberOfChannels)
									numberOfIn += numberOfChannels % my numberOfChannels;
								for (integer iin = 1; iin <= numberOfIn; iin ++)
									outValue += *in ++;
								outValue /= numberOfIn;
								*out ++ = outValue;
							}
						}
					}
				} else if (numberOfChannels == 1 && my numberOfChannels == 2) {
					if (playBuffer2)
						Melder_free (playBuffer2);
					playBuffer2 = Melder_malloc (int16, numberOfSamples * my numberOfChannels);
					Melder_assert (playBuffer2);
					int16 *in = & my playBuffer [0], *out = & playBuffer2 [0];
					for (integer isamp = 1; isamp <= numberOfSamples; isamp ++) {
						const int16 inValue = *in ++;
						*out ++ = inValue;
						*out ++ = inValue;
					}
				}

				my waveHeader. dwFlags = 0;
				my waveHeader. lpData =
						( numberOfChannels == 1 && my numberOfChannels == 2 ? (char *) playBuffer2 : (char *) my playBuffer );
				my waveHeader. dwBufferLength = my numberOfSamples * 2 * my numberOfChannels;
				my waveHeader. dwLoops = 1;
				my waveHeader. lpNext = nullptr;
				my waveHeader. reserved = 0;
				err = waveOutPrepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
			//waveOutReset (my hWaveOut);
				if (err != MMSYSERR_NOERROR) {
					waveOutClose (my hWaveOut), my hWaveOut = 0;
					MelderAudio_isPlaying = false;
					if (err == MMSYSERR_INVALHANDLE)
						Melder_throw (U"No device? Should not occur!");
					if (err == MMSYSERR_NODRIVER)
						Melder_throw (U"No driver? Should not occur!");
					if (err == MMSYSERR_NOMEM)
						Melder_throw (U"Not enough free memory to play this sound.\n"
							U"Remove some objects, play a shorter sound, or buy more memory.");
					Melder_throw (U"Unknown error ", err, U" while preparing header? Should not occur!");
				}

				err = waveOutWrite (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
				if (err != MMSYSERR_NOERROR) {
					waveOutReset (my hWaveOut);
					waveOutUnprepareHeader (my hWaveOut, & my waveHeader, sizeof (WAVEHDR));
					waveOutClose (my hWaveOut), my hWaveOut = 0;
					MelderAudio_isPlaying = false;
					Melder_throw (U"Error ", err, U" while writing audio output.");   // BUG: should flush
				}

				theStartingTime = Melder_clock ();
				if (my asynchronicity == kMelder_asynchronicityLevel::SYNCHRONOUS) {
					while (! (my waveHeader. dwFlags & WHDR_DONE)) {
						Sleep (10);
					}
					my samplesPlayed = my numberOfSamples;
				} else if (my asynchronicity <= kMelder_asynchronicityLevel::INTERRUPTABLE) {
					while (! (my waveHeader. dwFlags & WHDR_DONE)) {
						Sleep (10);
						my samplesPlayed = (Melder_clock () - theStartingTime) * my sampleRate;
						if (my callback && ! my callback (my closure, my samplesPlayed))
							break;
						if (my asynchronicity == kMelder_asynchronicityLevel::INTERRUPTABLE) {
							if (interruptedByEscapeKey ()) {
								my explicitStop = MelderAudio_EXPLICIT;
								break;
							}
						}
					}
				} else {
					my workProcId_motif = XtAddWorkProc (workProc_motif, nullptr);
					return;
				}
				flush ();
				return;
			} catch (MelderError) {
				Melder_throw (U"16-bit audio not played.");
			}
		#else
			Melder_throw (U"Cannot play a sound on this computer.\n16-bit audio not played.");
		#endif
	}
}

/* End of file melder_audio.cpp */
