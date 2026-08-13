/* manual_pitch.cpp
 *
 * Copyright (C) 1992-2007,2010,2011,2015-2017,2020-2025 Paul Boersma
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

#include "ManPagesM.h"

void manual_pitch_init (ManPages me);
void manual_pitch_init (ManPages me) {

MAN_BEGIN (U"Create PitchTier...", U"ppgb", 20110101)
INTRO (U"A command in the @@New menu@ to create an empty @PitchTier object.")
NORMAL (U"The resulting object will have the specified name and time domain, but contain no pitch points. "
	"To add some points to it, use @@PitchTier: Add point...@.")
NORMAL (U"For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN (U"Harmonicity", U"ppgb", 20030610)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A Harmonicity object represents the degree of acoustic periodicity, "
	"also called Harmonics-to-Noise Ratio (HNR). "
	"Harmonicity is expressed in dB: "
	"if 99\\%  of the energy of the signal is in the periodic part, and 1\\%  is noise, "
	"the HNR is 10*log10(99/1) = 20 dB. "
	"A HNR of 0 dB means that there is equal energy in the harmonics and in the noise.")
NORMAL (U"Harmonicity can be used as a measure for:")
LIST_ITEM (U"\\bu The signal-to-noise ratio of anything that generates a periodic signal.")
LIST_ITEM (U"\\bu Voice quality. For instance, a healthy speaker can produce a sustained [a] or [i] "
	"with a harmonicity of around 20 dB, and an [u] at around 40 dB; "
	"the difference comes from the high frequencies in [a] and [i], versus low frequencies in [u], "
	"resulting in a much higher sensitivity of HNR to jitter in [a] and [i] than in [u]. "
	"Hoarse speakers will have an [a] with a harmonicity much lower than 20 dB. "
	"We know of a pathological case where a speaker had an HNR of 40 dB for [i], "
	"because his voice let down above 2000 Hz.")
ENTRY (U"Harmonicity commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Sound: To Harmonicity (cc)...@: cross-correlation method (preferred).")
LIST_ITEM (U"\\bu @@Sound: To Harmonicity (ac)...@: autocorrelation method.")
MAN_END

MAN_BEGIN (U"Harmonicity: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Harmonicity objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get maximum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the maximum value, expressed in dB.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the Harmonicity object is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get mean...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the mean value, expressed in dB.")
ENTRY (U"Setting")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The mean harmonicity between the times %t__1_ and %t__2_ is defined as")
EQUATION (U"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt %x(%t)")
NORMAL (U"where %x(%t) is the harmonicity (in dB) as a function of time. "
	"Frames in which the value is undefined (i.e. in silent intervals) "
	"are ignored. If all the frames are silent, the returned value is @undefined.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get minimum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the minimum value, expressed in dB.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get standard deviation...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation, expressed in dB.")
ENTRY (U"Setting")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The standard deviation between the times %t__1_ and %t__2_ is defined as")
EQUATION (U"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt (%x(%t) - %\\mu)^2")
NORMAL (U"where %x(%t) is the harmonicity (in dB) as a function of time, and %\\mu its mean. "
	"For our discrete Harmonicity object, the standard deviation is approximated by")
EQUATION (U"1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2")
NORMAL (U"where %n is the number of frame centres between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (U"Harmonicity: Get time of maximum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object for the time associated with its maximum value.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the maximum HNR value.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get time of minimum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the minimum HNR value.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get value at time...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object to .")
ENTRY (U"Return value")
NORMAL (U"an estimate (in dB) of the value at a specified time. "
	"If this time is outside the time domain or outside the samples of the Harmonicity, the result is @undefined.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which the value is to be evaluated.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method, see @@vector value interpolation@. "
	"The standard is “cubic” because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Harmonicity: Get value in frame...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Harmonicity object.")
ENTRY (U"Return value")
NORMAL (U"the value in a specified frame, expressed in dB. "
	"If the index is less than 1 or greater than the number of frames, the result is @undefined.")
ENTRY (U"Setting")
TERM (U"##Frame number")
DEFINITION (U"the frame whose value is to be looked up.")
MAN_END

MAN_BEGIN (U"Pitch", U"ppgb", 20240722)  // 20101230, 20231115
INTRO (U"One of the @@types of objects@ in Praat. For tutorial information, see @@Intro 4. Pitch analysis@.")
NORMAL (U"A Pitch object represents periodicity candidates as a function of time. "
	"It does not mind whether this periodicity refers to acoustics, "
	"perception, or vocal-cord vibration. "
	"It is sampled into a number of %frames centred around equally spaced times.")
ENTRY (U"Pitch commands")
NORMAL (U"Creation:")
LIST_ITEM (U"\\bu @@Sound: To Pitch (filtered autocorrelation)...@: for intonation and vocal-fold vibration")
LIST_ITEM (U"\\bu @@Sound: To Pitch (raw cross-correlation)...@: for voice analysis")
LIST_ITEM (U"\\bu @@Sound: To Pitch (raw autocorrelation)...@: for raw periodicity")
LIST_ITEM (U"\\bu @@Sound: To Pitch (filtered cross-correlation)...@")
NORMAL (U"Drawing:")
LIST_ITEM (U"\\bu @@Pitch: Draw...@")
NORMAL (U"Viewing and editing:")
LIST_ITEM (U"\\bu @PitchEditor")
NORMAL (U"Synthesis:")
LIST_ITEM (U"\\bu @@Pitch: To PointProcess@: create points in voiced intervals.")
LIST_ITEM (U"\\bu @@Sound & Pitch: To PointProcess (cc)@: near locations of high amplitude.")
LIST_ITEM (U"\\bu @@Sound & Pitch: To PointProcess (peaks)...@: near locations of high amplitude.")
NORMAL (U"Conversion:")
LIST_ITEM (U"\\bu @@Pitch: To PitchTier@: time-stamp voiced intervals.")
LIST_ITEM (U"\\bu @@Pitch & PointProcess: To PitchTier...@: interpolate values at specified times.")
ENTRY (U"Inside a Pitch object")
NORMAL (U"With @Inspect, you will see the following attributes:")
TERM (U"%x__%min_")
DEFINITION (U"start time, in seconds.")
TERM (U"%x__%max_")
DEFINITION (U"end time, in seconds.")
TERM (U"%n__%x_")
DEFINITION (U"the number of frames (\\>_ 1).")
TERM (U"%dx")
DEFINITION (U"time step = frame length = frame duration, in seconds.")
TERM (U"%x__1_")
DEFINITION (U"the time associated with the first frame, in seconds. "
	"This will usually be in the range [%xmin, %xmax]. "
	"The time associated with the last frame (i.e., %x__1_ + (%n__%x_ \\-- 1) %dx)) "
	"will also usually be in that range.")
TERM (U"%ceiling")
DEFINITION (U"a frequency above which a candidate is considered voiceless.")
TERM (U"%frame__%i_, %i = 1 ... %n__%x_")
DEFINITION (U"the frames (see below).")
ENTRY (U"Attributes of a pitch frame")
NORMAL (U"Each frame contains the following attributes:")
TERM (U"%nCandidates")
DEFINITION (U"the number of candidates in this frame (at least one: the “unvoiced” candidate).")
TERM (U"%candidate__%j_, %j = 1 ... %nCandidates")
DEFINITION (U"the information about each candidate (see below).")
ENTRY (U"Attributes of each candidate")
NORMAL (U"Each candidate contains the following attributes:")
TERM (U"%frequency")
DEFINITION (U"the candidate's frequency in Hz (for a voiced candidate), or 0 (for an unvoiced candidate).")
TERM (U"%strength")
DEFINITION (U"the degree of periodicity of this candidate (between 0 and 1).")
ENTRY (U"Interpretation")
NORMAL (U"The current pitch contour is determined by the path through all first candidates. "
	"If the first candidate of a certain frame has a frequency of 0, "
	"or a frequency above %ceiling, this frame is considered voiceless.")
MAN_END

MAN_BEGIN (U"Pitch: Draw...", U"ppgb", 19960910)
INTRO (U"A command for drawing the selected @Pitch objects into the @@Picture window@.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (seconds)")
DEFINITION (U"the time domain along the horizontal axis. "
	"If these are both zero, the time domain of the #Pitch itself is taken (autowindowing).")
TERM (U"##Minimum frequency (Hz)")
TERM (U"##Maximum frequency (Hz)")
DEFINITION (U"the frequency range along the vertical axis. "
	"##Maximum frequency# must be greater than ##Minimum frequency#.")
ENTRY (U"Behaviour")
NORMAL (U"In unvoiced frames, nothing will be drawn.")
NORMAL (U"In voiced frames, the pitch frequency associated with the frame "
	"is thought to represent the time midpoint of the frame, "
	"but frequencies will be drawn at all time points in the frame, as follows:")
LIST_ITEM (U"\\bu If two adjacent frames are both voiced, the frequency of the time points between the midpoints "
	"of the frames is linearly interpolated from both midpoints.")
LIST_ITEM (U"\\bu If a voiced frame is adjacent to another voiced frame on one side, "
	"and to a voiceless frame on the other side, "
	"the frequencies in the half-frame on the unvoiced side will be linearly extrapolated "
	"from the midpoints of the two voiced frames involved.")
LIST_ITEM (U"\\bu If a voiced frame is adjacent to two unvoiced frames, "
	"a horizontal line segment will be drawn at the frequency of the midpoint.")
MAN_END

/*
F0beg = Get value at time... tbeg Hertz Linear
while F0beg = undefined
   tbeg = tbeg + 0.01
   F0beg = Get value at time... tbeg Hertz Linear
endwhile
*/

