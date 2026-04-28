/* SpeechRecognizer_def.h
 *
 * Copyright (C) 2025,2026 Anastasia Shchupak
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


#define ooSTRUCT SpeechRecognizer
oo_DEFINE_CLASS (SpeechRecognizer, Daata)

	oo_STRING (d_modelName)
	oo_STRING (d_languageName)

	#if oo_DECLARING
		void v1_info () override;
		autoWhisperContext whisperContext;
	#endif

	#if oo_DESTROYING
		theLivingSpeechRecognizers.erase (this);   // undangle
	#endif

oo_END_CLASS (SpeechRecognizer)
#undef ooSTRUCT


/* End of file SpeechRecognizer_def.h */
