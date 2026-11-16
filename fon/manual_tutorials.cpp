/* manual_tutorials.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
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

void manual_tutorials_init (ManPages me);
void manual_tutorials_init (ManPages me) {

MAN_PAGES_BEGIN R"~~~(
################################################################################
"Praat menu"
© Paul Boersma 2016-12-27

The first menu in the @@Objects window@.
In macOS, this menu is in the main menu bar along the top of the screen.

################################################################################
"Analyses menu"
© Paul Boersma 2022-12-02

A menu in the @SoundEditor or @TextGridEditor.

################################################################################
"Copy..."
© Paul Boersma 2011-10-18

One of the fixed buttons in the @@Objects window@.

Availability
============

You can choose this command after selecting one object of any type.

Behaviour
=========

The Objects window copies the selected object, and all the data it contains,
to a new object, which will appear at the bottom of the List of Objects.

Example
=======

If you select “Sound hallo” and click ##Copy...#,
a command window will appear, which prompts you for a name;
after you click #OK, a new object will appear in the list, bearing that name.

################################################################################
"Draw menu"
© Paul Boersma 2001-04-17

A menu that occurs in the @@Dynamic menu@ for many objects.

This menu contains commands for drawing the object to the @@Picture window@,
which will allow you to print the drawing or to copy it to your word processor.

################################################################################
"Dynamic menu"
© Paul Boersma 20110131  // 2023-06-08

A column of buttons in the right-hand part of the @@Objects window@,
plus the @@Save menu@ in the Objects window.

If you select one or more @objects in the list,
the possible actions that you can perform with the selected objects
will appear in the dynamic menu.
These actions can include viewing & editing, saving, drawing,
conversions to other types (including analysis and synthesis), and more.

Example of analysis:
====================

Record a Sound, select it, and click ##To Pitch...#.
This will create a new Pitch object and put it in the list of objects.
You can then edit, write, and draw this Pitch object.

Example of synthesis:
=====================

Create a Speaker, create and edit an Artword, and click ##To Sound...#.

################################################################################
"View & Edit"
© Paul Boersma 2011-01-28

A command in the @@Dynamic menu@ of several types of @objects.

This command puts an @@Editors|editor@ window on the screen, which shows the contents of the selected object.
This window will allow your to view and modify the contents of this object.

################################################################################
"Extract visible formant contour"
© Paul Boersma 2003-03-16

One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.

See @@Intro 5. Formant analysis@

################################################################################
"Extract visible intensity contour"
© Paul Boersma 2003-03-16

One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.

See @@Intro 6. Intensity analysis@

################################################################################
"Extract visible pitch contour"
© Paul Boersma 2003-03-16

One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.

See @@Intro 4. Pitch analysis@

################################################################################
"Extract visible spectrogram"
© Paul Boersma 2003-03-16

One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.

See @@Intro 3. Spectral analysis@

################################################################################
"FAQ (Frequently Asked Questions)"
© Paul Boersma 2007-12-10

,	@@FAQ: How to cite Praat
,	@Unicode
,	@@FAQ: Formant analysis
,	@@FAQ: Pitch analysis
,	@@FAQ: Spectrograms
,	@@FAQ: Scripts

// Hardware
// Pitch: octave errors

################################################################################
"FAQ: Formant analysis"
© Paul Boersma 2003-09-16

#Problem: I get different formant values if I choose to analyse 3 formants
than if I choose to analyse 4 formants.

Solution: the “number of formants” in formant analysis determines the
number of peaks with which the %entire spectrum is modelled. For an average
female voice, you should choose to analyse 5 formants in the region up to 5500 Hz,
even if you are interested only in the first three formants.

#Problem: I often get only 1 formant in a region where I see clearly 2 formants
in the spectrogram.

This occurs mainly in back vowels (F1 and F2 close together) for male voices,
if the “maximum formant” is set to the standard of 5500 Hz, which is appropriate
for female voices. Set the “maximum formant” down to 5000 Hz.
No, Praat comes without a guarantee: the formant analysis is based on LPC,
and this comes with several assumptions as to what a speech spectrum is like.

#Question: what algorithm is used for formant analysis?

Answer: see @@Sound: To Formant (burg)...@.

################################################################################
"FAQ: How to cite Praat"
© Paul Boersma 2005,2010,2014,2025,2026

#Question: how do I cite Praat in my articles?

Answer: nowadays most journals allow you to cite computer programs and web sites.
The style approved by the American Psychological Association,
and therefore by many journals, is like the following
(change the dates and version number as needed):
{-
	writeInfoLine: “Paul Boersma, David Weenink & Anastasia Shchupak (”, appYear(),
	... “). Praat: doing phonetics by computer [Computer program]. Version ”, appVersion$(),
	... “, retrieved ”, appDay(), “ ”, appMonth$(), “ ”, appYear(), “ from https://praat.org”
}
If the journal does not allow you to cite a web site, then try:

Boersma, Paul (2001). Praat, a system for doing phonetics by computer.
%%Glot International% ##5:9/10#, 341-345.

This paper can be downloaded from Boersma’s website.

################################################################################
"FAQ: Pitch analysis"
© Paul Boersma, 2002,2006,2022-2024

Please also consult @@how to choose a pitch analysis method@.

#Question: what algorithm is used for pitch analysis?

Answer: for how the raw pitch analysis method works,
see @@Sound: To Pitch (ac)...@. The 1993 article is downloadable from
https://www.fon.hum.uva.nl/paul/

#Question: why does Praat consider my sound voiceless while I hear it as voiced?

There are at least five possibilities. Most of them can be checked by zooming in on the @waveform.

The first possibility is that the pitch has fallen below the @@pitch floor@. For instance,
your pitch floor could be 50 Hz but the English speaker produces creak at the end of the utterance.
Or your pitch floor could be 50 Hz but the Chinese speaker is in the middle of a third tone.
If this happens, it may help to lower the pitch floor to e.g. 30 Hz (@@Pitch settings...@),
although that may also smooth the pitch curve too much in other places.

The second possibility is that the pitch has moved too fast. This could happen at the end of a Chinese fourth tone,
which drops very fast. If this happens, it may help to use @@pitch analysis by raw cross-correlation@,
although Praat may then hallucinate pitches in other places that you would prefer to consider voiceless.

The third possibility is that the periods are very irregular, as in some pathological voices.
If you want to see a pitch in those cases, it may help to use @@pitch analysis by raw cross-correlation@.
Or it may help to lower the ##voicing threshold# setting
to 0.25 (instead of the standard 0.50) or so (@@Pitch settings...@).

The fourth possibility is that there is a lot of background noise, as in a recording on a busy street.
In such a case, it may help to lower the ##voicing threshold# setting
to 0.25 (instead of the standard 0.50) or so. The disadvantage of lowering this setting is that for non-noisy
recordings, Praat will become too eager to find voicing in some places that you would prefer to consider voiceless;
so make sure to set it back to 0.50 once you have finished analysing the noisy recordings.

The fifth possibility is that the part analysed as voiceless is much less loud than the rest of the sound,
or that the sound contains a loud noise elsewhere. This can be checked by zooming in on the part analysed as voiceless:
if Praat suddenly considers it as voiced, this is a sign that this part is much quieter than the rest.
To make Praat analyse this part as voiced, you can lower the ##silence threshold# setting to 0.01
(instead of the standard 0.09) or so. The disadvantage of lowering this setting is that Praat may start to consider
some distant background sounds (and quiet echos, for instance) as voiced.

#Question: why do I get different results for the maximum pitch if...?

If you select a Sound and choose @@Sound: To Pitch (filtered ac)...@, the time step will usually
be 0.015 seconds. The resulting @Pitch object will have values for times that are
0.015 seconds apart. If you then click Info or choose ##Get maximum pitch# from the @@Query submenu@,
the result is based on those time points. By contrast, if you choose ##Get maximum pitch#
from the @@Pitch menu@ in the SoundEditor window, the result will be based on the visible points,
of which there tend to be a hundred in the visible window. These different time spacings will
lead to slightly different pitch contours.

If you choose ##Move cursor to maximum pitch#, then choose ##Get pitch# from the
@@Pitch menu@, the result will be different again. This is because ##Get maximum pitch#
can do a parabolic interpolation around the maximum, whereas ##Get pitch#, not realizing
that the cursor is at a maximum, does a stupid linear interpolation, which tends to lead to
lower values.

################################################################################
"FAQ: Scripts"
© Paul Boersma 2001,2004,2011,2013,2014,2023

#Question: how do I do something to all the files in a directory?

Answer: look at @`fileNames$#` () or @@Create Strings as file list...@.
,

#Question: why doesn't the editor window react to my commands?

Your commands are probably something like:
{;
	Read from file: “hello.wav”
	View & Edit
	Zoom: 0.3, 0.5
}

Answer: Praat doesn’t know it has to send the #Zoom command to the editor
window called ##14. Sound hello#. There could be several Sound editor windows on your
screen. According to @@Scripting 7.1. Scripting an editor from a shell script@,
you will have to say this explicitly:
{;
	Read from file: “hello.wav”
	View & Edit
	editor: “Sound hello”
	Zoom: 0.3, 0.5
}
,

#Problem: a line like “%`Number = 1`” does not work.

Solution: names of variables should start with a lower-case letter.
,

#Question: why do names of variables have to start with a lower-case letter?
I would like to do things like “`F0 = Get mean pitch`”.

Answer: Praat scripts combine button commands with things that only occur
in scripts. Button commands always start with a capital letter, e.g. `Play` (there is a button #Play).
Script commands always start with lower case, e.g. `writeInfoLine: “Hello”`
($writeInfoLine is a built-in function).
A minimal pair is “$select”, which simulates a mouse click in the object list,
versus “`Select...`”, which sets the selection in editor windows. If we allowed initial capitals,
variable names would become rather ambiguous in assignments,
as in “`x = Get`”, where “%`Get`” would be a variable, versus “`x = Get mean`”,
where ##Get mean# is a button command. To prevent this confusion, Praat enforces
a rigorous lower-case/upper-case distinction.
,

#Question: how do I convert a number into a string?

Answer: `a$ = string$ (a)`

#Question: how do I convert a string into a number?

Answer: `a = number (a$)`

################################################################################
"FAQ: Spectrograms"
© Paul Boersma 2002,2003

#Problem: the background is grey instead of white (too little contrast)

Solution: reduce the ##Dynamic range# in the spectrogram settings. The standard value is 50 dB,
which is fine for detecting small things like plosive voicing in well recorded speech.
For gross features like vowel formants, or for noisy speech, you may want to change the dynamic range
to 40 or even 30 dB.

################################################################################
"File menu"
© Paul Boersma 2002

One of the menus in all @editors, in the @manual, and in the @@Picture window@.

################################################################################
"Filtering"
© Paul Boersma 2001,2003,2008,2010

This tutorial describes the use of filtering techniques in Praat.
It assumes you are familiar with the @Intro.

Frequency-domain filtering
==========================

Modern computer techniques make possible an especially simple batch filtering method:
multiplying the complex spectrum in the frequency domain by any real-valued filter function.
This leads to a zero phase shift for each frequency component. The impulse response is symmetric
in the time domain, which also means that the filter is %acausal: the filtered signal will show components
before they start in the original.

- @@Spectrum: Filter (pass Hann band)...@
- @@Spectrum: Filter (stop Hann band)...@
- @@Sound: Filter (pass Hann band)...@
- @@Sound: Filter (stop Hann band)...@
- @@Sound: Filter (formula)...@

Spectro-temporal:

- @@band filtering in the frequency domain@

Fast time-domain filtering
==========================

Some very fast Infinite Impulse Response (IIR) filters can be defined in the time domain.
These include recursive all-pole filters and pre-emphasis. These filters are causal but have non-zero phase shifts.
There are versions that create new Sound objects:

- @@Sound: Filter (one formant)...@
- @@Sound: Filter (pre-emphasis)...@
- @@Sound: Filter (de-emphasis)...@

And there are in-place versions, which modify the existing Sound objects:

- @@Sound: Filter with one formant (in-place)...@
- @@Sound: Pre-emphasize (in-place)...@
- @@Sound: De-emphasize (in-place)...@

Convolution
===========

A Finite Impulse Response (FIR) filter can be described as a sampled sound.
Filtering with such a filter amounts to a %#convolution of the original sound and the filter:

- @@Sounds: Convolve...@

Described elsewhere
===================

Described in the @@Source-filter synthesis@ tutorial:

- @@Sound & Formant: Filter@
- @@Sound & FormantGrid: Filter@
- @@LPC & Sound: Filter...@
- @@LPC & Sound: Filter (inverse)@

################################################################################
"Formants menu"
© Paul Boersma 2022

A menu in the @SoundEditor or @TextGridEditor.

################################################################################
"Formants & LPC menu"
© Paul Boersma 2001

A menu that occurs in the @@Dynamic menu@ for a @Sound.

This menu contains commands for analysing the formant contours of the selected Sound:

,	@@Sound: To Formant (burg)...
,	@@Sound: To Formant (keep all)...
,	@@Sound: To Formant (sl)...
,	@@Sound: To LPC (autocorrelation)...
,	@@Sound: To LPC (covariance)...
,	@@Sound: To LPC (burg)...
,	@@Sound: To LPC (marple)...
,	@@Sound: To MFCC...

################################################################################
"Get first formant"
© Paul Boersma 2001,2022

One of the query commands in the @@Formants menu@ of the @SoundEditor and the @TextGridEditor.

################################################################################
"Get pitch"
© Paul Boersma 2001,2022

One of the query commands in the @@Pitch menu@ of the @SoundEditor and the @TextGridEditor.

################################################################################
"Get second formant"
© Paul Boersma 2001,2022

One of the query commands in the @@Formants menu@ of the @SoundEditor and the @TextGridEditor.

################################################################################
"Goodies"
© Paul Boersma 2002,2005

The title of a submenu of the @@Praat menu@.

################################################################################
"Info"
© Paul Boersma 1996,1998

One of the fixed buttons in the @@Objects window@.

Availability
============

You can choose this command after choosing one object.

Purpose
=======

To get some information about the selected object.

Behaviour
=========

The information will appear in the @@Info window@.

################################################################################
"Info window"
© Paul Boersma 1997,1998,2003,2023

A text window into which many query commands write their answers.

You can select text from this window and copy it to other places.

In a @@Praat script@, you can bypass the Info window by having a query command
writing directly into a script variable.

Apart from the @Info command, which writes general information about the selected object,
most commands in any @@Query submenu@ also write into the Info window.

################################################################################
"Inspect"
© Paul Boersma 1996

One of the fixed buttons in the @@Objects window@.

You can use this command after selecting one object in the list.

The contents of the selected object will become visible in a Data Editor.
You can then view and change the data in the object,
but beware: changing the data directly in this way may render them inconsistent.

Changes that you make to the data with another Editor (e.g., a SoundEditor),
or with the commands under #Modify,
are immediately reflected in the top-level Data Editor;
any subeditors are destroyed, however, because they may now refer to invalid data.

Changes that you make to the data with a Data Editor,
are immediately reflected in any open type-specific Editors (e.g., a SoundEditor).

################################################################################
"Intro"
© Paul Boersma 2003,2004,2007,2009,2011,2025

This is an introductory tutorial to Praat, a computer program
with which you can analyse, synthesize, and manipulate speech,
and create high-quality pictures for your articles and thesis.
You are advised to work through all of this tutorial.

You can read this tutorial sequentially with the help of the “##1 >#” and “##< 1#” buttons,
or go to the desired information by clicking on the blue links.

, @@Intro 1. How to get a sound@:
	@@Intro 1.1. Recording a sound|record@,
	@@Intro 1.2. Reading a sound from disk|read@,
	@@Intro 1.3. Creating a sound from a formula|formula@.
, @@Intro 2. What to do with a sound@:
	@@Intro 2.1. Saving a sound to disk|write@,
	@@Intro 2.2. Viewing and editing a sound|view@.
, @@Intro 3. Spectral analysis
	, spectrograms: @@Intro 3.1. Viewing a spectrogram|view@,
		@@Intro 3.2. Configuring the spectrogram|configure@,
		@@Intro 3.3. Querying the spectrogram|query@,
		@@Intro 3.4. Printing the spectrogram|print@,
		@@Intro 3.5. The Spectrogram object|the Spectrogram object@.
	, spectral slices: @@Intro 3.6. Viewing a spectral slice|view@,
		@@Intro 3.7. Configuring the spectral slice|configure@,
		@@Intro 3.8. The Spectrum object|the Spectrum object@.
, @@Intro 4. Pitch analysis
	, pitch contours: @@Intro 4.1. Viewing a pitch contour|view@,
		@@Intro 4.2. Configuring the pitch contour|configure@,
		@@Intro 4.3. Querying the pitch contour|query@,
		@@Intro 4.4. Printing the pitch contour|print@,
		@@Intro 4.5. The Pitch object|the Pitch object@.
, @@Intro 5. Formant analysis
	, formant contours: @@Intro 5.1. Viewing formant contours|view@,
		@@Intro 5.2. Configuring the formant contours|configure@,
		@@Intro 5.3. Querying the formant contours|query@,
		@@Intro 5.4. The Formant object|the Formant object@.
, @@Intro 6. Intensity analysis
	, intensity contours: @@Intro 6.1. Viewing an intensity contour|view@,
		@@Intro 6.2. Configuring the intensity contour|configure@,
		@@Intro 6.3. Querying the intensity contour|query@,
		@@Intro 6.4. The Intensity object|the Intensity object@.
, @@Intro 7. Annotation
, @@Intro 8. Manipulation@: of
	@@Intro 8.1. Manipulation of pitch|pitch@,
	@@Intro 8.2. Manipulation of duration|duration@,
	@@Intro 8.3. Manipulation of intensity|intensity@,
	@@Intro 8.4. Manipulation of formants|formants@.

There are also more specialized tutorials:
, Phonetics:
	• Voice analysis (jitter, shimmer, noise): @Voice
	• Listening experiments: @@ExperimentMFC@
	• @@Sound files@
	• @@Filtering@
	• @@Source-filter synthesis@
	• @@Articulatory synthesis@
, Learning:
	• @@Feedforward neural networks@
	• @@OT learning@
, Statistics:
	• @@Principal component analysis@
	• @@Multidimensional scaling@
	• @@Discriminant analysis@
, General:
	• @@Scripting@
	• @@Demo window@
	• @@Printing@

The authors
===========

The Praat program was created by Paul Boersma and David Weenink of
the Institute of Phonetics Sciences of the University of Amsterdam.
Home page: #`https://praat.org` or #`https://www.fon.hum.uva.nl/praat`.

For questions and suggestions, mail to the Praat discussion list,
which is reachable from the Praat home page, or directly to #`paul.boersma@uva.nl`.

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Intro 1. How to get a sound", U"ppgb", 20021212)
INTRO (U"Most of the things most people do with Praat start with a sound. "
	"There are at least three ways to get a sound:")
LIST_ITEM (U"@@Intro 1.1. Recording a sound")
LIST_ITEM (U"@@Intro 1.2. Reading a sound from disk")
LIST_ITEM (U"@@Intro 1.3. Creating a sound from a formula")
MAN_END

MAN_BEGIN (U"Intro 1.1. Recording a sound", U"ppgb", 20201120)
INTRO (U"To record a speech sound into Praat, you need a computer with a microphone.")
NORMAL (U"To record from the microphone, perform the following steps:")
LIST_ITEM (U"1. Choose @@Record mono Sound...@ from the @@New menu@ in the @@Objects window@. "
	"A @SoundRecorder window will appear on your screen.")
LIST_ITEM (U"2. On the left in the SoundRecorder window, choose the appropriate input device, e.g. choose ##Internal microphone#. "
	"(On Windows, instead right-click the loudspeaker symbol in the Start bar; see @SoundRecorder for more details.)")
LIST_ITEM (U"3. Use the #Record and #Stop buttons to record a few seconds of your speech.")
LIST_ITEM (U"4. Use the #Play button to hear what you have recorded.")
LIST_ITEM (U"5. Repeat steps 3 and 4 until you are satisfied with your recording.")
LIST_ITEM (U"6. Click the ##Save to list# button. Your recording will now appear in the Objects window, "
	"where it will be called “Sound sound”.")
LIST_ITEM (U"7. You can now close the SoundRecorder window.")
LIST_ITEM (U"8. When you saved your sound to the Objects window, some buttons appeared in that window. "
	"These buttons show you what you can do with the sound. Try the #Play and @@View & Edit@ buttons.")
NORMAL (U"For more information on recording, see the @SoundRecorder manual page.")
MAN_END

MAN_BEGIN (U"Intro 1.2. Reading a sound from disk", U"ppgb", 20041126)
INTRO (U"Apart from recording a new sound from a microphone, you could read an existing sound file from your disk.")
NORMAL (U"With @@Read from file...@ from the @@Open menu@, "
	"Praat will be able to read most standard types of sound files, e.g. WAV files. "
	"They will appear as @Sound objects in the Objects window. For instance, if you open the file ##hello.wav#, "
	"an object called “Sound hello” will appear in the list.")
NORMAL (U"If you do not have a sound file on your disk, you can download a WAV file (or so) from the Internet, "
	"then read that file into Praat with ##Read from file...#.")
MAN_END

MAN_BEGIN (U"Intro 1.3. Creating a sound from a formula", U"ppgb", 20070225)
INTRO (U"If you have no microphone, no sound files on disk, and no access to the Internet, "
	"you could still create a sound with @@Create Sound from formula...@ from the @@New menu@.")
MAN_END

MAN_BEGIN (U"Intro 2. What to do with a sound", U"ppgb", 20110131)
INTRO (U"As soon as you have a @Sound in the @@List of Objects@, "
	"the buttons in the @@Dynamic menu@ (the right-hand part of the @@Objects window@) "
	"will show you what you can do with it.")
LIST_ITEM (U"@@Intro 2.1. Saving a sound to disk")
LIST_ITEM (U"@@Intro 2.2. Viewing and editing a sound")
MAN_END

MAN_BEGIN (U"Intro 2.1. Saving a sound to disk", U"ppgb", 20110131)
INTRO (U"There are several ways to write a sound to disk.")
NORMAL (U"First, the @@File menu@ of the @SoundRecorder window contains commands to save the left "
	"channel, the right channel, or both channels of the recorded sound to any of four standard types "
	"of sound files (WAV, AIFC, NeXT/Sun, NIST). These four file types are all equally good for Praat: "
	"Praat will handle them equally well on every computer. The first three of these types will "
	"also be recognized by nearly all other sound-playing programs.")
NORMAL (U"Then, once you have a @Sound object in the @@List of Objects@, "
	"you can save it in several formats with the commands in the @@Save menu@. "
	"Again, the WAV, AIFF, AIFC, NeXT/Sun, and NIST formats are equally fine.")
NORMAL (U"For more information, see the @@Sound files@ tutorial.")
MAN_END

MAN_BEGIN (U"Intro 2.2. Viewing and editing a sound", U"ppgb", 20230202)   // 2023-06-08
NORMAL (U"To see the wave form of a @Sound that is in the list of objects, "
	"select that Sound and click @@View & Edit@. A @SoundEditor window will appear on your screen. "
	"You see a waveform (or two waveforms, if you have a stereo sound) and probably some “analyses” below it. "
	"You can zoom in and scroll to see the various parts of the sound in detail. "
	"You can select a part of the sound by dragging with the mouse. "
	"To play a part of the sound, click in any of the rectangles below it. "
	"To move a selected part of the sound to another location, use #Cut and #Paste from the Edit menu. "
	"You can open sound windows for more than one sound, and then cut, copy, and paste between the sounds, "
	"just as you are used to doing with text and pictures in word processing programs.")
NORMAL (U"If your sound file is longer than a couple of minutes, "
	"or if you want to see and listen to both channels of a stereo sound, "
	"you may prefer to open it with @@Open long sound file...@. "
	"This puts a @LongSound object into the list. In this way, most of the sound will stay in the file on disk, "
	"and at most 60 seconds will be read into memory each time you play or view a part of it. "
	"To change these 60 seconds to something else, e.g. 500 seconds, "
	"choose ##LongSound settings...# from the #Settings submenu of the Praat menu.")
MAN_END

MAN_BEGIN (U"Intro 3. Spectral analysis", U"ppgb", 20070905)
INTRO (U"This section describes how you can analyse the spectral content of an existing sound. "
	"You will learn how to use %spectrograms and %%spectral slices%.")
LIST_ITEM (U"@@Intro 3.1. Viewing a spectrogram")
LIST_ITEM (U"@@Intro 3.2. Configuring the spectrogram")
LIST_ITEM (U"@@Intro 3.3. Querying the spectrogram")
LIST_ITEM (U"@@Intro 3.4. Printing the spectrogram")
LIST_ITEM (U"@@Intro 3.5. The Spectrogram object")
LIST_ITEM (U"@@Intro 3.6. Viewing a spectral slice")
LIST_ITEM (U"@@Intro 3.7. Configuring the spectral slice")
LIST_ITEM (U"@@Intro 3.8. The Spectrum object")
MAN_END

MAN_BEGIN (U"Intro 3.1. Viewing a spectrogram", U"ppgb", 20110128)  // 2023-06-08
INTRO (U"To see the spectral content of a sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor or @LongSoundEditor window will appear on your screen. "
	"In the entire bottom half of this window you will see a greyish image, which is called a %spectrogram. "
	"If you do not see it, choose @@Show spectrogram@ from the Spectrogram menu.")
NORMAL (U"The spectrogram is a @@spectro-temporal representation@ of the sound. "
	"The horizontal direction of the spectrogram represents @time, the vertical direction represents @frequency. "
	"The time scale of the spectrogram is the same as that of the waveform, so the spectrogram reacts "
	"to your zooming and scrolling. "
	"To the left of the spectrogram, you see the frequency scale. The frequency at the bottom of the spectrogram "
	"is usually 0 Hz (hertz, cps, cycles per second), and a common value for the frequency at the top is 5000 Hz.")
NORMAL (U"Darker parts of the spectrogram mean higher energy densities, lighter parts mean lower energy densities. "
	"If the spectrogram has a dark area around a time of 1.2 seconds and a frequency of 4000 Hz, "
	"this means that the sound has lots of energy for those high frequencies at that time. "
	"For many examples of spectrograms of speech sounds, see the textbook by @@Ladefoged (2001)@ and "
	"the reference work by @@Ladefoged & Maddieson (1996)@.")
NORMAL (U"To see what time and frequency a certain part of the spectrogram is associated with, "
	"just click in the spectrogram and you will see the vertical time cursor showing the time above "
	"the waveform and the horizontal frequency cursor showing the frequency to the left of the spectrogram. "
	"This is one of the ways to find the %formant frequencies for vowels, or the main spectral peaks "
	"for fricatives.")
ENTRY (U"Hey, there are white vertical stripes at the edges!")
NORMAL (U"This is normal. Spectral analysis requires an %%analysis window% of a certain duration. "
	"For instance, if Praat wants to know the spectrum at 1.342 seconds, it needs to include information "
	"about the signal in a 10-milliseconds window around this time point, i.e., Praat will use "
	"signal information about all times between 1.337 and 1.347 seconds. At the very edges of the sound, "
	"this information is not available: "
	"if the sound runs from 0 to 1.8 seconds, no spectrum can be computed between 0 and 0.005 "
	"seconds or between 1.795 and 1.800 seconds. Hence the white stripes. If you do not see them "
	"immediately when you open the sound, zoom in on the beginning or end of the sound.")
NORMAL (U"When you zoom in on the middle of the sound (or anywhere not near the edges), the white stripes vanish. "
	"Suddenly you see only the time stretch between 0.45 and 1.35 seconds, for instance. "
	"But Praat did not forget what the signal looks like just outside the edges of this time window. "
	"To display a spectrogram from 0.45 to 1.35 seconds, Praat will use information from the wave form "
	"between 0.445 and 1.355 seconds, and if this is available, you will see no white stripes at the edges of the window.")
ENTRY (U"Hey, it changes when I scroll!")
NORMAL (U"This is normal as well, especially for long windows. If your visible time window is 20 seconds long, "
	"and the window takes up 1000 screen pixels horizontally, "
	"then you might think that every one-pixel-wide vertical line should represent the spectrum of 20 milliseconds of sound. "
	"But for reasons of computation speed, Praat will only show the spectrum of the part of the sound "
	"that lies around the centre of those 20 milliseconds, "
	"not the average or sum of all the spectra in those 20 milliseconds. "
	"This %undersampling of the underlying spectrogram is different from what happens in the drawing of the wave form, "
	"where a vertical black line connects the minimum and maximum amplitude of all the samples that fall inside a "
	"screen pixel. We cannot do something similar for spectrograms. And since scrolling goes by fixed time steps "
	"(namely, 5 percent of the duration of the visible window), rather than by a whole number of screen pixels, "
	"the centres of the pixels will fall in different parts of the spectrogram with each scroll. "
	"Hence the apparent changes. If your visible window is shorter than a couple of seconds, "
	"the scrolling spectrogram will appear much smoother.")
NORMAL (U"The darkness of the spectrogram will also change when you scroll, because the visible part with the most "
	"energy is defined as black. When a very energetic part of the signal scrolls out of view, the spectrogram "
	"will turn darker. The next section will describe a way to switch this off.")
MAN_END

MAN_BEGIN (U"Intro 3.2. Configuring the spectrogram", U"ppgb", 20220907)
NORMAL (U"With @@Spectrogram settings...@ from the #Spectrogram menu, "
	"you can determine how the spectrogram is computed and how it is displayed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (\"factory settings\"), which appear "
	"when you click ##Standards#.")
TERM (U"%%View range% (Hz)")
DEFINITION (U"the range of frequencies to display. The standard is 0 Hz at the bottom and 5000 Hz at the top. "
	"If this maximum frequency is higher than the Nyquist frequency of the Sound "
	"(which is half its sampling frequency), some values in the spectrogram will be zero, and the higher "
	"frequencies will be drawn in white. You can see this if you record a Sound at 44100 Hz and set the "
	"view range from 0 Hz to 25000 Hz.")
TERM (U"%%Window length")
DEFINITION (U"the duration of the analysis window. If this is 0.005 seconds (the standard), "
	"Praat uses for each frame the part of the sound that lies between 0.0025 seconds before "
	"and 0.0025 seconds after the centre of that frame "
	"(for Gaussian windows, Praat actually uses a bit more than that). "
	"The window length determines the %bandwidth of the spectral analysis, "
	"i.e. the width of the horizontal line in the spectrogram of a pure sine wave (see below). "
	"For a Gaussian window, the -3 dB bandwidth is 2*sqrt(6*ln(2))/(\\pi*%%Window length%), "
	"or 1.2982804 / %%Window length%. "
	"To get a “broad-band” spectrogram (bandwidth 260 Hz), keep the standard window length of 5 ms; "
	"to get a “narrow-band” spectrogram (bandwidth 43 Hz), set it to 30 ms (0.03 seconds). "
	"The other window shapes give slightly different values.")
TERM (U"%%Dynamic range% (dB)")
DEFINITION (U"All values that are more than %%Dynamic range% dB below the maximum (perhaps after dynamic compression, "
	"see @@Advanced spectrogram settings...@) "
	"will be drawn in white. Values in-between have appropriate shades of grey. Thus, if the highest "
	"peak in the spectrogram has a height of 30 dB/Hz, and the dynamic range is 50 dB (which is the standard value), "
	"then values below -20 dB/Hz will be drawn in white, and values between -20 dB/Hz and 30 dB/Hz will be drawn "
	"in various shades of grey.")
TERM (U"%%Unit%")
DEFINITION (U"the unit of the vertical frequency axis. With the standard value of “Hertz (linear)”, "
	"the vertical axis is linear in Hz. With “Mel”, the vertical axis is linear in Mel, "
	"a perceptual scale that gives relatively more room to the lower frequencies.")
ENTRY (U"The bandwidth")
NORMAL (U"To see how the window length influences the bandwidth, "
	"first create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing `1/2 * sin (2*pi*1000*x)` as the formula, then click ##View & Edit#. "
	"The spectrogram will show a horizontal black line. "
	"You can now vary the window length in the spectrogram settings and see how the thickness "
	"of the lines varies. The line gets thinner if you raise the window length. "
	"Apparently, if the analysis window comprises more periods of the wave, "
	"the spectrogram can tell us the frequency of the wave with greater precision.")
NORMAL (U"To see this more precisely, create a sum of two sine waves, with frequencies of 1000 and 1200 Hz. "
	"the formula is `1/4 * sin (2*pi*1000*x) + 1/4 * sin (2*pi*1200*x)`. In the editor, you will see "
	"a single thick band if the analysis window is short (5 ms), and two separate bands if the analysis "
	"window is long (30 ms). Apparently, the frequency resolution gets better with longer analysis windows.")
NORMAL (U"So why don't we always use long analysis windows? The answer is that their time resolution is poor. "
	"To see this, create a sound that consists of two sine waves and two short clicks. The formula is "
	"`0.02*(sin(2*pi*1000*x)+sin(2*pi*1200*x)) + (col=10000)+(col=10200)`. "
	"If you view this sound, you can see that the two clicks will overlap "
	"in time if the analysis window is long, and that the sine waves overlap in frequency if the "
	"analysis window is short. Apparently, there is a trade-off between time resolution and "
	"frequency resolution. One cannot know both the time and the frequency with great precision.")
ENTRY (U"Advanced settings")
NORMAL (U"The Spectrogram menu also contains @@Advanced spectrogram settings...@.")
MAN_END

MAN_BEGIN (U"Advanced spectrogram settings...", U"ppgb", 20120531)
ENTRY (U"Optimization")
TERM (U"%%Number of time steps%")
DEFINITION (U"the maximum number of points along the time window for which Praat has to compute "
	"the spectrum. If your screen is not wider than 1200 pixels, then the standard of 1000 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-wide vertical line. "
	"If you have a really wide screen, you may see improvement if you raise this number to 1500.")
TERM (U"%%Number of frequency steps%")
DEFINITION (U"the maximum number of points along the frequency axis for which Praat has to compute "
	"the spectrum. If your screen is not taller than 768 pixels, then the standard of 250 is "
	"appropriate, since there is no point in computing more than one spectrum per one-pixel-height horizontal line. "
	"If you have a really tall screen, you may see improvement if you raise this number.")
NORMAL (U"For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8\\Vr\\pi) of the window length, "
	"and the frequency step never needs to be smaller than (\\Vr\\pi)/8 of the inverse of the window length. "
	"For instance, if the window length is 5 ms, "
	"the actual time step will never be less than 5/(8\\Vr\\pi) = 0.353 ms, "
	"and the actual frequency step will never be less than (\\Vr\\pi)/8/0.005 = 44.31 Hz.")
ENTRY (U"Spectrogram analysis settings")
TERM (U"%%Method")
DEFINITION (U"there is currently only one method available in this window for computing a spectrum from "
	"a sound: the Fourier transform.")
TERM (U"%%Window shape")
DEFINITION (U"the shape of the analysis window. To compute the spectrum at, say, 3.850 seconds, "
	"samples that lie close to 3.850 seconds are given more weight than samples further away. "
	"The relative extent to which each sample contributes to the spectrum is given by the window shape. "
	"You can choose from: Gaussian, Square (none, rectangular), Hamming (raised sine-squared), "
	"Bartlett (triangular), Welch (parabolic), and Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no %sidelobes in your spectrogram (see below); "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
ENTRY (U"Sidelobes; anybody wants to win a cake?")
NORMAL (U"The Gaussian window is the only shape that we can consider seriously as a candidate for "
	"the analysis window. To see this, create a 1000-Hz sine wave with @@Create Sound from formula...@ "
	"by typing `1/2 * sin (2*pi*1000*x)` as the formula, then click ##View & Edit#. "
	"If the window shape is Gaussian, the spectrogram will show a horizontal black line. "
	"If the window shape is anything else, the spectrogram will show many horizontal grey lines (%sidelobes), "
	"which do not represent anything that is available in the signal. They are artifacts of the "
	"window shapes.")
NORMAL (U"We include these other window shapes only for pedagogical purposes "
	"and because the Hanning and Hamming windows have traditionally been used in other programs before "
	"computers were as fast as they are now (a spectrogram is computed twice as fast "
	"with these other windows). Several other programs still use these inferior window shapes, and you are "
	"likely to run into people who claim that the Gaussian window has disadvantages. "
	"We promise such people a large cake if they can come up with sounds that look better "
	"with Hanning or Hamming windows than with a Gaussian window. An example of the reverse is easy "
	"to find; we have just seen one.")
ENTRY (U"Spectrogram blackness settings")
TERM (U"%%Autoscaling%")
TERM (U"%%Maximum% (dB/Hz)")
DEFINITION (U"all parts of the spectrogram that have a power above %maximum (after preemphasis) "
	"will be drawn in black. The standard maximum is 100 dB/Hz, but if %autoscaling is on (which is the standard), "
	"Praat will use the maximum of the visible part of the spectrogram instead; "
	"this ensures that the window will always look well, but it also means that the blackness "
	"of a certain part of the spectrogram will change as you scroll.")
TERM (U"%%Preemphasis% (dB/octave)")
DEFINITION (U"determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this setting is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones. "
	"When you raise the preemphasis, frequency bands above 1000 Hz will become darker, those below 1000 Hz will become lighter.")
TERM (U"%%Dynamic compression")
DEFINITION (U"determines how much stronger weak spectra should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0 (the standard value), there is no dynamic compression. "
	"If it is 1, all spectra will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a spectrum with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN (U"Intro 3.3. Querying the spectrogram", U"ppgb", 20221202)
NORMAL (U"If you click anywhere inside the spectrogram, a cursor cross will appear, "
	"and you will see the time and frequency in red at the top and to the left of the window. "
	"To see the time in the Info window, "
	"choose ##Get cursor# from the #Spectrogram menu or press the F6 key. "
	"To see the frequency in the Info window, "
	"choose ##Get frequency# from the #Spectrogram menu.")
NORMAL (U"To query the power of the spectrogram at the cursor cross, "
	"choose ##Get spectral power at cursor cross# from the #Spectrum menu or press the F9 key. "
	"The Info window will show you the power density, expressed in Pascal^2/Hz.")
MAN_END

MAN_BEGIN (U"Intro 3.4. Printing the spectrogram", U"ppgb", 20220907)
NORMAL (U"To print a spectrogram, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to paint it into the @@Picture window@. "
	"You do this by choosing ##Paint visible spectrogram...# "
	"from the Spectrogram menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (U"Intro 3.5. The Spectrogram object", U"ppgb", 20220907)
NORMAL (U"To do more with spectrograms, you can create a @Spectrogram object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible spectrogram# "
	"from the Spectrogram menu in the Sound or TextGrid window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Spectrogram...@ from the #Spectrum menu. "
	"In either case, a new Spectrogram object will appear in the list. "
	"To draw this Spectrogram object to the @@Picture window@, "
	"select it and choose the @@Spectrogram: Paint...@ command. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard. "
	"Many other commands are available in the @@dynamic menu@.")
MAN_END

MAN_BEGIN (U"Intro 3.6. Viewing a spectral slice", U"ppgb", 20220907)
INTRO (U"With ##View spectral slice# from the #Spectrogram menu in the @SoundEditor and the @TextGridEditor, "
	"you can see the frequency spectrum at the time cursor "
	"or the average frequency spectrum in the time selection.")
ENTRY (U"Spectral slice at the cursor")
NORMAL (U"If you click anywhere in the wave form of the SoundEditor or TextGridEditor windows, "
	"a cursor will appear at that time. If you then choose ##View spectral slice#, "
	"Praat will create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal around the cursor position.")
ENTRY (U"Spectral slice from a selection")
NORMAL (U"If you drag the mouse through the wave form of the SoundEditor or TextGridEditor windows, "
	"a @@time selection@ will appear. If you then choose ##View spectral slice#, "
	"Praat will again create a @Spectrum object named %slice in the Objects window and show it in a @SpectrumEditor window. "
	"In this way, you can inspect the frequency contents of the signal in the selection.")
MAN_END


MAN_BEGIN (U"Intro 3.7. Configuring the spectral slice", U"ppgb", 20110128)
ENTRY (U"Spectral slice at the cursor")
NORMAL (U"What Praat does precisely, depends on your Spectrogram settings. "
	"Suppose that the %%window length% setting is 0.005 seconds (5 milliseconds). "
	"If the %%window shape% is not Gaussian, Praat will extract the part of the sound "
	"that runs from 2.5 milliseconds before the cursor to 2.5 ms after the cursor. "
	"Praat then multiplies this 5 ms long signal by the window shape, then computes a spectrum "
	"with the method of @@Sound: To Spectrum...@, which is put into the Objects window and opened in an editor window. "
	"If the window shape is Gaussian, Praat will extract a part of the sound "
	"that runs from 5 milliseconds before the cursor to 5 ms after the cursor. The spectrum will then be based "
	"on a “physical” window length of 10 ms, although the “effective” window length is still 5 ms "
	"(see @@Intro 3.2. Configuring the spectrogram@ for details).")
ENTRY (U"Spectral slice from a selection")
NORMAL (U"What Praat does precisely, again depends on the %%window shape% of your Spectrogram settings. "
	"Suppose that your selection is 50 ms long. Praat will extract the entire selection, "
	"then multiply this 50 ms long signal by the window shape, then compute a spectrum, put it into the Objects window and open it an editor window. "
	"This procedure is equivalent to choosing ##Extract windowed selection...# (with a %%relative duration% of 1.0), "
	"followed by ##To Spectrum...# (with %fast switched on), followed by #Edit.")
NORMAL (U"If the window is Gaussian, Praat will still only use the selection, without doubling its duration. "
	"This means that the spectrum that you see in this case will mainly be based on the centre half of the selection, "
	"and the signal near the edges will be largely ignored.")
MAN_END

MAN_BEGIN (U"Intro 3.8. The Spectrum object", U"ppgb", 20030403)
NORMAL (U"To compute a Fourier frequency spectrum of an entire sound, "
	"select a @Sound object and choose @@Sound: To Spectrum...|To Spectrum...@ from the #Spectrum menu. "
	"A new @Spectrum object will appear in the @@List of Objects@. "
	"To view or modify it (or listen to its parts), click @@View & Edit@. "
	"To print it, choose one of the #Draw commands to draw the Spectrum object to the @@Picture window@ first.")
MAN_END

MAN_BEGIN (U"Intro 4. Pitch analysis", U"ppgb", 20070905)
INTRO (U"This section describes how you can analyse the pitch contour of an existing sound.")
LIST_ITEM (U"@@Intro 4.1. Viewing a pitch contour")
LIST_ITEM (U"@@Intro 4.2. Configuring the pitch contour")
LIST_ITEM (U"@@Intro 4.3. Querying the pitch contour")
LIST_ITEM (U"@@Intro 4.4. Printing the pitch contour")
LIST_ITEM (U"@@Intro 4.5. The Pitch object")
MAN_END

MAN_BEGIN (U"Intro 4.1. Viewing a pitch contour", U"ppgb", 20110128)
NORMAL (U"To see the pitch contour of an existing sound as a function of time, "
	"select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The bottom half of this window will contain a pitch contour, drawn as a blue line or as a sequence of blue dots. "
	"If you do not see the pitch contour, choose @@Show pitch@ from the #Pitch menu.")
NORMAL (U"To the right of the window, you may see three pitch values, written with blue digits: "
	"at the bottom, you see the floor of the viewable pitch range, perhaps 75 Hz; at the top, "
	"you see the ceiling of the pitch range, perhaps 600 Hz; and somewhere in between, you see the pitch value "
	"at the cursor, or the average pitch in the selection.")
MAN_END

MAN_BEGIN (U"Intro 4.2. Configuring the pitch contour", U"ppgb", 20231115 /*20190331*/)  /*2024*/
NORMAL (U"With @@Pitch settings...@ from the #Pitch menu, "
	"you can determine how the pitch contour is displayed and how it is computed. "
	"These settings will be remembered across Praat sessions. "
	"All these settings have standard values (“factory settings”), which appear "
	"when you click #Standards.")