MAN_BEGIN (U"Pitch: Interpolate", U"ppgb", 19990811)
INTRO (U"A command that converts every selected @Pitch object.")
/*
	myID = selected ("Pitch")
	tmin = Get start time
	tmax = Get end time
	numberOfFrames = Get number of frames
	timeStep = Get time step
	t1 = Get time of frame... 1
	ceiling = Get ceiling
	Create Pitch... interpolated tmin tmax numberOfFrames timeStep t1 ceiling 2
	for iframe to numberOfFrames
		select 'myID'
		f = Get frequency... iframe 1
		if f > 0.0 and f < ceiling
			# The frame is voiced? Copy the frequency.
			select Pitch interpolated
			Set frequency... iframe 1 f
		else
			# The frame is unvoiced? Interpolate the frequency.
			fleft = 0.0
			fright = 0.0
			# Find nearest voiced frame to the left.
			left = iframe - 1
			while left >= 1 and fleft = 0.0
				fleft = Get frequency... left 1
				if fleft >= ceiling
					fleft = 0.0
				endif
				left = left - 1
			endwhile
			# Find nearest voiced frame to the right.
			right = iframe + 1
			while right <= numberOfFrames and fright = 0.0
				fright = Get frequency... right 1
				if fright >= ceiling
					fright = 0.0
				endif
				right = right + 1
			endwhile
			if fleft <> 0 and fright <> 0
				select Pitch interpolated
				Set frequency... iframe 1 ((iframe - left) * fright + (right - iframe) * fleft) / (right - left)
			endif
		endif
		select Pitch interpolated
		Set strength... iframe 1 0.9
	endfor
	select Pitch interpolated
*/
MAN_END

MAN_BEGIN (U"Pitch: Smooth...", U"ppgb", 19990811)
INTRO (U"A command that converts every selected @Pitch object.")
MAN_END

MAN_BEGIN (U"Pitch: To PitchTier", U"ppgb", 19960915)
INTRO (U"A command that converts a @Pitch object into a @PitchTier object.")
ENTRY (U"Algorithm")
NORMAL (U"The #PitchTier object will contain as many points as there were voiced frames in the #Pitch.")
NORMAL (U"The %time of each point is the time associated with the centre of the corresponding %frame "
	"of the #Pitch contour. The %frequency of the point is the pitch frequency associated in this frame "
	"with the current path through the candidates.")
MAN_END

