/* manual_SpeechRecognizer.cpp
 *
 * Copyright (C) 2026 Anastasia Shchupak
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

void manual_SpeechRecognizer_init (ManPages me);
void manual_SpeechRecognizer_init (ManPages me) {

MAN_PAGES_BEGIN R"~~~(
################################################################################
"Speech recognition"
© Anastasia Shchupak 2026-06-01

There are two speech recognition tools available in Praat: automatic transcription (turning speech
into text) and automatic speaker diarization (finding out who speaks when). This tutorial
describes how you can use these tools in Praat. It assumes that you are familiar with the @Intro,
especially with @@Intro 7. Annotation@.

Transcription is performed with @@whisper.cpp@ and it requires at least one external model
installed on your computer; speaker diarization is performed with an adapted @@pyannote.audio@
diarization pipeline and it works without any external models. Diarization can also be performed
together with transcription, so that transcribed text is divided among the detected speakers.

After you have read the chapters ##1. Automatic transcription# and ##2. Automatic speaker
diarization#, you might be interested in what can be done to improve the performance of these
tools in Praat; you can find this information in ##3. Performance#.

1. Automatic transcription
==========================
Transcription in Praat turns speech in a @Sound object into written text. Usually you might want
this text to be placed into a @TextGrid, aligning it with the Sound (this is described in ##1.2.
Transcribing into a TextGrid#). But you can also obtain it as plain text (see ##1.3. Transcribing
into plain text#). Either way, you first need a Whisper model installed, and how to do that is
explained in ##1.1. Installing Whisper models#.

1.1. Installing Whisper models
==============================
To transcribe, you need at least one Whisper model installed. In this case, installation
means downloading a model and placing it into a specific folder on your computer, where Praat
can later find it. In your Praat @@preferences folder@, create a folder called `models`, and
inside it another one called `whispercpp`. If you are on Windows, you will now have a folder called
something like `C:\Users\Your Name\Praat\models\whispercpp`.

Models can be downloaded from `https://huggingface.co/ggerganov/whisper.cpp`. This
page contains Whisper models in ggml format (files with extension `.bin`), which is what
whisper.cpp uses. Note that the original Whisper models, by OpenAI, are distributed in PyTorch format
(files with extension `.pt`) and cannot be used in Praat directly: they first need to be
converted to ggml format. The files on the page above are results of such conversions.

The list of available models is quite long, and you might wonder how to choose one. You will
probably want to experiment with different models to find a good balance between speed and
accuracy for your specific task, but below you can read a brief overview which can help you get
started.

A model is English-only if its name contains `.en` (e.g. `ggml-small.en.bin`); all other models are
multilingual. Size-wise, the models range from `tiny` (about 75 MB) through `base`, `small` and
`medium` to `large` (about 2.9 GB). The large models have three different versions: `large-v1`,
`large-v2` and `large-v3`. The later versions are improvements over the earlier ones. Generally
speaking, larger models tend to be more accurate but use more disk space and memory, and take
longer to transcribe. So, `base` or `small` can be good starting points.

Models whose names end in `-q5_0`, `-q5_1` or `-q8_0` are %quantized: their weights are stored with
fewer bits (5 or 8 instead of 16), so they take less disk space and memory, and run faster than the
normal (non-quantized) models with the corresponding names. There is also `large-v3-turbo`,
a reduced version of `large-v3` that has fewer decoder layers: it is about half the size and thus
faster than the original `large-v3`. These optimized model variants were created to offer better
speed at the cost of reduced accuracy.

Once you have decided which model you want, download its `.bin` file and place it in your newly
created `whispercpp` folder. Any `.bin` file placed in this folder will be found by Praat.
In fact, if you want to experiment with different models, you can download
as many of them as you like; Praat will let you select one before you start transcription.

1.2. Transcribing into a TextGrid
=================================
1.2.1. How to start
===================
You need a @Sound and a @TextGrid for this Sound (transcription modifies an existing TextGrid;
it does not create one). The TextGrid should have at least one interval tier: transcription is
run on one selected interval, so you need an interval tier to contain this interval. To
transcribe the whole Sound, you can use an interval tier without internal boundaries and thus
consisting of only one interval spanning the whole Sound (interval 1).

There are two ways to start transcription into a TextGrid:
1. from the @TextGridEditor, via the ##Transcribe interval# command in the #Interval menu; you can
get into the TextGridEditor by selecting the TextGrid and the Sound together in the @@Objects
window@ and choosing ##View & Edit# from the @@Dynamic menu@;
2. from the Objects window itself, when you select the TextGrid and the Sound together and choose
@@TextGrid & Sound: Transcribe interval...@.

These two ways achieve exactly the same result, and which one you use is a matter of preference.
They differ only in how you select the interval you want to transcribe (see ##1.2.4. Transcribe!#),
and whether the configured transcription settings are persistent across Praat sessions (see the
next two paragraphs).

To perform transcription from the TextGridEditor, you first need to adjust the settings via
the separate command ##Transcription settings...# (in the #Interval menu). These settings are
preserved across transcription runs and across Praat sessions, so you can skip this step later
when you want to reuse the settings from the last time you transcribed. Note that the settings in
the ##Diarization...# block can also be configured via ##Diarization settings...# (also in the
#Interval menu), which is used by standalone diarization (see ##2. Automatic speaker diarization#).
This means that a change in diarization settings made in either place affects both
transcription-with-diarization and standalone diarization.

If you are running transcription from the Objects window, all the transcription settings appear
in the ##Transcribe interval...# command window. In this window, settings are preserved across
transcription runs but not across Praat sessions.

1.2.2. How to configure settings
================================
There are three blocks of settings: ##Transcription...#, ##Non-speech detection...# and
##Diarization...#. The last two configure optional steps in the transcription process; each
step can be switched on and off by the first setting in its block.

The ##Transcription...# block has three settings: ##Whisper model#, #Language and ##Include words#.

##Whisper model# lists all the models you installed in ##1.1. Installing Whisper models#; now is
the time to choose the model that you want to use. If you find yourself at this step with an empty
model list, this means something went wrong at the installation step. You can go back to
##1.1. Installing Whisper models# to check that the downloaded model(s) are placed in the proper
folder. After that, reopen ##Transcription settings...#; your installed model(s) will appear in
the list.

#Language is the language you want to recognize; you can select it from the list of all
available languages or keep the default ##Autodetect language# (note that for an English-only model
you are not allowed to select a language other than #English or the default ##Autodetect
language#).

The third transcription setting is ##Include words#. If this setting is on, an extra tier is
added to the TextGrid below the sentence tier, with one interval per word. If diarization is also
included (described below), each speaker gets their own word tier. Examples 2 and 4 below show the
resulting TextGrids when ##Include words# is on without and with diarization respectively.

##Non-speech detection...# is switched on and off by its first setting ##Detect non-speech#: when
it is on, the non-speech parts are first removed from the Sound, before it is passed to a Whisper
model. This both speeds up transcription and prevents the model from inventing text in the
non-speech parts. Another benefit of using non-speech detection is that it makes sentence and
word boundaries more precise; without it, the text might be stretched over the silent parts of
the Sound. See @@speech activity detection with Silero VAD@ for the description of the
settings influencing non-speech detection.

##Diarization...# is switched on and off by its first setting ##Include diarization#: when it is
on, diarization is also run on the Sound (independently of transcription). The results from both
transcription and diarization are then combined: transcribed text is divided among the detected
speakers. See @@speaker diarization with adapted pyannote.audio@ and ##2.2 How to configure
settings# for the description of the diarization settings.

1.2.3. Examples
===============
The structure of the resulting TextGrid depends on the combination of ##Include words# and
##Include diarization# settings. Four examples below show the different TextGrid outcomes for
different combinations of these two settings when the interval selected for transcription spans
the whole tier “Mary”.

##Example 1#: both ##Include words# and ##Include diarization# are off. The interval selected for
transcription is split into many intervals: one interval for each sentence containing the sentence
text, with empty intervals for non-speech parts. Sentence boundaries are defined by the punctuation
that whisper.cpp returns. No new tiers are inserted.
{- 6.0x3.0
	tierName$ = "Mary"
	textgrid = Create TextGrid: 0, 11, tierName$, ""
	Insert boundary: 1, 0.8
	Insert boundary: 1, 8.4
	Insert boundary: 1, 8.5
	Insert boundary: 1, 9.5
	Set interval text: 1, 2, "I start the sentence and I finish it."
	Set interval text: 1, 4, "Right."
	Draw: 0.0, 0.0, 1, 1, 1
	Axes: 0, 100, 0, 7
	One mark right: 0.7, 0, 0, 0, tierName$
	selectObject: textgrid
	Remove
}

##Example 2#: ##Include words# is on and ##Include diarization# is off. The original interval
is split into sentences as in ##Example 1#, plus a tier called “Mary/word” is added just below
“Mary”, with one interval per word.
{- 6.0x3.0
	tier1Name$ = "Mary"
	tier2Name$ = "Mary/word"
	textgrid = Create TextGrid: 0, 11, tier1Name$ + " " + tier2Name$, ""
	Insert boundary: 1, 0.8
	Insert boundary: 1, 8.4
	Insert boundary: 1, 8.5
	Insert boundary: 1, 9.5
	Insert boundary: 2, 0.8
	Insert boundary: 2, 1.6
	Insert boundary: 2, 2.3
	Insert boundary: 2, 2.7
	Insert boundary: 2, 3.2
	Insert boundary: 2, 4.8
	Insert boundary: 2, 5.1
	Insert boundary: 2, 5.7
	Insert boundary: 2, 6.5
	Insert boundary: 2, 6.7
	Insert boundary: 2, 7.8
	Insert boundary: 2, 8.1
	Insert boundary: 2, 8.4
	Insert boundary: 2, 8.5
	Insert boundary: 2, 9.5
	Set interval text: 1, 2, "I start the sentence and I finish it."
	Set interval text: 1, 4, "Right."
	Set interval text: 2, 2, "I"
	Set interval text: 2, 3, "start"
	Set interval text: 2, 5, "the"
	Set interval text: 2, 6, "sentence"
	Set interval text: 2, 8, "and"
	Set interval text: 2, 10, "I"
	Set interval text: 2, 11, "finish"
	Set interval text: 2, 13, "it"
	Set interval text: 2, 15, "Right"
	Draw: 0.0, 0.0, 1, 1, 1
	Axes: 0, 100, 0, 7
	One mark right: 0.6, 0, 0, 0, tier2Name$
	One mark right: 1.7, 0, 0, 0, tier1Name$
	selectObject: textgrid
	Remove
}

##Example 3#: ##Include words# is off and ##Include diarization# is on (and diarization detects
 at least two speakers). Tier “Mary” is renamed to “Mary/sp1” and additional tiers “Mary/sp2”,
“Mary/sp3”, ... are added, one tier per detected speaker. Each speaker’s tier contains the
intervals with the sentences spoken by that speaker. It is possible that a sentence is started by
one speaker and finished by another one. In this case the sentence is split between these
speakers’ tiers.
{- 6.0x3.0
	tier1Name$ = "Mary/sp1"
	tier2Name$ = "Mary/sp2"
	textgrid = Create TextGrid: 0, 11, tier1Name$ + " " + tier2Name$, ""
	Insert boundary: 1, 0.8
	Insert boundary: 1, 4.8
	Insert boundary: 2, 5.1
	Insert boundary: 2, 8.4
	Insert boundary: 1, 8.5
	Insert boundary: 1, 9.5
	Set interval text: 1, 2, "I start the sentence..."
	Set interval text: 2, 2, "... and I finish it."
	Set interval text: 1, 4, "Right."
	Draw: 0.0, 0.0, 1, 1, 1
	Axes: 0, 100, 0, 7
	One mark right: 0.6, 0, 0, 0, tier2Name$
	One mark right: 1.7, 0, 0, 0, tier1Name$
	selectObject: textgrid
	Remove
}

##Example 4#: both ##Include words# and ##Include diarization# are on. In addition to a
sentence tier, each speaker also gets a word tier (“Mary/sp1/w” for speaker 1, “Mary/sp2/w” for
speaker 2, ...), which is inserted directly after their sentence tier.
{- 6.0x3.0
	tier1Name$ = "Mary/sp1"
	tier2Name$ = "Mary/sp1/w"
	tier3Name$ = "Mary/sp2"
	tier4Name$ = "Mary/sp2/w"
	textgrid = Create TextGrid: 0, 11, tier1Name$ + " " + tier2Name$ + " " + tier3Name$ + " " + tier4Name$, ""
	Insert boundary: 1, 0.8
	Insert boundary: 1, 4.8
	Insert boundary: 3, 5.1
	Insert boundary: 3, 8.4
	Insert boundary: 1, 8.5
	Insert boundary: 1, 9.5
	Set interval text: 1, 2, "I start the sentence..."
	Set interval text: 3, 2, "... and I finish it."
	Set interval text: 1, 4, "Right."
	Insert boundary: 2, 0.8
	Insert boundary: 2, 1.6
	Insert boundary: 2, 2.3
	Insert boundary: 2, 2.7
	Insert boundary: 2, 3.2
	Insert boundary: 2, 4.8
	Insert boundary: 2, 8.5
	Insert boundary: 2, 9.5
	Insert boundary: 4, 5.1
	Insert boundary: 4, 5.7
	Insert boundary: 4, 6.5
	Insert boundary: 4, 6.7
	Insert boundary: 4, 7.8
	Insert boundary: 4, 8.1
	Insert boundary: 4, 8.4
	Set interval text: 2, 2, "I"
	Set interval text: 2, 3, "start"
	Set interval text: 2, 5, "the"
	Set interval text: 2, 6, "sentence"
	Set interval text: 4, 2, "and"
	Set interval text: 4, 4, "I"
	Set interval text: 4, 5, "finish"
	Set interval text: 4, 7, "it"
	Set interval text: 2, 8, "Right"
	Draw: 0.0, 0.0, 1, 1, 1
	Axes: 0, 100, 0, 7
	One mark right: 0.45, 0, 0, 0, tier4Name$
	One mark right: 1.3, 0, 0, 0, tier3Name$
	One mark right: 2.15, 0, 0, 0, tier2Name$
	One mark right: 3, 0, 0, 0, tier1Name$
	selectObject: textgrid
	Remove
}

1.2.4. Transcribe!
==================
With the transcription settings configured, you are now ready to start the transcription.

If you are transcribing from the TextGridEditor, select the interval you want to transcribe by
clicking on it and choose ##Transcribe interval# from the #Interval menu.

If you are transcribing from the Objects window, use the settings ##Tier number# and ##Interval
number# at the top of the ##Transcribe interval...# command window to specify the interval you
want to transcribe.

In either case, note that you cannot transcribe an interval belonging to a tier whose name already
contains a slash. This is to prevent endlessly growing names of the derived tiers.

Running transcription takes some time; how long it takes depends on the length of the selected
interval, the Whisper model, and whether diarization is included. Please read ##3. Performance#
if you would like to make transcription run faster.

1.3. Transcribing into plain text
=================================
You can also transcribe a whole @Sound into plain text. For this you need a @SpeechRecognizer
object. You can create one by choosing @@Create SpeechRecognizer...@ from the @@New menu@
in the @@Objects window@. A command window will appear containing two settings: ##Whisper model#
and #Language, both of which are described in ##1.2.2. How to configure settings#. After you
click #OK, you will find a new #SpeechRecognizer object in the object list.

To transcribe, select the SpeechRecognizer object and the Sound object together and choose
##SpeechRecognizer & Sound: Transcribe# from the @@Dynamic menu@. The result of transcription will
be written to the @@Info window@. Note that the ##Detect non-speech# setting is not available here:
@@speech activity detection with Silero VAD|Silero VAD@ is always on, with default settings.

2. Automatic speaker diarization
================================
Diarization in Praat detects different speakers in a @Sound. For each detected speaker, it
identifies %%speech segments%, which are time intervals during which this speaker is active.
Diarization can be done as part of transcription or standalone. In either case, it modifies an
existing @TextGrid, producing one interval tier for each detected speaker.

Diarization settings (see ##2.2. How to configure settings#) are shared between standalone
diarization and diarization as part of transcription. Everything else in this chapter is specific
to standalone diarization; the transcription-with-diarization case is described in ##1.2.
Transcribing into a TextGrid#.

2.1. How to start
=================
To perform diarization, you need a @Sound and a @TextGrid for this Sound (diarization modifies an
existing TextGrid; it does not create one). The TextGrid should have at least one interval tier:
diarization is run on one selected interval, so you need an interval tier to contain this
interval. To diarize the whole Sound, you can use an interval tier without internal boundaries
and thus consisting of only one interval spanning the whole Sound (interval 1).

There are two ways to start diarization:
1. from the @TextGridEditor, via the ##Diarize interval# command in the #Interval menu; you can
get into the TextGridEditor by selecting the TextGrid and the Sound together in the @@Objects
window@ and choosing ##View & Edit# from the @@Dynamic menu@;
2. from the Objects window itself, when you select the TextGrid and the Sound together and choose
@@TextGrid & Sound: Diarize interval...@.

These two ways achieve exactly the same result, and which one you use is a matter of preference.
They differ only in how you select the interval you want to diarize (see ##2.4. Diarize!#),
and whether the configured diarization settings are persistent across Praat sessions (see the
next two paragraphs).

To perform diarization from the TextGridEditor, you first need to adjust the settings via
the separate command ##Diarization settings...# (in the #Interval menu). These settings are
preserved across diarization runs and across Praat sessions, so you can skip this step later
when you want to reuse the settings from your last diarization run. Note that the diarization
settings are shared between standalone diarization and transcription-with-diarization. So if you
last changed them via ##Transcription settings...#, it is worth checking them before you run
diarization.

If you are running diarization from the Objects window, all the diarization settings appear
in the ##Diarize interval...# command window. In this window, settings are preserved across
diarization runs but not across Praat sessions.

2.2. How to configure settings
==============================
##Non-speech interval label# and ##Speech interval label# are the labels in the resulting TextGrid
assigned to intervals classified as non-speech and speech respectively. These two settings only
influence the visual appearance of the result of diarization but not the result itself.

##Max. number of speakers (≥ 2)# and ##Clustering threshold (0-2)# both influence how many
speakers diarization detects. ##Max. number of speakers (≥ 2)# defines an upper limit on the number
of detected speakers, and while there is no equivalent setting for the lower limit, ##Clustering
threshold (0-2)# can be used to push this number up. These two settings can be used to improve the
quality of diarization if you know exactly how many speakers are in your Sound. If this is
the case, you might find the following recipe useful:
1. set the ##Max. number of speakers (≥ 2)# to the number of active speakers;
2. do a test diarization run on (a part of) your Sound;
3. if fewer speakers were detected than are active in the Sound, then try lowering the
##Clustering threshold (0-2)#, perhaps making it 0.1 lower than it is now.

Repeat steps 2 and 3 until diarization detects the correct number of speakers.

Tweaking the ##Clustering threshold (0-2)# might especially help when the voices of the speakers
are similar, or when recording is done in a noisy environment. If you would like to know the
details of what clustering threshold is and why it works this way, please read the #Algorithm
section of @@speaker diarization with adapted pyannote.audio@, specifically the part about
#Clustering.

The ##Allow speakers to overlap# setting does what its name suggests: if it is on, diarization
can detect when two speakers are speaking at the same time. Note that diarization does not detect
overlap of three or more speakers.

The last setting, ##Segmentation step (0-1)#, can be used to find a balance between speed and
accuracy. You can read more about it in ##3. Performance#. To start with, you can keep its
standard value of 0.1.

2.3. Example
============
This example shows the result of a standalone diarization run on the same Sound as in ##1.2.3.
Examples#; the interval selected for diarization spans the whole tier “Mary”. Diarization detects
two speakers, so tier “Mary” is renamed to “Mary/sp1”, and the second tier “Mary/sp2” is added.
Each speaker’s tier contains alternating intervals: non-speech and speech intervals labelled with
the configured ##Non-speech interval label# (blank in this case) and ##Speech interval label#
(“speech” in this case), respectively.
{- 6.0x3.0
	tier1Name$ = "Mary/sp1"
	tier2Name$ = "Mary/sp2"
	textgrid = Create TextGrid: 0, 11, tier1Name$ + " " + tier2Name$, ""
	Insert boundary: 1, 0.8
	Insert boundary: 1, 4.8
	Insert boundary: 2, 5.1
	Insert boundary: 2, 8.4
	Insert boundary: 1, 8.5
	Insert boundary: 1, 9.5
	Set interval text: 1, 2, "speech"
	Set interval text: 2, 2, "speech"
	Set interval text: 1, 4, "speech"
	Draw: 0.0, 0.0, 1, 1, 1
	Axes: 0, 100, 0, 7
	One mark right: 0.6, 0, 0, 0, tier2Name$
	One mark right: 1.7, 0, 0, 0, tier1Name$
	selectObject: textgrid
	Remove
}

2.4. Diarize!
=============
With the diarization settings configured, you are now ready to start the diarization.

If you are diarizing from the TextGridEditor, select the interval you want to diarize by
clicking on it and choose ##Diarize interval# from the #Interval menu.

If you are diarizing from the Objects window, use the settings ##Tier number# and ##Interval
number# at the top of the ##Diarize interval...# command window to specify the interval you
want to diarize.

In either case, note that you cannot diarize an interval belonging to a tier whose name already
contains a slash. This is to prevent endlessly growing names of the derived tiers.

Running diarization takes some time; how long it takes depends on the length of the selected
interval and on the diarization settings. Please read ##3. Performance# if you would like to make
diarization run faster.

3. Performance
==============
Speech recognition tools in Praat rely on neural models, which consume a lot of computational
resources. So the time spent on transcription and diarization might become an obstacle to using
them, especially if you need to analyse a corpus or a large set of recordings. This chapter offers
some advice on how you can try to make speech recognition tools run faster.

3.1. AI settings
================
Most modern computers have several %%physical processors%. Each physical processor can process one
or two threads that run computations in parallel. The number of threads that a computer can run
in parallel is the number of its %%logical processors%.

A reasonable assumption would be that transcription and diarization run fastest when Praat uses as
many threads as the computer has logical processors. However, in practice, using that many threads
can cause a dramatic slowdown on some computers. The question is then: exactly how many threads
is best to use? Unfortunately, the answer depends on the computer’s hardware architecture in ways
that are difficult to predict in advance. But it appears from our tests that half of the
available logical processors is a safe starting point that avoids the worst slowdowns, so this is
the default Praat uses for both transcription and diarization. If you suspect that this default
is not optimal for your computer, or if you would like to experiment, you can change it in ##AI
settings...#, which you can find in the #Settings submenu of the @@Praat menu@. But keep in mind
that it is probably better not to use more threads than the number of logical processors your
computer has.

The optimal number of threads can differ between transcription and diarization, because they use
different models and parallelize their work in different ways. So it is better to tune them
separately:
- to tune transcription, change its ##Max. number of threads# and measure the time spent on
transcription %without diarization;
- to tune diarization, change its ##Max. number of threads# and measure the time spent on
standalone diarization.

3.2. Other ways to make transcription faster
============================================
The choice of Whisper model has a strong influence on how long transcription takes. If your
transcription is too slow, you can try to switch to a smaller or a quantized model. You can read
more about available models in ##1.1. Installing Whisper models#.

Switching on ##Detect non-speech# (see ##1.2.2. How to configure settings#) also speeds up
transcription, especially on a Sound that contains a lot of parts without speech. This is because
non-speech parts are removed before the Sound is passed to a Whisper model, making the sound that
is actually analysed shorter. This is the setting which you may want to have always on, because
it also improves accuracy of the detected word and sentence boundaries.

If you use transcription with diarization (##Include diarization# setting is on), then the
overall time also depends on how fast diarization is.

3.3. Other ways to make diarization faster
==========================================
The ##Segmentation step (0-1)# setting (described in detail in @@speaker diarization with adapted
pyannote.audio@) controls the overlap between successive 10-second analysis windows and therefore
the overall number of analysis windows that the segmentation model processes. The segmentation
step itself is a distance between the starts of two consecutive windows as a fraction of the
window length, but the smaller this distance, the bigger the overlap.

For example, a segmentation step of 0.1 makes this distance 1 second, so that two consecutive
windows have a 90\%  overlap. Doubling the step value to 0.2 reduces the overlap to 80\%  and
halves the overall number of analysis windows the model has to process; as a result
diarization runs roughly twice as fast. Increasing the step value to 0.5 further reduces
the analysis window overlap, making diarization run approximately five times faster.

But the price for this speedup is reduced accuracy. After processing all analysis windows, the
diarization algorithm reconstructs the result for the whole Sound by averaging the model’s
predictions across the all the analysis windows. Less window overlap
means averaging across fewer windows, which produces less accurate results.

If you want to speed up your diarization, you may experiment with increasing this value. But
perhaps it’s best to keep it below 0.5 so that every moment in the sound is analysed at least in
two analysis windows.

################################################################################
"speech activity detection with Silero VAD"
© Anastasia Shchupak 2026-06-01

Praat uses the @@whisper.cpp@ implementation of the @@Silero VAD@ speech activity detector.
The pre-trained Silero VAD model weights have been converted to ggml format and compiled into Praat,
so no external model files are required. The sound is automatically resampled to 16 kHz
(the sampling frequency expected by the Silero VAD model) before being processed by the model.

Purpose
=======
to detect which parts of a sound contain speech. The output is a list of speech segments,
each defined by a start and an end time.

Algorithm
=========
The Silero VAD model processes the sound in fixed frames of 512 samples (32 ms, since the sound
is resampled to 16 kHz). For each frame, it outputs a probability that the frame contains speech.
Based on this output, the list of speech segments is constructed as described below. This
description reflects the whisper.cpp implementation, which differs slightly from Silero’s
original implementation.

A speech segment begins at the first frame whose speech probability exceeds the ##Speech probability
threshold#. The segment continues as long as the speech probability stays above a lower threshold
(called the “negative threshold” in the Silero source code, equal to the speech probability
threshold minus 0.15). The segment ends only when the probability stays below the negative
threshold for at least ##Min. gap between speech segments#. When a segment ends, it is kept only
if it is longer than ##Min. speech segment#.

After all segments have been formed, segments separated by a gap shorter than 0.2 s are merged
(this is hardcoded in whisper.cpp and not configurable). After that, padding is applied: each
segment is extended on both sides by ##Padding around speech segments#. If padding would cause
two segments to overlap, they instead meet at the midpoint of the gap between them.

The result is a list of speech segments.

Settings
========
##Speech probability threshold (0-1)# (standard value: 0.5)
:   determines the sensitivity of the speech detector. Higher values make the detector less
	sensitive, meaning that a frame requires a higher speech probability to be considered part of
	a speech segment. This reduces false positives (non-speech incorrectly classified as speech),
	but may cause some speech to be missed. Lower values make the detector more sensitive. The
	default of 0.5 works well for most use cases.

##Min. gap between speech segments (s)# (standard value: 0.1)
:   the minimum duration of a gap between two speech segments. You might want to increase this value
	if short silences within speech (e.g. plosive closures) are splitting speech into multiple
	segments. Note that gaps shorter than 0.2 s are removed from the output (with their adjacent
	speech segments merged), regardless of this setting.

##Min. speech segment (s)# (standard value: 0.25)
:   the minimum duration of a speech segment. Shorter segments are discarded.

##Padding around speech segments (s)# (standard value: 0.0)
:   extends each detected speech segment by this amount on both sides. You might want to increase
	this value if speech onsets and offsets are being clipped.

Availability in Praat
=====================
Silero VAD speech activity detection is available in Praat:
- as part of transcription, running just before it to remove non-speech regions from the
  analysed sound (see @@transcription with whisper.cpp@);
- standalone, producing a new @TextGrid with non-speech and speech intervals (see @@Sound: To
  TextGrid (speech activity, Silero)...@).

################################################################################
"Sound: To TextGrid (speech activity, Silero)..."
© Anastasia Shchupak 2026-06-01

A command that creates a @TextGrid with one interval tier from every selected @Sound object.
The interval tier contains non-speech and speech intervals with boundaries determined by the
Silero VAD model (for the algorithm and settings, see @@speech activity detection with Silero VAD@).
The labels of the intervals are specified by the settings ##Non-speech interval label# and ##Speech
interval label#.

Settings
========
##Speech probability threshold (0-1)# (standard value: 0.5)
:	see @@speech activity detection with Silero VAD@.

##Min. gap between speech segments (s)# (standard value: 0.1)
:	see @@speech activity detection with Silero VAD@.

##Min. speech segment (s)# (standard value: 0.25)
:	see @@speech activity detection with Silero VAD@.

##Padding around speech segments (s)# (standard value: 0.0)
:	see @@speech activity detection with Silero VAD@.

##Non-speech interval label# (standard value: “”)
:	the label assigned to intervals classified as non-speech in the resulting TextGrid.

##Speech interval label# (standard value: “speech”)
:	the label assigned to intervals classified as speech in the resulting TextGrid.

################################################################################
"transcription with whisper.cpp"
© Anastasia Shchupak 2026-06-01

Praat can perform automatic transcription of a sound using @@whisper.cpp@. To do this, at least
one Whisper model must be installed on your computer. You can find the details about how to
install models and how to use transcription in the @@Speech recognition@ tutorial. The sound is
automatically resampled to 16 kHz (the sampling frequency expected by whisper.cpp) before being
transcribed. This page documents the transcription settings.

Behaviour
=========
When transcription is run on a @TextGrid interval, it modifies the TextGrid: intervals are split,
tiers may be added or renamed. The exact resulting TextGrid structure depends on the combination
of the ##Include words# and ##Include diarization# settings. See the @@Speech recognition@ tutorial
for the details of the transcription output under different combinations of these settings.

Settings
========
##Whisper model#
:	determines which Whisper model is used.
	The list is populated with the `.bin` files found in the `whispercpp` subfolder of the
	`models` folder in the Praat preferences folder. See the @@Speech recognition@ tutorial for
	details on how to install models.

##Language# (standard value: ##Autodetect language#)
:	determines the language to be used for transcription.
	Choose ##Autodetect language# to let the model detect the language automatically. If you know
	the language you want to use for transcription, selecting it explicitly may improve
	transcription accuracy. Note that English-only models (those with ##.en# in the name) can only
	be used with ##Autodetect language# or ##English#.

##Include words# (standard: on)
:	if on, each transcribed word is given a start and an end time, computed using whisper.cpp’s
	internal dynamic time warping (DTW) algorithm.

##Detect non-speech# (standard: on)
:	if on, @@speech activity detection with Silero VAD@ runs before transcription to identify
	speech regions. Only those regions are then passed to the Whisper model. This generally
	improves both speed and accuracy of transcription. Speed is improved by reducing the length of
	the sound sent to the model, and accuracy by preventing the model from hallucinating text for
	silent regions.

##Speech probability threshold (0-1)# (standard value: 0.5)
:	see @@speech activity detection with Silero VAD@.

##Min. gap between speech segments (s)# (standard value: 0.1)
:	see @@speech activity detection with Silero VAD@.

##Min. speech segment (s)# (standard value: 0.25)
:	see @@speech activity detection with Silero VAD@.

##Padding around speech segments (s)# (standard value: 0.0)
:	see @@speech activity detection with Silero VAD@.

##Include diarization# (standard: off)
:	if on, speaker diarization is run alongside transcription (see @@speaker diarization with
	adapted pyannote.audio@). The results of both are later combined to attribute portions of
	transcribed speech to different speakers.

##Max. number of speakers (≥ 2)# (standard value: 2)
:	see @@speaker diarization with adapted pyannote.audio@.

##Allow speakers to overlap# (standard: on)
:	see @@speaker diarization with adapted pyannote.audio@.

##Clustering threshold (0-2)# (standard value: 0.7)
:	see @@speaker diarization with adapted pyannote.audio@.

##Segmentation step (0-1)# (standard value: 0.1)
:	see @@speaker diarization with adapted pyannote.audio@.

Availability in Praat
=====================
Transcription with whisper.cpp is available in two ways in Praat:
- if you select a @Sound together with its @TextGrid and choose
  @@TextGrid & Sound: Transcribe interval...|Transcribe interval...@;
- via ##Transcribe interval# from the #Interval menu in the @TextGridEditor.
  The settings for this command are set via ##Transcription settings...# in the same menu
  and are remembered across Praat sessions.

################################################################################
"TextGrid & Sound: Transcribe interval..."
© Anastasia Shchupak 2026-06-01

This command takes a specified interval of the @TextGrid, transcribes the corresponding part
of the @Sound, and writes the result back into the TextGrid.

Settings
========
##Tier number
:	the number of the tier containing the interval to be transcribed.

##Interval number
:	the number of the interval to be transcribed.

The remaining settings in this dialog control the transcription itself; see @@transcription with
whisper.cpp@ for their meaning.

################################################################################
"speaker diarization with adapted pyannote.audio"
© Anastasia Shchupak 2026-06-01

Speaker diarization detects which parts of a sound contain speech, and attributes each
part to one or more speakers. The output is a list of segments, each segment defined by a start
time, an end time and a speaker identifier. Speaker identifiers are natural numbers (1, 2, 3, ...);
they are arbitrary labels that roughly follow the order in which the speakers first appear in the
sound.

Diarization in Praat always modifies an existing TextGrid by producing one tier per detected
speaker. It can be done as part of transcription (so that the transcribed text is split among the
speaker tiers) or standalone (so that each speaker tier contains the intervals labelled as
non-speech or speech). See @@Speech recognition@ tutorial for details on how to use it.

Praat performs speaker diarization using a C++/ggml adaptation of @@pyannote.audio@’s
`pyannote/speaker-diarization-3.1` pipeline. The two neural models used by the pipeline,
`pyannote/segmentation-3.0` (for segmentation) and `wespeaker-voxceleb-resnet34-LM` (for
speaker embedding, see @@WeSpeaker@), have been converted to ggml format and compiled into Praat,
so no external model files are required. The sound is automatically resampled to 16 kHz
(the sampling frequency expected by both models) before being processed.

Settings
========
##Max. number of speakers (≥ 2)# (standard value: 2)
:	an upper bound on the number of speakers the algorithm may produce. Must be at least 2. There
	is no matching “minimum number of speakers” setting; if you want to push the algorithm towards
	distinguishing more speakers, you might try lowering the ##Clustering threshold#.

##Allow speakers to overlap# (standard: on)
:   if on, at most two speakers may be active at the same moment. If off, every moment is
	attributed to a single speaker, the one most active at that moment.

##Clustering threshold (0-2)# (standard value: 0.7)
:	controls the number of detected speakers. Lower thresholds produce more speakers (but anyway
	up to ##Max. number of speakers (≥ 2)#); higher thresholds produce fewer speakers. This is
	the setting that can push the number of detected speakers up, so consider lowering its value
	if fewer speakers are detected than are actually present in your sound.

##Segmentation step (0-1)# (standard value: 0.1)
:   the distance between the starts of consecutive overlapping analysis windows, as a fraction of
	the analysis window’s length. The sound is analysed in 10-second overlapping segments called
	%%analysis windows%; because of the overlap, each moment in time is covered by several
	analysis windows. A smaller segmentation step ensures more analysis windows with more overlap,
	which is generally more accurate but takes longer. A larger segmentation step ensures fewer
	windows with less overlap, which is faster but generally less accurate. You may try
	increasing this value if you want diarization to run faster, but it’s best to keep it below 0.5
	so that every moment in the sound is covered by at least two analysis windows.

Algorithm
=========
The algorithm is a port of @@pyannote.audio@’s `pyannote/speaker-diarization-3.1` pipeline (see
@@Bredin (2023)@) with some adaptations. It has four stages.

##1. Segmentation#. The sound is divided into overlapping 10-second %%analysis windows%. The
distance between the starts of two consecutive windows is defined by the ##Segmentation step
(0-1)# as a fraction of the window length. For example, a segmentation step of 0.1 makes this
distance 1 second, so that consecutive windows have a 90\%  overlap.

The sound from each analysis window is then sent to the %%segmentation model%, which divides it
into 589 frames (each frame spanning approximately 17 milliseconds) and assigns a label to each
frame in the following way:
- the model assumes that there are at most three speakers in one analysis window, of whom at most
two can be active in each frame;
- for every frame, the model computes a 7-dimensional vector of probabilities for the following
combinations of active speakers: {}, {1}, {2}, {3}, {1, 2}, {1, 3}, {2, 3};
- the most probable combination is then taken as that frame’s label.

The speaker numbers 1, 2 and 3 are local to each analysis window: speaker 1 in one window is not
necessarily the same person as speaker 1 in another. Mapping the window-local speakers to the
global ones is the task of stages 2 and 3.

##2. Speaker embeddings#. %Embedding is a term used in the field of machine learning which inherited
it from mathematics. In machine learning, an %embedding is a specific kind of representation of an
object as a vector in a space of many dimensions (or, in other words, a point in that space),
chosen in such a way that vectors representing similar objects end up close to each other. The object
is thus %embedded into a multidimensional space, and the resulting vector is itself called an
%embedding. What is an “object” and what counts as “similar” depends on the task; here an object is
a sound, and two sounds are considered similar if the same person is speaking in them.

The previous stage determined which speaker is active in which frame (for every analysis window).
Now, for each window and each local speaker in this window (1, 2 or 3), the frames are found where
this speaker is active. For example, speaker 1 is active in the frames where at least one of the
combinations {1}, {1, 2} or {1, 3} from stage 1 had non-zero probability. Then, for each window and
each speaker active somewhere in it, the frames where this speaker is active are glued together into
a single sound, which is then sent to the %%embedding model%. For every such sound, this model
produces an %embedding: a 256-dimensional vector representing one particular speaker in one
particular analysis window. The embeddings of the same speaker (from different analysis windows)
tend to be closer to each other than the embeddings of different speakers. This makes the next
stage possible.

##3. Clustering#. The previous stage produces one embedding for each active speaker in each
analysis window (so, up to three embeddings per window). The embeddings that are backed by
enough non-overlapping speech are considered to be “reliable” and are used in the clustering
process; the others are set aside for now.

The reliable embeddings from all analysis windows are first L2-normalized so that they are all
located on a 256-dimensional unit hypersphere. They are then grouped using %%agglomerative
hierarchical clustering with centroid linkage%. Grouping starts with each embedding forming its
own group (with the centre of the group being the embedding itself). At each step, the two groups
whose centres are the closest (as measured by Euclidean distance between them) are merged, and
the centre of the newly formed group is the mean of all the embeddings in this group. This centre
is not L2-normalized, therefore the centres of merged groups lie inside the 256-dimensional
hypersphere and become slightly shorter with every merge. This process stops when the distance
between the next two closest groups is larger than the ##Clustering threshold (0-2)#.

Because the embeddings are L2-normalized, the distance between any two of them lies between 0
(identical) and 2 (opposite), which explains the range of possible values for the clustering
threshold.

Each final group represents one speaker; if after reaching the threshold there are still more groups
than ##Max. number of speakers (≥ 2)#, the merging continues until the resulting number of groups
does not exceed that maximum.

Finally, the unreliable embeddings (those not used to form the groups) are attached to their
nearest groups. In this way, each window-local speaker is assigned to a global speaker.

The two figures below show an example of the clustering process for four embeddings in a
two-dimensional space. Each group centre is drawn as a solid arrow surrounded by a grey circle whose
radius is the clustering threshold (here 0.7, the default). Two groups can be merged only if
both their centres lie inside each other’s grey circles (or, in other words, if they are closer
than the clustering threshold).

The ##left figure# shows the initial four group centres (the same as the four embeddings). The
closest two group centres are drawn in red; these groups are merged first. The next merge
involves the groups with the next two closest centres (those drawn in blue). The ##right figure#
shows the state after the two merges: the solid arrows are the centres of the two newly formed
groups; the dotted arrows are the original embeddings making up each group. Now, neither of the
two group centres lies inside the grey circle surrounding the other one. So the two groups are
further apart than the clustering threshold; therefore, the clustering process stops, leaving
these two groups as the final result.

{- 5.5x3
	b1 = 35
	b2 = 75
	r1 = 235
	r2 = 245
	b1x = cos(b1*pi/180)
	b1y = sin(b1*pi/180)
	b2x = cos(b2*pi/180)
	b2y = sin(b2*pi/180)
	r1x = cos(r1*pi/180)
	r1y = sin(r1*pi/180)
	r2x = cos(r2*pi/180)
	r2y = sin(r2*pi/180)
	threshold = 0.7

	# ==== LEFT ========
	Select inner viewport: 0, 2.5, 0.25, 2.75
	Axes: -1.5, 1.5, -1.5, 1.5
	Solid line

	# thresholds
	Line width: 0.6
	Colour: "Grey"
	Paint circle: 0.9, b1x, b1y, threshold
	Paint circle: 0.9, b2x, b2y, threshold
	Paint circle: 0.9, r1x, r1y, threshold
	Paint circle: 0.9, r2x, r2y, threshold
	Draw circle: b1x, b1y, threshold
	Draw circle: b2x, b2y, threshold
	Draw circle: r1x, r1y, threshold
	Draw circle: r2x, r2y, threshold

	# axes
	Line width: 1
	Colour: "Grey"
	Draw line: -1.3, 0, 1.3, 0
	Draw line: 0, -1.3, 0, 1.3
	Text special: 1.03, "left", 0, "bottom", "Times", 10, "0", "1"
	Text special: -1.03, "right", 0, "bottom", "Times", 10, "0", "\-m1"
	Text special: 0.03, "left", 1.01, "bottom", "Times", 10, "0", "1"
	Text special: -0.03, "right", -1.02, "top", "Times", 10, "0", "\-m1"

	# unit hypersphere
	Line width: 2
	Colour: "Black"
	Draw circle: 0, 0, 1

	# blue vectors
	Colour: "Blue"
	Line width: 2
	Draw arrow: 0.0, 0.0, b1x, b1y
	Draw arrow: 0.0, 0.0, b2x, b2y

	# red vectors
	Colour: "Red"
	Line width: 2
	Draw arrow: 0.0, 0.0, r1x, r1y
	Draw arrow: 0.0, 0.0, r2x, r2y

	# ==== RIGHT ========
	b3x = (b1x + b2x) / 2
	b3y = (b1y + b2y) / 2
	r3x = (r1x + r2x) / 2
	r3y = (r1y + r2y) / 2

	Select inner viewport: 3, 5.5, 0.25, 2.75
	Axes: -1.5, 1.5, -1.5, 1.5
	Solid line

	# thresholds
	Line width: 0.6
	Colour: "Grey"
	Paint circle: 0.9, b3x, b3y, threshold
	Paint circle: 0.9, r3x, r3y, threshold
	Draw circle: b3x, b3y, threshold
	Draw circle: r3x, r3y, threshold

	# axes
	Line width: 1
	Colour: "Grey"
	Draw line: -1.3, 0, 1.3, 0
	Draw line: 0, -1.3, 0, 1.3
	Text special: 1.03, "left", 0, "bottom", "Times", 10, "0", "1"
	Text special: -1.03, "right", 0, "bottom", "Times", 10, "0", "\-m1"
	Text special: 0.03, "left", 1.01, "bottom", "Times", 10, "0", "1"
	Text special: -0.03, "right", -1.02, "top", "Times", 10, "0", "\-m1"

	# unit hypersphere
	Line width: 2
	Colour: "Black"
	Draw circle: 0, 0, 1

	# blue vectors
	Dotted line
	Colour: "Blue"
	Line width: 1
	Draw arrow: 0.0, 0.0, b1x, b1y
	Draw arrow: 0.0, 0.0, b2x, b2y
	Solid line
	Line width: 2
	Draw arrow: 0.0, 0.0, b3x, b3y

	# red vectors
	Dotted line
	Colour: "Red"
	Line width: 1
	Draw arrow: 0.0, 0.0, r1x, r1y
	Draw arrow: 0.0, 0.0, r2x, r2y
	Solid line
	Line width: 2
	Draw arrow: 0.0, 0.0, r3x, r3y
}

##4. Reconstruction#. At stage 1, each analysis window was divided into 589 frames of
approximately 17 milliseconds, and each window-frame received a 7-dimensional vector with
probabilities of different combinations of local speakers being active. Because the analysis
windows overlap, each frame on the global timeline is covered by several windows. The goal of
the current stage is to combine the window-frame information across all covering windows, using the
local-to-global speaker mapping established at stage 3.

Using the 7-dimensional vectors from stage 1, for each speaker in each window-frame, %%soft
activations% are computed, by adding together the probabilities of all combinations in which that
speaker is active. This is a number between 0 and 1 (where 0 means definitely silent and 1 means
definitely active). These local speakers’ soft activations are then attributed to the global
speakers using the mapping from stage 3. After that they are averaged across all the covering
windows, to produce an %%average activation% for each global speaker in each global frame.

Separately, the number of simultaneously active speakers in each global frame is found in a
similar way. Each window-frame had a winning combination of active speakers (the frame label from
stage 1), containing 0, 1 or 2 speakers. For each global frame, this number is averaged across
all covering windows, rounded, and capped at 1 when ##Allow speakers to overlap# is off. The
resulting number determines how many speakers (those with the highest average activations) are
marked active in this frame.

Finally, for each speaker, every uninterrupted sequence of frames in which that speaker is active
becomes one %segment. The result is a list of segments, where each segment is attributed to one
speaker.

Availability in Praat
=====================
For diarization as part of transcription, see @@transcription with whisper.cpp@.

Standalone diarization is available in two ways:
- if you select a @Sound together with its @TextGrid and choose
  @@TextGrid & Sound: Diarize interval...|Diarize interval...@;
- via ##Diarize interval# from the #Interval menu in the @TextGridEditor. The settings for this
  command are set via ##Diarization settings...# in the same menu and are remembered across Praat
  sessions.

################################################################################
"TextGrid & Sound: Diarize interval..."
© Anastasia Shchupak 2026-06-01

This command takes a specified interval of the @TextGrid, runs diarization on the corresponding part
of the @Sound, and writes the result back into the TextGrid. The labels of the intervals are
specified by the settings ##Non-speech interval label# and ##Speech interval label#.

Settings
========
##Tier number
:	the number of the tier containing the interval to be diarized.

##Interval number
:	the number of the interval to be diarized.

##Max. number of speakers (≥ 2)# (standard value: 2)
:	see @@speaker diarization with adapted pyannote.audio@.

##Allow speakers to overlap# (standard: on)
:	see @@speaker diarization with adapted pyannote.audio@.

##Clustering threshold (0-2)# (standard value: 0.7)
:	see @@speaker diarization with adapted pyannote.audio@.

##Segmentation step (0-1)# (standard value: 0.1)
:	see @@speaker diarization with adapted pyannote.audio@.

##Non-speech interval label# (standard value: “”)
:	the label assigned to intervals classified as non-speech in the resulting TextGrid.

##Speech interval label# (standard value: “speech”)
:	the label assigned to intervals classified as speech in the resulting TextGrid.

################################################################################
"SpeechRecognizer"
© Anastasia Shchupak 2026-03-15

One of the @@types of objects@ in Praat. It performs @@transcription with whisper.cpp@ on a
@Sound object. If you are new to speech recognition in Praat, see the @@Speech recognition@
tutorial first.

Commands
========

Creation:
,	@@Create SpeechRecognizer...@

Transcription:
,	@@SpeechRecognizer & Sound: Transcribe@

################################################################################
"Create SpeechRecognizer..."
© Anastasia Shchupak 2026-03-15

Creates the @SpeechRecognizer object.
If you are new to speech recognition in Praat, see the @@Speech recognition@ tutorial first.

Settings
========
##Whisper model
##Language
:	for both settings see @@transcription with whisper.cpp@.

################################################################################
"SpeechRecognizer & Sound: Transcribe"
© Anastasia Shchupak 2026-03-15

@@transcription with whisper.cpp|Transcribes@ selected @Sound using selected @SpeechRecognizer
object and writes the result of transcription to the @@Info window@.

The sound is automatically resampled to 16 kHz (the sampling frequency expected by @@whisper.cpp@)
before being processed.

The transcription uses the @@speech activity detection with Silero VAD@ built into @@whisper.cpp@
to skip non-speech parts of the sound, which improves both speed and accuracy.

The result is a flat text string containing the full transcription.

################################################################################
"Silero VAD"
© Anastasia Shchupak 2026-06-01

Silero VAD is a pre-trained speech activity detector created by Silero Team; see
@@Silero Team (2024)@ for the model and documentation. VAD stands for “voice activity detection”,
but we use “speech activity detection” instead, because Silero VAD detects unvoiced parts of speech
as well as voiced parts. The Silero VAD model is compiled into Praat (see @Acknowledgments).

For how Silero VAD is used in Praat, as well as its algorithm and settings, see @@speech activity
detection with Silero VAD@.

################################################################################
"whisper.cpp"
© Anastasia Shchupak 2026-06-01

Whisper is an automatic speech recognition (ASR) system by OpenAI for transcribing speech
into text; see @@Radford et al. (2022)@ for the model architecture, training and evaluation.
OpenAI’s Whisper implementation is in Python (PyTorch).

Praat uses whisper.cpp, a lightweight C/C++ port of Whisper built on top of the ggml tensor library
for machine learning, developed by Georgi Gerganov and many other contributors (see
@Acknowledgments). The original OpenAI Whisper models must be converted to ggml format for use with
whisper.cpp.

For how transcription is used in Praat, see the @@Speech recognition@ tutorial.
For the transcription settings, see @@transcription with whisper.cpp@.

################################################################################
"pyannote.audio"
© Anastasia Shchupak 2026-06-01

pyannote.audio is an automatic speaker diarization toolkit developed by Hervé Bredin and
collaborators (see @@Plaquet & Bredin (2023)@ and @@Bredin (2023)@).

Praat contains a C++/ggml adaptation of its `pyannote/speaker-diarization-3.1` pipeline.
The pipeline uses two neural models: `pyannote/segmentation-3.0` by pyannote.audio for segmentation
and `wespeaker-voxceleb-resnet34-LM` by @@WeSpeaker@ for speaker embedding.
The `pyannote/segmentation-3.0` weights have been converted to ggml format
and embedded into Praat (see @Acknowledgments).

For how speaker diarization is used in Praat, see the @@Speech recognition@ tutorial.
For the diarization settings, see @@speaker diarization with adapted pyannote.audio@.

################################################################################
"WeSpeaker"
© Anastasia Shchupak 2026-06-01

WeSpeaker is a speaker embedding learning toolkit developed by WeNet Community
(see @@Wang et al. (2023)@ and @@Wang et al. (2024)@). Praat uses one of its pretrained models,
the `wespeaker-voxceleb-resnet34-LM` embedding model, as part of @@pyannote.audio@’s
`pyannote/speaker-diarization-3.1` pipeline.

`wespeaker-voxceleb-resnet34-LM` was trained on the VoxCeleb2 dataset
(see @@Chung, Nagrani & Zisserman (2018)@ and @@Nagrani, Chung & Zisserman (2017)@).
The model weights have been converted to ggml format and embedded into Praat (see @Acknowledgments).
Praat contains a C++/ggml port of WeSpeaker’s ResNet34 architecture with TSTP pooling,
used for inference on this model.

################################################################################
"Silero Team (2024)"
© Anastasia Shchupak 2026-06-01

Silero Team (2024). Silero VAD: pre-trained enterprise-grade voice activity detector (VAD),
number detector and language classifier [Computer software]. Version 6.2.0.

Available on `https://github.com/snakers4/silero-vad`.

################################################################################
"Radford et al. (2022)"
© Anastasia Shchupak 2026-06-01

Alec Radford, Jong Wook Kim, Tao Xu, Greg Brockman, Christine McLeavey & Ilya Sutskever (2022):
“Robust speech recognition via large-scale weak supervision.”

Available on `https://cdn.openai.com/papers/whisper.pdf`.

################################################################################
"Plaquet & Bredin (2023)"
© Anastasia Shchupak 2026-06-01

Alexis Plaquet & Hervé Bredin (2023): “Powerset multi-class cross entropy loss for neural
speaker diarization.” %%Proc. Interspeech 2023%.

Available on `https://www.isca-archive.org/interspeech_2023/plaquet23_interspeech.html`.

################################################################################
"Bredin (2023)"
© Anastasia Shchupak 2026-06-01

Hervé Bredin (2023): “pyannote.audio 2.1 speaker diarization pipeline: principle, benchmark,
and recipe.” %%Proc. Interspeech 2023%.

Available on `https://www.isca-archive.org/interspeech_2023/bredin23_interspeech.html`.

################################################################################
"Wang et al. (2023)"
© Anastasia Shchupak 2026-06-01

Hongji Wang, Chengdong Liang, Shuai Wang, Zhengyang Chen, Binbin Zhang, Xu Xiang, Yanlei Deng
& Yanmin Qian (2023): “Wespeaker: a research and production oriented speaker embedding learning
toolkit.” %%ICASSP 2023 - 2023 IEEE International Conference on Acoustics, Speech and Signal
Processing (ICASSP)%, 1–5.

Available on `https://doi.org/10.48550/arXiv.2210.17016`.

################################################################################
"Wang et al. (2024)"
© Anastasia Shchupak 2026-06-01

Shuai Wang, Zhengyang Chen, Bing Han, Hongji Wang, Chengdong Liang, Binbin Zhang, Xu Xiang,
Wen Ding, Johan Rohdin, Anna Silnova, Yanmin Qian & Haizhou Li (2024): “Advancing speaker
embedding learning: Wespeaker toolkit for research and production.” %%Speech Communication%
##162#: 103104.

Available on `https://doi.org/10.1016/j.specom.2024.103104`.

################################################################################
"Chung, Nagrani & Zisserman (2018)"
© Anastasia Shchupak 2026-06-01

Joon Son Chung, Arsha Nagrani & Andrew Zisserman (2018): “VoxCeleb2: deep speaker recognition.”
%%Proc. Interspeech 2018%.

Available on `https://www.isca-archive.org/interspeech_2018/chung18b_interspeech.html`.

################################################################################
"Nagrani, Chung & Zisserman (2017)"
© Anastasia Shchupak 2026-06-01

Arsha Nagrani, Joon Son Chung & Andrew Zisserman (2017): “VoxCeleb: a large-scale speaker
identification dataset.” %%Proc. Interspeech 2017%.

Available on `https://www.isca-archive.org/interspeech_2017/nagrani17_interspeech.html`.

################################################################################
)~~~"
MAN_PAGES_END

}

/* End of file manual_SpeechRecognizer.cpp */
