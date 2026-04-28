/* manual_Fon.cpp
 *
 * Copyright (C) 1992-2008,2010,2011,2014-2017,2019-2025 Paul Boersma
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

void manual_Sampling_init (ManPages me);
void manual_sound_init (ManPages me);
void manual_pitch_init (ManPages me);
void manual_spectrum_init (ManPages me);
void manual_formant_init (ManPages me);
void manual_pointProcess_init (ManPages me);
void manual_annotation_init (ManPages me);
void manual_SpeechRecognizer_init (ManPages me);

void manual_Fon_init (ManPages me);
void manual_Fon_init (ManPages me) {

manual_Sampling_init (me);
manual_sound_init (me);
manual_pitch_init (me);
manual_spectrum_init (me);
manual_formant_init (me);
manual_pointProcess_init (me);
manual_annotation_init (me);
manual_SpeechRecognizer_init (me);


MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Get high index from time..."
© Paul Boersma 2010-12-30

A @@Query submenu|query@ to ask the selected tier object
(@DurationTier, @IntensityTier, @PitchTier)
which point is nearest to, but no earlier than, the specified time.

Setting
=======
##Time (s)
:	the time from which you want to get the point index.

Return value
============
This query returns the index of the point with the lowest time greater than or equal to #Time.
It is @undefined if there are no points.
It is the number of points plus 1 (offright) if the specified time is greater than the time of the last point.

################################################################################
"Get low index from time..."
© Paul Boersma 2010-12-30

A @@Query submenu|query@ to ask the selected tier object
(@DurationTier, @IntensityTier, @PitchTier)
which point is nearest to, but no later than, the specified time.

Setting
=======
##Time (s)
:	the time from which you want to get the point index.

Return value
============
This query returns the index of the point with the highest time less than or equal to #Time.
It is @undefined if there are no points.
It is 0 (offleft) if the specified time is less than the time of the first point.

################################################################################
"Get nearest index from time..."
© Paul Boersma 2010, 2023-04-14

A @@Query submenu|query@ to ask the selected tier object
(@DurationTier, @IntensityTier, @PitchTier)
which point is nearest to the specified time.

Setting
=======
##Time (s)
:	the time near which you want to get the point index.

Return value
============
This query returns the index of the point whose time is closest to #Time.
It is @undefined if there are no points.

################################################################################
"Remove point..."
© Paul Boersma 2010-12-30

A command to remove one point from every selected time-based tier object
(@DurationTier, @IntensityTier, @PitchTier).

Setting
=======
##Point number
:	the index of the point you want to remove.

Behaviour
=========
If ##Point number# is 3, the third point counted from the start of the tier (if it exists)
is removed from the tier.

################################################################################
"Remove point near..."
© Paul Boersma 2010-12-30

A command to remove one point from every selected time-based tier object
(@DurationTier, @IntensityTier, @PitchTier).

Setting
=======

##Time (s)
:	the time near which you want to remove a point.

Behaviour
=========
The point nearest to #Time (if there is any point) is removed from the tier.

################################################################################
"Remove points between..."
© Paul Boersma 2010-12-30

A command to remove some points from every selected time-based tier object
(@DurationTier, @IntensityTier, @PitchTier).

Settings
========
##From time (s), To time (s)
:	the times between which you want to remove all points.

Behaviour
=========
Any points between ##From time# and ##To Time# (inclusive) are removed from the tier.

################################################################################
"AmplitudeTier"
© Paul Boersma 2007

One of the @@types of objects@ in Praat.
An AmplitudeTier object represents a time-stamped amplitude contour,
i.e., it contains a series of (%time, %amplitude) points.
The amplitude values are in Pascal. To see some applications, consult the @IntensityTier information;
the difference between an AmplitudeTier and an IntensityTier is that the former has values in Pascal
which multiply linearly with a Sound (for instance), and the latter has values in dB,
which multiply logarithmically with a Sound.

################################################################################
"AmplitudeTier: Add point..."
© Paul Boersma 2023
I
A command to add a point to each selected @AmplitudeTier.

Settings
========
##Time (s)
: the time at which a point is to be added.

##Sound pressure (Pa)
: the amplitude value of the requested new point.

Behaviour
=========
The tier is modified so that it contains the new point.
If a point at the specified time was already present in the tier, nothing happens.

################################################################################
"Cochleagram"
© Paul Boersma 2003-03-16

One of the @@types of objects@ in Praat. It represents the excitation pattern
of the basilar membrane in the inner ear (see @Excitation) as a function of time.

################################################################################
"Cochleagram: Formula..."
© Paul Boersma 2002-12-06

A command for changing the data in all selected @Cochleagram objects.
See the @Formulas tutorial for examples and explanations.

################################################################################
"Create AmplitudeTier..."
© Paul Boersma 2023

A command in the @@New menu@ to create an empty @AmplitudeTier object.
The resulting object will have the specified name and time domain, but contain no amplitude points.
To add some points to it, use @@AmplitudeTier: Add point...@.

################################################################################
"Create DurationTier..."
© Paul Boersma 2014-04-21, 2023-04-30

A command in the @@New menu@ to create an empty @DurationTier object.
The resulting object will have the specified name and time domain, but contain no duration points.
To add some points to it, use @@DurationTier: Add point...@.

Example
=======
To create a tier 0.9 seconds long, with an deceleration around 0.6 seconds,
you do (either in a script or by choosing these 4 commands by hand):
{
	\#{Create DurationTier:} “dur”, 0, 0.9
	\@{DurationTier: ||Add point:} 0.3, 1
	Add point: 0.6, 2.3
	Add point: 0.7, 1
}
The result will look like
{- 5x2.5
	x# = { 0.0, 0.3, 0.6, 0.7, 0.9 }
	y# = { 1.0, 1.0, 2.3, 1.0, 1.0 }
	Red
	Create Polygon from values: "durationTier", x#, y#
	Draw: 0.0, 0.9, 0.0, 2.5
	Black
	Draw inner box
	Marks left every: 1.0, 1.0, "yes", "yes", "no"
	Marks bottom every: 1.0, 0.1, "yes", "yes", "no"
	Text bottom: "yes", "Time (s)"
	Text left: "yes", "Relative duration"
}
The target duration will be the area under this curve, which is 0.9 + 1/2 · 1.3 · 0.4 = 1.16 seconds.

################################################################################
"Create IntensityTier..."
© Paul Boersma 2002-12-04

A command in the @@New menu@ to create an empty @IntensityTier object.
The resulting object will have the specified name and time domain, but contain no intensity points.
To add some points to it, use @@IntensityTier: Add point...@.

################################################################################
"Create Matrix..."
© Paul Boersma 2002-12-12

A command in the #Matrix submenu of the #Generics submenu of the @@New menu@,
to create a @Matrix with the specified sampling attributes,
filled with values from a formula (see @@Matrix: Formula...@).

################################################################################
"Create simple Matrix..."
© Paul Boersma 2002-12-04

A command in the #Matrix submenu of the #Generics submenu of the @@New menu@,
to create a @Matrix with the specified number of rows and columns,
filled with values from a formula (see @@Matrix: Formula...@).

################################################################################
"Create simple Matrix from values..."
© Paul Boersma 2023

A command in the #Matrix submenu of the #Generics submenu of the @@New menu@,
to create a @Matrix with the specified cells.

################################################################################
"Create Photo..."
© Paul Boersma 2023

A command in the #Photo submenu of the #Generics submenu of the @@New menu@,
to create a @Photo with the specified sampling attributes,
filled with values from a formula for each of the three colour channels.

################################################################################
"Create simple Photo..."
© Paul Boersma 2023

A command in the #Photo submenu of the #Generics submenu of the @@New menu@,
to create a @Photo with the specified number of rows and columns,
filled with values from a formula for each of the three colour channels.

################################################################################
"Create Strings as folder list..."
© Paul Boersma 2006(with “directory list”),2020,2024

A command in the @@New menu@ to create a @Strings object containing a list of folders in a given parent folder.
It works completely analogously to @@Create Strings as file list...@.

Script usage
============
See @@Create Strings as file list...@.
If you don’t need the resulting @Strings object,
it may be easier to use @`folderNames$#` instead.

################################################################################
"Create Strings as directory list..."

A synonym for @@Create Strings as folder list...@.

You can see this command in older scripts, but not in Praat’s menus.
This command is kept for reasons of compatibility.

################################################################################
"Create Strings as file list..."
© Paul Boersma 1998,2006,2013-2015,2020,2024,2025

A command in the @@New menu@ to create a @Strings object containing a list of files in a given folder.

Settings
========
{- 5.4x2.44
)~~~"
	Manual_DRAW_SETTINGS_WINDOW ("Create Strings as file list", 2.6)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Name", "fileList")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("File path", "/Users/miep/Sounds/*.wav")
R"~~~(
}

##Name
: the name of the resulting Strings object.

##File path
: the folder name, with optional %wildcards (see below) for selecting files.

Behaviour
=========
The resulting Strings object will contain an alphabetical list of file names (by naïve Unicode-sorting),
without the preceding path through the folder structures. If there are no files that match the file path,
the Strings object will contain no strings.

Usage
=====
There are two ways to specify the file path.

One way is to specify a folder name only. On Unix, the file path could be
`/usr/people/miep/sounds` or `/usr/people/miep/sounds/`, for instance. On Windows,
`C:\Users\Miep\Sounds` or `C:\Users\Miep\Sounds\`.
On Macintosh, `/Users/miep/Sounds` or `/Users/miep/Sounds/`. Any of these produce
a list of all the files in the specified folder.

The other way is to specify a %wildcard (an asterisk) for the file names.
To get a list of all the files whose names start with “`hal`” and end in “`.wav`”,
type `/usr/people/miep/sounds/hal*.wav`, `C:\Users\Miep\Sounds\hal*.wav`,
or `/Users/miep/Sounds/hal*.wav`.

You can even use %two wildcards: `/usr/people/miep/sounds/*al*.wav` gives you a list
of all files whose names contain `al` and end in `wav`.

Script usage
============
In a script, you can use this command to cycle through the files in a folder.
For instance, to read in all the sound files in a specified folder,
you could use the following script:
{;
	folder$ = “/usr/people/miep/sounds”
	strings = \#{Create Strings as file list:} “list”, folder$ + “/*.wav”
	numberOfFiles = Get number of strings
	for ifile to numberOfFiles
		\`{selectObject}: strings
		fileName$ = Get string: ifile
		\@{Read from file:} folder$ + “/” + fileName$
	endfor
	removeObject: strings
}
If the script has been saved to a script file, you can use file paths that are relative to the folder
where you saved the script. Thus, with
{;
	\#{Create Strings as file list:} “list”, “*.wav”
}
you get a list of all the `.wav` files that are in the same folder as the script that contains this line.
And to get a list of all the `.wav` files in the folder `Sounds` that resides in the same folder as your script,
you can do
{;
	\#{Create Strings as file list:} “list”, “Sounds/*.wav”
}
As is usual in Praat scripting, the forward slash (“/”) in this example can be used on all platforms, including Windows.
This makes your script portable across platforms.

Note that the above functionality can also be written four lines shorter, using built-in functions:
{;
	folder$ = “/usr/people/miep/sounds”
	list$# = \#`{fileNames$#}: folder$ + “/*.wav”
	for ifile to \`{size} (list$#)
		\@{Read from file:} folder$ + “/” + list$# [ifile]
	endfor
}
This doesn’t produce a Strings object.

See also
========
To get a list of folders instead of files, use @@Create Strings as folder list...@.

################################################################################
"Photo"
© Paul Boersma 2023

One of the @@types of objects@ in Praat. It is comparable to a @Matrix
but has a matrix of cells for each of three colour channels: red, green and blue.

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Distributions", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. Inherits most actions from @TableOfReal.")
ENTRY (U"Actions")
LIST_ITEM (U"@@Distributions: To Strings...@")
MAN_END

MAN_BEGIN (U"Distributions: To Strings...", U"ppgb", 19971022)
INTRO (U"A command to create a @Strings object from every selected @Distributions object.")
ENTRY (U"Settings")
TERM (U"##Column number")
DEFINITION (U"the column (in the #Distributions object) that contains the distribution that you are "
	"interested in. Often the #Distributions object will only contain a single distribution, "
	"so this argument will often be 1. If the #Distributions object contains nine distributions, "
	"specify any number between 1 and 9.")
TERM (U"##Number of strings")
DEFINITION (U"the number of times a string will be drawn from the chosen distribution. "
	"This is the number of strings that the resulting @Strings object is going to contain.")
ENTRY (U"Behaviour")
NORMAL (U"Every string in the resulting #Strings object will be a row label of the #Distributions object. "
	"The number in each row at the specified column will be considered the relative frequency of "
	"occurrence of that row.")
NORMAL (U"%#Example. Suppose we have the following #Distributions:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Distributions\"")
CODE (U"2 (number of columns)")
CODE (U"         \"English\"  \"French\" (column labels)")
CODE (U"3 (number of rows)")
CODE (U"\"the\"     108        1.5")
CODE (U"\"a\"       58.1       33")
CODE (U"\"pour\"    0.7        15.5")
NORMAL (U"If you set %Column to 1 and %%Number of strings% to 1000, "
	"you will get a @Strings object with approximately 647 occurrences of “the”, "
	"348 occurrences of “a”, and 4 occurrences of “pour”. "
	"If you had set %Column to 2 (“French”), you would have gotten about "
	"30 times “the”, 660 times “a”, and 310 times “pour”. "
	"The actual numbers will vary because the choice of a string will not depend on previous choices.")
MAN_END

MAN_BEGIN (U"DurationTier", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. "
	"A DurationTier object contains a number of (%time, %duration) points, "
	"where %duration is to be interpreted as a relative duration (e.g. the duration of a manipulated sound "
	"as compared to the duration of the original). For instance, "
	"if your DurationTier contains two points, one with a duration value of 1.5 at a time of 0.5 seconds "
	"and one with a duration value of 0.6 at a time of 1.1 seconds, this is to be interpreted as "
	"a relative duration of 1.5 (i.e. a slowing down) for all original times before 0.5 seconds, "
	"a relative duration of 0.6 (i.e. a speeding up) for all original times after 1.1 seconds, "
	"and a linear interpolation between 0.5 and 1.1 seconds (e.g. a relative duration of "
	"1.2 at 0.7 seconds, and of 0.9 at 0.9 seconds).")
NORMAL (U"See @@Intro 8.2. Manipulation of duration@ and @@Create DurationTier...@.")
MAN_END

MAN_BEGIN (U"DurationTier: Add point...", U"ppgb", 20030216)
INTRO (U"A command to add a point to each selected @DurationTier. "
	"For an example, see @@Create DurationTier...@.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
TERM (U"##Relative duration")
DEFINITION (U"the relative duration value of the requested new point.")
ENTRY (U"Behaviour")
NORMAL (U"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (U"DurationTier: Get target duration...", U"ppgb", 20101228)
INTRO (U"A @@Query submenu|query@ to the selected @DurationTier for the target duration of a specified time range.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the start and end of the (original) time range.")
ENTRY (U"Return value")
NORMAL (U"the target duration in seconds.")
MAN_END

MAN_BEGIN (U"DurationTierEditor", U"ppgb", 20110128)
INTRO (U"One of the @editors in the Praat program, for viewing and editing a @DurationTier object. "
	"To create a DurationTierEditor window, select a DurationTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (U"Editors", U"ppgb", 20110128)
INTRO (U"Many @@types of objects@ in Praat can be viewed and edited in their own windows.")
ENTRY (U"Editor windows")
LIST_ITEM (U"• @SoundEditor")
LIST_ITEM (U"• @LongSoundEditor")
LIST_ITEM (U"• @TextGridEditor")
LIST_ITEM (U"• @ManipulationEditor")
LIST_ITEM (U"• @SpectrumEditor")
LIST_ITEM (U"• @PitchEditor")
LIST_ITEM (U"• @PointEditor")
LIST_ITEM (U"• @PitchTierEditor")
LIST_ITEM (U"• @IntensityTierEditor")
LIST_ITEM (U"• @DurationTierEditor")
LIST_ITEM (U"• #SpectrogramEditor")
LIST_ITEM (U"• #ArtwordEditor")
LIST_ITEM (U"• @OTGrammarEditor")
LIST_ITEM (U"• (any type: @Inspect)")
ENTRY (U"How to open an editor for an object")
NORMAL (U"To open an editor window for an object in the list, select the object and choose ##View & Edit# "
	"(if the ##View & Edit# button exists, it is usually at the top of the @@Dynamic menu@). "
	"The name of the object will appear as the title of the editor window.")
NORMAL (U"Objects that cannot be modified (e.g. LongSound) just have the command #View instead of ##View & Edit#.")
ENTRY (U"General behaviour")
NORMAL (U"Changes that you make to an object in its editor window will take effect immediately. "
	"For instance, you do not have close the editor window before saving the changed object to disk.")
NORMAL (U"If you @Remove an object that you are viewing or editing from the @@List of Objects@, "
	"the editor window will automatically disappear from the screen.")
NORMAL (U"All editors are independent windows: you can minimize and maximize them; "
	"if an editor window goes hiding behind another window, "
	"you can raise it by choosing the ##View & Edit# command again.")
NORMAL (U"If you rename an object that you are viewing or editing (with @@Rename...@), "
	"the title of the editor window immediately changes to the new name.")
ENTRY (U"Ways to control an editor window")
LIST_ITEM (U"• @@Click")
LIST_ITEM (U"• @@Shift-click")
LIST_ITEM (U"• @@Drag")
LIST_ITEM (U"• @@Shift-drag")
LIST_ITEM (U"• @@Time selection")
LIST_ITEM (U"• @@Keyboard shortcuts")
MAN_END

MAN_BEGIN (U"Excitation", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. It represents the excitation pattern "
	"of the basilar membrane in the inner ear.")
ENTRY (U"Inside an Excitation object")
NORMAL (U"With @Inspect, you will see the following attributes.")
TERM (U"%xmin = 0")
DEFINITION (U"minimum place or frequency (Bark).")
TERM (U"%xmax = 25.6 Bark")
DEFINITION (U"maximum place or frequency (Bark).")
TERM (U"%nx")
DEFINITION (U"number of places or frequencies.")
TERM (U"%dx = 25.6 / %nx")
DEFINITION (U"Place or frequency step (Bark).")
TERM (U"%x1 = %dx / 2")
DEFINITION (U"centre of first place or frequency band (Bark).")
TERM (U"%ymin = %ymax = %dy = %y_1 = 1; %ny = 1")
DEFINITION (U"dummies.")
TERM (U"%z [1]")
DEFINITION (U"intensity (sensation level) in phon.")
MAN_END

MAN_BEGIN (U"Excitation: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Excitation objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Excitation: Get loudness", U"ppgb", 19991016)
INTRO (U"A @@Query submenu|query@ to ask the selected @Excitation object for its loudness.")
ENTRY (U"Return value")
NORMAL (U"the loudness in sone units.")
ENTRY (U"Algorithm")
NORMAL (U"The loudness is defined as")
EQUATION (U"∫%df 2^^(%e(%f) - 40 phon) / 10^")
NORMAL (U"where %f is the frequency in Bark, and %e(%f) the excitation in phon. "
	"For our discrete Excitation object, the loudness is computed as")
EQUATION (U"Δ%f ∑ 2^^(%e_%i - 40) / 10")
NORMAL (U"where Δ%f is the distance between the excitation channels (in Bark).")
MAN_END

/*"Any object of one of the types Polygon, PointProcess, Sound, Pitch, Spectrum,\n"
"Spectrogram, Excitation, Cochleagram, VocalTract\n"
"is convertible to and from a Matrix.\n"
"This allows extra hacking and drawing possibilities.\n"
"For example, to square the y values of a Polygon, do the following:\n"
"   select a Polygon, choose ##To Matrix#, choose ##Formula...#,\n"
"   type \"self * if row=1 then 1 else self fi\", choose ##To Polygon#."*/

