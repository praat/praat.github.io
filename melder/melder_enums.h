/* melder_enums.h
 *
 * Copyright (C) 2007,2013,2015,2016,2018,2019,2025,2026 Paul Boersma
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

enums_begin (kMelder_number, 1)
	enums_add (kMelder_number, 1, EQUAL_TO, U"equal to")
	enums_add (kMelder_number, 2, NOT_EQUAL_TO, U"not equal to")
	enums_add (kMelder_number, 3, LESS_THAN, U"less than")
	enums_add (kMelder_number, 4, LESS_THAN_OR_EQUAL_TO, U"less than or equal to")
	enums_add (kMelder_number, 5, GREATER_THAN, U"greater than")
	enums_add (kMelder_number, 6, GREATER_THAN_OR_EQUAL_TO, U"greater than or equal to")
enums_end (kMelder_number, 6, EQUAL_TO)

enums_begin (kMelder_string, 1)
	enums_add (kMelder_string, 1, EQUAL_TO, U"is equal to")
	enums_add (kMelder_string, 2, NOT_EQUAL_TO, U"is not equal to")
	enums_add (kMelder_string, 3, CONTAINS, U"contains")
	enums_add (kMelder_string, 4, DOES_NOT_CONTAIN, U"does not contain")
	enums_add (kMelder_string, 5, STARTS_WITH, U"starts with")
	enums_add (kMelder_string, 6, DOES_NOT_START_WITH, U"does not start with")
	enums_add (kMelder_string, 7, ENDS_WITH, U"ends with")
	enums_add (kMelder_string, 8, DOES_NOT_END_WITH, U"does not end with")
	enums_add (kMelder_string, 9, CONTAINS_WORD, U"contains a word equal to")
	enums_add (kMelder_string, 10, DOES_NOT_CONTAIN_WORD, U"does not contain a word equal to")
	enums_add (kMelder_string, 11, CONTAINS_WORD_STARTING_WITH, U"contains a word starting with")
	enums_add (kMelder_string, 12, DOES_NOT_CONTAIN_WORD_STARTING_WITH, U"does not contain a word starting with")
	enums_add (kMelder_string, 13, CONTAINS_WORD_ENDING_WITH, U"contains a word ending with")
	enums_add (kMelder_string, 14, DOES_NOT_CONTAIN_WORD_ENDING_WITH, U"does not contain a word ending with")
	enums_add (kMelder_string, 15, CONTAINS_INK, U"contains ink equal to")
	enums_add (kMelder_string, 16, DOES_NOT_CONTAIN_INK, U"does not contain ink equal to")
	enums_add (kMelder_string, 17, CONTAINS_INK_STARTING_WITH, U"contains ink starting with")
	enums_add (kMelder_string, 18, DOES_NOT_CONTAIN_INK_STARTING_WITH, U"does not contain ink starting with")
	enums_add (kMelder_string, 19, CONTAINS_INK_ENDING_WITH, U"contains ink ending with")
	enums_add (kMelder_string, 20, DOES_NOT_CONTAIN_INK_ENDING_WITH, U"does not contain ink ending with")
	enums_add (kMelder_string, 21, MATCH_REGEXP, U"matches (regex)")
enums_end (kMelder_string, 21, EQUAL_TO)

enums_begin (kMelder_textInputEncoding, 1)
	enums_add (kMelder_textInputEncoding, 1, UTF8, U"UTF-8")
	enums_add (kMelder_textInputEncoding, 2, UTF8_THEN_ISO_LATIN1, U"try UTF-8, then ISO Latin-1")
	enums_add (kMelder_textInputEncoding, 3, ISO_LATIN1, U"ISO Latin-1")
	enums_add (kMelder_textInputEncoding, 4, UTF8_THEN_WINDOWS_LATIN1, U"try UTF-8, then Windows Latin-1")
	enums_add (kMelder_textInputEncoding, 5, WINDOWS_LATIN1, U"Windows Latin-1")
	enums_add (kMelder_textInputEncoding, 6, UTF8_THEN_MACROMAN, U"try UTF-8, then MacRoman")
	enums_add (kMelder_textInputEncoding, 7, MACROMAN, U"MacRoman")
#if defined (_WIN32)
enums_end (kMelder_textInputEncoding, 7, UTF8_THEN_WINDOWS_LATIN1)
#elif defined (macintosh)
enums_end (kMelder_textInputEncoding, 7, UTF8_THEN_MACROMAN)
#else
enums_end (kMelder_textInputEncoding, 7, UTF8_THEN_ISO_LATIN1)
#endif

enums_begin (kMelder_textOutputEncoding, 1)
	enums_add (kMelder_textOutputEncoding, 1, UTF8, U"UTF-8")
	enums_add (kMelder_textOutputEncoding, 2, UTF16, U"UTF-16")
	enums_add (kMelder_textOutputEncoding, 3, ASCII_THEN_UTF16, U"try ASCII, then UTF-16")
	enums_add (kMelder_textOutputEncoding, 4, ISO_LATIN1_THEN_UTF16, U"try ISO Latin-1, then UTF-16")
enums_end (kMelder_textOutputEncoding, 4, UTF8)

enums_begin (kMelder_asynchronicityLevel, 0)
	enums_add (kMelder_asynchronicityLevel, 0, SYNCHRONOUS, U"synchronous (nothing)")
	enums_add (kMelder_asynchronicityLevel, 1, CALLING_BACK, U"calling back (view running cursor)")
	enums_add (kMelder_asynchronicityLevel, 2, INTERRUPTABLE, U"interruptable (Escape key stops playing)")
	enums_add (kMelder_asynchronicityLevel, 3, ASYNCHRONOUS, U"asynchronous (anything)")
enums_end (kMelder_asynchronicityLevel, 3, ASYNCHRONOUS)

#if defined (_WIN32)

	enums_begin (kMelder_inputSoundSystem, 1)
		enums_add (kMelder_inputSoundSystem, 1, MME_VIA_PORTAUDIO, U"MME via PortAudio")
	enums_end (kMelder_inputSoundSystem, 1, MME_VIA_PORTAUDIO)
			// in order to allow recording for over 64 megabytes (paMME)

	enums_begin (kMelder_outputSoundSystem, 1)
		enums_add (kMelder_outputSoundSystem, 1, MME, U"MME")
		enums_add (kMelder_outputSoundSystem, 2, MME_VIA_PORTAUDIO, U"MME via PortAudio")
	enums_end (kMelder_outputSoundSystem, 2, MME)
			// in order to reduce the long latencies of paMME and to avoid the incomplete implementation of paDirectSound

#elif defined (macintosh)

	enums_begin (kMelder_inputSoundSystem, 1)
		enums_add (kMelder_inputSoundSystem, 1, COREAUDIO_VIA_PORTAUDIO, U"CoreAudio via PortAudio")
	enums_end (kMelder_inputSoundSystem, 1, COREAUDIO_VIA_PORTAUDIO)
			// in order to have CoreAudio

	enums_begin (kMelder_outputSoundSystem, 1)
		enums_add (kMelder_outputSoundSystem, 1, COREAUDIO_VIA_PORTAUDIO, U"CoreAudio via PortAudio")
	enums_end (kMelder_outputSoundSystem, 1, COREAUDIO_VIA_PORTAUDIO)
			// in order to have CoreAudio

#elif defined (raspberrypi)

	enums_begin (kMelder_inputSoundSystem, 1)
		enums_add (kMelder_inputSoundSystem, 1, JACK_VIA_PORTAUDIO, U"JACK via PortAudio")
	enums_end (kMelder_inputSoundSystem, 1, JACK_VIA_PORTAUDIO)
			// in order to use JACK

	enums_begin (kMelder_outputSoundSystem, 1)
		enums_add (kMelder_outputSoundSystem, 1, JACK_VIA_PORTAUDIO, U"JACK via PortAudio")
	enums_end (kMelder_outputSoundSystem, 1, JACK_VIA_PORTAUDIO)
			// in order to use JACK

#else

	enums_begin (kMelder_inputSoundSystem, 1)
		enums_add (kMelder_inputSoundSystem, 1, ALSA_OR_JACK_VIA_PORTAUDIO, U"ALSA or JACK via PortAudio")
		enums_alt (kMelder_inputSoundSystem,    ALSA_OR_JACK_VIA_PORTAUDIO, U"ALSA via PortAudio")
			// in order to use ALSA and therefore be compatible with Ubuntu 10.10 and later
			// JACK compatible with PipeWire
	enums_end (kMelder_inputSoundSystem, 1, ALSA_OR_JACK_VIA_PORTAUDIO)

	enums_begin (kMelder_outputSoundSystem, 1)
		enums_add (kMelder_outputSoundSystem, 1, PULSEAUDIO, U"PulseAudio")   // compatible with PipeWire
		enums_add (kMelder_outputSoundSystem, 2, ALSA_OR_JACK_VIA_PORTAUDIO, U"ALSA or JACK via PortAudio")
		enums_alt (kMelder_outputSoundSystem,    ALSA_OR_JACK_VIA_PORTAUDIO, U"ALSA via PortAudio")
			// JACK compatible with PipeWire
	enums_end (kMelder_outputSoundSystem, 2, PULSEAUDIO)
			// because we prefer to try PulseAudio directly

#endif

/* End of file melder_enums.h */