ENTRY (U"The %%pitch range% (%%pitch floor and top%) setting")
NORMAL (U"This is the most important setting for pitch analysis. "
	"The standard range (for filtered autocorrelation) is from 50 to 800 hertz, "
	"which means that the pitch analysis method will only find values between 50 and 800 Hz "
	"(and values between 400 and 800 Hz only rarely). "
	"The range that you set here will be shown to the right of the analysis window.")
NORMAL (U"You may have set the range to values appropriate for your speaker, "
	"because speakers can vary enormously in their pitch ranges. "
	"For some low-pitched (e.g. average male) voices, you might want to set the floor to 50 Hz, and the top to 600 Hz; "
	"for some high-pitched (e.g. average female) voices, a range of 100-800 Hz might instead be appropriate; "
	"however, it may well be the case that the standard setting of 50–800 Hz will work for all of these voices. "
	"On the high side, however, some children can reach almost 2000 Hz when yelling; "
	"on the low side, creaky voice can go as low as 30 Hz; "
	"if you investigate such cases, you may therefore want to experiment with this setting.")
NORMAL (U"Here is why you have to supply these settings. If the pitch floor is 50 Hz, "
	"the pitch analysis method requires a 60-millisecond analysis window, "
	"i.e., in order to measure the F0 at a time of, say, 0.850 seconds, "
	"Praat needs to consider a part of the sound that runs from 0.820 to 0.880 seconds. "
	"These 60 milliseconds correspond to 3 maximum pitch periods (3/50 = 0.060). "
	"If you set the pitch floor down to 25 Hz, the analysis window will grow to 120 milliseconds "
	"(which is again 3 maximum pitch periods), i.e., all times between 0.790 and 0.910 seconds will be considered. "
	"This makes it less easy to see fast F0 changes.")