MAN_BEGIN (U"Formula...", U"ppgb", 19980319)
NORMAL (U"See @@Matrix: Formula...@")
MAN_END

MAN_BEGIN (U"Frequency selection", U"ppgb", 20010402)
INTRO (U"The way to select a frequency domain in the @SpectrumEditor. "
	"This works completely analogously to the @@time selection@ in other editors.")
MAN_END

MAN_BEGIN (U"Get area...", U"ppgb", 20030216)
INTRO (U"A @@Query submenu|query@ to the selected tier object (@PitchTier, @IntensityTier, @DurationTier).")
ENTRY (U"Return value")
NORMAL (U"the area under the curve.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
TERM (U"##To time (s)")
DEFINITION (U"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area is")
EQUATION (U"1/2 (%f__1_ + %f__2_) (%t__2_ – %t__1_)")
MAN_END

MAN_BEGIN (U"Intensity", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An Intensity object represents an intensity contour at linearly spaced time points "
	"%t__%i_ = %t__1_ + (%i – 1) %dt, with values in dB SPL, i.e. dB relative to 2·10^^-5^ Pascal, "
	"which is the normative auditory threshold for a 1000-Hz sine wave.")
MAN_END

MAN_BEGIN (U"Intensity: Get maximum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the maximum value within the specified time domain, expressed in dB.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TERM (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get mean...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the mean (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TERM (U"##Averaging method")
DEFINITION (U"the units in which the averaging is performed. If the method is #energy, "
	"the returned dB value is based on the mean power (in Pa^2/s) between %t__1_ and %t__2_. "
	"If the method is #dB, the returned value is the mean of the intensity curve in dB. "
	"If the method is #sones, the returned value is in between these two, "
	"and based on averaging properties of the human ear.")
ENTRY (U"Algorithm")
NORMAL (U"If the averaging method is #dB, the mean intensity between the times %t__1_ and %t__2_ is defined as")
EQUATION (U"1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL (U"where %x(%t) is the intensity as a function of time, in dB. If the method is #energy, the result is")
EQUATION (U"10 log__10_ { 1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ 10^^%x(%t)/10^ %dt }")
NORMAL (U"If the method is #sones, the result is")
EQUATION (U"10 log__2_ { 1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ 2^^%x(%t)/10^ %dt }")
ENTRY (U"Behaviour")
NORMAL (U"After you do @@Sound: To Intensity...@, the mean intensity of the resulting #Intensity, "
	"if the averaging method is #energy, should be close to the mean SPL of the original #Sound, "
	"which can be found with #Info.")
MAN_END

MAN_BEGIN (U"Intensity: Get minimum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the minimum value within a specified time domain, expressed in dB.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get standard deviation...", U"ppgb", 20041107)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (U"Settings")
TERM (U"%%Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The standard deviation between the times %t__1_ and %t__2_ is defined as")
EQUATION (U"√ {1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ %dt (%x(%t) - %μ)^2}")
NORMAL (U"where %x(%t) is the intensity (in dB) as a function of time, and %μ its mean. "
	"For our discrete Intensity object, the standard deviation is approximated by")
EQUATION (U"√ {1/(%n-1) ∑__%i=%m..%m+%n-1_ (%x__%i_ - %μ)^2}")
NORMAL (U"where %n is the number of frames between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (U"Intensity: Get time of maximum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the maximum intensity within a specified time domain.")
ENTRY (U"Settings")
TERM (U"%%Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TERM (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get time of minimum...", U"ppgb", 20200912)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the minimum intensity within a specified time domain.")
ENTRY (U"Settings")
TERM (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get value at time...", U"ppgb", 20030916)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the intensity (in dB) at a specified time. If %time is outside the frames of the Intensity, the result is 0.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which the value is to be evaluated.")
TERM (U"##Interpolation")
DEFINITION (U"the interpolation method, see @@vector value interpolation@. "
	"The standard is “cubic” because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get value in frame...", U"ppgb", 19991016)
INTRO (U"A @@Query submenu|query@ to the selected @Intensity object.")
ENTRY (U"Setting")
TERM (U"##Frame number")
DEFINITION (U"the frame whose value is to be looked up.")
ENTRY (U"Return value")
NORMAL (U"the intensity value (in dB) in the specified frame. "
	"If the index is less than 1 or greater than the number of frames, the result is 0; "
	"otherwise, it is %z [1] [%%frame number%].")
MAN_END

MAN_BEGIN (U"Intensity: To IntensityTier", U"ppgb", 19970321)
INTRO (U"A command to convert each selected @Intensity object to an @IntensityTier.")
ENTRY (U"Behaviour")
NORMAL (U"Every sample in the @Intensity object is copied to a point on the @IntensityTier.")
ENTRY (U"Postconditions")
DEFINITION (U"Equal time domains:")
LIST_ITEM (U"• %result. %xmin == %intensity. %xmin")
LIST_ITEM (U"• %result. %xmax == %intensity. %xmax")
DEFINITION (U"Equal number of points:")
LIST_ITEM (U"• %result. %points. %size == %intensity. %nx")
NORMAL (U"For all points %i = 1 ... %intensity. %nx:")
DEFINITION (U"   Explicit times:")
LIST_ITEM (U"   • %result. %points. %item [%i]. %time == %intensity. %x1 + (%i – 1) * %intensity. %dx")
DEFINITION (U"   Equal number of points:")
LIST_ITEM (U"   • %result. %points. %item [%i]. %value == %intensity. %z [1] [%i]")
MAN_END

MAN_BEGIN (U"Intensity & PointProcess: To IntensityTier...", U"ppgb", 20101230)
INTRO (U"A command to copy information from an @Intensity, at times specified by a @PointProcess, "
	"to points on an @IntensityTier.")
ENTRY (U"Behaviour")
NORMAL (U"For all the times of the points in the PointProcess, an intensity is computed from the "
	"information in the Intensity object, by linear interpolation.")
MAN_END

MAN_BEGIN (U"IntensityTier", U"ppgb", 20101230)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An IntensityTier object represents a time-stamped intensity contour, i.e., it contains a series of (%time, %intensity) points. "
	"The intensity values are in dB.")
NORMAL (U"For examples, see @@Source-filter synthesis@.")
ENTRY (U"IntensityTier commands")
NORMAL (U"Creation:")
LIST_ITEM (U"From scratch:")
LIST_ITEM (U"• @@Create IntensityTier...")
LIST_ITEM (U"• @@IntensityTier: Add point...")
LIST_ITEM (U"Copy from another object:")
LIST_ITEM (U"• @@Intensity: To IntensityTier@: trivial copying of linearly spaced points.")
LIST_ITEM (U"• @@Intensity & PointProcess: To IntensityTier...@: copying interpolated values at specified points.")
LIST_ITEM (U"• @@PointProcess: Up to IntensityTier...@: equal values at specified points.")
NORMAL (U"Viewing and editing:")
LIST_ITEM (U"• @IntensityTierEditor")
NORMAL (U"Conversion:")
LIST_ITEM (U"• @@IntensityTier: Down to PointProcess@: copy times.")
NORMAL (U"Synthesis (see @@Source-filter synthesis@):")
LIST_ITEM (U"• @@Sound & IntensityTier: Multiply@")
NORMAL (U"Queries:")
LIST_ITEM (U"• @@Get low index from time...")
LIST_ITEM (U"• @@Get high index from time...")
LIST_ITEM (U"• @@Get nearest index from time...")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@Remove point...")
LIST_ITEM (U"• @@Remove point near...")
LIST_ITEM (U"• @@Remove points between...")
LIST_ITEM (U"• @@IntensityTier: Add point...@")
MAN_END

MAN_BEGIN (U"IntensityTier: Add point...", U"ppgb", 20010410)
INTRO (U"A command to add a point to each selected @IntensityTier.")
ENTRY (U"Settings")
TERM (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
TERM (U"##Intensity (dB)")
DEFINITION (U"the intensity value of the requested new point.")
ENTRY (U"Behaviour")
NORMAL (U"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (U"IntensityTier: Down to PointProcess", U"ppgb", 20010410)
INTRO (U"A command to degrade every selected @IntensityTier to a @PointProcess.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. The intensity information is lost.")
MAN_END

MAN_BEGIN (U"IntensityTierEditor", U"ppgb", 20110128)
INTRO (U"One of the @editors in the Praat program, for viewing and editing an @IntensityTier object. "
	"To create a IntensityTierEditor window, select an IntensityTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (U"Command-click", U"ppgb", 20210228)
INTRO (U"To %%Command-click% means to click while the %%Command key% is pressed. "
	"The Command key is the key that is marked with an apple or the word \"command\" on Apple keyboards, "
	"or to the key that is marked \"Ctrl\" if you are on a Windows or Linux computer.")
MAN_END

MAN_BEGIN (U"Keyboard shortcuts", U"ppgb", 20220701)  // 2026
INTRO (U"To accelerate menu commands in Praat, you can sometimes choose them with the keyboard, "
	"sometimes while also pressing the Command key, Option key, or Shift key. "
	"All of these commands can also be chosen from a menu.")
NORMAL (U"When mentioning the %%Command key%, this manual refers to the key that is marked with an apple "
	"or the word \"command\" on Apple keyboards "
	"or to the key that is marked \"Ctrl\" if you are on a Windows or Linux computer.")
NORMAL (U"When mentioning the %%Option key%, this manual refers to the key that is marked with "
	"the word \"option\" on Apple keyboards or with \"Alt\" if you are on Windows or Linux.")
NORMAL (U"When mentioning the %%Extra-Command key%, this manual refers to "
	"pressing the “Command” and “Shift” keys together (on Windows) "
	"or pressing the “Command” and “Option” keys together (on the Mac). "
	"In Praat, Extra-Command is sometimes used "
	"for destructive actions that are the reverse of the actions invoked by using the Command key only. "
	"For instance, if Command-T means \"add a target at the cursor position\", "
	"Extra-Command-T may mean \"remove the selected targets\".")
ENTRY (U"Shortcuts")
LIST_ITEM (U"Command-A: Zoom all")
LIST_ITEM (U"Command-C: Copy (the selected text, or the selected sound, or the selected part of the Picture window)")
LIST_ITEM (U"Command-D (in TextGrid window): Align interval")
LIST_ITEM (U"Command-D (in Manipulation window): Insert duration point at cursor")
LIST_ITEM (U"Extra-Command-D (in Manipulation window): Remove selected duration points")
LIST_ITEM (U"Command-E (in Picture window): Erase all")
LIST_ITEM (U"Command-E (in OT windows): Edit ranking")
LIST_ITEM (U"Command-F: Find")
LIST_ITEM (U"Extra-Command-F: Replace (yes, this is destructive...)")
LIST_ITEM (U"Command-G: Find again")
LIST_ITEM (U"Extra-Command-G: Replace again (yes, this is destructive...)")
LIST_ITEM (U"Command-H (in script window): Paste history")
LIST_ITEM (U"Extra-Command-H: Move cursor to maximum pitch")
LIST_ITEM (U"Command-I: Zoom in")
LIST_ITEM (U"Command-L (in Objects window): @@Open long sound file...@")
LIST_ITEM (U"Command-L (in sound windows): @@Intro 3.6. Viewing a spectral slice|View spectral slice@")
LIST_ITEM (U"Extra-Command-L: Move cursor to minimum pitch")
LIST_ITEM (U"Command-M: Search Praat manual...")
LIST_ITEM (U"Command-N: Zoom to selection")
LIST_ITEM (U"Command-O (in Objects window): @@Read from file...@")
LIST_ITEM (U"Command-O (in sound windows): Zoom out")
LIST_ITEM (U"Command-P (in Picture window): Print")
LIST_ITEM (U"Command-P (in PointProcess window): Add point at cursor")
LIST_ITEM (U"Command-P (in Manipulation window): Add pulse at cursor")
LIST_ITEM (U"Extra-Command-P (in Manipulation window): Remove selected pulses")
LIST_ITEM (U"Command-Q: Quit Praat")
LIST_ITEM (U"Command-R (in Script window): Run")
LIST_ITEM (U"Command-R: Reverse selection")
LIST_ITEM (U"Command-S: Save")
LIST_ITEM (U"Command-T (in script window): Run selection")
LIST_ITEM (U"Command-T (in TextGrid window): Transcribe interval")
LIST_ITEM (U"Command-T (in PitchTier/DurationTier/RealTier/FormantGrid window): Add point at cursor")
LIST_ITEM (U"Command-T (in Manipulation window): Add pitch point at cursor")
LIST_ITEM (U"Extra-Command-T (in Manipulation window): Remove selected pitch points")
LIST_ITEM (U"Command-U: @@Calculator...@")
LIST_ITEM (U"Command-V (in text window or TextGrid window): Paste (insert over the selected text")
LIST_ITEM (U"Command-V (in Sound window): Paste after selection (insert the sound clipboard after the selected sound)")
LIST_ITEM (U"Extra-Command-V (in Sound window): Paste over selection (insert the sound clipboard over the selected sound)")
LIST_ITEM (U"Command-W: Close window")
LIST_ITEM (U"Command-X: Cut (the selected text or the selected sound)")
LIST_ITEM (U"Command-Y: Redo")
LIST_ITEM (U"Command-Z: Undo")
LIST_ITEM (U"Command-0 (in sound windows): Move cursor to nearest zero crossing")
LIST_ITEM (U"Command-0 (in OT windows): Evaluate with zero noise")
LIST_ITEM (U"Command-2 (in Manipulation window): Stylize pitch (2 semitones)")
LIST_ITEM (U"Command-2 (in OT windows): Evaluate with noise 2.0")
LIST_ITEM (U"Command-4 (in Manipulation window): Interpolate quadratically (4 points)")
LIST_ITEM (U"Shift-Command-?: Local help")
LIST_ITEM (U"Command-,: Move start of selection to nearest zero crossing")
LIST_ITEM (U"Command-.: Move end of selection to nearest zero crossing")
LIST_ITEM (U"F1: Get first formant")
LIST_ITEM (U"F2: Get second formant")
LIST_ITEM (U"F3: Get third formant")
LIST_ITEM (U"F4: Get fourth formant")
LIST_ITEM (U"F5: Get pitch")
LIST_ITEM (U"Option-F5: Get minimum pitch")
LIST_ITEM (U"Shift-F5: Get maximum pitch")
LIST_ITEM (U"F6: Get cursor")
LIST_ITEM (U"F7: Get spectral power at cursor cross")
LIST_ITEM (U"F8: Get intensity")
LIST_ITEM (U"Option-F8: Get minimum intensity")
LIST_ITEM (U"Shift-F8: Get maximum intensity")
LIST_ITEM (U"F12: Log 1")
LIST_ITEM (U"Shift-F12: Log 2")
LIST_ITEM (U"Option-F12: Log script 3")
LIST_ITEM (U"Command-F12: Log script 4   [note: Command-Fxx combinations are not available to apps on all computers]")
LIST_ITEM (U"Tab (in sound windows): Play selection")
LIST_ITEM (U"Shift-Tab (in sound windows): Play window")
LIST_ITEM (U"Arrow-up (in sound windows): Select earlier")
LIST_ITEM (U"Arrow-down (in sound windows): Select later")
LIST_ITEM (U"Shift-Arrow-up (in sound windows): Move start of selection left")
LIST_ITEM (U"Shift-Arrow-down (in sound windows): Move start of selection right")
LIST_ITEM (U"Command-Arrow-up (in sound windows): Move end of selection left")
LIST_ITEM (U"Command-Arrow-down (in sound windows): Move end of selection right")
LIST_ITEM (U"Page-up (in sound windows): Scroll page back")
LIST_ITEM (U"Page-down (in sound windows): Scroll page forward")
LIST_ITEM (U"Escape: Interrupt playing")
MAN_END

MAN_BEGIN (U"Log files", U"ppgb", 20230122)
INTRO (U"With some commands in the @@Analyses menu@ of the @SoundEditor and @TextGridEditor, "
	"you can write combined information about times, pitch values, formants, and intensities "
	"to the @@Info window@ and/or to a log file.")
NORMAL (U"A log file is a text file on disk. It consists of a number of similar lines, "
	"whose format you determine with the log settings in the Analyses menu.")
NORMAL (U"Every time you press @@Keyboard shortcuts|F12@ (or choose ##Log 1# from the Analysis menu), "
	"Praat will write a line to the Info window and/or to log file 1. "
	"If you press @@Keyboard shortcuts|Shift-F12@ (or choose ##Log 2# from the Analysis menu), "
	"Praat will write a line to the Info window and/or to log file 2.")
NORMAL (U"With the ##log settings# command window, you determine the following:")
TERM (U"##Write log 1 to")
DEFINITION (U"this determines whether your log line will be written to log file 1 only, to the Info window only, or to both.")
TERM (U"##Log file 1")
DEFINITION (U"the name of the log file. Click Browse to select a file to write to. "
	"On most platforms you can also use a home-relative name such as `~/Desktop/pitchLog.txt`.")
TERM (U"##Log 1 format")
DEFINITION (U"the format of the line that Praat will write. See below.")
NORMAL (U"The same goes for log file 2.")
ENTRY (U"Usage")
NORMAL (U"The logging facility has been implemented in Praat especially for former users of Kay CSL, "
	"who have been used to doing it for years and like to continue doing it in Praat. "
	"Otherwise, you may prefer to use the @TextGridEditor to mark time points and run "
	"an automatic analysis afterwards.")
NORMAL (U"If you do want to use the logging facility, you typically start by deleting any old "
	"log file (by choosing ##Delete log file 1# or ##Delete log file 2#), if you want to re-use "
	"the file name. Otherwise, you can change the log file name (with ##Log settings...#). "
	"After this, you will move the cursor to various time locations and press @@Keyboard shortcuts|F12@ (or @@Keyboard shortcuts|Shift-F12@) "
	"each time, so that information about the current time will be written to the log file.")
ENTRY (U"Example 1: pitch logging")
NORMAL (U"Suppose you want to log the time of the cursor and the pitch value at the cursor. "
	"You could use the following log format:")
CODE (U"Time \'time:6\' seconds, pitch \'f0:2\' hertz")
NORMAL (U"If you now click at 3.456789876 seconds, and the pitch happens to be 355.266 hertz "
	"at that time, the following line will be appended to the log file and/or to the Info window:")
CODE (U"Time 3.456790 seconds, pitch 355.27 hertz.")
NORMAL (U"The parts \":6\" and \":2\" denote the number of digits after the decimal point. "
	"If you leave them out, the values will be written with a precision of 17 digits.")
NORMAL (U"The words \'time\' and \'f0\' mean exactly the same as the result of the commands "
	"##Get cursor# and ##Get pitch#. Therefore, if instead of setting a cursor line you selected a larger "
	"piece of the sound, \'time\' will give the centre of the selection and \'f0\' will give the mean pitch "
	"in the selection.")
NORMAL (U"Beware of the following pitfall: if your pitch units are not hertz, but semitones, "
	"then \'f0\' will give the result in semitones. A format as in this example will then be misleading.")
ENTRY (U"Example 2: formant logging")
NORMAL (U"Suppose you want to log the start and finish of the selection, its duration, and the mean values "
	"of the first three formants, all separated by tab stops for easy importation into Microsoft® Excel™. "
	"You could use the following log format:")
CODE (U"\'t1:4\'\'tab$\'\'t2:4\'\'tab$\'\'f1:0\'\'tab$\'\'f2:0\'\'tab$\'\'f3:0\'")
NORMAL (U"You see that \'t1\' and \'t2\' are the start and finish of the selection, respectively, "
	"and that they are written with 4 digits after the decimal point. By using \":0\", the three formant values "
	"are rounded to whole numbers in hertz. The word `tab$` is the tab stop.")
ENTRY (U"Loggable values")
NORMAL (U"The following values can be logged:")
LIST_ITEM (U"`time`: the time of the cursor, or the centre of the selection.")
LIST_ITEM (U"`t1`: the start of the selection (\"B\").")
LIST_ITEM (U"`t2`: the end of the selection (\"E\").")
LIST_ITEM (U"`dur`: the duration of the selection.")
LIST_ITEM (U"`freq`: the frequency at the frequency cursor.")
LIST_ITEM (U"`f0`: the pitch at the cursor time, or the mean pitch in the selection.")
LIST_ITEM (U"`f1`, `f2`, `f3`, `f4`, `f5`: the first/second/third/fourth/fifth formant at the cursor time, "
	"or the mean first/second/third/fourth/fifth formant in the selection.")
LIST_ITEM (U"`b1`, `b2`, `b3`, `b4`, `b5`: the bandwidth of the first/second/third/fourth/fifth formant "
	"at the cursor time or at the centre of the selection.")
LIST_ITEM (U"`intensity`: the intensity at the cursor time, or the mean intensity in the selection, in dB.")
LIST_ITEM (U"`power`: the spectral power at the cursor cross, in Pa^2/Hz.")
LIST_ITEM (U"`tab$`: the tab stop.")
LIST_ITEM (U"`editor$`: the title of the editor window (i.e. the name of the visible Sound or TextGrid).")
ENTRY (U"More flexibility in logging")
NORMAL (U"You may sometimes require information in your log file that cannot be generated directly "
	"by the loggable values above. Suppose, for instance, that you want to log the values for F1 and F2-F1 "
	"at the points where you click. You could write the following script:")
CODE (U"f1 = Get first formant")
CODE (U"f2 = Get second formant")
CODE (U"f21 = f2 - f1")
CODE (U"appendInfoLine: fixed$ (f1, 0), \" \", fixed$ (f21, 0)")
CODE (U"appendFileLine: \"D:\\Praat logs\\Formant log.txt\", fixed$ (f1, 0), tab$, fixed$ (f21, 0)")
NORMAL (U"With this script, the information would be appended both to the Info window and to the "
	"file \"Formant log.txt\" on your desktop.")
NORMAL (U"You can make this script accessible with @@Keyboard shortcuts|Option-F12@ (or @@Keyboard shortcuts|Command-F12@) "
	"by saving the script and specifying the name of the script file in the ##Log script 3# (or #4) field "
	"in the ##Log settings...# window.")
NORMAL (U"These scripts may take arguments. Suppose, for instance, that you want to specify a vowel symbol "
	"as you press @@Keyboard shortcuts|Option-F12@. The following script will take care of that:")
CODE (U"form: \"Save vowel and formants\"")
	CODE1 (U"word: \"Vowel\", \"a\"")
CODE (U"endform")
CODE (U"f1 = Get first formant")
CODE (U"f2 = Get second formant")
CODE (U"f21 = f2 - f1")
CODE (U"appendInfoLine: vowel$, \" \", fixed$ (f1, 0), \" \", fixed$ (f21, 0)")
CODE (U"appendFileLine: \"~/Praat logs/Vowels and formants log\", vowel$, tab$, fixed$ (f1, 0), tab$, fixed$ (f21, 0)")
NORMAL (U"Beware of the following pitfall: because of the nature of scripts, you should not try to do this "
	"when you have two editor windows with the same name. We cannot predict which of the two windows "
	"will answer the #Get queries...")
MAN_END

MAN_BEGIN (U"Manipulation", U"ppgb", 20030316)   // 2023
INTRO (U"One of the @@types of objects@ in Praat, for changing the pitch and duration contours of a sound.")
ENTRY (U"Inside a manipulation object")
NORMAL (U"With @Inspect, you will see the following attributes:")
TERM (U"##timeStep")
DEFINITION (U"the time step (or %%frame length%) used in the pitch analysis. A common value is 0.010 seconds.")
TERM (U"##pitchFloor")
DEFINITION (U"the minimum pitch frequency considered in the pitch analysis. A common value is 75 hertz.")
TERM (U"##pitchCeiling")
DEFINITION (U"the maximum pitch frequency considered in the pitch analysis. A common value is 600 hertz.")
NORMAL (U"A Manipulation object also contains the following smaller objects:")
LIST_ITEM (U"1. The original @Sound.")
LIST_ITEM (U"2. A @PointProcess representing glottal pulses.")
LIST_ITEM (U"3. A @PitchTier.")
LIST_ITEM (U"4. A @DurationTier.")
ENTRY (U"Analysis")
NORMAL (U"When a Manipulation object is created from a sound, the following steps are performed:")
LIST_ITEM (U"1. A pitch analysis is performed on the original sound, with the method of @@Sound: To Pitch...@. "
	"This uses the time step, pitch floor, and pitch ceiling parameters.")
LIST_ITEM (U"2. The information of the resulting pitch contour (frequency and voiced/unvoiced decisions) "
	"is used to posit glottal pulses where the original sound contains much energy. "
	"The method is the same as in @@Sound & Pitch: To PointProcess (cc)@.")
LIST_ITEM (U"3. The pitch contour is converted to a pitch tier with many points (targets), "
	"with the method of @@Pitch: To PitchTier@.")
LIST_ITEM (U"4. An empty @DurationTier is created.")
ENTRY (U"Resynthesis")
TERM (U"A Manipulation object can produce Sound input. This Sound can be computed in several ways:")
LIST_ITEM (U"• @@overlap-add@: from original sound + pulses + pitch tier + duration tier;")
LIST_ITEM (U"• #LPC: from LPC (from original sound) + pulses + pitch tier;")
LIST_ITEM (U"• from the pulses only, as a pulse train or hummed;")
LIST_ITEM (U"• from the pitch tier only, as a pulse train or hummed.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract duration tier", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the duration information in each selected @Manipulation object into a new @DurationTier object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract original sound", U"ppgb", 20010330)
INTRO (U"A command to copy the original sound in each selected @Manipulation object to a new @Sound object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract pitch tier", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the pitch information in each selected @Manipulation object into a new @PitchTier object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract pulses", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the vocal-pulse information in each selected @Manipulation object into a new @PointProcess object.")
MAN_END

MAN_BEGIN (U"Manipulation: Play (overlap-add)", U"ppgb", 20070722)
INTRO (U"A command to play each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (U"Manipulation: Get resynthesis (overlap-add)", U"ppgb", 20070722)
INTRO (U"A command to extract the sound from each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace duration tier", U"ppgb", 20030216)
INTRO (U"You can replace the duration tier that you see in your @Manipulation object "
	"with a separate @DurationTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create DurationTier...@.")
NORMAL (U"To do this, select your Manipulation object together with the @DurationTier object and click ##Replace duration tier#.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace pitch tier", U"ppgb", 20030216)
INTRO (U"You can replace the pitch tier that you see in your @Manipulation object "
	"with a separate @PitchTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create PitchTier...@.")
NORMAL (U"To do this, select your Manipulation object together with the @PitchTier object and click ##Replace pitch tier#.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace pulses", U"ppgb", 20010330)
INTRO (U"A command to replace the vocal-pulse information in the selected @Manipulation object with the selected @PointProcess object.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace original sound", U"ppgb", 20010330)
INTRO (U"A command to replace the original sound in the selected @Manipulation object with the selected @Sound object.")
MAN_END

MAN_BEGIN (U"ManipulationEditor", U"ppgb", 20030316)  // 2023-06-08
	INTRO (U"One of the @Editors in Praat, for viewing and manipulating a @Manipulation object.")
ENTRY (U"Objects")
	NORMAL (U"The editor shows:")
	LIST_ITEM (U"• The original @Sound.")
	LIST_ITEM (U"• The @PointProcess that represents the glottal %pulses. "
		"You can edit it for improving the pitch analysis.")
	LIST_ITEM (U"• A pitch contour based on the locations of the pulses, for comparison (drawn as grey dots). "
		"Changes shape if you edit the pulses.")
	LIST_ITEM (U"• The @PitchTier that determines the pitch contour of the resynthesized @Sound (drawn as blue circles). "
		"At the creation of the @Manipulation object, it is computed from the original pitch contour. "
		"You can manipulate it by simplifying it (i.e., removing targets), "
		"or by moving parts of it up and down, and back and forth.")
	LIST_ITEM (U"• A @DurationTier for manipulating the relative durations of the voiced parts of the sound.")
ENTRY (U"Playing")
	NORMAL (U"To play (a part of) the %resynthesized sound (by any of the methods shown in the #Synth menu, "
		"like @@overlap-add@ and #LPC), @click any of the 1 to 8 buttons below and above the drawing area "
		"or use the commands from the Play menu.")
	NORMAL (U"To play the %original sound instead, use ##Shift-click#.")
ENTRY (U"Pulses")
	TERM (U"To add:")
	DEFINITION (U"#click at the desired time location, and choose ##Add pulse at cursor# or type @@Keyboard shortcuts|Command-P@.")
	TERM (U"To remove:")
	DEFINITION (U"make a @@time selection@, and choose ##Remove pulse(s)# or type @@Keyboard shortcuts|Option-Command-P@. "
		"If there is no selection, the pulse nearest to the cursor is removed.")
ENTRY (U"Pitch points")
	TERM (U"To add one at a specified %%time and frequency%:")
	DEFINITION (U"#click at the desired time-frequency location, and choose ##Add pitch point at cursor# or type @@Keyboard shortcuts|Command-t@.")
	TERM (U"To add one at a specified %time only:")
	DEFINITION (U"#click at the desired time, and choose ##Add pitch point at time slice#. ManipulationEditor tries to compute the frequency from the "
		"intervals between the pulses, basically by a median-of-three method.")
	TERM (U"To remove:")
	DEFINITION (U"make a @@time selection@, and choose ##Remove pitch point(s)# or type @@Keyboard shortcuts|Option-Command-t@. "
		"If there is no selection, the pitch point nearest to the cursor is removed.")
	TERM (U"To move %some:")
	DEFINITION (U"make a @@time selection@ (the points become red) and ##Shift-drag# the points across the window. "
		"You cannot drag them across adjacent points, or below 50 Hz, or above the maximum frequency. "
		"You can only drag them horizontally if the %%dragging strategy% is ##All# or ##Only horizontal#, "
		"and you can drag them vertically if the dragging strategy is not ##Only horizontal#. "
		"You can change the dragging strategy with ##Set pitch dragging strategy...# from the #Pitch menu.")
	TERM (U"To move %one:")
	DEFINITION (U"@drag that point across the window. "
		"You can only drag it horizontally if the dragging strategy is not ##Only vertical#, "
		"and you can drag it vertically if the dragging strategy is not ##Only horizontal#.")
ENTRY (U"Duration points")
	NORMAL (U"Work pretty much the same as pitch points.")
ENTRY (U"Stylization")
	NORMAL (U"Before editing the Pitch points, you may want to reduce their number by choosing any of the #Stylize "
		"commands from the #Pitch menu.")
MAN_END

MAN_BEGIN (U"Matrix", U"ppgb", 20240712)
INTRO (U"One of the @@types of objects@ in Praat. "
	"A Matrix object represents a function %z (%x, %y) "
	"on the domain [%x__%min_, %x__%max_] × [%y__%min_, %y__%max_]. "
	"The domain has been sampled in the %x and %y directions "
	"with constant sampling intervals (%dx and %dy) along each direction. "
	"The samples are thus %z [%i_%y] [%i_%x], %i_%x = 1 ... %n_%x, %i_%y = 1 ... %n_%y. "
	"The samples represent the function values %z (%x__1_ + (%i_%x - 1) %dx, %y__1_ + (%i_%y - 1) %dy).")
ENTRY (U"Matrix commands")
NORMAL (U"Creation:")
LIST_ITEM (U"• @@Create Matrix...")
LIST_ITEM (U"• @@Create simple Matrix...")
LIST_ITEM (U"• @@Read from file...")
LIST_ITEM (U"• @@Read Matrix from raw text file...")
LIST_ITEM (U"• ##Read Matrix from LVS AP file...")
NORMAL (U"Drawing:")
LIST_ITEM (U"• ##Matrix: Draw rows...")
LIST_ITEM (U"• ##Matrix: Draw contours...")
LIST_ITEM (U"• ##Matrix: Paint contours...")
LIST_ITEM (U"• ##Matrix: Paint cells...")
LIST_ITEM (U"• ##Matrix: Scatter plot...")
LIST_ITEM (U"• @@Matrix: Draw as squares...")
LIST_ITEM (U"• ##Matrix: Draw value distribution...")
LIST_ITEM (U"• ##Matrix: Paint surface...")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@Matrix: Formula...")
LIST_ITEM (U"• ##Matrix: Scale...")
ENTRY (U"Inside a Matrix object")
NORMAL (U"With @Inspect, you will see the following attributes.")
TERM (U"%xmin, %xmax ≥ %xmin")
DEFINITION (U"%x domain.")
TERM (U"%nx ≥ 1")
DEFINITION (U"number of columns.")
TERM (U"%dx > 0.0")
DEFINITION (U"distance between columns.")
TERM (U"%x1")
DEFINITION (U"%x value associated with first column.")
TERM (U"%ymin, %ymax ≥ %ymin")
DEFINITION (U"%y domain.")
TERM (U"%ny ≥ 1")
DEFINITION (U"number of rows.")
TERM (U"%dy > 0.0")
DEFINITION (U"distance between rows.")
TERM (U"%y1")
DEFINITION (U"%y value associated with first row.")
TERM (U"%z [1..%ny] [1..%nx]")
DEFINITION (U"The sample values.")
NORMAL (U"After creation of the #Matrix, %xmin, %xmax, %ymin, %ymax, "
	"%nx, %ny, %dx, %dy, %x1, and %y1 "
	"do not usually change. The contents of %z do.")
NORMAL (U"Normally, you will want %xmin ≤ %x1 and %xmax ≥ %x1 + (%nx - 1) %dx.")
ENTRY (U"Example: simple matrix")
NORMAL (U"If a simple matrix has %x equal to column number "
	"and %y equal to row number, it has the following attributes:")
LIST_ITEM (U"%xmin = 1;   %xmax = %nx;   %dx = 1;  %x1 = 1;")
LIST_ITEM (U"%ymin = 1;   %ymax = %ny;   %dy = 1;  %y1 = 1;")
ENTRY (U"Example: sampled signal")
NORMAL (U"If the matrix represents a sampled signal of 1 second duration "
	"with a sampling frequency of 10 kHz, it has the following attributes:")
LIST_ITEM (U"%xmin = 0.0;   %xmax = 1.0;   %nx = 10000 ;   %dx = 1.0·10^^-4^;   %x1 = 0.5·10^^-4^;")
LIST_ITEM (U"%ymin = 1;   %ymax = 1;   %ny = 1;   %dy = 1;   %y1 = 1;")
ENTRY (U"Example: complex signal")
NORMAL (U"If the matrix represents a complex spectrum "
	"derived with an @FFT from the sound of example 2, it has the following attributes:")
LIST_ITEM (U"%xmin = 0.0;   %xmax = 5000.0;   %nx = 8193 ;   %dx = 5000.0 / 8192;   %x1 = 0.0;")
LIST_ITEM (U"%ny = 2 (real and imaginary part);")
LIST_ITEM (U"%ymin = 1 (first row, real part);")
LIST_ITEM (U"%ymax = 2 (second row, imaginary part);")
LIST_ITEM (U"%dy = 1;   %y1 = 1;  (so that %y is equal to row number)")
MAN_END

MAN_BEGIN (U"Matrix: Draw as squares...", U"ppgb", 19980319)
INTRO (U"A command to draw a @Matrix object into the @@Picture window@.")
ENTRY (U"Settings")
TERM (U"##Xmin")
TERM (U"##Xmax")
DEFINITION (U"the windowing domain in the %x direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Xmin ≥ %Xmax), the entire %x domain [%x__%min_, %x__%max_] of the Matrix is used.")
TERM (U"##Ymin")
TERM (U"##Ymax")
DEFINITION (U"the windowing domain in the %y direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Ymin ≥ %Ymax), the entire %y domain [%y__%min_, %y__%max_] of the Matrix is used.")
TERM (U"##Garnish")
DEFINITION (U"determines whether axes are drawn around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
ENTRY (U"Behaviour")
NORMAL (U"For every element of the Matrix inside the specified windowing domain, "
	"an opaque white or black rectangle is painted (white if the value of the element is positive, "
	"black if it is negative), surrounded by a thin black box. "
	"The %area of the rectangle is proportional to the value of the element.")
ENTRY (U"Trick")
NORMAL (U"If you prefer the %sides of the rectangle (instead of the area) to be proportional "
	"to the value of the element, you can use the formula \"`self^2`\" before drawing (see @@Matrix: Formula...@).") 
MAN_END

MAN_BEGIN (U"Matrix: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Matrix objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Matrix: Paint cells...", U"ppgb", 20021204)
INTRO (U"A command to draw the contents of a @Matrix to the @@Picture window@.")
NORMAL (U"Every cell of the matrix is drawn as a rectangle filled with a grey value between white (if the content "
	"of the cell is small) and black (if the content is large).")
MAN_END

MAN_BEGIN (U"Matrix: Set value...", U"ppgb", 19980319)
INTRO (U"A command to change the value of one cell in each selected @Matrix object.")
ENTRY (U"Settings")
TERM (U"##Row number")
DEFINITION (U"the number of the row of the cell whose value you want to change.")
TERM (U"##Column number")
DEFINITION (U"the number of the column of the cell whose value you want to change.")
TERM (U"##New value")
DEFINITION (U"the value that you want the specified cell to have.")
MAN_END

/*
1. The Hilbert transform.

I wondered whether my use of taking the square was worse than taking
the Hilbert envelope. My formula is

   my_envelope1(t) = x(t) ^ 2

For a sampled signal, this introduces higher frequencies, so I should have
filtered the signal at half the Nyquist frequency to prevent aliasing. I wonder
how the Hilbert envelope behaves in this respect:

   your_envelope1(t) = x(t) ^ 2 + Px(t) ^ 2

I think it has the same problem, since it involves squaring the real and imaginary parts.

Of course, the Hilbert transform is much smoother, but that cannot matter for
frequencies above 100 Hz or so, since we are going to filter at 30 Hz anyway.
Thus, I think these methods are comparable. Do you agree?


2. The non-linearity.

In the second step, I take the logarithm:

   my_envelope2(t) = 10 log10 (my_envelope1(t) + 1e-6)

Of course, the 1e-6 is there to guard against zero values in my_envelope1.
I took the logarithm because it can handle negative values, which will arise
in the filtering process. You take the square root instead:

   your_envelope2(t) = sqrt (your_envelope1(t))


3. Filtering between 3 and 30 Hz.

You use a second-order Butterworth filter in the time domain:

   your_envelope3(t) = your_envelope2(t) * Butterworth(t)

What do you mean by "forward and backward filtering"?
Did you use the second-order filter twice, thus making it fourth-order effectively?

My alternative was to use the two Gaussians:

   my_spectrum2(f) = fft (my_envelope2(t))
   my_spectrum3(f) = my_spectrum2(f) (exp(-(f/35)^2)-exp(-(f/3.5)^2)
   my_envelope3(t) = ifft (my_spectrum3 (f))

This is a filter with several desirable properties: no ringing, phase-preserving,
an area of zero. On the other hand, it is not causal, but I think that that is not
an issue here. Is it?


4. The new band.

In my algorithm, I have to undo the non-linearity:

   my_envelope4(t) = 10 ^ (my_envelope3(t) / 2)

You see that I divide by 2, not 10, thus effectively multiplying the dynamics,
expressed in dB, by a factor of 5. Since I am going to use the new envelope as
a multiplication factor, I will have to limit it to a factor of 10, because the
maximum enhancement is 20 dB. But this limiting factor is band-dependent:

   ceiling(Fmid) = 1 + 9 (1/2 - 1/2 cos (pi Fmid / 13))

where Fmid is expressed in Bark. The limiting is done smoothly:

   my_envelope5(t) = 1 / (1 / my_envelope4(t) + 1 / ceiling)

So this is the final multiplication factor for the band-filtered signal:

   new_band(t) = x(t) my_envelope5(t)

Your multiplication factor is very different. Please tell me whether I am correct.

   your_envelope4(t) = rectify (your_envelope3(t))

This step is needed because you cannot handle negative values. It introduces
higher-frequency components, which will have to be filtered out later.
The new signal will have to carry the new envelope, so the multiplication
factor is taken relative to the envelope of the original signal:

   your_envelope5(t) = your_envelope4(t) / your_envelope2(t)

The components that were introduced by the rectification must be thrown out:

   your_envelope6(t) = your_envelope5(t) * Butterworth?(t)

What were the characteristics of that filter? You did not specify them in your paper.
Well, the next step is normalization to the power of the original band:

   new_band(t) = your_envelope6(t) ||x(t)|| / ||your_envelope5(t)||

At least, that's your formula (12). I suppose that it contains a mistake, and that
the norm in the denominator should refer to the filtered your_envelope6(t) instead?

By the normalization, the fast movements within each band have been strengtened,
and the slow parts have been weakened. Thus, there is dynamic expansion within
each band, whereas my algorithm has this only in the mid frequency range, am I correct?
Could you also explain figure 5 to me? Some of the thick curves show enhanced peaks,
but how about the valleys, are some of them deepened as well?


5. The new signal.

My algorithm simply adds all the bands:

   new_signal(t) = SUM new_band(Fmid,t)

The mid-frequency bands are favoured because they have stronger modulation deepening
than the other bands. In your algorithm, the favouring of the mid frequencies is
implemented in the last step:

   new_signal(t) = SUM new_band(Fmid,t) gain(Fmid)

This would mean, for instance, that if the modulation is not deepened (for instance
if there are no fast movements), the mid-frequency range is still multiplied by a factor
of 10, whereas my algorithm would not change the signal at all in such a case. Is that so?

If so, could any of the learning results have been produced by the general emphasis
in the 1-4 kHz range? Have you any proof that the modulation deepening itself
is the cause of the learning results?

So the differences between the two algorithms are clear now: mine does modulation
deepening selectively in the F2 range, yours does modulation deepening everywhere
plus an independent emphasis in the F2 range. Since both algorithms will lead to
emphasis in the F2 range, that leaves the question of whether a modulation change
is necessary for the other frequency bands. Have you any idea, or was your choice
for this just one of the minor decisions while constructing your algorithm?

To sum up, I think that both algorithms perform what they were intended to do,
namely selectively enhancing fast F2 transitions. If you agree with this, I may
advise XXX to use my algorithm without change. I would, however, welcome any
suggestions as to oversights in my implementation, or other possible problems
or incorrect choices.
*/

MAN_BEGIN (U"Matrix: To TableOfReal", U"ppgb", 19991030)
INTRO (U"A command to convert every selected @Matrix to a @TableOfReal.")
NORMAL (U"This command is available from the #Cast menu. The resulting TableOfReal "
	"has the same number of rows and columns as the original Matrix, "
	"and the same data in the cells. However, it does not yet have any row or column "
	"labels; you can add those with some commands from the TableOfReal #Modify menu.")
MAN_END

MAN_BEGIN (U"Modify", U"ppgb", 20021204)
INTRO (U"The title of a submenu of the @@dynamic menu@ for many object types. "
	"This submenu usually collects all the commands that can change the selected object.")
MAN_END

MAN_BEGIN (U"PairDistribution", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. A "
	"PairDistribution object represents the relative probabilities with which "
	"the specified pairs of strings occur.")
ENTRY (U"Class description")
TERM (U"##struct-list# pairs")
DEFINITION (U"a list of relative string-pair probabilities. Each element consists of:")
TERM1 (U"#string %string1")
DEFINITION1 (U"the first string.")
TERM1 (U"#string %string2")
DEFINITION1 (U"the second string.")
TERM1 (U"#real %weight")
DEFINITION1 (U"the relative probability associated with the string pair. This value cannot be negative.")
MAN_END

MAN_BEGIN (U"PairDistribution: To Stringses...", U"ppgb", 20030916)
INTRO (U"A command to generate a number of string pairs from the selected @PairDistribution object. "
	"This command will create two aligned @Strings objects of equal size.")
ENTRY (U"Settings")
TERM (U"##Number# (standard: 1000)")
DEFINITION (U"the number of the strings in either resulting Strings object.")
TERM (U"##Name of first Strings# (standard: \"input\")")
DEFINITION (U"the name of the resulting Strings object associated with the first string of each pair.")
TERM (U"##Name of second Strings# (standard: \"output\")")
DEFINITION (U"the name of the resulting Strings object associated with the second string of each pair.")
ENTRY (U"Example")
NORMAL (U"Suppose the PairDistribution contains the following:")
CODE (U"4 pairs")
CODE (U"\"at+ma\"  \"atma\"  100")
CODE (U"\"at+ma\"  \"apma\"    0")
CODE (U"\"an+pa\"  \"anpa\"   20")
CODE (U"\"an+pa\"  \"ampa\"   80")
NORMAL (U"The resulting Strings object \"input\" may then contain:")
EQUATION (U"at+ma, an+pa, an+pa, at+ma, at+ma, an+pa, an+pa, an+pa, an+pa, at+ma, ...")
NORMAL (U"The Strings object \"output\" may then contain:")
EQUATION (U"atma,  ampa,  ampa,  atma,  atma,  ampa,  anpa,  ampa,  ampa,  atma, ...")
MAN_END

MAN_BEGIN (U"ParamCurve", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of class #ParamCurve represents a sequence of time-stamped points (%x (%%t__i_), %y (%%t__i_)) "
	"in a two-dimensional space.")
MAN_END

#if 0

MAN_BEGIN (U"Polygon", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A Polygon object represents a sequence of points (%%x__i_, %%y__i_) in a two-dimensional space.")
MAN_END

MAN_BEGIN (U"Create Polygon from values...", U"David Weenink & Paul Boersma", 20230116)
INTRO (U"Creates a @@Polygon@ from user-supplied x\\--y pairs (“points” or “vertices”).")
ENTRY (U"Settings")
TERM (U"##Name")
DEFINITION (U"defines the name of the resulting Polygon.")
TERM (U"##X values")
DEFINITION (U"the X-coordinates of the vertices of the Polygon.")
TERM (U"##Y values")
DEFINITION (U"the Y-coordinates of the vertices of the Polygon.")
ENTRY (U"Example")
NORMAL (U"The command ##Create Polygon from values: \"triangle\", \"0 50 20\", \"0 10 80\"# "
	"defines a Polygon with the three points (0, 0), (50, 10), and (20, 80). "
	"In the figure the three points are indicated with open circles while the Polygon is drawn as a closed figure.")
{ kManPage_type::SCRIPT, UR"~~~(
	Create Polygon from values: "triangle", "0 50 20", "0 10 80"
	Draw circles: 0, 0, 0, 0, 3
	Draw closed: 0, 0, 0, 0
	Remove
)~~~", 4, 4, nullptr },
MAN_END

#else


MAN_PAGES_BEGIN R"~~~(
"Polygon"
© ppgb 2003-03-16

One of the @@types of objects@ in Praat.

A Polygon object represents a sequence of points (%%x__i_, %%y__i_) in a two-dimensional space.
)~~~" MAN_PAGES_END


MAN_PAGES_BEGIN R"~~~(
"Create Polygon from values..."
© David Weenink & Paul Boersma 2023-01-16

Creates a @@Polygon@ from user-supplied x\--y pairs (“points” or “vertices”).

Settings
===

##Name
: defines the name of the resulting Polygon.

##X values
: the X-coordinates of the vertices of the Polygon.

##Y values
: the Y-coordinates of the vertices of the Polygon.

Example
===
The command ##Create Polygon from values: "triangle", "0 50 20", "0 10 80"#
defines a Polygon with the three points (0, 0), (50, 10), and (20, 80).
In the figure the three points are indicated with open circles while the Polygon is drawn as a closed figure.
{+ 4x4
	Create Polygon from values: "triangle", "0 50 20", "0 10 80"
	Draw circles: 0, 0, 0, 0, 3
	Draw closed: 0, 0, 0, 0
}
)~~~" MAN_PAGES_END

#endif


MAN_BEGIN (U"Read Matrix from raw text file...", U"ppgb", 19980322)
INTRO (U"A command to read a @Matrix object from a file on disk.")
ENTRY (U"File format")
NORMAL (U"The file should contain each row of the matrix on a separate line. Within each row, "
	"the elements must be separated by spaces or tabs.")
NORMAL (U"For instance, the following text file will be read as a Matrix with three rows and four columns:")
CODE (U"0.19 3 245 123")
CODE (U"18e-6 -3e18 0 0.0")
CODE (U"1.5 2.5 3.5 4.5")
NORMAL (U"The resulting Matrix will have the same domain and sampling as Matrices created with "
	"##Create simple Matrix...#. In the above example, this means that the Matrix will have "
	"%x__%min_ = 0.5, %x__%max_ = 4.5, %n__%x_ = 4, %dx = 1.0, %x__1_ = 1.0, "
	"%y__%min_ = 0.5, %y__%max_ = 3.5, %n__%y_ = 3, %dy = 1.0, %y__1_ = 1.0.")
MAN_END

MAN_BEGIN (U"Read Strings from raw text file...", U"ppgb", 19990502)
INTRO (U"A command to read a @Strings object from a simple text file. "
	"Each line is read as a separate string. See @Strings for an example.")
MAN_END

MAN_BEGIN (U"RealTier", U"ppgb", 20210612)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An RealTier object represents a time-stamped curve, i.e., it contains a series of (%time, %value) points. "
	"The values have no physical units, so a RealTier is a generic, \"mathematical\", type, "
	"just as a @Matrix is. Special, physically interpretable, kinds of RealTiers "
	"are @PitchTier, @IntensityTier, @DurationTier and @AmplitudeTier. "
	"You can convert a RealTier to and from these other tier types.")
NORMAL (U"A RealTier can come in handy if you have time-stamped real-valued data "
	"that cannot be interpreted as any of the physical tier types. "
	"You can \"View & Edit\" a RealTier in its own window.")
ENTRY (U"How to import a RealTier from raw data")
NORMAL (U"While you create a RealTier from the New menu, you will often have data in a text file produced by "
	"a different app than Praat. For instance, your text file may look like this, with a header:")
CODE (U"Time Value")
CODE (U"0.134 67.9")
CODE (U"0.178 -138.6")
CODE (U"0.211 78.9")
CODE (U"0.213 100.0")
CODE (U"0.456 -97.0")
NORMAL (U"You can import this text file as a Table object with ##Read Table from whitespace-separated file...# "
	"or perhaps with ##Read Table from tab-separated file...#). You can then choose ##To RealTier...# from the Convert menu. "
	"Alternatively, your text file may simply look like this, without header:")
CODE (U"0.134 67.9")
CODE (U"0.178 -138.6")
CODE (U"0.211 78.9")
CODE (U"0.213 100.0")
CODE (U"0.456 -97.0")
NORMAL (U"You can import this text file as a Matrix object with @@Read Matrix from raw text file...@. "
	"You can then choose ##To RealTier...# from the Cast menu.")
MAN_END

MAN_BEGIN (U"Sound: To Intensity...", U"ppgb", 20100605)   // 2023
INTRO (U"A command to create an @Intensity object from every selected @Sound.")
ENTRY (U"Settings")
TERM (U"##Pitch floor (Hz)")
DEFINITION (U"the minimum periodicity frequency in your signal. If you set it too high, "
	"you will end up with a pitch-synchronous intensity modulation. If you set it too low, "
	"your intensity contour may appear smeared, so you should set it as high as allowed by the signal "
	"if you want a sharp contour.")
TERM (U"##Time step (s)")
DEFINITION (U"the time step of the resulting intensity contour. If you set it to zero, the time step is computed as "
	"one quarter of the effective window length, i.e. as 0.8 / %pitchFloor.")
TERM (U"##Subtract mean")
DEFINITION (U"See @@Intro 6.2. Configuring the intensity contour@.")
ENTRY (U"Algorithm")
NORMAL (U"The values in the sound are first squared, then convolved with a Gaussian analysis window (Kaiser-20; sidelobes below -190 dB). "
	"The effective duration of this analysis window is 3.2 / %pitchFloor, which will guarantee that a periodic signal is analysed as having a "
	"pitch-synchronous intensity ripple not greater than 0.00001 dB.")
MAN_END

MAN_BEGIN (U"Sound & IntensityTier: Multiply", U"ppgb", 20000724)
INTRO (U"A command to create a new Sound from the selected @Sound and @Intensity objects.")
NORMAL (U"The resulting Sound equals the original sound, multiplied by a linear interpolation of the intensity. "
	"Afterwards, the resulting Sound is scaled so that its maximum absolute amplitude is 0.9.")
MAN_END

MAN_BEGIN (U"Strings", U"ppgb", 20201226)
INTRO (U"One of the @@types of objects@ in Praat. Represents an ordered list of strings.")
ENTRY (U"Creation")
NORMAL (U"The difficult way is to create a #Strings object from a generic Praat text file "
	"(if there are non-ASCII symbols, use UTF-8 or UTF-16 format):")
CODE (U"\"ooTextFile\"")
CODE (U"\"Strings\"")
CODE (U"5 ! number of strings")
CODE (U"\"Hello\"")
CODE (U"\"Goodbye\"")
CODE (U"\"Auf wiedersehen\"")
CODE (U"\"Tschüss\"")
CODE (U"\"Arrivederci\"")
NORMAL (U"In this example, we see that a double quote within a string should be written twice; "
	"the fourth string will therefore be read as ##Tschüss#. "
	"This file can be read simply with the generic @@Read from file...@ command from the #Open menu.")
NORMAL (U"An easier way is to use the special command @@Read Strings from raw text file...@. "
	"The file can then simply look like this:")
CODE (U"Hello")
CODE (U"Goodbye")
CODE (U"Auf wiedersehen")
CODE (U"Tschüss")
CODE (U"Arrivederci")
NORMAL (U"You can also create a #Strings object from a folder listing or from some other objects:")
LIST_ITEM (U"• @@Create Strings as file list...")
LIST_ITEM (U"• @@Distributions: To Strings...@")
LIST_ITEM (U"• @@OTGrammar: Generate inputs...@")
LIST_ITEM (U"• @@OTGrammar & Strings: Inputs to outputs...@")
MAN_END

MAN_BEGIN (U"Strings: To Distributions", U"ppgb", 19971025)
INTRO (U"A command to analyse each selected @Strings object into a @Distributions object.")
NORMAL (U"The resulting #Distributions will collect the occurrences of every string in the #Strings object, "
	"and put the number of occurrences in its first and only column.")
ENTRY (U"Example")
NORMAL (U"We start from the following #Strings:")
CODE (U"6 (number of strings)")
CODE (U"\"hallo\"")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
NORMAL (U"This will give us the following #Distributions:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"hallo\"         3")
CODE (U"\"dag allemaal\"  1")
CODE (U"\"tot morgen\"    2")
MAN_END

}

/* End of file manual_Fon.cpp */
