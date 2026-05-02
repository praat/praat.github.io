/* TextGrid_Sound.h
 *
 * Copyright (C) 1992-2019,2025 Paul Boersma
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

#include "TextGrid.h"
#include "Sound.h"
#include "Pitch.h"

void TextGrid_Sound_draw (TextGrid me, Sound sound, Graphics g, double tmin, double tmax,
	bool showBoundaries, bool useTextStyles, bool garnish);

autoSoundList TextGrid_Sound_extractAllIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes);
autoSoundList TextGrid_Sound_extractNonemptyIntervals (TextGrid me, Sound sound, integer tierNumber, bool preserveTimes);
autoSoundList TextGrid_Sound_extractIntervalsWhere (TextGrid me, Sound sound,
	integer tierNumber, kMelder_string which, conststring32 text, bool preserveTimes);

void TextGrid_Pitch_draw (TextGrid grid, Pitch pitch, Graphics g,
	integer tierNumber, double tmin, double tmax, double fmin, double fmax,
	double fontSize, bool useTextStyles, int horizontalAlignment, bool garnish, bool speckle, kPitch_unit unit);
void TextGrid_Pitch_drawSeparately (TextGrid grid, Pitch pitch, Graphics g, double tmin, double tmax,
	double fmin, double fmax, bool showBoundaries, bool useTextStyles, bool garnish, bool speckle, kPitch_unit unit);

void TextGrid_anySound_alignInterval (TextGrid me, Function anySound, integer tierNumber, integer intervalNumber,
	conststring32 languageName, bool includeWords, bool includePhonemes);

struct WhisperSegment;
void splitIntervalIntoWhisperSegments (IntervalTier tier, integer tierNumber,
	double originalTmin, double originalTmax, autovector<WhisperSegment> const& segments);
void TextGrid_Sound_transcribeInterval (TextGrid me, Sound sound, integer tierNumber, integer intervalNumber,
	conststring32 modelName, conststring32 languageName, bool includeWords, bool diarize, bool useVad,
	double speechProbabilityThreshold, double minNonSpeechDuration, double minSpeechDuration, double speechPad);

autoSound Sound_readWithAdjacentAnnotationFiles_buckeye (conststring32 soundFileName, autoTextGrid *out_textgrid);
autoSound Sound_readWithAdjacentAnnotationFiles_timit   (conststring32 soundFileName, autoTextGrid *out_textgrid);

/* End of file TextGrid_Sound.h */