MAN_BEGIN (U"Pitch: To PointProcess", U"ppgb", 19960917)
INTRO (U"A command that uses a @Pitch object to generate a @PointProcess.")
ENTRY (U"Purpose")
NORMAL (U"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (U"Algorithm")
LIST_ITEM (U"1. A @PitchTier is created with @@Pitch: To PitchTier@.")
LIST_ITEM (U"2. The algorithm of @@PitchTier: To PointProcess@ generates points along the entire time domain "
	"of the PitchTier.")
LIST_ITEM (U"3. The PitchTier is removed (it never appeared in the List of Objects).")
LIST_ITEM (U"4. The voiced/unvoiced information in the Pitch is used to remove all points "
	"that lie within voiceless frames.")
MAN_END

MAN_BEGIN (U"Pitch & PointProcess: To PitchTier...", U"ppgb", 20101230)
INTRO (U"A command that creates a @PitchTier object from one selected @Pitch and one selected @PointProcess object.")
ENTRY (U"Purpose")
NORMAL (U"to return the frequencies in the Pitch contour at the times specified by the PointProcess.")
ENTRY (U"Setting")
TERM (U"##Check voicing# (standard: on)")
DEFINITION (U"determines whether, if the time of a mark is not within a voiced frame, you will get a message like "
	"\"No periodicity at time %xxx.\", and no PitchTier is created. If this button is off, "
	"the resulting pitch frequency will be 0.0 Hz.")
ENTRY (U"Normal behaviour")
NORMAL (U"For all the times in the PointProcess, a pitch frequency is computed from the "
	"information in the Pitch, by linear interpolation.")
NORMAL (U"All the resulting time-frequency pairs are put in a new PitchTier object.")
NORMAL (U"The time domain of the resulting PitchTier is a union of the domains of the original Pitch "
	"and PointProcess functions.")
MAN_END

MAN_BEGIN (U"PitchEditor", U"ppgb", /*20110808 20220814 20231115*/ 20240722)
INTRO (U"One of the @Editors in Praat, for viewing and modifying a @Pitch object.")
ENTRY (U"What the Pitch editor shows")
NORMAL (U"In the window of the PitchEditor, you will see the following features:")
LIST_ITEM (U"\\bu Digits between 0 and 9 scattered all over the drawing area. Their locations represent "
	"the pitch %#candidates, of which there are several for every time frame. The digits themselves "
	"represent the goodness of a candidate, multiplied by ten. For instance, if you see a \"9\" "
	"at the location (1.23 seconds, 189 hertz), this means that in the time frame at 1.23 seconds, "
	"there is a pitch candidate with a value of 189 hertz, and its goodness is 0.9. "
	"The number 0.9 may be the relative height of an autocorrelation peak, a cross-correlation peak, "
	"or a spectral peak, depending on the method by which the Pitch object was computed.")
LIST_ITEM (U"\\bu A %#path of red disks. These disks represent the best path through the candidates, "
	"i.e. our best guess at what the pitch contour is. The path will usually have been determined "
	"by the %%path finder%, which was called by the pitch-extraction algorithm, and you can change "
	"the path manually. The path finder takes into account the goodness of each candidate, "
	"the intensity of the sound in the frame, voiced-unvoiced transitions, and frequency jumps. "
	"It also determines whether each frame is voiced or unvoiced.")
LIST_ITEM (U"\\bu A %%##voicelessness bar%# at the bottom of the drawing area. If there is no suitable "
	"pitch candidate in a frame, the frame is considered voiceless, which is shown as a blue rectangle "
	"in the voicelessness bar.")
LIST_ITEM (U"\\bu A line of digits between 0 and 9 along the top. These represent the relative intensity "
	"of the sound in each frame.")
ENTRY (U"Moving the marks")
NORMAL (U"To move the cursor hair or the beginning or end of the selection, "
	"use the @@time selection@ mechanism.")
ENTRY (U"Changing the path")
NORMAL (U"To change the path through the candidates manually, click on the candidates of your choice. "
	"The changes will immediately affect the Pitch object that you are editing. To make a voiced frame "
	"voiceless, click in the voicelessness bar.")
NORMAL (U"To change the path automatically, choose ##Path finder...# from the #Edit menu; "
	"this will have the same effect as filling in different values in "
	"the @@Sound: To Pitch (filtered autocorrelation)...@ or @@Sound: To Pitch (raw autocorrelation)...@ command window, "
	"but is much faster because the candidates do not have to be determined again.")
ENTRY (U"Resynthesis")
NORMAL (U"To hum any part of the pitch contour, click one of the buttons "
	"below or above the data area (there can be 1 to 8 of these buttons), or use a command from the #Play menu.")
ENTRY (U"Changing the ceiling")
NORMAL (U"To change the ceiling, but not the path, choose ##Change ceiling...# from the #Edit menu; "
	"if the new ceiling is lower than the old ceiling, some formerly voiced frames may become unvoiced; "
	"if the new ceiling is higher than the old ceiling, some formerly unvoiced frames may become voiced.")
MAN_END

MAN_BEGIN (U"PitchTier", U"ppgb", 20101230) // 20251229
INTRO (U"One of the @@types of objects@ in Praat. "
	"A PitchTier object represents a time-stamped pitch contour, "
	"i.e. it contains a number of (%time, %pitch) points, without voiced/unvoiced information. "
	"For instance, if your PitchTier contains two points, namely 150 Hz at a time of 0.5 seconds and "
	"200 Hz at a time of 1.5 seconds, then this is to be interpreted as a pitch contour that "
	"is constant at 150 Hz for all times before 0.5 seconds, constant at 200 Hz for all times after 1.5 seconds, "
	"and linearly interpolated for all times between 0.5 and 1.5 seconds (i.e. 160 Hz at 0.7 seconds, "
	"180 Hz at 1.1 seconds, and so on).")
NORMAL (U"PitchTier objects are used for two purposes: "
	"for manipulating the pitch curve of an existing sound "
	"(see @@Intro 8.1. Manipulation of pitch@) "
	"and for synthesizing a new sound "
	"(see @@Source-filter synthesis 1. Creating a source from pitch targets@, "
	"and for an example @@Source-filter synthesis 3. The ba-da continuum@).")
ENTRY (U"PitchTier commands")
NORMAL (U"Creation:")
LIST_ITEM (U"From scratch:")
LIST_ITEM (U"\\bu @@Create PitchTier...")
LIST_ITEM (U"\\bu @@PitchTier: Add point...")
LIST_ITEM (U"Copy from another object:")
LIST_ITEM (U"\\bu @@Pitch: To PitchTier@: trivial copying of voiced frames.")
LIST_ITEM (U"\\bu @@PointProcess: Up to PitchTier...@: single value at specified times.")
LIST_ITEM (U"\\bu @@Pitch & PointProcess: To PitchTier...@: copying interpolated values at specified points.")
LIST_ITEM (U"Synthesize from another object:")
LIST_ITEM (U"\\bu @@PointProcess: To PitchTier...@: periodicity analysis.")
LIST_ITEM (U"Extract from a @Manipulation object:")
LIST_ITEM (U"\\bu @@Manipulation: Extract pitch tier@")
NORMAL (U"Viewing and editing:")
LIST_ITEM (U"\\bu @PitchTierEditor: with or without a Sound.")
LIST_ITEM (U"\\bu @ManipulationEditor")
NORMAL (U"Conversion:")
LIST_ITEM (U"\\bu @@PitchTier: Down to PointProcess@: copy the times.")
NORMAL (U"Synthesis:")
LIST_ITEM (U"\\bu @@PitchTier: To PointProcess@: area-1 pulse generation (used in @@overlap-add@).")
LIST_ITEM (U"\\bu @@Manipulation: Replace pitch tier@")
NORMAL (U"Queries:")
LIST_ITEM (U"\\bu @@time domain")
LIST_ITEM (U"\\bu @@Get low index from time...")
LIST_ITEM (U"\\bu @@Get high index from time...")
LIST_ITEM (U"\\bu @@Get nearest index from time...")
LIST_ITEM (U"\\bu @@PitchTier: Get mean (curve)...")
LIST_ITEM (U"\\bu @@PitchTier: Get mean (points)...")
LIST_ITEM (U"\\bu @@PitchTier: Get standard deviation (curve)...")
LIST_ITEM (U"\\bu @@PitchTier: Get standard deviation (points)...")
LIST_ITEM (U"\\bu @@Get area...@: the number of periods")
NORMAL (U"Modification:")
LIST_ITEM (U"\\bu @@Remove point...")
LIST_ITEM (U"\\bu @@Remove point near...")
LIST_ITEM (U"\\bu @@Remove points between...")
LIST_ITEM (U"\\bu @@PitchTier: Add point...")
LIST_ITEM (U"\\bu @@PitchTier: Stylize...")
MAN_END

MAN_BEGIN (U"PitchTier: Add point...", U"ppgb", 20010410)
INTRO (U"A command to add a point to each selected @PitchTier.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
TERM (U"##Pitch (Hz)")
DEFINITION (U"the pitch value of the requested new point.")
ENTRY (U"Behaviour")
NORMAL (U"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (U"PitchTier: Down to PointProcess", U"ppgb", 20010410)
INTRO (U"A command to degrade every selected @PitchTier to a @PointProcess.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the pitch points are trivially copied, and so is the time domain. The pitch information is lost.")
MAN_END

MAN_BEGIN (U"PitchTier: Get mean (curve)...", U"ppgb", 20170618)
INTRO (U"A @@Query submenu|query@ to the selected @PitchTier object.")
ENTRY (U"Return value")
NORMAL (U"the mean of the curve within a specified time window.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the time window, in seconds. Values outside this window are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area under the curve is")
EQUATION (U"1/2 (%f__1_ + %f__2_) (%t__2_ \\-- %t__1_)")
NORMAL (U"The mean is the sum of these values divided by %toTime \\-- %fromTime.")
NORMAL (U"For a PitchTier that was created from a @Pitch object, this command gives the same result as "
	"##Get mean....# for the original Pitch object (but remember that the median, "
	"as available for Pitch objects, is more robust).")
NORMAL (U"To get the mean of the pitch points, i.e. not weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get mean (points)...@ instead.")
MAN_END

MAN_BEGIN (U"PitchTier: Get mean (points)...", U"ppgb", 20010821)
INTRO (U"A @@Query submenu|query@ to the selected @PitchTier object.")
ENTRY (U"Return value")
NORMAL (U"the mean of the points within a specified time window.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the time window, in seconds. Values outside this window are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
NORMAL (U"To get the mean in the entire curve, i.e. weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get mean (curve)...@ instead.")
MAN_END

MAN_BEGIN (U"PitchTier: Get standard deviation (curve)...", U"ppgb", 20010821)
INTRO (U"A @@Query submenu|query@ to the selected @PitchTier object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation in the curve within a specified time window.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the variance-multiplied-by-time is")
EQUATION (U"[ 1/4 (%f__1_ + %f__2_)^2 + 1/12 (%f__1_ \\-- %f__2_)^2 ] (%t__2_ \\-- %t__1_)")
NORMAL (U"The standard deviation is the square root of: the sum of these values divided by %toTime \\-- %fromTime.")
NORMAL (U"To get the standard deviation in the points only, i.e. not weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get standard deviation (points)...@ instead.")
MAN_END

MAN_BEGIN (U"PitchTier: Get standard deviation (points)...", U"ppgb", 20010821)
INTRO (U"A @@Query submenu|query@ to the selected @PitchTier object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation in the points within a specified time window.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
NORMAL (U"For a PitchTier that was created from a @Pitch object, this command gives the same result as "
	"##Get standard deviation...# for the original Pitch object (but remember that variation measures "
	"based on quantiles, as available for Pitch objects, are more robust).")
NORMAL (U"To get the standard deviation in the entire curve, i.e. weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get standard deviation (curve)...@ instead.")
MAN_END

MAN_BEGIN (U"PitchTier: Stylize...", U"ppgb", 20050831)
INTRO (U"A command that modifies the selected @PitchTier object "
	"(or the pitch curve in the @ManipulationEditor).")
ENTRY (U"Purpose")
NORMAL (U"to end up with a much simplified pitch curve.")
ENTRY (U"Settings")
TERM (U"##Frequency resolution# (standard: 2.0 semitones)")
DEFINITION (U"the minimum amount by which every remaining pitch point will lie "
	"above or below the line that connects its two neigbours.")
ENTRY (U"Algorithm")
LIST_ITEM (U"1. Look up the pitch point that is closest to the straight line "
	"that connects its two neighbouring points.")
LIST_ITEM (U"2. If this pitch point is further away from that straight line "
	"than ##Frequency resolution#, we are finished: the curve cannot be stylized any further.")
LIST_ITEM (U"3. If we are not finished, the pitch point we found in step 1 is removed.")
LIST_ITEM (U"4. Go back to step 1.")
MAN_END

MAN_BEGIN (U"PitchTier: To PointProcess", U"ppgb", 19960915)
INTRO (U"A command that uses a @PitchTier object to generate a @PointProcess.")
ENTRY (U"Purpose")
NORMAL (U"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (U"Algorithm")
NORMAL (U"Points are generated along the entire time domain of the #PitchTier, "
	"because there is no voiced/unvoiced information. The area between two adjacent points "
	"under the linearly interpolated pitch contour, is always 1.")
MAN_END

MAN_BEGIN (U"PitchTierEditor", U"ppgb", /*20110128 20220814*/ 20230608)
INTRO (U"One of the @Editors in Praat, for viewing and manipulating a @PitchTier object, "
	"which is optionally shown together with a @Sound object.")
ENTRY (U"Objects")
NORMAL (U"The editor shows:")
LIST_ITEM (U"\\bu The @Sound, if you selected a Sound object together with the PitchTier object "
	"before you clicked ##View & Edit#.")
LIST_ITEM (U"\\bu The @PitchTier: blue points connected with blue lines.")
ENTRY (U"Playing")
NORMAL (U"To play (a part of) the %resynthesized sound: "
	"@click any of the 8 buttons below and above the drawing area, or choose a command from the Play menu.")
NORMAL (U"To play the %original sound instead, use @@Shift-click@.")
ENTRY (U"Adding a point")
NORMAL (U"@Click at the desired time location, and choose ##Add point at cursor# or type @@Keyboard shortcuts|Command-P@.")
ENTRY (U"Removing points")
NORMAL (U"To remove one or more pitch points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN (U"PointProcess: To PitchTier...", U"ppgb", 19970402)
INTRO (U"A command to compute a @PitchTier from a @PointProcess.")
ENTRY (U"Setting")
TERM (U"##Maximum interval (s)")
DEFINITION (U"the maximum duration of a period; intervals longer than this are considered voiceless.")
ENTRY (U"Algorithm")
NORMAL (U"A pitch point is constructed between each consecutive pair of points in the #PointProcess, "
	"if these are more than %maximumInterval apart. "
	"The associated pitch value will be the inverse of the duration of the interval between the two points.")
MAN_END

MAN_BEGIN (U"Sound: To Harmonicity (ac)...", U"ppgb", 20030916)
INTRO (U"A command that creates a @Harmonicity object from every selected @Sound object.")
ENTRY (U"Purpose")
NORMAL (U"to perform a short-term HNR analysis.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm performs an acoustic periodicity detection on the basis of an "
	"accurate autocorrelation method, as described in @@Boersma (1993)@.")
ENTRY (U"Settings")
TERM (U"%%Time step% (standard value: 0.01 s)")
DEFINITION (U"the measurement interval (frame duration), in seconds.")
TERM (U"%%Pitch floor% (standard value: 75 Hz)")
DEFINITION (U"determines the length of the analysis window.")
TERM (U"%%Silence threshold% (standard value: 0.1)")
DEFINITION (U"frames that do not contain amplitudes above this threshold (relative to the global maximum amplitude), "
	"are considered silent.")
TERM (U"%%Number of periods per window% (standard value: 4.5)")
DEFINITION (U"4.5 is best for speech: "
	"HNR values up to 37 dB are guaranteed to be detected reliably; "
	"6 periods per window raises this figure to more than 60 dB, "
	"but the algorithm becomes more sensitive to dynamic changes in the signal.")
ENTRY (U"Usage")
NORMAL (U"You will normally use @@Sound: To Harmonicity (cc)...@ instead of this command, "
	"because that has a much better time resolution (though its sensitivity is 60, not 80 dB).")   // TODO: check these values, because we say 60 dB above
MAN_END

MAN_BEGIN (U"Sound: To Harmonicity (cc)...", U"ppgb", 19961003)
INTRO (U"A command that creates a @Harmonicity object from every selected @Sound object.")
ENTRY (U"Purpose")
NORMAL (U"to perform a short-term HNR analysis.")
ENTRY (U"Algorithm")
NORMAL (U"The algorithm performs an acoustic periodicity detection on the basis of a "
	"forward cross-correlation analysis. "
	"For information on the settings, see @@Sound: To Harmonicity (ac)...@.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"how to choose a pitch analysis method"
© Paul Boersma 2023,2024

Pitch analysis is the determination of a pitch curve from a given sound.

In Praat you can choose from at least four methods for pitch analysis:
- @@pitch analysis by filtered autocorrelation
- @@pitch analysis by raw cross-correlation
- @@pitch analysis by raw autocorrelation
- @@pitch analysis by filtered cross-correlation

These method have been listed here in order of usefulness for the %average speech researcher:
for measuring vocal-fold vibration frequency or intonation,
you are advised to use @@pitch analysis by ||filtered autocorrelation@;
for voice analysis (such as is used in the area of voice pathology),
you are advised to use @@pitch analysis by ||raw cross-correlation@;
and for measuring pure periodicity,
you are advised to use @@pitch analysis by ||raw autocorrelation@.

Measuring intonation
====================

From 1993 to 2023, Praat’s preferred method for measuring intonation
(and for most use cases involving vocal-fold vibration) was
@@pitch analysis by ||raw autocorrelation@, and this is still available if you
want to measure raw periodicity (see below). From 2023 on,
Praat’s preferred method for measuring intonation
(and for most use cases involving vocal-fold vibration) has been
@@pitch analysis by ||filtered autocorrelation@.

All these methods measure pitch in terms of the self-similarity of the waveform.
If the waveform is almost identical if you shift it by 10 milliseconds in time,
then 100 Hz will be a good candidate for being the F0. This idea has been used in Praat from 1993 on,
using the "autocorrelation" and "cross-correlation" methods,
which are nowadays called "raw autocorrelation" and "raw cross-correlation".
Both methods measure self-similarity as a number between -1.0 and +1.0.
From 2023 on, we also have "filtered autocorrelation" and "filtered cross-correlation",
which use low-pass filtering of the waveform (by a Gaussian filter)
prior to doing autocorrelation or cross-correlation.

The filtering comes with several benefits:

- ##Filtering leads to fewer unwanted octave drops.# An unwanted octave drop can occur,
for instance, if you set the pitch floor to 75 Hz (the standard value for raw AC)
but the F0 is 200 Hz and there is background noise.
The waveshape will be self-similar over time shifts of 5 ms,
but then necessarily also over time shifts of 10 ms and 15 ms, and so on.
Background noise can sometimes randomly make the degree of self-similarity higher
over 10 ms than over 5 ms, leading to an octave drop in the measured pitch, from 200 to 100 Hz.
In raw AC and raw CC, this problem is alleviated a bit by applying an "octave cost",
whose standard setting is 0.01 per octave (on a self-similarity scale from 0.0 to 1.0).
However, low-pass filtering reduces the problem more strongly,
by taking away the higher-frequency parts of the background noise.
- ##Filtering leads to fewer unwanted octave rises.# If you set the pitch ceiling to 600 Hz
(the standard value), and the F0 is 150 Hz, and the F1 is 450 Hz,
then the harmonic relation between F0 and F1 (F1 is exactly three times F0)
will lead to a very strong F1 in the waveform, sometimes causing the pitch to be analysed
as 450 instead of 150 Hz. This problem is exacerbated by the existence of the “octave cost”,
which favours higher pitch candidates. Low-pass filtering reduces this problem,
because the 450-Hz component of the waveform is weakened with respect to the 150-Hz component
by a factor of 0.373 (if the pitch top is 800 Hz, and the “attenuation at top” is set to 0.03).
This effect is so strong that the standard “octave cost” can be much higher for filtered pitch analysis
(the standard value is 0.055 per octave) than for raw pitch analysis (where the standard value is 0.01 per octave):
this still reduces the formant problem heavily, and it reduces the unwanted octave drops
by making the analysis more resistant to %lower-frequency parts of the background noise.
- ##Filtering leads to better tracking of formant transitions.# Fast formant movements
in transitions between consonants and vowels cause waveforms to change their shape strongly in time.
This reduces the self-similarity of the waveform, leading to the “raw AC” method judging
some of these transitions as “unvoiced”. With low-pass filtering, the transitions become
more sine-like, which leads “filtered AC” being more likely to judge these transitions as “voiced”.

On a database containing EGG recordings as a gold standard (AtakeDB),
the incidence of unwanted big frequency drops falls from 0.54% (of voiced frames) for raw AC to 0.15% for filtered AC,
and the number of unwanted big frequency rises falls from 0.25% to 0.10% (of voiced frames);
together, these numbers indicate that the incidence of gross frequency errors is better
for filtered AC than for raw AC by a factor of more than 3.

On the other hand, low-pass filtering also produces potentially unwanted side effects:

- The reduction of higher frequency components causes voicing decisions to be based on smoother curves.
As this influences especially the loud parts (the vowels),
this reduction may have to lead to a higher optimal “silence threshold” setting.
Indeed, in testing the algorithm on AtakeDB,
the best silence threshold for filtered AC seems to be 0.18,
whereas for raw AC it seems to be 0.06. Accounting for recordings with a larger dynamic range than AtakeDB,
the current standard silence threshold is half that: 0.03 for raw AC, and 0.09 for filtered AC.
This can lead to more difficulty measuring plosive voicing (during prevoiced [b] or [d]);
if you want to measure those, then you are advised to lower the silence threshold
from the standard of 0.09 to e.g. 0.01 (this is also somewhat true for raw AC,
where the standard value is 0.03 but one should probably use 0.01 for prevoicing).
- Generalizingly, voicing decisions seem to be less predictable for filtered AC than for raw AC,
i.e. it is possible that the incidence of unvoiced-to-voiced errors and voiced-to-unvoiced errors is greater.
This may have to be shown in practice. In the future, voicing decisions may have to be relegated
to a trained neural network, rather than to the current simple “costs” and “thresholds”.

All in all, it seems likely that filtered AC works better in practice than raw AC
(your feedback is invited, though), which is why filtered AC is the preferred pitch analysis method
for vocal-fold vibration and intonation, as is shown in the Periodicity menu in the Objects window,
in the Pitch menu of the Sound window, and in many places in this manual.
While the standard pitch range for raw AC runs from 75 to 600 Hz,
filtered AC has less trouble discarding very low or very high pitch candidates,
so the standard pitch range for filtered AC is wider, running from 50 Hz to 800 Hz,
and may not have to be adapted to the speaker’s gender.

Voice analysis
==============

For voice analysis, we probably shouldn’t filter away noise, so @@pitch analysis by ||raw cross-correlation@ is the preferred method.

Measuring prevoicing
====================

The high standard “silence threshold” of 0.09 for filtered AC (or, to a lesser extent, 0.03 for raw AC)
probably leads to trouble measuring voicing during the closure phase of prevoiced plosives such as [b] or [d].
You are still advised to use filtered AC, but to lower the silence threshold to 0.01 or so.

Raw periodicity
===============

Mathematically generated periodic signals aren’t necessarily speechlike. For instance:
{
	sine = Create Sound from formula: "sine", 1, 0, 0.1, 44100, "0.4*sin(2*pi*200*x)"
	tricky = Create Sound from formula: "tricky", 1, 0, 0.1, 44100, "0.001*sin(2*pi*400*x)
	... + 0.001*sin(2*pi*600*x) - 0.2*cos(2*pi*800*x+1.5) - 0.2*cos(2*pi*1000*x+1.5)"
	selectObject: sine, tricky
	Concatenate
	Erase all
	Draw: 0.08, 0.12, 0, 0, "yes", "curve"
}
Both the left part of this sound and the right part have a period of 5 ms,
and therefore an F0 of 200 Hz. The @@pitch analysis by ||raw autocorrelation@ method measures an equally
strong pitch of 200 Hz throughout this signal, while @@pitch analysis by ||filtered autocorrelation@
considers the right part voiceless. This is because the right part contains
only components at 800 and 1000 Hz, which will be filtered out by the low-pass filter,
and only very small components at 200, 400 or 600 Hz.
This problem of the %%missing fundamental% was the reason why low-pass filtering
was not included in @@pitch analysis by ||raw autocorrelation@ in 1993. However,
this situation is %very rare in speech, so for speech we do nowadays recommend @@pitch analysis by ||filtered autocorrelation@,
while we recommend @@pitch analysis by ||raw autocorrelation@ only if you want to measure %raw periodicity.

################################################################################
"Sound: To Pitch..."
© Paul Boersma 1996,2003,2023,2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

Purpose
=======

to perform a pitch analysis, optimized for speech.

Settings
========

The settings that control the recruitment of the candidates are:

##Time step (s)# (standard value: 0.0)
:	the measurement interval (frame duration), in seconds. If you supply 0.0,
Praat will use a time step of 0.75 / (%%pitch floor%), e.g. 0.01 seconds if the pitch floor is 75 Hz;
in this example, Praat computes 100 pitch values per second.

##Pitch floor (Hz)# (standard value: 75 Hz)
:	candidates below this frequency will not be recruited.
This parameter determines the length of the analysis window: it will be 3 longest periods long,
i.e., if the pitch floor is 75 Hz, the window will be 3/75 = 0.04 seconds long.

Note that if you set the time step to zero, the analysis windows for consecutive measurements
will overlap appreciably: Praat will always compute 4 pitch values
within one window length, i.e., the degree of %oversampling is 4.

##Pitch ceiling (Hz)# (standard value: 600 Hz)
:	candidates above this frequency will be ignored.

Algorithm
=========

This is the algorithm described at @@Sound: To Pitch (raw autocorrelation)...@,
with all the parameters not mentioned above set to their standard values.

################################################################################
"Sound: To Pitch (ac)..."
© Paul Boersma 1996,2001–2003,2022–2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

The parameters are in the following order:
- ##Time step (s)# (standard value: 0.0)
- ##Pitch floor (Hz)# (standard value: 75 Hz)
- ##Max. number of candidates# (standard value: 15)
- ##Very accurate# (standard value: %off)
- ##Silence threshold# (standard value: 0.03)
- ##Voicing threshold# (standard value: 0.45)
- ##Octave cost# (standard value: 0.01 per octave)
- ##Octave-jump cost# (standard value: 0.35)
- ##Voiced / unvoiced cost# (standard value: 0.14)
- ##Pitch ceiling (Hz)# (standard value: 600 Hz)

With the advent of @@Sound: To Pitch (filtered autocorrelation)...@ in 2023,
this command was replaced with @@Sound: To Pitch (raw autocorrelation)...@.

Transitioning
=============

While ##Sound: To Pitch (ac)...# will still work in scripts,
you make like to transition to @@Sound: To Pitch (raw autocorrelation)...@.
Please note that the %%Pitch ceiling% parameter was moved immediately after %%Pitch floor%,
where it fits more logically.
For example, you can replace the line
{;
	\#{To Pitch (ac):} 0.0, 75, 15, “off”, 0.03, 0.45, 0.01, 0.35, 0.14, 600
}
with the entirely equivalent line
{;
	\#@{Sound: ||To Pitch (raw autocorrelation):} 0.0, 75, 600, 15, “off”, 0.03, 0.45, 0.01, 0.35, 0.14
}

################################################################################
"Sound: To Pitch (cc)..."
© Paul Boersma 1996,2003,2023,2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

The parameters are in the following order:
- ##Time step (s)# (standard value: 0.0)
- ##Pitch floor (Hz)# (standard value: 75 Hz)
- ##Max. number of candidates# (standard value: 15)
- ##Very accurate# (standard value: %off)
- ##Silence threshold# (standard value: 0.03)
- ##Voicing threshold# (standard value: 0.45)
- ##Octave cost# (standard value: 0.01 per octave)
- ##Octave-jump cost# (standard value: 0.35)
- ##Voiced / unvoiced cost# (standard value: 0.14)
- ##Pitch ceiling (Hz)# (standard value: 600 Hz)

With the advent of @@Sound: To Pitch (filtered cross-correlation)...@ in 2023,
this command was replaced with @@Sound: To Pitch (raw cross-correlation)...@.

Transitioning
=============

While ##Sound: To Pitch (cc)...# will still work in scripts,
you may like to transition to @@Sound: To Pitch (raw cross-correlation)...@.
Please note that the %%Pitch ceiling% parameter was moved immediately after %%Pitch floor%,
where it fits more logically.
For example, you can replace the line
{;
	\#{To Pitch (cc):} 0.0, 75, 15, “off”, 0.03, 0.45, 0.01, 0.35, 0.14, 600
}
with the entirely equivalent line
{;
	\#@{Sound: ||To Pitch (raw cross-correlation):} 0.0, 75, 600, 15, “off”, 0.03, 0.45, 0.01, 0.35, 0.14
}

################################################################################
"Sound: To Pitch (raw ac)..."
© Paul Boersma 1996,2001–2003,2022-2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

This command is a precise synonym of @@Sound: To Pitch (raw autocorrelation)...@.

################################################################################
"Sound: To Pitch (raw autocorrelation)..."
© Paul Boersma 1996,2001–2003,2022-2024

A command that creates a @Pitch object from every selected @Sound object.

For purpose, algorithm and settings, see @@pitch analysis by raw autocorrelation@.

For usage, see @@how to choose a pitch analysis method@.

Synonyms
========

In older scripts you may find:
- @@Sound: To Pitch (raw ac)...@, a precise synonym with an identical list of parameters.
- @@Sound: To Pitch (ac)...@, a command with the same meaning, with parameters in a different order.

################################################################################
"Pitch settings..."
© Paul Boersma 2003,2024

Four commands in the Pitch menu of the @SoundEditor and @TextGridEditor windows:

- ##Pitch settings (filtered autocorrelation)...#: see @@pitch analysis by filtered autocorrelation@
- ##Pitch settings (raw cross-correlation)...#: see @@pitch analysis by raw cross-correlation@
- ##Pitch settings (raw autocorrelation)...#: see @@pitch analysis by raw autocorrelation@
- ##Pitch settings (filtered cross-correlation)...#: see @@pitch analysis by filtered cross-correlation@

Also see @@Intro 4.2. Configuring the pitch contour@ and @@how to choose a pitch analysis method@.

################################################################################
"pitch analysis by raw autocorrelation"
© Paul Boersma 1996,2001–2003,2022-2024

Purpose
=======

to perform a pitch analysis based on an autocorrelation method.

Usage
=====

Raw autocorrelation is the pitch analysis method of choice if you want measure the raw periodicity of a signal.

Note that the preferred method for speech (intonation, vocal fold vibration) is @@pitch analysis by filtered autocorrelation@.
See @@how to choose a pitch analysis method@ for details.

Algorithm
=========

The algorithm performs an acoustic periodicity detection on the basis of an
accurate autocorrelation method, as described in @@Boersma (1993)@.
This method is more accurate, noise-resistant, and robust, than methods based on cepstrum or combs,
or the original autocorrelation methods. The reason why other methods were invented,
was the failure to recognize the fact that if you want to estimate a signal's short-term autocorrelation
function on the basis of a windowed signal, you should divide the autocorrelation function of the
windowed signal by the autocorrelation function of the window:

~	%r_%x (%\ta) \~~ %r__%xw_ (%\ta) / %r_%w (%\ta)

The pitch is basically determined as the inverse of the time (%lag) where the autocorrelation function %r
has its maximum. However, there are likely to be multiple peaks in %r,
and all of these can be %%pitch candidates%. For each moment in time (e.g. every 10 ms),
the algorithm determines the (typically) 15 highest peaks in %r,
regards these as %candidates, and then tracks an optimal path through the candidates over time.

Settings
========

Several settings are already described in @@Intro 4.2. Configuring the pitch contour@.
The explanations below assume that you have gone through that part of the Intro.

The settings that control the recruitment of the candidates are:

##Time step (s)# (standard value: 0.0)
:	the measurement interval (frame duration), in seconds. If you supply 0,
Praat will use a time step of 0.75 / (%%pitch floor%), e.g. 0.01 seconds if the pitch floor is 75 Hz;
in this example, Praat computes 100 pitch values per second.

##Pitch floor (Hz)# (standard value: 75 Hz)
:	candidates below this frequency will not be recruited.
This parameter determines the effective length of the analysis window: it will be 3 longest periods long,
i.e., if the pitch floor is 75 Hz, the window will be effectively 3/75 = 0.04 seconds long.

Note that if you set the time step to zero, the analysis windows for consecutive measurements
will overlap appreciably: Praat will always compute 4 pitch values
within one window length, i.e., the degree of %oversampling is 4.

##Pitch ceiling (Hz)# (standard value: 600 Hz)
:	candidates above this frequency will be ignored.

##Max. number of candidates# (standard value: 15)
:	each frame will contain at least this many pitch candidates.
One of them is the “unvoiced candidate”; the others correspond to
time lags over which the signal is more or less similar to itself.

##Very accurate# (standard value: %off)
:	if %off, the window is a Hanning window with a physical length of  3 / (%%pitch floor%).
If %on, the window is a Gaussian window with a physical length of  6 / (%%pitch floor%),
i.e. twice the effective length.

A post-processing algorithm seeks the cheapest path through the candidates.
The settings that determine the cheapest path are:

##Silence threshold# (standard value: 0.03)
:	frames that do not contain amplitudes above this threshold (relative to the global maximum amplitude),
are probably silent.

##Voicing threshold# (standard value: 0.45)
:	the strength of the unvoiced candidate, relative to the maximum possible autocorrelation.
If the amount of periodic energy in a frame is more than this fraction of the total energy (the remainder being noise),
then Praat will prefer to regard this frame as voiced; otherwise as unvoiced.
To increase the number of unvoiced decisions, increase the voicing threshold.

##Octave cost# (standard value: 0.01 per octave)
:	degree of favouring of high-frequency candidates, relative to the maximum possible autocorrelation.
This is necessary because even (or: especially) in the case of a perfectly periodic signal,
all undertones of %F_0 are equally strong candidates as %F_0 itself.
To more strongly favour recruitment of high-frequency candidates, increase this value.

##Octave-jump cost# (standard value: 0.35)
:	degree of disfavouring of pitch changes, relative to the maximum possible autocorrelation.
To decrease the number of large frequency jumps, increase this value. In contrast with what is described
in the article, this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get
the value in the way it is used in the formulas in the article.

##Voiced / unvoiced cost# (standard value: 0.14)
:	degree of disfavouring of voiced/unvoiced transitions, relative to the maximum possible autocorrelation.
To decrease the number of voiced/unvoiced transitions, increase this value. In contrast with what is described
in the article, this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get
the value in the way it is used in the formulas in the article.

The standard settings are best in most cases.
For some pathological voices, you will want to set the voicing threshold to much less than the standard of 0.45,
in order to get pitch values even in irregular parts of the signal.
For prevoiced plosives, you may want to lower the silence threshold from 0.03 to 0.01 or so.

Availability in Praat
=====================

Pitch analysis by raw autocorrelation is available in two ways in Praat:

- via @@Sound: To Pitch (raw autocorrelation)...@
  from the ##Analyse periodicity# menu in the Objects window when you select a Sound object;
- via ##Show Pitch# and ##Pitch analysis method is raw autocorrelation#
  from the #Pitch menu when you are viewing a Sound or TextGrid object (@SoundEditor, @TextGridEditor).

################################################################################
"Sound: To Pitch (raw cc)..."
© Paul Boersma 1996,2003,2023,2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

This command is a precise synonym of @@Sound: To Pitch (raw cross-correlation)...@.

################################################################################
"Sound: To Pitch (raw cross-correlation)..."
© Paul Boersma 1996,2003,2023,2024

A command that creates a @Pitch object from every selected @Sound object.

For purpose, algorithm and settings, see @@pitch analysis by raw cross-correlation@.

For usage, see @@how to choose a pitch analysis method@.

Synonyms
========

In older scripts you may find:
- @@Sound: To Pitch (raw cc)...@, a precise synonym with an identical list of parameters.
- @@Sound: To Pitch (cc)...@, a command with the same meaning, with parameters in a different order.

################################################################################
"pitch analysis by raw cross-correlation"
© Paul Boersma 1996,2003,2023,2024

Purpose
=======

to perform a pitch analysis based on a cross-correlation method.

Usage
=====

Raw cross-correlation is the pitch analysis method of choice if you want to do voice analysis (e.g. jitter, shimmer).

Note that the preferred method for speech (intonation, vocal fold vibration) is @@pitch analysis by filtered autocorrelation@.
Raw cross-correlation is mainly for experimenting,
or for applications where you need short time windows;
for this latter reason, it is also the standard method for voice analysis.

See @@how to choose a pitch analysis method@ for details.

Algorithm
=========

The algorithm performs an acoustic periodicity detection on the basis of a
forward cross-correlation analysis.

Settings
========

Several settings are already described in @@Intro 4.2. Configuring the pitch contour@.
The explanations below assume that you have gone through that part of the Intro.

##Time step (s)# (standard value: 0.0)
:	the measurement interval (frame duration), in seconds. If you supply 0,
Praat will use a time step of 0.25 / (%%pitch floor%), e.g. 0.00333333 seconds if the pitch floor is 75 Hz;
in this example, Praat computes 300 pitch values per second.

##Pitch floor (Hz)# (standard value: 75 Hz)
:	candidates below this frequency will not be recruited.
This parameter determines the length of the analysis window: it will be 1 longest period long,
i.e., if the pitch floor is 75 Hz, the window will be 1/75 = 0.01333333 seconds long.

Note that if you set the time step to zero, the analysis windows for consecutive measurements
will overlap appreciably: Praat will always compute 4 pitch values
within one window length, i.e., the degree of %oversampling is 4.

The other settings are the same as for @@pitch analysis by raw autocorrelation@.

Availability in Praat
=====================

Pitch analysis by raw cross-correlation is available in two ways in Praat:

- via @@Sound: To Pitch (raw cross-correlation)...@
  from the ##Analyse periodicity# menu in the Objects window when you select a Sound object;
- via ##Show Pitch# and ##Pitch analysis method is raw cross-correlation#
  from the #Pitch menu when you are viewing a Sound or TextGrid object (@SoundEditor, @TextGridEditor).

################################################################################
"Sound: To Pitch (filtered ac)..."
© Paul Boersma 2023,2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

This command is a precise synonym of @@Sound: To Pitch (filtered autocorrelation)...@.

################################################################################
"Sound: To Pitch (filtered autocorrelation)..."
© Paul Boersma 2023,2024

A command that creates a @Pitch object from every selected @Sound object.

For purpose, algorithm and settings, see @@pitch analysis by filtered autocorrelation@.

For usage, see @@how to choose a pitch analysis method@.

Synonyms
========

In older scripts you may find:
- @@Sound: To Pitch (filtered ac)...@, a precise synonym with an identical list of parameters.

################################################################################
"pitch analysis by filtered autocorrelation"
© Paul Boersma 2023,2024

A command that creates a @Pitch object from every selected @Sound object.

Purpose
=======

to perform a pitch analysis based on the autocorrelation of the low-pass filtered signal.

Usage
=====

Filtered autocorrelation is the pitch analysis method of choice if you want to measure intonation or vocal-fold vibration frequency.
See @@how to choose a pitch analysis method@ for details.

Algorithm
=========

This command will first low-pass filter the signal, then apply @@pitch analysis by raw autocorrelation@
on the filtered signal.

The low-pass filter is Gaussian in the frequency domain. If, for instance,
you set the %%pitch top% to 800 Hz, and the %%attenuation at top% to 0.03,
then the attenuation at 400 Hz is the fourth root of 0.03, i.e. about 42\% .
As a function of frequency %f, the attenuation is 0.03^^(%f/800)²^.
Here’s a table of attenuation factors, also in dB (in this logarithmic domain, the shape is parabolic):

| frequency | attenuation | logarithmic
|   100 Hz  |     0.95    |   -0.5 dB
|   200 Hz  |     0.80    |   -1.9 dB
|   300 Hz  |     0.61    |   -4.3 dB
|   400 Hz  |     0.42    |   -7.6 dB
|   500 Hz  |     0.25    |  -11.9 dB
|   600 Hz  |     0.14    |  -17.1 dB
|   700 Hz  |     0.07    |  -23.3 dB
|   800 Hz  |     0.03    |  -30.5 dB

Note: the attenuation curve will be identical to the curve shown here if you use a pitch top of 500 Hz
and an %%attenuation at top% of 0.25; however, this is not advised, because the example table
provides a more gradual suppression of higher pitches, almost as if there were no pitch top at all.

Settings
========

Several settings are already described in @@Intro 4.2. Configuring the pitch contour@.
The explanations below assume that you have gone through that part of the Intro.

The settings that control the recruitment of the candidates are:

##Time step (s)# (standard value: 0.0)
:	the measurement interval (frame duration), in seconds. If you supply 0,
Praat will use a time step of 0.75 / (%%pitch floor%), e.g. 0.015 seconds if the pitch floor is 50 Hz;
in this example, Praat computes 66.7 pitch values per second.

##Pitch floor (Hz)# (standard value: 50 Hz)
:	candidates below this frequency will not be recruited.
This parameter determines the effective length of the analysis window: it will be 3 longest periods long,
i.e., if the pitch floor is 50 Hz, the window will be effectively 3/50 = 0.06 seconds long.

Note that if you set the time step to zero, the analysis windows for consecutive measurements
will overlap appreciably: Praat will always compute 4 pitch values
within one window length, i.e., the degree of %oversampling is 4.

##Pitch top (Hz)# (standard value: 800 Hz)
:	candidates above this frequency will be ignored. Note, however, that candidates around one half of this (i.e. 400 Hz)
will already be reduced by 7.6 dB, i.e. they are already moderately disfavoured,
and that candidates around three-quarters of this (i.e. 600 Hz) will already be reduced by 17.1 dB,
i.e. they are strongly disfavoured. Hence, the %%pitch top%
needs to be be set much higher than the %%pitch ceiling% of @@pitch analysis by ||raw autocorrelation@,
which is why the standard is 800 Hz whereas the standard for raw autocorrelation can be 500 or 600 Hz.
To illustrate this, consider the search space for raw autocorrelation on the right (with a ceiling of 600 Hz)
and the search space for filtered autocorrelation on the right (with a top of 800 Hz):
{- 6x3
	colour# = { 1.0, 0.5, 0.5 }
	attenuationAtFloor = 0.03 ^ (50/800)^2
	info$ = Picture info
	fontSize = extractNumber: info$, "Font size:"

	Axes: 0, 1, 0, 1000
	Select outer viewport: 0, 3, 0, 3
	Draw inner box
	Marks bottom every: 1.0, 1.0, "yes", "yes", "no"
	One mark left: 50, "yes", "yes", "yes", ""
	One mark right: 50, "no", "yes", "no", "floor"
	One mark left: 600, "yes", "yes", "yes", ""
	One mark right: 600, "no", "yes", "no", "ceiling"
	Text bottom: "no", "attenuation"
	Text left: "yes", "Pitch (Hz)"
	Colour: colour#
	Line width: 3
	Draw line: 0, 1000, 0, 600
	Draw line: 0, 600, 1, 600
	Draw line: 1, 600, 1, 50
	Draw line: 1, 50, 0, 50
	Draw line: 0, 50, 0, 0
	Red
	Text: 0.5, "centre", 50, "bottom", "%%floor"
	Text: 0.5, "centre", 600, "bottom", "%%ceiling"
	Line width: 1
	Black

	Select outer viewport: 3, 6, 0, 3
	Draw inner box
	Marks bottom every: 1.0, 1.0, "yes", "yes", "no"
	One mark left: 50, "yes", "yes", "yes", ""
	One mark right: 50, "no", "yes", "no", "floor"
	One mark left: 100, "yes", "yes", "yes", ""
	One mark left: 200, "yes", "yes", "yes", ""
	One mark left: 400, "yes", "yes", "yes", ""
	One mark left: 800, "yes", "yes", "yes", ""
	One mark right: 800, "no", "yes", "no", "top"
	Text bottom: "no", "attenuation"
	Text left: "yes", "Pitch (Hz)"
	Colour: colour#
	Line width: 3
	Draw function: 0, attenuationAtFloor, 300, ~ min (800, 800 * sqrt (ln (x) / ln (0.03)))
	Draw line: 0, 800, 0, 1000
	Draw line: attenuationAtFloor, 50, 0, 50
	Draw line: 0, 50, 0, 0
	Red
	Text: 0.5, "centre", 50, "bottom", "%%floor"
	Text special: 0.5, "centre", 360, "bottom", "Times", fontSize, "-35", "%%ceiling"
	Line width: 1
	Black
}
: Because of the reduction in strength of high pitch candidates, it may be preferable
to view pitch on a logarithmic pitch scale, so that the suppressed top octave (from 400 to 800 Hz)
occupies less space:
{- 3x3
	Axes: 0, 1, log10 (40), log10(1000)
	Draw inner box
	Marks bottom every: 1.0, 1.0, "yes", "yes", "no"
	One logarithmic mark left: 50, "yes", "yes", "yes", ""
	One logarithmic mark right: 50, "no", "yes", "no", "floor"
	One logarithmic mark left: 100, "yes", "yes", "yes", ""
	One logarithmic mark left: 200, "yes", "yes", "yes", ""
	One logarithmic mark left: 400, "yes", "yes", "yes", ""
	One logarithmic mark left: 800, "yes", "yes", "yes", ""
	One logarithmic mark right: 800, "no", "yes", "no", "top"
	Text bottom: "no", "attenuation"
	Text left: "yes", "pitch (Hz)"
	Colour: colour#
	Line width: 3
	Draw function: 0, attenuationAtFloor, 300, ~ log10 (min (800, 800 * sqrt (ln (x) / ln (0.03))))
	Draw line: 0, log10(800), 0, log10(1000)
	Draw line: attenuationAtFloor, log10(50), 0, log10(50)
	Draw line: 0, log10(50), 0, log10(40)
	Red
	Text: 0.5, "centre", log10(50), "bottom", "%%floor"
	Text special: 0.5, "centre", log10(360), "bottom", "Times", fontSize, "-30", "%%ceiling"
	Line width: 1
	Black
}
We could say that the whole range from 300 to 800 Hz can be regarded as a skewed “ceiling”.
This is why we distinguish between the terms “ceiling” and “top”. If you have a speaker
with an especially high F0, then you can raise the top to e.g. 1200 Hz;
the attenuation of higher candidates will then have the exact same shape:
{- 3x3
	Axes: 0, 1, log10 (40), log10(1500)
	Draw inner box
	Marks bottom every: 1.0, 1.0, "yes", "yes", "no"
	One logarithmic mark left: 50, "yes", "yes", "yes", ""
	One logarithmic mark right: 50, "no", "yes", "no", "floor"
	One logarithmic mark left: 75, "yes", "yes", "yes", ""
	One logarithmic mark left: 150, "yes", "yes", "yes", ""
	One logarithmic mark left: 300, "yes", "yes", "yes", ""
	One logarithmic mark left: 600, "yes", "yes", "yes", ""
	One logarithmic mark left: 1200, "yes", "yes", "yes", ""
	One logarithmic mark right: 1200, "no", "yes", "no", "top"
	Text bottom: "no", "attenuation"
	Text left: "yes", "pitch (Hz)"
	Colour: colour#
	Line width: 3
	attenuationAtFloor = 0.03 ^ (50/1200)^2
	Draw function: 0, attenuationAtFloor, 300, ~ log10 (min (1200, 1200 * sqrt (ln (x) / ln (0.03))))
	Draw line: 0, log10(1200), 0, log10(1500)
	Draw line: attenuationAtFloor, log10(50), 0, log10(50)
	Draw line: 0, log10(50), 0, log10(40)
	Red
	Text: 0.5, "centre", log10(50), "bottom", "%%floor"
	Text special: 0.5, "centre", log10(540), "bottom", "Times", fontSize, "-30", "%%ceiling"
	Line width: 1
	Black
}

##Max. number of candidates# (standard value: 15)
:	each frame will contain at least this many pitch candidates.
One of them is the “unvoiced candidate”; the others correspond to
time lags over which the signal is more or less similar to itself.

##Very accurate# (standard value: %off)
:	if %off, the window is a Hanning window with a physical length of  3 / (%%pitch floor%).
If %on, the window is a Gaussian window with a physical length of  6 / (%%pitch floor%),
i.e. twice the effective length.

A pre-processing algorithm filters the sound before the @@pitch analysis by raw autocorrelation@ begins.
The shape of the attenutation curve is determined not only by the height of the %pitch top% (in hertz),
but also by how wide it is (in the pictures above, it’s the tiny horizontal linepiece at the top):

##Attenuation at top# (standard value: 0.03)
:	this is how much the frequency components of the original sound have been attenuated at the top.
In the example table above, you can see that at the top (800 Hz) the sounds was attenuated by a factor of 0.03.
We known of no reasons to change this value, except for experimenting.

A post-processing algorithm seeks the cheapest path through the candidates.
The settings that determine the cheapest path are:

##Silence threshold# (standard value: 0.09)
:	frames that do not contain amplitudes above this threshold (relative to the global maximum amplitude),
are probably silent.

##Voicing threshold# (standard value: 0.50)
:	the strength of the unvoiced candidate, relative to the maximum possible autocorrelation.
If the amount of periodic energy in a frame is more than this fraction of the total energy (the remainder being noise),
then Praat will prefer to regard this frame as voiced; otherwise as unvoiced.
To increase the number of unvoiced decisions, increase the voicing threshold.

##Octave cost# (standard value: 0.055 per octave)
:	degree of favouring of high-frequency candidates, relative to the maximum possible autocorrelation.
This is necessary because even (or: especially) in the case of a perfectly periodic signal,
all undertones of %F_0 are equally strong candidates as %F_0 itself.
To more strongly favour recruitment of high-frequency candidates, increase this value.

##Octave-jump cost# (standard value: 0.35)
:	degree of disfavouring of pitch changes, relative to the maximum possible autocorrelation.
To decrease the number of large frequency jumps, increase this value. In contrast with what is described
in the article (@@Boersma (1993)@), this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get
the value in the way it is used in the formulas in the article.

##Voiced / unvoiced cost# (standard value: 0.14)
:	degree of disfavouring of voiced/unvoiced transitions, relative to the maximum possible autocorrelation.
To decrease the number of voiced/unvoiced transitions, increase this value. In contrast with what is described
in the article, this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get
the value in the way it is used in the formulas in the article.

The standard settings are best in most cases.
For some pathological voices, you will want to set the voicing threshold to much less than the standard of 0.50,
in order to get pitch values even in irregular parts of the signal.
For prevoiced plosives, you may want to lower the silence threshold from 0.09 to 0.01 or so.

Availability in Praat
=====================

Pitch analysis by filtered autocorrelation is available in two ways in Praat:

- via @@Sound: To Pitch (filtered autocorrelation)...@
  from the ##Analyse periodicity# menu in the Objects window when you select a Sound object;
- via ##Show Pitch# and ##Pitch analysis method is filtered autocorrelation#
  from the #Pitch menu when you are viewing a Sound or TextGrid object (@SoundEditor, @TextGridEditor).

################################################################################
"Sound: To Pitch (filtered cc)..."
© Paul Boersma 2023,2024

A @@hidden commands|hidden command@ that creates a @Pitch object from every selected @Sound object.

This command is a precise synonym of @@Sound: To Pitch (filtered cross-correlation)...@.

################################################################################
"Sound: To Pitch (filtered cross-correlation)..."
© Paul Boersma 2023,2024

A command that creates a @Pitch object from every selected @Sound object.

For purpose, algorithm and settings, see @@pitch analysis by filtered cross-correlation@.

For usage, see @@how to choose a pitch analysis method@.

Synonyms
========

In older scripts you may find:
- @@Sound: To Pitch (filtered cc)...@, a precise synonym with an identical list of parameters.

################################################################################
"pitch analysis by filtered cross-correlation"
© Paul Boersma 2023,2024

A command that creates a @Pitch object from every selected @Sound object.

Purpose
=======

to perform a pitch analysis based on the forward cross-correlation of the low-pass filtered signal.

Usage
=====

We know of no situation yet in which this method is preferred over @@pitch analysis by filtered autocorrelation@
(which seems better for measuring intonation and vocal-fold vibration frequency),
@@pitch analysis by raw cross-correlation@ (which is definitely better for voice analysis, such as jitter and shimmer),
or @@pitch analysis by raw autocorrelation@ (which is better for determining raw periodicity).
It is possible that filtered cross-correlation can play a role in voice analysis in the future,
in combination with one of the other methods; this is yet to be determined.

Algorithm
=========

This command will first low-pass filter the signal, then apply @@pitch analysis by raw cross-correlation@
on the filtered signal. For the filter function, see @@pitch analysis by filtered autocorrelation@.

Settings
========

The settings are the same as for @@pitch analysis by filtered autocorrelation@.

Availability in Praat
=====================

Pitch analysis by raw cross-correlation is available in two ways in Praat:

- via @@Sound: To Pitch (raw cross-correlation)...@
  from the ##Analyse periodicity# menu in the Objects window when you select a Sound object;
- via ##Show Pitch# and ##Pitch analysis method is raw cross-correlation#
  from the #Pitch menu when you are viewing a Sound or TextGrid object (@SoundEditor, @TextGridEditor).

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Sound: To PointProcess (periodic, cc)...", U"ppgb", 20231115)   // 20030309
INTRO (U"A command that analyses the selected @Sound objects, and creates @PointProcess objects.")
NORMAL (U"This command combines the actions of @@Sound: To Pitch (raw autocorrelation)...@ and @@Sound & Pitch: To PointProcess (cc)@.")
MAN_END

MAN_BEGIN (U"Sound: To PointProcess (periodic, peaks)...", U"ppgb", 20231115)   // 20030309
INTRO (U"A command that analyses the selected @Sound objects, and creates @PointProcess objects.")
NORMAL (U"This command combines the actions of @@Sound: To Pitch (raw autocorrelation)...@ and @@Sound & Pitch: To PointProcess (peaks)...@.")
MAN_END

MAN_BEGIN (U"Sound & Pitch: To PointProcess (cc)", U"ppgb", 19980322)
INTRO (U"A command to create a @PointProcess from the selected @Sound and @Pitch objects.")
ENTRY (U"Purpose")
NORMAL (U"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (U"Algorithm")
NORMAL (U"The voiced intervals are determined on the basis of the voiced/unvoiced decisions in the @Pitch object. "
	"For every voiced interval, a number of %points (or glottal pulses) is found as follows:")
LIST_ITEM (U"1. The first point %t__1_ is the absolute extremum of the amplitude of the #Sound, "
	"between %t__%mid_ \\-- %T__0_ / 2 and %t__%mid_ + %T__0_ / 2, where %t__%mid_ is the midpoint of the interval, "
	"and %T__0_ is the period at %t__%mid_, as can be interpolated from the #Pitch contour.")
LIST_ITEM (U"2. From this point, we recursively search for points %t__%i_ to the left "
	"until we reach the left edge of the interval. These points must be located between "
	"%t__%i\\--1_ \\-- 1.2 %T__0_(%t__%i\\--1_) and %t__%i\\--1_ \\-- 0.8 %T__0_(%t__%i\\--1_), "
	"and the cross-correlation of the amplitude "
	"in its environment [%t__%i_ \\-- %T__0_(%t__%i_) / 2; %t__%i_ + %T__0_(%t__%i_) / 2] "
	"with the amplitude of the environment of the existing point %t__%i\\--1_ must be maximal "
	"(we use parabolic interpolation between samples of the correlation function).")
LIST_ITEM (U"3. The same is done to the right of %t__1_.")
LIST_ITEM (U"4. Though the voiced/unvoiced decision is initially taken by the #Pitch contour, "
	"points are removed if their correlation value is less than 0.3; "
	"furthermore, one extra point may be added at the edge of the voiced interval "
	"if its correlation value is greater than 0.7.")
MAN_END

MAN_BEGIN (U"Sound & Pitch: To PointProcess (peaks)...", U"ppgb", 20070722)
INTRO (U"A command to create a @PointProcess from the selected @Sound and @Pitch objects.")
ENTRY (U"Purpose")
NORMAL (U"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (U"Algorithm")
NORMAL (U"The voiced intervals are determined on the basis of the voiced/unvoiced decisions in the @Pitch object. "
	"For every voiced interval, a number of %points (or glottal pulses) is found as follows:")
LIST_ITEM (U"1. The first point %t__1_ is the absolute extremum (or the maximum, or the minimum, depending on "
	"your %%Include maxima% and %%Include minima% settings) of the amplitude of the #Sound, "
	"between %t__%mid_ \\-- %T__0_ / 2 and %t__%mid_ + %T__0_ / 2, where %t__%mid_ is the midpoint of the interval, "
	"and %T__0_ is the period at %t__%mid_, as can be interpolated from the #Pitch contour.")
LIST_ITEM (U"2. From this point, we recursively search for points %t__%i_ to the left "
	"until we reach the left edge of the interval. These points are the absolute extrema (or the maxima, or the minima) "
	"between the times "
	"%t__%i\\--1_ \\-- 1.2 %T__0_(%t__%i\\--1_) and %t__%i\\--1_ \\-- 0.8 %T__0_(%t__%i\\--1_).")
LIST_ITEM (U"3. The same is done to the right of %t__1_.")
NORMAL (U"The periods that are found in this way are much more variable than those found by "
	"@@Sound & Pitch: To PointProcess (cc)@, and therefore less useful for analysis and subsequent @@overlap-add@ synthesis.")
MAN_END

}

/* End of file manual_pitch.cpp */