NORMAL (U"So setting the floor of the pitch range is a technical requirement for the pitch analysis. "
	"If you set it too low, you will miss very fast F0 changes, and if you set it too high, "
	"you will miss very low F0 values. For children's voices you can often use 200 Hz, "
	"although 50 Hz will still give you the same time resolution as you get for low-pitched voices.")
ENTRY (U"The %units setting")
NORMAL (U"This setting determines the units of the vertical pitch scale. Most people like to see the pitch range "
	"in hertz, but there are several other possibilities.")
ENTRY (U"View range different from analysis range")
NORMAL (U"Normally, the range of pitch values that can be seen in the editor window is equal to the range of pitch values "
	"that the analysis algorithm can determine. If you set the analysis range from 50 to 800 Hz, this will be the range "
	"you see in the editor window as well. If the pitch values in the curve happen to be between 350 and 400 Hz, "
	"you may want to zoom in to the 350-400 Hz pitch region. "
	"You will usually do this by changing the pitch range in the @@Pitch settings...@ window. "
	"However, the analysis range will also change in that case, so that the curve itself may change. "
	"If you do not want that, you can change the %%View range% settings "
	"from “0.0 (= auto)” - “0.0 (= auto)” to something else, perhaps “350” - “400”.")
ENTRY (U"Further reading")
NORMAL (U"For more details, see:")
LIST_ITEM (U"@@how to choose a pitch analysis method@")
LIST_ITEM (U"@@pitch analysis by filtered autocorrelation@")
LIST_ITEM (U"@@pitch analysis by raw cross-correlation@")
LIST_ITEM (U"@@pitch analysis by raw autocorrelation@")
LIST_ITEM (U"@@pitch analysis by filtered cross-correlation@")
MAN_END

MAN_BEGIN (U"Time step settings...", U"ppgb", 20231115 /*20031003,20220814*/)
INTRO (U"A command in the #Analysis menu of the @SoundEditor and @TextGridEditor "
	"to determine the time interval between consecutive measurements "
	"of pitch, formants, and intensity.")
ENTRY (U"Automatic time steps")
NORMAL (U"It is recommended that you set the %%Time step strategy% to #Automatic. "
	"In this way, Praat computes just enough pitch, formant, and intensity values to draw "
	"reliable pitch, formant, and intensity contours. In general, Praat will compute 4 values "
	"within an analysis window (“four times oversampling”).")
NORMAL (U"As described in @@Sound: To Pitch (filtered ac)...@, "
	"Praat's standard time step for pitch analysis is 0.75 divided by the pitch floor, "
	"e.g., if the pitch floor is 50 Hz, the time step will be 0.015 seconds. "
	"In this way, there will be 4 pitch measurements within an analysis window, which is 3 / (50 Hz) = 60 milliseconds long.")
NORMAL (U"As described in @@Sound: To Formant (burg)...@, Praat's standard time step for formant measurements is the %%Window length% divided by 4, "
	"e.g. if the window length is 0.025 seconds, the time step will be 6.25 milliseconds.")
NORMAL (U"As described in @@Sound: To Intensity...@, Praat's standard time step for intensity measurements is 0.8 divided by the pitch floor, "
	"e.g. if the pitch floor is 50 Hz, the time step will be 16 milliseconds. "
	"In this way, there will be 4 intensity measurements within an intensity analysis window, "
	"which is 3.2 / (50 Hz) = 64 milliseconds long.")
ENTRY (U"Fixed time step")
NORMAL (U"You can override the automatic time step by setting the %%Time step strategy% to #Fixed. "
	"The %%Fixed time step% setting then determines the time step that Praat will use: "
	"if you set it to 0.001 seconds, Praat will compute pitch, formant, and intensity values for every millisecond. "
	"Beware that this can slow down the editor window appreciably, because this step is much smaller "
	"than usual values of the automatic time step (see above).")
NORMAL (U"Enlarging the time step to e.g. 0.1 seconds will speed up the editor window "
	"but may render the pitch, formant, and intensity curves less exact (they become %undersampled), "
	"which will influence your measurements and the locations of the pulses.")
NORMAL (U"If there are fewer than 2.0 pitch measurement points per analysis window, "
	"Praat will draw the pitch curve as separate little blue disks "
	"rather than as a continuous blue curve, in order to warn you of the undersampling. "
	"E.g. if the pitch floor is 75 Hz, Praat will draw the pitch curve as disks if the time step is greater than 0.02 seconds.")
ENTRY (U"View-dependent time step")
NORMAL (U"Another way to override the standard time step is by setting the %%Time step strategy% to ##View-dependent#. "
	"The %%Number of time steps per view% setting then determines the time step that Praat will use: "
	"if you set it to 100, Praat will always compute 100 pitch, formant, and intensity values within the view window. "
	"More precisely: if you zoom the view window to 3 seconds, Praat will show you 100 pitch, formant, and intensity points at distances "
	"of 0.03 seconds (or fewer than 100, if you are near the left or right edge of the signal). "
	"As with the %%Fixed time step% setting, Praat will draw the pitch as separate disks in case of undersampling. "
	"You may want to use this setting if you want the pitch curve to be drawn equally fast independently of the degree "
	"of zooming.")
MAN_END

MAN_BEGIN (U"Intro 4.3. Querying the pitch contour", U"ppgb", 20040614)
NORMAL (U"With @@Get pitch@ from the #Pitch menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the pitch at the cursor or in the selection. "
	"If a cursor is visible in the window, ##Get pitch# writes to the @@Info window@ "
	"the linearly interpolated pitch at that time; "
	"if a time selection is visible inside the window, ##Get pitch# writes to the @@Info window@ "
	"the mean (average) pitch in the visible part of that selection; "
	"otherwise, ##Get pitch# writes the average pitch in the visible part of the sound.")
MAN_END

MAN_BEGIN (U"Intro 4.4. Printing the pitch contour", U"ppgb", 20070905)
NORMAL (U"To print a pitch contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to draw it into the @@Picture window@. "
	"You do this by choosing ##Draw visible pitch contour...# "
	"from the Pitch menu in the Sound or TextGrid window. "
	"From the File menu in the Picture window, you can then print it, save it to an EPS file, "
	"or copy it to the clipboard (to do Paste in your word processor, for instance).")
MAN_END

MAN_BEGIN (U"Intro 4.5. The Pitch object", U"ppgb", 20110128)
NORMAL (U"The pitch contour that is visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Pitch object to the @@List of Objects@. To do this, "
	"choose @@Extract visible pitch contour@ from the #Pitch menu.")
NORMAL (U"Another way to get a separate Pitch object is to select a @Sound object in the list "
	"choose @@Sound: To Pitch...@ (preferred) or any of the other methods from the @@Periodicity menu@.")
NORMAL (U"To view and modify the contents of a Pitch object, select it and choose @@View & Edit@. "
	"This creates a @PitchEditor window on your screen.")
NORMAL (U"To save a pitch contour to disk, select the @Pitch object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (U"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
NORMAL (U"To draw a @Pitch object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (U"Advanced pulses settings...", U"ppgb", 20110220)
INTRO (U"A command in the #Pulses menu of the @SoundEditor or @TextGridEditor windows. "
	"Before changing the advanced pulses settings, make sure you understand "
	"the @@Voice@ tutorial.")
NORMAL (U"For information about the ##Maximum period factor# setting, see @@PointProcess: Get jitter (local)...@. "
	"The standard setting is best in most cases. "
	"The ##Period floor# and ##Period ceiling# settings derive from the pitch floor and pitch ceiling (@@Pitch settings...@), "
	"according to a formula given in @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN (U"Intro 5. Formant analysis", U"ppgb", 20030316)
INTRO (U"This section describes how you can analyse the formant contours of an existing sound.")
LIST_ITEM (U"@@Intro 5.1. Viewing formant contours")
LIST_ITEM (U"@@Intro 5.2. Configuring the formant contours")
LIST_ITEM (U"@@Intro 5.3. Querying the formant contours")
LIST_ITEM (U"@@Intro 5.4. The Formant object")
MAN_END

MAN_BEGIN (U"Intro 5.1. Viewing formant contours", U"ppgb", 20110128)
NORMAL (U"To see the formant contours of a sound as functions of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain formant contours, drawn as red speckles. "
	"If you do not see the formant contours, choose @@Show formant@ from the #Formant menu.")
MAN_END

MAN_BEGIN (U"Intro 5.2. Configuring the formant contours", U"ppgb", 20030316)
NORMAL (U"The formant analysis parameters, with you can set with the #Formant menu, are important. "
	"For a female voice, you may want to set the maximum frequency to 5500 Hz; "
	"for a male voice, set it to 5000 Hz instead. "
	"For more information about analysis parameters, see @@Sound: To Formant (burg)...@.")
MAN_END

MAN_BEGIN (U"Intro 5.3. Querying the formant contours", U"ppgb", 20040616)
NORMAL (U"With @@Get first formant@ from the Formant menu in the @SoundEditor or @TextGridEditor, "
	"you get information about the first formant at the cursor or in the selection. "
	"If there is a cursor, ##Get first formant# writes to the @@Info window@ the linearly interpolated first formant at that time. "
	"If there is a true selection, ##Get first formant# writes to the @@Info window@ the mean first formant in the visble part of that selection. "
	"The same goes for @@Get second formant@ and so on.")
MAN_END

MAN_BEGIN (U"Intro 5.4. The Formant object", U"ppgb", 20030316)
NORMAL (U"The formant contours that are visible in the @SoundEditor or @TextGridEditor window, "
	"can be copied as a separate @Formant object to the @@List of Objects@. To do this, "
	"choose @@Extract visible formant contour@ from the Formant menu.")
NORMAL (U"Another way to get a separate Formant object is to select a @Sound object in the list "
	"choose @@Sound: To Formant (burg)...@ (preferred) or any of the other methods "
	"from the @@Formants & LPC menu@.")
ENTRY (U"Saving formant contours to disk")
NORMAL (U"To save formant contours to disk, select the @Formant object in the list and choose one of the commands in the @@Save menu@.")
NORMAL (U"Later on, you can read the saved file again with @@Read from file...@ from the @@Open menu@.")
ENTRY (U"Drawing formant contours")
NORMAL (U"To draw a @Formant object to the @@Picture window@, select it and choose any of the commands in the @@Draw menu@. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard for inclusion in your word processor.")
MAN_END

MAN_BEGIN (U"Intro 6. Intensity analysis", U"ppgb", 20030316)
INTRO (U"This section describes how you can analyse the intensity contour of an existing sound.")
LIST_ITEM1 (U"@@Intro 6.1. Viewing an intensity contour")
LIST_ITEM1 (U"@@Intro 6.2. Configuring the intensity contour")
LIST_ITEM1 (U"@@Intro 6.3. Querying the intensity contour")
LIST_ITEM1 (U"@@Intro 6.4. The Intensity object")
MAN_END

MAN_BEGIN (U"Intro 6.1. Viewing an intensity contour", U"ppgb", 20110128)
INTRO (U"To see the intensity contour of a sound as a function of time, select a @Sound or @LongSound object and choose @@View & Edit@. "
	"A @SoundEditor window will appear on your screen. "
	"The analysis part of this window will contain an intensity contour, drawn as a yellow or green line "
	"(choose ##Show intensity# from the #Intensity menu if it is not visible). "
	"This also works in the @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Intro 6.2. Configuring the intensity contour", U"ppgb", 20041123)   // 2023
INTRO (U"With ##Intensity settings...# from the #Intensity menu, "
	"you can control how the intensity contour is computed and how it is shown.")
ENTRY (U"The view range")
NORMAL (U"By changing these two numbers you can set the vertical scale. "
	"The standard setting is from 50 to 100 dB, but if you are interested in the power of the background noise "
	"you may want to set it to the range from 0 to 100 dB.")
ENTRY (U"The averaging method")
NORMAL (U"Averaging is what occurs if you make a @@time selection@ (rather than setting the cursor at a single time). "
	"The green number in dB to the left or right side of the intensity contour will show the average "
	"intensity in the selection. The same value is what you get when choosing ##Get intensity# from "
	"the #Intensity menu. You can choose any of the three averaging methods "
	"available for @@Intensity: Get mean...@, or choose for the #median value in the selection.")
ENTRY (U"Pitch floor")
NORMAL (U"The intensity curve is %smoothed, since you usually do not want the intensity curve to go up and down with "
	"the intensity variations within a pitch period. To avoid such pitch-synchronous variations, "
	"the intensity at every time point is a weighted average over many neighbouring time points. "
	"The weighting is performed by a Gaussian (“double Kaiser”) window that has a duration that is determined by the "
	"##Pitch floor# setting (see @@Intro 4.2. Configuring the pitch contour@). "
	"To see more detail than usual, raise the pitch floor; to get more smoothing than usual, lower it. "
	"For more information, see @@Sound: To Intensity...@.")
ENTRY (U"Subtract mean pressure or not?")
NORMAL (U"Many microphones, microphone preamplifiers, or other components in the recording system "
	"can add a constant level to the air pressure (a %%DC offset%). "
	"You can see this at quiet locations in the wave form, "
	"where the average pressure is not zero but the background noise hovers about some non-zero value. "
	"You will usually not want to take this constant pressure seriously, i.e. the intensity curve should "
	"normally not take into account the energy provided by this constant pressure level. "
	"If you switch on the ##Subtract mean pressure# switch (whose standard setting is also on), "
	"the intensity value at a time point is computed by first subtracting the mean pressure around this point, "
	"and then applying the Gaussian window.")
MAN_END

MAN_BEGIN (U"Intro 6.3. Querying the intensity contour", U"ppgb", 20041123)
INTRO (U"To ask for the intensity at the cursor, or the average intensity in the visible part of the selection, "
	"choose ##Get intensity# from the #Intensity menu or press the F11 key. You get the same value in dB that is visible "
	"to the left or right side of the intensity curve, but with more digits.")
MAN_END

MAN_BEGIN (U"Intro 6.4. The Intensity object", U"ppgb", 20041123)
NORMAL (U"To print an intensity contour, or to put it in an EPS file or on the clipboard for inclusion in your word processor, "
	"you first have to create an @Intensity object in the @@List of Objects@. "
	"You do this either by choosing ##Extract visible intensity contour# "
	"from the #Intensity menu in the SoundEditor or TextGridEditor window, "
	"or by selecting a Sound object in the list and choosing @@Sound: To Intensity...@. "
	"In either case, a new Intensity object will appear in the list. "
	"To draw the Intensity object to the @@Picture window@, select it and choose ##Draw...#. "
	"From the Picture window, you can print it, save it to an EPS file, or copy it to the clipboard.")
MAN_END

MAN_BEGIN (U"Intro 7. Annotation", U"ppgb", 20110129)
INTRO (U"You can annotate existing @Sound objects and sound files (@LongSound objects).")
NORMAL (U"The labelling data will reside in a @TextGrid object. This object is separate "
	"from the sound, which means that you will often see two objects in the list: a Sound or LongSound, "
	"and a TextGrid.")
ENTRY (U"Creating a TextGrid")
NORMAL (U"You create a new empty TextGrid from the Sound or LongSound with @@Sound: To TextGrid...@ "
	"or @@LongSound: To TextGrid...@ from the #Annotate menu (which shows up in the Objects window if you select a Sound or LongSound). In this way, the time domain "
	"of the @TextGrid will automatically equal that of the sound (if you choose @@Create TextGrid...@ from "
	"the @@New menu@ instead, you will have to supply the time domain by yourself).")
NORMAL (U"When you create a TextGrid, you specify the names of the %tiers. For instance, if you want to segment "
	"the sound into words and into phonemes, you may want to create two tiers and call them "
	"\"words\" and \"phonemes\" (you can easily add, remove, and rename tiers later). "
	"Since both of these tiers are %%interval tiers% (you label the intervals between the word and phoneme "
	"boundaries, not the boundaries themselves), you specify \"phonemes words\" for %%Tier names%, "
	"and you leave the %%Point tiers% empty.")
ENTRY (U"View and edit")
NORMAL (U"You can edit a TextGrid object all by itself, but you will normally want to see "
	"the sound in the editor window as well. To achieve this, you select both the Sound (or LongSound) and "
	"the TextGrid, and click @@View & Edit@. A @TextGridEditor will appear on your screen. "
	"Like the Sound editor, the TextGrid editor will show you a spectrogram, a pitch contour, a formant contour, "
	"and an intensity contour. This editor will allow "
	"you to add, remove, and edit labels, boundaries, and tiers. Under Help in the TextGridEditor, you will "
	"find the @TextGridEditor manual page. You are strongly advised to read it, because it will show you "
	"how you can quickly zoom (drag the mouse), play (click a rectangle), or edit a label (just type).")
ENTRY (U"Save")
NORMAL (U"You will normally write the TextGrid to disk with @@Save as text file...@ "
	"or @@Save as short text file...@. It is true that @@Save as binary file...@ will also work, but the others "
	"give you a file you can read with any text editor.")
NORMAL (U"However you saved it, you can read the TextGrid into Praat later with @@Read from file...@.")
MAN_END

MAN_BEGIN (U"Intro 8. Manipulation", U"ppgb", 20021212) //2025
LIST_ITEM (U"@@Intro 8.1. Manipulation of pitch")
LIST_ITEM (U"@@Intro 8.2. Manipulation of duration")
LIST_ITEM (U"@@Intro 8.3. Manipulation of intensity")
LIST_ITEM (U"@@Intro 8.4. Manipulation of formants")
MAN_END

MAN_BEGIN (U"Intro 8.1. Manipulation of pitch", U"ppgb", 20220814)
INTRO (U"To modify the pitch contour of an existing @Sound object, "
	"you select this @Sound and click ##To Manipulation#. "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show the pitch contour (@PitchTier) as a series of thick blue dots. "
	"To reduce the number of dots, choose ##Stylize pitch (2 st)# "
	"from the #Pitch menu; it will then be easy to drag the dots "
	"around the time–pitch area (press the Option key to restrict dragging to vertical, "
	"and the Command key to restrict dragging to horizontal; mnemonics: Uption, Come Later).")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the commands from the #Play menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (U"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (U"If you modify the duration curve as well (see @@Intro 8.2. Manipulation of duration@), "
	"the modified sound will be based on the modified pitch and duration.")
ENTRY (U"Cloning a pitch contour")
NORMAL (U"To use the pitch contour of one Manipulation object as the pitch contour of another Manipulation object, "
	"you first choose ##Extract pitch tier# for the first Manipulation object, "
	"then select the resulting PitchTier object together with the other Manipulation object "
	"(e.g. by a click on the PitchTier and a @@Command-click@ on the Manipulation), "
	"and choose ##Replace pitch tier#.")
ENTRY (U"Precise manipulation of pitch")
NORMAL (U"If you know exactly what pitch contour you want, "
	"you can create an empty PitchTier with @@Create PitchTier...@ from the New menu, "
	"then add points with @@PitchTier: Add point...@.")
NORMAL (U"For instance, suppose you want to have a pitch that falls from 350 to 150 Hz in one second. "
	"You create the PitchTier, then add a point at 0 seconds and a frequency of 350 Hz, "
	"and a point at 1 second with a frequency of 150 Hz. "
	"You can put this PitchTier into a Manipulation object in the way described above.")
MAN_END

MAN_BEGIN (U"Intro 8.2. Manipulation of duration", U"ppgb", 20200901)
INTRO (U"You can use Praat to modify the relative durations in an existing sound.")
NORMAL (U"First, you select a @Sound object and click \"To Manipulation\". "
	"A @Manipulation object will then appear in the list. "
	"You can then click @@View & Edit@ to raise a @ManipulationEditor, "
	"which will show an empty @DurationTier. "
	"You can add targets to this tier by choosing \"Add duration point at cursor\" "
	"from the \"Dur\" menu. The targets will show up as blue dots, which you can easily drag "
	"around the duration area.")
NORMAL (U"If you click any of the rectangles "
	"(or choose any of the commands from the @Play menu), "
	"you will hear the modified sound. By shift-clicking, you will hear "
	"the original sound.")
NORMAL (U"To get the modified sound as a separate object, "
	"choose ##Publish resynthesis# from the @@File menu@.")
NORMAL (U"If you modify the pitch curve as well (see @@Intro 8.1. Manipulation of pitch@), "
	"the modified sound will be based on the modified duration and pitch.")
ENTRY (U"Precise manipulation of duration")
NORMAL (U"If you know exactly the times and relative durations, it is advisable to write a script (see @Scripting). "
	"Suppose, for instance, that you have a 355-ms piece of sound, and you want to shorten the first 85 ms to 70 ms, "
	"and the remaining 270 ms to 200 ms.")
NORMAL (U"In your first 85 ms, your relative duration should be 70/85, "
	"and during the last 270 ms, it should be 200/270. "
	"The DurationTier does linear interpolation, so it can only approximate these precise times, "
	"but fortunately to any precision you like:")
CODE (U"Create DurationTier: \"shorten\", 0, 0.085 + 0.270")
CODE (U"Add point: 0.000, 70/85")
CODE (U"Add point: 0.084999, 70/85")
CODE (U"Add point: 0.085001, 200/270")
CODE (U"Add point: 0.355, 200/270")
NORMAL (U"To put this DurationTier back into a Manipulation object, you select the two objects together "
	"(e.g. a click on the DurationTier and a @@Command-click@ on the Manipulation), "
	"and choose ##Replace duration tier#.")
MAN_END

MAN_BEGIN (U"Intro 8.3. Manipulation of intensity", U"ppgb", 20110128)
INTRO (U"You can modify the intensity contour of an existing sound.")
NORMAL (U"While the pitch and duration of a sound can be modified with the @ManipulationEditor "
	"(see @@Intro 8.1. Manipulation of pitch@ and @@Intro 8.2. Manipulation of duration@), "
	"the modification of the intensity curve is done in a different way.")
NORMAL (U"You can create an @IntensityTier with the command @@Create IntensityTier...@ "
	"from the @@New menu@. With @@View & Edit@, you can add points to this tier. "
	"You can then \"multiply\" this tier with a sound, by selecting the @Sound and the "
	"IntensityTier together and clicking @@Sound & IntensityTier: Multiply|Multiply@. "
	"The points in the IntensityTier represent relative intensities in dB; "
	"therefore, the sound pressure values in the Sound are multiplied by 10^^(dB/20)^.")
NORMAL (U"Instead of an IntensityTier, you can use an @AmplitudeTier; "
	"when you click #Multiply for a selected Sound and AmplitudeTier, "
	"the sound pressure values in the Sound are directly multiplied by the values in the AmplitudeTier.")
MAN_END

MAN_BEGIN (U"Intro 8.4. Manipulation of formants", U"ppgb", 20010408)
INTRO (U"The manipulation of formant contours cannot be as straightforward as the manipulation "
	"of pitch, duration, or intensity contours. See the @@Source-filter synthesis@ tutorial "
	"for an explanation of how formants can be modified in Praat.")
MAN_END


/*
 * Who uses Praat?
 * phonetics
 * linguistics
 * speech pathology
 * psychology
 * bioacoustics: e.g.
hapalemur, crow, monkey, frog, turtle dove, grasshopper, cricket, alligator, crocodile,
gecko, elephant, swallow, chicken, dolphin, humpback, [killer] whale, [electric] fish,
sparrow, meerkat, [fruit] bat, lion, parrot, red deer, sperm whale, [bearded] seal, mouse, cracid, chacma baboon
* music
*/

MAN_BEGIN (U"Labelling", U"ppgb", 20010408)
INTRO (U"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (U"List of Objects", U"ppgb", 20210228)
INTRO (U"A list in the left-hand part of the @@Objects window@.")
ENTRY (U"Purpose")
NORMAL (U"If you select one or more @objects in this list, "
	"the possible actions that you can perform with the selected objects "
	"will appear in the @@Dynamic menu@.")
ENTRY (U"How to select objects")
NORMAL (U"To select one object (and deselect all the others), click on the object.")
NORMAL (U"To extend the selection, drag the mouse or use Shift-click.")
NORMAL (U"To change the selection of one object (without changing the others), use @@Command-click@.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"New menu"
© Paul Boersmma 2016,2023

The ##New menu# is one of the menus in the @@Objects window@.
You use this menu to create new objects from scratch. It contains the following commands:

• @@Record mono Sound...
• @@Record stereo Sound...

, Sound:
	• @@Create Sound as pure tone...
	• @@Create Sound from formula...
	• @@Create Sound as tone complex...
	• @@Create Sound as gammatone...
	• @@Create Sound as Shepard tone...
• @@Create TextGrid...

, Tiers:
	• @@Create empty PointProcess...
	• @@Create Poisson process...
	• @@Create PitchTier...
	• @@Create FormantGrid...
	• @@Create IntensityTier...
	• @@Create DurationTier...
	• @@Create AmplitudeTier...

• @@Create Corpus...

, Tables:
	• @@Create Table with column names...
	• @@Create Table without column names...
	• ##Create TableOfReal...
	, Datasets from the literature:
		• @@Create formant table (Peterson & Barney 1952)
		///• Create formant table (Hillenbrand et al. 1995)
		• @@Create formant table (Pols & Van Nierop 1973)
		• @@Create formant table (Weenink 1985)
		• @@Create H1H2 table (Keating & Esposito 2006)
		• @@Create Table (Ganong 1980)
		• @@Create iris data set
		,
		• @@Create TableOfReal (Pols 1973)...
		• @@Create TableOfReal (Van Nierop 1973)...
		• @@Create TableOfReal (Weenink 1985)...
		• @@Create TableOfReal (Sandwell 1987)

, Stats:
	, Multidimensional scaling:
		• @@Multidimensional scaling@ tutorial
		• Create letter R example...
		• Create INDSCAL Carrol Wish example...
		• Create Configuration...
		• Draw splines...
		• Draw MDS class relations

, Generics:
	, Matrix:
		• @@Create Matrix...@ (from a formula)
		• @@Create simple Matrix...@ (from a formula)
		• @@Create simple Matrix from values...
	, Photo:
		• @@Create Photo...@ (from a formula)
		• @@Create simple Photo...@ (from a formula)
	, Polygon:
		• @@Create Polygon from values...
	, Strings:
		• @@Create Strings as file list...
		• @@Create Strings as folder list...
		• @@Create Strings from tokens...
	• @@Create Permutation...
	, Polynomial:
		• @@Create Polynomial...
		• ##Create Polynomial from product terms...
		• ##Create Polynomial from real zeros...
		• @@Create LegendreSeries...
		• @@Create ChebyshevSeries...
		• @@Create MSpline...
		• @@Create ISpline...
, Acoustic synthesis (Klatt):
	• @KlattGrid help
	• @@Create KlattGrid...
	• @@Create KlattGrid from vowel...
, Articulatory synthesis:
	• @@Articulatory synthesis@ tutorial
	• ##Create Articulation...
	• @@Create Speaker...
	• @@Create Artword...
	• ##Create VocalTract from phone...
, Text-to-speech synthesis:
	• @@Create SpeechSynthesizer...
, Constraint grammars:
	• @@OT learning@ tutorial
	• @@Create NoCoda grammar
	• ##Create place assimilation grammar
	• ##Create place assimilation distribution
	• @@Create tongue-root grammar...
	• ##Create metrics grammar...
	• ##Create multi-level metrics grammar...
, Symmetric neural networks:
	• ##Create empty Network...
	• ##Create rectangular Network...
	• ##Create rectangular Network (vertical)...
	• ##Create Net as deep belief network...
, Feedforward neural networks:
	• @@Create FFNet...
	• @@Create iris example...
	, Advanced:
		• @@Create FFNet (linear outputs)...
		• ##Create PatternList...
		• ##Create Categories...

To create new objects from files on disk, use the @@Open menu@ instead.
Objects can also often be created from other objects, with commands that start with ##To#.
################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Objects window", U"ppgb", 20230325)
INTRO (U"One of the two main windows in the Praat program.")
ENTRY (U"Subdivision")
LIST_ITEM (U"To the left: the @@List of Objects@.")
LIST_ITEM (U"To the right: the @@Dynamic menu@.")
ENTRY (U"Fixed buttons")
NORMAL (U"The following buttons appear below the List of Objects:")
LIST_ITEM (U"• @@Rename...")
LIST_ITEM (U"• @@Info")
LIST_ITEM (U"• @@Copy...")
LIST_ITEM (U"• @@Remove")
LIST_ITEM (U"• @@Inspect")
ENTRY (U"Menus")
LIST_ITEM (U"The Objects window contains several fixed menus: "
	"the #Praat, #New, #Open, and #Help menus. "
	"It also contains the @@Save menu@, whose contents vary with the kinds of selected objects, "
	"and must, therefore, be considered part of the dynamic menu.")
ENTRY (U"The Praat menu")
LIST_ITEM (U"• @@New Praat script@: creates an empty @@ScriptEditor@")
LIST_ITEM (U"• @@New Praat notebook@: creates an empty @@NotebookEditor@")
LIST_ITEM (U"• @@Open Praat script...@: creates a @@ScriptEditor@ with a script from disk")
LIST_ITEM (U"• @@Open Praat notebook...@: creates a @@NotebookEditor@ with a notebook from disk")
LIST_ITEM (U"• The ##Goodies submenu#: for doing things (like using the Calculator) "
	"that do not create new objects and do not depend on the kinds of selected objects.")
LIST_ITEM (U"• The ##Settings submenu#: for program-wide preferences, "
	"like audio input and output settings.")
LIST_ITEM (U"• ##Buttons...#: raises a @@ButtonEditor@")
LIST_ITEM (U"• (@@Add menu command...@)")
LIST_ITEM (U"• (@@Add action command...@)")
LIST_ITEM (U"• @@Quit")
ENTRY (U"Other menus")
LIST_ITEM (U"• The @@New menu@: for creating objects from scratch.")
LIST_ITEM (U"• The @@Open menu@: for reading objects from file into memory.")
LIST_ITEM (U"• The @@Save menu@: for writing objects from memory to file.")
LIST_ITEM (U"• The ##Help menu#: for viewing the manual.")
MAN_END

MAN_BEGIN (U"Periodicity menu", U"ppgb", 20240722 /*20010417, 20231115*/)
INTRO (U"A menu that occurs in the @@Dynamic menu@ for a @Sound.")
NORMAL (U"This menu contains commands for analysing the pitch contour of the selected Sound:")
LIST_ITEM (U"@@Sound: To Pitch (filtered autocorrelation)...")
LIST_ITEM (U"@@Sound: To Pitch (raw cross-correlation)...")
LIST_ITEM (U"@@Sound: To Pitch (raw autocorrelation)...")
LIST_ITEM (U"@@Sound: To Pitch (filtered cross-correlation)...")
LIST_ITEM (U"@@Sound: To Pitch (shs)...")
LIST_ITEM (U"@@Sound: To Harmonicity (cc)...")
LIST_ITEM (U"@@Sound: To Harmonicity (ac)...")
MAN_END

MAN_BEGIN (U"Pitch menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Play", U"ppgb", /*20021212*/ 20220814)
INTRO (U"A command that is available if you select a @Sound, @Pitch, or @PointProcess object. "
	"It gives you an acoustic representation of the selected object, if your loudspeakers are on "
	"and you did not \"mute\" your computer sound system.")
NORMAL (U"A Play button is also available in the @SoundRecorder window "
	"and in the @Play menu of the @SoundEditor or @TextGridEditor. In the editors, "
	"you will usually play a sound by clicking on any of the rectangles around the data.")
MAN_END

MAN_BEGIN (U"Query submenu", U"ppgb", 20221202)
INTRO (U"A submenu that appears in the @@Objects window@ if you select an object of almost any type.")
INTRO (U"Query commands give you information about objects.")
NORMAL (U"Most query commands start with the word #%Get or sometimes the word #%Count. "
	"You will find these commands in two places: under the @@Query submenu@ that usually appears if you "
	"select an @@Objects|object@ in the list, and in the query parts of several menus of the @editors "
	"(such as the @@Pitch menu@, the @@Formants menu@ or the @@Spectrogram menu@.")
ENTRY (U"Behaviour")
NORMAL (U"If you click a query command, the answer will be written to the @@Info window@.")
ENTRY (U"Scripting")
NORMAL (U"In a script, you can still use query commands to write the information to the Info window "
	"but you can also use any query command to put the information into a variable. "
	"(see @@Scripting 6.3. Query commands@). In such a case, the value will not be written into the Info window.")
ENTRY (U"Some pages that link here")
ENTRY (U"Query commands in the Praat program")
LIST_ITEM (U"@@FAQ: Pitch analysis")
LIST_ITEM (U"@@Formulas 8. Attributes of objects")
LIST_ITEM (U"@@OT learning 7. Learning from overt forms")
LIST_ITEM (U"@@Script for listing F0 statistics")
LIST_ITEM (U"@@Scripting 3.3. Numeric queries")
LIST_ITEM (U"@@Scripting 3.5. String queries")
LIST_ITEM (U"@@Scripting 6.2. Writing to the Info window")
LIST_ITEM (U"@@time domain")
LIST_ITEM (U"@@Voice 6. Automating voice analysis with a script")
NORMAL (U"The Praat program contains at least the following query commands:")
MAN_END

MAN_BEGIN (U"Quit", U"ppgb", 20050822)
INTRO (U"One of the commands in the @@Praat menu@.")
ENTRY (U"Purpose")
NORMAL (U"To leave the program.")
ENTRY (U"Behaviour")
NORMAL (U"All @objects not written to a file will be lost. "
	"However, file-based objects (like large lexica) will be saved correctly.")
ENTRY (U"Usage")
NORMAL (U"To save your data to a disk file before quitting, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (U"Read from file...", U"ppgb", 20111018)
INTRO (U"One of the commands in the @@Open menu@.")
ENTRY (U"Purpose")
NORMAL (U"To read one or more @objects from a file on disk.")
ENTRY (U"Behaviour")
NORMAL (U"Many kinds of files are recognized:")
LIST_ITEM (U"1. Text files that are structured as described under @@Save as text file...@; "
	"these can contain an object of any type, or a collection of objects.")
LIST_ITEM (U"2. Files that were produced by @@Save as binary file...@ (any type).")
LIST_ITEM (U"3. Files in a LISP text format (only for object types that can be written to a LISP file).")
LIST_ITEM (U"4. Files that were made recognizable by the libraries built on Praat. "
	"For instance, the phonetics library adds recognizers for many kinds of sound files.")
NORMAL (U"If the file contains more than one object, these objects will appear in the list, "
	"and their names will be the same as the names that they had "
	"when they were saved with ##Save as text file...# or ##Save as binary file...#.")
ENTRY (U"Examples")
LIST_ITEM (U"• If the file contains only one Pitch object and is called \"hallo.pit\", "
	"an object with the name \"Pitch hallo\" will appear in the list of objects. "
	"You may have more objects with the same name.")
LIST_ITEM (U"• If the file contains one object of type Pitch, named \"hallo\", "
	"and one object of type Polygon, named \"kromme\", "
	"there will appear two objects in the list, "
	"called \"Pitch hallo\" and \"Polygon kromme\".")
MAN_END

MAN_BEGIN (U"Open menu", U"ppgb", 20110111)
INTRO (U"One of the menus in the @@Objects window@.")
NORMAL (U"With the Open menu, you read one or more @objects from a file on disk into memory. "
	"The resulting object(s) will appear in the @@List of Objects@.")
NORMAL (U"The Open menu contains the command @@Read from file...@, which recognizes most file types, "
	"and perhaps several other commands for reading unrecognizable file types (e.g., raw sound data), "
	"or for interpreting known file types in a different way "
	"(e.g., reading two mono sounds from one stereo sound file):")
MAN_END

MAN_BEGIN (U"pause window", U"ppgb", 20230723)
INTRO (U"A window, popped up by a script, that asks the user for input.")
NORMAL (U"For details, see @@Scripting 6.6. Controlling the user@.")
MAN_END

MAN_BEGIN (U"Remove", U"ppgb", 20021212)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
NORMAL (U"You can choose this command after selecting one or more @objects in the list.")
NORMAL (U"The selected objects will permanently disappear from the list, "
	"and the computer memory that they occupied will be freed.")
NORMAL (U"To save your data before removing, choose a command from the @@Save menu@.")
MAN_END

MAN_BEGIN (U"Rename...", U"ppgb", 20111018)
INTRO (U"One of the fixed buttons in the @@Objects window@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one object of any type.")
ENTRY (U"Purpose")
NORMAL (U"You can give the selected object a new name.")
ENTRY (U"Behaviour")
NORMAL (U"If you type special symbols or spaces, the Objects window will replace them with underscores.")
MAN_END

MAN_BEGIN (U"Save menu", U"ppgb", 20211015)
INTRO (U"One of the menus in the @@Objects window@.")
ENTRY (U"Purpose")
NORMAL (U"With the #Save menu, you write one or more selected @objects from memory to a file on disk. "
	"The data can be read in again with one of the commands in the @@Open menu@ "
	"(most often simply with @@Read from file...@).")
ENTRY (U"Usage: save your work")
NORMAL (U"You will often choose a command from this menu just before clicking the @Remove button "
	"or choosing the @Quit command.")
ENTRY (U"Fixed commands")
NORMAL (U"If no object is selected, the #Save menu is empty. "
	"If any object is selected, it will at least contain the following commands:")
LIST_ITEM (U"• @@Save as text file...")
LIST_ITEM (U"• @@Save as short text file...")
LIST_ITEM (U"• @@Save as binary file...")
ENTRY (U"Dynamic commands")
NORMAL (U"Depending on the type of the selected object, the following commands may be available "
	"in the #Save menu:")
MAN_END

MAN_BEGIN (U"Save as binary file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Objects window will ask you for a file name. "
	"After you click OK, the objects will be written to a binary file on disk.")
ENTRY (U"Usage")
NORMAL (U"The file can be read again with @@Read from file...@.")
ENTRY (U"File format")
NORMAL (U"These files are in a device-independent binary format, "
	"and can be written and read on any machine.")
MAN_END

MAN_BEGIN (U"Save as short text file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Objects window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (U"File format")
NORMAL (U"The format is much shorter than the one described at @@Save as text file...@. "
	"Most of the comments are gone, and there is normally one piece of data per line.")
NORMAL (U"The file can be read again with the all-purpose @@Read from file...@.")
MAN_END

MAN_BEGIN (U"Save as text file...", U"ppgb", 20110129)
INTRO (U"One of the commands in the @@Save menu@.")
ENTRY (U"Availability")
NORMAL (U"You can choose this command after selecting one or more @objects.")
ENTRY (U"Behaviour")
NORMAL (U"The Objects window will ask you for a file name. "
	"After you click OK, the objects will be written to a text file on disk.")
ENTRY (U"File format")
NORMAL (U"If you selected a single object, e.g., of type Pitch, "
	"the file will start with the lines:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Pitch\"")
NORMAL (U"After this, the pitch data will follow.")
LIST_ITEM (U"If you selected more than one object, e.g., “Pitch hallo” and “Polygon kromme”, "
	"the file will look like:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Collection\"")
CODE (U"size = 2")
CODE (U"item []:")
CODE (U"    item [1]:")
CODE (U"        class = \"Pitch\"")
CODE (U"        name = \"hallo\"")
CODE (U"        (pitch data...)")
CODE (U"    item [2]:")
CODE (U"        class = \"Polygon\"")
CODE (U"        name = \"kromme\"")
CODE (U"        (polygon data...)")
NORMAL (U"The file can be read again with @@Read from file...@, "
	"which, by the way, does not need the verbosity of the above example. "
	"The following minimal format will also be read correctly:")
CODE (U"\"ooTextFile\"")
CODE (U"\"Collection\"  2")
CODE (U"\"Pitch\"  \"hallo\"  (pitch data...)")
CODE (U"\"Polygon\"  \"kromme\"  (polygon data...)")
NORMAL (U"Thus, all text that is not a free-standing number and is not enclosed in double quotes or < >, "
	"is considered a comment, as is all text following an exclamation mark (“!”) on the same line.")
MAN_END

MAN_BEGIN (U"Segmentation", U"ppgb", 20010408)
INTRO (U"See @@Intro 7. Annotation@.")
MAN_END

MAN_BEGIN (U"Show formant", U"ppgb", 20030316)
INTRO (U"One of the commands in the Formant menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 5. Formant analysis@.")
MAN_END

MAN_BEGIN (U"Show intensity", U"ppgb", 20030316)
INTRO (U"One of the commands in the Intensity menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 6. Intensity analysis@.")
MAN_END

MAN_BEGIN (U"Show pitch", U"ppgb", 20030316)
INTRO (U"One of the commands in the Pitch menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 4. Pitch analysis@.")
MAN_END

MAN_BEGIN (U"Show pulses", U"ppgb", 20030316)
INTRO (U"One of the commands in the Pulses menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @Voice.")
MAN_END

MAN_BEGIN (U"Show spectrogram", U"ppgb", 20030316)
INTRO (U"One of the commands in the Spectrogram menu of the @SoundEditor and the @TextGridEditor.")
NORMAL (U"See @@Intro 3. Spectral analysis@.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Source-filter synthesis"
© Paul Boersma 2005-07-13

This tutorial describes how you can do acoustic synthesis with Praat.
It assumes that you are familiar with the @Intro.

1. The source-filter theory of speech production
======================================

The source-filter theory (@@Fant (1960)|Fant 1960@) hypothesizes that an acoustic speech signal can be seen
as a %source signal (the glottal source, or noise generated at a constriction in the vocal tract),
%filtered with the resonances in the cavities of the vocal tract downstream from the glottis
or the constriction. The %%Klatt synthesizer% (@@Klatt & Klatt (1990)|Klatt & Klatt 1990@), for instance,
is based on this idea.

In the Praat program, you can create a %source signal from scratch of from an existing
speech signal, and you can create a %filter from scratch or extract it from an existing speech signal.
You can manipulate (change, adapt) both the source and the filter before doing
the actual synthesis, which combines the two.

,	@@Source-filter synthesis 1. Creating a source from pitch targets
,	@@Source-filter synthesis 2. Filtering a source
,	@@Source-filter synthesis 3. The ba-da continuum
,	@@Source-filter synthesis 4. Using existing sounds

################################################################################
"Source-filter synthesis 1. Creating a source from pitch targets"
© Paul Boersma 2014-04-21

Creating a glottal source signal for speech synthesis involves creating a @PointProcess,
which is a series of time points that should represent the exact moments of glottal closure.

You may want to start with creating a well-defined pitch contour.
Suppose you want to create a sound with a duration of half a second with a pitch that falls from 300 to 200 Hz
during that time. You first create an empty @PitchTier by choosing @@Create PitchTier...@ from the #New menu
(I call this PitchTier “empty” because it does not contain any pitch information yet);
you may want to name the PitchTier “source” and have it start at 0 seconds and end at 0.5 seconds.
Once the PitchTier exists and is selected, you can ##View & Edit# it to add pitch points (pitch targets) to it at certain times
(or you choose @@PitchTier: Add point...@ from the #Modify menu repeatedly).
You could add a pitch point of 150 Hz at time 0.0 and a pitch point of 100 Hz at time 0.5.
In the PitchTier window, you can see that the pitch curve falls linearly
from 150 to 100 Hz during its time domain:
{- 4.5x2.5
	Create PitchTier: “source”, 0, 0.5
	Add point: 0.0, 150
	Add point: 0.5, 100
	Draw: 0, 0, 0, 200, “yes”, “lines and speckles”
	Text top: “no”, ~%%An F0 linearly falling between two points
	One mark left: 100, “yes”, “yes”, “no”, “”
	Remove
}

You can hear the falling pitch by clicking on the rectangles in the PitchTier window
(or by clicking ##Play pulses#, #Hum, or ##Play sine# in the Objects window).

From this PitchTier, you can create a @PointProcess with @@PitchTier: To PointProcess@.
The resulting PointProcess now represents a series of glottal pulses. To make some parts of this
point process voiceless, you can use @@PointProcess: Remove points between...@.
It is advisable to make the very beginning and end of this point process voiceless, so that the filtered sound
will not start or end abruptly. In the following example, the first and last 20 ms are devoiced,
and a stretch of 70 ms in the middle
is made voiceless as well, perhaps because you want to simulate a voiceless plosive there:
{- 4.5x2.5
	pitchTier = Create PitchTier: “source”, 0, 0.5
	Add point: 0.0, 150
	Add point: 0.5, 100
	pulses = To PointProcess
	Remove points between: 0, 0.02
	Remove points between: 0.24, 0.31
	Remove points between: 0.48, 0.5
	Draw: 0, 0, “yes”
	Text top: “no”, ~%%The times of the glottal pulses
	One mark bottom: 0.24, “yes”, “yes”, “no”, “”
	One mark bottom: 0.31, “yes”, “yes”, “no”, “”
	removeObject: pitchTier, pulses
}

Now that we have a glottal point process (a glottal pulse train), the only thing left
is to turn it into a sound by choosing @@PointProcess: To Sound (phonation)...@.
If you use the standard settings of this command (but with ##Adaptation factor# set to 0.6), the result will be a @Sound with
reasonable glottal flow derivatives centred around each of the original pulses in the point process.
You can check this by selecting the Sound and choosing ##View & Edit#:
{- 4.5x2.5
	pitchTier = Create PitchTier: “source”, 0, 0.5
	Add point: 0.0, 150
	Add point: 0.5, 100
	pulses = To PointProcess
	Remove points between: 0, 0.02
	Remove points between: 0.24, 0.31
	Remove points between: 0.48, 0.5
	source = To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0
	Draw: 0, 0, 0, 0, “yes”, “curve”
	Text top: “no”, ~%%The source waveform
	One mark bottom: 0.24, “yes”, “yes”, “no”, “”
	One mark bottom: 0.31, “yes”, “yes”, “no”, “”
	removeObject: pitchTier, pulses, source
}

You will also see that the amplitude of the first two glottal wave shapes of every voiced stretch
is (realistically) somewhat smaller than the amplitudes of the following wave shapes;
This is the result of setting ##Adaptation factor# to 0.6.

What you have now is what we call a %%glottal source signal%. It does two things: it contains information on the glottal flow,
and it already takes into account one aspect of the %filter, namely the radiation at the lips.
This combination is standard procedure in acoustic synthesis.

The glottal source signal sounds as a voice without a vocal tract.
The following section describes how you add vocal-tract resonances, i.e. the %filter.

Automation
==========

In a clean Praat script, the procedure described above will look as follows:
{
	pitchTier = Create PitchTier: “source”, 0, 0.5
	Add point: 0.0, 150
	Add point: 0.5, 100
	pulses = To PointProcess
	Remove points between: 0, 0.02
	Remove points between: 0.24, 0.31
	Remove points between: 0.48, 0.5
	source = To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0
	removeObject: pitchTier, pulses
	selectObject: source
}

################################################################################
"Source-filter synthesis 2. Filtering a source"
© Paul Boersma 2014-04-21

Once you have a glottal source signal, you are ready to create a filter that represents
the resonances of the vocal tract, as a function of time. In other words, you create a @FormantGrid object.

For a vowel spoken by an average (i.e. adult female) human voice, tradition assumes five formants in the range
between 0 and 5500 hertz. This number comes from a computation of the formants of a
straight tube, which has resonances at wavelengths of four tube lengths, four thirds of a tube length,
four fifths, and so on. For a straight tube 16 centimetres long, the shortest wavelength is 64 cm,
which, with a sound velocity of 352 m/s, means a resonance frequency of 352/0.64 = 550 hertz.
The other resonances will be at 1650, 2750, 3850, and 4950 hertz.

You can create a @FormantGrid object with @@Create FormantGrid...@ from the #New menu (submenu #Tiers):
{
	Create FormantGrid: “filter”, 0, 0.5, 10, 550, 1100, 60, 50
}
This creates a FormantGrid with 10 formants and a single frequency value for each formant: %F_1 is 550 Hz
and the higher formants are spaced 1100 Hz apart, i.e., they are
1650, 2750, 3850, 4950, 6050, 7150, 8250, 9350, and 10450 hertz;
the ten bandwidths start at 60 Hz and have a spacing of 50 Hz,
i.e., they are 60, 110, 160, 210, 260, 310, 360, 410, 460, and 510 hertz.

You can then create formant contours with @@FormantGrid: Add formant point...@:
{
	Remove formant points between: 1, 0, 0.5
	Add formant point: 1, 0.00, 100
	Add formant point: 1, 0.05, 700
	Remove formant points between: 2, 0, 0.5
	Add formant point: 2, 0.00, 500
	Add formant point: 2, 0.05, 1100
}
This example creates a spectral specification whose %F_1 rises from 100 to 700 hertz during the
first 50 milliseconds (as for any obstruent), and whose %F_2 rises from 500 to 1100 hertz.
The other eight formants keep their original values, as do the ten bandwidths.
The resulting sound may be a [ba]-like formant transition.

To get the final acoustic result (the sound), you select the glottal source signal
together with the FormantGrid and choose @@Sound & FormantGrid: Filter@.

The resulting sound will have a fairly straight intensity contour. You can change this with
the #Formula command (@@Sound: Formula...@), or by multiplying the source signal or the
acoustic result with an @Intensity or @IntensityTier object.

################################################################################
"Source-filter synthesis 3. The ba-da continuum"
© Paul Boersma 2014-04-21,2025

As an example, we are going to create a male [ba]-[da] continuum in six steps. The acoustic difference
between [ba] and [da] is the initial %F_2, which is 500 Hz for [ba], and 2500 Hz for [da].

We use the same @PitchTier throughout, to model a falling intonation contour:
{
	Create PitchTier: “f0”, 0.00, 0.50
	Add point: 0.00, 150
	Add point: 0.50, 100
}

The first and last 50 milliseconds are voiceless:
{
	To PointProcess
	Remove points between: 0.00, 0.05
	Remove points between: 0.45, 0.50
}

Generate the glottal source signal:
{
	To Sound (phonation): 44100, 0.6, 0.05, 0.7, 0.03, 3.0, 4.0
}

During the labial or coronal closure, the sound is almost silent, so we use an @IntensityTier
that models this:
{
	Create IntensityTier: “intens”, 0.00, 0.50
	Add point: 0.05, 60
	Add point: 0.10, 80
}

Generate the source signal:
{
	plusObject: “Sound f0”
	Multiply
	Rename: “source”
}

The ten sounds are generated in a loop:
{
	for i from 1 to 10
		f2_locus = 500 + (2500/9) * (i - 1) ; variable names start with lower case!
		Create FormantGrid: “filter”, 0.0, 0.5, 9, 800, 1000, 60, 80
		Add formant point: 1, 0.05, 100
		Add bandwidth point: 1, 0.05, 50
		Add formant point: 1, 0.10, 800
		Add bandwidth point: 1, 0.10, 60
		Add formant point: 2, 0.05, f2_locus
		Add bandwidth point: 2, 0.05, 100
		Add formant point: 2, 0.05, 1800
		Add bandwidth point: 2, 0.05, 140
		plusObject: “Sound source”
		Filter (no scale)
		Rename: “bada” + string$ (i)
		removeObject: “FormantGrid filter”
	endfor
}

Clean up:
{
	removeObject: “Sound source”, “Sound f0”, “IntensityTier intens”,
	... “PointProcess f0”, “PitchTier f0”
}

In this example, filtering was done without automatic scaling, so that
the resulting signals have equal intensities in the areas where they have
equal formants. You will probably want to multiply all these signals with
the same value in order to bring their amplitudes in a suitable range
between -1 and +1 Pascal.

################################################################################
"Source-filter synthesis 4. Using existing sounds"
© Paul Boersma 2017-08-28

1. How to extract the %filter from an existing speech sound
======================================

You can separate source and filter with the help of the technique of %%linear prediction%
(see @@Sound: LPC analysis@). This technique tries to approximate a given frequency spectrum with
a small number of peaks, for which it finds the mid frequencies and the bandwidths.
If we do this for an overlapping sequence of windowed parts of a sound signal
(i.e. a %%short-term analysis%), we get a quasi-stationary approximation of the signal's
spectral characteristics as a function of time, i.e. a smoothed version of the @Spectrogram.

For a speech signal, the peaks are identified with the resonances (%formants) of the vocal tract.
Since the spectrum of a vowel spoken by an average human being falls off with approximately
6 dB per octave, %%pre-emphasis% is applied to the signal before the linear-prediction analysis,
so that the algorithm will not try to match only the lower parts of the spectrum.

For an adult female human voice, tradition assumes five formants in the range
between 0 and 5500 hertz, say at 550, 1650, 2750, 3850, and 4950 hertz. For the linear prediction in
Praat, you will have to implement this 5500-Hz band-limiting by resampling the original
speech signal to 11 kHz. For a male voice, you would use 10 kHz; for a young child, 20 kHz.

To perform the resampling, you use @@Sound: Resample...@:
you select a @Sound object, and click ##Resample...#.
In the rest of this tutorial, I will use the syntax that you would use in a script,
though you will usually do these things by clicking on objects and buttons. Thus:
{;
	selectObject: “Sound hallo”
	Resample: 11000, 50
}
You can then perform a linear-prediction analysis on the resampled sound
with @@Sound: To LPC (burg)...@:
{;
	selectObject: “Sound hallo_11000”
	To LPC (burg): 10, 0.025, 0.005, 50
}
This says that your analysis is done with 10 linear-prediction parameters
(which will yield at most five formant-bandwidth pairs), with an analysis window
effectively 25 milliseconds long, with time steps of 5 milliseconds (so that the windows
will appreciably overlap), and with a pre-emphasis frequency of 50 Hz (which is the point
above which the sound will be amplified by 6 dB/octave prior to the analysis proper).

As a result, an object called “LPC hallo” will appear in the list of objects.
This @LPC object is a time function with 10 %%linear-prediction coefficients% in each %%time frame%.
These coefficients are rather opaque even to the expert (try to view them with @Inspect),
but they are the raw material from which formant and bandwidth values can be computed.
To see the smoothed @Spectrogram associated with the LPC object, choose @@LPC: To Spectrogram...@:
{;
	selectObject: “LPC hallo_11000”
	To Spectrogram: 20, 0, 50
	Paint: 0, 0, 0, 0, 50, 0, 0, “yes”
}
Note that when drawing this Spectrogram, you will want to set the pre-emphasis to zero
(the fifth 0 in the last line), because pre-emphasis has already been applied in the analysis.

You can get and draw the formant-bandwidth pairs from the LPC object,
with @@LPC: To Formant@ and @@Formant: Speckle...@:
{;
	selectObject: “LPC hallo_11000”
	To Formant
	Speckle: 0, 0, 5500, 30, “yes”
}
Note that in converting the @LPC into a @Formant object, you may have lost some
information about spectral peaks at very low frequencies (below 50 Hz) or at very high
frequencies (near the @@Nyquist frequency@ of 5500 Hz. Such peaks usually try to fit
an overall spectral slope (if the 6 dB/octave model is inappropriate), and are not seen
as related with resonances in the vocal tract, so they are ignored in a formant analysis.
For resynthesis purposes, they might still be important.

Instead of using the intermediate LPC object, you could have done a formant analysis
directly on the original Sound, with @@Sound: To Formant (burg)...@:
{;
	selectObject: “Sound hallo”
	To Formant (burg): 0.005, 5, 5500, 0.025, 50
}
A @Formant object has a fixed sampling (time step, frame length), and for every
%%formant frame%, it contains a number of formant-bandwidth pairs.

From a Formant object, you can create a @FormantGrid with @@Formant: Down to FormantGrid@.
A FormantGrid object contains a number of tiers with time-stamped %%formant points% and %%bandwidth points%.

Any of these three types (@LPC, @Formant, and @FormantGrid) can represent the %filter
in source-filter synthesis.

2. How to extract the %source from an existing speech sound
======================================

If you are only interested in the %filter characteristics, you can get by with @Formant objects.
To get at the %source signal, however, you need the raw @LPC object:
you select it together with the resampled @Sound, and apply %%inverse filtering%:
{;
	selectObject: “Sound hallo_11000”, “LPC hallo_11000”
	Filter (inverse)
}
A new Sound named `hallo_11000` will appear in the list of objects
(you could rename it to `source`).
This is the estimated source signal. Since the LPC analysis was designed to yield a spectrally
flat filter (through the use of pre-emphasis), this source signal represents everything in the
speech signal that cannot be attributed to the resonating cavities. Thus, the “source signal”
will consist of the glottal volume-velocity source (with an expected spectral slope of
-12 dB/octave for vowels) and the radiation characteristics at the lips, which cause a
6 dB/octave spectral rise, so that the resulting spectrum of the “source signal”
is actually the %derivative of the glottal flow, with an expected spectral slope of -6 dB/octave.

Note that with inverse filtering you cannot measure the actual spectral slope of the source signal.
Even if the actual slope is very different from -6 dB/octave, formant extraction will try to
match the pre-emphasized spectrum. Thus, by choosing a pre-emhasis of -6 dB/octave,
you %impose a slope of -6 dB/octave on the source signal.

3. How to do the synthesis
==========================

You can create a new Sound from a source Sound and a filter, in at least four ways.

If your filter is an @LPC object, you select it and the source, and choose @@LPC & Sound: Filter...@:
{;
	selectObject: “Sound source”, “LPC filter”
	Filter: “no”
}
If you had computed the source and filter from an LPC analysis, this procedure should give
you back the original Sound, except that windowing has caused 25 milliseconds at the beginning
and end of the signal to be set to zero.

If your filter is a @Formant object, you select it and the source, and choose @@Sound & Formant: Filter@:
{;
	selectObject: “Sound source”, “Formant filter”
	Filter
}
If you had computed the source and filter from an LPC analysis, this procedure will not generally give
you back the original Sound, because some linear-prediction coefficients will have been ignored
in the conversion to formant-bandwidth pairs.

If your filter is a @FormantGrid object, you select it and the source, and choose @@Sound & FormantGrid: Filter@:
{;
	selectObject: “Sound source”, “FormantGrid filter”
	Filter
}
Finally, you could just know the %%impulse response% of your filter (in a @Sound object).
You then select both Sound objects, and choose @@Sounds: Convolve...@:
{;
	selectObject: “Sound source”, “Sound filter”
	Convolve: “integral”, “zero”
}
As a last step, you may want to bring the resulting sound within the [-1; +1] range:
{;
	Scale peak: 0.99
}

4. How to manipulate the filter
===============================

You can hardly change the values in an @LPC object in a meaningful way:
you would have to manually change its rather opaque data with the help of @Inspect.

A @Formant object can be changed in a friendlier way, with @@Formant: Formula (frequencies)...@
and @@Formant: Formula (bandwidths)...@. For instance, to multiply all formant frequencies by 0.9,
you do
{;
	selectObject: “Formant filter”
	Formula (frequencies): “self * 0.9”
}
To add 200 hertz to all values of %F_2, you do
{;
	Formula (frequencies): ~ if row = 2 then self + 200 else self fi
}
A @FormantGrid object can be changed by adding or removing points:
,	@@FormantGrid: Add formant point...@
,	@@FormantGrid: Add bandwidth point...@
,	@@FormantGrid: Remove formant points between...@
,	@@FormantGrid: Remove bandwidth points between...@

5. How to manipulate the source signal
======================================

You can manipulate the source signal in the same way you that would manipulate any sound,
for instance with the @ManipulationEditor.

)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Spectrogram menu", U"ppgb", 20221202)
INTRO (U"A menu in the @SoundEditor or @TextGridEditor.")
MAN_END

MAN_BEGIN (U"Spectrogram settings...", U"ppgb", 20221202)
INTRO (U"A command in the @@Spectrogram menu@ of the @SoundEditor and @TextGridEditor windows. "
	"See @@Intro 3.2. Configuring the spectrogram@.")
MAN_END

MAN_BEGIN (U"Technical", U"ppgb", 20120915)
INTRO (U"The title of a submenu of the @@Praat menu@.")
MAN_END

MAN_BEGIN (U"Types of objects", U"ppgb", 20190928)
INTRO (U"Praat contains the following types of objects and @Editors. "
	"For an introduction and tutorials, see @Intro.")
NORMAL (U"General purpose:")
LIST_ITEM (U"• @Matrix: a sampled real-valued function of two variables")
LIST_ITEM (U"• @Polygon")
LIST_ITEM (U"• @PointProcess: a point process (@PointEditor)")
LIST_ITEM (U"• @Sound: a sampled continuous process (@SoundEditor, @SoundRecorder, @@Sound files@)")
LIST_ITEM (U"• @LongSound: a file-based version of a sound (@LongSoundEditor)")
LIST_ITEM (U"• @Strings")
LIST_ITEM (U"• @Distributions, @PairDistribution")
LIST_ITEM (U"• @Table, @TableOfReal")
LIST_ITEM (U"• @Permutation")
LIST_ITEM (U"• @ParamCurve")
NORMAL (U"Periodicity analysis:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 4. Pitch analysis")
LIST_ITEM1 (U"• @@Intro 6. Intensity analysis")
LIST_ITEM1 (U"• @Voice (jitter, shimmer, noise)")
LIST_ITEM (U"• @Pitch: articulatory fundamental frequency, acoustic periodicity, or perceptual pitch (@PitchEditor)")
LIST_ITEM (U"• @Harmonicity: degree of periodicity")
LIST_ITEM (U"• @Intensity, @IntensityTier: intensity contour")
LIST_ITEM (U"• @Electroglottogram")
NORMAL (U"Spectral analysis:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 3. Spectral analysis")
LIST_ITEM1 (U"• @@Intro 5. Formant analysis")
LIST_ITEM (U"• @Spectrum: complex-valued equally spaced frequency spectrum (@SpectrumEditor)")
LIST_ITEM (U"• @Ltas: long-term average spectrum")
LIST_ITEM (U"• Spectro-temporal: @Spectrogram, @BarkSpectrogram, @MelSpectrogram")
LIST_ITEM (U"• @Formant: acoustic formant contours")
LIST_ITEM (U"• @LPC: coefficients of Linear Predictive Coding, as a function of time")
LIST_ITEM (U"• @Cepstrum, @CC, @LFCC, @MFCC (cepstral coefficients)")
LIST_ITEM (U"• @Excitation: excitation pattern of basilar membrane")
LIST_ITEM (U"• @Excitations: an ensemble of #Excitation objects")
LIST_ITEM (U"• @Cochleagram: excitation pattern as a function of time")
NORMAL (U"Labelling and segmentation (see @@Intro 7. Annotation@):")
LIST_ITEM (U"• @TextGrid (@TextGridEditor)")
NORMAL (U"Listening experiments:")
LIST_ITEM (U"• @ExperimentMFC")
NORMAL (U"Manipulation of sound:")
LIST_ITEM (U"• Tutorials:")
LIST_ITEM1 (U"• @@Intro 8.1. Manipulation of pitch")
LIST_ITEM1 (U"• @@Intro 8.2. Manipulation of duration")
LIST_ITEM1 (U"• @@Intro 8.3. Manipulation of intensity")
LIST_ITEM1 (U"• @@Filtering")
LIST_ITEM1 (U"• @@Source-filter synthesis")
LIST_ITEM (U"• @PitchTier (@PitchTierEditor)")
LIST_ITEM (U"• @Manipulation (@ManipulationEditor): @@overlap-add@")
LIST_ITEM (U"• @DurationTier")
LIST_ITEM (U"• @FormantGrid")
NORMAL (U"Articulatory synthesis (see the @@Articulatory synthesis@ tutorial):")
LIST_ITEM (U"• @Speaker: speaker characteristics of a woman, a man, or a child")
LIST_ITEM (U"• #Articulation: snapshot of articulatory specifications (muscle activities)")
LIST_ITEM (U"• @Artword: articulatory target specifications as functions of time")
LIST_ITEM (U"• (@VocalTract: area function)")
NORMAL (U"Neural net package:")
LIST_ITEM (U"• @FFNet: feed-forward neural net")
LIST_ITEM (U"• @PatternList")
LIST_ITEM (U"• @Categories: for classification (#CategoriesEditor)")
NORMAL (U"Numerical and statistical analysis:")
LIST_ITEM (U"• @Eigen: eigenvectors and eigenvalues")
LIST_ITEM (U"• @Polynomial, @Roots, @ChebyshevSeries, @LegendreSeries, @ISpline, @MSpline")
LIST_ITEM (U"• @Covariance: covariance matrix")
LIST_ITEM (U"• @Confusion: confusion matrix")
LIST_ITEM (U"• @@Discriminant analysis@: @Discriminant")
LIST_ITEM (U"• @@Principal component analysis@: @PCA")
LIST_ITEM (U"• @Correlation, @ClassificationTable, @SSCP")
LIST_ITEM (U"• @DTW: dynamic time warping")
NORMAL (U"@@Multidimensional scaling@:")
LIST_ITEM (U"• @Configuration (@Salience)")
LIST_ITEM (U"• @@Kruskal analysis@: @Dissimilarity (@Weight), @Similarity")
LIST_ITEM (U"• @@INDSCAL analysis@: @Distance, @ScalarProduct")
LIST_ITEM (U"• @@Correspondence analysis@: @ContingencyTable")
NORMAL (U"Optimality-theoretic learning (see the @@OT learning@ tutorial)")
LIST_ITEM (U"• @OTGrammar (@OTGrammarEditor)")
NORMAL (U"Bureaucracy")
LIST_ITEM (U"• @WordList, @SpellingChecker")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Unicode"
© Paul Boersma 2023

Praat aims at being a fully international program:
the texts in Praat’s TextGrids, Tables, scripts, or Info window (and elsewhere) can contain many types of characters
(see @@special symbols@).
For this reason, Praat saves its text files in one of two possible formats: UTF-8 (including ASCII) or UTF-16.

ASCII text files
================

If your TextGrid (or Table, or script, or Info window...) contains only characters that can be encoded as ASCII,
namely the characters `!\"#$%&'()*+,-./0123456789:;<=>?@
ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`` abcdefghijklmnopqrstuvwxyz{|}~`,
then when you say @@Save as text file...@ or #Save, Praat will write an ASCII text file,
which is a text file in which every character is encoded in a single byte (8 bits).
All programs that can read plain text files can read such files produced by Praat.

UTF-16 text files
=================

If your TextGrid (or Table, or script, or Info window...) contains one or more characters that cannot be encoded as ASCII,
for instance West-European characters such as \ao\c,\e'\o"\ss\?d, East-European characters such as \c<\l/\o:\s<\uo\z',
or Hebrew characters such as \?+\B+\G+\M%\vO\sU,
then when you say @@Save as text file...@ or #Save, Praat will write an UTF-16 text file,
which is a text file in which every character is encoded in two bytes (and some very rare characters in four bytes).
Many programs can read such text files, for instance NotePad, WordPad, Microsoft Word, and TextWrangler.

What if my other programs cannot read UTF-16 text files?
========================================================

If you want to export your Table to Microsoft Excel or to SPSS, or if you want your TextGrid file to be read
by somebody else's Perl script, then there will be no problem if your Table contains only ASCII characters (see above).
But if your Table contains any other (i.e. non-ASCII) characters, you may be in trouble, because Praat will write the Table
as an UTF-16 text file, and not all of the programs just mentioned can read such files yet.

What you can do is go to ##Text writing settings...# in the #Settings submenu of the #Praat menu,
and there set the output encoding to ##UTF-8#. Praat will from then on save your text files in the UTF-8 format,
which means one byte for every ASCII character and 2 to 4 bytes for every non-ASCII character.
Especially on Linux, many programs understand UTF-8 text and will display the correct characters.
Programs such as SPSS do not understand UTF-8 but will still display ASCII characters correctly;
for instance, the names München and Wałęsa may appear as M\A~\:-nchen and Wa\Ao,\A"\TMsa or so.

If you can get by with West-European characters (on Windows),
then you may choose ##try ISO Latin-1, then UTF-16# for the output encoding.
It is possible (but not guaranteed) that programs like SPSS then display your West-European text correctly.
This trick is of limited use, because it will not work if your operating system is set to a “codepage”
differently from ISO Latin-1 (or “ANSI”), or if you need East-European or Hebrew characters, or if you want
to share your text files with Macintosh users.

If you already have some UTF-16 text files and you want to convert them to UTF-8 or ISO Latin-1 (the latter only if
they do not contain non-West-European characters), then you can read them into Praat and save them again
(with the appropriate output encoding setting).
Other programs, such a NotePad and TextWrangler, can also do this conversion.

Finally, it is still possible to make sure that all texts are ASCII, e.g. you type the characters ß and ő
as “`\ss`” and “`\o:`” respectively. See @@special symbols@.

################################################################################
"Play"
© Paul Boersma 2022

One of the menus in several @editors and in the @manual.
################################################################################
)~~~"
MAN_PAGES_END

}

/* End of file manual_tutorials.cpp */
